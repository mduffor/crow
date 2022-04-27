/* -----------------------------------------------------------------
                             Linked List

     This module implements a simple linked list class for holding
     void pointers.
     
   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2002-2014, Michael T. Duffy II.  All rights reserved.
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

//-----------------------------------------------------------------------------
//  Header Files
//-----------------------------------------------------------------------------


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Containers/LinkedList.hpp"

//-----------------------------------------------------------------------------
//  LinkedListEntry
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
EStatus  LinkedListEntry::InsertAfter  (LinkedListEntry *  pentIn)
  {

  if (pNext == NULL) return (EStatus::kFailure);

  // check for an invalid entry
  ASSERT (pNext         != NULL);
  ASSERT (pentIn        != NULL);
  ASSERT (pentIn->pPrev == NULL);
  ASSERT (pentIn->pNext == NULL);


  pNext->pPrev  = pentIn;
  pentIn->pNext = pNext;
  pentIn->pPrev = this;
  pNext         = pentIn;

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
VOID LinkedListEntry::Remove  (VOID)
  {


  if (pPrev != NULL)   pPrev->pNext = pNext;
  if (pNext != NULL)   pNext->pPrev = pPrev;

  pNext = NULL;
  pPrev = NULL;
  };


//-----------------------------------------------------------------------------
//  LinkedList
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
LinkedList::LinkedList  ()
  {

  InitializeVars ();
  };


//-----------------------------------------------------------------------------
LinkedList::LinkedList  (LinkedList &  listIn)
  {

  // copy constructor

  InitializeVars ();

  // copy the list

  LinkedListIterator itrCurr;


  for (itrCurr = listIn.GetFirst (); itrCurr.IsValid (); ++itrCurr)
    {
    PushBack (itrCurr.GetValue ());
    };
  };


//-----------------------------------------------------------------------------
LinkedList::~LinkedList ()
  {

  // free the list

  Empty ();
  };


//-----------------------------------------------------------------------------
VOID LinkedList::InitializeVars  ()
  {

  // Initialize the root chain.  The sentinel entries will be the only ones in the chain
  //   with a NULL in either the Next or Prev slot.
  entBeginRoot.SetNext (&entEndRoot);
  entBeginRoot.SetPrev (NULL);
  entEndRoot.  SetNext (NULL);
  entEndRoot.  SetPrev (&entBeginRoot);

  uiSize = 0;
  };


//-----------------------------------------------------------------------------
LinkedListEntry *   LinkedList::NewEntry  (PVOID  pvDataIn)
  {

  return new LinkedListEntry (pvDataIn);
  };


//-----------------------------------------------------------------------------
EStatus  LinkedList::PushEntryFront  (LinkedListEntry *  pentIn)
  {
  EStatus       esReturn;


  esReturn = entBeginRoot.InsertAfter (pentIn);
  if (esReturn == EStatus::kSuccess)
    {
    ++uiSize;
    };

  return (esReturn);
  };


//-----------------------------------------------------------------------------
EStatus  LinkedList::PushEntryBack  (LinkedListEntry *  pentIn)
  {
  EStatus       esReturn;


  esReturn = entEndRoot.InsertBefore (pentIn);
  if (esReturn == EStatus::kSuccess)
    {
    ++uiSize;
    };

  return (esReturn);
  };


//-----------------------------------------------------------------------------
EStatus  LinkedList::PushFront  (PVOID  pvDataIn)
  {

  return (PushEntryFront (new LinkedListEntry (pvDataIn)));
  };


//-----------------------------------------------------------------------------
EStatus  LinkedList::PushBack  (PVOID  pvDataIn)
  {
  return (PushEntryBack (new LinkedListEntry (pvDataIn)));
  };


//-----------------------------------------------------------------------------
PVOID  LinkedList::PopFront  (VOID)
  {
  // remove and return the first item in the list, if there is one.

  PVOID  pvReturn = NULL;
  
  if (entBeginRoot.Next () != &entEndRoot)
    {
    pvReturn = entBeginRoot.Next ()->Data ();
    Delete (entBeginRoot.Next ());
    };
  return (pvReturn);
  };


//-----------------------------------------------------------------------------
PVOID  LinkedList::PopBack  (VOID)
  {
  // remove and return the first item in the list, if there is one.

  PVOID  pvReturn = NULL;
  
  if (entEndRoot.Prev () != &entBeginRoot)
    {
    pvReturn = entEndRoot.Prev ()->Data ();
    Delete (entEndRoot.Prev ());
    };
  return (pvReturn);
  };


//-----------------------------------------------------------------------------
EStatus  LinkedList::Delete  (LinkedListEntry *  pentIn)
  {


  ASSERT (uiSize != 0);

  pentIn->Remove ();
  delete (pentIn);
  if (uiSize > 0) --uiSize;

  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus  LinkedList::Empty  (VOID)
  {
  while (entBeginRoot.Next () != &entEndRoot)
    {
    Delete (entBeginRoot.Next ());
    };
  ASSERT (uiSize == 0);
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
EStatus  LinkedList::EmptyAndFree  (VOID)
  {
  EStatus   esReturn = EStatus::kSuccess;

  while (entBeginRoot.Next () != &entEndRoot)
    {
    if (FreeElement (entBeginRoot.Next ()) != EStatus::kSuccess)
      {
      esReturn = EStatus::kFailure;
      };
    Delete (entBeginRoot.Next ());
    };
  ASSERT (uiSize == 0);
  return (esReturn);
  };


//-----------------------------------------------------------------------------
EStatus  LinkedList::FreeElement  (PLinkedListEntry  pEntryIn)
  {
  // default implementation does nothing.
  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
BOOL  LinkedList::IsEmpty  (VOID)
  {
  return (uiSize == 0);
  };


//-----------------------------------------------------------------------------
UINT32  LinkedList::Size  (VOID)
  {
  return (uiSize);
  };


//-----------------------------------------------------------------------------
LinkedListIterator  LinkedList::GetFirst  (VOID)
  {
  return (LinkedListIterator (entBeginRoot.Next ()));
  };


//-----------------------------------------------------------------------------
LinkedListIterator  LinkedList::GetLast  (VOID)
  {
  return (LinkedListIterator (entEndRoot.Prev ()));
  };

//-----------------------------------------------------------------------------
BOOL  LinkedList::Find  (PVOID                  pvDataIn,
                          LinkedListIterator &  itrOut)
  {
  LinkedListIterator itrCurr;
  INT                 iCount = 0;


  for (itrCurr = GetFirst (); itrCurr.IsValid (); ++itrCurr)
    {
    // make sure you don't get into an infinate loop
    if (itrCurr == GetFirst ()) ++iCount;
    if (iCount > 1) break;
    
    if (itrCurr.GetValue () == pvDataIn)
      {
      itrOut = itrCurr;
      return (TRUE);
      };
    };
  // didn't find it
  return (FALSE);
  };



