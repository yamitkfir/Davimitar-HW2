/* we use this TEMP file to test things we wish to add to module, since its bugs 
will be shown only in a much later step, bc the "<Python.h>" doesn't compile by definition.
So, please, use this as much as possible to prevent Taut Nigreret */

#include "kmeansOG.h"

int main() {
    vector *vectors_1 = NULL;
    vector *clusters_initially = NULL;
    // TODO YAMIT: why doesn't it recognize kmeansOG's funcs?
    print_vector(vectors_1);
    printf("I beg to die");
    return 0;
    // run by: "gcc temp.c kmeansOG.c -o temp -lm; .\temp.exe"
}