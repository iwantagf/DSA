#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

static void free_int(void *a) {
    free(*(int **)a);
}

int main() {
    vector(int) t = NULL;
    vector_init(t, 0, free_int);

    for (int i = 1; i <= 10000; ++i)
        vector_push_back(t, i);

    vector_clear(t);

    printf("%d ", vector_size(t));
}