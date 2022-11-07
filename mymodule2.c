#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/sched/signal.h>

// Meta Information
MODULE_LICENSE("GPL");
MODULE_AUTHOR("OMER FARUK TAL");
//MODULE_DESCRIPTION("A module that knows how to greet");

int processID = 0;

module_param(processID, int, 0);
MODULE_PARM_DESC(processID, "id of the process");

void treeTraversal(struct task_struct *t);

// A function that runs when the module is first loaded
int simple_init(void) {
    struct pid *pid_struct;
    struct task_struct *task;
    
    printk(KERN_INFO "Loading Module PSVIS\n");
    printk(KERN_INFO " %d NAN 1\n", processID);


    pid_struct = find_get_pid(processID);
    task = pid_task(pid_struct, PIDTYPE_PID);

    if (task == NULL) {
        printk(KERN_ERR "The process ID is not found!\n");
        return 1;
    }
    
    treeTraversal(task);

    return 0;
}

// A function that runs when the module is removed
void simple_exit(void) {
}

void treeTraversal(struct task_struct *t) {
    
    

    struct task_struct *inputTask = t;
    struct task_struct *oldestTask = NULL;
    struct task_struct *tempTask;
    struct list_head *list;

    long oldestTime = -1;
    list_for_each(list, &inputTask->children) {
        tempTask = list_entry(list, struct task_struct, sibling);
        if (oldestTime == -1 || oldestTime > tempTask->start_time) {
            oldestTask = tempTask;
            oldestTime = tempTask->start_time;
        }
        printk(KERN_INFO "\t%d %d 0", (tempTask->parent)->pid, tempTask->pid);
        treeTraversal(tempTask);
    }

    if (oldestTask != NULL) {
        printk(KERN_INFO "\t%d %d 1", (oldestTask->parent)->pid, oldestTask->pid);
    }

}


module_init(simple_init);
module_exit(simple_exit);












