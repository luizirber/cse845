//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2000 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////
// This file is for misc. functions which are of general use...

#ifndef FUNCTIONS_HH
#define FUNCTIONS_HH

#include <iostream.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include "tArray.hh"

#ifndef NULL
#define NULL 0
#endif


inline int Min(int in1, int in2);
inline int Max(int in1, int in2);
inline int Range(int value, int min, int max);
inline int Mod(int value, int base);
inline int Pow(int bas, int expon);
inline double Log(double value);

//////////////////////
// Generic (inline) functions...
//////////////////////

inline void Swap(int & in1, int & in2)
{
  const int tmp = in1;
  in1 = in2;
  in2 = tmp;
}

inline bool ToggleBool(bool & in_bool)
{
  if (in_bool == true) in_bool = false;
  else in_bool = true;
  return in_bool;
}

inline int Min(int in1, int in2)
{
  return (in1 > in2) ? in2 : in1;
}

inline int Max(int in1, int in2)
{
  return (in1 < in2) ? in2 : in1;
}

inline int Range(int value, int min, int max)
{
  value = (value < min) ? min : value;
  return (value > max) ? max : value;
}

inline int Mod(int value, int base)
{
  value %= base;
  if (value < 0) value += base;
  return value;
}

inline int Pow(int base, int expon)
{
  int value = 1;
  for (int i = 0; i < expon; i++) value *= base;
  return value;
}

inline double Log(double value)
{
  assert(value > 0);
  return log(value);
}

/*
 *   Special array functions...
 */

inline int Min(const tArray<int> & test_array)
{
  assert(test_array.GetSize() > 0);

  int min_val = test_array[0];
  for (int i = 1; i < test_array.GetSize(); i++) {
    if (test_array[i] < min_val) min_val = test_array[i];
  }
  return min_val;
}

inline int Max(const tArray<int> & test_array)
{
  assert(test_array.GetSize() > 0);

  int max_val = test_array[0];
  for (int i = 1; i < test_array.GetSize(); i++) {
    if (test_array[i] > max_val) max_val = test_array[i];
  }
  return max_val;
}

inline int Sum(const tArray<int> & test_array)
{
  int sum = 0;
  for (int i = 0; i < test_array.GetSize(); i++) {
    sum += test_array[i];
  }
  return sum;
}

#endif
