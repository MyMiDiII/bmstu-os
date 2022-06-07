#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <asm/io.h>

#include "ascii.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maslova Marina");

struct tasklet_struct *my_tasklet;
char my_tasklet_data[] = "key is pressed";

int keyboard_irq = 1;

void my_tasklet_func(unsigned long data)
{
    int code;

    printk(KERN_INFO "MyTasklet: -------------------------");
    printk(KERN_INFO "MyTasklet: tasklet count = %u", my_tasklet->count.counter);
    printk(KERN_INFO "MyTasklet: tasklet state = %lu", my_tasklet->state);

    printk(KERN_INFO "MyTasklet: %s", (char *) data);

    code = inb(0x60);
    printk(KERN_INFO "MyTasklet: key code is %d", code);

    if (code < 84)
        printk(KERN_INFO "MyTasklet: the key is %s", ascii[code]);

    printk(KERN_INFO "MyTasklet: -------------------------");
}

irqreturn_t my_irq_handler(int irq, void *dev)
{
    printk(KERN_INFO "MyTasklet: my_irq_handler");

    if (irq == keyboard_irq)
    {
        printk(KERN_INFO "MyTasklet: called by keyboard_irq");
        printk(KERN_INFO "MyTasklet: tasklet state (before) = %lu",
                   my_tasklet->state);
        printk(KERN_INFO "MyTasklet: scheduling");


        tasklet_schedule(my_tasklet);

        printk(KERN_INFO "MyTasklet: tasklet state (after) = %lu",
                    my_tasklet->state);

        return IRQ_HANDLED;
    }

    printk(KERN_INFO "MyTasklet: called not by keyboard_irq");

    return IRQ_NONE;
}

static int __init my_tasklet_init(void)
{
    int ret;
    printk(KERN_INFO "MyTasklet: init");

    my_tasklet = kmalloc(sizeof(struct tasklet_struct), GFP_KERNEL);

    if (my_tasklet == NULL)
    {
        printk(KERN_ERR "MyTasklet: kmalloc error");
        return -1;
    }

    tasklet_init(my_tasklet, my_tasklet_func, (unsigned long)my_tasklet_data);

    ret = request_irq(keyboard_irq, my_irq_handler, IRQF_SHARED,
                      "test_my_irq_handler", (void *) my_irq_handler);

    if (ret)
        printk(KERN_ERR "MyTasklet: request_irq error");
    else
        printk(KERN_ERR "MyTasklet: loaded");

    return ret;
}

static void __exit my_tasklet_exit(void)
{
    printk(KERN_INFO "MyTasklet: exit");

    synchronize_irq(keyboard_irq);
    free_irq(keyboard_irq, my_irq_handler);

    tasklet_kill(my_tasklet);
    kfree(my_tasklet);
    
    printk(KERN_INFO "MyTasklet: unloaded");
}

module_init(my_tasklet_init);
module_exit(my_tasklet_exit);
