/* This file contains the variable and function declarations */

# ifndef _GLOBAL_H_
# define _GLOBAL_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

# define INF 1.0e14
# define EPS 1.0e-14
# define E  2.71828182845905
# define PI 3.14159265358979

//added to encapsulate variables
typedef struct NSGAIIProject
{
    int nreal;
    int nbin;
    int nobj;
    int ncon;
    int popsize;
    double pcross_real;
    double pcross_bin;
    double pmut_real;
    double pmut_bin;
    double eta_c;
    double eta_m;
    int ngen ;
    int nbinmut;
    int nrealmut;
    int nbincross;
    int nrealcross;
    int *nbits;
    double *min_realvar;
    double *max_realvar;
    double *min_binvar;
    double *max_binvar;
    int bitlength;
    int currentGen;
    int max_nbits;
    FILE *fpt1;
    FILE *fpt2;
    FILE *fpt3;
    FILE *fpt4;
    FILE *fpt5;
}NSGAIIProject;


typedef struct
{
    int rank;
    double constr_violation;
    double *xreal;
    int **gene;
    double *xbin;
    double *obj;
    double *constr;
    double crowd_dist;
}
individual;

typedef struct
{
    individual *ind;
}
population;

typedef struct lists
{
    int index;
    struct lists *parent;
    struct lists *child;
}
list;

void allocate_memory_project(NSGAIIProject **project);
void deallocate_memory_project(NSGAIIProject *project);

void allocate_memory_pop (NSGAIIProject *project, population *pop, int size);
void allocate_memory_ind (NSGAIIProject *project, individual *ind);
void deallocate_memory_pop (NSGAIIProject *project,  population *pop, int size);
void deallocate_memory_ind (NSGAIIProject *project, individual *ind);

double maximum (double a, double b);
double minimum (double a, double b);

void crossover (NSGAIIProject *project, individual *parent1, individual *parent2, individual *child1, individual *child2);
void realcross (NSGAIIProject *project, individual *parent1, individual *parent2, individual *child1, individual *child2);
void bincross  (NSGAIIProject *project, individual *parent1, individual *parent2, individual *child1, individual *child2);

void assign_crowding_distance_list (NSGAIIProject *project, population *pop, list *lst, int front_size);
void assign_crowding_distance_indices (NSGAIIProject *project, population *pop, int c1, int c2);
void assign_crowding_distance (NSGAIIProject *project, population *pop, int *dist, int **obj_array, int front_size);

void decode_pop (NSGAIIProject *project, population *pop);
void decode_ind (NSGAIIProject *project, individual *ind);

int check_dominance (NSGAIIProject *project, individual *a, individual *b);

//void evaluate_pop (population *pop);
//void evaluate_ind (int index, individual *ind);


void fill_nondominated_sort (NSGAIIProject *project, population *mixed_pop, population *new_pop);
void crowding_fill (NSGAIIProject *project, population *mixed_pop, population *new_pop, int count, int front_size, list *cur);

void initialize_pop (NSGAIIProject *project, population *pop);
void initialize_ind (NSGAIIProject *project, individual *ind);

void insert (list *node, int x);
list* del (list *node);

void merge(NSGAIIProject *project,  population *pop1, population *pop2, population *pop3);
void copy_ind (NSGAIIProject *project,  individual *ind1, individual *ind2);

void mutation_pop (NSGAIIProject *project, population *pop);
void mutation_ind (NSGAIIProject *project, individual *ind);
void bin_mutate_ind (NSGAIIProject *project, individual *ind);
void real_mutate_ind (NSGAIIProject *project, individual *ind);

//void test_problem (double *xreal, double *xbin, int **gene, double *obj, double *constr);

void assign_rank_and_crowding_distance (NSGAIIProject *project,  population *new_pop);

void report_pop (NSGAIIProject *project,  population *pop, FILE *fpt);
void report_feasible (NSGAIIProject *project, population *pop, FILE *fpt);
void report_ind (individual *ind, FILE *fpt);

void quicksort_front_obj(population *pop, int objcount, int obj_array[], int obj_array_size);
void q_sort_front_obj(population *pop, int objcount, int obj_array[], int left, int right);
void quicksort_dist(population *pop, int *dist, int front_size);
void q_sort_dist(population *pop, int *dist, int left, int right);

void selection (NSGAIIProject *project, population *old_pop, population *new_pop);
individual* tournament (NSGAIIProject *project, individual *ind1, individual *ind2);


#ifdef __cplusplus
}
#endif
# endif
