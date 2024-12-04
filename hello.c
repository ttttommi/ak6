#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/ktime.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/types.h>
#include <asm-generic/bug.h>
#include "hello.h"

MODULE_AUTHOR("Borysenko Tamila, IO-25");
MODULE_DESCRIPTION("Hello, world in Linux Kernel Training");
MODULE_LICENSE("Dual BSD/GPL");

#define BUG_ON_MSG(condition, message) \
	do { \
		if (unlikely(condition)) { \
			pr_warn(message); \
			BUG(); \
		} \
	} while (0)

static unsigned int myparam = 1;
static unsigned int alloc_count;

struct log_event {
	struct list_head list;
	ktime_t timestamp;
};

module_param(myparam, uint, 0644);
MODULE_PARM_DESC(myparam, "The number of times to print the message 'Hello, world!'");
static struct list_head event_list_head;
static LIST_HEAD(event_list_head);

static int __init hello_init(void)
{
	int i = 0;

	if (myparam == 0 || (myparam >= 5 && myparam <= 10))
		pr_warn("Warning: myparam is 0 or between 5 and 10.");

	BUG_ON(myparam > 10);

	for (i = 0; i < myparam; i++) {
		pr_info("Hello, world!\n");
		new_event();
	}

	return 0;
}

static void __exit hello_exit(void)
{
	struct log_event *entry, *tmp;

	list_for_each_entry_safe(entry, tmp, &event_list_head, list) {
		pr_info("Time: %lld\n", entry->timestamp);
		list_del(&entry->list);
		kfree(entry);
	}
}

void new_event(void)
{
	struct log_event *new_log;

	new_log = kmalloc(sizeof(struct log_event), GFP_KERNEL);
	alloc_count++;
	BUG_ON_MSG(alloc_count == 5, "kmallo returned NULL");
	new_log->timestamp = ktime_get();
	list_add_tail(&new_log->list, &event_list_head);
}

module_init(hello_init);
module_exit(hello_exit);