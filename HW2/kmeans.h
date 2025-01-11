#include <stdlib.h>
#ifndef KMEANS_H
#define KMEANS_H

PyObject* kmeans_c(double **vectors, double **clusters, int  k, int maxIter, double eps, int dim, int vector_count);

#endif