#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros
 
MODULE_LICENSE("MIT");
MODULE_AUTHOR("D0Lim");
MODULE_DESCRIPTION("A Simple Hello World module");
 
static int __init simple_init(void)
{
    printk(KERN_INFO "simple module\n");
    return 0;    // Non-zero return means that the module couldn't be loaded.
}
 

 
module_init(simple_init);


