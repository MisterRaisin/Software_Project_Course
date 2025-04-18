#define _GNU_SOURCE
#include "symnmf.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

/*Error message helper. Params: quit-whether to also exit. Ret: None*/
void printError(char quit){
    printf("An Error Has Occurred\n");
    if (quit)
        exit(1);
}

/* Helper function to allocate a 2D array. Params: rows&cols - size. Ret: matrix, NULL on failure.*/
double** allocate_matrix(int rows, int cols) {
    int i, j;
    double** matrix = (double**)malloc(rows * sizeof(double*));

    if (!matrix) return NULL;
    for (i = 0; i < rows; i++) {
        matrix[i] = (double*)malloc(cols * sizeof(double));
        if (!matrix[i]) {
            for (j = 0; j < i; j++) 
                free(matrix[j]); /* Free the previous rows*/
            free(matrix);
            return NULL;
        }
    }
    return matrix;
}

/* Helper function to free a 2D array. Params: matrix - the matrix to free, rows - the num of rows. Ret: None.*/
void free_matrix(double** matrix, int rows) {
    if(!matrix) return; /* Check if matrix is NULL*/
    int i;
    for (i = 0; i < rows; i++)
        free(matrix[i]);
    
    free(matrix);
}

/* Helper function to compute the squared Euclidean distance.
Params: a - first array, b - second array,d - dimension. Ret: distance.*/
double squared_euclidean_distance(const double* a, const double* b, int d) {
    double diff, sum = 0;
    int i;
    
    for (i = 0; i < d; i++) {
        diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sum;
}

/* Compute the similarity matrix. Params: data - input matrix,
n - number of vectors, d - dimension. Ret: similarity matrix, NULL on failure.*/
double** compute_similarity_matrix(double** data, int n, int d) {
    int i, j;
    double** similarity = allocate_matrix(n, n);
    if (!similarity) return NULL;

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (i == j) 
                similarity[i][j] = 0.0;
            else {
                double dist = squared_euclidean_distance(data[i], data[j], d);
                similarity[i][j] = exp(-dist/2);
            }
        }
    }
    return similarity;
}

/* Compute the diagonal degree matrix. Params: similarity - the similarity matrix,
n - size of matrix. Ret: the eigenvalues (diagonal values), NULL on failure.*/
double* compute_degree_array(double** similarity, int n) {
    double* degrees = (double*)malloc(n * sizeof(double));
    int i, j;
    if (!degrees) return NULL;

    for (i = 0; i < n; i++) {
        degrees[i] = 0.0;
        for (j = 0; j < n; j++)
            degrees[i] += similarity[i][j];
    }

    return degrees;
}

/* Compute the normalized similarity matrix. Params: similarity - the similarity matrix,
n - size of matrix. Ret: the normalized similarity matrix (diagonal values), NULL on failure.*/
double** compute_normalized_similarity(double** similarity, int n) {
    double* degrees = compute_degree_array(similarity, n);
    int i, j;
    double** normalized = allocate_matrix(n, n);
    
    if (!degrees || !normalized) {
        if(normalized) free_matrix(normalized, n);
        if(degrees) free(degrees);
        return NULL;
    }

    for (i = 0; i < n; i++) {
        for (j = 0; j < n; j++) {
            if (degrees[i] > 0 && degrees[j] > 0)
                normalized[i][j] = similarity[i][j] / sqrt(degrees[i] * degrees[j]);
            else
                normalized[i][j] = 0.0;
        }
    }

    free(degrees);
    return normalized;
}

/* Computes the product of matrix A and its transposed A^t (In place). Params: A - the matrix,
AxAt - matrix to hold the result, n - size of matrix. Ret: NONE.*/
void multiply_matrix_by_its_transposed(double** A, double** AxAt, int n, int k) {
    int i, j, l;
    double sum;

    for (i = 0; i < n; i++) { /* Compute Matrix H*H^t for calculating the denominator*/
        for (j = 0; j < n; j++) {
            sum = 0;
            for (l = 0; l < k; l++) 
                sum += A[i][l] * A[j][l]; /* not l then j because we want to multiply by the transposed matrix*/
            AxAt[i][j] = sum;
        }
    }
}

/* Perform the SymNMF algorithm. Params: W - normalized similarity matrix, H - staring matrix,
n - rows (number of vectors), k - cols (dimension). Ret: the final SymNMF H matrix, NULL on failure.*/
double** perform_symnmf(double** W, double** H, int n, int k) {
    const double epsilon = 1e-4, betta = 0.5; /* betta from the given formula */
    const int max_iter = 300;
    double diff, numerator, denominator; /* helper temp variables*/
    int i, j, l, iter; /* iterators */
    double **temp, **HxHt = allocate_matrix(n, n), **H_new = allocate_matrix(n, k);
    if (!H_new || !HxHt || n==0) {
        if (HxHt) free_matrix(HxHt, n);
        if (H_new) free_matrix(H_new, n);
        return n==0 ? H : NULL;
    }
    for (iter = 0; iter < max_iter; iter++) {
        diff = 0;
        multiply_matrix_by_its_transposed(H, HxHt, n, k); /* H*H^t */
        for (i = 0; i < n; i++) {
            for (j = 0; j < k; j++) {
                numerator = 0;
                denominator = 0;
                for (l = 0; l < n; l++) {
                    numerator += W[i][l] * H[l][j]; /* W*H */
                    denominator += HxHt[i][l] * H[l][j]; /* (H*H^t)*H */
                }
                H_new[i][j] = H[i][j] * (1 - betta + betta*numerator/denominator); /* formula */
                diff += (H_new[i][j] - H[i][j]) * (H_new[i][j] - H[i][j]); /* squared Frobenius partial sum*/
            }
        }
        temp = H; /* Swap H and H_new (We dont want to lose the allocated space)*/
        H = H_new;
        H_new = temp;
        if (diff < epsilon) break; /* Check convergence*/
    }
    free_matrix(H_new, n);
    free_matrix(HxHt, n);
    return H;
}

/*Function to print the matrix. Params: matrix - the matrix, rows&cols - size. Ret: None.*/
void print_matrix(double** matrix, int rows, int cols) {
    int i, j;

    for (i = 0; i < rows; i++) {
        for (j = 0; j < cols-1; j++) { /* N-1 because we dont want to have a comma after the last element*/
            printf("%.4f,", matrix[i][j]);
        }
        printf("%.4f", matrix[i][cols-1]); /* Print the last element without a comma*/
        printf("\n");
    }
}

/*Function to print the diagonal matrix. Params: ei_values - the diagonal values,
N - matrix size. Ret: None.*/
void print_diagonal_matrix(double* ei_values, int N) {
    int i, j;
    
    for (i = 0; i < N; i++) {
        for (j = 0; j < N-1; j++) { /* N-1 because we dont want to have a comma after the last element*/
            if (i == j)
                printf("%.4f,", ei_values[i]);
            else
                printf("0.0000,");
        }
        if (i == N-1) /* Print the last element without a comma*/
            printf("%.4f", ei_values[i]);
        else  
            printf("0.0000");
        
        printf("\n");
    }
}

/* Function to find the dimensions of the vectors and their count (rows and cols of matrix).
Params: filename - path to file, rows&cols will have sizes at end of func. Ret: None. EXITS: on failure.*/
int get_matrix_dimensions(const char *filename, int *rows, int *cols) { 
    FILE *file = fopen(filename, "r"); /* Open the file for reading*/
    char *line = NULL;
    size_t len = 0;
    int first_row_cols = -1; /* Stores the column count of the first row to check consistency*/
    *rows = *cols = 0;

    if (!file)
        return 1;

    while (getline(&line, &len, file) != -1) { /* Read file line by line*/
        int current_cols = 0; /* Count columns in the current row*/
        char *tmp = strdup(line); /* Create a copy of the line for tokenization*/
        char *token = strtok(tmp, ",\n"); /* Split line by comma or newline*/
        
        while (token) { /* Count the number of columns in the row*/
            current_cols++;
            token = strtok(NULL, ",\n");
        }
        free(tmp); 
        
        if (first_row_cols == -1) { 
            first_row_cols = current_cols;
            *cols = first_row_cols;
        } else if (current_cols != first_row_cols){ /* Ensure all rows have the same column count*/
            free(line);
            fclose(file);
            return 1;
        }
        (*rows)++; /* Increment row count*/
    }

    free(line);
    fclose(file); 
    if (*rows == 0 || *cols == 0) return 1; /*(1 is error)*/
    return 0;
}

/* Function to read the txt file into a dynamically allocated 2D array.
Params: filename - path to file, rows&cols - size. Ret: None, NULL on failure.*/
double **read_matrix(const char *filename, int rows, int cols) {
    char *line = NULL;
    size_t len = 0;
    int row = 0;
    double **matrix;
    FILE *file = fopen(filename, "r"); 
    if (!file) return NULL;
    matrix = allocate_matrix(rows, cols);
    if (!matrix) return NULL;
    
    while (getline(&line, &len, file) != -1 && row < rows) {
        char *token = strtok(line, ",\n");
        int col = 0;
        while (token && col < cols) {
            char *endptr;
            double value = strtod(token, &endptr);

            /* Check if token is a valid number */
            if (*endptr != '\0' && *endptr != '\n') {
                free(line);
                free_matrix(matrix, rows);
                fclose(file);
                return NULL;
            }
            matrix[row][col] = value;
            token = strtok(NULL, ",\n");
            col++;
        }
        if (col != cols) {
            free(line);
            free_matrix(matrix, rows);
            fclose(file);
            return NULL;
        }
        row++; 
    }

    free(line); 
    fclose(file);  
    return matrix; 
}

/*Main function to implement the required functionality.
Params: Cmd rgs. Ret: status code.*/
int main(int argc, char **argv) {
    char* goal, *fileName;
    int N = 0, d = 0;
    double** matrix,**similarity,**normalized;
    double* degreeArray;
    if (argc != 3) printError(1); /* 1 for quitting */
    goal = argv[1];
    fileName = argv[2];

    if (get_matrix_dimensions(fileName, &N, &d)) printError(1); /* 1 for quitting*/

    matrix = read_matrix(fileName,N,d); 
    if (matrix == NULL) printError(1); /* 1 for quitting*/

    similarity = compute_similarity_matrix(matrix, N, d);
    free_matrix(matrix,N);
    if (similarity == NULL) printError(1); /* 1 for quitting*/

    if (!strcmp(goal, "sym")) 
        print_matrix(similarity, N, N);
    else if (!strcmp(goal, "ddg")) {
        degreeArray = compute_degree_array(similarity, N);
        if (degreeArray == NULL) printError(0);
        else {
            print_diagonal_matrix(degreeArray, N);
            free(degreeArray);
        }
    } else if (!strcmp(goal, "norm")) {
        normalized = compute_normalized_similarity(similarity, N);
        if (normalized == NULL) printError(0);
        else {
            print_matrix(normalized, N,N);
            free_matrix(normalized,N);
        }
    } else printError(0);
    free_matrix(similarity,N);
    return 0;
}
