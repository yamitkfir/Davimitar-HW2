#include <Python.h> /* its OK that it doesnt compile in editor, he's gay */
#define PY_SSIZE_T_CLEAN
#define MAX_ITER 1000
#include "kmeansOG.h"
#include <math.h>

/* Function Declarations: */
PyMODINIT_FUNC PyInit_kmeansmodule(void);
static PyObject* fit(PyObject *self, PyObject* args);
static vector* list_to_vector(PyObject* list);
static PyObject* vector_to_pyList(vector* vec);

static PyObject* fit(PyObject *self, PyObject* args){
    int clusters_num;
    double epsilon;
    int iter = MAX_ITER;
    int dimension;
    PyObject* transferred_points;
    PyObject* transferred_clusters;
    PyObject* output;
    vector* final_clusters;

    if(!PyArg_ParseTuple(args, "idiiOO", &clusters_num, &epsilon, &iter, &dimension, &transferred_points, &transferred_clusters)){
        return NULL; /* a NULL value is never valid so it's used to signal an error has occurred. */
    }

    vector *points = list_to_vector(transferred_points);
    vector *clusters_initially = list_to_vector(transferred_clusters);
    final_clusters = kmeans_general(clusters_num, iter, epsilon, points, clusters_initially, dimension);
    output = vector_to_pyList(final_clusters);
    free_vector(final_clusters);

    return Py_BuildValue("O", output); /*  Py_BuildValue(...) returns a PyObject */
}   

static vector* list_to_vector(PyObject* list) {
    vector* head = NULL;
    vector** current_vector = &head;
    cord** current_cord;
    Py_ssize_t num_items = PyList_Size(list);

    for (Py_ssize_t i = 0; i < num_items; i++) {
        PyObject* item = PyList_GetItem(list, i);
        *current_vector = calloc(1, sizeof(vector));
        current_cord = &((*current_vector)->cords);
        
        for(Py_ssize_t j = 0; j < PyList_Size(item); j++) {
            *current_cord = calloc(1, sizeof(cord));
            (*current_cord)->value = PyFloat_AsDouble(PyList_GetItem(item, j));
            current_cord = &((*current_cord)->next);
        }
        current_vector = &((*current_vector)->next);
   }
   return head;
}

static PyObject* vector_to_pyList(vector* vec) {
    PyObject* list = PyList_New(0);
    while (vec) {
        PyObject* inner_list = PyList_New(0);
        cord* cord = vec->cords;
        while (cord) {
            PyList_Append(inner_list, PyFloat_FromDouble(cord->value));
            cord = cord->next;
        }
        PyList_Append(list, inner_list);
        vec = vec->next;
    }
    return list;
}

static PyMethodDef kmeansMethods[] = {
    {"fit",             /* the Python method name that will be used */
    (PyCFunction)fit,   /* the C-function that implements the Python function and returns static PyObject*  */
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