#include "symnmf.h"
#include <Python.h>

/* Function declarations for Python module*/
static PyObject* py_sym(PyObject* self, PyObject* args);
static PyObject* py_ddg(PyObject* self, PyObject* args);
static PyObject* py_norm(PyObject* self, PyObject* args);
static PyObject* py_symnmf(PyObject* self, PyObject* args);

/* Method table for Python module*/
static PyMethodDef SymNmfMethods[] = {
    {"sym", py_sym, METH_VARARGS, "Calculate the similarity matrix."},
    {"ddg", py_ddg, METH_VARARGS, "Calculate the diagonal degree matrix."},
    {"norm", py_norm, METH_VARARGS, "Calculate the normalized similarity matrix."},
    {"symnmf", py_symnmf, METH_VARARGS, "Perform symmetric Non-negative Matrix Factorization."},
    {NULL, NULL, 0, NULL}  /* Sentinel*/
};

/* Module definition*/
static struct PyModuleDef symnmfmodule = {
    PyModuleDef_HEAD_INIT,
    "symnmf",
    "Python interface for Symmetric Non-negative Matrix Factorization.",
    -1,
    SymNmfMethods
};

/* Initialize module*/
PyMODINIT_FUNC PyInit_symnmf(void) {
    return PyModule_Create(&symnmfmodule);
}

/* Implementation of py_sym*/
static PyObject* py_sym(PyObject* self, PyObject* args) {
    PyObject* py_data;
    double** data;
    double** similarity_matrix;
    int n, d;
    PyObject* result;
    if (!PyArg_ParseTuple(args, "O", &py_data)) {
        PyErr_SetString(PyExc_ValueError, "Invalid input");
        return NULL;
    }

    
    if (!parse_input(py_data, &data, &n, &d)) {
        PyErr_SetString(PyExc_ValueError, "Failed to parse input data");
        return NULL;
    }

    similarity_matrix = compute_similarity_matrix(data, n, d);
    if (!similarity_matrix) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to compute similarity matrix");
        return NULL;
    }

    result = convert_matrix_to_pylist(similarity_matrix, n, n);
    free_matrix(data, n);
    free_matrix(similarity_matrix, n);

    return result;
}

/* Implementation of py_ddg*/
static PyObject* py_ddg(PyObject* self, PyObject* args) {
    PyObject* py_data;
    double** data;
    int n, d;
    double* degree_matrix;
    PyObject* result;
    if (!PyArg_ParseTuple(args, "O", &py_data)) {
        PyErr_SetString(PyExc_ValueError, "Invalid input");
        return NULL;
    }

    
    if (!parse_input(py_data, &data, &n, &d)) {
        PyErr_SetString(PyExc_ValueError, "Failed to parse input data");
        return NULL;
    }

    degree_matrix = compute_degree_matrix(data, n); /* d was also the third argument*/
    if (!degree_matrix) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to compute degree matrix");
        return NULL;
    }

    result = convert_vector_to_pylist(degree_matrix, n);
    free_matrix(data, n);
    free(degree_matrix);

    return result;
}

/* Implementation of py_norm */
static PyObject* py_norm(PyObject* self, PyObject* args) {
    PyObject* py_data;
    PyObject* result;
    double** norm_matrix;
    double** data;
    int n, d;
    if (!PyArg_ParseTuple(args, "O", &py_data)) {
        PyErr_SetString(PyExc_ValueError, "Invalid input");
        return NULL;
    }

   
    if (!parse_input(py_data, &data, &n, &d)) {
        PyErr_SetString(PyExc_ValueError, "Failed to parse input data");
        return NULL;
    }

    norm_matrix = compute_normalized_similarity(data, n); /* d was also the third argument*/
    if (!norm_matrix) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to compute normalized similarity matrix");
        return NULL;
    }

    result = convert_matrix_to_pylist(norm_matrix, n, n);
    free_matrix(data, n);
    free_matrix(norm_matrix, n);

    return result;
}

/* Implementation of py_symnmf*/
static PyObject* py_symnmf(PyObject* self, PyObject* args) {
    PyObject *py_W, *py_H_init;
    int k;
    PyObject* result;
    double** W;
    double** H_init;
    int n;
    double** H_final;

    if (!PyArg_ParseTuple(args, "OOi", &py_W, &py_H_init, &k)) {
        PyErr_SetString(PyExc_ValueError, "Invalid input");
        return NULL;
    }

   

    if (!parse_input(py_W, &W, &n, &n) || !parse_input(py_H_init, &H_init, &n, &k)) {
        PyErr_SetString(PyExc_ValueError, "Failed to parse input matrices");
        return NULL;
    }

    H_final = perform_symnmf(W, H_init, n, k);
    if (!H_final) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to perform SymNMF");
        return NULL;
    }

    result = convert_matrix_to_pylist(H_final, n, k);
    free_matrix(W, n);
    free_matrix(H_init, n);
    free_matrix(H_final, n);

    return result;
}
