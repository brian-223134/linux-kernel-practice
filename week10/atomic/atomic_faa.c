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

/* Forward declaration */
static int worker_faa(void *data);

static int __init atomic_faa_init(void)
{
    int i;

    pr_info("[FAA] module init\n");
    counter = 0;

    for (i = 0; i < NUM_THREADS; i++) {
        threads[i] = kthread_run(worker_faa, (void *)(long)(i + 1),
                                 "atomic_faa_%d", i + 1);
        if (IS_ERR(threads[i])) {
            pr_err("[FAA] failed to create thread %d\n", i + 1);
            threads[i] = NULL;
        }
    }

    return 0;
}

static void __exit atomic_faa_exit(void)
{
    int i;

    for (i = 0; i < NUM_THREADS; i++) {
        if (threads[i]) {
            /* * Threads are kept alive in the wait loop, 
             * so it is safe to call kthread_stop().
             */
            kthread_stop(threads[i]);
        }
    }

    pr_info("[FAA] module exit, final counter=%d\n", counter);
}

static int worker_faa(void *data)
{
    int id = (long)data;

    /* Loop until kthread_stop() signal is received */
    while (!kthread_should_stop()) {
        
        /* 1. Check if target reached (Wait mode) */
        if (counter >= MAX_COUNT) {
            msleep(100); // Sleep longer to prevent CPU waste
            continue;    // Go back to the start to check stop signal
        }

        /* * 2. Execute Fetch-And-Add 
         * Note: A slight race condition might occur here where multiple 
         * threads pass the (counter >= MAX_COUNT) check simultaneously.
         * The final counter might slightly exceed MAX_COUNT (e.g., 21, 22).
         */
        {
            int old, now;
            
            old = __sync_fetch_and_add(&counter, 1); // Atomic operation
            now = old + 1;

            printk(KERN_INFO "[FAA] thread #%d (pid=%d) counter=%d\n",
                   id, current->pid, now);
        }

        msleep(10);
    }

    printk(KERN_INFO "[FAA] thread #%d exiting\n", id);
    return 0;
}

module_init(atomic_faa_init);
module_exit(atomic_faa_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("student");
MODULE_DESCRIPTION("Atomic fetch-and-add example with safe thread exit");
