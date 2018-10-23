#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>

#define SHSIZE 10000000
int *arr = NULL;

void Merge(int beg, int mid, int end)
{
    int n1 = mid - beg + 1;
    int n2 = end - mid;
    int i, j, k;
    int left[n1], right[n2];

    for (i = 0; i < n1; i++)
        left[i] = arr[beg + i];
    for (j = 0; j < n2; j++)
        right[j] = arr[mid + 1 + j];

    i = 0;
    j = 0;
    k = beg;
    while ((i < n1) && (j < n2))
    {
        if (left[i] < right[j])
        {
            arr[k] = left[i];
            i++;
        }
        else
        {
            arr[k] = right[j];
            j++;
        }
        k++;
    }

    while (i < n1)
    {
        arr[k] = left[i];
        i++;
        k++;
    }
    while (j < n2)
    {
        arr[k] = right[j];
        j++;
        k++;
    }
}

void MergeSort(int beg, int end)
{
    pid_t left_child, right_child;
    if (beg < end)
    {
        int x = end - beg + 1;
        if (x < 5)
        {
            int i, j, smallest, temp;
            for (i = beg; i <= end; i++)
            {
                smallest = i;
                for (j = i + 1; j <= end; j++)
                {
                    if (arr[j] < arr[smallest])
                        smallest = j;
                }
                temp = arr[i];
                arr[i] = arr[smallest];
                arr[smallest] = temp;
            }
        }
        else
        {
            int mid = beg + (end - beg) / 2;
            left_child = fork();
            if (left_child < 0)
            {
                perror("fork failed\n");
                exit(1);
            }
            if (left_child == 0)
            {
                MergeSort(beg, mid);
                exit(0);
            }
            if (left_child > 0)
            {
                right_child = fork();
                if (right_child < 0)
                {
                    perror("fork failed\n");
                    exit(1);
                }
                else if (right_child == 0)
                {
                    MergeSort(mid + 1, end);
                    exit(0);
                }
            }
            int status;
            waitpid(left_child, &status, 0);
            waitpid(right_child, &status, 0);
            Merge(beg, mid, end);
        }
    }
    return;
}

int main()
{
    int n, i;
    scanf("%d\n", &n);
    key_t key = IPC_PRIVATE;
    int shmid = shmget(key, sizeof(int) * n, IPC_CREAT | 0666);
    if (shmid == -1)
    {
        perror("shmget failed\n");
        exit(1);
    }
    arr = shmat(shmid, 0, 0);
    if (arr == (void *)-1)
    {
        perror("shmat failed\n");
        exit(1);
    }
    for (i = 0; i < n; i++)
        scanf("%d", &arr[i]);
    MergeSort(0, n - 1);
    for (i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");
    return 0;
}