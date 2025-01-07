#define PY_SSIZE_T_CLEAN
#define MAX_ITER 1000
#include "kmeansOG.h"
#include <Python.h> /* its OK that it doesnt compile in editor, he's gay */

/* Function Declarations: */
static PyObject* fit(PyObject *self, PyObject *args);
static PyMethodDef geoMethods[];
/* TODO declare all other functions */


static PyObject* fit(PyObject *self, PyObject *args)
{
    int clusters_num;
    double epsilon;
    int iter = MAX_ITER;
    int dimension;

    /* TODO other args according to instructor's answer on file-transfers */
    if(!PyArg_ParseTuple(args, "idi", &clusters_num, &epsilon, &iter)) {
        return NULL; /* a NULL value is never valid so it's used to signal an error has occurred. */
    }

    vector *points = init_all_points();
    vector *clusters_initially = init_clusters();
    // TODO SAAR: update, among others, the "dimension" field bc it's sent to kmeans_general:)
    kmeans_general(clusters_num, iter, epsilon, points, clusters_initially, dimension);
    /* now */

    /* TODO return args according to instructor's answer on file-transfers */
    return Py_BuildValue("_", ); /*  Py_BuildValue(...) returns a PyObject */
}

static init_all_points(){
    /* TODO according to ...*/
}

static init_clusters(){
    /* TODO according to ...*/
}

static PyMethodDef kmeansMethods[] = {
    {"fit",                     /* the Python method name that will be used */
    (PyCFunction)fit, /* the C-function that implements the Python function and returns static PyObject*  */
      METH_VARARGS,
      PyDoc_STR("Returns Kmeans of given points, based on instructions.")},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef kmeansmodule = {
    PyModuleDef_HEAD_INIT,
    "kmeansmodule", /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,  /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    kmeansMethods /* the PyMethodDef array from before containing the methods of the extension */
};

PyMODINIT_FUNC PyInit_geo_capi(void)
{
    PyObject *m;
    m = PyModule_Create(&kmeansmodule);
    if (!m) {
        return NULL;
    }
    return m;
}