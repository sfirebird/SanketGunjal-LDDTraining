## Overview: These are my experiments and exploration while writing the device drivers and communicating with kernel.

#### Q. What is the behaviour of the name of the device driver? For example, the name of the .c device driver file is kv_store.c after you compile this you will get a file called, kv_store.ko which is a kernel object file and when you insert it using the insmod command, you are inserting the .ko file but later on if you change the name? what happens then?
- It is possible to assign a different name to the special character file but the device driver kv_store name would remain same just the file name would be different.
