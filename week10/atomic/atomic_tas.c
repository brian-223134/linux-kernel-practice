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
static int tas_lock = 0;   // 0: unlocked, 1: locked

/* Forward declaration */
static int worker_tas(void *data);

static int __init atomic_tas_init(void)
{
    int i;

    pr_info("[TAS] module init\n");
    counter = 0;
    tas_lock = 0;

    for (i = 0; i < NUM_THREADS; i++) {
        threads[i] = kthread_run(worker_tas, (void *)(long)(i + 1),
                                 "atomic_tas_%d", i + 1);
        if (IS_ERR(threads[i])) {
            pr_err("[TAS] failed to create thread %d\n", i + 1);
            threads[i] = NULL;
        }
    }

    return 0;
}

static void __exit atomic_tas_exit(void)
{
    int i;

    for (i = 0; i < NUM_THREADS; i++) {
        if (threads[i]) {
            /* * Threads are kept alive in the loop, 
             * so it is safe to call kthread_stop().
             */
            kthread_stop(threads[i]);
        }
    }

    pr_info("[TAS] module exit, final counter=%d\n", counter);
}

static int worker_tas(void *data)
{
    int id = (long)data;

    while (!kthread_should_stop()) {
        int now;

        /* 1. Check if target reached (Wait mode) */
        /* Note: Reading 'counter' here is not atomic, but safe enough for a hint */
        if (counter >= MAX_COUNT) {
            msleep(100); // Sleep longer to save CPU
            continue;    // Skip logic and check stop signal again
        }

        /* * 2. Acquire lock: spin until we successfully set it to 1 
         * __sync_lock_test_and_set returns the previous value.
         * If it returns 1, it was already locked -> keep spinning.
         */
        while (__sync_lock_test_and_set(&tas_lock, 1)) {
            /* * Safety: Check stop signal even while spinning.
             * If rmmod is called while waiting for lock, we must exit.
             */
            if (kthread_should_stop())
                goto exit_thread;

            cpu_relax();
        }

        /* --- Critical Section Start --- */
        
        /* Check counter again inside the lock to ensure correctness */
        if (counter < MAX_COUNT) {
            counter++;
            now = counter;
            
            printk(KERN_INFO "[TAS] thread #%d (pid=%d) counter=%d\n",
                   id, current->pid, now);
        } else {
            /* Another thread might have finished the job while we were spinning */
            now = counter;
        }

        /* --- Critical Section End --- */

        /* 3. Release lock: set to 0 */
        __sync_lock_release(&tas_lock);

        msleep(10);
    }

exit_thread:
    printk(KERN_INFO "[TAS] thread #%d exiting\n", id);
    return 0;
}

module_init(atomic_tas_init);
module_exit(atomic_tas_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("student");
MODULE_DESCRIPTION("Atomic test-and-set example with safe thread exit");
