#include <linux/init.h>
#include <linux/module.h>

#include "md.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maslova Marina");


static int __init md_init(void)
{
	printk("md2 init\n");
	printk("md2 exported from md1 : %s\n", md1_data);
	printk("md2 md1_proc() returned string : %s\n", md1_proc());

    //printk("md2 local from md1: %s\n", md1_local());
    //printk("md2 noexport from md1: %s\n", md1_noexport());	

	return 0;
}


static void __exit md_exit(void)
{
	printk("md2 exit\n");
}

module_init(md_init);
module_exit(md_exit);
