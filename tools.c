#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("mind, muscle, and the other thing");
MODULE_DESCRIPTION("Fibonacci driver");
MODULE_VERSION("0.1");

static dev_t fib_dev = 0;
static struct cdev *fib_cdev;
static struct class *fib_class;
static DEFINE_MUTEX(fib_mutex);

static long long fib_sequence(long long k) {
    long long f[k + 2];

    for (int i = 0; i <= k; i++)
        if (i < 2)
            f[i] = i;
        else
            f[i] = f[i - 1] + f[i - 2];

    return f[k];
}

static int fib_open(struct inode *inode, struct file *file) {
    if (!mutex_trylock(&fib_mutex)) {
        printk(KERN_ALERT
        "driver is being held busy by another program");
        return -EBUSY;
    }
    return 0;
}

static ssize_t fib_read(struct file *file, char *buf, size_t size, loff_t *offset) {
    return (ssize_t) fib_sequence(*offset);
}

static ssize_t fib_write(struct file *file, const char *buf, size_t size, loff_t *offset) {
    return 1;
}

static int fib_release(struct inode *inode, struct file *file) {
    mutex_unlock(&fib_mutex);
    return 0;
}

static loff_t fib_device_lseek(struct file *file, loff_t offset, int orig) {
    loff_t new_pos = 0;
    switch (orig) {
        case 0:
            new_pos = offset;
            break;
        case 1:
            new_pos = file->f_pos + offset;
            break;
        case 2:
            new_pos = 92 - offset;
            break;
        default:
            break;
    }

    if (new_pos < 0)
        new_pos = 0;
    else if (new_pos > 92)
        new_pos = 92;

    file->f_pos = new_pos;
    return new_pos;
}

const struct file_operations fib_fops = {
        .owner = THIS_MODULE,
        .open = fib_open,
        .read = fib_read,
        .write = fib_write,
        .release = fib_release,
        .llseek = fib_device_lseek,
};

static int __init

initFunc(void) {

    mutex_init(&fib_mutex);
    int rc = alloc_chrdev_region(&fib_dev, 0, 1, "fibonacci");

    if (rc < 0) {
        printk(KERN_ALERT
        "fibonacci char device cannot be registered. rc = %i", rc);
        return rc;
    }

    fib_cdev = cdev_alloc();
    if (fib_cdev == NULL) {
        printk(KERN_ALERT
        "allocating fib_cdev failed due to NULL");
        rc = -1;
        unregister_chrdev_region(fib_dev, 1);
        return rc;
    }

    cdev_init(fib_cdev, &fib_fops);
    rc = cdev_add(fib_cdev, fib_dev, 1);

    if (rc < 0) {
        printk(KERN_ALERT
        "cdev cannot be added to, due to negative rc");
        rc = -2;
        unregister_chrdev_region(fib_dev, 1);
        return rc;
    }

    fib_class = class_create(THIS_MODULE, "fibonacci");

    if (!fib_class) {
        printk(KERN_ALERT
        "device class cannot be created");
        rc = -3;
        cdev_del(fib_cdev);
        return rc;
    }
    if (!device_create(fib_class, NULL, fib_dev, NULL, "fibonacci")) {
        printk(KERN_ALERT
        "device cannot be created");
        rc = -4;
        class_destroy(fib_class);
        return rc;
    }
    return rc;
}

static void __exit

exitFunc(void) {
    mutex_destroy(&fib_mutex);
    device_destroy(fib_class, fib_dev);
    class_destroy(fib_class);
    cdev_del(fib_cdev);
    unregister_chrdev_region(fib_dev, 1);
}

module_init(initFunc);
module_exit(exitFunc);
