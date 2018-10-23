#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#define MAXN 1000

typedef struct Booth
{
    int id;
    int num_evms;
    int num_voters;
} Booth;

typedef struct Voter
{
    int booth_id;
    int id;
} Voter;

typedef struct EVM
{
    int booth_id;
    int id;
} EVM;

int voters[MAXN][MAXN];
Booth *booths[MAXN];

pthread_t booth_threads[MAXN];
pthread_t evm_threads[MAXN][MAXN];
pthread_t voter_threads[MAXN][MAXN];
pthread_mutex_t booth_mutex[MAXN];

void *booth_function(void *args);
void *voter_function(void *args);
void *evm_function(void *args);
void polling_ready_evm(Booth *booth, int count, int evm_id);
void voter_wait_for_evm(Booth *booth, int voter_id);
void voter_in_slot(Booth *booth);
int check(int booth_id);

int main()
{
    int num_booths;
    scanf("%d", &num_booths);
    int i, j;
    for (i = 1; i <= num_booths; i++)
        pthread_mutex_init(&booth_mutex[i], NULL);
    for (i = 1; i <= num_booths; i++)
    {
        int n_evms, n_voters;
        scanf("%d %d", &n_voters, &n_evms);
        booths[i] = (Booth *)malloc(sizeof(Booth));
        booths[i]->id = i;
        booths[i]->num_evms = n_evms;
        booths[i]->num_voters = n_voters;
    }
    printf("\x1b[31m"
           "ELECTION STARTED"
           "\x1b[0m"
           "\n");
    for (i = 1; i <= num_booths; i++)
        pthread_create(&(booth_threads[i]), NULL, booth_function, booths[i]);

    for (i = 1; i <= num_booths; i++)
        pthread_join(booth_threads[i], NULL);
    printf("\x1b[31m"
           "ELECTION COMPLETED SUCCESSFULLY"
           "\x1b[0m"
           "\n");
}

void *booth_function(void *args)
{
    Booth *booth = (Booth *)args;
    int i;
    for (i = 1; i <= booth->num_voters; i++)
    {
        Voter *temp = (Voter *)malloc(sizeof(Voter));
        temp->booth_id = booth->id;
        temp->id = i;
        pthread_create(&(voter_threads[booth->id][i]), NULL, voter_function, temp);
    }
    for (i = 1; i <= booth->num_evms; i++)
    {
        EVM *temp = (EVM *)malloc(sizeof(EVM));
        temp->booth_id = booth->id;
        temp->id = i;
        pthread_create(&(evm_threads[booth->id][i]), NULL, evm_function, temp);
    }
    for (i = 1; i <= booth->num_voters; i++)
        pthread_join(voter_threads[booth->id][i], NULL);
    for (i = 1; i <= booth->num_evms; i++)
        pthread_join(evm_threads[booth->id][i], NULL);

    printf("\x1b[32m"
           "Voters at Booth %d are done with voting\n"
           "\x1b[0m",
           booth->id);
}

void *voter_function(void *args)
{
    Voter *voter = (Voter *)args;
    Booth *booth = (Booth *)malloc(sizeof(Booth));
    booth = booths[voter->booth_id];
    voter_wait_for_evm(booth, voter->id);
    booth = booths[voter->booth_id];
    voter_in_slot(booth);
}

void *evm_function(void *args)
{
    EVM *evm = (EVM *)args;
    while (check(evm->booth_id))
    {
        int slots = rand() % 10 + 1;
        Booth *temp = booths[evm->booth_id];
        printf("EVM %d of Booth %d is free with slots = %d\n", evm->id, evm->booth_id, slots);
        polling_ready_evm(temp, slots, evm->id);
        printf("EVM %d of Booth %d is moving for voting phase\n", evm->id, evm->booth_id);
        printf("\x1b[34m"
               "EVM %d of Booth %d finished voting phase\n"
               "\x1b[0m",
               evm->id, evm->booth_id);
    }
}

void voter_wait_for_evm(Booth *booth, int voter_id)
{
    while (voters[booth->id][voter_id] == 0)
        ;
}

void voter_in_slot(Booth *booth)
{
    // voter votes
}

void polling_ready_evm(Booth *booth, int count, int evm_id)
{
    int i, j;
    int t = count;
    for (i = 1; i <= booth->num_voters; i++)
    {
        if (voters[booth->id][i] == 0)
        {
            pthread_mutex_lock(&booth_mutex[booth->id]);
            voters[booth->id][i] = 1;
            pthread_mutex_unlock(&booth_mutex[booth->id]);
            t--;
            printf("Voter %d of Booth %d is allocated EVM %d\n", i, booth->id, evm_id);
        }
        if (t == 0)
            break;
    }
    sleep(1);
}

int check(int booth_id)
{
    int i;
    for (i = 1; i <= booths[booth_id]->num_voters; i++)
    {
        if (voters[booth_id][i] == 0)
        {
            return 1;
        }
    }
    return 0;
}
