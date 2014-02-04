//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2002 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "label.hh"

#include "../tools/tools.hh" // for g_memory & g_debug


////////////////////////////////
// cCodeLabel stuff...
////////////////////////////////

cCodeLabel::cCodeLabel(int in_base)
  : size(0)
  , base(in_base)
{
}

cCodeLabel::cCodeLabel(const cCodeLabel &in_label)
  : nop_sequence(in_label.nop_sequence)
  , size(in_label.size)
  , base(in_label.base)
{
}

cCodeLabel::~cCodeLabel()
{
}

bool cCodeLabel::OK()
{
  bool result = true;

  assert (size <= MAX_LABEL_SIZE);
  assert (size <= nop_sequence.GetSize());
  for (int i = 0; i < size; i++) {
    assert (nop_sequence[i] < base);
  }

  return result;
}

bool cCodeLabel::operator==(const cCodeLabel & other_label) const
{
  if (size != other_label.GetSize()) {
    return false;
  }

  for (int i = 0; i < size; i++) {
    if (nop_sequence[i] != other_label[i]) {
      return false;
    }
  }

  return true;
}


// This function returns true if the sub_label can be found within
// the label affected.
int cCodeLabel::FindSublabel(cCodeLabel & sub_label)
{
  bool error = false;

  for (int offset = 0; offset <= size - sub_label.GetSize(); offset++) {
    for (int i = 0; i < sub_label.GetSize(); i++) {
      if (nop_sequence[i + offset] != sub_label[i]) {
	error = true;
	break;
      }
    }
    if (!error) return offset;
    error = false;
  }

  return -1;
}

void cCodeLabel::SaveState(ostream & fp)
{
//   assert(fp.good());
//   fp<<"cCodeLabel"<<" ";

//   fp<<"|"; // marker

//   fp << size << " " << base << " ";
//   for( int i=0; i < size; ++i ){
//     fp<<nop_sequence[i];
//   }
//   fp<<endl;
}


void cCodeLabel::LoadState(istream & fp)
{
//   assert(fp.good());
//   cString foo;
//   fp>>foo;
//   assert( foo == "cCodeLabel");

//   char marker;
//   fp>>marker;
//   assert( marker == '|' );

//   fp.get(size);
//   fp.get(base);
//   for( int i=0; i<MAX_LABEL_SIZE; ++i ){
//     fp.get(nop_sequence[i]);
//   }
}



