#include <Python.h> /* its OK that it doesnt compile in editor, he's gay */
#define PY_SSIZE_T_CLEAN
#define MAX_ITER 1000
#include "kmeansOG.h"
#include <math.h>

/* Function Declarations: */
static vector* transferred_to_vectors(char* transferred); 
PyMODINIT_FUNC PyInit_kmeansmodule(void);
static PyObject* kmeans_capi(PyObject *self, PyObject* args);

static PyObject* kmeans_capi(PyObject *self, PyObject* args){
    int clusters_num;
    double epsilon;
    int iter = MAX_ITER;
    int dimension;
    char* transferred_points;
    char* transferred_clusters;
    char* output;

    if(!PyArg_ParseTuple(args, "idiiss", &clusters_num, &epsilon, &iter, &dimension, &transferred_points, &transferred_clusters)){
        return NULL; /* a NULL value is never valid so it's used to signal an error has occurred. */
    }

    vector *points = transferred_to_vectors(transferred_points);
    vector *clusters_initially = transferred_to_vectors(transferred_clusters);
    output = kmeans_general(clusters_num, iter, epsilon, points, clusters_initially, dimension);

    /* TODO YAMIT return string after packing */
    return Py_BuildValue("s", output); /*  Py_BuildValue(...) returns a PyObject */
}   

static vector* transferred_to_vectors(char* transferred){
    return NULL; 
    /* TODO DAVID - decide mindfully on a format for strings and convert in a way that can be unpacked later*/
}

static PyMethodDef kmeansMethods[] = {
    {"kmeans_capi",             /* the Python method name that will be used */
    (PyCFunction)kmeans_capi,   /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,
      PyDoc_STR("Returns Kmeans of given points, based on instructions.")},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef kmeansmodule = {
    PyModuleDef_HEAD_INIT,
    "kmeansmodule", /* name of module */
    NULL,           /* module documentation, may be NULL */
    -1,             /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    kmeansMethods   /* the PyMethodDef array from before containing the methods of the extension */
};

PyMODINIT_FUNC PyInit_kmeansmodule(void)
{
    PyObject *m;
    m = PyModule_Create(&kmeansmodule);
    if (!m) {
        return NULL;
    }
    return m;
}