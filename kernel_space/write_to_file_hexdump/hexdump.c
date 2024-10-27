#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define DEVICE_NAME "hexdump"
#define LINE_SIZE 16
#define BUFFER_SIZE 65536
#define OUTPUT_FILE "/tmp/loop"
#define IS_LITTLE_ENDIAN (*(unsigned char *)&(uint16_t){1})

/* Variables for device and device class */
static char* local_buffer = NULL;
static struct file* output_file = NULL;
static struct class *dev_class;
static struct cdev dev_device;
static dev_t dev_device_nr;
static unsigned int v_addr = 0;
static size_t rep_valid_idx = 0;
static bool rep_valid = false;
static bool is_first_iter = true;

/* This function checks the return of  kernel_write */
static void check_kernel_write(size_t r) {
    if (r < 0) {
        printk(KERN_WARNING
               "Warning: Could not write to file: %zd\n", r);
    }
}

/* This function writes hex char into a file */
static void hex_char_write(char c) {
    char hexbuffer[3];
    size_t r;
    snprintf(hexbuffer, 3, "%02x", (unsigned char)c);
    r = kernel_write(output_file, hexbuffer, 2, 0);
    check_kernel_write(r);
}

/* This function writes pair of hex chars into a file */
static void hex_chars_write(char f, char s) {
    size_t r = kernel_write(output_file, " ", 1, 0);
    check_kernel_write(r);
    if (IS_LITTLE_ENDIAN) {
        hex_char_write(s);
        hex_char_write(f);
    } else {
        hex_char_write(f);
        hex_char_write(s);
    }
}

/* This function writes hex address into a file */
static void hex_addr_write(void) {
    size_t r;
    char hex_buffer[8];
    snprintf(hex_buffer, 8, "%07x", v_addr);
    if (!is_first_iter) {
        r = kernel_write(output_file, "\n", 1, 0);
        check_kernel_write(r);
    }
    r = kernel_write(output_file, hex_buffer, 7, 0);
    check_kernel_write(r);
}

/* This function writes repetition symbol into a file */
static void hex_repeated_write(void) {
    size_t r;
    if (!is_first_iter) {
        r = kernel_write(output_file, "\n", 1, 0);
        check_kernel_write(r);
        r = kernel_write(output_file, "*", 1, 0);
        check_kernel_write(r);
    }
}

/* This function writes repetition symbol into a file */
static size_t compare_substrings(size_t i, size_t j, size_t l) {
    size_t c = 0;
    for (; c < l; ++c) {
        if (local_buffer[i + c] != local_buffer[j + c])
            return 0;
    }
    return 1;
}

/* This function checks for repetition */
static size_t check_repetition(size_t i, size_t s) {
    /* No repetition when first line or uncompleted line left*/
    if (i == 0) {
        return 0;
    }
    if (!rep_valid) {
        if (compare_substrings(i - s, i, s)) {
            rep_valid_idx = i - s;
            rep_valid = true;
            hex_repeated_write();
            return 1;
        } else {
            rep_valid = false;
            return 0;
        }
    } else {
        if (compare_substrings(rep_valid_idx, i, s)) {
            return 1;
        } else {
            rep_valid = false;
            return 0;
        }
    }
}

/* This function checks for repeating lines and returns the line in hex */
void hex_data_write(size_t l, size_t s) {
    /* Fill buffer with empty spaces */
    char fb, sb;
    size_t i = 0;
    for (; i < s; i += 2) {
        fb = local_buffer[l + i];
        sb = local_buffer[l + i + 1];
        hex_chars_write(fb, sb);
    }
}

/* This function writes fills uncompleted spaces of the last line */
static void empty_data_write(void) {
    size_t r_d;
    size_t r, c;
    unsigned int p_addr;
    /* Count uncompleted data count in line */
    if (v_addr % LINE_SIZE != 0) {
        p_addr = (v_addr / LINE_SIZE) * LINE_SIZE;
        r_d = LINE_SIZE - (v_addr - p_addr);
        r_d = (r_d % 2 == 1) ? r_d - 1 : r_d;
        for (c = 0; c < r_d; c += 2) {
            r = kernel_write(output_file, "     ", 5, 0);
            check_kernel_write(r);
        }
    }
}

/* This function write data from the buffer to the output file */
static ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
    size_t to_copy, s, i;
    unsigned int total_written = 0;
    /* Core logic, iterating line by line over user buffer, processing data */
    while (count > 0) {
        to_copy = min(count, (size_t)BUFFER_SIZE);
        if (copy_from_user(local_buffer, user_buffer, to_copy)) {
            printk("Failed to copy from user buffer\n");
            return -1;
        }
        i = 0;
        for (; i < to_copy; i += LINE_SIZE) {
            s = to_copy - i >= LINE_SIZE ? LINE_SIZE : to_copy - i;
            if (!check_repetition(i, s)) {
              hex_addr_write();
              is_first_iter = false;
              hex_data_write(i, s);
            }
            v_addr += s;
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
    size_t r;
    if (!rep_valid) { empty_data_write();}
    hex_addr_write();
    r = kernel_write(output_file, "\n", 1, 0);
    check_kernel_write(r);
    v_addr = 0;
    rep_valid_idx = 0;
    rep_valid = false;
    is_first_iter = true;
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
        goto class_error;
    }
    /* Create device file */
    if (device_create(dev_class, NULL, dev_device_nr, NULL, DEVICE_NAME) == NULL) {
        printk("Device class can not be created!\n");
        goto file_error;
    }
    /* Initialize device file */
    cdev_init(&dev_device, &fops);
    /* Registering device to kernel */
    if (cdev_add(&dev_device, dev_device_nr, 1) == -1) {
        printk("Registering of device to kernel failed class can not be created!\n");
        goto add_error;
    }
    return 0;

add_error:
    device_destroy(dev_class, dev_device_nr);
file_error:
    class_destroy(dev_class);
class_error:
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
