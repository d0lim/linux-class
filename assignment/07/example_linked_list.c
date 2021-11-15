#include <linux/ktime.h>
#include <linux/module.h> // included for all kernel modules
#include <linux/kernel.h> // included for KERN_INFO
#include <linux/init.h>   // included for __init and __exit macros
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/random.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("D0Lim");
MODULE_DESCRIPTION("A Simple Linked List module");

struct my_node
{
    struct list_head entry;
    int data;
};

void example(int count)
{
    ktime_t start_time, stop_time, elapsed_time;
    struct list_head my_list;
    INIT_LIST_HEAD(&my_list);

    int i;

    /**
     * Insert
     */

    elapsed_time = 0;
    for (i = 0; i < count; i++)
    {
        start_time = ktime_get();
        struct my_node *new = kmalloc(sizeof(struct my_node), GFP_KERNEL);
        new->data = i;
        list_add(&new->entry, &my_list);
        stop_time = ktime_get();
        elapsed_time += ktime_sub(stop_time, start_time);
    }

    printk(KERN_INFO "%d INSERT TIME : %lldns\n", count, ktime_to_ns(elapsed_time));

    /**
     * Make Search Array
     */

    int *num_arr = (int *)kmalloc(sizeof(int) * count, GFP_KERNEL);
    for (i = 0; i < count; i++)
    {
        num_arr[i] = i;
    }

    int j;
    for (i = 0; i < count - 1; i++)
    {
        int rn = 0;
        get_random_bytes(&rn, sizeof(int) - 1);
        j = rn % (count - i) + i;
        int t = num_arr[j];
        num_arr[j] = num_arr[i];
        num_arr[i] = t;
    }

    /**
     * Search
     */

    start_time = 0;
    stop_time = 0;
    elapsed_time = 0;

    for (i = 0; i < count; i++)
    {
        struct my_node *current_node = NULL;
        int target = num_arr[i];
        start_time = ktime_get();
        list_for_each_entry(current_node, &my_list, entry)
        {
            // printk(KERN_INFO "current value : %d\n", current_node->data);
            if (current_node->data == target)
            {
                break;
            }
        }
        stop_time = ktime_get();
        elapsed_time += ktime_sub(stop_time, start_time);
    }
    printk(KERN_INFO "%d SEARCH TIME : %lldns\n", count, ktime_to_ns(elapsed_time));

    /**
     * Delete
     */

    start_time = 0;
    stop_time = 0;
    elapsed_time = 0;

    for (i = 0; i < count; i++)
    {
        struct my_node *current_node = NULL;
        struct my_node *tmp;
        int target = i;
        start_time = ktime_get();
        list_for_each_entry_safe(current_node, tmp, &my_list, entry)
        {
            if (current_node->data == target)
            {
                list_del(&current_node->entry);
                kfree(current_node);
                break;
            }
        }
        stop_time = ktime_get();
        elapsed_time += ktime_sub(stop_time, start_time);
    }
    printk(KERN_INFO "%d DELETE TIME : %lldns\n", count, ktime_to_ns(elapsed_time));
}

void struct_example(void)
{
    ktime_t start_time, stop_time, elapsed_time;
    struct list_head my_list;
    INIT_LIST_HEAD(&my_list);

    int i;
    start_time = ktime_get();
    for (i = 0; i < 10; i++)
    {
        struct my_node *new = kmalloc(sizeof(struct my_node), GFP_KERNEL);
        new->data = i;
        list_add(&new->entry, &my_list);
    }
    stop_time = ktime_get();
    elapsed_time = ktime_sub(stop_time, start_time);
    printk(KERN_INFO "TIME : %lldns\n", ktime_to_ns(elapsed_time));

    struct my_node *current_node = NULL;
    list_for_each_entry(current_node, &my_list, entry)
    {
        printk(KERN_INFO "current value : %d\n", current_node->data);
    }
    list_for_each_entry_reverse(current_node, &my_list, entry) { printk(KERN_INFO "current value : %d\n", current_node->data); } /* list element delete */
    struct my_node *tmp;
    list_for_each_entry_safe(current_node, tmp, &my_list, entry)
    {
        if (current_node->data == 2)
        {
            printk(KERN_INFO "current node value : %d\n", current_node->data);
            list_del(&current_node->entry);
            kfree(current_node);
        }
    }
    list_for_each_entry(current_node, &my_list, entry) { printk(KERN_INFO "current value : %d\n", current_node->data); }
}

static int __init linked_list_init(void)
{
    printk(KERN_INFO "Linked List test start!\n");
    example(1000);
    example(10000);
    example(100000);

    return 0; // Non-zero return means that the module couldn't be loaded.
}

static void __exit linked_list_cleanup(void)
{
    printk("\n");
}

module_init(linked_list_init);
module_exit(linked_list_cleanup);
