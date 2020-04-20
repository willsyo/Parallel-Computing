#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/slab.h>

typedef struct person {
    int month;
    int day;
    int year;
    struct person *next;

} person;



// creates a new person struct, fills it with the parameter values

person* create(int m, int d, int y) {
    person* new = kmalloc(sizeof (*new), GFP_KERNEL);
    if (new == NULL) {
        printk(KERN_INFO "Not enough memory to create person item\n");
    } else {
        new->month = m;
        new->day = d;
        new->year = y;
        new->next = NULL;
    }
    return new;
}

person* append(person* head, person* new) {
    person* tmp = head;
    if (head != NULL) {
        while (tmp->next != NULL) {
            tmp = tmp->next;
        }
        tmp->next = new;
    } else {
        head = new;
    }
    return head;
}

void printList(person *head) {
    person *tmp = head;
    
    printk("Contents of list is: ");
    printk("Month, Day, Year\n");
    printk("----------------\n");
    while (tmp != NULL) {
        printk("%d, %d, %d\n", tmp->month, tmp->day, tmp->year);
        tmp = tmp->next;
    }

}

/* This function is called when the module is loaded. */
int simple_init(void) {
    printk(KERN_INFO "Loading Module...\n");

    person* Head;
    person* p1 = create(7, 2, 1995);
    Head = append(Head, p1);
    person* p2 = create(10, 22, 2000);
    Head = append(Head, p2);
    person* p3 = create(1, 1, 1800);
    Head = append(Head, p3);
    person* p4 = create(1, 1, 1802);
    Head = append(Head, p4);
    person* p5 = create(2, 1, 1830);
    Head = append(Head, p5);
    printList(Head);

    return 0;
}

/* This function is called when the module is removed. */
void simple_exit(void) {
    printk("Month: %d, Day: %d, Year: %d \n", 0, 0, 0);
    printk(KERN_INFO "Removing Module\n");
}

/* Macros for registering module entry and exit points. */
module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");

