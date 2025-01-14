#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/slab.h>
#include "bn.h"

MODULE_LICENSE("Dual MIT/GPL");
MODULE_AUTHOR("National Cheng Kung University, Taiwan");
MODULE_DESCRIPTION("Fibonacci engine driver");
MODULE_VERSION("0.1");

#define DEV_FIBONACCI_NAME "fibonacci"

/* MAX_LENGTH is set to 92 because
 * ssize_t can't fit the number > 92
 */
#define MAX_LENGTH 1000

static dev_t fib_dev = 0;
static struct class *fib_class;
static DEFINE_MUTEX(fib_mutex);
static int major = 0, minor = 0;
static ktime_t kt;

static long long fib_sequence(long long k)
{
    /* FIXME: C99 variable-length array (VLA) is not allowed in Linux kernel. */
    long long f[k + 2];

    f[0] = 0;
    f[1] = 1;

    for (int i = 2; i <= k; i++) {
        f[i] = f[i - 1] + f[i - 2];
    }

    return f[k];
}

static long long fib_fast_doubling(long long k)
{
    long long f[k + 2];
    f[0] = 0;
    f[1] = 1;
    for (uint64_t i = 1ULL << 63; i; i >>= 1) {
        long long n0 = f[0] * (2 * f[1] - f[0]);
        long long n1 = f[0] * f[0] + f[1] * f[1];
        if (k & i) {
            f[0] = n1;
            f[1] = n0 + n1;
        } else {
            f[0] = n0;
            f[1] = n1;
        }
    }
    return f[0];
}

// ref
int fibn_per_32bit(int k)
{
    return k < 2 ? 1 : ((long) k * 694242 - 1160964) / (1000000) + 1;
}

void bn_fib(bn *ret, unsigned int n)
{
    int nsize = fibn_per_32bit(n) / 32 + 1;
    bn_resize(ret, nsize);
    if (n < 2) {
        ret->number[0] = n;
        return;
    }

    bn *f1 = bn_alloc(nsize);
    bn *tmp = bn_alloc(nsize);
    ret->number[0] = 0;
    f1->number[0] = 1;

    for (unsigned int i = 1; i < n + 1; i++) {
        bn_add(ret, f1, tmp);
        bn_swap(f1, ret);
        bn_swap(f1, tmp);
    }

    bn_free(f1);
    bn_free(tmp);
}

static int fib_open(struct inode *inode, struct file *file)
{
    if (!mutex_trylock(&fib_mutex)) {
        printk(KERN_ALERT "fibdrv is in use");
        return -EBUSY;
    }
    return 0;
}

static int fib_release(struct inode *inode, struct file *file)
{
    mutex_unlock(&fib_mutex);
    return 0;
}

/* calculate the fibonacci number at given offset */
static ssize_t fib_read(struct file *file,
                        char *buf,
                        size_t size,
                        loff_t *offset)
{
    bn *fib = bn_alloc(1);
    ktime_t start = ktime_get();
    // long long bn = fib_sequence(*offset);
    // long long bn = fib_fast_doubling(*offset);
    bn_fib(fib, *offset);
    kt = ktime_sub(ktime_get(), start);
    char *p = bn_to_string(*fib);
    size_t len = strlen(p) + 1;
    copy_to_user(buf, p, len);
    bn_free(fib);
    // return (ssize_t) bn;
    return ktime_to_ns(kt);
}

/* write operation is skipped */
static ssize_t fib_write(struct file *file,
                         const char *buf,
                         size_t size,
                         loff_t *offset)
{
    return (ssize_t) ktime_to_ns(kt);
}

static loff_t fib_device_lseek(struct file *file, loff_t offset, int orig)
{
    loff_t new_pos = 0;
    switch (orig) {
    case 0: /* SEEK_SET: */
        new_pos = offset;
        break;
    case 1: /* SEEK_CUR: */
        new_pos = file->f_pos + offset;
        break;
    case 2: /* SEEK_END: */
        new_pos = MAX_LENGTH - offset;
        break;
    }

    if (new_pos > MAX_LENGTH)
        new_pos = MAX_LENGTH;  // max case
    if (new_pos < 0)
        new_pos = 0;        // min case
    file->f_pos = new_pos;  // This is what we'll use now
    return new_pos;
}

const struct file_operations fib_fops = {
    .owner = THIS_MODULE,
    .read = fib_read,
    .write = fib_write,
    .open = fib_open,
    .release = fib_release,
    .llseek = fib_device_lseek,
};

static int __init init_fib_dev(void)
{
    int rc = 0;
    mutex_init(&fib_mutex);

    // Let's register the device
    // This will dynamically allocate the major number
    rc = major = register_chrdev(major, DEV_FIBONACCI_NAME, &fib_fops);
    if (rc < 0) {
        printk(KERN_ALERT "Failed to add cdev");
        rc = -2;
        goto failed_cdev;
    }
    fib_dev = MKDEV(major, minor);

    fib_class = class_create(THIS_MODULE, DEV_FIBONACCI_NAME);

    if (!fib_class) {
        printk(KERN_ALERT "Failed to create device class");
        rc = -3;
        goto failed_class_create;
    }

    if (!device_create(fib_class, NULL, fib_dev, NULL, DEV_FIBONACCI_NAME)) {
        printk(KERN_ALERT "Failed to create device");
        rc = -4;
        goto failed_device_create;
    }
    return rc;
failed_device_create:
    class_destroy(fib_class);
failed_class_create:
failed_cdev:
    unregister_chrdev(major, DEV_FIBONACCI_NAME);
    return rc;
}

static void __exit exit_fib_dev(void)
{
    mutex_destroy(&fib_mutex);
    device_destroy(fib_class, fib_dev);
    class_destroy(fib_class);
    unregister_chrdev(major, DEV_FIBONACCI_NAME);
}

module_init(init_fib_dev);
module_exit(exit_fib_dev);
