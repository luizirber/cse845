//////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1993 - 2001 California Institute of Technology             //
//                                                                          //
// Read the COPYING and README files, or contact 'avida@alife.org',         //
// before continuing.  SOME RESTRICTIONS MAY APPLY TO USE OF THIS FILE.     //
//////////////////////////////////////////////////////////////////////////////

#ifndef TLIST_HH
#define TLIST_HH

#ifndef NULL
#define NULL 0
#endif

template <class T> class tListNode {
public:
  T * data;
  tListNode<T> * next;
  tListNode<T> * prev;

  tListNode() : data(NULL), next(this), prev(this) { ; }
};

template <class T> class tList;

template <class T> class tBaseIterator {
  friend class tList<T>;
protected:
  virtual const tList<T> & GetConstList() = 0;
  virtual const tListNode<T> * GetConstNode() = 0;
public:
  tBaseIterator() { ; }
  virtual ~tBaseIterator() { ; }

  virtual const T * GetConst() = 0;
  virtual const T * NextConst() = 0;
  virtual const T * PrevConst() = 0;

  virtual bool AtRoot() const = 0;
  virtual bool AtEnd() const = 0;
};

template <class T> class tListIterator : public tBaseIterator<T> {
  friend class tList<T>;
private:
  tList<T> & list;
  tListNode<T> * node;

  const tList<T> & GetConstList() { return list; }
  const tListNode<T> * GetConstNode() { return node; }
public:
  explicit tListIterator(tList<T> & _list);
  ~tListIterator();

  void Reset();

  T * Get();
  T * Next();
  T * Prev();
  const T * GetConst() { return Get(); }
  const T * NextConst() { return Next(); }
  const T * PrevConst() { return Prev(); }

  bool Find(T * test_data);

  bool AtRoot() const;
  bool AtEnd() const;

  // Unique methods...
  T * Remove();
};

template <class T> class tConstListIterator : public tBaseIterator<T> {
  friend class tList<T>;
private:
  const tList<T> & list;
  const tListNode<T> * node;

  const tList<T> & GetConstList() { return list; }
  const tListNode<T> * GetConstNode() { return node; }
public:
  explicit tConstListIterator(const tList<T> & _list);
  ~tConstListIterator();

  void Reset();

  const T * Get();
  const T * Next();
  const T * Prev();
  const T * GetConst() { return Get(); }
  const T * NextConst() { return Next(); }
  const T * PrevConst() { return Prev(); }
  bool Find(const T * test_data);

  bool AtRoot() const;
  bool AtEnd() const;
};

template <class T> class tList {
  friend class tBaseIterator<T>;
  friend class tListIterator<T>;
  friend class tConstListIterator<T>;
protected:
  tListNode<T> root;                     // Data root
  int size;
  mutable tListNode< tBaseIterator<T> > it_root; // Iterator root
  mutable int it_count;

  T * RemoveNode(tListNode<T> * out_node) {
    // Make sure we're not trying to delete the root node!
    if (out_node == &root) return NULL;

    // Adjust any iterators on the deleted node.
    tListNode< tBaseIterator<T> > * test_it = it_root.next;
    while (test_it != &it_root) {
      // If this iterator is on this node, move it back one.
      if (test_it->data->GetConstNode() == out_node) {
	test_it->data->PrevConst();
      }
      test_it = test_it->next;
    }

    // Save the data and patch up the linked list.
    T * out_data = out_node->data;
    out_node->prev->next = out_node->next;
    out_node->next->prev = out_node->prev;

    // Cleanup and return
    size--;
    delete out_node;
    return out_data;
  }

  // To be called from iterator constructor only!
  void AddIterator(tBaseIterator<T> * new_it) const {
    tListNode< tBaseIterator<T> > * new_node =
      new tListNode< tBaseIterator<T> >;
    new_node->data = new_it;
    new_node->next = it_root.next;
    new_node->prev = &it_root;
    it_root.next->prev = new_node;
    it_root.next = new_node;
    it_count++;
  }

  // To be called from iterator destructor only!
  void RemoveIterator(tBaseIterator<T> * old_it) const {
    tListNode< tBaseIterator<T> > * test_it = it_root.next;
    while (test_it->data != old_it) test_it = test_it->next;
    test_it->prev->next = test_it->next;
    test_it->next->prev = test_it->prev;
    delete test_it;
    it_count--;
  }

public:
  T * Pop() { return RemoveNode(root.next); }
  T * PopRear() { return RemoveNode(root.prev); }

  void Push(T * _in) {
    tListNode<T> * new_node = new tListNode<T>;
    new_node->data = _in;
    new_node->next = root.next;
    new_node->prev = &root;
    root.next->prev = new_node;
    root.next = new_node;
    size++;
  }

  void PushRear(T * _in) {
    tListNode<T> * new_node = new tListNode<T>;
    new_node->data = _in;
    new_node->next = &root;
    new_node->prev = root.prev;
    root.prev->next = new_node;
    root.prev = new_node;
    size++;
  }

  void Clear() { while (size > 0) Pop(); }

  const T * GetFirst() const { return root.next->data; }
  const T * GetLast()  const { return root.prev->data; }
  T * GetFirst()             { return root.next->data; }
  T * GetLast()              { return root.prev->data; }

  T * GetPos(int pos) {
    if (pos >= GetSize()) return NULL;
    tListNode<T> * test_node = root.next;
    for (int i = 0; i < pos; i++) test_node = test_node->next;
    return test_node->data;
  }

  void CircNext() { if (size > 0) PushRear(Pop()); }
  void CircPrev() { if (size > 0) Push(PopRear()); }

  T * Remove(tListIterator<T> & other) {
    if (&(other.list) != this) return NULL;
    return RemoveNode(other.node);
  }

  T * Insert(tBaseIterator<T> & list_it, T * in_data) {
    tListNode<T> * new_node = new tListNode<T>;
    new_node->data = in_data;
    new_node->next = list_it.node->next;
    new_node->prev = list_it.node;

    list_it.node->next->prev = new_node;
    list_it.node->next = new_node;
    size++;
    return in_data;
  }


  bool Remove(T * other) {
    tListNode<T> * test = root.next;
    while (test != &root) {
      if (test->data == other) {
	RemoveNode(test);
	return true;
      }
      test = test->next;
    }
    return false;
  }

  int GetSize() const { return size; }

  void Append(tList<T> & other_list) {
    tListIterator<T> other_it(other_list);
    while (other_it.Next() != NULL) PushRear(other_it.Get());
  }

  T * Find(T * _in) const {
    tListNode<T> * test = root.next;
    while (test != &root) {
      if ( *(test->data) == *(_in) ) return test->data;
      test = test->next;
    }
    return NULL;
  }

  T * PopIntValue(int (T::*fun)() const, int value) {
    tListNode<T> * test = root.next;
    while (test != &root) {
      if ( (test->data->*fun)() == value) return RemoveNode(test);
      test = test->next;
    }
    return NULL;
  }

  T * PopIntMax(int (T::*fun)() const) { 
    if (size == 0) return NULL;
    tListNode<T> * test = root.next;
    tListNode<T> * best = test;
    int max_val = (test->data->*fun)();
    while (test != &root) {
      const int cur_val = (test->data->*fun)();
      if ( cur_val > max_val ) {
	max_val = cur_val;
	best = test;
      }
      test = test->next;
    }
    return RemoveNode(best);
  }

  T * PopDoubleMax(double (T::*fun)() const) {
    if (size == 0) return NULL;
    tListNode<T> * test = root.next;
    tListNode<T> * best = test;
    double max_val = (test->data->*fun)();
    while (test != &root) {
      const double cur_val = (test->data->*fun)();
      if ( cur_val > max_val ) {
	max_val = cur_val;
	best = test;
      }
      test = test->next;
    }
    return RemoveNode(best);
  }
public:
  tList() : size(0), it_count(0) { }
  ~tList() { Clear(); }
private:
  tList(tList & _list) { ; }  // Never should be used...
};

////////////////////
//  tListIterator

template <class T> tListIterator<T>::tListIterator(tList<T> & _list)
  : list(_list), node(&(_list.root))
{
  list.AddIterator(this);
}

template <class T> tListIterator<T>::~tListIterator()
{
  list.RemoveIterator(this);
}

template <class T> void tListIterator<T>::Reset()
{
  node = &(list.root);
}

template <class T> T * tListIterator<T>::Get()
{
  return node->data;
}

template <class T> T * tListIterator<T>::Next()
{
  node = node->next;
  return node->data;
}

template <class T> T * tListIterator<T>::Prev()
{
  node = node->prev;
  return node->data;
}

template <class T> bool tListIterator<T>::Find(T * test_data)
{
  for (node = list.root.next;
       node != &(list.root);
       node = node->next) {
    if (node->data == test_data) return true;
  }
  return false;
}

template <class T> bool tListIterator<T>::AtRoot() const
{
  return (node == &(list.root));
}

template <class T> bool tListIterator<T>::AtEnd() const
{
  return (node->next == &(list.root));
}

template <class T> T* tListIterator<T>::Remove()
{
  return list.RemoveNode(node);
}

/////////////////////////
//  tConstListIterator

template <class T> tConstListIterator<T>::tConstListIterator(const tList<T> & _list)
  : list(_list), node(&(_list.root))
{
  list.AddIterator(this);
}

template <class T> tConstListIterator<T>::~tConstListIterator()
{
  list.RemoveIterator(this);
}

template <class T> void tConstListIterator<T>::Reset()
{
  node = &(list.root);
}

template <class T> const T * tConstListIterator<T>::Get()
{
  return node->data;
}

template <class T> const T * tConstListIterator<T>::Next()
{
  node = node->next;
  return node->data;
}

template <class T> const T * tConstListIterator<T>::Prev()
{
  node = node->prev;
  return node->data;
}

template <class T> bool tConstListIterator<T>::Find(const T * test_data)
{
  for (node = list.root.next;
       node != &(list.root);
       node = node->next) {
    if (node->data == test_data) return true;
  }
  return false;
}

template <class T> bool tConstListIterator<T>::AtRoot() const
{
  return (node == &(list.root));
}

template <class T> bool tConstListIterator<T>::AtEnd() const
{
  return (node->next == &(list.root));
}

#endif
