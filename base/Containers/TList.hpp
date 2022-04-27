/* -----------------------------------------------------------------
                        Templated Linked List

     This module implements a simple templated linked list class.

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


#ifndef TLIST_HPP
#define TLIST_HPP

#include "Sys/Types.hpp"

// REFACTOR : This module could be sped up by using a pool to control the TListEntry
//   allocations.  Since the use case for TList is almost always to store a list
//   of pointers, this should be relatively straightforward with few problems in
//   clearing/initializing memory.  Perhaps pool storage could be an optional flag
//   that is set.

//------------------------------------------------------------------------
template <class T>
class TListEntry
  {
  private:

    T                tData;
    TListEntry<T> *  pNext;
    TListEntry<T> *  pPrev;

  public:

                                  /** @brief  Constructor
                                      @return None
                                  */
                     TListEntry   ()                           {pNext = pPrev = NULL;};


                                  /** @brief  Constructor
                                      @param  tDataIn This entry's data (usually a pointer).
                                      @return None
                                  */
                     TListEntry   (T   tDataIn)                {tData = tDataIn; pNext = pPrev = NULL;};

                                  /** @brief  Destructor
                                      @return None
                                  */
                     ~TListEntry  ()                           {Remove ();};

                                  /** @brief  Sets this entry's data (usually a pointer)
                                      @param  tDataIn The entry's data
                                      @return None
                                  */
    VOID             SetData      (T   tDataIn)                {tData = tDataIn;};

                                  /** @brief  Gets the pointer to the entry's data
                                      @return The pointer to the data.
                                  */
    T                Data         (VOID) const                 {return tData;};

                                  /** @brief  Returns a pointer to the next entry in the list
                                      @return The pointer to the next entry
                                  */
    TListEntry<T> *  Next         (VOID) const                 {return pNext;};

                                  /** @brief  Returns a pointer to the previous entry in the list
                                      @return The pointer to the previous entry
                                  */
    TListEntry<T> *  Prev         (VOID) const                 {return pPrev;};

                                  /** @brief  Inserts an entry after this one.  The pNext and pPrev pointers are
                                              updated for this entry, the one that currently follows it, and
                                              the passed entry.
                                      @param  pentIn Pointer to the entry that will be inserted after this entry.
                                      @return True on success, False on failure.
                                  */
    BOOL             InsertAfter  (TListEntry<T> *  pentIn)    {
                                                               if (pNext == NULL)
                                                                 {
                                                                 // REFACTOR: This only works if the calling file include Debug.hpp
                                                                 DBG_ERROR ("InsertAfter failed because pNext is null");
                                                                 return (FALSE);
                                                                 };
                                                               // check for an invalid entry
                                                               //ASSERT ((pNext != NULL) && (pentIn != NULL) && (pentIn->pPrev == NULL) && (pentIn->pNext == NULL));
                                                               pNext->pPrev  = pentIn;
                                                               pentIn->pNext = pNext;
                                                               pentIn->pPrev = this;
                                                               pNext         = pentIn;
                                                               return (TRUE);
                                                               };

                                  /** @brief  Inserts an entry before this one.  The pNext and pPrev pointers are
                                              updated for this entry, the one that currently precedes it, and
                                              the passed entry.
                                      @param  pentIn Pointer to the entry that will be inserted before this entry.
                                      @return True on success, False on failure.
                                  */
    BOOL             InsertBefore (TListEntry<T> *  pentIn)    {
                                                               return (pPrev == NULL) ? FALSE : pPrev->InsertAfter (pentIn);
                                                               };

                                  /** @brief  Removes this entry from the linked list.  The pNext and pPrev pointers
                                              are updated for the entries that follow and precede this entry,
                                              and the pNext and pPrev pointers for this entry are set to NULL.
                                      @return None
                                  */
    VOID             Remove       (VOID)                       {
                                                               if (pPrev != NULL)   pPrev->pNext = pNext;
                                                               if (pNext != NULL)   pNext->pPrev = pPrev;
                                                               pNext = pPrev = NULL;
                                                               };

                                  /** @brief  This routine simply sets the value of the pNext pointer.  Intended for
                                              use in initialization.
                                      @param  pentIn The value for pNext
                                      @return None
                                  */
    VOID             SetNext      (TListEntry<T> *  pentIn)    {pNext = pentIn;};

                                  /** @brief  This routine simply sets the value of the pPrev pointer.  Intended for
                                              use in initialization.
                                      @param  pentIn The value for pPrev
                                      @return None
                                  */
    VOID             SetPrev      (TListEntry<T> *  pentIn)    {pPrev = pentIn;};

                                  /** @brief  Tests whether or not this element is in a list.
                                      @return True if in a list, false if a sentinel or not currently in a list.
                                  */
    BOOL             IsInList     (VOID)                       {return ((Next() != NULL) && (Prev() != NULL));};

    VOID             Swap         (TListEntry<T> *  pIn)       {
                                                               TListEntry<T> *  pTemp;
                                                               pTemp = pNext;
                                                               pNext = pIn->pNext;
                                                               pIn->pNext = pTemp;
                                                               pTemp = pPrev;
                                                               pPrev = pIn->pPrev;
                                                               pIn->pPrev = pTemp;
                                                               };

  };


//------------------------------------------------------------------------
template <class T>
class TListItr
  {
  private:

    T                   tUninitialized;
    TListEntry<T> *     pentCurr;

  public:

                   TListItr    ()                            {pentCurr = NULL; tUninitialized = T();};
    explicit       TListItr    (TListItr<T> *  pitrIn)       {pentCurr = pitrIn->pentCurr; tUninitialized = T();};
                   TListItr    (const TListItr<T> &  itrIn)  {pentCurr = itrIn.pentCurr; tUninitialized = T();};
    explicit       TListItr    (TListEntry<T> *  pentIn)     {pentCurr = pentIn; tUninitialized = T();};
                   ~TListItr   ()                            {};

    T              GetValue    (VOID)    {return (pentCurr == NULL) ? tUninitialized : pentCurr->Data();};
    BOOL           IsValid     (VOID)    {return ((pentCurr != NULL) && (pentCurr->IsInList()));};
    TListItr<T> &  operator++  (VOID)    {if (pentCurr->Next () != NULL) {pentCurr = pentCurr->Next ();}; return (*this);};
    TListItr<T> &  operator--  (VOID)    {if (pentCurr->Prev () != NULL) {pentCurr = pentCurr->Prev ();}; return (*this);};
    BOOL           operator==  (const TListItr<T> &  itrIn)  {return (itrIn.pentCurr == pentCurr);};
    TListItr<T> &  operator=   (const TListItr<T> &  itrIn)  {pentCurr = itrIn.pentCurr;  tUninitialized = T(); return (*this);};
    T              operator*   ()                            {return pentCurr->Data ();};

    VOID           Next        (VOID)    {++(*this);};
    VOID           Prev        (VOID)    {--(*this);};

    // For Internal Use Only
    TListEntry<T> *  GetEntryPtr (VOID)  {return pentCurr;};

  };


//------------------------------------------------------------------------
template <class T>
class TList
  {
  private:

    TListEntry<T>       entBeginRoot; ///< Sentinel entry for the start of the list.  pPrev will always be NULL.
    TListEntry<T>       entEndRoot;   ///< Sentinel entry for the end of the list.  pNext will always be NULL.
    UINT32              uiSize;       ///< The number of elements in the list.


  protected:
                                        /** @brief  Sets the initial value of internal varibles.
                                            @return None
                                        */
    VOID                InitializeVars  ()                           {
                                                                     // Initialize the root chain.  The sentinel entries will be the only ones in the chain
                                                                     //   with a NULL in either the Next or Prev slot.
                                                                     entBeginRoot.SetNext (&entEndRoot);
                                                                     entBeginRoot.SetPrev (NULL);
                                                                     entEndRoot.  SetNext (NULL);
                                                                     entEndRoot.  SetPrev (&entBeginRoot);
                                                                     uiSize = 0;
                                                                     };

                                        /** @brief  Allocates and returns a new linked list entry, though the entry is not
                                                    inserted into the list itself.
                                            @param  tDataIn The data value the new entry will be initialized with.
                                            @return None
                                        */
    TListEntry<T> *     NewEntry        (T    tDataIn)              {return new TListEntry<T> (tDataIn);};

                                        /** @brief  Removes the given entry from the list, and deletes it.
                                            @param  pentIn  The entry to delete.
                                        */
    VOID                DeleteEntry     (TListEntry<T> *  pentIn)   {//ASSERT (uiSize != 0);
                                                                     pentIn->Remove ();
                                                                     delete (pentIn);
                                                                     if (uiSize > 0) --uiSize;};

                                        /** @brief  Inserts the passed entry into the beginning of the list.
                                            @param  pentIn The entry to insert.
                                            @return None
                                        */
    VOID                PushEntryFront  (TListEntry<T> *  pentIn)   {if (entBeginRoot.InsertAfter (pentIn)) {++uiSize;};};

                                        /** @brief  Inserts the passed entry into the end of the list
                                            @param  pentIn The entry to insert.
                                            @return None
                                        */
    VOID                PushEntryBack   (TListEntry<T> *  pentIn)   {
                                                                    if (entEndRoot.InsertBefore (pentIn))
                                                                      {
                                                                      ++uiSize;
                                                                      }
                                                                    else
                                                                      {
                                                                      // REFACTOR: This only works if the calling file include Debug.hpp
                                                                      DBG_ERROR ("PushEntryBack Failed.");
                                                                      };
                                                                    };

  public:

                                        /** @brief  Constructor
                                            @return None
                                        */
                        TList           ()                           {InitializeVars ();};

                                        /** @brief  Copy Constructor.  A complete copy of the given list will be made.
                                            @param  listIn The list which is to be copied.
                                            @return None
                                        */
                        TList           (const TList<T> &  listIn)   {InitializeVars ();

                                                                      for (TListItr<T> itrCurr = listIn.First ();
                                                                           itrCurr.IsValid ();
                                                                           ++itrCurr)
                                                                        {
                                                                        PushBack (itrCurr.GetValue ());
                                                                        };
                                                                      };

                                        /** @brief  Destructor
                                            @return None
                                        */
    virtual             ~TList          ()                           {
                                                                     ASSERT (Validate ());
                                                                     Empty ();
                                                                     };

                                        /** @brief  Creates a new entry at the beginning of the list, initialized with the passed data.
                                            @param  tDataIn The data to add to the list.
                                            @return Returns The passed data that was inserted.
                                        */
    T                   PushFront       (T    tDataIn)              {PushEntryFront (new TListEntry<T> (tDataIn)); return (tDataIn);};

                                        /** @brief  Creates a new entry at the end of the list, initialized with the passed data.
                                            @param  tDataIn The data to add to the list
                                            @return Returns The passed data that was inserted.
                                            NOTE: Returns the passed data, which if passed by value will return the value, but not the actual object/memory stored.
                                        */
    T                   PushBack        (T    tDataIn)              {
                                                                    //DBG_INFO ("PushBack before end(%x):prev(%x)", &entEndRoot, entEndRoot.pPrev);
                                                                    PushEntryBack (new TListEntry<T> (tDataIn));
                                                                    //DBG_INFO ("PushBack after end(%x):prev(%x)", &entEndRoot, entEndRoot.pPrev);
                                                                    return (tDataIn);
                                                                    };

                                        /** @brief  Creates a new entry before the given list entry, initialized with the passed data.
                                            @param  tDataIn The data to insert into the list.
                                            @param  itrIn An iterator into the data before which the data will be inserted.
                                            @return Returns The passed data that was inserted.
                                        */
    T                   InsertBefore   (T                tDataIn,
                                        TListItr<T> &    itrIn)   {
                                                                  if (itrIn.IsValid ())
                                                                    {
                                                                    itrIn.GetEntryPtr()->InsertBefore (new TListEntry<T> (tDataIn));
                                                                    }
                                                                  else
                                                                    {
                                                                    PushFront (tDataIn);
                                                                    };
                                                                  ++uiSize;
                                                                  return (tDataIn);
                                                                  };

                                        /** @brief  Creates a new entry before the given list entry, initialized with the passed data.
                                            @param  tDataIn The data to insert into the list.
                                            @param  itrIn An iterator into the data after which the data will be inserted.
                                            @return Returns The passed data that was inserted.
                                        */
    T                   InsertAfter    (T                tDataIn,
                                        TListItr<T> &    itrIn)   {
                                                                  if (itrIn.IsValid ())
                                                                    {
                                                                    itrIn.GetEntryPtr()->InsertAfter (new TListEntry<T> (tDataIn));
                                                                    }
                                                                  else
                                                                    {
                                                                    PushBack (tDataIn);
                                                                    };
                                                                  ++uiSize;
                                                                  return (tDataIn);
                                                                  };

                                        /** @brief  Removes the first entry from the list, and returns its data pointer.
                                            @return Returns the contents of the entry's data, or NULL if the list is empty.  (call IsEmpty () to truly check if the list is empty).
                                        */
    T                   PopFront        (VOID)                      {if (IsEmpty()) return ((T)0);
                                                                     T  pReturn = entBeginRoot.Next ()->Data ();
                                                                     DeleteEntry (entBeginRoot.Next ());
                                                                     return (pReturn);};

                                        /** @brief  Returns the value of the first entry from the list.
                                            @return Returns the contents of the entry's data, or NULL if the list is empty.
                                        */
    T                   PeekFront       (VOID)                      {if (IsEmpty()) return ((T)0);
                                                                     return entBeginRoot.Next ()->Data ();};


                                        /** @brief  Removes the last entry from the list, and returns its data pointer.
                                            @return Returns the contents of the entry's data, or NULL if the list is empty.  (call IsEmpty () to truly check if the list is empty).
                                        */
    T                   PopBack         (VOID)                      {if (IsEmpty()) return ((T)0);
                                                                     T  pReturn = entEndRoot.Prev ()->Data ();
                                                                     DeleteEntry (entEndRoot.Prev ());
                                                                     return (pReturn);};

    T                   PeekBack        (VOID)                      {if (IsEmpty()) return ((T)0);
                                                                     return (entEndRoot.Prev ()->Data ());
                                                                     };


                                        /** @brief  Removes the given entry from the list, and deletes it.
                                            @param  pIn  The entry to delete.
                                        */
    VOID                Delete          (T    pIn)                   {
                                                                     for (TListEntry<T> *  pCurr = entBeginRoot.Next();
                                                                          pCurr != &entEndRoot;
                                                                          pCurr = pCurr->Next ())
                                                                       {
                                                                       if (pCurr->Data() == pIn)
                                                                         {
                                                                         DeleteEntry (pCurr);
                                                                         return;
                                                                         }
                                                                       }
                                                                     };

                                        /** @brief  Removes the given entry from the list, and deletes it.
                                            @param  itrIn  An iterator pointing to the entry to delete.
                                        */
    VOID                Delete          (TListItr<T> &  itrIn)      {TListEntry<T> *  pEntry = itrIn.GetEntryPtr();
                                                                     if (pEntry != NULL) {pEntry->Remove ();};
                                                                     delete (pEntry);
                                                                     if (uiSize > 0) --uiSize;};

                                        /** @brief  Deletes all entries out of the list.  Note that the items pointed to by the entries are not affected.
                                        */
    VOID                Empty           (VOID)                      {
                                                                    while (entBeginRoot.Next () != &entEndRoot)
                                                                      {
                                                                      DeleteEntry (entBeginRoot.Next ());
                                                                      };
                                                                    };


                                        /** @brief  Query if the list has any entries.
                                            @return True if the list is empty, false if it isn't.
                                        */
    BOOL                IsEmpty         (VOID) const   {return (uiSize == 0);};

                                        /** @brief  Query the number of entries in the list.
                                            @return The number of entries in the list.
                                        */
    UINT32              Size            (VOID) const   {return (uiSize);};

                                        /** @brief  Get an iterator to the first element in the list.
                                            @return An iterator to the first list element.
                                        */
    TListItr<T>         First           (VOID) const   {return (TListItr<T> (entBeginRoot.Next ()));};

                                        /** @brief  Get an iterator to the last element in the list.
                                            @return An iterator to the last list element.
                                        */
    TListItr<T>         Last            (VOID) const   {return (TListItr<T> (entEndRoot.Prev ()));};

                                        /** @brief  Get an iterator to the element at the given index in the list.
                                            @param  iIndexIn The 0-based index to the desired data.
                                            @return An iterator to the indexed list element.
                                        */
    TListItr<T>         AtIndex         (INT  iIndexIn) const
                                                        {
                                                        TListItr<T> itrCurr = First ();
                                                        for (; itrCurr.IsValid (); ++itrCurr)
                                                          {
                                                          if (--iIndexIn < 0)
                                                            {
                                                            break;
                                                            };
                                                          };
                                                        return (itrCurr);
                                                        };

                                        /** @brief  Search the list for an entry that matches the given data.
                                            @param  tDataIn The data which will be compared to the list entries.
                                            @param  itrOut The iterator to set to the found item, if successful.
                                            @return TRUE if found, FALSE if not.
                                        */
    BOOL                Find            (T              tDataIn,
                                         TListItr<T> &  itrOut)
                                                {
                                                for (TListItr<T> itrCurr = First ();
                                                      itrCurr.IsValid ();
                                                      ++itrCurr)
                                                  {
                                                  if (itrCurr.GetValue () == tDataIn)
                                                    {
                                                    itrOut = itrCurr;
                                                    return (TRUE);
                                                    };
                                                  };
                                                return (FALSE);
                                                };

                                        /** @brief  Search the list for an entry that matches the given data.
                                            @param  tDataIn The data which will be compared to the list entries.
                                            @return TRUE if found, FALSE if not.
                                        */
    BOOL                Contains        (T              tDataIn)
                                                {
                                                for (TListItr<T> itrCurr = First ();
                                                      itrCurr.IsValid ();
                                                      ++itrCurr)
                                                  {
                                                  if (itrCurr.GetValue () == tDataIn)
                                                    {
                                                    return (TRUE);
                                                    };
                                                  };
                                                return (FALSE);
                                                };


    VOID                Reverse         (VOID)  {
                                                if (Size () == 0) return;
                                                // we will move entries to a temp list, then reverse order as we move them back.
                                                TListEntry<T>       entBeginTemp; // temp list sentinels
                                                TListEntry<T>       entEndTemp;

                                                entBeginTemp.SetNext (entBeginRoot.Next ());  entBeginTemp.SetPrev (NULL); // move list to temp sentinels
                                                entEndTemp.  SetNext (NULL);                  entEndTemp.  SetPrev (entEndRoot.Prev ());

                                                entBeginTemp.Next ()->SetPrev (&entBeginTemp); entEndTemp.Prev ()->SetNext   (&entEndTemp);
                                                entBeginRoot.SetNext (&entEndRoot);           entEndRoot.  SetPrev (&entBeginRoot); // remove from main list

                                                while (entBeginTemp.Next () != &entEndTemp) // revese list by pop front temp / push front main
                                                  {
                                                  TListEntry<T> *  pTemp = entBeginTemp.Next ();
                                                  pTemp->Remove ();
                                                  entBeginRoot.InsertAfter (pTemp);
                                                  };
                                                };


                                        /** @brief  Checks and makes sure the list is valid and that entBeginRoot and entEndRoot lead to each other
                                            return TRUE if valid, FALSE if not
                                        */
    BOOL                Validate       (VOID)   {
                                                for (TListEntry<T> *  pCurr = entBeginRoot.Next();
                                                     pCurr != NULL;
                                                     pCurr = pCurr->Next ())
                                                  {
                                                  if (pCurr == &entEndRoot)
                                                    {
                                                    return (TRUE);
                                                    }
                                                  };
                                                return (FALSE);
                                                };


                                        /* @brief Perform an insertion sort of the list, given the comparison function.
                                                  Comparison fn (a,b) should return -1 if a<b, 0 if a==b, or 1 if a>b
                                        */
    VOID                Sort           (INT (*Compare)(T,T))
                                                {
                                                TListEntry<T> *  pCurr = entBeginRoot.Next();
                                                TListEntry<T> *  pNext = NULL;
                                                if (pCurr != NULL) pCurr = pCurr->Next();
                                                for (;
                                                     (pCurr != NULL) && (pCurr != &entEndRoot);
                                                     pCurr = pNext)
                                                  {
                                                  pNext = pCurr->Next();
                                                  if ((*Compare)(pCurr->Prev()->Data(), pCurr->Data()) > 0)
                                                    {
                                                    // out of order.  Move
                                                    TListEntry<T> *  pSearch = pCurr->Prev();
                                                    pCurr->Remove ();
                                                    while (pSearch->Prev () != NULL)
                                                      {
                                                      if ((*Compare)(pSearch->Data(), pCurr->Data()) <= 0) break;
                                                      pSearch = pSearch->Prev();
                                                      };
                                                    pSearch->InsertAfter (pCurr);
                                                    };
                                                  };
                                                };


                                        /* @brief Swap the positions of two list entries in the linked list.
                                        */
    VOID                Swap           (T              tDataOneIn,
                                        T              tDataTwoIn)
                                                {
                                                TListItr<T>  itrOne;
                                                TListItr<T>  itrTwo;
                                                if (Find (tDataOneIn, itrOne) && Find (tDataTwoIn, itrTwo))
                                                  {
                                                  TListEntry<T> *  pEntryOne = itrOne.GetEntryPtr ();
                                                  TListEntry<T> *  pEntryTwo = itrTwo.GetEntryPtr ();
                                                  pEntryOne->Swap (pEntryTwo);
                                                  };
                                                };








  };

#endif // TLIST_HPP






