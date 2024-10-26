#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "hexdump"
#define LINE_SIZE 16
#define BUFFER_SIZE 65536
#define DATA_SIZE LINE_SIZE + LINE_SIZE / 2
#define OUTPUT_FILE "/tmp/loop"
#define WRITE_KERNEL_LIMIT 0x20000
#define IS_LITTLE_ENDIAN (*(unsigned char *)&(uint16_t){1})

/* Variables for device and device class */
static char* local_buffer = NULL;
static struct file* output_file = NULL;
static dev_t dev_device_nr;
static struct class *dev_class;
static struct cdev dev_device;

/* This function checks the return of  kernel_write */
static void check_kernel_write(size_t r) {
    if (r < 0) {
        printk(KERN_WARNING
               "Warning: Could not write to file: %zd\n", r);
    }
}

/* This function writes hex char into a file */
static void hex_char_write(struct file* o, char c) {
    char hexbuffer[3];
    size_t r;
    snprintf(hexbuffer, 3, "%02x", (unsigned char)c);
    r = kernel_write(o, hexbuffer, 2, 0);
    check_kernel_write(r);
}

/* This function writes pair of hex chars into a file */
static void hex_chars_write(struct file* o, char f, char s) {
    size_t r = kernel_write(o, " ", 1, 0);
    check_kernel_write(r);
    if (IS_LITTLE_ENDIAN) {
        hex_char_write(o, s);
        hex_char_write(o, f);
    } else {
        hex_char_write(o, f);
        hex_char_write(o, s);
    }
}

/* This function writes hex address into a file */
static void hex_addr_write(struct file* o, unsigned int a, bool f) {
    size_t r;
    char hex_buffer[8];
    snprintf(hex_buffer, 8, "%07x", a);
    if (!f) {
        r = kernel_write(o, "\n", 1, 0);
        check_kernel_write(r);
    }
    r = kernel_write(o, hex_buffer, 7, 0);
    check_kernel_write(r);
}

/* This function checks for repeating lines and returns the line in hex */
void hex_data_write(struct file* o, size_t l, size_t s) {
    /* Fill buffer with empty spaces */
    char fb, sb;
    size_t i = 0;
    for (; i < s; i += 2) {
        fb = local_buffer[l + i];
        sb = local_buffer[l + i + 1];
        hex_chars_write(o, fb, sb);
    }
}

/* This function reads data from the buffer */
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
    int to_copy;
    static unsigned int v_addr = 0;
    unsigned int total_written = 0;
    static bool f = true; // first iter

    while (count > 0) {
        to_copy = min(count, BUFFER_SIZE);
        if (copy_from_user(local_buffer, user_buffer, to_copy)) {
            printk("Failed to copy from user buffer\n");
            return -1;
        }
        size_t i = 0;
        for (; i < to_copy; i += LINE_SIZE) {
            size_t s = to_copy - i >= LINE_SIZE ? LINE_SIZE : to_copy - i;
            hex_addr_write(output_file, v_addr, f);
            f = false;
            hex_data_write(output_file, i, s);
            v_addr += s;
        }
        total_written += to_copy;
        count -= to_copy;
        user_buffer += to_copy;
    }
    hex_addr_write(output_file, v_addr, f);
    v_addr = 0;
    f = true;
    return total_written;
}

/* This function is called when the device file is opened */
static int driver_open(struct inode *device_file, struct file *instance) {
    /* Open / Create the output file for writing */
    output_file = filp_open(OUTPUT_FILE, O_CREAT | O_WRONLY | O_APPEND | O_LARGEFILE, 0666);
    if (IS_ERR(output_file)) {
        printk("Failed to open output file \n");
        return -1;
    }
    /* Buffer for data */
    local_buffer = (char*)kmalloc(BUFFER_SIZE, GFP_KERNEL);
    if (!local_buffer) {
        printk("Failed to allocate local buffer\n");
        return -1;
    }
    printk("The device is opened\n");
    return 0;
}

/* This function is called when the device file is closed */
static int driver_close(struct inode *device_file, struct file *instance) {
    filp_close(output_file, NULL);
    kfree(local_buffer);
    printk("The device is closed\n");
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .write = driver_write,
    .release = driver_close
};

static int __init ModuleInit(void) {
    /* Allocating a device nr */
    if (alloc_chrdev_region(&dev_device_nr, 0, 1, DEVICE_NAME) < 0) {
        printk("Device Nr. could not be allocated\n");
        return -1;
    }
    printk("hexdump - Device Nr. Major: %d, Minor: %d was registered!\n", dev_device_nr >> 20, dev_device_nr && 0xfffff);
    /* Create device class */
    if ((dev_class = class_create(THIS_MODULE, DEVICE_NAME)) == NULL) {
        printk("Device class can not be created!\n");
        goto ClassError;
    }
    /* Create device file */
    if (device_create(dev_class, NULL, dev_device_nr, NULL, DEVICE_NAME) == NULL) {
        printk("Device class can not be created!\n");
        goto FileError;
    }
    /* Initialize device file */
    cdev_init(&dev_device, &fops);
    /* Registering device to kernel */
    if (cdev_add(&dev_device, dev_device_nr, 1) == -1) {
        printk("Registering of device to kernel failed class can not be created!\n");
        goto AddError;
    }
    return 0;

AddError:
    device_destroy(dev_class, dev_device_nr);
FileError:
    class_destroy(dev_class);
ClassError:
    unregister_chrdev(dev_device_nr, DEVICE_NAME);

    return -1;
}

static void __exit ModuleExit(void) {
    device_destroy(dev_class, dev_device_nr);
    class_destroy(dev_class);
    cdev_del(&dev_device);
    unregister_chrdev(dev_device_nr, DEVICE_NAME);
    printk("%s exit", DEVICE_NAME);
}

module_init(ModuleInit);
module_exit(ModuleExit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Arkadi Hakobyan");
MODULE_DESCRIPTION("LKM for hexdumping files");
