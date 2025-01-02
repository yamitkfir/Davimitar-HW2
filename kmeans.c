# include <stdio.h>
# include <stdlib.h>
#include <math.h>
#ifndef INFINITY
#define INFINITY (1.0 / 0.0)
#endif
# define epsilon 0.001

struct cord
{
    double value;
    struct cord *next;
};
struct vector
{
    struct vector *next;
    struct cord *cords;
};

/* Freeing functions for cords and vectors. Frees the given cord\vector and all following ones.*/
void free_cord_list(struct cord *head_cord)
{
    if (head_cord != NULL)
        free_cord_list(head_cord->next);
    free(head_cord);
}
void free_vector_list(struct vector *head_vec)
{
    if (head_vec != NULL)
    {
        free_vector_list(head_vec->next);
        free_cord_list(head_vec->cords); 
    }
    free(head_vec);
}


/* Printing functions for cords and vector lists. */
void print_cord_list(struct cord *head_cord)
{
    struct cord *curr_cord = head_cord;
    while (curr_cord != NULL)
    {
        printf("%.4f", curr_cord->value);
        if (curr_cord->next != NULL)
            printf(", ");
        curr_cord = curr_cord->next;
    }
}
void print_vector_list(struct vector *head_vec)
{
    struct vector *curr_vec = head_vec;
    while (curr_vec != NULL)
    {
        print_cord_list(curr_vec->cords);
        printf("\n");
        curr_vec = curr_vec->next;
    }
}


struct cord *copy_cords(struct cord *head_cord)
{
    struct cord *new_head_cord, *curr_cord, *new_curr_cord;
    new_head_cord = (struct cord*)malloc(sizeof(struct cord));
    curr_cord = head_cord;
    new_curr_cord = new_head_cord;
    while(curr_cord != NULL)
    {
        new_curr_cord->value = curr_cord->value;
        if (curr_cord->next != NULL)
        {
            new_curr_cord->next = (struct cord*)malloc(sizeof(struct cord));
            new_curr_cord = new_curr_cord->next;
        }
        else
            new_curr_cord->next = NULL;
        curr_cord = curr_cord->next;
    }
    return new_head_cord;
}


int check_k_legality(int k, struct vector *head_vec)
/* Checks if 1 < K < N. Returns 1 if yes, and 0 if not. */
{
    int N = 0;
    struct vector *curr_vec = head_vec;
    while (curr_vec != NULL)
    {
        N++;
        curr_vec = curr_vec->next;
    }
    if (k < 1 || k >= N)
        return 0;
    return 1;
}


struct vector* init_centroids(int k, struct vector* head_vec)
{
    int i;
    struct vector *head_centroid, *curr_centroid, *curr_head = head_vec;
    head_centroid = (struct vector*)malloc(sizeof(struct vector));
    curr_centroid = head_centroid;

    curr_centroid->cords = copy_cords(curr_head->cords);
    curr_centroid->next = NULL; 
    curr_head = curr_head->next;

    for (i = 1; i < k; i++)
    {
        curr_centroid->next = (struct vector*)malloc(sizeof(struct vector));
        curr_centroid = curr_centroid->next;
        curr_centroid->cords = copy_cords(curr_head->cords);
        curr_centroid->next = NULL;
        curr_head = curr_head->next;   
    }
    return head_centroid;
}


struct vector* init_vector_list()
{
    struct vector *head_vec, *curr_vec;
    struct cord *head_cord, *curr_cord;
    double n;
    int is_vector_start = 0; /* Do we start a new vector in the next iteration? */
    char c;

    head_cord = malloc(sizeof(struct cord));
    curr_cord = head_cord;
    curr_cord->next = NULL;

    head_vec = malloc(sizeof(struct vector));
    curr_vec = head_vec;
    curr_vec->next = NULL;
    curr_vec->cords = head_cord;
    while (scanf("%lf%c", &n, &c) == 2)
    {
        if (n != '\n' && c != EOF)
        {
            if (is_vector_start)
            {
                curr_vec->next = malloc(sizeof(struct vector));
                curr_vec = curr_vec->next; 
                curr_vec->next = NULL;
                head_cord = malloc(sizeof(struct cord));
                curr_cord = head_cord;
                curr_cord->next = NULL;
            }
            if (c == '\n') /* We have reached the end of the vector. */
            {
                curr_cord->value = n;
                curr_vec->cords = head_cord;  
                is_vector_start = 1;
            }
            else
            {
                is_vector_start = 0;
                curr_cord->value = n;
                curr_cord->next = malloc(sizeof(struct cord));
                curr_cord = curr_cord->next;
                curr_cord->next = NULL;
            }
        }
    }
    return head_vec;
}


double euclidian_distance(struct vector *vec1, struct vector *vec2)
/* Given two vectors, calculates the Euclidian distance between them. */
{
    struct cord *cord1 = vec1->cords;
    struct cord *cord2 = vec2->cords;
    double sum = 0;
    while (cord1 != NULL)
    {
        sum += pow((cord1->value - cord2->value), 2);
        cord1 = cord1->next;
        cord2 = cord2->next;
    }
    return sqrt(sum);
}


int find_closest_centroid(struct vector *head_centroid, struct vector *vec)
/* Given a vector, returns the number of the closest centroid to it (from 0 to K-1). */
{
    struct vector *curr_centroid = head_centroid;
    double min_distance = INFINITY;
    int closest_centroid = 0;
    double curr_distance = 0.0;
    int i = 0;
    do
    {
        curr_distance = euclidian_distance(curr_centroid, vec);
        if (curr_distance < min_distance)
        {
            min_distance = curr_distance;
            closest_centroid = i;
        }
        i++;
        curr_centroid = curr_centroid->next;
    } while (curr_centroid != NULL);
    return closest_centroid;
}


void assign_vectors_to_centroids(struct vector *vectors_to_centroid, struct vector *head_centroid, struct vector *head_vec)
/* Assigns every vector to the closest centroid. */
{
    struct vector *curr_vec = head_vec;
    struct vector *vector_list_in_index;
    struct vector *temp;
    int centroid_index = 0;

    while (curr_vec != NULL)
    {
        centroid_index = find_closest_centroid(head_centroid, curr_vec);
        vector_list_in_index = &vectors_to_centroid[centroid_index];
        if (vector_list_in_index->cords == NULL) /* First point assigned to the centroid. */
            vector_list_in_index->cords = copy_cords(curr_vec->cords);
        else /* Centroid already has points assigned to it, we need to add the new point to the list. */
        {
            temp = (struct vector*)malloc(sizeof(struct vector));
            temp->cords = copy_cords(curr_vec->cords);
            temp->next = vector_list_in_index->next;
            vector_list_in_index->next = temp;
        }
        curr_vec = curr_vec->next;
    }
}


struct vector* create_new_centroid(struct vector *vec_list)
/* Creates a centroid from a given vector list, and returns it as a new vector struct. Assumes the list isn't empty. */
{
    int i,j;
    struct vector *curr_vec = vec_list, *new_centroid;
    struct cord *curr_cord = curr_vec->cords, *new_centroid_cords,*tmp;
    int vector_size = 0, vector_amount = 0;
    double *new_coords;

    while (curr_cord != NULL)
    {
        vector_size++;
        curr_cord = curr_cord->next;
    }
    new_coords = (double*)calloc(vector_size, sizeof(double)); /* An array for calculating the new centroid's coordinates. */
    while (curr_vec != NULL)
    {
        vector_amount++;
        curr_cord = curr_vec->cords;
        for (i = 0; i < vector_size; i++)
        {
            new_coords[i] += curr_cord->value;
            curr_cord = curr_cord->next;
        }
        curr_vec = curr_vec->next;
    }
   
    new_centroid_cords = (struct cord*)malloc(sizeof(struct cord));
    tmp=new_centroid_cords;
    for (j = 0; j < vector_size; j++) /* Creating the new centroid, one coordinate after another. */
    {     
        new_centroid_cords->value = (new_coords[j] / vector_amount);
        if (j < vector_size - 1)
        {
            new_centroid_cords->next = (struct cord*)malloc(sizeof(struct cord));
            new_centroid_cords = new_centroid_cords->next;
        }
        else /* We have reached the last coordinate. */
            new_centroid_cords->next = NULL;
    }
    new_centroid = (struct vector*)malloc(sizeof(struct vector));
    new_centroid->cords = tmp;
    new_centroid->next = NULL;
    return new_centroid;
}

double update_centroid_list(struct vector *head_centroid, struct vector *vectors_to_centroid, int k)
/* Updates the centroids to be the average of their assigned vectors, and returns the maximum difference between a previous centroid and its new one. */
{
    int i = 0; 
    double diff = 0, max_diff = 0;
    struct vector *curr_centroid = head_centroid, *new_centroid;
    while (i < k)
    {
        new_centroid = create_new_centroid(&vectors_to_centroid[i]); /* Will create a new vector struct! */
        diff = euclidian_distance(new_centroid, curr_centroid);
        if (diff > max_diff)
            max_diff = diff;
        curr_centroid->cords = copy_cords(new_centroid->cords);
        free_vector_list(new_centroid);
        curr_centroid = curr_centroid->next;
        i++;
    }
    return max_diff;
}


int main(int argc, char **argv)
{
    struct vector *head_points = NULL, *head_centroid = NULL, *vector_to_centroid=NULL, *tmp_vec = NULL;
    struct cord *tmp_cord = NULL;
    int s,k = 0, iter = 0, iter_num = 0, i = 0, vector_difference = 0;

    if (argc == 3)
    {
        iter = 200;
    }
    else if (argc == 4)
    {
        iter = atoi(argv[2]);
        if (iter < 1 || iter > 1000)
        {
            printf("Invalid maximum iteraion!\n"); 
   
            return 1;
        }
    }
    k = atoi(argv[1]);
    
    head_points = (struct vector*)malloc(sizeof(struct vector));
    head_points = init_vector_list();
    if (!check_k_legality(k, head_points))
    {
        printf("Invalid number of clusters!\n");
        free_vector_list(head_points);
        return 1;
    }

    head_centroid = init_centroids(k,head_points);
    vector_to_centroid = (struct vector*)malloc(sizeof(struct vector) * k);
    for (s = 0; s < k; s++)
    {
        vector_to_centroid[s].cords = NULL;
        vector_to_centroid[s].next = NULL;
    }

    while(iter_num < iter) /* The actual K-means algorithm. */
    {     
        assign_vectors_to_centroids(vector_to_centroid, head_centroid, head_points);
        vector_difference = update_centroid_list(head_centroid, vector_to_centroid, k);       
        for (i = 0; i < k; i++)
        {
            tmp_vec = vector_to_centroid[i].next;
            tmp_cord = vector_to_centroid[i].cords;
            free_vector_list(tmp_vec);
            free_cord_list(tmp_cord);
            tmp_vec = NULL;
            tmp_cord = NULL;
            vector_to_centroid[i].cords = NULL;
            vector_to_centroid[i].next = NULL;
        }
        if (vector_difference < epsilon)
            break;
        iter_num++; 
    }
    printf("%d Means Result:\n", k);
    print_vector_list(head_centroid);

    free_vector_list(head_centroid);
    free_vector_list(head_points); 
    free(vector_to_centroid);
    return 0;
}