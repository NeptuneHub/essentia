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

DEFINE_PYTHON_TYPE(VectorStereoSample);

// converts a vector of size n to a numpy array of dimension n x 2
PyObject* VectorStereoSample::toPythonCopy(const vector<StereoSample>* v) {
  npy_intp dims[2] = {0, 0};
  dims[0] = v->size();
  dims[1] = 2;
  PyArrayObject* result = (PyArrayObject*)PyArray_SimpleNew(2, dims, NPY_FLOAT);

  if (!result) throw EssentiaException("VectorStereoSample::toPythonCopy: could not create PyArray");

  for (int i=0; i<int(dims[0]); ++i) {
    Real* left = (Real*)((char*)PyArray_DATA(result) + i*PyArray_STRIDES(result)[0]);
    Real* right = (Real*)((char*)PyArray_DATA(result) + i*PyArray_STRIDES(result)[0] + PyArray_STRIDES(result)[1]);
    *left = (*v)[i].left();
    *right = (*v)[i].right();
  }

  return (PyObject*)result;
}


void* VectorStereoSample::fromPythonCopy(PyObject* obj) {
  if (!PyArray_Check(obj)) {
    throw EssentiaException("VectorStereoSample::fromPythonCopy: given input "
                            "is not a numpy array: ", strtype(obj));
  }

  PyArrayObject* arr = (PyArrayObject*)obj;
  
  if (PyArray_NDIM(arr) != 2) {
    throw EssentiaException("VectorStereoSample::fromPythonCopy: given input "
                            "is not a 2-dimensional numpy array: ", PyArray_NDIM(arr));
  }

  if (PyArray_DIM(arr, 1) != 2) {
    throw EssentiaException("VectorStereoSample::fromPythonCopy: given input's "
                            "second dimension is not 2: ", PyArray_DIM(arr, 1));
  }

  Py_ssize_t total = PyArray_DIM(arr, 0);
  vector<StereoSample>* result = new vector<StereoSample>(total);

  for (int i=0; i<int(total); ++i) {
    Real* left = (Real*)((char*)PyArray_DATA(arr) + i*PyArray_STRIDES(arr)[0]);
    Real* right = (Real*)((char*)PyArray_DATA(arr) + i*PyArray_STRIDES(arr)[0] + PyArray_STRIDES(arr)[1]);
    (*result)[i].left() = *left;
    (*result)[i].right() = *right;
  }

  return result;
}
