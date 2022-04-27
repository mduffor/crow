/* -----------------------------------------------------------------
                            Base List

     This module implements a basic double linked list class for 
   holding null pointers

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2007, Michael T. Duffy II.  All rights reserved.
//
// Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
// Redistributions of source code must retain the above copyright notice, 
//  this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above copyright notice, 
//  this list of conditions and the following disclaimer in the documentation 
//  and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
//  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
//  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
//  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
//  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
//  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF 
//  THE POSSIBILITY OF SUCH DAMAGE. 

#include "Containers/BaseList.hpp"

//-----------------------------------------------------------------------------
//  BaseList
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
BaseList::BaseList   ()
  {
  elemBegin.SetNext (&elemEnd);
  elemEnd.SetPrev (&elemBegin);
  };

//-----------------------------------------------------------------------------
BaseList::BaseList   (const BaseList &  listIn)
  {
  elemBegin.SetNext (&elemEnd);
  elemEnd.SetPrev (&elemBegin);

  *this = (BaseList &) listIn;
  };


//-----------------------------------------------------------------------------
BaseList::~BaseList  ()
  {
  };


//-----------------------------------------------------------------------------
void *  BaseList::PushBackPtr  (void *  pvIn)
  {
  if (pvIn == NULL)
    {
    return (NULL);
    };

  BaseListElement *  pNew = new BaseListElement (pvIn);
  InsertBefore (pNew, GetEnd ());
  return pNew->GetDataPtr ();
  };


//-----------------------------------------------------------------------------
void *  BaseList::PushFrontPtr  (void *  pvIn)
  {
  if (pvIn == NULL)
    {
    return (NULL);
    };

  BaseListElement *  pNew = new BaseListElement (pvIn);
  InsertBefore (pNew, (GetBegin ())->GetNext ());
  return pNew->GetDataPtr ();
  };


//-----------------------------------------------------------------------------
void * BaseList::PopBack  (VOID)
  {
  void *  retValue = Last ().GetDataPtr ();
  Delete ((Last  ()).GetElement ());
  return retValue;
  };


//-----------------------------------------------------------------------------
void *  BaseList::PopFront  (VOID)
  {
  void *  retValue = First ().GetDataPtr ();
  Delete ((First ()).GetElement ());
  return retValue;
  };


//-----------------------------------------------------------------------------
VOID  BaseList::InsertBefore  (BaseListElement *  pNew,
                                BaseListElement *  pInsertBefore)
  {
  pNew->SetNext (pInsertBefore);
  pNew->SetPrev (pInsertBefore->GetPrev ());
  (pInsertBefore->GetPrev ())->SetNext (pNew);
  pInsertBefore->SetPrev (pNew);
  };


//-----------------------------------------------------------------------------
VOID  BaseList::Delete  (BaseListElement *  pToDel)
  {
  (pToDel->GetPrev())->SetNext (pToDel->GetNext ());
  (pToDel->GetNext())->SetPrev (pToDel->GetPrev ());
  delete pToDel;
  };


//-----------------------------------------------------------------------------
VOID  BaseList::Delete  (BaseListItr &  itrIn)
  {
  BaseListItr  itrNext = itrIn;
  ++itrNext;
  Delete (itrIn.GetElement ());
  itrIn = itrNext;
  };


//-----------------------------------------------------------------------------
INT32  BaseList::Count  (VOID)
  {
  INT32  iCount = 0;
  for (BaseListItr  itrCurr = First (); !itrCurr.IsDone (); ++itrCurr)
    {
    ++iCount;
    };
  return iCount;
  };


//-----------------------------------------------------------------------------
VOID  BaseList::Clear  (VOID)
  {
  // delete all the entries

  BaseListItr  itrCurr = First ();
  BaseListItr  itrNext = itrCurr;
  ++itrNext;

  while (!itrCurr.IsDone ())
    {
    delete (itrCurr.GetElement ());
    itrCurr = itrNext;
    ++itrNext;
    };
  elemBegin.SetNext (&elemEnd);
  elemEnd.SetPrev (&elemBegin);
  };


//-----------------------------------------------------------------------------
BaseListItr  BaseList::First  (VOID)
  {
  return (BaseListItr (elemBegin.GetNext (), this));
  };


//-----------------------------------------------------------------------------
BaseListItr  BaseList::Last     (VOID)
  {
  return (BaseListItr (elemEnd.GetPrev (), this));
  };

//-----------------------------------------------------------------------------
BaseList&  BaseList::operator=  (const BaseList&  listIn)
  {

  // clear our current list
  Clear ();

  // copy over every element in the other list.
  for (BaseListItr  itrCurr = const_cast <BaseList&>(listIn).First (); !itrCurr.IsDone (); ++itrCurr)
    {
    // copy pointer
    PushBackPtr (itrCurr.GetDataPtr ());
    };

  return *this;
  };


//-----------------------------------------------------------------------------
BaseListItr  BaseList::FindByPtr  (void *  pvIn)
  {
  BaseListItr  itrCurr;

  for (itrCurr = First (); !itrCurr.IsDone (); ++itrCurr)
    {
    if (itrCurr.GetDataPtr () == pvIn)
      {
      break;
      };
    };
  return (itrCurr);
  };
