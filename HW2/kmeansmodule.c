#include <stdlib.h>
#include <stdio.h>

#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "kmeans.h"

// Function to convert a Python list of lists to a C 2D double array
double** python_list_of_lists_to_double_array(PyObject* py_list, size_t* outer_size, size_t** inner_sizes) {
    if (!PyList_Check(py_list)) {
        printf("Error: The provided Python object is not a list.\n");
        return NULL;
    }

    *outer_size = PyList_Size(py_list);
    *inner_sizes = (size_t*)malloc((*outer_size) * sizeof(size_t));
    if (!(*inner_sizes)) {
        printf("Error: Memory allocation failed for inner_sizes.\n");
        return NULL;
    }

    double** c_array = (double**)malloc((*outer_size) * sizeof(double*));
    if (!c_array) {
        printf("Error: Memory allocation failed for c_array.\n");
        free(*inner_sizes);
        return NULL;
    }

    for (size_t i = 0; i < *outer_size; i++) {
        PyObject* inner_list = PyList_GetItem(py_list, i);
        if (!PyList_Check(inner_list)) {
            printf("Error: Element %zu is not a list.\n", i);
            free(*inner_sizes);
            free(c_array);
            return NULL;
        }

        (*inner_sizes)[i] = PyList_Size(inner_list);
        c_array[i] = (double*)malloc((*inner_sizes)[i] * sizeof(double));
        if (!c_array[i]) {
            printf("Error: Memory allocation failed for inner array %zu.\n", i);
            free(*inner_sizes);
            for (size_t j = 0; j < i; j++) {
                free(c_array[j]);
            }
            free(c_array);
            return NULL;
        }

        for (size_t j = 0; j < (*inner_sizes)[i]; j++) {
            PyObject* item = PyList_GetItem(inner_list, j);
            if (PyFloat_Check(item) || PyLong_Check(item)) {
                c_array[i][j] = PyFloat_AsDouble(item);
                if (PyErr_Occurred()) {
                    printf("Error: Failed to convert item at [%zu][%zu] to double.\n", i, j);
                    free(*inner_sizes);
                    for (size_t k = 0; k <= i; k++) {
                        free(c_array[k]);
                    }
                    free(c_array);
                    return NULL;
                }
            } else {
                printf("Error: Element [%zu][%zu] is not numeric.\n", i, j);
                free(*inner_sizes);
                for (size_t k = 0; k <= i; k++) {
                    free(c_array[k]);
                }
                free(c_array);
                return NULL;
            }
        }
    }

    return c_array;
}

// Function exposed to Python
static PyObject* fit(PyObject* self, PyObject* args) {
    PyObject *list1, *list2;
    int k, maxIter;
    double eps;

    // Parse arguments: two lists of lists followed by three integers
    if (!PyArg_ParseTuple(args, "OOiif", &list1, &list2, &k, &maxIter, &eps)) {
        return NULL;
    }

    // Convert the first list of lists to a C array
    size_t outer_size1;
    size_t* inner_sizes_1;
    double** array1 = python_list_of_lists_to_double_array(list1, &outer_size1, &inner_sizes_1);
    if (!array1) {
        PyErr_SetString(PyExc_ValueError, "Failed to convert first list of lists to C array.");
        return NULL;
    }
    int vector_amount = PyObject_Length(list1);
    // int dim = 7;
    int dim = PyObject_Length(PyList_GetItem(list1, 0));

    // Convert the second list of lists to a C array
    size_t outer_size2;
    size_t* inner_sizes2;
    double** array2 = python_list_of_lists_to_double_array(list2, &outer_size2, &inner_sizes2);
    if (!array2) {
        PyErr_SetString(PyExc_ValueError, "Failed to convert second list of lists to C array.");
        return NULL;
    }

    /*
    // Print data for demonstration
    printf("First List of Lists:\n");
    for (size_t i = 0; i < outer_size1; i++) {
        printf("[");
        for (size_t j = 0; j < inner_sizes_1[i]; j++) {
            printf("%0.4f", array1[i][j]);
            if (j < inner_sizes_1[i] - 1) printf(", ");
        }
        printf("]\n");
    }

    printf("Second List of Lists:\n");
    for (size_t i = 0; i < outer_size2; i++) {
        printf("[");
        for (size_t j = 0; j < inner_sizes2[i]; j++) {
            printf("%0.4f", array2[i][j]);
            if (j < inner_sizes2[i] - 1) printf(", ");
        }
        printf("]\n");
        free(array2[i]); // Free inner arrays
    }

     printf("Integers: %d, %d, %d, %f\n", k, maxIter, inner_sizes_1, eps);
     */

    PyObject* clusters = kmeans_c(array1, array2, k, maxIter, eps, dim, vector_amount);

    /* Free memory*/
    for (size_t i = 0; i < outer_size1; i++) free(array1[i]);
    free(array1);
    free(inner_sizes_1);

    for (size_t i = 0; i < outer_size2; i++) free(array2[i]);
    free(array2);
    free(inner_sizes2);

    

    return clusters; 
}

// Method definition table
static PyMethodDef kmeans_methods[] = {
    {"fit", fit, METH_VARARGS, "Recieves: vector_lst, cluster_lst, k, maxIter, eps and activates k-means algorithm"},
    {NULL, NULL, 0, NULL}
};

// Module definition
static struct PyModuleDef kmeans_module = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp",  // Module name
    "Module to apply k-means algorithm",
    -1,
    kmeans_methods
};

// Module initialization
PyMODINIT_FUNC PyInit_mykmeanssp(void) {
    PyObject *m;
    m = PyModule_Create(&kmeans_module);
    if (!m) {
        return NULL;
    }
    return m;
}
