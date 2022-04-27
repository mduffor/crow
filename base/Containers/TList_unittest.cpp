#include <gtest/gtest.h>

#include "Sys/Types.hpp"
#include "Debug.hpp"
#include "Util/RStr.hpp"

ASSERTFILE (__FILE__);


#include "Containers/KVPArray.hpp"
#include "Util/RStrParser.hpp"
#include "Containers/TList.hpp"

INT IntCompare (INT  a, INT  b);

// NOTE: https://github.com/google/googletest/blob/master/googletest/docs/Primer.md

//------------------------------------------------------------------------------
TEST (TList, PushPeekPop)
  {
  TList<INT>   list;

  list.PushBack  (2);
  list.PushBack  (3);
  list.PushBack  (4);
  list.PushBack  (5);
  list.PushFront (1);

  // test basic Push (above), Peek, and Pop functionality
  ASSERT_EQ (list.PeekFront(), 1);
  ASSERT_EQ (list.PeekBack(),  5);

  ASSERT_EQ (list.PopFront(), 1);
  ASSERT_EQ (list.PopBack(),  5);

  ASSERT_EQ (list.PeekFront(), 2);
  ASSERT_EQ (list.PeekBack(),  4);
  };

//------------------------------------------------------------------------------
TEST (TList, Copy)
  {
  TList<INT>   list;

  list.PushBack  (2);
  list.PushBack  (3);
  list.PushBack  (4);
  ASSERT_EQ (list.Size(), 3);

  // test making a copy of the list, and modifying it
  TList<INT>   listCopy (list);
  ASSERT_EQ (listCopy.Size(), 3);

  ASSERT_EQ (listCopy.PeekFront(), 2);
  ASSERT_EQ (listCopy.PeekBack(),  4);

  ASSERT_EQ (listCopy.PopFront(), 2);
  ASSERT_EQ (listCopy.PopBack(),  4);

  // make sure the original wasn't modified
  ASSERT_EQ (list.PeekFront(), 2);
  ASSERT_EQ (list.PeekBack(),  4);
  };

//------------------------------------------------------------------------------
TEST (TList, EmptySizeFindContains)
  {
  TList<INT>   list;

  list.PushBack  (2);
  list.PushBack  (3);
  list.PushBack  (4);

  // test Empty, IsEmpty, Size, Find, and Contains

  ASSERT_EQ    (list.Size(), 3);
  ASSERT_FALSE (list.IsEmpty());  // list should be [2,3,4]
  list.Empty();
  ASSERT_EQ    (list.Size(), 0);
  ASSERT_TRUE  (list.IsEmpty());  // list should be []

  list.PushFront (1);
  list.PushBack  (2);
  list.PushBack  (3);
  list.PushBack  (4);
  list.PushBack  (5);
  ASSERT_EQ (list.Size(), 5);

  ASSERT_TRUE (list.Contains(2));  // list should be [1,2,3,4,5]
  ASSERT_TRUE (list.Contains(4));  // list should be [1,2,3,4,5]
  ASSERT_FALSE (list.Contains(0));  // list should be [1,2,3,4,5]
  ASSERT_FALSE (list.Contains(6));  // list should be [1,2,3,4,5]

  list.Delete(3);                   // delete from middle
  ASSERT_EQ    (list.Size(), 4);
  ASSERT_FALSE (list.Contains(3));  // list should be [1,2,4,5]

  list.Delete(1);                   // delete from head
  ASSERT_EQ    (list.Size(), 3);
  ASSERT_FALSE (list.Contains(1));  // list should be [2,4,5]

  list.Delete(5);                   // delete from tail
  ASSERT_EQ    (list.Size(), 2);
  ASSERT_FALSE (list.Contains(5));  // list should be [2,4]

  list.Delete(6);                   // failed delete
  ASSERT_EQ    (list.Size(), 2);
  ASSERT_FALSE (list.Contains(6));  // list should be [2,4]
  };

//------------------------------------------------------------------------------
TEST (TList, Iterators)
  {
  // test iterators
  TList<INT>      list;
  TListItr<INT>   itr;

  list.PushBack  (1);
  list.PushBack  (2);
  list.PushBack  (3);
  list.PushBack  (4);
  list.PushBack  (5);
  ASSERT_EQ (list.Size(), 5);

  itr = list.First();
  ASSERT_EQ   ((*itr), 1);

  itr = list.Last();
  ASSERT_EQ   ((*itr), 5);

  // test AtIndex
  itr = list.AtIndex(3);    // 0-based index
  ASSERT_EQ   ((*itr), 4);

  // test Find
  ASSERT_TRUE (list.Find (4, itr));
  ASSERT_EQ   ((*list.AtIndex(3)), (*itr));

  // test Delete, InsertBefore, InsertAfter

  ASSERT_EQ (list.Size(), 5);
  list.Find (2, itr);
  list.Delete (itr);
  list.Find (4, itr);
  list.Delete (itr);
  ASSERT_EQ (list.Size(), 3);

  ASSERT_EQ ((*list.AtIndex(0)), 1);
  ASSERT_EQ ((*list.AtIndex(1)), 3);
  ASSERT_EQ ((*list.AtIndex(2)), 5);

  list.Find (3, itr);
  ASSERT_TRUE (itr.IsValid());
  list.InsertBefore (2, itr);
  list.InsertAfter  (4, itr);
  ASSERT_EQ (list.Size(), 5);

  list.Delete (1);
  list.Delete (5);
  ASSERT_EQ (list.Size(), 3);

  TListItr<INT>   itrFirst = list.First();
  TListItr<INT>   itrLast  = list.Last();

  list.InsertBefore (1, itrFirst);
  list.InsertAfter  (5, itrLast);

  ASSERT_EQ (list.Size(), 5);
  ASSERT_EQ ((*list.AtIndex(0)), 1);
  ASSERT_EQ ((*list.AtIndex(1)), 2);
  ASSERT_EQ ((*list.AtIndex(2)), 3);
  ASSERT_EQ ((*list.AtIndex(3)), 4);
  ASSERT_EQ ((*list.AtIndex(4)), 5);
  }

//------------------------------------------------------------------------------
TEST (TList, Algorithms)
  {
  TList<INT>      list;
  TListItr<INT>   itr;

  list.PushBack  (1);
  list.PushBack  (2);
  list.PushBack  (3);
  list.PushBack  (4);
  list.PushBack  (5);
  ASSERT_EQ (list.Size(), 5);

  ASSERT_EQ (list.Size(), 5);
  ASSERT_EQ ((*list.AtIndex(0)), 1);
  ASSERT_EQ ((*list.AtIndex(1)), 2);
  ASSERT_EQ ((*list.AtIndex(2)), 3);
  ASSERT_EQ ((*list.AtIndex(3)), 4);
  ASSERT_EQ ((*list.AtIndex(4)), 5);

  ASSERT_TRUE (list.Validate ());

  list.Reverse ();

  ASSERT_EQ (list.Size(), 5);
  ASSERT_EQ ((*list.AtIndex(0)), 5);
  ASSERT_EQ ((*list.AtIndex(1)), 4);
  ASSERT_EQ ((*list.AtIndex(2)), 3);
  ASSERT_EQ ((*list.AtIndex(3)), 2);
  ASSERT_EQ ((*list.AtIndex(4)), 1);

  // sort a reversed list
  list.Sort (&IntCompare);

  ASSERT_EQ (list.Size(), 5);
  ASSERT_EQ ((*list.AtIndex(0)), 1);
  ASSERT_EQ ((*list.AtIndex(1)), 2);
  ASSERT_EQ ((*list.AtIndex(2)), 3);
  ASSERT_EQ ((*list.AtIndex(3)), 4);
  ASSERT_EQ ((*list.AtIndex(4)), 5);

  // sort a sorted list
  list.Sort (&IntCompare);

  ASSERT_EQ (list.Size(), 5);
  ASSERT_EQ ((*list.AtIndex(0)), 1);
  ASSERT_EQ ((*list.AtIndex(1)), 2);
  ASSERT_EQ ((*list.AtIndex(2)), 3);
  ASSERT_EQ ((*list.AtIndex(3)), 4);
  ASSERT_EQ ((*list.AtIndex(4)), 5);

  // try another order
  list.Empty ();
  list.PushBack  (2);
  list.PushBack  (5);
  list.PushBack  (1);
  list.PushBack  (4);
  list.PushBack  (3);
  ASSERT_EQ (list.Size(), 5);

  list.Sort (&IntCompare);

  ASSERT_EQ (list.Size(), 5);
  ASSERT_EQ ((*list.AtIndex(0)), 1);
  ASSERT_EQ ((*list.AtIndex(1)), 2);
  ASSERT_EQ ((*list.AtIndex(2)), 3);
  ASSERT_EQ ((*list.AtIndex(3)), 4);
  ASSERT_EQ ((*list.AtIndex(4)), 5);

  };

//------------------------------------------------------------------------------
INT IntCompare (INT  a,
                INT  b)
  {
  return ((a == b) ? 0 : (a < b) ? -1 : 1);
  };
