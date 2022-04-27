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


#ifndef LINKEDLIST_HPP
#define LINKEDLIST_HPP

#include "Sys/Types.hpp"

// Linked List Container


class   LinkedList;
typedef LinkedList *  PLinkedList;
class   LinkedListEntry;
typedef LinkedListEntry *  PLinkedListEntry;
class   LinkedListIterator;
typedef LinkedListIterator *  PLinkedListIterator;


//------------------------------------------------------------------------
class LinkedListEntry
  {
  private:

    PVOID              pvData;
    LinkedListEntry *  pNext;
    LinkedListEntry *  pPrev;

  public:

                                           /** @brief  Constructor
                                               @return None
                                           */
                        LinkedListEntry   ()                            {pvData = NULL; pNext = pPrev = NULL;};


                                           /** @brief  Constructor
                                               @param  pvDataIn A pointer to this entry's data.
                                               @return None
                                           */
    explicit            LinkedListEntry   (PVOID  pvDataIn)             {pvData = pvDataIn; pNext = pPrev = NULL;};

                                           /** @brief  Destructor
                                               @return None
                                           */
                        ~LinkedListEntry  ()                            {Remove ();};


                                           /** @brief  Sets the pointer to this entry's data
                                               @param  pvDataIn A pointer to the entry's data
                                               @return None
                                           */
    VOID                SetData            (PVOID  pvDataIn)             {pvData = pvDataIn;};

                                           /** @brief  Gets the pointer to the entry's data
                                               @return The pointer to the data.
                                           */
    PVOID               Data               (VOID)                        {return pvData;};

                                           /** @brief  Returns a pointer to the next entry in the list
                                               @return The pointer to the next entry
                                           */
    LinkedListEntry *   Next               (VOID)                        {return pNext;};

                                           /** @brief  Returns a pointer to the previous entry in the list
                                               @return The pointer to the previous entry
                                           */
    LinkedListEntry *   Prev               (VOID)                        {return pPrev;};

                                           /** @brief  Inserts an entry after this one.  The pNext and pPrev pointers are
                                                       updated for this entry, the one that currently follows it, and
                                                       the passed entry.
                                               @param  pentIn Pointer to the entry that will be inserted after this entry.
                                               @return None
                                           */
    EStatus             InsertAfter        (LinkedListEntry *  pentIn);

                                           /** @brief  Inserts an entry before this one.  The pNext and pPrev pointers are
                                                       updated for this entry, the one that currently precedes it, and
                                                       the passed entry.
                                               @param  pentIn Pointer to the entry that will be inserted before this entry.
                                               @return None
                                           */
    EStatus             InsertBefore       (LinkedListEntry *  pentIn) {return (pPrev == NULL) ? EStatus::kFailure : pPrev->InsertAfter (pentIn);};


                                           /** @brief  Removes this entry from the linked list.  The pNext and pPrev pointers
                                                       are updated for the entries that follow and precede this entry,
                                                       and the pNext and pPrev pointers for this entry are set to NULL.
                                               @return None
                                           */
    VOID                Remove             (VOID);


                                           /** @brief  This routine simply sets the value of the pNext pointer.  Intended for
                                                       use in initialization.
                                               @param  pentIn The value for pNext
                                               @return None
                                           */
    VOID                SetNext            (LinkedListEntry *  pentIn)    {pNext = pentIn;};

                                           /** @brief  This routine simply sets the value of the pPrev pointer.  Intended for
                                                       use in initialization.
                                               @param  pentIn The value for pPrev
                                               @return None
                                           */
    VOID                SetPrev            (LinkedListEntry *  pentIn)    {pPrev = pentIn;};

                                           /** @brief  Tests whether or not this element is in a list.
                                               @return True if in a list, false if a sentinel or not currently in a list.
                                           */
    BOOL                IsInList           (VOID)                             {return ((Next() != NULL) && (Prev() != NULL));};

  };


//------------------------------------------------------------------------
class LinkedListIterator
  {
  private:

    PLinkedListEntry        pentCurr;

  public:

                           LinkedListIterator  ()                                   {pentCurr = NULL;};
    explicit               LinkedListIterator  (LinkedListIterator *  pitrIn)       {pentCurr = pitrIn->pentCurr;};
                           LinkedListIterator  (const LinkedListIterator &  itrIn)  {pentCurr = itrIn.pentCurr;};
    explicit               LinkedListIterator  (LinkedListEntry *  pentIn)          {pentCurr = pentIn;};
                           ~LinkedListIterator ()                                   {};

    PVOID                  GetValue            (VOID)   {return (pentCurr == NULL) ? NULL : pentCurr->Data();};
    BOOL                   IsValid             (VOID)   {return ((pentCurr != NULL) && (pentCurr->IsInList()));};
    LinkedListIterator &   operator++          (VOID)   {if (pentCurr->Next () != NULL) {pentCurr = pentCurr->Next ();}; return (*this);};
    LinkedListIterator &   operator--          (VOID)   {if (pentCurr->Prev () != NULL) {pentCurr = pentCurr->Prev ();}; return (*this);};
    BOOL                   operator==          (const LinkedListIterator &  itrIn)  {return (itrIn.pentCurr == pentCurr);};

    VOID                   Next                (VOID)   {++(*this);};
    VOID                   Prev                (VOID)   {--(*this);};
  };


//------------------------------------------------------------------------
class LinkedList
  {
  private:

    LinkedListEntry     entBeginRoot;  ///< Sentinel entry for the start of the list.  pPrev will always be NULL.
    LinkedListEntry     entEndRoot;    ///< Sentinel entry for the end of the list.  pNext will always be NULL.
    UINT32              uiSize;        ///< The number of elements in the list.

  public:

                                           /** @brief  Constructor
                                               @return None
                                           */
                        LinkedList       ();

                                           /** @brief  Copy Constructor.  A complete copy of the given list will be made.
                                               @param  listIn The list which is to be copied.
                                               @return None
                                           */
                        LinkedList       (LinkedList &  listIn);

                                           /** @brief  Destructor
                                               @return None
                                           */
    virtual             ~LinkedList      ();

                                           /** @brief  Sets the initial value of internal varibles.
                                               @return None
                                           */
    VOID                InitializeVars    ();

                                           /** @brief  Allocates and returns a new linked list entry, though the entry is not
                                                       inserted into the list itself.
                                               @param  pvDataIn The data value the new entry will be initialized with.
                                               @return None
                                           */
    LinkedListEntry *   NewEntry          (PVOID  pvDataIn);

                                           /** @brief  Inserts the passed entry into the beginning of the list.
                                               @param  pentIn The entry to insert.
                                               @return Returns errorSuccess or errorFailure.
                                           */
    EStatus             PushEntryFront    (LinkedListEntry *  pentIn);

                                           /** @brief  Inserts the passed entry into the end of the list
                                               @param  pentIn The entry to insert.
                                               @return Returns errorSuccess or errorFailure.
                                           */
    EStatus             PushEntryBack     (LinkedListEntry *  pentIn);

                                           /** @brief  Creates a new entry at the beginning of the list, initialized with the passed data.
                                               @param  pvDataIn The data to add to the list.
                                               @return Returns errorSuccess or errorFailure.
                                           */
    EStatus             PushFront         (PVOID  pvDataIn);

                                           /** @brief  Creates a new entry at the end of the list, initialized with the passed data.
                                               @param  pvDataIn The data to add to the list
                                               @return Returns errorSuccess or errorFailure.
                                           */
    EStatus             PushBack          (PVOID  pvDataIn);


                                           /** @brief  Removes the first entry from the list, and returns its data pointer.
                                               @return Returns the contents of the entry's data, or NULL if the list is empty.  (call IsEmpty () to truly check if the list is empty).
                                           */
    PVOID               PopFront          (VOID);

                                           /** @brief  Removes the last entry from the list, and returns its data pointer.
                                               @return Returns the contents of the entry's data, or NULL if the list is empty.  (call IsEmpty () to truly check if the list is empty).
                                           */
    PVOID               PopBack          (VOID);

                                           /** @brief  Removes the given entry from the list, and deletes it.
                                               @param  pentIn  The entry to delete.
                                               @return Returns errorSuccess or errorFailure.
                                           */
    EStatus             Delete            (LinkedListEntry *  pentIn);

                                           /** @brief  Deletes all entries out of the list.  Note that the items pointed to by the entries are not affected.
                                               @return Returns errorSuccess or errorFailure.
                                           */
    EStatus             Empty             (VOID);

                                           /** @brief  Deletes all entries out of the list, and calls FreeElement () to free the items pointed to by the Entry pointers.
                                               @return Returns errorSuccess or errorFailure.
                                           */
    EStatus             EmptyAndFree      (VOID);

                                           /** @brief  This virtual function should be overridden by children to delete the specific type of item that the Entry is pointing to.  Default implementation does nothing.
                                               @return Returns errorSuccess or errorFailure.
                                           */
    virtual EStatus     FreeElement       (PLinkedListEntry  pEntryIn);

                                           /** @brief  Query if the list has any entries.
                                               @return True if the list is empty, false if it isn't.
                                           */
    BOOL                IsEmpty           (VOID);

                                           /** @brief  Query the number of entries in the list.
                                               @return The number of entries in the list.
                                           */
    UINT32              Size              (VOID);

                                           /** @brief  Get an iterator to the first element in the list.
                                               @return An iterator to the first list element.
                                           */
    LinkedListIterator  GetFirst          (VOID);

                                           /** @brief  Get an iterator to the last element in the list.
                                               @return An iterator to the last list element.
                                           */
    LinkedListIterator  GetLast           (VOID);

                                           /** @brief  Search the list for an entry that matches the given data pointer.
                                               @param  pvDataIn The data pointer which will be compared to the list entries.
                                               @param  itrOut The iterator to set to the found item, if successful.
                                               @return TRUE if found, FALSE if not.
                                           */
    BOOL                Find              (PVOID                  pvDataIn,
                                            LinkedListIterator &  itrOut);

                                           /** @brief  Get the sentinel instance for the iterator update list.
                                               @return A pointer to the update list sentinel.
                                           */



  };

#endif // LINKEDLIST_HPP






