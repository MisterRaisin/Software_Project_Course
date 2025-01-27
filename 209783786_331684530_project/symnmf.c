#include "symnmf.h"
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

// Helper function to allocate a 2D array
double** allocate_matrix(int rows, int cols) {
    double** matrix = (double**)malloc(rows * sizeof(double*));
    if (!matrix) return NULL;

    for (int i = 0; i < rows; i++) {
        matrix[i] = (double*)malloc(cols * sizeof(double));
        if (!matrix[i]) {
            for (int j = 0; j < i; j++) free(matrix[j]);
            free(matrix);
            return NULL;
        }
    }
    return matrix;
}

// Helper function to free a 2D array
void free_matrix(double** matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

// Helper function to compute the squared Euclidean distance
double squared_euclidean_distance(const double* a, const double* b, int d) {
    double sum = 0.0;
    for (int i = 0; i < d; i++) {
        double diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sum;
}

// Compute the similarity matrix
double** compute_similarity_matrix(double** data, int n, int d) {
    double** similarity = allocate_matrix(n, n);
    if (!similarity) return NULL;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) {
                similarity[i][j] = 0.0;
            } else {
                double dist = squared_euclidean_distance(data[i], data[j], d);
                similarity[i][j] = exp(-dist);
            }
        }
    }
    return similarity;
}

// Compute the diagonal degree matrix
double* compute_degree_matrix(double** similarity, int n) {
    double* degrees = (double*)malloc(n * sizeof(double));
    if (!degrees) return NULL;

    for (int i = 0; i < n; i++) {
        degrees[i] = 0.0;
        for (int j = 0; j < n; j++) {
            degrees[i] += similarity[i][j];
        }
    }
    return degrees;
}

// Compute the normalized similarity matrix
double** compute_normalized_similarity(double** similarity, int n) {
    double* degrees = compute_degree_matrix(similarity, n);
    if (!degrees) return NULL;

    double** normalized = allocate_matrix(n, n);
    if (!normalized) {
        free(degrees);
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (degrees[i] > 0 && degrees[j] > 0) {
                normalized[i][j] = similarity[i][j] / sqrt(degrees[i] * degrees[j]);
            } else {
                normalized[i][j] = 0.0;
            }
        }
    }

    free(degrees);
    return normalized;
}

// Perform the SymNMF algorithm
double** perform_symnmf(double** W, double** H, int n, int k) {
    const double epsilon = 1e-4;
    const int max_iter = 300;
    double diff;

    double** H_new = allocate_matrix(n, k);
    if (!H_new) return NULL;

    for (int iter = 0; iter < max_iter; iter++) {
        diff = 0.0;

        for (int i = 0; i < n; i++) {
            for (int j = 0; j < k; j++) {
                double numerator = 0.0;
                double denominator = 0.0;

                for (int l = 0; l < n; l++) {
                    numerator += W[i][l] * H[l][j];
                    denominator += H[i][l] * H[l][j];
                }

                double update = H[i][j] * (numerator / (denominator + epsilon));
                diff += fabs(update - H[i][j]);
                H_new[i][j] = update;
            }
        }

        // Check convergence
        if (diff < epsilon) break;

        // Swap H and H_new
        double** temp = H;
        H = H_new;
        H_new = temp;
    }

    free_matrix(H_new, n);
    return H;
}
