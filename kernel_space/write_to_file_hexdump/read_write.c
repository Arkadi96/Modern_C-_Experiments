#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Staples");
MODULE_DESCRIPTION("LKM with an device number");

/* Globals needed for hexdump */
#define LINE_SIZE 16
#define LINE_END 10 // ASCII Line feed
#define BUFFER_SIZE 1024 * 1024
#define OUTPUT_FILE "/tmp/loop"

/* Globals needed for driver */
#define DRIVER_NAME "readwrite"

/* Variables for device and device class */
static dev_t my_device_nr;
static struct class *my_class;
static struct cdev my_device;

/* Buffer for data */
static char* buffer;

/* This function hexdumps into a file */
static void hexdump_write(struct file* o, char* c) {
    char hexbuffer[3];
    snprintf(hexbuffer, 3, "%02x", *c);
    kernel_write(o, hexbuffer, 2, 0);
}

/* This function prints address into a file */
static void address_write(struct file* o, unsigned int* a) {
    static bool f = true;
    char hex_buffer[8];
    unsigned int pa = *a;
    if (!f) {
        kernel_write(o, "\n", 1, 0);
    }
    f = false;
    *a += LINE_SIZE;
    snprintf(hex_buffer, 8, "%07x", pa);
    kernel_write(o, hex_buffer, 7, 0);
}

/* This function reads data from the buffer */
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
    int to_copy, not_copied, i;
    unsigned int v_addr = 0;
    struct file *output_file = NULL;
    char fb, sb;

    /* Get amount of data to copy */
    to_copy = min(count, (size_t)BUFFER_SIZE);

    /* Copy data from user */
    not_copied = copy_from_user(buffer, user_buffer, to_copy);

    /* Convert data to hex and write to output file */
    output_file = filp_open(OUTPUT_FILE, O_CREAT | O_WRONLY | O_APPEND | O_LARGEFILE, 0666);
    if (IS_ERR(output_file)) {
        printk("Error in opening: %ld\n", (long)output_file);
        return -1;
    }
    if (output_file == NULL) {
        printk("Error in opening: null\n");
        return -1;
    }
    /* Iterating through buffer and writing into a file */
    int el_cnt = 0;
    for (i = 0; i < to_copy; i += 2) {
        if (i % LINE_SIZE == 0) {
            address_write(output_file, &v_addr);
            el_cnt = 0;
        }
        kernel_write(output_file, " ", 1, 0);
        if (i == to_copy - 1) {
            fb = buffer[to_copy - 1];
            sb = 0;
            el_cnt += 1;
        } else {
            fb = buffer[i];
            sb = buffer[i + 1];
            el_cnt += 2;
        }
        hexdump_write(output_file, &sb);
        hexdump_write(output_file, &fb);

        // if this is the last iteration
        if (i + 2 >= to_copy) {
            v_addr -= LINE_SIZE;
            v_addr += el_cnt;
            address_write(output_file, &v_addr);
            kernel_write(output_file, "\n", 1, 0);
        }
    }
    filp_close(output_file, NULL);
    return to_copy;
}

/* This function is called when the device file is opened */
static int driver_open(struct inode *device_file, struct file *instance) {
    printk("dev nr - open was called!\n");
    return 0;
}

/* This function is called when the device file is closed */
static int driver_close(struct inode *device_file, struct file *instance) {
    printk("dev nr - close was called!\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .write = driver_write
};

static int __init ModuleInit(void) {
    printk("Hello, Kernel!\n");
    /* Allocating a local buffer with*/
    buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
    /* Allocating a device nr */
    if (alloc_chrdev_region(&my_device_nr, 0, 1, DRIVER_NAME) < 0) {
        printk("Device Nr. could not be allocated\n");
        return -1;
    }
    printk("read_write - Device Nr. Major: %d, Minor: %d was registered!\n", my_device_nr >> 20, my_device_nr && 0xfffff);

    /* Create device class */
    if ((my_class = class_create(THIS_MODULE, DRIVER_NAME)) == NULL) {
        printk("Device class can not be created!\n");
        goto ClassError;
    }

    /* Create device file */
    if (device_create(my_class, NULL, my_device_nr, NULL, DRIVER_NAME) == NULL) {
        printk("Device class can not be created!\n");
        goto FileError;
    }

    /* Initialize device file */
    cdev_init(&my_device, &fops);

    /* Registering device to kernel */
    if (cdev_add(&my_device, my_device_nr, 1) == -1) {
        printk("Registering of device to kernel failed class can not be created!\n");
        goto AddError;
    }

    return 0;

AddError:
    device_destroy(my_class, my_device_nr);
FileError:
    class_destroy(my_class);
ClassError:
    unregister_chrdev(my_device_nr, DRIVER_NAME);

    return -1;
}

static void __exit ModuleExit(void) {
    kfree(buffer);
    cdev_del(&my_device);
    device_destroy(my_class, my_device_nr);
    class_destroy(my_class);
    unregister_chrdev(my_device_nr, DRIVER_NAME);
    printk("%s exit", DRIVER_NAME);
}

module_init(ModuleInit);
module_exit(ModuleExit);
