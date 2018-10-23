#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct Person
{
    int state;
    // 0 : waiting for organizer
    // 1 : with organizer
    // 2 : warm up for player, adjusting equipment for refree
    // 3 : start game
} Person;

int player = 0;
int refree = 1;

typedef struct Args
{
    int person_type;
    int person_id;
} Args;

typedef struct Organiser
{
    int players;
    int refree;
    int allowed; // 0 if not allowed and 1 if allowed
    int entered; // 1, 2, 3 people entering
} Organiser;

typedef struct Thread
{
    pthread_t thread;
    int person_type;
} Thread;

int players_remaining;
int refrees_remaining;
int players_done;
int refrees_done;
int num_args;
int players_warmedup;
int players_ready_to_enter;
int refree_ready_to_enter;
int progress = 0;
Person players[1000000];
Person refrees[1000000];
Args args[10000000];
Organiser organiser;
Thread threads[1000000];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *enterAcademy(void *args);
void meetOrganizer(Args *args);
void enterCourt(Args *arg);
void warmUp(Args *arg);
void adjustEquipment(Args *arg);
void startGame(Args *arg);

int main()
{
    int n;
    scanf("%d", &n);
    players_remaining = 2 * n;
    refrees_remaining = n;
    players_done = 0;
    refrees_done = 0;
    num_args = 0;
    organiser.players = 0;
    organiser.refree = 0;
    organiser.allowed = 0;
    organiser.entered = 0;
    players_warmedup = 0;
    players_ready_to_enter = 0;
    refree_ready_to_enter = 0;
    int s = 1;
    int time_over = 0;
    int time_gen = rand() % 3;

    int wait;

    while (1)
    {
        if (time_over == time_gen && (players_remaining > 0 || refrees_remaining > 0))
        {
            s = rand() % 3;
            time_gen = time_over + s;
            int num = rand() % (players_remaining + refrees_remaining);
            if (num < players_remaining)
            {
                // player
                players_remaining--;
                players[players_done].state = 0;
                args[num_args].person_type = player;
                args[num_args].person_id = players_done;
                players_done++;
            }
            else
            {
                // refree
                refrees_remaining--;
                refrees[refrees_done].state = 0;
                args[num_args].person_type = refree;
                args[num_args].person_id = refrees_done;
                refrees_done++;
            }
            threads[players_done + refrees_done].person_type = args[num_args].person_type;
            if (pthread_create(&(threads[players_done + refrees_done].thread), NULL, enterAcademy, &args[num_args]))
            {
                perror("thread creation failed\n");
                exit(1);
            }
            num_args++;
        }

        if (s)
        {
            sleep(1);
            time_over++;
        }

        if (players_remaining == 0 && refrees_remaining == 0)
        {
            int i, flag = 0;
            for (i = 0; i < 2 * n; i++)
            {
                if (players[i].state != 3)
                {
                    flag = 1;
                    break;
                }
            }
            for (i = 0; i < n; i++)
            {
                if (refrees[i].state != 3)
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 0)
            {
                for (i = 0; i < 3 * n; i++)
                {
                    pthread_join(threads[i].thread, NULL);
                }
                return 0;
            }
        }

        wait = rand() % 3;
        sleep(wait);
    }
    return 0;
}

void *enterAcademy(void *arg)
{
    Args *args = (Args *)arg;
    if (args->person_type == player)
    {
        printf("Player %d entered academy\n", args->person_id + 1);
        // player meets organiser only if 2 players ahead of him have met the organiser
        while (!(args->person_id < 2 || players[args->person_id - 2].state == 3))
            ;
        players[args->person_id].state = 1;
        pthread_mutex_lock(&mutex);
        organiser.players++;
        pthread_mutex_unlock(&mutex);
        meetOrganizer(args);
    }
    else
    {
        printf("Refree %d entered academy\n", args->person_id + 1);
        // refree meets organier only if one refree ahead of him has met the organiser
        while (!(args->person_id < 1 || refrees[args->person_id - 1].state == 3))
            ;
        refrees[args->person_id].state = 1;
        pthread_mutex_lock(&mutex);
        organiser.refree++;
        pthread_mutex_unlock(&mutex);
        meetOrganizer(args);
    }
}

void meetOrganizer(Args *arg)
{
    while (organiser.players < 2 || organiser.refree < 1)
        ;
    pthread_mutex_lock(&mutex);
    if (arg->person_type == player)
    {
        players[arg->person_id].state = 2;
        printf("Player %d met the organiser\n", arg->person_id + 1);
        players_ready_to_enter++;
    }
    else
    {
        refrees[arg->person_id].state = 2;
        printf("Refree %d met the organiser\n", arg->person_id + 1);
        refree_ready_to_enter++;
    }

    if (organiser.allowed == 0 && organiser.entered == 0)
    {
        organiser.allowed = 1;
        organiser.entered = 1;
    }
    else
    {
        if (organiser.entered == 2)
        {
            organiser.allowed = 0;
            organiser.entered = 0;
            organiser.players = 0;
            organiser.refree = 0;
        }
        else
            organiser.entered++;
    }
    pthread_mutex_unlock(&mutex);
    enterCourt(arg);
}

void enterCourt(Args *arg)
{
    while (players_ready_to_enter != 2 || refree_ready_to_enter != 1)
        ;
    if (arg->person_type == player)
    {
        printf("Player %d entered the court\n", arg->person_id + 1);
        warmUp(arg);
    }
    else
    {
        printf("Refree %d entered the court\n", arg->person_id + 1);
        adjustEquipment(arg);
    }
}

void warmUp(Args *arg)
{
    printf("Player %d started warming up for the game\n", arg->person_id + 1);
    sleep(1);
    pthread_mutex_lock(&mutex);
    printf("Player %d finished warming up\n", arg->person_id + 1);
    players[arg->person_id].state = 3;
    players_warmedup++;
    pthread_mutex_unlock(&mutex);
}

void adjustEquipment(Args *arg)
{
    printf("Refree %d started adjusting equipment for the game\n", arg->person_id + 1);
    sleep(0.5);
    pthread_mutex_lock(&mutex);
    printf("Refree %d finished adjusting equipment\n", arg->person_id + 1);
    refrees[arg->person_id].state = 3;
    pthread_mutex_unlock(&mutex);
    startGame(arg);
}

void startGame(Args *arg)
{
    while (players_warmedup != 2)
        ;
    printf("\x1b[32m"
           "Refree %d started the Game\n"
           "\x1b[0m",
           arg->person_id + 1);
    pthread_mutex_lock(&mutex);
    players_warmedup = 0;
    players_ready_to_enter = 0;
    refree_ready_to_enter = 0;
    printf("Organiser is now free\n");
    progress++;
    pthread_mutex_unlock(&mutex);
}
