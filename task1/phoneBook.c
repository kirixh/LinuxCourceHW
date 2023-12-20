#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/idr.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/syscalls.h>
#include "phoneBook.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kirill Piatkovskyi");
MODULE_DESCRIPTION("");
MODULE_VERSION("0.0");

// static size_t num_users = 0;
static struct idr phonebook;
static char kernelBuffer[BUFFER_SIZE];

/////////////////////////////////////////////////////////////////////////////////
// MODULE realization
/*-----------------------------------------------------------------------------*/
struct found_users_data* get_user_by_surname(const char *surname) {
    struct found_users_data* found_users_data = kmalloc(sizeof(struct found_users_data), GFP_KERNEL);
    struct user_data* users_data = kmalloc(sizeof(struct user_data) * REPETATIVE_NUMBER, GFP_KERNEL);
    int idx = MAX_USERS;
    size_t found_num = 0;

    while (idx >= 0) {
        struct user_data* user = idr_find(&phonebook, idx);
        if (user != NULL && found_num <= REPETATIVE_NUMBER && strcmp(user->surname, surname) == 0) {
            memcpy(&users_data[found_num], user, sizeof(struct user_data));
            ++found_num;
        }
        --idx;
    }
    found_users_data->found_num = found_num;
    found_users_data->users_data = users_data;
    return found_users_data;
}

/*-----------------------------------------------------------------------------*/
long add_user(struct user_data *user) {
    struct user_data *new_user = kmalloc(sizeof(struct user_data), GFP_KERNEL);
    memcpy(new_user, user, sizeof(struct user_data));
    idr_alloc(&phonebook, new_user, 0, MAX_USERS, GFP_KERNEL);
    return 0;
}

/*-----------------------------------------------------------------------------*/
long remove_user(const char *surname) {
    int idx = MAX_USERS;

    while (idx >= 0) {
        struct user_data* user = idr_find(&phonebook, idx);
        if (user != NULL && strcmp(user->surname, surname) == 0) {
            kfree(user);
            idr_remove(&phonebook, idx);
            return 0;
        }
        --idx;
    }
    return -1;
}

/////////////////////////////////////////////////////////////////////////////////
// PARSE COMMANDS

void parseCommand(void) {
    struct user_data user;
    char surname[NAME_SIZE];
    if (strncmp(kernelBuffer, "adduser", 7) == 0) {
        sscanf(kernelBuffer + 8, "%s%s%zu%s%s",
               user.name, user.surname, &user.age, user.number, user.email);
        
        if (add_user(&user) == 0) {
            snprintf(kernelBuffer, BUFFER_SIZE, "User added successfully\n\n");
        } else {
            snprintf(kernelBuffer, BUFFER_SIZE, "Failed to add user with: \
            \n Name: %s\nSurname: %s\nAge: %zu\nPhone number: %s\nEmail: %s\n\n",
            user.name, user.surname, user.age, user.number, user.email);
        }
    } else if (strncmp(kernelBuffer, "checkuser", 9) == 0) {
        sscanf(kernelBuffer + 10, "%s", surname);
        struct found_users_data* found_users = get_user_by_surname(surname);
        if (found_users && found_users->users_data && found_users->found_num > 0) {
            for (size_t cnt = found_users->found_num; cnt > 0; --cnt) {
                //pr_info("cnt = %d\n", cnt);
                user = found_users->users_data[cnt - 1];
                snprintf(kernelBuffer, BUFFER_SIZE, "Name: %s\nSurname: %s\nAge: %zu\nPhone number: %s\nEmail: %s\n\n",
                     user.name, user.surname, user.age, user.number, user.email);
            }
                
        } else {
            snprintf(kernelBuffer, BUFFER_SIZE, "No such user in phonebook\n\n");
        }
        
        kfree(found_users->users_data);
        kfree(found_users);
    } else if (strncmp(kernelBuffer, "rmuser", 6) == 0) {
        sscanf(kernelBuffer + 7, "%s", surname);
        if (remove_user(surname) == 0) {
            snprintf(kernelBuffer, BUFFER_SIZE, "Removed successfully\n\n");
        } else {
            snprintf(kernelBuffer, BUFFER_SIZE, "Failed to remove\n\n");
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////
// SYSCALL DEFINES

SYSCALL_DEFINE3(get_user, const char*, surname, unsigned int, len, struct user_data *, output_data)
{
    char* kernelSurname = kmalloc(sizeof(char) * len, GFP_KERNEL);
    struct user_data user;
    
    int err = copy_from_user(kernelSurname, (const char*) surname, sizeof(char) * len);
    if (err) {
        pr_alert("Phonebook syscall_get_user copy_from_user failed with %d\n", err);
        kfree(kernelSurname);
        return err;
    }
    struct found_users_data* found_users = get_user_by_surname(kernelSurname);
    if (found_users && found_users->users_data && found_users->found_num > 0) {
        user = found_users->users_data[found_users->found_num - 1];
    }

    err = copy_to_user((struct user_data *)output_data, &user, sizeof(struct user_data));
    if (err) {
        pr_alert("Phonebook syscall_get_user copy_to_user failed with %d\n", err);
        kfree(found_users->users_data);
        kfree(found_users);
        kfree(kernelSurname);
        return err;
    }

    kfree(found_users->users_data);
    kfree(found_users);
    kfree(kernelSurname);
    return 0;
}

SYSCALL_DEFINE1(add_user, struct user_data*, intput_data)
{
    struct user_data kernel_user;
    int err = copy_from_user(&kernel_user, (struct user_data*) intput_data, sizeof(struct user_data));
    if (err) {
        pr_alert("Phonebook syscall_add_user copy_from_user failed with %d\n", err);
        return err;
    }

    return add_user(&kernel_user);
}

SYSCALL_DEFINE2(del_user, const char*, surname, unsigned int, len)
{
    char* kernelSurname = kmalloc(sizeof(char) * len, GFP_KERNEL);

    int err = copy_from_user(kernelSurname, (const char*) surname, sizeof(char) * len);
    if (err) {
        pr_alert("Phonebook syscall_get_user copy_from_user failed with %d\n", err);
        kfree(kernelSurname);
        return err;
    }
    
    int res = remove_user(kernelSurname);
    kfree(kernelSurname);
    return res;
}

/////////////////////////////////////////////////////////////////////////////////
// CHAR DEVICE INIT

struct char_device {
    struct cdev cdevice;
    struct class *cls;
    int major;
};

static struct char_device device;

static int phoneBookDevice_open(struct inode *inode, struct file *file);
static int phoneBookDevice_release(struct inode *inode, struct file *file);
static ssize_t phoneBookDevice_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos);
static ssize_t phoneBookDevice_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos);

static struct file_operations phonebook_fops = {
    .open = phoneBookDevice_open,
    .release = phoneBookDevice_release,
    .write = phoneBookDevice_write,
    .read = phoneBookDevice_read
};

static int phoneBookDevice_open(struct inode *inode, struct file *file) {
    pr_info("Opening device %s\n", DEVICE_NAME);
    try_module_get(THIS_MODULE);
    return 0;
}

static int phoneBookDevice_release(struct inode *inode, struct file *file) {
    pr_info("Closing device %s\n", DEVICE_NAME);
    module_put(THIS_MODULE);
    return 0;
}

static ssize_t phoneBookDevice_write(struct file *file, const char __user *buf, size_t length, loff_t *offset) {
    pr_info("Writing in device %s\n", DEVICE_NAME);
    if (BUFFER_SIZE - *offset < length) {
        length = BUFFER_SIZE - *offset;
    }
    int err = copy_from_user(kernelBuffer + *offset, buf, length);
    if (err) {
        pr_alert("Phonebook device write copy_from_user failed with %d\n", err);
        return err;
    }
    *offset += length;
    kernelBuffer[*offset] = '\0';
    parseCommand();
    return length;
}

static ssize_t phoneBookDevice_read(struct file *file, char __user *buf, size_t length, loff_t *offset) {
    pr_info("Read device %s\n\n", DEVICE_NAME);
    size_t data_len = strlen(kernelBuffer);
    if (!*(kernelBuffer + *offset)) { 
        *offset = 0; 
        return 0;
    }

    if (data_len - *offset < length) {
        length = data_len - *offset;
    }
    int err = copy_to_user(buf, kernelBuffer + *offset, length);
    if (err) {
        pr_alert("Phonebook device read copy_to_user failed with %d\n", err);
        return err;
    }
    *offset += length;
    return length;
}

/*-----------------------------------------------------------------------------*/
static int __init phonebook_module_added(void) {
    idr_init(&phonebook);
    device.major = MKDEV(222, 0);

    int err = register_chrdev_region(device.major, 1, DEVICE_NAME);
    if (err < 0) {
        pr_alert("Registering chrdev_region failed with %d\n", err);
        return err;
    }

    device.cls = class_create(DEVICE_NAME);
    if (device.cls == NULL) {
        pr_alert("Class_create failed!\n");
        unregister_chrdev_region(device.major, 1);
        return -1;
    }
    device_create(device.cls, NULL, device.major, NULL, DEVICE_NAME);
    pr_info("Device created on /dev/%s\n", DEVICE_NAME);
    
    cdev_init(&device.cdevice, &phonebook_fops);
    err = cdev_add(&device.cdevice, device.major, 1);
    if (err < 0) {
        pr_alert("Registering char device into system failed with %d\n", err);
        
        cdev_del(&device.cdevice);
        device_destroy(device.cls, device.major);
        unregister_chrdev_region(device.major, 1);

        return err;
    }

    pr_info("Module PHONEBOOK has been installed\n");
    return 0;
} 

/*-----------------------------------------------------------------------------*/
static void __exit phonebook_module_removed(void)
{
    for (size_t idx = MAX_USERS; idx > 0; --idx) {
        idr_remove(&phonebook, idx);
    }

    pr_info("Leaving %s\n", DEVICE_NAME);
    device_destroy(device.cls, device.major);
    cdev_del(&device.cdevice);
    if (device.cls)
        class_destroy(device.cls);
    unregister_chrdev_region(device.major, 1);

    pr_info("module PHONEBOOK has been uninstalled\n");
}

module_init(phonebook_module_added)
module_exit(phonebook_module_removed)