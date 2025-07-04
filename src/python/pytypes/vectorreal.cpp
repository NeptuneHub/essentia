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

#include "typedefs.h"
using namespace std;
using namespace essentia;

DEFINE_PYTHON_TYPE(VectorReal);

PyObject* VectorReal::toPythonRef(RogueVector<Real>* v) {
  npy_intp dim = v->size();
  PyObject* result;

  if (dim > 0) result = PyArray_SimpleNewFromData(1, &dim, NPY_FLOAT, &((*v)[0]));
  else         result = PyArray_SimpleNew(1, &dim, NPY_FLOAT);

  if (result == NULL) {
    throw EssentiaException("VectorReal: dang null object");
  }

  if (PyArray_SetBaseObject((PyArrayObject*)result, TO_PYTHON_PROXY(VectorReal, v)) < 0) {
    Py_DECREF(result);
    throw EssentiaException("VectorReal: failed to set base object");
  }

  return result;
}


void* VectorReal::fromPythonRef(PyObject* obj) {
  // if input is a numpy array, just wrap its data with the RogueVector
  if (!PyArray_Check(obj)) {
    throw EssentiaException("VectorReal::fromPythonRef: expected PyArray, received: ", strtype(obj));
  }

  PyArrayObject* array = (PyArrayObject*)obj;

  if (PyArray_DESCR(array)->type_num != NPY_FLOAT) {
    throw EssentiaException("VectorReal::fromPythonRef: this NumPy array doesn't contain Reals (maybe you forgot dtype='f4')");
  }
  if (PyArray_NDIM(array) != 1) {
    throw EssentiaException("VectorReal::fromPythonRef: this NumPy array has dimension ", PyArray_NDIM(array), " (expected 1)");
  }

  return new RogueVector<Real>((Real*)PyArray_DATA(array), PyArray_SIZE(array));
}

Parameter* VectorReal::toParameter(PyObject* obj) {
  RogueVector<Real>* value = (RogueVector<Real>*)fromPythonRef(obj);
  Parameter* result = new Parameter(*value);
  delete value;
  return result;
}
