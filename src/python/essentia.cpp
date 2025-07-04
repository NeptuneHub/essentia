/*
 * Copyright (C) 2006-2021  Music Technology Group - Universitat Pompeu Fabra
 *
 * This file is part of Essentia
 *
 * Essentia is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License as published by the Free
 * Software Foundation (FSF), either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the Affero GNU General Public License
 * version 3 along with this program.  If not, see http://www.gnu.org/licenses/
 */

#include <Python.h>

// numpy import
#define PY_ARRAY_UNIQUE_SYMBOL PyArray_API
#include "numpy/arrayobject.h"

// essentia import
#include "essentia.h"
#include "algorithmfactory.h"
using namespace std;
using namespace essentia;


#include "typedefs.h"
#include "pyalgorithm.cpp"
#include "pystreamingalgorithm.cpp"
#include "pyvectorinput.cpp"

// global functions available in the _essentia module, such as version, keys, etc...
#include "globalfuncs.cpp"

static PyObject* Essentia__Module = NULL;


//extern "C" ESSENTIA_API void
PyMODINIT_FUNC
init_essentia() {

  // import our wrapper types
  if (PyType_Ready(&PyAlgorithmType)          < 0 ||
      PyType_Ready(&PyStreamingAlgorithmType) < 0 ||
      PyType_Ready(&PyVectorInputType)        < 0 ||
      PyType_Ready(&StringType)               < 0 ||
      PyType_Ready(&BooleanType)              < 0 ||
      PyType_Ready(&IntegerType)              < 0 ||
      PyType_Ready(&PyRealType)               < 0 ||
      PyType_Ready(&VectorRealType)           < 0 ||
      PyType_Ready(&VectorComplexType)        < 0 ||
      PyType_Ready(&VectorStringType)         < 0 ||
      PyType_Ready(&VectorIntegerType)        < 0 ||
      PyType_Ready(&VectorVectorRealType)     < 0 ||
      PyType_Ready(&VectorVectorStringType)   < 0 ||
      PyType_Ready(&MatrixRealType)           < 0 ||
      PyType_Ready(&PyPoolType)               < 0 ||
      PyType_Ready(&PyStereoSampleType)       < 0 ||
      PyType_Ready(&VectorStereoSampleType)   < 0 ||
      PyType_Ready(&VectorMatrixRealType)     < 0 ||
      PyType_Ready(&TensorRealType)           < 0 ||
      PyType_Ready(&VectorTensorRealType)     < 0 ||
      PyType_Ready(&VectorVectorStereoSampleType) < 0) {

    cerr << "Unable to instantiate Essentia's wrapper types." << endl;
    return NULL;
  }

  // import the NumPy C api
  import_array();


  static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "_essentia",     /* m_name */
    "Module that allows access to essentia plugins and algorithms.",  /* m_doc */
    -1,                  /* m_size */
    Essentia__Methods,   /* m_methods */
    NULL,                /* m_reload */
    NULL,                /* m_traverse */
    NULL,                /* m_clear */
    NULL,                /* m_free */
  };

  Essentia__Module = PyModule_Create(&moduledef);

  if (Essentia__Module == NULL) {
    cerr << "Error loading _essentia python/C module" << endl;
    return NULL;
  }

  // insert the Algorithm class
  Py_INCREF(&PyAlgorithmType);
  PyModule_AddObject(Essentia__Module, (char*)"Algorithm", (PyObject*)&PyAlgorithmType);

  Py_INCREF(&PyStreamingAlgorithmType);
  PyModule_AddObject(Essentia__Module, (char*)"StreamingAlgorithm", (PyObject*)&PyStreamingAlgorithmType);

  Py_INCREF(&PyVectorInputType);
  PyModule_AddObject(Essentia__Module, (char*)"VectorInput", (PyObject*)&PyVectorInputType);

  Py_INCREF(&PyPoolType);
  PyModule_AddObject(Essentia__Module, (char*)"Pool", (PyObject*)&PyPoolType);

  // register algorithms in the factory
  essentia::init();

  E_DEBUG(EPyBindings, "Successfully initialized _essentia python/C module");
  return Essentia__Module;
}
