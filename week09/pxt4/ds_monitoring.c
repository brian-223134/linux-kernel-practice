// ds_monitoring.c
#include "ds_monitoring.h"
#include <linux/string.h>
#include <linux/rcupdate.h>
#include <linux/xarray.h>

static void insert_ds_monitoring(struct ds_monitoring *dm,
                                 unsigned long index, void *elem)
{
    const char *src = NULL;
    struct ds_monitoring_elem *new;

    new = kmalloc(sizeof(*new), GFP_KERNEL);
    if (!new)
        return;

    new->key   = index;
    new->count = 1ULL;

    if (dm->dm_ops->get_name) {
        src = dm->dm_ops->get_name(elem);
        if (src) {
            new->name = kstrdup(src, GFP_KERNEL);
            if (!new->name) {
                kfree(new);
                return;
            }
        } else {
            new->name = NULL;
        }
    } else {
        new->name = NULL;
    }

    xa_store(dm->elements, new->key, (void *)new, GFP_KERNEL);
}

void find_ds_monitoring(struct ds_monitoring *dm, void *elem)
{
    struct ds_monitoring_elem *cur;
    unsigned long xa_index;

    if (!dm || !dm->dm_ops || !dm->dm_ops->get_index)
        return;

    xa_index = dm->dm_ops->get_index(elem);

    cur = (struct ds_monitoring_elem *)xa_load(dm->elements, xa_index);
    if (cur) {
        __sync_fetch_and_add(&cur->count, 1ULL);
    } else {
        insert_ds_monitoring(dm, xa_index, elem);
    }
    __sync_fetch_and_add(&dm->total_counts, 1ULL);
}

void print_ds_monitoring(struct ds_monitoring *dm)
{
    unsigned long idx;
    void *p;

    if (!dm || !dm->total_counts)
        return;

    xa_for_each(dm->elements, idx, p) {
        const char *name;
        unsigned long long cnt;
        int pct;

        name = ((struct ds_monitoring_elem *)p)->name;
        cnt  = ((struct ds_monitoring_elem *)p)->count;
        pct  = (int)(cnt * 100 / dm->total_counts);

        dm->dm_ops->print_elem(idx, name, cnt, pct);
    }
}

void delete_ds_monitoring(struct ds_monitoring *dm)
{
    unsigned long idx;
    void *p;

    if (!dm)
        return;

    xa_for_each(dm->elements, idx, p) {
        struct ds_monitoring_elem *cur = p;
        kfree(cur->name);
        kfree(cur);
    }
    xa_destroy(dm->elements);
}

