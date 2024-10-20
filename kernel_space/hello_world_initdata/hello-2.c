/*
 * This example illustrates  
 *
 *
 *
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int hello2_data __initdata = 3;

static int __init hello2_int(void)
{
    printk(KERN_INFO "Hello, world %d\n", hello2_data);
    return 0;
}

static void __exit hello2_exit(void)
{
    printk(KERN_INFO "Goodbye, world\n");
}

module_init(hello2_int);
module_exit(hello2_exit);

MODULE_LICENSE("GPL"); // without this it was not possible to make at all
MODULE_INFO(intree, "Y"); // this helped to solve taint kernel issue
