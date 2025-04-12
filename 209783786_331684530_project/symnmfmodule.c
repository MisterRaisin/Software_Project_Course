#define PY_SSIZE_T_CLEAN
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include "symnmf.h"
#include <Python.h>
#include <numpy/arrayobject.h>

/* Function declarations for Python module*/
static PyObject* py_sym(PyObject* self, PyObject* args);
static PyObject* py_ddg(PyObject* self, PyObject* args);
static PyObject* py_norm(PyObject* self, PyObject* args);
static PyObject* py_symnmf(PyObject* self, PyObject* args);

/* Method table for Python module*/
static PyMethodDef SymnmfMethods[] = {
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
    SymnmfMethods
};

/* Initialize module*/
PyMODINIT_FUNC PyInit_symnmf(void) {
    import_array();
    return PyModule_Create(&symnmfmodule);
}

/* Function for converting the given NumPy array into a workable 2d double array. Params: np_arr - python nparray object. Ret: NULL on failure.*/
double** numpy_to_double_array(PyArrayObject* np_arr) {
    int rows = (int) PyArray_DIM(np_arr, 0); /* Get dims*/
    int cols = (int) PyArray_DIM(np_arr, 1);

    double** c_array = (double**) malloc(rows * sizeof(double*)); /* Allocate memory for row arrays*/
    if (!c_array) return NULL;

    for (int i = 0; i < rows; i++) { /* For each row*/
        c_array[i] = (double*) malloc(cols * sizeof(double)); /* Allocate*/
        if (!c_array[i]) {
            for (int j = 0; j < i; j++) free(c_array[j]);
            free(c_array);
            return NULL;
        }
        
        for (int j = 0; j < cols; j++)
            c_array[i][j] = *(double*)PyArray_GETPTR2(np_arr, i, j); /* Copy data */
    }
    return c_array;
}

/* Function for expanding the vector of diagonal values to a full matrix. Params: degreeVector - array of degrees, n - size. Ret: full matrix, NULL on failure.*/
double ** expand_degree_vector_to_matrix(double* degreeVector, int n){
    double** degreeMatrix = allocate_matrix(n,n);
    if (!degreeMatrix) {
        PyErr_SetString(PyExc_RuntimeError, "Memory allocation failed.");
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            degreeMatrix[i][j] = i == j ? degreeVector[i] : 0.0; /* Value if on diagonal, 0 if not*/
    }
    return degreeMatrix;
}

int convert_arg_ndarray_to_matrix(PyObject* args, double*** out_data, int* out_rows, int* out_cols) {
    PyArrayObject* input_array;

    if (!PyArg_ParseTuple(args, "O!", &PyArray_Type, &input_array)) {/* Parse Python arguments */
        PyErr_SetString(PyExc_TypeError, "Parsing Python arguments failed.");
        return 0; /* 0 means error */
    }

    if (PyArray_NDIM(input_array) != 2 || PyArray_TYPE(input_array) != NPY_FLOAT64) { /* Ensure it's a 2D float64 array */
        PyErr_SetString(PyExc_TypeError, "Arguments incorrect type.");
        return 0; /* 0 means error */
    }


    int rows = (int) PyArray_DIM(input_array, 0); /* Get dims*/
    int cols = (int) PyArray_DIM(input_array, 1);

    double** data = numpy_to_double_array(input_array);
    if (!data) {
        PyErr_SetString(PyExc_RuntimeError, "Memory allocation failed in numpy_to_double_array.");
        return 0; /* 0 means error */
    }

    *out_data = data;
    
    *out_rows = rows;
    *out_cols = cols;
    return 1;  /* 1 means success */
}

/* Function for packaging the 2d double array to a NumPy array. Params: data - matrix. rows&cols - size. Ret - the packaged NumPy object.*/
static PyObject* packageArray(double** data, int rows, int cols) {
    npy_intp dims[2] = {rows, cols}; /*Init dimensions*/
    PyArrayObject* output_array = (PyArrayObject*) PyArray_SimpleNew(2, dims, NPY_FLOAT64); /*Initialize output array*/

    for (int i = 0; i < rows; i++) { /*Copy array*/
        for (int j = 0; j < cols; j++) {
            *(double*)PyArray_GETPTR2(output_array, i, j) = data[i][j];
        }
    }
    return PyArray_Return(output_array);
}

/* Bridge to sym function. Ret : NULL on failure (Will raise a python error)*/
static PyObject* py_sym(PyObject* self, PyObject* args) {
    int rows, cols;
    double** data;
    if (!convert_arg_ndarray_to_matrix(args, &data, &rows, &cols))
        return NULL;

    double** similarity = compute_similarity_matrix(data, rows,cols);
    free_matrix(data, rows); /* No need for it any more */
    if (!similarity) {
        PyErr_SetString(PyExc_RuntimeError, "Memory allocation failed for similarity matrix.");
        return NULL;
    }

    PyObject* packagedResult = packageArray(similarity, rows, rows);

    free_matrix(similarity, rows);

    return packagedResult;
}

/* Bridge to dgg function. Ret : NULL on failure (Will raise a python error)*/
static PyObject* py_ddg(PyObject* self, PyObject* args) {
    int rows, cols;
    double** data;
    if (!convert_arg_ndarray_to_matrix(args, &data, &rows, &cols))
        return NULL;
    
    double** similarity = compute_similarity_matrix(data, rows,cols);
    free_matrix(data, rows); /* No need for it any more */
    if (!similarity) {
        PyErr_SetString(PyExc_RuntimeError, "Memory allocation failed for similarity matrix.");
        return NULL;
    }

    double* dgg = compute_degree_array(similarity, rows);
    free_matrix(similarity, rows); /* No need for it any more */
    if (!dgg) return NULL;

    double** expandedDegreeMatrix = expand_degree_vector_to_matrix(dgg,rows); 
    free(dgg); /* No need for it any more */
    if (!expandedDegreeMatrix) {
        PyErr_SetString(PyExc_RuntimeError, "Memory allocation failed for diagonal matrix.");
        return NULL;
    }

    PyObject* packagedResult = packageArray(expandedDegreeMatrix, rows, rows);

    free_matrix(expandedDegreeMatrix, rows);

    return packagedResult;
}

/* Bridge to norm sym function. Ret : NULL on failure (Will raise a python error)*/
static PyObject* py_norm(PyObject* self, PyObject* args) {
    int rows, cols;
    double** data;
    if (!convert_arg_ndarray_to_matrix(args, &data, &rows, &cols))
        return NULL;

    double** similarity = compute_similarity_matrix(data, rows,cols);
    free_matrix(data, rows); /* No need for it any more */
    if (!similarity) {
        PyErr_SetString(PyExc_RuntimeError, "Memory allocation failed for similarity matrix.");
        return NULL;
    }

    double** normsym = compute_normalized_similarity(similarity, rows);
    free_matrix(similarity, rows); /* No need for it any more */
    if (!normsym) {
        PyErr_SetString(PyExc_RuntimeError, "Memory allocation failed for normalized similarity matrix.");
        return NULL;
    }

    PyObject* packagedResult = packageArray(normsym, rows, rows);

    free_matrix(normsym, rows);

    return packagedResult;
}

/* Bridge to nsymnmf function. Ret : NULL on failure (Will raise a python error)*/
static PyObject* py_symnmf(PyObject* self, PyObject* args) {
    PyArrayObject *array1, *array2;
    if (!PyArg_ParseTuple(args, "O!O!", &PyArray_Type, &array1, &PyArray_Type, &array2)) {/* Parse Python arguments */
        PyErr_SetString(PyExc_TypeError, "Parsing Python arguments failed.");
        return NULL;
    }

    if (PyArray_NDIM(array1) != 2 || PyArray_TYPE(array1) != NPY_FLOAT64 ||    /* Ensure both are 2D float64 arrays */
        PyArray_NDIM(array2) != 2 || PyArray_TYPE(array2) != NPY_FLOAT64) {
            PyErr_SetString(PyExc_TypeError, "Arguments incorrect type.");
            return NULL;
    }

    int rows = (int) PyArray_DIM(array1, 0); /* N from W */
    int cols = (int) PyArray_DIM(array2, 1); /* k from H */

    double** c_array1 = numpy_to_double_array(array1); /* Convert NumPy arrays to C double** */
    double** c_array2 = numpy_to_double_array(array2);
    if (!c_array1 || !c_array2) {
        if (c_array1) free_matrix(c_array1, rows);
        if (c_array2) free_matrix(c_array2, rows);
        PyErr_SetString(PyExc_RuntimeError, "Memory allocation failed.");
        return NULL;
    }
    
    /* Process the arrays */
    double** result = perform_symnmf(c_array1, c_array2, rows, cols);
    free_matrix(c_array1, rows); /* No need for it any more c_array2 was freed in perform symnmf*/
    if (!result) {
        PyErr_SetString(PyExc_RuntimeError, "perform_symnmf failed");
        return NULL;
    }

    PyObject* packagedResult = packageArray(result, rows, cols);    /* Create NumPy array for output (double -> float64) */
    
    free_matrix(result, rows);

    return packagedResult;
}