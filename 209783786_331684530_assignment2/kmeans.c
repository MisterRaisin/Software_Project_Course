#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Python.h>


double getDistance(double a[],double b[],int dim);
void print_data(double **data, int rows, int cols);

double getDistance(double a[], double b[], int dim) {
    double sum = 0, diff;
    int i;
    for (i = 0; i < dim; i++) {
        diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

void print_data(double **data, int rows, int cols) {
    int i, j;
    for (i = 0; i < rows; i++)
    {
        for (j = 0; j < cols; j++) {
            printf("%.4f", data[i][j]);
                if (j < cols - 1) printf(",");
        }
        printf("\n");
    }
}


PyObject* kmeans_c(double **vectors, double **clusters, int k, int maxIter, double eps, int dim, int vector_count) {
    // print_data(vectors, vector_count, dim);
    // printf("\n\n");
    // print_data(clusters, k, dim);
    // printf("\n\n");

    
    int *clusterSizes; 
    double **sums, **prevClusters;
    int i, j, iter, converged = 0;

//print epsilon

    printf("c kmeans %lf",eps);

    /* Create previous clusters 2d array - k x dim */
    prevClusters = malloc(k * sizeof(double *));
    for (i = 0; i < k; i++) {
        prevClusters[i] = malloc(dim * sizeof(double));
    }

    /* Initiliaze sums 2d array - k x dim
       and clusterSize 1d array - k */
    sums = malloc(k * sizeof(double *));
    for (i = 0; i < k; i++) {
        sums[i] = malloc(dim * sizeof(double));
    }
    clusterSizes = calloc(k, sizeof(int));

    
    for (iter = 0; iter < maxIter && !converged; iter++) {
        /* Reset sums and sizes*/
        for (i = 0; i < k; i++) {
            for (j = 0; j < dim; j++) {
                sums[i][j] = 0;
            }
            clusterSizes[i] = 0;
        }

        /*Assign vectors to clusters*/
        for (i = 0; i < vector_count; i++) {
            int minCluster = 0;
            double minDist = getDistance(vectors[i], clusters[0], dim);

            for (j = 1; j < k; j++) {
                double dist = getDistance(vectors[i], clusters[j], dim);
                if (dist < minDist) {
                    minDist = dist;
                    minCluster = j;
                }
            }

            clusterSizes[minCluster]++;
            for (j = 0; j < dim; j++) {
                sums[minCluster][j] += vectors[i][j];
            }
        }

        converged = 1;
        /* Update clusters*/
        for (i = 0; i < k; i++) {
            for (j = 0; j < dim; j++) {
                prevClusters[i][j] = clusters[i][j];
                clusters[i][j] = clusterSizes[i] ? sums[i][j] / clusterSizes[i] : clusters[i][j];
            }

            if (getDistance(clusters[i], prevClusters[i], dim) > eps)
                converged = 0;
        }
    }

    /* Create return Python List */
    PyObject* py_list = PyList_New(k);
    if (!py_list) {
        return NULL; /* Return NULL on allocation failure */
    }

    // Populate the list
    for (i = 0; i < k; i++) {
        PyObject* vector = PyList_New(dim);
        if (!vector) {
            Py_DECREF(py_list);  /* Clean up the outer list on failure */
            return NULL;
        }
        for (j = 0; j < dim; j++) {
            PyList_SetItem(vector, j, PyFloat_FromDouble(clusters[i][j]));
        }
        
        PyList_SetItem(py_list, i, vector);
    }


    /* Print final clusters*/
    // print_data(clusters, k, dim);
    



    /* Free memory*/
    for (i = 0; i < k; i++) {
        free(sums[i]);
        free(prevClusters[i]);
    }
    free(sums);
    free(prevClusters);
    free(clusterSizes);

    return py_list; /* Return the Python list */
}