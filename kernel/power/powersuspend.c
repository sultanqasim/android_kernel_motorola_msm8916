/* kernel/power/powersuspend.c
 *
 * Copyright (C) 2005-2008 Google, Inc.
 * Copyright (C) 2013 Paul Reioux
 *
 * Modified by Jean-Pierre Rasquin <yank555.lu@gmail.com>
 *
 *   make powersuspend not depend on a userspace initiator anymore,
 *   but use a hook in autosleep instead.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/powersuspend.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/workqueue.h>

#define MAJOR_VERSION	1
#define MINOR_VERSION	1

//#define POWER_SUSPEND_DEBUG

struct workqueue_struct *suspend_work_queue;

static DEFINE_MUTEX(power_suspend_lock);
static LIST_HEAD(power_suspend_handlers);
static void power_suspend(struct work_struct *work);
static void power_resume(struct work_struct *work);
static DECLARE_WORK(power_suspend_work, power_suspend);
static DECLARE_WORK(power_resume_work, power_resume);
static DEFINE_SPINLOCK(state_lock);

static int state;

bool screen_on = true;

void register_power_suspend(struct power_suspend *handler)
{
	struct list_head *pos;

	mutex_lock(&power_suspend_lock);
	list_for_each(pos, &power_suspend_handlers) {
		struct power_suspend *p;
		p = list_entry(pos, struct power_suspend, link);
	}
	list_add_tail(&handler->link, pos);
	mutex_unlock(&power_suspend_lock);
}
EXPORT_SYMBOL(register_power_suspend);

void unregister_power_suspend(struct power_suspend *handler)
{
	mutex_lock(&power_suspend_lock);
	list_del(&handler->link);
	mutex_unlock(&power_suspend_lock);
}
EXPORT_SYMBOL(unregister_power_suspend);

static void power_suspend(struct work_struct *work)
{
	struct power_suspend *pos;
	unsigned long irqflags;
	int abort = 0;

#ifdef POWER_SUSPEND_DEBUG
	pr_warn("power_suspend: entering suspend...\n");
#endif
	mutex_lock(&power_suspend_lock);
	spin_lock_irqsave(&state_lock, irqflags);
	if (state == POWER_SUSPEND_INACTIVE)
		abort = 1;
	spin_unlock_irqrestore(&state_lock, irqflags);

	if (abort)
		goto abort_suspend;

#ifdef POWER_SUSPEND_DEBUG
	pr_warn("power_suspend: suspending...\n");
#endif
	list_for_each_entry(pos, &power_suspend_handlers, link) {
		if (pos->suspend != NULL) {
			pos->suspend(pos);
		}
	}
#ifdef POWER_SUSPEND_DEBUG
	pr_warn("power_suspend: suspended.\n");
#endif
abort_suspend:
	mutex_unlock(&power_suspend_lock);
}

static void power_resume(struct work_struct *work)
{
	struct power_suspend *pos;
	unsigned long irqflags;
	int abort = 0;

#ifdef POWER_SUSPEND_DEBUG
	pr_warn("powersuspend: entering resume...\n");
#endif
	mutex_lock(&power_suspend_lock);
	spin_lock_irqsave(&state_lock, irqflags);
	if (state == POWER_SUSPEND_ACTIVE)
		abort = 1;
	spin_unlock_irqrestore(&state_lock, irqflags);

	if (abort)
		goto abort_resume;

#ifdef POWER_SUSPEND_DEBUG
	pr_warn("powersuspend: resuming...\n");
#endif
	list_for_each_entry_reverse(pos, &power_suspend_handlers, link) {
		if (pos->resume != NULL) {
			pos->resume(pos);
		}
	}
#ifdef POWER_SUSPEND_DEBUG
	pr_warn("power_suspend: resumed.\n");
#endif
abort_resume:
	mutex_unlock(&power_suspend_lock);
}

void set_power_suspend_state_hook(int new_state)
{
	unsigned long irqflags;
	int old_sleep;

	spin_lock_irqsave(&state_lock, irqflags);
	old_sleep = state;
	if (old_sleep == POWER_SUSPEND_INACTIVE && new_state == POWER_SUSPEND_ACTIVE) {
#ifdef POWER_SUSPEND_DEBUG
		pr_warn("power_suspend: activated.\n");
#endif
		state = new_state;
		queue_work(suspend_work_queue, &power_suspend_work);
	} else if (old_sleep == POWER_SUSPEND_INACTIVE || new_state == POWER_SUSPEND_INACTIVE) {
#ifdef POWER_SUSPEND_DEBUG
		pr_warn("power_suspend: deactivated.\n");
#endif
		state = new_state;
		queue_work(suspend_work_queue, &power_resume_work);
	}
	spin_unlock_irqrestore(&state_lock, irqflags);
}

EXPORT_SYMBOL(set_power_suspend_state_hook);

static ssize_t power_suspend_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
        return sprintf(buf, "%u\n", state);
}

static struct kobj_attribute power_suspend_attribute =
	__ATTR(power_suspend_state, 0444,
		power_suspend_show,
		NULL);

static ssize_t power_suspend_version_show(struct kobject *kobj,
		struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "version: %d.%d\n", MAJOR_VERSION, MINOR_VERSION);
}

static struct kobj_attribute power_suspend_version_attribute =
	__ATTR(power_suspend_version, 0444,
		power_suspend_version_show,
		NULL);

static struct attribute *power_suspend_attrs[] =
{
	&power_suspend_attribute.attr,
	&power_suspend_version_attribute.attr,
	NULL,
};

static struct attribute_group power_suspend_attr_group =
{
	.attrs = power_suspend_attrs,
};

static struct kobject *power_suspend_kobj;

static int __init power_suspend_init(void)
{

	int sysfs_result;

        power_suspend_kobj = kobject_create_and_add("power_suspend",
				kernel_kobj);
        if (!power_suspend_kobj) {
                pr_err("%s kobject create failed!\n", __FUNCTION__);
                return -ENOMEM;
        }

        sysfs_result = sysfs_create_group(power_suspend_kobj,
			&power_suspend_attr_group);

        if (sysfs_result) {
                pr_info("%s group create failed!\n", __FUNCTION__);
                kobject_put(power_suspend_kobj);
                return -ENOMEM;
        }

	suspend_work_queue = create_singlethread_workqueue("p-suspend");

	if (suspend_work_queue == NULL) {
		return -ENOMEM;
	}

	return 0;
}

static void __exit power_suspend_exit(void)
{
	if (power_suspend_kobj != NULL)
		kobject_put(power_suspend_kobj);

	destroy_workqueue(suspend_work_queue);
} 

core_initcall(power_suspend_init);
module_exit(power_suspend_exit);

MODULE_AUTHOR("Paul Reioux <reioux@gmail.com> / Jean-Pierre Rasquin <yank555.lu@gmail.com>");
MODULE_DESCRIPTION("power_suspend - A replacement kernel PM driver for"
        "Android's deprecated early_suspend/late_resume PM driver!");
MODULE_LICENSE("GPL v2");


