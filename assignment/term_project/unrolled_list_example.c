#include <linux/ktime.h>
#include <linux/module.h> // included for all kernel modules
#include <linux/kernel.h> // included for KERN_INFO
#include <linux/init.h>   // included for __init and __exit macros
#include <linux/slab.h>
#include <linux/random.h>

#define NULL ((void *)0)
// #define true 1
// #define false 0
#define maxElements 4

typedef __kernel_size_t size_t;
typedef int elem;

bool unrolled_equality(elem elem1, elem elem2)
{
    if (elem1 == elem2)
        return true;
    else
        return false;
}

void printFunction(elem e)
{
    printk("%d\n", e);
}

// Structure for each node of the list
struct linked_node
{
    elem *data;               // Array to store elements.
    struct linked_node *next; // Pointer to next node
};

typedef struct linked_node node;

// Client provided equality function.
typedef bool equiv(elem x, elem y);
// Client provided print function.
typedef void printElem(elem x);

struct unrolled_list
{
    int nextAvailSpot;   // Next spot in an array to insert element.
    size_t numNodes;     // Number of nodes in the list
    size_t totalElems;   // Total number of elements in all arrays of all nodes.
    size_t slotsPerNode; // User defined size of each node's array
    node *head;          // Head of the list
    node *tail;          // Tail of the list
    equiv *equality;     // Pointer to the equality function.
};

typedef struct unrolled_list ull;

// Makes a generic node
// The ull* list is passed in to determine slotsPerNode to kmalloc
// Returns pointer to the new node
node *makeNewNode(ull *list)
{
    node *first = kmalloc(sizeof(node), GFP_KERNEL);
    if (first == NULL)
    {
        printk("Not enough space for new node. (Malloc failed)\n");
        return NULL;
    }

    first->data = kmalloc(sizeof(elem) * list->slotsPerNode, GFP_KERNEL);
    first->next = NULL;

    return first;
}

// New unrolled linked list.
// Pass in size for slots per node.
ull *new_unrolled(size_t slotsPerNode, equiv *equal)
{
    ull *new = kmalloc(sizeof(ull), GFP_KERNEL);
    if (new == NULL)
    {
        printk("Not enough space for new list. (Malloc failed)\n");
        return NULL;
    }

    new->numNodes = 1;
    new->totalElems = 0;
    new->slotsPerNode = slotsPerNode;
    new->nextAvailSpot = 0;
    node *first = makeNewNode(new);

    new->head = first;
    new->tail = first;

    // Add the equality function the struct for future comparisions.
    if (equal == NULL)
    {
        printk("NULL given as equality function \n");
        return NULL;
    }

    new->equality = equal;

    return new;
}

// Find the last node and find nextAvailSpot in the array. Insert in this node.
// If the next spot updates to a size greater than the limit, then we reset
// the nextAvailSpot and create a new node.
ull *insert_unrolled(elem e, ull *list)
{
    if (list == NULL)
    {
        printk("NULL List given to insert\n");
        return NULL;
    }
    node *tail = list->tail;
    tail->data[list->nextAvailSpot] = e;
    list->nextAvailSpot++;
    list->totalElems++;

    // In case we need to make new node
    if (list->nextAvailSpot >= list->slotsPerNode)
    {
        list->nextAvailSpot = 0;
        list->numNodes++;
        node *newTail = makeNewNode(list);
        tail->next = newTail;
        list->tail = newTail;
    }

    return list;
}

// Helper function for the delete function. Finds the node before the tail.
node *findSecondLastTail(ull *list)
{
    if (list->head == list->tail)
        return list->head;

    node *temp = list->head;
    node *temp1 = list->head->next;
    while (temp != NULL)
    {
        if (temp1 == list->tail)
            return temp;

        temp = temp1;
        temp1 = temp->next;
    }
    return NULL;
}

// Helper function for the delete function. Finds the element at the end.
elem findLastElem(ull *list)
{

    if (list->nextAvailSpot != 0)
    {
        node *F = list->tail;
        return F->data[list->nextAvailSpot - 1];
    }

    if (list->nextAvailSpot == 0 && list->tail == list->head)
    {
        printk("Cannot find last elem in empty list\n");
    }

    node *temp = list->head;
    node *temp1 = list->head->next;
    // Go to end
    while (temp != NULL)
    {
        if (temp1 == list->tail)
            return temp->data[list->slotsPerNode - 1];

        temp = temp1;
        temp1 = temp->next;
    }

    return list->head->data[0];
}

// Returns true if the element is the last one in the unrolled linked list
bool isLastElem(ull *list, node *nodeOfElem, size_t numOfElem)
{
    if (list->nextAvailSpot == 0)
    {
        if (findSecondLastTail(list) == nodeOfElem &&
            numOfElem == list->slotsPerNode - 1)
            return true;
    }
    else
    {
        if (nodeOfElem == list->tail && numOfElem == list->nextAvailSpot - 1)
            return true;
    }
    return false;
}

// A delete that does not preserve ordering of inserts.
// Deletes the element from the array and replaces it with
// the last element of the list. If the element is the last one, then just
// adjust metadata and return.
ull *delete_unrolled(elem e, ull *list)
{
    node *i;
    node *nodeOfElem;

    size_t j;
    size_t numOfElem;

    bool stop = false;

    // Find the element to delete, go thru each node and each array to search.
    for (i = list->head; i != NULL && !stop; i = i->next)
    {
        for (j = 0; j < list->slotsPerNode && !stop; j++)
        {
            if ((*list->equality)(i->data[j], e))
            {
                nodeOfElem = i;
                numOfElem = j;
                stop = true;
            }
        }
    }

    if (stop == false)
    {
        printk("Element not found in list to delete\n");
        return NULL;
    }

    // If element not the last element, just replace it with last element
    if (!isLastElem(list, nodeOfElem, numOfElem))
        nodeOfElem->data[numOfElem] = findLastElem(list);

    list->totalElems--;
    list->nextAvailSpot--;

    // In case we delete the first element of the last node, need to roll back
    if (list->nextAvailSpot < 0)
    {
        list->nextAvailSpot = list->slotsPerNode - 1;
        node *oldTail = list->tail;
        list->tail = findSecondLastTail(list);
        list->tail->next = NULL;
        kfree(oldTail->data);
        kfree(oldTail);
        list->numNodes--;
    }

    return list;
}

// Return size of the unrolled linked list (total elements in the list)
size_t getSize_unrolled(ull *list)
{
    return list->totalElems;
}

// Return number of nodes.
size_t getNumNodes_unrolled(ull *list)
{
    return list->numNodes;
}

// Print function, Client must pass in a printing function for the element.
void printList(ull *list, printElem *printFunction)
{
    node *i;
    size_t j;
    for (i = list->head; i != NULL; i = i->next)
    {
        for (j = 0; (i == list->tail && j < list->nextAvailSpot) || (i != list->tail && j < list->slotsPerNode); j++)
        {
            (*printFunction)(i->data[j]);
        }
    }
}

bool lookup(elem e, ull *list)
{
    node *i;
    size_t j;

    for (i = list->head; i != NULL; i = i->next)
    {
        for (j = 0; j < list->slotsPerNode; j++)
        {
            if ((*list->equality)(i->data[j], e))
            {
                return true;
            }
        }
    }
    return false;
}

// Standard kfree function. First kfrees each array, and then kfrees the
void kfree_unrolled(ull *list)
{
    if (list == NULL)
    {
        printk("NULL List given to kfree\n");
        return;
    }

    node *i = list->head;
    node *j;

    while (i != NULL)
    {
        kfree(i->data); // kfree the array in the node
        j = i;          // Save the struct itself
        i = i->next;    // get the next struct
        kfree(j);       // kfree the struct itself
    }

    kfree(list); // Free the list header struct
    return;
}

int example(int number_of_nodes)
{
    ull *new = new_unrolled(number_of_nodes, &unrolled_equality);

    int i;
    for (i = 0; i < number_of_nodes; i++)
    {
        insert_unrolled(i, new);
    }
    printList(new, &printFunction);

    // Free the list
    kfree_unrolled(new);
    return 0;
}

int __init hello_module_init(void)
{
    printk(KERN_EMERG "Hello Module\n");
    example(1000);
    return 0;
}

void __exit hello_module_cleanup(void)
{
    printk("Bye Module\n");
}
module_init(hello_module_init);
module_exit(hello_module_cleanup);
