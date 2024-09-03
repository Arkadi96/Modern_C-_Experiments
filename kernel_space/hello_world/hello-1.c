/*
 *  hello-1.c - The simplest kernel module.
 *
 *  NOTE: i was unable to load this kernel code as a module untill i turned of
 *        the secure boot from the bios itself. Before that it the command
 *        "sudo insmod ..." failed by saying "Operation is not permitted"
 */
#include <linux/init.h>	/* Needed by all modules */
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */


/*
 * kernel modules must have at least two functions:
 * a "start" function called init_module()
 */
static int __init hello_init(void)
{
	printk(KERN_INFO "Hello world 1.\n");

	/*
	 * A non 0 return means init_module failed; module can't be loaded.
	 */
	return 0;
}


/*
 * and an "end" function, called cleanup_module() which is called just before rmmode.
 */
static void __exit hello_exit(void)
{
	printk(KERN_INFO "Goodbye world 1.\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL"); // without this it was not possible to make at all
MODULE_INFO(intree, "Y"); // this helped to solve taint kernel issue
