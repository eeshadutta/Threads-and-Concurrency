# PROBLEM 1: BADMINTON ACADEMY PROBLEM

#### Running the code
`gcc 1.c -pthread`
`./a.out`
Then enter the number of refrees.

#### Structs
1) Person
Holds the state of the person, ie whether he has entered court, or meeting the organiser, or warming up etc

2) Organizer
Holds information of the organizer like the number of players the organiser has met, number of refrees he has met, whether he is allowing anyone to enter the court and how many people he has allowed to enter the court.

3) Args
Used for passing information about a person into the various functions like the person's id and whether he is a player or refree.

#### Threads
There is a person thread which is used for both players and refrees.

#### Functions 
1) void *enterAcademy(void * args)
The threads call this function wen thy are created. It is used by the players and refrees to enter the academy.

2) void meetOrganizer(Args *args)
Player/Refree has entered the court and he now meets the organizer.

3) void enterCourt(Args *arg)
Player/Refree has met the organizer. This function waits for a team to be formed and then the team of 2 players and a refree enter the court.

4) void warmUp(Args *arg)
Once the player has entered the court, he warms up.

5) void adjustEquipment(Args *arg)
After entering the court, refree starts adjusting the equipment.

6) void startGame(Args *arg)
Once the refree has finished adjusting the equipment and players have warmed up, the refree starts the game. Once the game has started, the organizer is free to meet other players and refrees.

# PROBLEM 2: The Queue at the Polling Booth

#### Running the code
`gcc 2.c -pthread`
`./a.out`
Then enter the number of booths n
In the next n lines, enter the number of voters and the number of evms in the ith booth

#### Structs
1) Booth
- id
- number of voters
- number of evms

2) EVM
- id
- booth_id

3) Voter
- id
- booth_id

#### Threads
1) Booth Thread for every booth
2) EVM thread for every EVM of every booth
3) Voter thread for every Voter of every booth

#### Functions
1) void *booth_function(void *args)
Booth thread calls this function when it is initialised. It creates the EVM and voter threads of the given booth.

2) void *voter_function(void *args)
Calls voter_wait_for_evm and once it has been allocated to an EVM, calls voter_in_slot to let the EVM know that it has reached its place.

3) void *evm_function(void *args)
Creates random slots for an EVM. Calls polling_ready_evm to allocate these slots to voters. After allocation, moves to voting phase and completes voting.

4) void polling_ready_evm(Booth *booth, int count, int evm_id)
Used for allocating slots too voters. Returns only when the slots are allocated or when all voters in the booth have already voted.

5) void voter_wait_for_evm(Booth *booth, int voter_id)
This function is called when a voter first arrives. This function returns only when a free EVM is available and there are enough free slots to allocate to the voter

6) void voter_in_slot(Booth *booth)
Once the voter enters the slot, this function is called to let the EVM know that the voter has reached the slot.

7) int check(int booth_id)
Checks if there are any waiting voters in a given booth.

# PROBLEM 3: CONCURRENT MERGESORT

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