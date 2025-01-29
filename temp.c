/* we use this TEMP file to test things we wish to add to module, since its bugs 
will be shown only in a much later step, bc the "<Python.h>" doesn't compile by definition.
So, please, use this as much as possible to prevent Taut Nigreret */

#include "kmeansOG.h"

int main() {
    int n = 10;
    int **a;
    int i;
    a = malloc(n * sizeof(int *));
    for (i = 0; i < n; i++)
        a[i] = malloc(n * sizeof(int));

        
    // Fill the array with some values for demonstration
    for (i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            a[i][j] = i * n + j;
        }
    }

    // Print the array
    for (i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", a[i][j]);
        }
        printf("\n");
    }

    // Free the allocated memory
    for (i = 0; i < n; i++) {
        free(a[i]);
    }
    free(a);

    return 0;
}