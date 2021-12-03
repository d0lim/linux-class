#include <linux/ktime.h>
#include <linux/module.h> // included for all kernel modules
#include <linux/kernel.h> // included for KERN_INFO
#include <linux/init.h>   // included for __init and __exit macros
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/random.h>

#define MAX_DATA_CATEGORY 100000

struct node
{
    int data;
    struct node *next; // Pointer to next node
    struct node *prev; // Pointer to previous node
};

typedef struct node *Node;

struct chain
{
    Node dst;
    struct chain *next;
    struct chain *prev;
};

typedef struct chain *Chain;

Node create_node(int data);
void delete_node(Node temp);
void insert_head(Node head, int data);
void insert_tail(Node head, int data);
void delete_tail(Node head);
void delete_head(Node head);
void display(Node head);

void init_chain_array(void);
Chain create_chain(Node new_node);
void delete_chain(Chain temp);
void insert_chain_head(int data, Node new_node);
void insert_chain_tail(int data, Node new_node);
void delete_chain_head(int data);
void delete_chain_tail(int data);
void display_chain(int count);

Node search_by_data(int data);

Chain chain_arr[MAX_DATA_CATEGORY];

Node create_node(int data)
{
    Node temp = (Node) kmalloc(sizeof(struct node), GFP_KERNEL);
    temp->next = temp;
    temp->prev = temp;
    temp->data = data;
    return temp;
}

void delete_node(Node temp)
{
    // printk("%d deleted\n", temp->data);
    kfree(temp);
    return;
}

void insert_head(Node head, int data)
{
    // Insert to Doubly Linked List
    Node new_node = create_node(data);
    Node first = head->next;
    head->next = new_node;
    new_node->next = first;
    new_node->prev = head;
    first->prev = new_node;
    
    // Chain Node
    insert_chain_head(data, new_node);
}

void insert_tail(Node head, int data)
{
    // Insert to Doubly Linked List
    Node new_node = create_node(data);
    Node last = head->prev;
    head->prev = new_node;
    new_node->next = head;
    new_node->prev = last;
    last->next = new_node;
    
    // Chain Node
    insert_chain_tail(data, new_node);
}

void delete_tail(Node head)
{
    if(head->next == head)
    {
        // printk("List is empty\n");
        return;
    }
    Node last = head->prev;
    Node slast = last->prev;
    delete_chain_tail(last->data);
    slast->next = head;
    head->prev = slast;
    delete_node(last);
}

void delete_head(Node head)
{
    if(head->next == head)
    {
        // printk("List is empty\n");
        return;
    }
    Node first = head->next;
    Node sfirst = first->next;
    delete_chain_head(first->data);
    sfirst->prev = head;
    head->next = sfirst;
    delete_node(first);
}

void display(Node head)
{
    Node cur = head->next;
    if(cur == head)
    {
        // printk("List is empty\n");
        return;
    }
    // printk("Contents of the Circular Doubly Linked list are\n");
    while(cur!=head)
    {
        printk("%d ", cur->data);
        cur = cur->next;
    }
    printk("\n");
}

void init_chain_array(void)
{
    int i;
    for (i = 0; i < MAX_DATA_CATEGORY; i++)
    {
        Chain temp = create_chain(NULL);
        chain_arr[i] = temp;
    }
}

Chain create_chain(Node new_node)
{
    Chain temp = (Chain) kmalloc(sizeof(struct chain), GFP_KERNEL);
    temp->dst = new_node;
    temp->prev = temp;
    temp->next = temp;
    return temp;
}

void delete_chain(Chain temp)
{
    // printk("chain of data %d deleted\n", temp->dst->data);
    kfree(temp);
    return;
}

void insert_chain_head(int data, Node new_node)
{
    Chain new_chain = create_chain(new_node);
    Chain first = chain_arr[data]->next;
    chain_arr[data]->next = new_chain;
    new_chain->next = first;
    new_chain->prev = chain_arr[data];
    first->prev = new_chain;
}

void insert_chain_tail(int data, Node new_node)
{
    Chain new_chain = create_chain(new_node);
    Chain last = chain_arr[data]->prev;
    chain_arr[data]->prev = new_chain;
    new_chain->prev = last;
    new_chain->next = chain_arr[data];
    last->next = new_chain;
}

void delete_chain_head(int data)
{
    if (chain_arr[data]->next == chain_arr[data])
    {
        // printk("List is empty\n");
        return;
    }
    Chain first = chain_arr[data]->next;
    Chain sfirst = first->next;
    chain_arr[data]->next = sfirst;
    sfirst->prev = chain_arr[data];
    delete_chain(first);
}

void delete_chain_tail(int data)
{
    if (chain_arr[data]->next == chain_arr[data])
    {
        // printk("List is empty\n");
        return;
    }
    Chain last = chain_arr[data]->prev;
    Chain slast = last->prev;
    chain_arr[data]->prev = slast;
    slast->next = chain_arr[data];
    delete_chain(last);
}

void display_chain(int count)
{
    int i;
    for (i = 0; i < count; i++)
    {
        // printk("data %d chain list:\n", i);
        Chain cur = chain_arr[i]->next;
        if(cur == chain_arr[i])
        {
            // printk("Chain list is empty\n");
            continue ;
        }
        while(cur != chain_arr[i])
        {
            // printk("%d ", cur->dst->data);
            cur = cur->next;
        }
        // printk("\n");
    }
}

Node search_by_data(int data)
{
    if (chain_arr[data]->next != chain_arr[data])
    {
        return chain_arr[data]->next->dst;
    }
    return NULL;
}

void delete_by_data(int data)
{
    Node node = search_by_data(data);
    Node _prev = node->prev;
    Node _next = node->next;
    delete_chain_head(data);
    _prev->next = _next;
    _next->prev = _prev;
    delete_node(node);
}

void example(int count)
{
    ktime_t start_time, stop_time, elapsed_time;
    Node head = create_node(0);
    init_chain_array();
    int i;

    /**
     * Insert
     */

    elapsed_time = 0;
    for (i = 0; i < count; i++)
    {

        start_time = ktime_get();
        insert_tail(head, i);
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
        struct my_node *cur_node = NULL;
        int target = num_arr[i];
        start_time = ktime_get();
        search_by_data(target);
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
        struct my_node *cur_node = NULL;
        struct my_node *tmp;
        int target = num_arr[i];
        start_time = ktime_get();
        delete_by_data(target);
        stop_time = ktime_get();
        elapsed_time += ktime_sub(stop_time, start_time);
    }
    printk(KERN_INFO "%d DELETE TIME : %lldns\n", count, ktime_to_ns(elapsed_time));
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
MODULE_LICENSE("GPL");
