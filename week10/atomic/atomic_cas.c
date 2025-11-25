#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/slab.h>

#define NUM_THREADS 4
#define MAX_COUNT   20

static int counter = 0;
static struct task_struct *threads[NUM_THREADS];


static int worker_cas(void *data);

static int __init atomic_cas_init(void)
{
    int i;

    pr_info("[CAS] module init\n");
    counter = 0;

    for (i = 0; i < NUM_THREADS; i++) {
        threads[i] = kthread_run(worker_cas, (void *)(long)(i + 1),
                                 "atomic_cas_%d", i + 1);

        if (IS_ERR(threads[i])) {
            pr_err("[CAS] failed to create thread %d\n", i + 1);
            threads[i] = NULL;
        }
    }

    return 0;
}

static void __exit atomic_cas_exit(void)
{
    int i;

    /* Tell all threads to stop */
    for (i = 0; i < NUM_THREADS; i++) {
        if (threads[i]) {
            kthread_stop(threads[i]);
        }
    }

    pr_info("[CAS] module exit, final counter=%d\n", counter);
}

static int worker_cas(void *data)
{
    int id = (long)data;

    while (!kthread_should_stop()) {
        int old, new, prev;

        if (counter >= MAX_COUNT) {
            msleep(100);
            continue;
        }

        /* CAS loop */
        do {
            if (kthread_should_stop())
                goto exit_thread;

            old  = counter;
            
            if (old >= MAX_COUNT) {
                break;
            }

            new  = old + 1;
            prev = __sync_val_compare_and_swap(&counter, old, new);

            if (prev != old)
                cpu_relax();   // reduce tight spinning
        } while (prev != old);
        
        if (prev == old && new <= MAX_COUNT) {
             printk(KERN_INFO "[CAS] thread #%d (pid=%d) counter=%d\n",
                   id, current->pid, new);
        }

        msleep(10);
    }

exit_thread:
    printk(KERN_INFO "[CAS] thread #%d exiting\n", id);
    return 0;
}

module_init(atomic_cas_init);
module_exit(atomic_cas_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("student");
MODULE_DESCRIPTION("Atomic compare-and-swap example with safe thread exit");
