// #include <stdio.h>

// int main() {
//     int n;
//     printf("Enter a number: ");
//     scanf("%d", &n);

//     if (n % 2 == 0)
//         printf("%d is Even\n", n);
//     else
//         printf("%d is Odd\n", n);
//     return 0;
//  }

// Largest Element in an Array
// #include <stdio.h>

// int main() {
//     int arr[100], n, i, max;
//     printf("Enter size: ");
//     scanf("%d", &n);

//     printf("Enter %d elements: ", n);
//     for (i = 0; i < n; i++)
//         scanf("%d", &arr[i]);

//     max = arr[0];
//     for (i = 1; i < n; i++)
//         if (arr[i] > max) max = arr[i];

//     printf("Largest = %d\n", max);
//     return 0;
// }


// Function Example (Swap using Pointers)

#include <stdio.h>

void swap(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

int main() {
    int a = 5, b = 10;
    printf("Before: a = %d, b = %d\n", a, b);
    swap(&a, &b);
    printf("After:  a = %d, b = %d\n", a, b);
    return 0;
}