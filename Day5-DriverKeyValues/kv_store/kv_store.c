#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

#define DEVICE_NAME         "kv_store"
#define VALUE_LENGTH        128

/*  __kvg storage limits    
*   Hash capacity, lower limit, higher limit    */
#define HASH_CAP            10
#define HASH_LL             -1
#define HASH_HL             10

#define DEVICE_NUMBERS      1

/*  Create a structre which contains the key-value pairs    */
struct __key_value_organized{
    unsigned long long int __key;
    char __value[VALUE_LENGTH];
};

typedef struct __key_value_organized __kvg;

/* Keeps track of the number of times the device was opened */
static int dev_open;

/*  Buffer, soon to be deprecated  */
//static char device_buffer[1024];

#define BUFFER_SIZE         (sizeof(__kvg) + VALUE_LENGTH)
//#define BUFFER_SIZE             sizeof(__kvg)
static int TOP = -1;

/*  Create a global pointer variable of type __kvg  */
__kvg *hash = NULL;

/*  Create a cdev strucutre */
static struct cdev kv_store_device;

/*  for the device numbers    */
static dev_t kv_store_number;

/*  Custom file operation function signatures to be mapped with fops */
static int device_open(struct inode *inode, struct file *file);
static int device_close(struct inode *inode, struct file *file);
static ssize_t device_read(struct file *file, char __user *user_buffer, size_t size, loff_t *offset);
static ssize_t device_write(struct file *file, const char __user *user_buffer, size_t size, loff_t *offset);

/*  Custom implementation of given tasks    */
#ifdef PLUGGED
static int attoi_user(const volatile char **pBuffer);
#endif
static int get_kvg_status(void);

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
    ssize_t len = sizeof(__kvg) + strlen(hash->__value);

    if(*offset >= len)
        return 0;

    if(size > len - *offset)
        size = len - *offset;

    if (copy_to_user(user_buffer, ((hash + TOP) + *offset), size)) {
        return -EFAULT;    
    }

    *offset += size;
    printk(KERN_INFO "%s: Sent %zu bytes to the user\n", (char *)DEVICE_NAME, size);

    return size;
}

static ssize_t device_write(struct file *file, const char __user *user_buffer, size_t size, loff_t *offset){
    if (!hash) {
        pr_err("%s: hash is not allocated\n", DEVICE_NAME);
        return -ENOMEM;
    }
    
    TOP++;
    /* key-value is received in the format __kvg 
       Check the top for available capacity in hash
    */  
    if(TOP >= HASH_HL){
        pr_err("%s: Buffer full, delete data!", DEVICE_NAME);
        TOP--; // Revert increment
        return -EFAULT;
    }

    if (copy_from_user((void *)(hash + TOP), user_buffer, size)) {
        TOP--;
        return -EFAULT;
    }

    printk(KERN_INFO "%s: Received %zu bytes from the user\n", (char *)DEVICE_NAME, size);

    /* print to the dmesg for debugging purpose */
    printk(KERN_INFO "%s: key-value=%llu:%s", (char *)DEVICE_NAME, hash[TOP].__key, hash[TOP].__value);
    
    pr_info("%s: sizeof hash = %ld\n", DEVICE_NAME, sizeof(hash));

    return size;
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

static void dynamic_clean_mem(void){
    if(hash){
        /*  Free the memory for the __value member  
        int loop_start;
        for(loop_start = 0; loop_start < HASH_CAP; loop_start++){
            kfree(hash[loop_start].__value);
        }
        */
        kfree(hash);
    }
}

static int __init kv_store_init(void){

    /*  Error checking variable */
    int xRet = -1;

    if((xRet = alloc_chrdev_region(&kv_store_number, 0, DEVICE_NUMBERS, DEVICE_NAME))){
        // handle error here.
        pr_info("%s: Failed to allocate device number to the driver with error code = %d\n", DEVICE_NAME, xRet);
        return xRet;
    }

    printk(KERN_INFO "%s: Registered with major number %d\n", (char *)DEVICE_NAME, MAJOR(kv_store_number));

    /*
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "kv_store: Failed to register device\n");
        return major_number;
    }
    */
   
   /*   Init the cdev type structure and add it to the kernel space */
   cdev_init(&kv_store_device, &fops);
   if((xRet = cdev_add(&kv_store_device, kv_store_number, DEVICE_NUMBERS))){
        //handle error!
        pr_info("%s: Failed to add the structure to the kernel with error code = %d\n", DEVICE_NAME, xRet);
        unregister_chrdev_region(kv_store_number, DEVICE_NUMBERS);
        return xRet;
   }

    /*  Dynamically allocate the memory for the hash    */
    hash = kzalloc(sizeof(__kvg) * HASH_CAP, GFP_KERNEL);
    if(!hash){
        printk(KERN_ALERT "%s: Failed to allocate memory using kzalloc()!\n", (char *)DEVICE_NAME);
        cdev_del(&kv_store_device);
        unregister_chrdev_region(kv_store_number, DEVICE_NUMBERS);
        return xRet;
    }

    /*  Allocate dynamic memory for the char *value member  
    int loop_start;
    for(loop_start = 0; loop_start < HASH_CAP; loop_start++){
        hash[loop_start].__value = kzalloc(VALUE_LENGTH, GFP_KERNEL);
        if(!hash[loop_start].__value){
            printk(KERN_ALERT "%s: Failed to allocate memory for __value at index %d!\n", DEVICE_NAME, loop_start);

            // Free already allocated entries to prevent memory leaks
            while (--loop_start >= 0) {
                kfree(hash[loop_start].__value);
            }
            kfree(hash);
            cdev_del(&kv_store_device);
            unregister_chrdev_region(kv_store_number, DEVICE_NUMBERS);
            return -ENOMEM;
        
        }

    }
    */

    pr_info("%s: successfully loaded!\n", DEVICE_NAME);

   return 0;
}

static void __exit kv_store_exit(void){
    dynamic_clean_mem();
    cdev_del(&kv_store_device);
    unregister_chrdev_region(kv_store_number, DEVICE_NUMBERS);
    printk(KERN_INFO "%s: Unregistered device\n", (char *)DEVICE_NAME);
}

/*  Initialization and clean up */
module_init(kv_store_init);
module_exit(kv_store_exit);

/*  Driver Information  */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("sanketgunjal@mirafra.com");
MODULE_DESCRIPTION("Driver for storing and parsing keyvalue pairs!");
