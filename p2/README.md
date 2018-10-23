# The Queue at the Polling Booth

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