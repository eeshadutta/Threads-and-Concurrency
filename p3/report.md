**Input.txt contains 10^4 numbers which are in random order**

1. **Normal Mergesort**

Its the normal way of coding mergesort.

`gcc mergesort_normal.c `
`time ./a.out < input.txt > output_1.txt`

2. **Mergesort using child processes**

It uses **shmget**  (for shared memory allocation) and **shmat** (for shared memory operations) functions. We create a shared memory space between the child process that we fork.  Each segment is split into left and right child which is sorted, and each part works concurrently. The shmget()  requests the kernel to allocate a shared page for both the processes. shmat() attaches the System V shared memory segment identified by **shmid** to the address space of the calling process. 

`gcc 3_process.c`
`time ./a.out < input.txt > output_2.txt`

**Why we need shared memory?**

The traditional fork does not work because the child process and the parent process run in separate memory spaces and memory writes performed by one of the processes do not affect the other. Hence we need a shared memory segment.

3. **Mergesort using threads**

It uses threads and  mutex lock.
There is one thread for every left and right child.
Lock is used when we are writing in array to prevent race condition, basically when we call merge().

`gcc  3_threads.c -lpthread`
`time ./a.out < input.txt > output_3.txt`

**Time Analysis :**

| Cases                         | Real     | User     | Sys      |
| ----------------------------- | -------- | -------- | -------- |
| 3_normal.c                    | 0m0.009s | 0m0.005s | 0m0.004s |
| 3_process.c                   | 0m0.380s | 0m2.492s | 0m0.329s |
| 3_thread.c                    | 0m0.231s | 0m0.035s | 0m0.195s |

**Conclusion**

The **2nd case** takes more time than the **3rd case** which takes more time than the **1st case**. In the **2nd case**, as child processes are created, the entire memory space and registers of the parent process have to be duplicated. It also invloves the overhead of multiple context switches and this  outweighs the optimization achieved by concurrency. In the **3rd case** multiple threads are created which do limited work but invlove the overhead of thread creation. Thus this is also poorer than sequential merge sort. The **2nd case** however is the poorest in terms of performance since everytime fork() is called, eveything related to the parent process has to be suplicated for the child and the overhead of the context switches is more than thread creation.


