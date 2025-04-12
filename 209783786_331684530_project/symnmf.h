#ifndef SYMNMF_H
#define SYMNMF_H

void printError(char quit);

double** allocate_matrix(int rows, int cols);
void free_matrix(double** matrix, int rows);

double squared_euclidean_distance(const double* a, const double* b, int d);

double** compute_similarity_matrix(double** data, int n, int d);
double* compute_degree_array(double** similarity, int n);
double** compute_normalized_similarity(double** similarity, int n);

double** perform_symnmf(double** W, double** H, int n, int k);
void multiply_matrix_by_its_transposed(double** A,double** AxAt,int n,int k);

void print_matrix(double** matrix, int row,int cols);
double **read_matrix(const char *filename, int rows, int cols);
int get_matrix_dimensions(const char *filename, int *rows, int *cols);


#endif
