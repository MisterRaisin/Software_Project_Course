#ifndef SYMNMF_H
#define SYMNMF_H

/*Function prototypes*/

double** allocate_matrix(int rows, int cols);
void free_matrix(double** matrix, int rows);

double squared_euclidean_distance(const double* a, const double* b, int d);

double** compute_similarity_matrix(double** data, int n, int d);
double* compute_degree_matrix(double** similarity, int n);
double** compute_normalized_similarity(double** similarity, int n);

double** perform_symnmf(double** W, double** H, int n, int k);

#endif /*SYMNMF_H*/ 
