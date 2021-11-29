#include <linux/module.h>
#include <linux/hashtable.h>
#include <linux/types.h>
 
struct metadata{
    uint64_t pbn;
    struct hlist_node my_hash_list;
};
 
static int __init init_test(void)
{
    struct metadata first = {
        .pbn = 10,
        .my_hash_list = 0
    };
 
    struct metadata second = {
        .pbn = 20,
        .my_hash_list = 0
    };
 
    struct metadata third = {
        .pbn = 30,
        .my_hash_list = 0
    };
 
    struct metadata fourth = {
        .pbn = 41,
        .my_hash_list = 0
    };
 
    struct metadata fifth = {
        .pbn = 41,
        .my_hash_list = 0
    };
 
    DEFINE_HASHTABLE(hash_head, 9);
    hash_init(hash_head);
 
    hash_add(hash_head, &first.my_hash_list, first.pbn);
    hash_add(hash_head, &second.my_hash_list, second.pbn);
    hash_add(hash_head, &third.my_hash_list, third.pbn);
    hash_add(hash_head, &fourth.my_hash_list, third.pbn);
    hash_add(hash_head, &fifth.my_hash_list, third.pbn);
 
    printk("init_test\n");
    int bkt;
    struct metadata * cur;
    struct metadata arr[100];
    int i = 0;
    for(i = 0; i < 100; i++){
        arr[i].pbn = i;
        memset(&arr[i].my_hash_list, 0, sizeof(struct hlist_node));
        hash_add(hash_head, &arr[i].my_hash_list, arr[i].pbn);
    }
 
    hash_for_each(hash_head, bkt, cur, my_hash_list) { // 모든 bucket 순회
        printk(KERN_INFO "pbn=%d is in bucket %d\n", cur->pbn, bkt);
    }
 
    hash_for_each_possible(hash_head, cur, my_hash_list, 41){ // 41에 있는 bucket을 순회
        printk(KERN_INFO "hash_for_each_possible\n");
        printk(KERN_INFO "pbn=%d is in bucket %d\n", cur->pbn, bkt);
    }
 
}
 
static void __exit exit_test(void)
{
    printk("exit_test\n");
    return;
}
 
 
module_init(init_test); module_exit(exit_test);
 
MODULE_LICENSE("GPL");