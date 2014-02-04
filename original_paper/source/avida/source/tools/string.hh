//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef STRING_HH
#define STRING_HH

#include <stdlib.h>
#include <iostream.h>
#include <string.h>
#include <assert.h>

#define MAX_STRING_LENGTH 4096

/**
 * A multipurpose string class with many convenient methods of
 * manipulating and comparing strings.
 **/

class cString {

protected:
  void CopyOnWrite(){
    if( value->IsShared() ){  // if it is shared
      value->RemoveRef();     // remove our reference count
      value = new cStringData(*value);  // make own copy of value
    }
  }

  // -- Contained Classes --
private:
  // Declarations (only needed)
  class cStringData;

  // {{{ -- cCharProxy -- To detect rvalue vs lvalue ---------------------

  class cCharProxy{
  private:
    cString & string;
    unsigned short index;

  public:
    cCharProxy( cString & _string, unsigned short _index) :
     string(_string), index(_index) {;}

    inline cCharProxy & operator= (char c);     // lvalue
    inline cCharProxy & operator+= (char c);    // lvalue
    inline cCharProxy & operator-= (char c);    // lvalue
    inline cCharProxy & operator++ ();          // lvalue (prefix)
    inline char         operator++ (int dummy); // lvalue (postfix)
    inline cCharProxy & operator-- ();          // lvalue (prefix)
    inline char         operator-- (int dummy); // lvalue (postfix)
    inline operator char () const ;             // rvalue
  };

  friend cCharProxy;  // Telling rvalue vs lvalue ....

  // }}}  End cCharProxy
  // {{{ -- cStringData -- Holds the actual data and is reference count --
  class cStringData{
    // NOTE: Terminating NULL is always there (you can't assign!!)

  private:
    unsigned short refs;   // Number of references
    unsigned short size;   // size of data (NOT INCLUDING TRAILING NULL)
    char * data;

  public:
    explicit cStringData(unsigned short in_size);
    cStringData(unsigned short in_size, const char * in);
    cStringData(const cStringData & in);

    ~cStringData(){
      assert(refs == 0);  // Deleting cStringData with References!!
      delete [] data; }


    cStringData & operator= (const cStringData & in) {
      delete [] data;
      size = in.GetSize();
      data = new char [size+1];
      assert(data != NULL);   // Memory Allocation Error: Out of Memory
      for( unsigned short i=0; i<size; ++i )  data[i]=in[i];
      data[size] = '\0';
      return (*this);
    }


    unsigned short GetSize() const { return size; }

    const char * GetData() const { return data; }

    char operator[] (int index) const {
      assert(index >= 0);    // Lower Bounds Error
      assert(index <= size); // Upper Bounds Error
      return data[index];
    }

    char & operator[](int index) {
      assert(index >= 0);     // Lower Bounds Error
      assert(index <= size);  // Upper Bounds Error
      assert(index != size);  // Cannot Change Terminating NULL
      return data[index];
    }


    bool IsShared() { return( refs > 1 ); }

    unsigned short RemoveRef() {
      assert( refs > 0 );  // Reference count corrupted
      return(--refs);
    }

    cStringData * NewRef() { ++refs; return this; }

  };
  // }}} End cStringData

  // -- Constants --
public:
  static const int MAX_LENGTH;



  //  -- INTERFACE -----------------------------------------------------------
public:

  // -- Constructors --
  cString(const char * in = "") : value(new cStringData(strlen(in), in)) {
    assert( in != NULL );     // NULL input string
    assert( value != NULL );  // Memory Allocation Error: Out of Memory
  }
  explicit cString(const int size) : value(new cStringData(size)) {
    assert( value!=NULL );    // Memory Allocation Error: Out of Memory
  }
  cString(const cString & in) : value(in.value->NewRef()) {;}


  // -- Destructor --
  virtual ~cString() { if( value->RemoveRef() == 0 )  delete value; }


  // Cast to const char *
  operator const char * () const { return value->GetData(); }
  const char * operator() () const { return value->GetData(); } // Depricated
  const char * GetData() const { return value->GetData(); } // Depricated


  // Assignment Operators
  cString & operator= (const cString & in){
    ChangeValue(in.value);
    return *this; }
  cString & operator= (const char * in){
    assert( in != NULL ); // NULL input string
    if( value->RemoveRef() == 0 ) delete value;
    value = new cStringData(strlen(in),in);
    assert(value != NULL);  // Memory Allocation Error: Out of Memory
    return *this;
  }
  
  
  /**
   * Get the size of the string (not including the terminating '\0').
   **/
  int GetSize() const { return value->GetSize(); }


  // Comparisons
  int Compare(const char * in) const;  // strcmp like function
  bool operator== (const char * in)    const { return (Compare(in)==0); }
  bool operator== (const cString & in) const;  // A bit optimized
  bool operator!= (const char * in)    const { return !(*this==in); }
  bool operator<  (const char * in)    const { return (Compare(in)<0); }
  bool operator>  (const char * in)    const { return (Compare(in)>0); }
  bool operator<= (const char * in)    const { return (Compare(in)<=0); }
  bool operator>= (const char * in)    const { return (Compare(in)>=0); }

  // Concatenation
  cString & operator+= (const char in)  { return AppendStr(1,&in); }
  cString & operator+= (const char * in){ return AppendStr(strlen(in),in); }
  cString & operator+= (const cString & in){return AppendStr(in.GetSize(),in);}
  cString operator+ (const char in_char){ return (cString(*this) += in_char); }
  cString operator+ (const char * in)   { return (cString(*this) += in); }
  cString operator+ (const cString & in){ return (cString(*this) += in); }


  // Additional modifiers
  cString & Set(const char * fmt, ...);

  cString & Insert(const char in, int pos=0, int excise=0){
    return InsertStr(1, &in, pos, excise); }
  cString & Insert(const char * in, int pos=0, int excise=0){
    return InsertStr(strlen(in), in, pos, excise); }
  cString & Insert(const cString & in, int pos=0, int excise=0){
    return InsertStr(in.GetSize(), in, pos, excise); }


  // Removes 'size' characters from 'pos' (default size = to end of string)
  cString & Clip(int pos, int size = -1 /*end of string*/ ){
    if( size < 0 ) size = GetSize()-pos;
    return InsertStr(0, NULL, pos, size); }
  cString & ClipFront(int size){  // Clip off first 'clip_size' chars
    return InsertStr(0, NULL, 0, size); }
  cString & ClipEnd(int size){    // Clip off last 'clip_size' chars
    return InsertStr(0, NULL, GetSize()-size, size); }
  
  /**
   * Find and replace a substring in the string with a different substring.
   * If the substring is not found, the string object is not changed.
   *
   * @return The position at which the substring was found, or -1 if it wasn't found.
   * @param old_string The substring that is going to be replaced.
   * @param new_string The replacement.
   * @param pos The position at which the search should start.
   **/
  int Replace(const cString & old_st, const cString & new_st, int pos=0);

  cString Pop(const char delim);  // Remove and return up to delim char
  
  /**
   * Remove the first word.
   *
   * @return The removed word.
   **/
  cString PopWord();
  
  /**
   * Remove the first line.
   *
   * @return The removed line.
   **/
  cString PopLine(){ return Pop('\n'); } 
  
  /**
   * Remove begining whitespace.
   *
   * @return The number of characters removed.
   **/
  int LeftJustify(); 
  
  /**
   * Remove ending whitespace.
   *
   * @return The number of characters removed.
   **/
  int RightJustify(); 
  
  /**
   * Reverse the order of the characters in the string.
   **/
  void Reverse(); 
  
  /**
   * Convert the string to lowercase.
   **/
  void ToLower(); 
  
  /** 
   * Convert the string to uppercase.
   **/
  void ToUpper();
  
  /**
   * Replace all blocks of whitespace with a single space (' ').
   *
   * @see cString::IsWhitespace()
   **/
  void CompressWhitespace();  
  
  /**
   * Get rid of all(!) whitespace. 
   *
   * @see cString::IsWhitespace()
   **/
  void RemoveWhitespace(); 

  /**
   * Get rid of all occurances of a specific character.
   *
   * @see cString::RemoveWhitespace()
   **/
  void RemoveChar(char out_char); 
  
  // Individal Char Access
  inline char operator[] (int index) const {
    return (char)((*value)[index]);
    // Below should work, but fails in HP CC -- TCC
    //    return const_cast(const cStringData&, *value)[index];
  }
  cCharProxy operator[] (int index) { return cCharProxy(*this,index); }


  /**
   * Convert string to int.
   *
   * @return The integer value corresponding to the string.
   **/
  int AsInt() const { return atoi(*this); }
  
  /**
   * Convert string to double.
   *
   * @return The double value corresponding to the string.
   **/
  double AsDouble() const { return atof(*this); }

  // Accessors & Information
  /**
   * Tests whether the string is empty.
   **/
  bool IsEmpty() const { return GetSize()==0; } // Can just call GetSize
    
  /**
   * Counts whitespace, beginning at the given position.
   *
   * @param start The index at which counting should begin.
   **/
  int CountWhitespace(int start=0) const; 
  
  /**
   * Counts non-whitespace, beginning at the given position.
   *
   * @param start The index at which counting should begin.
   **/
  int CountWordsize(int start=0) const;
    
  /**
   * Counts until the first occurrence of '\n', beginning at the 
   * given position.
   *
   * @param start The index at which counting should begin.
   **/
  int CountLinesize(int start=0) const; 
  
  /**
   * Counts the number of lines in a string.
   **/
  int CountNumLines() const;           
  
  /**
   * Counts the number of separate words in a string.
   **/
  int CountNumWords() const;         
    
  /**
   * Get a specific word from a string.
   *
   * @param word_id The number of the word, counted from the beginning of 
   * the string, starting with 0.
   **/
  cString GetWord(int word_id=0) const;
    
  /**
   * Get the next word after the specified position. Any leading whitespace 
   * is removed.
   *
   * @param start The position at which the function should start 
   * searching for a word.
   **/
  cString GetWordAt(int start=0) const; 

  /**
   * Test if a character is whitespace. Currently, as whitespace count
   * ' ', '\r', '\t', '\n'.
   *
   * @param pos The position of the character to test.
   **/
  bool IsWhitespace(int pos) const {
    return ( (*this)[pos] == ' '  || (*this)[pos] == '\t' ||
	     (*this)[pos] == '\r' || (*this)[pos] == '\n' );
  }
  
  /**
   * Test if a character is a capital letter.
   *
   * @param pos The position of the character to test.
   **/
  bool IsUpperLetter(int pos) const {
    return ((*this)[pos] >= 'A' && (*this)[pos] <= 'Z');
  }
  
  /**
   * Test if a character is not a capital letter.
   *
   * @param pos The position of the character to test.
   **/
  bool IsLowerLetter(int pos) const {
    return ((*this)[pos] >= 'a' && (*this)[pos] <= 'z');
  }
  
  /**
   * Test if a character is a letter.
   *
   * @param pos The position of the character to test.
   **/
  bool IsLetter(int pos) const {
    return IsUpperLetter(pos) || IsLowerLetter(pos);
  }
  
  /**
   * Test if a character is a number (this includes expressions
   * such as -3.4e5).
   *
   * @param pos The position of the character to test.
   **/
  bool IsNumber(int pos) const {
    return ( ( (*this)[pos] >= '0' && (*this)[pos] <= '9' ) ||
	     (*this)[pos] == '-' || (*this)[pos] == '+' ||
	     (*this)[pos] == '.' || (*this)[pos] == 'e' ||
	     (*this)[pos] == 'E' );
  }
  
  /**
   * Test if a character is a numeral (0, 1, ..., 9).
   *
   * @param pos The position of the character to test.
   **/
  bool IsNumeric(int pos) const {
    return ((*this)[pos] >= '0' && (*this)[pos] <= '9');
  }
  
  /**
   * Test if a character is either a numeral or a letter.
   *
   * @param pos The position of the character to test.
   **/
  bool IsAlphaNumeric(int pos) const {
    return IsLetter(pos) || IsNumber(pos);
  }
  
  /**
   * Test whether the complete string consits only of whitespace.
   **/
  bool IsWhitespace() const;
  
  /**
   * Test whether the complete string consits only of uppercase letters.
   **/
  bool IsUpperLetter() const;
  
  /**
   * Test whether the complete string consits only of lowercase letters.
   **/
  bool IsLowerLetter() const;
  
  /**
   * Test whether the complete string consits only of letters.
   **/
  bool IsLetter() const;
  
  /**
   * Test whether the complete string can be seen as a number.
   **/
  bool IsNumber() const;
  
  /**
   * Test whether the complete string consits only of numerals.
   **/
  bool IsNumeric() const;
  
  /**
   * Test whether the complete string consits only of letters or numerals.
   **/
  bool IsAlphaNumeric() const;
  
  /**
   * Search for a single character.
   *
   * @return The first occurence after pos, or -1 if not found
   **/
  int Find(char in_char, int pos=0) const;
  
  /**
   * Search for a substring.
   *
   * @return The first occurence after pos, or -1 if not found
   **/
  int Find(const char * in, int pos=0) const{
    return FindStr(in, strlen(in), pos); }
  
  /**
   * Search for a substring.
   *
   * @return The first occurence after pos, or -1 if not found
   **/
  int Find(const cString & in, int pos=0) const{
    return FindStr(in, in.GetSize(), pos); }
  
  /**
   * Search for a word.
   *
   * @return The first occurence after pos, or -1 if not found
   **/
  int FindWord(const cString & in, int pos=0) const;
  
  
  /**
   * Cut out a substring.
   *
   * @return The substring.
   * @param start The beginning of the substring in the string.
   * @param size The number of characters in the substring.
   **/
  cString Substring(int start, int size) const ;
  
  bool IsSubstring(const cString & in_string, int start) const;
  
  
  // {{{ -- INTERNALS -------------------------------------------------------
protected:
  // -- Internal Functions --
  void ChangeValue(cStringData * new_ref){   // If you didn't make new_value
    if( value->RemoveRef() == 0 ) delete value;
    value = new_ref->NewRef();
  }

  void TakeValue(cStringData * new_ref){     // If you made new_value!
    if( value->RemoveRef() == 0 ) delete value;
    value = new_ref;
  }

  // Methods that take input string size (unsafe to call from outside)
  cString & AppendStr(const int in_size, const char * in);  // Optimized
  cString & InsertStr(const int in_size, const char * in,
		      int pos, int excise=0);
  int FindStr(const char * in_string, const int in_size, int pos) const;

  // -- Internal Data --
protected:
  cStringData * value;

// }}} End Internals

};


// {{{ ** External cString Functions **

// iostream input
istream & operator >> (istream & in, cString & string);
ostream & operator << (ostream & out, const cString & string);

// }}}




// Comment this class later
class cBuffer {
private:
  char * data;
  int data_size;
  int buf_size;
  int start_pos;
public:
  cBuffer(int size=8);
  ~cBuffer();

  void Add(char in_char);
  void Add(const cString & in_string);

  char operator[](int pos);

  cString AsString();

  inline int GetSize() { return data_size; }
};


// -- INLINE INCLUDES --

cString::cCharProxy & cString::cCharProxy::operator= (char c){  // lvalue
  string.CopyOnWrite();
  (*(string.value))[index] = c;
  return *this;
}

cString::cCharProxy & cString::cCharProxy::operator+= (char c){  // lvalue
  string.CopyOnWrite();
  (*(string.value))[index] += c;
  return *this;
}

cString::cCharProxy & cString::cCharProxy::operator-= (char c){  // lvalue
  string.CopyOnWrite();
  (*(string.value))[index] -= c;
  return *this;
}

cString::cCharProxy & cString::cCharProxy::operator++ (){  // lvalue (prefix)
  string.CopyOnWrite();
  ++(*(string.value))[index];
  return *this;
}

char cString::cCharProxy::operator++ (int dummy){  // lvalue (postfix)
  (void)dummy;
  char rv = (*(string.value))[index];
  string.CopyOnWrite();
  ++(*(string.value))[index];
  return rv;
}

cString::cCharProxy & cString::cCharProxy::operator-- (){  // lvalue (prefix)
  string.CopyOnWrite();
  --(*(string.value))[index];
  return *this;
}

char cString::cCharProxy::operator-- (int dummy){  // lvalue (postfix)
  (void)dummy;
  char rv = (*(string.value))[index];
  string.CopyOnWrite();
  --(*(string.value))[index];
  return rv;
}

cString::cCharProxy::operator char () const {  // rvalue
  return (char)((*(string.value))[index]);
  // Below should work, but fails in HP CC -- TCC
  //   return const_cast(const cStringData&, *(string.value))[index];
}

#endif

