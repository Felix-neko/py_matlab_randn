#include <stdlib.h>
#include <stdio.h>
#include <Python.h>
#include <numpy/arrayobject.h>
#include "matlab_random.h"


#if PY_MAJOR_VERSION >= 3
  #define MOD_ERROR_VAL NULL
  #define MOD_SUCCESS_VAL(val) val
  #define MOD_INIT(name) PyMODINIT_FUNC PyInit_##name(void)
  #define MOD_DEF(ob, name, doc, methods) \
          static struct PyModuleDef moduledef = { \
            PyModuleDef_HEAD_INIT, name, doc, -1, methods, }; \
          ob = PyModule_Create(&moduledef);
#else
  #define MOD_ERROR_VAL
  #define MOD_SUCCESS_VAL(val)
  #define MOD_INIT(name) void init##name(void)
  #define MOD_DEF(ob, name, doc, methods) \
          ob = Py_InitModule3(name, methods, doc);
#endif


static char
module___doc__[] =
"Random number generator based on matlab rand";


typedef struct {
    PyObject_HEAD
    MatlabRandn mr;
} Generator;

static void Generator_dealloc(Generator *self)
{
    self->mr.~MatlabRandn();
    Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* Generator_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    Generator *self;
    self = (Generator*)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->mr = MatlabRandn();
    }
    return (PyObject*)self;
}
static PyObject* randn(Generator *self, PyObject *args, PyObject *kw)
{
    PyArrayObject* arr = NULL;
    PyObject *shape = NULL;
    static char *keywords[] = {"shape", NULL};
    NpyIter *arr_iter;
    NpyIter_IterNextFunc *arr_iternext;
    double **arr_dataptr;
    npy_intp *arr_dims;
    double *out;
    int tuple_size = 0;
    PyObject *tuple_item = NULL;
    long tuple_item_long = 0;
    long random_length = 1;
    int i;
    if (!PyArg_ParseTupleAndKeywords((PyObject*)args, kw, "O", keywords, &shape)) {
        return NULL;
    }
    if (PyLong_Check(shape)) {
        random_length = PyLong_AsLong(shape);
        arr_dims = (npy_intp*)malloc(sizeof(npy_intp));
        arr_dims[0] = random_length;
        tuple_size = 1;
    } else if (PyList_Check(shape)) {
        tuple_size = PyList_Size(shape);
        arr_dims = (npy_intp*)malloc(tuple_size*sizeof(npy_intp));
        for (i = 0; i < tuple_size; ++i) {
			tuple_item = PyList_GetItem(shape, i);
            if (PyLong_Check(tuple_item) || PyLong_Check(tuple_item)) {
                tuple_item_long = PyLong_AsLong(tuple_item);
                arr_dims[i] = tuple_item_long;
                random_length *= tuple_item_long;
            } else {
                return NULL;
            }
        }
    } else if (PyTuple_Check(shape)) {
        tuple_size = PyTuple_Size(shape);
        arr_dims = (npy_intp*)malloc(tuple_size*sizeof(npy_intp));
        for (i = 0; i < tuple_size; ++i) {
			tuple_item = PyTuple_GetItem(shape, i);
            if (PyLong_Check(tuple_item) || PyLong_Check(tuple_item)) {
                tuple_item_long = PyLong_AsLong(tuple_item);
                arr_dims[i] = tuple_item_long;
                random_length *= tuple_item_long;
            } else {
                return NULL;
            }
        }
    } else {
        PyErr_SetString(PyExc_ValueError, "randn takes only int, long, tuple or list as size parameter");
        return NULL;
    }
    arr = (PyArrayObject*)PyArray_SimpleNew(tuple_size, arr_dims, NPY_DOUBLE);
    out = new double[random_length];
    self->mr.randn(random_length, out);
    arr_iter = NpyIter_New(arr, NPY_ITER_READWRITE, NPY_FORTRANORDER,
                            NPY_NO_CASTING, NULL);
    if (arr_iter == NULL) {
        return NULL;
    }
    arr_iternext = NpyIter_GetIterNext(arr_iter, NULL);
    if (arr_iternext == NULL) {
        NpyIter_Deallocate(arr_iter);
        return NULL;
    }
    arr_dataptr = (double **) NpyIter_GetDataPtrArray(arr_iter);
    i = 0;
    do {
        **arr_dataptr = out[i++];
    } while(arr_iternext(arr_iter));
    delete[] out;
    NpyIter_Deallocate(arr_iter);
    //Py_INCREF(arr);
    free(arr_dims);
    return (PyObject*)arr;
}

static PyObject* random_sample(Generator *self, PyObject *args, PyObject *kw)
{
    PyArrayObject* arr = NULL;
    PyObject *shape = NULL;
    static char *keywords[] = {"shape", NULL};
    NpyIter *arr_iter;
    NpyIter_IterNextFunc *arr_iternext;
    double **arr_dataptr;
    npy_intp *arr_dims;
    double *out;
    int tuple_size = 0;
    PyObject *tuple_item = NULL;
    long tuple_item_long = 0;
    long random_length = 1;
    int i;
    if (!PyArg_ParseTupleAndKeywords((PyObject*)args, kw, "O", keywords, &shape)) {
        return NULL;
    }
    if (PyLong_Check(shape)) {
        random_length = PyLong_AsLong(shape);
        arr_dims = (npy_intp*)malloc(sizeof(npy_intp));
        arr_dims[0] = random_length;
        tuple_size = 1;
    } else if (PyList_Check(shape)) {
        tuple_size = PyList_Size(shape);
        arr_dims = (npy_intp*)malloc(tuple_size*sizeof(npy_intp));
        for (i = 0; i < tuple_size; ++i) {
            tuple_item = PyList_GetItem(shape, i);
            if (PyLong_Check(tuple_item) || PyLong_Check(tuple_item)) {
                tuple_item_long = PyLong_AsLong(tuple_item);
                arr_dims[i] = tuple_item_long;
                random_length *= tuple_item_long;
            } else {
                return NULL;
            }
        }
    } else if (PyTuple_Check(shape)) {
        tuple_size = PyTuple_Size(shape);
        arr_dims = (npy_intp*)malloc(tuple_size*sizeof(npy_intp));
        for (i = 0; i < tuple_size; ++i) {
            tuple_item = PyTuple_GetItem(shape, i);
            if (PyLong_Check(tuple_item) || PyLong_Check(tuple_item)) {
                tuple_item_long = PyLong_AsLong(tuple_item);
                arr_dims[i] = tuple_item_long;
                random_length *= tuple_item_long;
            } else {
                return NULL;
            }
        }
    } else {
        PyErr_SetString(PyExc_ValueError, "random_sample takes only int, long, tuple or list as size parameter");
        return NULL;
    }
    arr = (PyArrayObject*)PyArray_SimpleNew(tuple_size, arr_dims, NPY_DOUBLE);
    out = new double[random_length];
    self->mr.rand(random_length, out);
    arr_iter = NpyIter_New(arr, NPY_ITER_READWRITE, NPY_FORTRANORDER,
                            NPY_NO_CASTING, NULL);
    if (arr_iter == NULL) {
        return NULL;
    }
    arr_iternext = NpyIter_GetIterNext(arr_iter, NULL);
    if (arr_iternext == NULL) {
        NpyIter_Deallocate(arr_iter);
        return NULL;
    }
    arr_dataptr = (double **) NpyIter_GetDataPtrArray(arr_iter);
    i = 0;
    do {
        **arr_dataptr = out[i++];
    } while(arr_iternext(arr_iter));
    delete[] out;
    NpyIter_Deallocate(arr_iter);
    //Py_INCREF(arr);
    free(arr_dims);
    return (PyObject*)arr;
}

static PyObject* rng(Generator *self, PyObject *args, PyObject *kw)
{
    int seed;
    static char *keywords[] = {"seed", NULL};
    if (!PyArg_ParseTupleAndKeywords((PyObject*)args, kw, "i", keywords, &seed)) {
        return NULL;
    }
    self->mr.rng(seed);
    return Py_BuildValue("");
}

static PyMethodDef Methods[] =
{
     {"randn", (PyCFunction)randn, METH_VARARGS | METH_KEYWORDS,
         "return ndarray of normally distributed variables using matlab algorithm"},
     {"random_sample", (PyCFunction)random_sample, METH_VARARGS | METH_KEYWORDS,
         "return ndarray of normally distributed variables using matlab algorithm"},
     {"rng", (PyCFunction)rng, METH_VARARGS | METH_KEYWORDS,
         "seed the random number generator"},
     {NULL, NULL, 0, NULL}
};

static PyTypeObject GeneratorType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "matlab_random.Generator",     /*tp_name*/
    sizeof(Generator),             /*tp_basicsize*/
    0,                         /*tp_itemsize*/
    (destructor)Generator_dealloc, /*tp_dealloc*/
    0,                         /*tp_print*/
    0,                         /*tp_getattr*/
    0,                         /*tp_setattr*/
    0,                         /*tp_compare*/
    0,                         /*tp_repr*/
    0,                         /*tp_as_number*/
    0,                         /*tp_as_sequence*/
    0,                         /*tp_as_mapping*/
    0,                         /*tp_hash */
    0,                         /*tp_call*/
    0,                         /*tp_str*/
    0,                         /*tp_getattro*/
    0,                         /*tp_setattro*/
    0,                         /*tp_as_buffer*/
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,        /*tp_flags*/
    "Generator objects",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    Methods,               /* tp_methods */
    0,             /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,      /* tp_init */
    0,                         /* tp_alloc */
    Generator_new,                 /* tp_new */
};

extern "C"
MOD_INIT(matlab_random)
{
    PyObject *m;

    MOD_DEF(m, "matlab_random", module___doc__,
            Methods)

    if (m == NULL)
        return MOD_ERROR_VAL;

    if (PyType_Ready(&GeneratorType) < 0)
        return MOD_ERROR_VAL;

    Py_INCREF(&GeneratorType);
    PyModule_AddObject(m, "Generator", (PyObject *)&GeneratorType);

    /* IMPORTANT: this must be called */
    import_array();

    return MOD_SUCCESS_VAL(m);
}
