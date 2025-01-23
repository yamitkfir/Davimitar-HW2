#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/* #include "kmeansOG.h" */

#define EPSILON 0.001
#define ITER 200
#define MAX_ITER 1000
/* #define INFINITY2 (1.0 / 0.0); */
#define INFINITY2 INFINITY

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

/* Function declarations */
double distant_two_cords(cord *cord1, cord *cord2);
void assign_cluster(vector *single_point, vector *centroids, modified_vector *newborn_centroids);
vector* update_clusters(modified_vector *newborn_centroids);
vector* kmeans_general(int K, int iter, double epsilon, vector *points, vector *cluster_points, int dimension);
void readFile(vector **points, vector **cluster_points, int num_clusters);
void free_vector(vector *points);
void free_modified_vector(modified_vector *points);
void free_cord(cord *cord);
modified_vector* initiate_k_modified_vectors(int k, int dimension);
double calc_eps(vector *old_clusters, vector *new_clusters);
void get_points_from_file(vector **points); /* helper function for readFile */
void initialize_cluster_points(vector* points, vector **cluster_points, int num_clusters); /* helper function for readFile */
int get_vector_dimensions(vector* vec);
// char check_if_int(char* string);
// static char* vectors_to_transferred(vector* vectors, int dimension, int num_clusters);
// vector* from_string_to_vec(char* input);
// cord* read_str_to_cord(char* data);


/* TEMP for debugging: */
void print_vector(vector *points);
void print_single_vector(vector *points);
void print_modified_vector(modified_vector *points);
char check_readFile(vector *points, vector *cluster_points, int num_clusters);
char compare_vectors(cord* first_cord, cord* second_cord);


double distant_two_cords(cord *cord1, cord *cord2) {
    double dist = 0.0;
    while (cord1 && cord2) {
        dist += (cord1->value - cord2->value) * (cord1->value - cord2->value);
        cord1 = cord1->next;
        cord2 = cord2->next;
    }
    return sqrt(dist);
}

/* Given a single point, and a LinkedList of current clusters, and a LinkedList of newborn_centroids,
Find its fitting cluster and add its cords to the fitting place in newborn_centroids */
void assign_cluster(vector *single_point, vector *centroids, modified_vector *newborn_centroids) {
    modified_vector *closest_newborn = newborn_centroids; /* by default it'll be the first one */
    cord *cord_point = NULL;
    cord *cord_newborn = NULL;
    double min_dist = INFINITY2;

    /* If something's wrong with input */
    if (!single_point || !centroids || !newborn_centroids){
        printf("An Error Has Occurred\n");
        exit(1);
    }

    while(centroids && newborn_centroids) { /* they're supposed to be similar-length anyway */
        double dist = distant_two_cords(single_point->cords, centroids->cords);
        if (dist < min_dist) {
            min_dist = dist;
            closest_newborn = newborn_centroids;
        }
        centroids = centroids->next;
        newborn_centroids = newborn_centroids->next;
    }
    
    /* Now that "closest_newborn" is initialized, add all cords of "single_point" to its cords */
    cord_point = single_point->cords;
    cord_newborn = closest_newborn->cords;

    while (cord_point && cord_newborn) { /* they're supposed to be similar-length anyway */
        cord_newborn->value += cord_point->value;
        /* updates */
        cord_point = cord_point->next;
        cord_newborn = cord_newborn->next;
    }
    closest_newborn->num_points++;
}

/* Given LinkedList of modified_vectors, calculate and return an updated LinkedList of new centroids */
vector* update_cluster(modified_vector *newborn_centroids) {
    cord *newborn_cord = NULL;
    cord *current_cord = NULL;
    cord *current_cord_holder = NULL;
    vector *new_clusters = malloc(sizeof(vector));
    vector *new_clusters_holder = new_clusters;
    int num_points;

    if (new_clusters == NULL) {
        printf("An Error Has Occurred\n"); /* failed to allocate memory - why? */
        exit(1);
    }

    while (newborn_centroids) {
        num_points = newborn_centroids->num_points;
        current_cord = malloc(sizeof(cord));
        if (current_cord == NULL) {
            printf("An Error Has Occurred\n"); /* failed to allocate memory - why? */
            exit(1);
        }
        current_cord_holder = current_cord;
        newborn_cord = newborn_centroids->cords;
        while (newborn_cord) {
            current_cord->value = (newborn_cord->value / num_points);
            newborn_cord = newborn_cord->next;
            if (newborn_cord) {
                current_cord->next = malloc(sizeof(cord));
                if (current_cord->next == NULL) {
                    printf("An Error Has Occurred\n"); /* failed to allocate memory - why? */
                    exit(1);
                }
                current_cord = current_cord->next;
            } else {
                current_cord->next = NULL;
            }
        }

        new_clusters->cords = current_cord_holder;
        newborn_centroids = newborn_centroids->next;
        if (newborn_centroids) {
            new_clusters->next = malloc(sizeof(vector));
            if (new_clusters->next == NULL) {
                printf("An Error Has Occurred\n"); /* failed to allocate memory - why? */
                exit(1);
            }
            new_clusters = new_clusters->next;
        } else {
            new_clusters->next = NULL;
        }
    }

    return new_clusters_holder;
}

/* Does all logical stuff of PDF's algorithm, including the general loop until exit term. */
vector* kmeans_general(int K, int iter, double epsilon, vector *points, vector *cluster_points, int dimension) {
    modified_vector *newborn_m_clusters = NULL;
    vector *new_cluster_points = NULL;
    vector *still_points;
    double current_eps = INFINITY2;

    while (iter > 0 && current_eps > epsilon) {
        newborn_m_clusters = initiate_k_modified_vectors(K, dimension);
        still_points = points;
        while (still_points) {
            assign_cluster(still_points, cluster_points, newborn_m_clusters);
            still_points = still_points->next;
        }
        new_cluster_points = update_cluster(newborn_m_clusters);
        current_eps = calc_eps(cluster_points, new_cluster_points);
        free_vector(cluster_points); /* free the old clusters */
        cluster_points = new_cluster_points;
        iter--;

        /* Free the memory allocated for newborn_m_clusters */
        free_modified_vector(newborn_m_clusters);
    }
    /* print_vector(cluster_points); */
    //transferred = vectors_to_transferred(cluster_points, dimension, K);
    /******************************************************************* */
    /*                        Major Changes                              */
    /* now we return the cluster points vector, as it needs to be packed */
    /* and sent to Python threw capi. Therefore we are freeing in only   */
    /* in the module.                                                    */
    /******************************************************************* */
    free_vector(points);
    return cluster_points;
}

// static char* vectors_to_transferred(vector* transferred, int dimension, int num_clusters){
//     char* output = calloc((sizeof(double) + 1)*dimension*num_clusters, 1);
//     char* buffer;
//     vector* current_vector = transferred;
//     cord* current_cord;
//     int i = 0;

//     while(current_vector){
//         current_cord = current_vector->cords;
//         while(current_cord){
//             buffer = calloc(sizeof(double)+1, 1);
//             sprintf(buffer, "%lf,", current_cord->value);
//             memcpy(output + i, buffer, strlen(buffer));
//             i += strlen(buffer);
//             current_cord = current_cord->next;
//             free(buffer); 
//         }
//         current_vector = current_vector->next;
//         output[i-1] = '\n';
//     }

//     return output;
// }

void readFile(vector **points, vector **cluster_points, int num_clusters) {
    get_points_from_file(points);
    initialize_cluster_points(*points, cluster_points, num_clusters);
}

void get_points_from_file(vector **points) {
    double temp; 
    cord** current_coordinate;
    char new_vector = 1;
    char c;

    while (scanf("%lf%c", &temp,&c) == 2) {
        if(!(c == ',') && !(c == '\n')){
            printf("An Error Has Occurred\n");
            exit(1);
        }
        if (new_vector) {
            *points = calloc(1,sizeof(vector));
            current_coordinate = &((*points)->cords);  
            new_vector = 0;
        }
        (*current_coordinate) = calloc(1,sizeof(cord));
        (*current_coordinate)->value = temp;
        current_coordinate = &((*current_coordinate)->next);
        if (c == '\n') {
            new_vector = 1;
            points = &((*points)->next);
        }
    }
    
    /* Theres still input after WHILE -> input is in wrong format*/
    if (scanf("%c", &c) != EOF) {
        printf("An Error Has Occurred\n");
        exit(1);
    }

    /*
    dimension = get_vector_dimensions(*points);
    printf("dimension: %d\n", dimension);
    if (dimension == 0) {
        printf("An Error Has Occurred\n");
        exit(1);
    }
    */
}

void initialize_cluster_points(vector* points, vector **cluster_points, int num_clusters) {
    cord** current_cluster_coordinate;
    cord* current_coordinate;
    int i;

    for(i=0; i < num_clusters; i++) {
        if(points == NULL || points->next == NULL) {
            printf("Invalid number of clusters!\n");
            exit(1);
        }
        (*cluster_points) = calloc(1,sizeof(vector));
        current_cluster_coordinate = &((*cluster_points)->cords);
        current_coordinate = points->cords;
        while(current_coordinate) {
            (*current_cluster_coordinate) = calloc(1,sizeof(cord));
            (*current_cluster_coordinate)->value = current_coordinate->value;
            current_cluster_coordinate = &((*current_cluster_coordinate)->next);
            current_coordinate = current_coordinate->next;
        }
        points = points->next;
        cluster_points = &((*cluster_points)->next);
    }
}

void print_vector(vector *points) {
    while(points) {
        print_single_vector(points);
        points = points->next;
    }
    /* printf("\n"); */
}
void print_modified_vector(modified_vector *points) {
    while(points) {
        cord *cord = points->cords;
        if(cord){
            printf("%.4f", cord->value);
        }
        cord = cord->next;
        while(cord) {
            printf(",%.4f", cord->value);
            cord = cord->next;
        }
        printf(" | num_points: %d\n", points->num_points);
        points = points->next;
    }
    /* printf("\n"); */
}
void print_single_vector(vector *points) {
    cord *cordi = points->cords;
    if(cordi){
        printf("%.4f", cordi->value);
    }
    cordi = cordi->next;
    while(cordi) {
        printf(",%.4f", cordi->value);
        cordi = cordi->next;
    }
    printf("\n");
}

/* Check correctness of ReadFile; Returns 1 if it's cool and 0 if it was wrong (and prints error). */
char check_readFile(vector *points, vector *cluster_points, int num_clusters) {
    int i=0;
    for(i = 0; i < num_clusters; i++) {
        if(cluster_points == NULL) {
            /*Error: not enough cluster points*/
            printf("An Error Has Occurred\n");
            return 0;
        }
        if(points == NULL) {
            /*Error: not enough points*/
            printf("An Error Has Occurred\n");
            return 0;
        }
        if(!compare_vectors(points->cords, cluster_points->cords)) {
            /*Error: points and cluster points do not match*/
            printf("An Error Has Occurred\n");
            return 0;
        }
        points = points->next;
        cluster_points = cluster_points->next;
    }
    return 1;
}

/* Returns 1 if similar, 0 if different. */
char compare_vectors(cord* first_cord, cord* second_cord) {
    while(first_cord && second_cord) {
        if(first_cord->value != second_cord->value) {
            return 0;
        }
        first_cord = first_cord->next;
        second_cord = second_cord->next;
    }
    return 1;
}

/* Free vector and all its nexts, including recursive freeing of cords inside them. */
void free_vector(vector *points){
    vector *temp_vector;
    while (points) {
        temp_vector = points;
        points = points->next;
        free_cord(temp_vector->cords);
        free(temp_vector);
    }
}
void free_cord(cord *cords){
    cord *temp_cord;
    while (cords) {
        temp_cord = cords;
        cords = cords->next;
        free(temp_cord);
    }
}
void free_modified_vector(modified_vector *points){
    modified_vector *temp_vector;
    while (points) {
        temp_vector = points;
        points = points->next;
        free_cord(temp_vector->cords);
        free(temp_vector);
    }
}
/* Calloc (initialize with 0s) LinkedList of K modified_vectors */
modified_vector* initiate_k_modified_vectors(int k, int dimension){
    modified_vector *head = NULL;
    modified_vector *current = NULL;
    cord *current_cord = NULL;
    int i; int j;

    for (i = 0; i < k; i++) {
        modified_vector *new_vector = calloc(1, sizeof(modified_vector));
        new_vector->cords = calloc(1, sizeof(cord));
        current_cord = new_vector->cords;

        for (j = 1; j < dimension; j++) {
            current_cord->next = calloc(1, sizeof(cord));
            current_cord = current_cord->next;
        }
        if (head == NULL) {
            head = new_vector;
        } else {
            current->next = new_vector;
        }
        current = new_vector;
    } 
    return head;
}
/* Eclidean Distance, between updated centroid to the previous (for every centroid)*/
double calc_eps(vector *old_clusters, vector *new_clusters){
    double max_distance = 0.0;
    double dist;
    cord *old_cord; cord *new_cord;

    while (old_clusters && new_clusters) {
        dist = 0.0;
        old_cord = old_clusters->cords;
        new_cord = new_clusters->cords;

        while (old_cord && new_cord) {
            dist += (old_cord->value - new_cord->value) * (old_cord->value - new_cord->value);
            old_cord = old_cord->next;
            new_cord = new_cord->next;
        }
        dist = sqrt(dist);
        if (dist > max_distance) {
            max_distance = dist;
        }
        old_clusters = old_clusters->next;
        new_clusters = new_clusters->next;
    }
    return max_distance;
}

int get_vector_dimensions(vector* vec){
    int count =  0;
    cord* current_coordinate = vec->cords;
    while(current_coordinate){
        count++;
        current_coordinate = current_coordinate->next;
    }
    return count;
}

char check_if_int(char* string){
    int i = 0;
    while(string[i] != 0){
        if(string[i] < '0' || string[i] > '9'){
            return 0;
        }
        i++;
    }
    return 1;
}

// int main(int argc, char *argv[]) {
//     vector *original_points = NULL;
//     vector *cluster_points = NULL; /* centroids */
//     int k;
//     int iterations = ITER;
//     int dimension;
    
//     if(argc != 3 && argc != 2){
//         printf("An Error Has Occurred\n");
//         return 1;
//     }
//     if (check_if_int(argv[1])) {
//         k = atoi(argv[1]);
//         if (k <= 1) {
//             printf("Invalid number of clusters!\n");
//             return 1;
//         }
//     }
//     else {
//         printf("Invalid number of clusters!\n");
//         return 1;
//     }

//     /* also set iteration: */
//     if (argc == 3){
//         if(check_if_int(argv[2])){
//             iterations = atoi(argv[2]);
//             if (iterations >= MAX_ITER || iterations <= 1) {
//                 printf("Invalid maximum iteration!\n");
//                 return 1;
//             }
//         }
//         else{
//             printf("Invalid maximum iteration!\n");
//             return 1;
//         }
//     }

//     readFile(&original_points, &cluster_points, k);
//     if(!check_readFile(original_points, cluster_points, k)) {
//         /*Error: points and cluster points do not match*/
//         printf("An Error Has Occurred\n");
//         return 1;
//     }
//     else {
//         dimension = get_vector_dimensions(original_points);
//         kmeans_general(k, iterations, EPSILON, original_points, cluster_points, dimension);
//     }
//     return 0;
// }
