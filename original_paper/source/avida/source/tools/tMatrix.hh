//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2000 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////
/*
   tMatrix.hh
   Matrix Templates

   Constructor:
     tMatrix( int rows, int cols )
     tMatrix( const tMatrix & rhs )

   Interface:
     tMatrix & operator= ( const tMatrix & rhs )

     unsigned int GetNumRows () const
     unsigned int GetNumCols () const
       returns the size of the array

     const T& ElementAt ( const int row, col ) const
           T& ElementAt ( const int row, col )
     const T& operator() ( const int row, col ) const
           T& operator() ( const int row, col )
       return the element at position in the matrix

     const tArray<T>& operator[] ( const int row ) const
           tArray<T>& operator[] ( const int row )
       return the array at the row in the matrix.


*/

#ifndef TMATRIX_HH
#define TMATRIX_HH

#include <assert.h>

#include "tArray.hh"

/**
 * This class provides a matrix template.
 **/ 

template <class T> class tMatrix {
protected:
  // Internal Variables
  tArray<T> * data;  // Data Elements
  int num_rows;
public:
  int GetNumRows() const { return num_rows; }
  int GetNumCols() const { return data[0].GetSize(); }

  void ResizeClear(const int _rows, const int _cols){
    if (_rows != GetNumRows()) {
      num_rows = _rows;
      assert(_rows > 0); // Invalid size specified for matrix resize
      if( data != NULL )  delete [] data;  // remove old data if exists
      data = new tArray<T>[_rows];  // Allocate block for data
      assert(data != NULL); // Memory Allocation Error: Out of Memory?
    }
    for (int i = 0; i < GetNumRows(); i++) data[i].ResizeClear(_cols);
  }

  void Resize(int _rows, int _cols) {
    // Rows and cols must be > 0
    assert(_rows > 0 && _cols > 0); // Invalid size specified for matrix resize

    if( data != NULL )  {
      tArray<T> * new_data = new tArray<T>[_rows];
      for (int i = 0; i < GetNumRows() && i < _rows; i++) {
	new_data[i] = data[i];
      }
      delete [] data;  // remove old data if exists
      data = new_data;
    } else {
      data = new tArray<T>[_rows];  // Allocate block for data
    }
    assert(data != NULL); // Memory Allocation Error: Out of Memory?
    num_rows = _rows;

    for (int i = 0; i < _rows; i++) data[i].Resize(_cols);
  }

  T & ElementAt(int _row, int _col) { return data[_row][_col]; }
  const T & ElementAt(int _row, int _col) const { return data[_row][_col]; }

        T & operator()(int _r, int _c)       { return ElementAt(_r, _c); }
  const T & operator()(int _r, int _c) const { return ElementAt(_r, _c); }

        tArray<T> & operator[](int row)       { return data[row]; }
  const tArray<T> & operator[](int row) const { return data[row]; }

  void SetAll(const T & value) {
    for (int i = 0; i < num_rows; i++) {
      data[i].SetAll(value);
    }
  }


public:
  // Default Constructor
  explicit tMatrix() : data(NULL), num_rows(0) { ResizeClear(1,1); }

  // Constructor
  explicit tMatrix(const int _rows, const int _cols) :
    data(NULL), num_rows(0) {
      ResizeClear(_rows, _cols);
  }

  // Assingment Operator
  tMatrix & operator= (const tMatrix & rhs){
    if( GetNumRows() != rhs.GetNumRows() || GetNumCols() != rhs.GetNumCols()) {
      ResizeClear(rhs.GetNumRows(), rhs.GetNumCols());
    }
    for (int row = 0; row < GetNumRows(); row++) {
      for (int col = 0; col < GetNumCols(); col++) {
	data[row][col] = rhs.data[row][col];
      }
    }
    return *this;
  }

  // Copy constructor
  explicit tMatrix(const tMatrix & rhs) : data(NULL), num_rows(0) {
    this->operator=(rhs);
  }

  // Destructor
  virtual ~tMatrix(){ if(data!=NULL) delete [] data; }
};

#endif // TARRAY_HH
