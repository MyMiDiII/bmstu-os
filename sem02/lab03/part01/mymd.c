#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/init_task.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maslova Marina");

static int __init md_init(void)
{
    struct task_struct *task = &init_task;

    do {
        printk(KERN_INFO "mymd - comm: %s - pid: %d, state: %ld,\
 static_prio: %d; parent_comm: %s - ppid: %d\n",
               task->comm, task->pid, task->state, task->static_prio,
               task->parent->comm, task->parent->pid);
    } while ((task = next_task(task)) != &init_task);

    printk(KERN_INFO "mymd cur - comm: %s - pid: %d, state: %ld,\
 static_prio: %d; parent_comm: %s - ppid: %d\n",
               current->comm, current->pid, current->state,
               current->static_prio,
               current->parent->comm, current->parent->pid);
    return 0;
}

static void __exit md_exit(void)
{
    printk(KERN_INFO "mymd: Good by\n");
}

module_init(md_init);
module_exit(md_exit);
