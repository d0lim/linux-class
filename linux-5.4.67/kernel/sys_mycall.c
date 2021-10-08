#include <linux/kernel.h>

asmlinkage long sys_mycall(void) {
    printk("20176603 Dongyoung Lim\n");

    return 0;
}