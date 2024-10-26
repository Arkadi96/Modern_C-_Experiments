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
#define OUTPUT_FILE "/tmp/loop"
#define WRITE_KERNEL_LIMIT 0x20000
#define IS_LITTLE_ENDIAN (*(unsigned char *)&(uint16_t){1})

/* Variables for device and device class */
static char* local_buffer = NULL;
static struct file* output_file = NULL;
static dev_t dev_device_nr;
static struct class *dev_class;
static struct cdev dev_device;

/* This function hexdumps into a file */
static void hexdump_write(struct file* o, char* c) {
    char hexbuffer[3];
    snprintf(hexbuffer, 3, "%02x", (unsigned char)*c);
    kernel_write(o, hexbuffer, 2, 0);
}

/* This function prints address into a file */
static void address_write(struct file* o, unsigned int a) {
    char hex_buffer[8];
    snprintf(hex_buffer, 8, "%07x", a);
    kernel_write(o, hex_buffer, 7, 0);
}

/* This function reads data from the buffer */
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
    //printk("count numb : %x\n", count);
    int to_copy, not_copied, i;
    static unsigned int v_addr = 0;
    short el_cnt = 0;
    unsigned int total_written = 0;
    char fb, sb, tmp;
    bool f = true;
    bool l = false;
    bool t = false;

    while (count > 0) {
        //printk("count %d, to_copy %d", count, to_copy);
        to_copy = min(count, BUFFER_SIZE);
        if (copy_from_user(local_buffer, user_buffer, to_copy)) {
            printk("Failed to copy from user buffer\n");
            return -1;
        }
        if (t) {
            fb = tmp;
            sb = local_buffer[0];
            el_cnt = 2;
            v_addr += el_cnt;
            hexdump_write(output_file, &sb);
            hexdump_write(output_file, &fb);
        }
        int i;
        i = (t) ? 1 : 0;
        t = false;
        for (i; i < to_copy; i+=2) {
            if (v_addr % LINE_SIZE == 0) {
                if (!f) {
                    kernel_write(output_file, "\n", 1, 0);
                }
                f = false;
                address_write(output_file, v_addr);
            }
            kernel_write(output_file, " ", 1, 0);
            if (i != to_copy - 1) {
                fb = local_buffer[i];
                sb = local_buffer[i + 1];
                el_cnt = 2;
            }
            if ((i == to_copy - 1) && (count - to_copy > 0)) {
                t = true;
                tmp = local_buffer[i];
            }
            if ((i == to_copy - 1) && (count - to_copy <= 0)) {
                t = false;
                fb = local_buffer[to_copy - 1];
                sb = 0;
                el_cnt = 1;
            }
            if (count - to_copy <= 0) {
                l = true;
            }
            if (!t) {
                v_addr += el_cnt;
                hexdump_write(output_file, &sb);
                hexdump_write(output_file, &fb);
            }
        }
        if (l) {
            if (v_addr % WRITE_KERNEL_LIMIT != 0) {
                /* Fill the line with spaces*/
                unsigned int p_addr = (v_addr / 16) * 16;
                printk("last addr : %x\n", p_addr);
                size_t r_d = LINE_SIZE - (v_addr - p_addr);
                r_d = (r_d % 2 == 1) ? r_d - 1 : r_d;
                printk("rem bytes : %d\n", r_d);
                int c;
                for (c = 0; c < r_d; c += 2) {
                    kernel_write(output_file, "     ", 5, 0);
                }
                kernel_write(output_file, "\n", 1, 0);
                address_write(output_file, v_addr);
                v_addr = 0;
            }
            kernel_write(output_file, "\n", 1, 0);
        }
        total_written += to_copy;
        count -= to_copy;
        user_buffer += to_copy;
    }
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
    local_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
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

