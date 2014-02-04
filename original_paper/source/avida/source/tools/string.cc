//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#include "string.hh"

extern "C"{
#include <stdio.h>
#include <stdarg.h>
}


// ** class cStringData **
// -- Constructors --
cString::cStringData::cStringData(unsigned short in_size)
  : refs(1)
  , size(in_size)
  , data(new char[size+1])
{
  assert(data!=NULL); // Memory Allocation Error: Out of Memory
  data[0] = '\0';
  data[size] = '\0';
}

cString::cStringData::cStringData(unsigned short in_size, const char * in)
  : refs(1)
  , size(in_size)
  , data(new char[size+1])
{
  assert(data!=NULL); // Memory Allocation Error: Out of Memory
  for( unsigned short i=0; i<size; ++i )  data[i]=in[i];
  data[size] = '\0';
}

cString::cStringData::cStringData(const cStringData & in)
  : refs(1)
  , size(in.GetSize())
  , data(new char[size+1])
{
  assert(data!=NULL); // Memory Allocation Error: Out of Memory
  for( unsigned short i=0; i<size; ++i )  data[i]=in[i];
  data[size] = '\0';
}



// ** class cString **

// -- Constants --
const int cString::MAX_LENGTH = MAX_STRING_LENGTH;


// -- Comparisons --

bool cString::operator== (const cString & in) const {
  // Compares sizes first since we have that info anyway
  int i=-1;
  if( GetSize() == in.GetSize() ){
    for( i=0; i<GetSize() && (*this)[i]==in[i]; ++i );
  }
  return ( i == GetSize() );  // i == GetSize if all chars matched
}


int cString::Compare(const char * in) const
{
  // -1 (*this<in), 0 (*this==in), 1 (*this>in) ... just like strcmp()
  assert(in!=NULL);  // NULL input string
  int i;
  for( i = 0;
       i < GetSize() && in[i]!='\0' && (*this)[i] == in[i];
       i++ );

  if( i == GetSize()  &&  in[i] == '\0' ) {
    return 0;                // *this == in
  }

  // They're not equal!
  if ( i < GetSize() && (*this)[i] > in[i] )  return 1;  // *this > in
  return -1;                                             // *this < in
}



// -- Information --

int cString::CountWhitespace(int start) const
{
  assert(start >= 0); // Negative Index into String
  if (start >= GetSize()) return 0;
  int count = 0;
  while (start+count<GetSize() && IsWhitespace(start+count)) count++;
  return count;
}


int cString::CountWordsize(int start) const {
  assert(start >= 0); // Negative Index into String
  if( start>=GetSize() )  return 0;
  int count = 0;
  while( start+count<GetSize() && !IsWhitespace(start+count) )
    ++count;
  return count;
}


int cString::CountLinesize(int start) const
{
  assert(start >= 0); // Negative Index into String
  if( start>=GetSize() )  return 0;
  int count = 0;
  while( start+count<GetSize() && (*this)[start+count]!='\n' )
    count++;
  return count;
}


int cString::CountNumLines() const
{
  int num_lines = 1;
  for( int i=0; i<GetSize(); ++i ){
    if( (*this)[i] == '\n' )  num_lines++;
  }
  return num_lines;
}


int cString::CountNumWords() const
{
  int num_words = 0;
  int pos = CountWhitespace();     // Skip initial whitespace.
  while( pos<GetSize() ) {
    pos += CountWordsize(pos);
    pos += CountWhitespace(pos);
    num_words++;
  }
  return num_words;
}


// -- Search --
int cString::Find(char in_char, int start) const
{
  int pos = start;
  assert (pos >= 0);         // Negative Position: setting to 0
  assert (pos <= GetSize()); // Position Past End of String: setting to end.
  if (pos <= 0) pos = 0;
  else if (pos > GetSize()) pos = GetSize();

  while( pos < GetSize() ) {
    if( (*this)[pos] == in_char) return pos; // Found!
    pos++;
  }
  return -1; // Not Found
}


int cString::FindWord(const cString & in, int pos) const
{
  assert (pos >= 0);         // Negative Position: setting to 0
  assert (pos <= GetSize()); // Position Past End of String: setting to end.
  if (pos <= 0) pos = 0;
  else if (pos > GetSize()) pos = GetSize();

  // While there is enough space to find
  while (pos != -1 && pos + in.GetSize() < GetSize()) {
    cerr << in << " " << pos << endl;
    if( (pos=Find(in, pos)) >= 0 ){      // try to find it
      // if it's got whitespace on both sides, it's a word
      if( ( pos==0 || IsWhitespace(pos-1) )
	  && ( pos==GetSize()-1 || IsWhitespace(pos+in.GetSize()) ) ){
	return pos;
      } else {
	pos++; // go on and look further down
      }
    }
  }
  return -1;
}
	

cString cString::GetWord(int word_id) const
{
  // Find positon of word
  int pos = 0;
  int cur_word = 0;
  while( pos<GetSize() && cur_word<word_id ) {  // If this isn't the word
    pos += CountWhitespace(pos);                 // Skip leading whitespace
    pos += CountWordsize(pos);                   // Skip this word
    cur_word++;
  }
  // Return GetWordAt position... (it will skip any leading whitespace)
  return GetWordAt(pos);
}


cString cString::GetWordAt(int start) const
{
  int pos = start + CountWhitespace(start);  // Skip past initial whitespace.
  int word_size = CountWordsize(pos);        // Get size of word
  cString new_string(word_size);             // Allocate new_string that size
  for (int i = 0; i < word_size; i++) {      // Copy the chars to new_string
    new_string[i] = (*this)[pos + i];
  }
  return new_string;
}

bool cString::IsWhitespace() const
{
  for( int i=0; i < GetSize(); ++i){
    if ( IsWhitespace(i) == false ) return false;
  }
  return true;
}

bool cString::IsUpperLetter() const
{
  for (int i = 0; i < GetSize(); ++i) {
    if ( IsUpperLetter(i) == false ) return false;
  }
  return true;
}

bool cString::IsLowerLetter() const
{
  for (int i = 0; i < GetSize(); ++i) {
    if ( IsLowerLetter(i) == false ) return false;
  }
  return true;
}

bool cString::IsLetter() const
{
  for (int i = 0; i < GetSize(); ++i) {
    if ( IsLetter(i) == false ) return false;
  }
  return true;
}

bool cString::IsNumber() const
{
  for (int i = 0; i < GetSize(); ++i) {
    if ( IsNumber(i) == false ) return false;
  }
  return true;
}

bool cString::IsNumeric() const
{
  for (int i = 0; i < GetSize(); ++i) {
    if ( IsNumber(i) == false ) return false;
  }
  return true;
}

bool cString::IsAlphaNumeric() const
{
  for (int i = 0; i < GetSize(); ++i) {
    if ( IsAlphaNumeric(i) == false ) return false;
  }
  return true;
}



cString cString::Substring(int start, int size) const
{
  assert(size > 0); // Non-Positive Size
  assert(start >= 0); // Negative Position
  assert(start + size <= GetSize()); // Position+Size Past End of String

  cString new_string(size);
  for (int i=0; i<size; i++) {
    new_string[i] = (*this)[i+start];
  }
  return new_string;
}

bool cString::IsSubstring(const cString & in_string, int start) const
{
  assert (start >= 0); // Negative start position

  // If the potential sub-string won't fit, return false;
  if ( start + in_string.GetSize() > GetSize() ) return false;

  // Otherwise, check character by character.
  for (int i = 0; i < in_string.GetSize(); i++) {
    if ( (*this)[i+start] != in_string[i] ) return false;
  }

  return true;
}


// -- Modifiers --

cString & cString::Set(const char * fmt, ...)
{
  va_list argp;
  char buf[MAX_LENGTH];
  va_start(argp, fmt);
  vsprintf(buf, fmt, argp);
  va_end(argp);
  (*this) = buf;
  return *this;
}


int cString::Replace(const cString & old_st, const cString & new_st, int pos)
{
  int location;
  if( (location=Find(old_st, pos)) >= 0 ){ // If we find the old_st
    Insert(new_st, location, old_st.GetSize());
  }
  return location;
}


int cString::LeftJustify()
{
  int ws_count = CountWhitespace();
  if( ws_count>0 )
    InsertStr(0, NULL, 0, ws_count);
  return ws_count;
}


int cString::RightJustify()
{
  int ws_count = 0;
  while( GetSize()-ws_count-1>0 && IsWhitespace(GetSize()-ws_count-1) )
    ws_count++;
  if( ws_count>0 )
    InsertStr(0, NULL, GetSize()-ws_count, ws_count);
  return ws_count;
}


cString cString::Pop(const char delim)
{
  int pos;
  cString rv("");
  if( (pos=Find(delim)) >= 0 ){ // if it is found at all
    if( pos > 0 ){  // if the first char is not delim, return substring
      rv = Substring(0,pos);
    }
    // Trim off the front
    InsertStr(0, NULL, 0, pos+1);
  }

  // If the deliminator is *not* found, return the whole string.
  else {
    rv = *this;
    *this = "";
  }
  return rv;
}


cString cString::PopWord()
{
  // If there is nothing here, there is nothing to be popped.
  if (GetSize() == 0) return "";

  const int start_pos = CountWhitespace();
  const int word_size = CountWordsize(start_pos);

  // If the string is not all whitespace, save the word we cut off...
  cString rv("");
  if (word_size > 0) rv = Substring(start_pos, word_size);

  // Trim off the front
  const int word_end = start_pos + word_size;
  const int new_start = word_end + CountWhitespace(word_end);
  InsertStr(0, NULL, 0, new_start);  // Insert null in place of old word.

  return rv;
}


void cString::ToLower()
{
  for (int pos = 0; pos < GetSize(); pos++) {
    if( (*this)[pos] >= 'A' && (*this)[pos] <= 'Z' )
      (*this)[pos] += 'a' - 'A';
  }
}


void cString::ToUpper()
{
  for (int pos = 0; pos < GetSize(); pos++) {
    if( (*this)[pos] >= 'a' && (*this)[pos] <= 'z' )
      (*this)[pos] += 'A' - 'a';
  }
}


void cString::Reverse()
{
  cString new_st(GetSize());
  for( int i=0; i<GetSize(); ++i ){
    // new_st[i] = (*this)[GetSize()-i-1];       // @CAO Problem in new gcc
    new_st[i] = GetData()[GetSize()-i-1];
  }
  (*this) = new_st;
}


void cString::CompressWhitespace()
{
  // Eats initial whitespace
  // 2 pass method...
  int i;
  int pos = 0;
  int start_pos = CountWhitespace();
  int new_size = 0;
  bool ws = false;

  // count the number of characters that we will need in the new string
  for( i=start_pos; i<GetSize(); ++i ){
    if( IsWhitespace(i) ){ // if it whitespace...
      if( ws == false ){     // if we arn't already in a whitespace block
	ws = true;             // we are now in a whitespace block
      }
    }else{                 // it isn't whitespace, so count
      if( ws==true ){        // if there was a whitespace block
	++new_size;            // inc once for the block
	ws = false;
      }
      ++new_size;
    }
  }

  cString new_st(new_size);  // Allocate new string

  // Copy over the characters
  // pos will be the location in new_st, while i is the index into this
  ws = false;
  for( i=start_pos; i<GetSize(); ++i ){
    if( IsWhitespace(i) ){ // if it whitespace...
      if( ws == false ){     // if we arn't already in a whitespace block
	ws = true;             // we are now in a whitespace block
      }
    }else{                 // it isn't whitespace, so count
      if( ws==true ){        // if there was a whitespace block
	new_st[pos] = ' ';     // put a space in for the whitespace block
	++pos;                 // inc once for the block
	ws = false;
      }
      // new_st[pos] = (*this)[i]; // copy it & increment pos  @CAO prob in gcc
      new_st[pos] = GetData()[i]; // copy it & increment pos
      ++pos;
    }
  }

  (*this) = new_st;  // assign the new_st to this
}


void cString::RemoveWhitespace(){
  int i;
  int new_size = 0;
  for( i=0; i<GetSize(); ++i ){  // count new size
    if( !IsWhitespace(i) )  ++new_size;
  }
  cString new_st(new_size);      // allocate new string
  int pos = 0;
  for( i=0; i<GetSize(); ++i ){  // count new size
    if( !IsWhitespace(i) ){
      new_st[pos] = GetData()[i]; // copy it & increment pos   @CAO prob in GCC
      ++pos;
    }
  }
  (*this) = new_st;  // assign the new_st to this
}


void cString::RemoveChar(char out_char){
  int i;
  int new_size = 0;
  for( i=0; i<GetSize(); ++i ){  // count new size
    if( GetData()[i] != out_char )  ++new_size;
  }
  cString new_st(new_size);      // allocate new string
  int pos = 0;
  for( i=0; i<GetSize(); ++i ){  // count new size
    if( GetData()[i] != out_char ){
      new_st[pos] = GetData()[i]; // copy it & increment pos   @CAO prob in GCC
      ++pos;
    }
  }
  (*this) = new_st;  // assign the new_st to this
}



// -- Internal Methods --

cString & cString::AppendStr(const int in_size, const char * in)
{
  assert (in_size == 0 || in != NULL); // NULL input string

  // Allocate a new string
  cStringData * new_value = new cStringData(GetSize()+in_size);
  assert (new_value != NULL);       // Memory Allocation Error: Out of Memory
  for(int i=0; i<GetSize(); ++i ) { // Copy self up to pos
    (*new_value)[i] = this->operator[](i);
  }
  for(int i=0; i<in_size; ++i ) {   // Copy in
    assert(in[i] != '\0');          // Input String Contains '\\0' or too Short
    (*new_value)[i+GetSize()] = in[i];
  }
  TakeValue(new_value);             // Reassing data to new data
  return(*this);
}


cString & cString::InsertStr(const int in_size, const char * in,
			     int pos, int excise )
{
  // Inserts 'in' (of length 'in_size') at postition 'pos'
  // Also excises 'excise' characters from 'pos'
  // If 'in_size'==0 then 'in' can == NULL and only excise happens

  // Validate inputs:
  assert (in_size >= 0);               // Negative input size
  assert (pos >= 0);                   // Negative position
  assert (pos <= GetSize());           // Position past end of string
  assert (excise >= 0);                // Negative excise
  assert (excise <= GetSize()-pos);    // Excise number too large
  assert (excise > 0 || in_size > 0);  // Make sure a change is made!
  assert (in_size == 0 || in != NULL); // NULL input string

  // Allocate a new string
  const int new_size = GetSize() + in_size - excise;
  cStringData * new_value = new cStringData(new_size);
  assert (new_value != NULL);  // Memory Allocation Error: Out of Memory

  for(int i = 0; i < pos; ++i ){             // Copy self up to pos
    (*new_value)[i] = this->operator[](i);
  }
  for(int i = 0; i < in_size; ++i ){         // Copy in
    assert( in[i] != '\0');  // Input String Contains '\\0' or too Short
    (*new_value)[i+pos] = in[i];
  }
  for(int i=pos+excise; i<GetSize(); ++i ){  // Copy rest of self
    (*new_value)[i+in_size-excise] = this->operator[](i);
  }

  TakeValue(new_value);                      // Reassing data to new data
  return(*this);
}


int cString::FindStr(const char * in, const int in_size, int pos) const
{
  assert (pos>=0);         // Negative position
  assert (pos<=GetSize()); // Position past end of string

  while (pos < GetSize()) {
    if( GetSize()-pos < in_size ) return -1; // Too near this string's end.
    if( (*this)[pos] == in[0] ){
      // see if we have found the string...
      int i;
      for( i = 1; i < in_size; i++ ){
	assert (pos+i < GetSize()); // Reached end of (*this) in Find
	assert (in[i] != '\0');     // Reached end of 'in' in Find
	if( (*this)[pos + i] != in[i] ) break; // Match failure!
      }
      // If we have made it fully through the loop, we have found a match!
      if( i == in_size ) return pos;
    }
    pos++;
  }
  return -1;
}



// {{{ ** External cString Functions **

istream & operator >> (istream & in, cString & string)
{
  char buf[cString::MAX_LENGTH];
  in>>buf;
  string=buf;
  return in;
}

ostream & operator << (ostream & out, const cString & string)
{
  out << string.GetData();
  return out;
}

