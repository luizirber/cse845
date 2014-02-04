//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////
#ifndef TVECTOR_HH
#define TVECTOR_HH

#include <stdlib.h>
#include <stdio.h>
#include "assert.hh"

/**
 * This class provides a vector template.
 **/ 

template <class T>
class tVector {
private:
  T *		_data;
  int		_size;
  int		_capacity;
  int		_capIncrement;

public:
  tVector(void): _size(0), _capacity(1), _capIncrement(-1) {
    _data = new T[_capacity];
    #ifdef EXCEPTION_HANDLING
    if(_data == NULL) throw InsufficientMemory();
    #endif
  }

  tVector(int cap, int incr = -1): _size(0), _capacity(cap),
				  _capIncrement(incr){
    _data = new T[_capacity];
    #ifdef EXCEPTION_HANDLING
    if(_data == NULL) throw InsufficientMemory();
    #endif
  }


  tVector(const tVector &v): _size(v._size), _capacity(v._capacity),
			   _capIncrement(v._capIncrement)
  {
    _data = new T[_capacity];
    #ifdef EXCEPTION_HANDLING
    if(_data == NULL) throw InsufficientMemory();
    #endif
    for(int i = 0; i < _size; i++) {
      _data[i] = v._data[i];
    }
  }

  ~tVector() {
    if(_data) delete [] _data;
  }

public:
  int Size(void) const {
    return _size;
  }

  int Capacity(void) const {
    return _capacity;
  }

  void Add(T data) {
    if(Size() + 1 > Capacity()) { // if we have to allocate new space, do so
      T* newdata;
      if(_capIncrement == -1) {
	// double size
	newdata = new T[Capacity() * 2];
	#ifdef EXCEPTION_HANDLING
	if(newdata == NULL) throw InsufficientMemory();
	//#else
	//if(newdata == NULL) cerr << "Memory error" << endl;
	#endif
	_capacity = Capacity() * 2;
      }
      else {
	newdata = new T[Capacity() + _capIncrement];
	#ifdef EXCEPTION_HANDLING
	if(newdata == NULL) throw InsufficientMemory();
	//#else
	//if(newdata == NULL) cerr << "Memory error" << endl;
	#endif
	_capacity = Capacity() + _capIncrement;
      }

      for(int i = 0; i < Size(); i++) {
	newdata[i] = _data[i];
      }
      newdata[_size++] = data;
      delete [] _data;
      _data = newdata;
      return;
    }
    else { // no need to allocate new data
      _data[_size++] = data;
      return;
    }
  }

  void Remove(T data) {
    int i, j;
    for(i = 0; _data[i] != data && i < Size(); i++);
    if(i < Size()) {
      T *newdata = new T[Capacity()];
      for(j = 0; j < i; j++) {
	newdata[j] = _data[j];
      }
      for(; j < Size() - 1; j++) {
	newdata[j] = _data[j + 1];
      }
      delete [] _data;
      _data = newdata;
      --_size;
      return;
    }
    return;
  }

  void RemoveAt(int idx) {
    int i, j;
    if(idx >= 0 && idx < Size()) {
      T *newdata = new T[Capacity()];
      for(i = 0; i < idx; i++) {
	newdata[i] = _data[i];
      }
      for(; i < Size() - 1; i++) {
	newdata[i] = _data[i + 1];
      }
      delete [] _data;
      _data = newdata;
      --_size;
      return;
    }
    return;
  }

  void Clear(void) {
    delete [] _data;
    _data = new T[Capacity()];
    _size = 0;
  }

public:
  T& operator[](int idx) {
    assert_error( idx >= 0, "Lower Bounds Error" );
    assert_error( idx < Size(), "Upper Bounds Error" );
    if(idx >= 0 && idx < Size()) { // it is in range
      return _data[idx];
    }
    #ifdef EXCEPTION_HANDLING
    throw IndexOutofRangeError(idx);
    #endif
    return _data[0];
  }

  T operator[](int idx) const {
    assert_error( idx >= 0, "Lower Bounds Error" );
    assert_error( idx < Size(), "Upper Bounds Error" );
    if(idx >= 0 && idx < Size()) {
      return _data[idx];
    }
    #ifdef EXCEPTION_HANDLING
    throw IndexOutofRangeError(idx);
    #endif
    return _data[0];
  }
};

#endif // _VECTOR_TEMPLATED_H_
