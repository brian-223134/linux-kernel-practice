#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/rwsem.h>
#include "calclock.h"

#define NUM_THREADS 4
#define TOTAL_NODES 1000000
#define NODES_PER_THREAD (TOTAL_NODES / NUM_THREADS)

static DECLARE_RWSEM(my_rwsem);
static LIST_HEAD(my_list);

struct my_node {
    int data;
    struct list_head list;
};

KTDEF(insert_time);
KTDEF(search_time);
KTDEF(delete_time);

static struct task_struct *threads[NUM_THREADS];

void add_to_list(int thread_id, int range_bound[])
{
    int i;
    ktime_t localclocks[2];
    struct my_node *new_node;

    ktget(&localclocks[0]);

    for (i = range_bound[0]; i < range_bound[1]; i++) {
        new_node = kmalloc(sizeof(struct my_node), GFP_KERNEL);
        if (!new_node) continue;
        new_node->data = i;
        INIT_LIST_HEAD(&new_node->list);

        down_write(&my_rwsem); // 쓰기 락
        list_add_tail(&new_node->list, &my_list);
        up_write(&my_rwsem);
    }

    ktget(&localclocks[1]);
    ktput(localclocks, insert_time);
}

void search_list(int thread_id, int range_bound[])
{
    struct my_node *cur;
    ktime_t localclocks[2];
    int count = 0;

    ktget(&localclocks[0]);

    // [최적화] 리스트 전체를 한 번만 순회 (O(N))
    down_read(&my_rwsem); // 읽기 락 (여러 스레드 동시 접근 가능)
    list_for_each_entry(cur, &my_list, list) {
        if (cur->data >= range_bound[0] && cur->data < range_bound[1]) {
            count++;
        }
    }
    up_read(&my_rwsem);

    ktget(&localclocks[1]);
    ktput(localclocks, search_time);
}

void delete_from_list(int thread_id, int range_bound[])
{
    struct my_node *cur, *tmp;
    ktime_t localclocks[2];

    ktget(&localclocks[0]);

    down_write(&my_rwsem); // 쓰기 락
    list_for_each_entry_safe(cur, tmp, &my_list, list) {
        if (cur->data >= range_bound[0] && cur->data < range_bound[1]) {
            list_del(&cur->list);
            kfree(cur);
        }
    }
    up_write(&my_rwsem);

    ktget(&localclocks[1]);
    ktput(localclocks, delete_time);
}

static int work_fn(void *data)
{
    int thread_id = (long)data;
    int range_bound[2];

    range_bound[0] = (thread_id - 1) * NODES_PER_THREAD;
    range_bound[1] = thread_id * NODES_PER_THREAD;

    printk(KERN_INFO "thread #%d range: %d~%d\n", thread_id, range_bound[0], range_bound[1] - 1);

    add_to_list(thread_id, range_bound);
    search_list(thread_id, range_bound);
    printk(KERN_INFO "thread #%d searched range: %d~%d\n", thread_id, range_bound[0], range_bound[1] - 1);
    
    delete_from_list(thread_id, range_bound);
    printk(KERN_INFO "thread #%d deleted range: %d~%d\n", thread_id, range_bound[0], range_bound[1] - 1);

    while (!kthread_should_stop()) {
        msleep(100);
    }

    printk(KERN_INFO "thread #%d stopped!\n", thread_id);
    return 0;
}

static int __init rwsem_module_init(void)
{
    int i;
    pr_info("rwsem_module_init: Entering RW Semaphore Module!\n");

    for (i = 0; i < NUM_THREADS; i++) {
        threads[i] = kthread_run(work_fn, (void *)(long)(i + 1), "worker_%d", i + 1);
    }
    return 0;
}

static void __exit rwsem_module_exit(void)
{
    int i;
    for (i = 0; i < NUM_THREADS; i++) {
        if (threads[i]) kthread_stop(threads[i]);
    }

    pr_info("rwsem_module_cleanup: RW Sem linked list insert time: ");
    ktprint(0, insert_time);
    pr_info("rwsem_module_cleanup: RW Sem linked list search time: ");
    ktprint(0, search_time);
    pr_info("rwsem_module_cleanup: RW Sem linked list delete time: ");
    ktprint(0, delete_time);
    pr_info("rwsem_module_cleanup: Exiting RW Semaphore Module!\n");
}

module_init(rwsem_module_init);
module_exit(rwsem_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("Linked List Synchronization with RW Semaphore");
