#include "device_file.h"
#include <linux/init.h>       /* module_init, module_exit */
#include <linux/module.h>     /* version info, MODULE_LICENSE, MODULE_AUTHOR, printk() */

MODULE_DESCRIPTION("Custom Linux driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("arkadi_hak");

static int custom_driver_init(void)
{
    int result = 0;
    printk( KERN_NOTICE "Custom-driver: Initialization started\n" );

    result = register_device();
    return result;
}

static void custom_driver_exit(void)
{
    printk( KERN_NOTICE "Custom-driver: Exiting\n" );
    unregister_device();
}

module_init(custom_driver_init);
module_exit(custom_driver_exit);
