#include <stdio.h>
int arr[10000000];

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
    if (beg < end)
    {
        int mid;
        mid = beg + (end - beg) / 2;
        MergeSort(beg, mid);
        MergeSort(mid + 1, end);
        Merge(beg, mid, end);
    }
    return;
}

int main()
{
    int n, i;
    scanf("%d", &n);
    for (i = 0; i < n; i++)
        scanf("%d", &arr[i]);

    MergeSort(0, n - 1);
    for (i = 0; i < n; i++)
        printf("%d ", arr[i]);
    printf("\n");
    return 0;
}