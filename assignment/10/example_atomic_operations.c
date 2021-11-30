#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kthread.h> // for thread
#include <linux/slab.h>    // for kmalloc
#include <linux/delay.h>



int test_thread_1(void *_arg)
{
    int *value = (int *)_arg;
    printk(KERN_INFO "Thread 1 First value is %d\n", *value);
    __sync_fetch_and_add(value, 1);
    printk(KERN_INFO "Thread 1 After fetch_and_add, value is %d\n", *value);

    __sync_lock_test_and_set(value, 10);
    printk(KERN_INFO "Thread 1 After test_and_set, value is %d\n", *value);

    __sync_val_compare_and_swap(value, 10, 5);
    printk(KERN_INFO "Thread 1 After compare_and_swap, value is %d\n", *value);
}

int test_thread_2(void *_arg)
{
    int *value = (int *)_arg;
    printk(KERN_INFO "Thread 2 First value is %d\n", *value);
    __sync_fetch_and_add(value, 1);
    printk(KERN_INFO "Thread 2 After fetch_and_add, value is %d\n", *value);

    __sync_lock_test_and_set(value, 10);
    printk(KERN_INFO "Thread 2 After test_and_set, value is %d\n", *value);

    __sync_val_compare_and_swap(value, 10, 4);
    printk(KERN_INFO "Thread 2 After compare_and_swap, value is %d\n", *value);
}

int test_thread_3(void *_arg)
{
    int *value = (int *)_arg;
    printk(KERN_INFO "Thread 3 First value is %d\n", *value);
    __sync_fetch_and_add(value, 1);
    printk(KERN_INFO "Thread 3 After fetch_and_add, value is %d\n", *value);

    __sync_lock_test_and_set(value, 8);
    printk(KERN_INFO "Thread 3 After test_and_set, value is %d\n", *value);

    __sync_val_compare_and_swap(value, 10, 5);
    printk(KERN_INFO "Thread 3 After compare_and_swap, value is %d\n", *value);
}

int test_thread_4(void *_arg)
{
    int *value = (int *)_arg;
    printk(KERN_INFO "Thread 4 First value is %d\n", *value);
    __sync_fetch_and_add(value, 1);
    printk(KERN_INFO "Thread 4 After fetch_and_add, value is %d\n", *value);

    __sync_lock_test_and_set(value, 10);
    printk(KERN_INFO "Thread 4 After test_and_set, value is %d\n", *value);

    __sync_val_compare_and_swap(value, 10, 5);
    printk(KERN_INFO "Thread 4 After compare_and_swap, value is %d\n", *value);
}

static int __init simple_module_init(void)
{
    /* thread create */
    
    int *arg = (int *)kmalloc(sizeof(int), GFP_KERNEL);
    *arg = 0;
    kthread_run(&test_thread_1, (void *)arg, "test_thread 1");
    kthread_run(&test_thread_2, (void *)arg, "test_thread 2");
    kthread_run(&test_thread_3, (void *)arg, "test_thread 3");
    kthread_run(&test_thread_4, (void *)arg, "test_thread 4");

    return 0;
}

static void __exit simple_module_exit(void)
{
    printk("exit_test\n");
    return;
}
 
 
module_init(simple_module_init); module_exit(simple_module_exit);
 
MODULE_LICENSE("GPL");