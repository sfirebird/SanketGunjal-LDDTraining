#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME         "kv_store"
#define BUFFER_SIZE         1024

#define HASH_CAP            10
#define HASH_LL             -1
#define HASH_HL             10

static int major_number;
static int dev_open;
static char device_buffer[1024];

/*  Create a structre which contains the key-value pairs    */
struct __key_value_organized{
    unsigned long long int __key;
    char *__value;
};

typedef struct __key_value_organized __kvg;

static int TOP = -1;
__kvg *hash = NULL;

/*  Custom file operation function signatures to be mapped with fops */
static int device_open(struct inode *inode, struct file *file);
static int device_close(struct inode *inode, struct file *file);
static ssize_t device_read(struct file *file, char __user *user_buffer, size_t size, loff_t *offset);
static ssize_t device_write(struct file *file, const char __user *user_buffer, size_t size, loff_t *offset);

/*  Custom implementation of given tasks    */
static void get_kv(void);
static int attoi_user(const volatile char **pBuffer);

/*  Register custom file operation function */
static struct file_operations fops = {     
    .open = device_open,
    .release = device_close,
    .read = device_read,                                                                                            
    .write = device_write,
};

static int device_open(struct inode *inode, struct file *file){
    dev_open++;
    printk(KERN_INFO "%s: opened %d times\n", (char *)DEVICE_NAME, dev_open);
    return 0;
}

static int device_close(struct inode *inode, struct file *file){
    printk(KERN_INFO "%s: closed!\n", (char *)DEVICE_NAME);
    return 0;
}

static ssize_t device_read(struct file *file, char __user *user_buffer, size_t size, loff_t *offset){
    /*  get the length of the string    */
    //ssize_t len = strlen(device_buffer);
    ssize_t len = sizeof(struct kvNode);


    if(*offset >= len)
        return 0;

    if(size > len - *offset)
        size = len - *offset;

    if (copy_to_user(user_buffer, hash + *offset, size)) {
        return -EFAULT;    
    }

    *offset += size;
    printk(KERN_INFO "%s: Sent %zu bytes to the user\n", (char *)DEVICE_NAME, size);

    return size;
}

static ssize_t device_write(struct file *file, const char __user *user_buffer, size_t size, loff_t *offset){
    if (size > BUFFER_SIZE - 1) // Limit size to buffer capacity
        size = BUFFER_SIZE - 1;

    if (copy_from_user(device_buffer, user_buffer, size)) {
        return -EFAULT;
    }

    device_buffer[size] = '\0'; // Null-terminate the string
    printk(KERN_INFO "%s: Received %zu bytes from the user\n", (char *)DEVICE_NAME, size);
    get_kv();
    return size;
}

/* Responsible for extracting the key:value from the buffer */
static void get_kv(void){
    char *buff = device_buffer;

    /* key-value is received in the format = "key:value" */
    /* Extract the key */
    int kv = attoi_user((const volatile char **)&buff);
    if(kv == -1)
        printk(KERN_ERR "%s: NULL buffer", (char *)DEVICE_NAME);
    
    /* store the key in buffer */
    TOP++;
    if(TOP == HASH_HL){
        printk(KERN_ERR "%s: Buffer full, delete data!", (char *)DEVICE_NAME);
    }
    hash[TOP].key = kv;

    /* extract the value */
    buff++;
    kv = attoi_user((const volatile char **)&buff);
    if(kv == -1)
        printk(KERN_ERR "%s: NULL buffer", (char *)DEVICE_NAME);

    printk(KERN_INFO "kv = %d", kv);

    /* store the value for the key in buffer */
    if(TOP == HASH_HL){
        printk(KERN_ERR "%s: Buffer full, delete data!", (char *)DEVICE_NAME);
    }
    hash[TOP].value = kv;

    /* print to the dmesg for debugging purpose */
    printk(KERN_INFO "%s: key-value:%d:%d", (char *)DEVICE_NAME, hash[TOP].key, hash[TOP].value);

}

#ifdef PLUGGED
/* Converts the first set of characters of ascii value 48 to 57 to integers 
PROBLEM: Not able to understand the problem for converting string decimal ASCII
into integers is failing for more than 2 decimal characters.
Ex: String to convert: "2332"
Expected int value: 2332,
instead the return output is 2303002.

I have tried the same code or function and executed it from the user space application,
where it worked fine and gave the expected value.
----------------------------------------------------------------------------------------
*/
static int attoi_user(const char **pBuffer){
    if(pBuffer == NULL){
        return -1;
    }

    const int mul = 10;
    int parsed_val = 0;

    while(**pBuffer >= 48 && **pBuffer <= 57){
        /*  Since this function will be only responsible for parsing the digits */
        parsed_val = (parsed_val * mul) + (**pBuffer - '0');
        (*pBuffer)++;
    }
    return parsed_val;
}
#endif

static int __init kv_store_init(void){

    /*  Dynamically allocate the memory for the hash    */
    hash = (__kvg *)kzalloc(sizeof(__kvg) * HASH_CAP, GFP_KERNEL);
    if(hash == NULL){
        printk(KERN_ALERT "%s: Failed to allocate memory using kzalloc()!\n", (char *)DEVICE_NAME);
    }

    /*  Allocate dynamic memory for the char *value member  */
    int loop_start;
    for(loop_start = 0; loop_start < HASH_CAP; loop_start++){
        hash[loop_start]->__value = (char *)kzalloc(sizeof(char) * VALUE_LENGTH);
        //hash->__value = (char *)kzalloc(sizeof(char) * VALUE_LENGTH);
    }

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "kv_store: Failed to register device\n");
        return major_number;
    }
    printk(KERN_INFO "%s: Registered with major number %d\n", (char *)DEVICE_NAME, major_number);
    return 0;
}

static void __exit kv_store_exit(void){
    if(hash != NULL){
        /*  Free the memory for the __value member  */
        int loop_start;
        for(loop_start = 0; loop_start < HASH_CAP; loop_start++){
            kfree(hash[loop_start]->__value);
        }
    }
    
    if(hash != NULL){
        kfree(hash);
    }
    
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "%s: Unregistered device\n", (char *)DEVICE_NAME);
}

module_init(kv_store_init);
module_exit(kv_store_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sanketgunjal@mirafra.com");
MODULE_DESCRIPTION("Driver for storing and parsing keyvalue pairs!");
