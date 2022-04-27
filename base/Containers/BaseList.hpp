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

#ifndef BASELIST_HPP
#define BASELIST_HPP


#include "Sys/Types.hpp"

//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------


//------------------------------------------------------------------------
// Forward Declarations
//------------------------------------------------------------------------

class   BaseListElement;
typedef BaseListElement *  PBaseListElement;
class   BaseListItr;
typedef BaseListItr *  PBaseListItr;
class   BaseList;
typedef BaseList *  PBaseList;


//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------


/**
  The BaseListElement class stores null pointers in a linked list.  Elements link to one
    another, and BaseList contains them.  The BaseListElement class stores pointers to the actual
    data.
  */

//-----------------------------------------------------------------------------
class BaseListElement
  {
  private:
                       /// points to the actual data being stored in the list.  BaseList* does not allocate or deallocate the memory at this pointer.
    void *             pvData;
                       /// points to the previous element in the doubly linked list.
    BaseListElement *  pPrev;
                       /// points to the next element in the doubly linked list.
    BaseListElement *  pNext;

  public:

                       BaseListElement  ()                        {SetPrev (NULL); SetNext (NULL); pvData = NULL; };
    explicit           BaseListElement  (void *  pvIn)            {SetPrev (NULL); SetNext (NULL); pvData = NULL; SetDataPtr (pvIn);};
                       ~BaseListElement ()                        {};

    VOID               SetPrev          (BaseListElement *  pPrevIn)  {pPrev = pPrevIn;};
    VOID               SetNext          (BaseListElement *  pNextIn)  {pNext = pNextIn;};
    VOID               SetDataPtr       (void *             pvIn)     {pvData = pvIn; };

    BaseListElement *  GetPrev          (VOID)                        {return pPrev;};
    BaseListElement *  GetNext          (VOID)                        {return pNext;};
    void *             GetDataPtr       (VOID)                        {return pvData;};
  };


/**
  The BaseList class stores a linked list of BaseListElement objects, which in turn contain the
    attributes.
  */
//-----------------------------------------------------------------------------
class BaseList
  {
  private:

    BaseListElement    elemBegin;    // sentry node for a doubly-linked list
    BaseListElement    elemEnd;      // sentry node for a doubly-linked list

  public:


                       BaseList      ();
                       BaseList      (const BaseList &  listIn);
                       ~BaseList     ();

    void *             PushBackPtr   (void *  pvIn);
    void *             PushFrontPtr  (void *  pvIn);

    void *             PopBack       (VOID);
    void *             PopFront      (VOID);

    VOID               InsertBefore  (BaseListElement *  pNew,
                                      BaseListElement *  pInsertBefore);
    VOID               Delete        (BaseListElement *  pToDel);
    VOID               Delete        (BaseListItr &      itrIn);
    INT32              Count         (VOID);
    VOID               Clear         (VOID);

    BaseListItr        First         (VOID);
    BaseListItr        Last          (VOID);

    BaseListElement *  GetBegin      (VOID)                        {return &elemBegin;};
    BaseListElement *  GetEnd        (VOID)                        {return &elemEnd;};
    BaseList&          operator=     (const BaseList&  listIn);
    void               Set           (const BaseList&  listIn)     {*this = listIn;};
    BaseListItr        FindByPtr     (void *  ptrAttrIn);
  };



// Note:  BaseListElements may need to be stored in a pool in the future for performance.


/**
  The BaseListItr class implements an iterator used to traverse an BaseList.
  */
//-----------------------------------------------------------------------------
class BaseListItr
  {
  private:
    BaseListElement *  pelemCurr;
    BaseList *         plistOwner;

  public:
                       BaseListItr  ()                                  {pelemCurr = NULL; plistOwner = NULL;};
                       BaseListItr  (BaseListElement *   pelemCurrIn,
                                     BaseList *          plistOwnerIn)  {pelemCurr = pelemCurrIn; plistOwner = plistOwnerIn;};
                       BaseListItr  (const BaseListItr &  itrIn)        {pelemCurr = itrIn.pelemCurr; plistOwner = itrIn.plistOwner;;}
                       ~BaseListItr ()                                  {};
    BaseListItr&       operator=    (const BaseListItr&  itrIn)         {pelemCurr = itrIn.pelemCurr; plistOwner = itrIn.plistOwner; return *this;};
    VOID               Set          (const BaseListItr&  itrIn)         {*this = itrIn;};
    BaseListItr&       operator++   (VOID)                        {if (pelemCurr != plistOwner->GetEnd ()) {pelemCurr = pelemCurr->GetNext ();}; return *this;};
    BaseListItr&       operator--   (VOID)                        {if (pelemCurr != plistOwner->GetBegin ()) {pelemCurr = pelemCurr->GetPrev ();}; return *this;};
    BaseListItr&       Next         (VOID)                        {if (pelemCurr != plistOwner->GetEnd ()) {pelemCurr = pelemCurr->GetNext ();}; return *this;};
    BaseListItr&       Prev         (VOID)                        {if (pelemCurr != plistOwner->GetBegin ()) {pelemCurr = pelemCurr->GetPrev ();}; return *this;};


    BOOL               IsDone       (VOID)                        {return ((pelemCurr == plistOwner->GetBegin ()) || (pelemCurr == plistOwner->GetEnd ()));};
    BOOL               IsValid      (VOID)                        {return (! IsDone ());};
    void *             GetDataPtr   (VOID)                        {return pelemCurr->GetDataPtr ();};
    BaseListElement *  GetElement   (VOID)                        {return pelemCurr;};
  };


#endif // BASELIST_HPP

