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
void delete_head(Node head);
void delete_tail(Node head);
void delete_head_lazy(Node head, Node garbage_head);
void delete_tail_lazy(Node head, Node garbage_head);
void display(Node head);

void init_chain_array(void);
void init_garbage_array(void);
Chain create_chain(Node new_node);
void delete_chain(Chain temp);
void insert_chain_head(int data, Node new_node);
void insert_chain_tail(int data, Node new_node);
void delete_chain_head(int data);
void delete_chain_tail(int data);
void delete_chain_head_lazy(int data);
void delete_chain_tail_lazy(int data);
void display_chain(int count);

Node search_by_data(int data);

void insert_garbage_head(Node garbage_head, Node new_node);
void insert_garbage_tail(Node garbage_head, Node new_node);
void delete_garbage_head(Node garbage_head);
void delete_garbage_tail(Node garbage_head);

void insert_garbage_chain_head(int data, Node new_node);
void insert_garbage_chain_tail(int data, Node new_node);
void delete_garbage_chain_head(int data);
void delete_garbage_chain_tail(int data);

Chain chain_arr[MAX_DATA_CATEGORY];
Chain garbage_chain_arr[MAX_DATA_CATEGORY];


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


void delete_head(Node head)
{
    if(head->next == head)
    {
        return;
    }
    Node first = head->next;
    Node sfirst = first->next;
    delete_chain_head(first->data);
    sfirst->prev = head;
    head->next = sfirst;
    delete_node(first);
}

void delete_tail(Node head)
{
    if(head->next == head)
    {
        return;
    }
    Node last = head->prev;
    Node slast = last->prev;
    delete_chain_tail(last->data);
    slast->next = head;
    head->prev = slast;
    delete_node(last);
}


void delete_head_lazy(Node head, Node garbage_head)
{
    if(head->next == head)
    {
        return;
    }
    Node first = head->next;
    Node sfirst = first->next;
    delete_chain_head(first->data);
    sfirst->prev = head;
    head->next = sfirst;
    
    insert_garbage_tail(garbage_head, first);
}

void delete_tail_lazy(Node head, Node garbage_head)
{
    if(head->next == head)
    {
        return;
    }
    Node last = head->prev;
    Node slast = last->prev;
    delete_chain_tail(last->data);
    slast->next = head;
    head->prev = slast;
    
    insert_garbage_tail(garbage_head, last);
}

void display(Node head)
{
    Node curr = head->next;
    if(curr == head)
    {
        return;
    }
    printk("Contents of the Circular Doubly Linked list are\n");
    while(curr!=head)
    {
        printk("%d ", curr->data);
        curr = curr->next;
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

void init_garbage_array(void)
{
    int i;
    for (i = 0; i < MAX_DATA_CATEGORY; i++)
    {
        Chain temp = create_chain(NULL);
        garbage_chain_arr[i] = temp;
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
        printk("data %d chain list:\n", i);
        Chain curr = chain_arr[i]->next;
        if(curr == chain_arr[i])
        {
            printk("Chain list is empty\n");
            continue ;
        }
        while(curr != chain_arr[i])
        {
            printk("%d ", curr->dst->data);
            curr = curr->next;
        }
        printk("\n");
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

void insert_garbage_head(Node garbage_head, Node new_node)
{
    Node first = garbage_head->next;
    garbage_head->next = new_node;
    new_node->next = first;
    new_node->prev = garbage_head;
    first->prev = new_node;
    
    insert_garbage_chain_head(new_node->data, new_node);
}

void insert_garbage_tail(Node garbage_head, Node new_node)
{
    Node last = garbage_head->prev;
    garbage_head->prev = new_node;
    new_node->prev = last;
    new_node->next = garbage_head;
    last->next = new_node;
    
    insert_garbage_chain_tail(new_node->data, new_node);
}

void delete_garbage_head(Node garbage_head)
{
    if(garbage_head->next == garbage_head)
    {
        return;
    }
    Node first = garbage_head->next;
    Node sfirst = first->next;
    delete_garbage_chain_head(first->data);
    sfirst->prev = garbage_head;
    garbage_head->next = sfirst;
    delete_node(first);
}

void delete_garbage_tail(Node garbage_head)
{
    if(garbage_head->next == garbage_head)
    {
        return;
    }
    Node last = garbage_head->prev;
    Node slast = last->prev;
    delete_garbage_chain_tail(last->data);
    slast->next = garbage_head;
    garbage_head->prev = slast;
    delete_node(last);
}

void insert_garbage_chain_head(int data, Node new_node)
{
    Chain new_chain = create_chain(new_node);
    Chain first = garbage_chain_arr[data]->next;
    garbage_chain_arr[data]->next = new_chain;
    new_chain->next = first;
    new_chain->prev = garbage_chain_arr[data];
    first->prev = new_chain;
}

void insert_garbage_chain_tail(int data, Node new_node)
{
    Chain new_chain = create_chain(new_node);
    Chain last = garbage_chain_arr[data]->prev;
    garbage_chain_arr[data]->prev = new_chain;
    new_chain->prev = last;
    new_chain->next = garbage_chain_arr[data];
    last->next = new_chain;
}

void delete_garbage_chain_head(int data)
{
    if (garbage_chain_arr[data]->next == garbage_chain_arr[data])
    {
        return;
    }
    Chain first = garbage_chain_arr[data]->next;
    Chain sfirst = first->next;
    garbage_chain_arr[data]->next = sfirst;
    sfirst->prev = garbage_chain_arr[data];
    delete_chain(first);
}

void delete_garbage_chain_tail(int data)
{
    if (garbage_chain_arr[data]->next == garbage_chain_arr[data])
    {
        return;
    }
    Chain last = garbage_chain_arr[data]->prev;
    Chain slast = last->prev;
    garbage_chain_arr[data]->prev = slast;
    slast->next = garbage_chain_arr[data];
    delete_chain(last);
}

void example(int count)
{
    ktime_t start_time, stop_time, elapsed_time;
    Node head = create_node(0);
    Node garbage_head = create_node(0);
    init_chain_array();
    init_garbage_array();
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
        int target = i;
        start_time = ktime_get();
        delete_tail_lazy(head, garbage_head);
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
