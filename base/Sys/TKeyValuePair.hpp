/* -----------------------------------------------------------------
                   Templated Key Value Pair (Dictionary)

     This module implements a simple templated key-value container.

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


#ifndef TKEYVALUEPAIR_HPP
#define TKEYVALUEPAIR_HPP

#include "Sys/Types.hpp"

// NOTE:  This should be sped up by implementing the storage as a balanced tree.

// REFACTOR : This module could be sped up by using a pool to control the TKVPEntry
//   allocations.  Since the use case for TKVP is almost always to store a list
//   of pointers, this should be relatively straightforward with few problems in
//   clearing/initializing memory.  Perhaps pool storage could be an optional flag
//   that is set.

//------------------------------------------------------------------------
template <class T>
class TKVPEntry
  {
  private:

    RStr            strKey;
    T               tValue;
    TKVPEntry<T> *  pNext;
    TKVPEntry<T> *  pPrev;

  public:

                                  /** @brief  Constructor
                                      @return None
                                  */
                     TKVPEntry    ()                           {pNext = pPrev = NULL;};


                                  /** @brief  Constructor
                                      @param  szKeyIn A unique key to identify this value.
                                      @param  tValueIn This entry's data.
                                      @return None
                                  */
                     TKVPEntry    (const char *  szKeyIn,
                                   T             tValueIn)     {strKey.SetHash (szKeyIn); tValue = tValueIn; pNext = pPrev = NULL;};

                                  /** @brief  Destructor
                                      @return None
                                  */
                     ~TKVPEntry  ()                            {Remove ();};

                                  /** @brief  Sets this entry's data (usually a pointer)
                                      @param  tValueIn The entry's data
                                      @return None
                                  */
    VOID             SetValue     (T       tValueIn)           {tValue = tValueIn;};

    BOOL             KeyEquals    (RStr &  strCompareIn)       {return strKey.Equals (strCompareIn);};

    BOOL             KeyEquals    (const char *  szCompareIn)  {return strKey.Equals (szCompareIn);};

    const RStr &     Key          (VOID) const                 {return strKey;};

    T                Value        (VOID) const                 {return tValue;};

    T &              ValueRef     (VOID)                       {return tValue;};

                                  /** @brief  Returns a pointer to the next entry in the list
                                      @return The pointer to the next entry
                                  */
    TKVPEntry<T> *   Next         (VOID) const                 {return pNext;};

                                  /** @brief  Returns a pointer to the previous entry in the list
                                      @return The pointer to the previous entry
                                  */
    TKVPEntry<T> *   Prev         (VOID) const                 {return pPrev;};

                                  /** @brief  Inserts an entry after this one.  The pNext and pPrev pointers are
                                              updated for this entry, the one that currently follows it, and
                                              the passed entry.
                                      @param  pentIn Pointer to the entry that will be inserted after this entry.
                                      @return True on success, False on failure.
                                  */
    BOOL             InsertAfter  (TKVPEntry<T> *  pentIn)     {if (pNext == NULL) return (FALSE);
                                                                // check for an invalid entry
                                                                //ASSERT ((pNext != NULL) && (pentIn != NULL) && (pentIn->pPrev == NULL) && (pentIn->pNext == NULL));

                                                                pNext->pPrev  = pentIn;
                                                                pentIn->pNext = pNext;
                                                                pentIn->pPrev = this;
                                                                pNext         = pentIn;
                                                                return (TRUE);};

                                  /** @brief  Inserts an entry before this one.  The pNext and pPrev pointers are
                                              updated for this entry, the one that currently precedes it, and
                                              the passed entry.
                                      @param  pentIn Pointer to the entry that will be inserted before this entry.
                                      @return True on success, False on failure.
                                  */
    BOOL             InsertBefore (TKVPEntry<T> *  pentIn)     {return (pPrev == NULL) ? FALSE : pPrev->InsertAfter (pentIn);};

                                  /** @brief  Removes this entry from the linked list.  The pNext and pPrev pointers
                                              are updated for the entries that follow and precede this entry,
                                              and the pNext and pPrev pointers for this entry are set to NULL.
                                      @return None
                                  */
    VOID             Remove       (VOID)                       {if (pPrev != NULL)   pPrev->pNext = pNext;
                                                                if (pNext != NULL)   pNext->pPrev = pPrev;
                                                                pNext = pPrev = NULL;};

                                  /** @brief  This routine simply sets the value of the pNext pointer.  Intended for
                                              use in initialization.
                                      @param  pentIn The value for pNext
                                      @return None
                                  */
    VOID             SetNext      (TKVPEntry<T> *  pentIn)     {pNext = pentIn;};

                                  /** @brief  This routine simply sets the value of the pPrev pointer.  Intended for
                                              use in initialization.
                                      @param  pentIn The value for pPrev
                                      @return None
                                  */
    VOID             SetPrev      (TKVPEntry<T> *  pentIn)     {pPrev = pentIn;};

                                  /** @brief  Tests whether or not this element is in a list.
                                      @return True if in a list, false if a sentinel or not currently in a list.
                                  */
    BOOL             IsInList     (VOID)                       {return ((Next() != NULL) && (Prev() != NULL));};


  };


//------------------------------------------------------------------------
template <class T>
class TKVPItr
  {
  private:

    TKVPEntry<T> *     pentCurr;
    T                  tInvalid;

  public:

                   TKVPItr    ()                           {pentCurr = NULL;};
    explicit       TKVPItr    (TKVPItr<T> *  pitrIn)       {pentCurr = pitrIn->pentCurr;};
                   TKVPItr    (const TKVPItr<T> &  itrIn)  {pentCurr = itrIn.pentCurr;};
    explicit       TKVPItr    (TKVPEntry<T> *  pentIn)     {pentCurr = pentIn;};
                   ~TKVPItr   ()                           {};

    BOOL           KeyEquals   (RStr &        strCompareIn) {return (pentCurr == NULL) ? FALSE : pentCurr->KeyEquals(strCompareIn);};
    BOOL           KeyEquals   (const char *  szCompareIn)  {return (pentCurr == NULL) ? FALSE : pentCurr->KeyEquals(szCompareIn);};
    const RStr &   Key         (VOID)    {return (pentCurr == NULL) ? RStr::kEmpty : pentCurr->Key();};
    T              Value       (VOID)    {return (pentCurr == NULL) ? tInvalid : pentCurr->Value();};
    T &            ValueRef    (VOID)    {return (pentCurr == NULL) ? tInvalid : pentCurr->ValueRef();};
    VOID           SetValue    (T  tValueIn)  {if (pentCurr != NULL) {pentCurr->SetValue (tValueIn);};};

    BOOL           IsValid     (VOID)    {return ((pentCurr != NULL) && (pentCurr->IsInList()));};
    TKVPItr<T> &   operator++  (VOID)    {if (pentCurr->Next () != NULL) {pentCurr = pentCurr->Next ();}; return (*this);};
    TKVPItr<T> &   operator--  (VOID)    {if (pentCurr->Prev () != NULL) {pentCurr = pentCurr->Prev ();}; return (*this);};
    BOOL           operator==  (const TKVPItr<T> &  itrIn)  {return (itrIn.pentCurr == pentCurr);};
    T              operator*   ()                           {return (pentCurr == NULL) ? NULL : pentCurr->Value();};

    VOID           Next        (VOID)    {++(*this);};
    VOID           Prev        (VOID)    {--(*this);};

    // For Internal Use Only
    TKVPEntry<T> *  GetEntryPtr (VOID)  {return pentCurr;};

  };


//------------------------------------------------------------------------
template <class T>
class TKVP
  {
  private:

    TKVPEntry<T>        entBeginRoot; ///< Sentinel entry for the start of the list.  pPrev will always be NULL.
    TKVPEntry<T>        entEndRoot;   ///< Sentinel entry for the end of the list.  pNext will always be NULL.
    UINT32              uiSize;       ///< The number of elements in the list.
    T                   tInvalid;

  public:

                                        /** @brief  Constructor
                                            @return None
                                        */
                TKVP            ()                        {InitializeVars ();};

                                        /** @brief  Copy Constructor.  A complete copy of the given list will be made.
                                            @param  listIn The list which is to be copied.
                                            @return None
                                        */
                TKVP            (const TKVP<T> &  listIn) {
                                                          InitializeVars ();
                                                          for (TKVPItr<T> itrCurr = listIn.First ();
                                                               itrCurr.IsValid ();
                                                               ++itrCurr)
                                                            {
                                                            Add (itrCurr.Key (), itrCurr.Value ());
                                                            };
                                                          };

                                        /** @brief  Destructor
                                            @return None
                                        */
    virtual     ~TKVP           ()                        {Empty ();};

                                        /** @brief  Sets the initial value of internal varibles.
                                            @return None
                                        */
    VOID        InitializeVars  ()                        {
                                                          // Initialize the root chain.  The sentinel entries will be the only ones in the chain
                                                          //   with a NULL in either the Next or Prev slot.
                                                          entBeginRoot.SetNext (&entEndRoot);
                                                          entBeginRoot.SetPrev (NULL);
                                                          entEndRoot.  SetNext (NULL);
                                                          entEndRoot.  SetPrev (&entBeginRoot);
                                                          uiSize = 0;
                                                          };

                                        /** @brief  Creates a new entry at the end of the list, initialized with the passed data.
                                            @param  szKeyIn A unique key to identify this value.
                                            @param  tValueIn The data to add to the list
                                            @return Returns errorSuccess or errorFailure.
                                        */
    VOID        Add             (const char *  szKeyIn,
                                 T             tValueIn)  {
                                                          for (TKVPItr<T> itrCurr = First (); itrCurr.IsValid (); ++itrCurr)
                                                            {
                                                            if (itrCurr.KeyEquals (szKeyIn)) {itrCurr.SetValue(tValueIn);  return;};
                                                            };
                                                          if (entEndRoot.InsertBefore (new TKVPEntry<T> (szKeyIn, tValueIn))) {++uiSize;};
                                                          };

                                        /** @brief  Removes the given entry from the list, and deletes it.
                                            @param  pentIn  The entry to delete.
                                        */
    VOID        DeleteEntry     (TKVPEntry<T> *  pentIn)  {//ASSERT (uiSize != 0);
                                                          pentIn->Remove ();
                                                          delete (pentIn);
                                                          if (uiSize > 0) --uiSize;
                                                          };

                                        /** @brief  Finds the entry identified by the key, and deletes it.
                                            @param  szKeyIn  The entry to delete.
                                        */
    VOID        Delete          (const char *  szKeyIn)   {
                                                          RStr  strKey;  strKey.SetHash (szKeyIn);
                                                          for (TKVPEntry<T> *  pCurr = entBeginRoot.Next();
                                                               pCurr != &entEndRoot;
                                                               pCurr = pCurr->Next ())
                                                            {
                                                            if (pCurr->KeyEquals(strKey))
                                                              {
                                                              DeleteEntry (pCurr);
                                                              return;
                                                              }
                                                            }
                                                          };

                                        /** @brief  Removes the given entry from the list, and deletes it.
                                            @param  itrIn  An iterator pointing to the entry to delete.
                                        */
    VOID        Delete          (TKVPItr<T> &  itrIn)     {
                                                          TKVPEntry<T> *  pEntry = itrIn.GetEntryPtr();
                                                          if (pEntry != NULL) {pEntry->Remove ();};
                                                          delete (pEntry);
                                                          if (uiSize > 0) --uiSize;
                                                          };

                                        /** @brief  Deletes all entries out of the list.  Note that the items pointed to by the entries are not affected.
                                            @return Returns errorSuccess or errorFailure.
                                        */
    VOID        Empty           (VOID)                    {
                                                          while (entBeginRoot.Next () != &entEndRoot)
                                                            {
                                                            DeleteEntry (entBeginRoot.Next ());
                                                            };
                                                          };


                                        /** @brief  Query if the list has any entries.
                                            @return True if the list is empty, false if it isn't.
                                        */
    BOOL        IsEmpty         (VOID) const              {return (uiSize == 0);};


                                        /** @brief  Set the value returned when an invalid value must be returned.
                                            @param  tIn The value to return when an invalid value is needed.
                                        */
    VOID        SetInvalid      (T  tIn)                  {tInvalid = tIn;};

                                        /** @brief  Query the number of entries in the list.
                                            @return The number of entries in the list.
                                        */
    UINT32      Size            (VOID) const              {return (uiSize);};

                                        /** @brief  Get an iterator to the first element in the list.
                                            @return An iterator to the first list element.
                                        */
    TKVPItr<T>  First           (VOID) const              {return (TKVPItr<T> (entBeginRoot.Next ()));};

                                        /** @brief  Get an iterator to the last element in the list.
                                            @return An iterator to the last list element.
                                        */
    TKVPItr<T>  Last            (VOID) const              {return (TKVPItr<T> (entEndRoot.Prev ()));};

                                        /** @brief  Search the list for an entry with the given key.
                                            @param  szKeyIn The key to search for.
                                            @return TRUE if found, FALSE if not.
                                        */
    T           Find            (const char *  szKeyIn)   {
                                                          for (TKVPItr<T> itrCurr = First ();
                                                               itrCurr.IsValid ();
                                                               ++itrCurr)
                                                            {
                                                            if (itrCurr.KeyEquals (szKeyIn)) {return (itrCurr.Value ());};
                                                            };
                                                          return (tInvalid);
                                                          };

    BOOL        HasKey          (const char *  szKeyIn)   {
                                                          RStr  strKey;  strKey.SetHash (szKeyIn);
                                                          for (TKVPItr<T> itrCurr = First ();
                                                               itrCurr.IsValid ();
                                                               ++itrCurr)
                                                            {
                                                            if (itrCurr.KeyEquals (strKey)) {return (TRUE);};
                                                            };
                                                          return (FALSE);
                                                          };

    T &         operator []     (const char *  szKeyIn)   {
                                                          RStr  strKey;  strKey.SetHash (szKeyIn);
                                                          for (TKVPItr<T> itrCurr = First ();
                                                               itrCurr.IsValid ();
                                                               ++itrCurr)
                                                            {
                                                            if (itrCurr.KeyEquals (strKey)) {return (itrCurr.ValueRef ());};
                                                            };
                                                          TKVPEntry<T> *  pNew = new TKVPEntry<T> (szKeyIn, tInvalid);
                                                          if (entEndRoot.InsertBefore (pNew)) {++uiSize;};
                                                          return pNew->ValueRef ();
                                                          };
  };

#endif // TKEYVALUEPAIR_HPP






