#ifndef KMEANSOG_H
#define KMEANSOG_H

#include <stdio.h>
#include <stdlib.h>

typedef struct cord {
    double value;
    struct cord *next;
} cord;

typedef struct vector {
    cord *cords;
    struct vector *next;
} vector;

typedef struct modified_vector {
    cord *cords;
    int num_points;
    struct modified_vector *next;
} modified_vector;

/* Function prototypes */

void print_vector(vector *points);
void print_single_vector(vector *points);
void print_modified_vector(modified_vector *points);
void free_vector(vector *points);
void free_modified_vector(modified_vector *points);
void free_cord(cord *cord);
vector* kmeans_general(int K, int iter, double epsilon, vector *points, vector *cluster_points, int dimension);

#endif /* KMEANSOG_H */