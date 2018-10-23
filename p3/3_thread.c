#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int arr[10000000];
typedef struct array
{
    int beg;
    int end;
} ARRAY;

pthread_mutex_t mutex;

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

void *MergeSort(void *array)
{
    ARRAY *a = (ARRAY *)array;
    int beg = a->beg;
    int end = a->end;
    int mid = beg + (end - beg) / 2;

    pthread_t leftid, rightid;
    ARRAY left_child, right_child;
    left_child.beg = beg;
    left_child.end = mid;
    right_child.beg = mid + 1;
    right_child.end = end;

    if (beg < end)
    {
        int x = end - beg + 1;
        if (x < 5)
        {
            pthread_mutex_lock(&mutex);
            int i, j, smallest, temp;
            for (i = beg; i <= end; i++)
            {
                smallest = i;
                for (j = i; j <= end; j++)
                {
                    if (arr[j] < arr[smallest])
                        smallest = j;
                }
                temp = arr[i];
                arr[i] = arr[smallest];
                arr[smallest] = temp;
            }
            pthread_mutex_unlock(&mutex);
            pthread_exit(0);
        }

        pthread_create(&leftid, NULL, MergeSort, &left_child);
        pthread_join(leftid, NULL);
        pthread_create(&rightid, NULL, MergeSort, &right_child);
        pthread_join(rightid, NULL);

        pthread_mutex_lock(&mutex);
        Merge(beg, mid, end);
        pthread_mutex_unlock(&mutex);
        pthread_exit(0);
    }
    pthread_exit(0);
}

int main()
{
    int n, i;
    scanf("%d", &n);
    for (i = 0; i < n; i++)
        scanf("%d", &arr[i]);
    ARRAY array;
    pthread_t sortid;
    array.beg = 0;
    array.end = n - 1;
    pthread_create(&sortid, NULL, MergeSort, &array);
    pthread_join(sortid, NULL);
    for (i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");
    return 0;
}
