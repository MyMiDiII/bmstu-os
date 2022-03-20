#include <linux/init.h>
#include <linux/module.h>

#include "md.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maslova Marina");

char *md1_data = "I will become a Hokage!";
char *md1_data2 = "Random string!";
char *md1_data3 = "ИУ7-63Б";

extern char *md1_proc(void)
{
    return md1_data;
}


static char *md1_local(void)
{
    return md1_data2;
}


extern char *md1_noexport(void)
{
    return md1_data3;
}

EXPORT_SYMBOL(md1_data);
EXPORT_SYMBOL(md1_proc);

static int __init md_init(void)
{
    printk(KERN_INFO "md1 init\n");
    printk(KERN_INFO "md1 proc: %s\n", md1_proc());
    printk(KERN_INFO "md1 local: %s\n", md1_local());
    printk(KERN_INFO "md1 noexport: %s\n", md1_noexport());

    return 0;
}


static void __exit md_exit(void)
{
    printk(KERN_INFO "md1 exit\n");
}

module_init(md_init);
module_exit(md_exit);
