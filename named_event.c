#include<linux/kernel.h>
#include<linux/string.h>
#include<linux/list.h>
#include<linux/slab.h>
#include<linux/uaccess.h>
#include<linux/wait.h>
#include<linux/spinlock.h>

struct my_event{
	int id;
	char *name;
	wait_queue_head_t wait_q;
	struct list_head event_list;
};
struct my_event_mgr{
	int freshid;
	struct list_head event_list;
	spinlock_t lock;
};
static struct my_event_mgr event_mgr = {
	.freshid = 0,
	.event_list = LIST_HEAD_INIT(event_mgr.event_list),
	.lock = __SPIN_LOCK_INITIALIZER(lock),
};

//event open section
asmlinkage long sys_event_open(char *name, int namelen, int *id)
{
	struct list_head *position;
	struct my_event *event_1;
	struct my_event *event_2;
	char *name_1;
	bool flag = false;
	int user_id = 0;

	spin_lock(&event_mgr.lock);

	list_for_each(position, &event_mgr.event_list)
	{
		event_1 = list_entry(position, struct my_event, event_list);
		printk("id : [%d] the event name is : %s, the given name is : %s\n", *id, name_1, name);
		if(strcmp(event_1->name, name) == 0)
		{
			flag = true;
			break;
		}
	}
	if(flag)
	{
		user_id = event_1->id;
		if(copy_to_user(id, &user_id, sizeof(user_id)))
		{	
			spin_unlock(&event_mgr.lock);
			return -EFAULT;
		}
	}
	else
	{
		event_2 = kmalloc(sizeof(struct my_event), GFP_KERNEL);
		event_2->name = kmalloc(sizeof(char*)*namelen, GFP_KERNEL);
		snprintf(event_2->name, namelen, "%s", name);
		

		if(list_empty(&event_mgr.event_list))
		{
			event_2->id = event_mgr.freshid + 1;
			if(copy_to_user(id, &event_2->id, sizeof(user_id)))
			{
				spin_unlock(&event_mgr.lock);
				return -EFAULT;
			}
		}
		else
		{
			event_1 = list_last_entry(&event_mgr.event_list, struct my_event, event_list);
			event_2->id = event_1->id + 1;
			user_id = event_2->id;
			if(copy_to_user(id, &user_id, sizeof(user_id)))
			{
				spin_unlock(&event_mgr.lock);
				return -EFAULT;
			}
		}
		init_waitqueue_head(&event_2->wait_q);
		INIT_LIST_HEAD(&event_2->event_list);
		list_add(&event_2->event_list, &event_mgr.event_list);
	}
	spin_unlock(&event_mgr.lock);

	return 0;
	
}

//event close section
asmlinkage long sys_event_close(int id)
{
	struct list_head *position;
	struct my_event *event_1;

	spin_lock(&event_mgr.lock);
	if(list_empty(&event_mgr.event_list))
	{
		spin_unlock(&event_mgr.lock);
		return -EFAULT;
	}
	else
	{
		list_for_each(position, &event_mgr.event_list)
		{
			event_1 = list_entry(position, struct my_event, event_list);
			if(event_1->id == id)
			{
				if(list_empty(&event_1->wait_q.task_list))
				{
					list_del(&event_1->event_list);
					kfree(event_1);
					spin_unlock(&event_mgr.lock);
					return 0;
				}
			}
		}

	}
	spin_unlock(&event_mgr.lock);
	return -EFAULT;
}

//event wait section
asmlinkage long sys_event_wait(int id)
{
	struct list_head *position;
	struct my_event *event_1;
	DEFINE_WAIT(wait);
	spin_lock(&event_mgr.lock);

	init_waitqueue_entry(&wait, current);
	current->state = TASK_INTERRUPTIBLE;
	if(list_empty(&event_mgr.event_list))//empty
	{
		spin_unlock(&event_mgr.lock);
		return -EFAULT;
	}
	else
	{
		list_for_each(position, &event_mgr.event_list)
		{
			event_1 = list_entry(position, struct my_event, event_list);
			if(event_1->id == id)
			{
				add_wait_queue_exclusive(&event_1->wait_q, &wait);

				printk("before scheduling takes place\n");
				spin_unlock(&event_mgr.lock);
				schedule();//wait
				spin_lock(&event_mgr.lock);
				printk("after scheduling takes place\n");

				remove_wait_queue(&event_1->wait_q, &wait);

				printk("leaving the function %s\n", __FUNCTION__);
				spin_unlock(&event_mgr.lock);
				return 0;
			}
		}

	}

	spin_unlock(&event_mgr.lock);

	return -EFAULT;

}

//event signal section
asmlinkage long sys_event_sig(int id)
{
	struct list_head *position;
	struct my_event *event_1;
	spin_lock(&event_mgr.lock);

	if(list_empty(&event_mgr.event_list))
	{
		spin_unlock(&event_mgr.lock);
		return -EFAULT;
	}
	else
	{
		list_for_each(position, &event_mgr.event_list)
		{
			event_1 = list_entry(position, struct my_event, event_list);
			if(event_1->id == id)
			{
				
				wake_up(&event_1->wait_q);
				spin_unlock(&event_mgr.lock);	
				return 0;
			}
		}
	}
	

	spin_unlock(&event_mgr.lock);	
	return -EFAULT;
}
	
