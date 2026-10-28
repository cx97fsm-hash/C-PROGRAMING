// String Length and Reverse
// #include <stdio.h>
// #include <string.h>

// int main() {
//     char str[100], rev[100];
//     int i, len;
//     printf("Enter a string: ");
//     scanf("%s", str);

//     len = strlen(str);
//     for (i = 0; i < len; i++)
//         rev[i] = str[len - 1 - i];
//     rev[len] = '\0';

//     printf("Length = %d\n", len);
//     printf("Reversed = %s\n", rev);
//     return 0;
// }

// Function Example (Swap using Pointers)
// #include <stdio.h>

// void swap(int *x, int *y) {
//     int temp = *x;
//     *x = *y;
//     *y = temp;
// }

// int main() {
//     int a = 5, b = 10;
//     printf("Before: a = %d, b = %d\n", a, b);
//     swap(&a, &b);
//     printf("After:  a = %d, b = %d\n", a, b);
//     return 0;
// }





// Largest Element in an Array

// #include <stdio.h>

// int main()
// {
//     int arr[100], n, i, max;
//     printf("Enter size: ");
//     scanf("%d", &n);

//     printf("Enter %d elements: ", n);
//     for (i = 0; i < n; i++)
//     {
//         scanf("%d", &arr[i]);
//     }

//     max = arr[0];
//     for (i = 1; i < n; i++)
//     {
//         if (arr[i] > max)
//         {
//             max = arr[i];
//         }
//     }

//     printf("Largest = %d\n", max);
//     return 0;
// }



// Palindrome Check (Number)

// #include <stdio.h>

// int main() {
//     int n, original, rev = 0;
//     printf("Enter a number: ");
//     scanf("%d", &n);
//     original = n;

//     while (n != 0) {
//         rev = rev * 10 + n % 10;
//         n /= 10;
//     }
//     if (original == rev)
//         printf("Palindrome\n");
//     else
//         printf("Not a Palindrome\n");
//     return 0;
// }