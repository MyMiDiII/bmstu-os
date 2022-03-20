#include <linux/init.h>
#include <linux/module.h>

#include "md.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Maslova Marina");

static int __init md_init(void)
{
	printk("md3 init\n");

	return -1;
}

module_init(md_init);
