/* -----------------------------------------------------------------
                        Templated Array

     This module implements a simple templated array class.

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


#ifndef TARRAY_HPP
#define TARRAY_HPP

#include "Sys/Types.hpp"

//-----------------------------------------------------------------------------
template <class T>
class TArray
  {
  protected:
    T *           pArray;      ///< Pointer to the actual data
    INT           iLength;     ///< Number of elements in the array.
    INT           iAllocSize;  ///< Number of elements space has been allocated for.
    INT           iAllocInc;   ///< Number of new elements added when reallocation is needed.
    BOOL          bAutoGrow;   ///< If true, the array will automatically resize on out-of-bounds access

  public:


    // these need to be overridden in the child class
                                     /** @brief  Constructor
                                         @return None
                                     */
                 TArray            ()                         {Init ();};
                                     /** @brief  Copy constructor
                                         @param  arrayIn The source object to copy from.
                                         @return None
                                     */
                 TArray            (const TArray<T> &  arrayIn)  {Init (); *this = arrayIn;};
                                     /** @brief  Constructor
                                         @param  iInitialSize The number of elements to allocate upon initialization.
                                         @return None
                                     */
   explicit      TArray            (INT   iInitialSize)       {Init (); SetLength (iInitialSize);};
                                     /** @brief  Destructor
                                         @return None
                                     */
                 ~TArray           ()                         {DeleteArray (&pArray);};

   VOID          SetAutoGrow       (BOOL  bStatusIn)          {bAutoGrow = bStatusIn;};

                                     /** @brief  Return a pointer to the raw data array.  Used to pass array on to other APIs.  Be very careful when using this function.
                                         @return Pointer to the raw buffer of memory that stores the array data.
                                     */
    T *          GetRawBuffer      (VOID)                     {return pArray;};

                                     /** @brief  Test an index against the array boundaries.
                                         @return True if the given index is within the array bounds, and False if not.
                                     */
    BOOL         BoundsCheck       (INT  iIndexIn) const      {return ((iIndexIn < iLength) && (iIndexIn >= 0));};


                                     /** @brief  Allocate an array of data elements.  This routine needs to be specific to the data type.
                                         @param  iSizeIn The number of data elements to allocate
                                         @return Pointer to the allocated data
                                     */
    T *          AllocArray        (INT      iSizeIn)         {return new T [iSizeIn];};

                                     /** @brief  Delete the array of data elements.  This routine needs to be specific to the data type.
                                         @param  pvArrayIn Pointer to the array to deallocate.
                                         @return None
                                     */
    VOID         DeleteArray       (T * *  ppArrayIn)         {if (*ppArrayIn != NULL) {delete [] ((T *) *ppArrayIn);  *ppArrayIn = NULL;};};

                                     /** @brief  Copies a number of data elements from a source buffer to the target elements.  This routine needs to be specific to the data type.
                                         @param  pvSourceData Pointer to a memory buffer containing an array of the data-type, sequential in memory.
                                         @param  iSourceOffsetIn The index into pvSourceData where the copy operation will start reading.  Given in units of number of data elements to skip.
                                         @param  iStartOffset The index into the array of data elements within this class where the writing operation will begin.
                                         @param  iNumToCopy The number of data elements to copy from the source data.
                                         @return None
                                     */
    VOID         CopyValues        (T *      pSourceDataIn,
                                    INT      iSourceOffsetIn,
                                    INT      iStartOffsetIn,
                                    INT      iNumToCopyIn)
                                      {
                                      for (INT  iIndex = 0; iIndex < iNumToCopyIn; ++iIndex)
                                        {
                                        pArray [iIndex + iStartOffsetIn] = pSourceDataIn [iIndex + iSourceOffsetIn];
                                        };
                                      };


                                     /** @brief  Copies a number of data elements from a source buffer to the target elements, starting at the last data element and working towards the first.  Used for overlapping copy operations.  This routine needs to be specific to the data type.
                                         @param  pvSourceData Pointer to a memory buffer containing an array of the data-type, sequential in memory.
                                         @param  iSourceOffsetIn The index into pvSourceData where the copy operation will start reading.  Given in units of number of data elements to skip.
                                         @param  iStartOffset The index into the array of data elements within this class where the writing operation will begin.
                                         @param  iNumToCopy The number of data elements to copy from the source data.
                                         @return None
                                     */
    VOID         CopyValuesRev     (T *      pSourceDataIn,
                                    INT      iSourceOffsetIn,
                                    INT      iStartOffsetIn,
                                    INT      iNumToCopyIn)
                                      {
                                      for (INT  iIndex = iNumToCopyIn - 1; iIndex >= 0; --iIndex)
                                        {
                                        pArray [iStartOffsetIn + iIndex] = pSourceDataIn [iIndex + iSourceOffsetIn];
                                        };
                                      };


    VOID         InitValues        (INT        iStartOffset,
                                    INT        iNumToInit,
                                    const T &  tValue)
                                      {
                                      INT iEndOffset = iStartOffset + iNumToInit;

                                      for (INT  iIndex = iStartOffset; iIndex < iEndOffset; ++iIndex)
                                        {
                                        pArray [iIndex] = tValue;
                                        };
                                      };


    VOID         SwapIndexes       (INT  iIndexOne,
                                    INT  iIndexTwo)
                                      {
                                      T            tTemp = pArray [iIndexOne];
                                      pArray [iIndexOne] = pArray [iIndexTwo];
                                      pArray [iIndexTwo] = tTemp;
                                      }

    EStatus      Copy              (const TArray<T> &  arraySource)
                                      {
                                      if (SetLength (arraySource.iLength) == EStatus::kFailure) {return EStatus::kFailure;};
                                      CopyValues (arraySource.pArray, 0, 0, arraySource.Length ());
                                      bAutoGrow = arraySource.bAutoGrow;
                                      return (EStatus::kSuccess);
                                      };


    EStatus      SetAt             (INT     iIndex,
                                    T       tElement)           {if (iIndex >= iLength) return (EStatus::kFailure); pArray [iIndex] = tElement;  return (EStatus::kSuccess);};

    EStatus      Append            (T       tElement)
                                      {
                                      INT  iOldLength = iLength;
                                      if (SetLength (iOldLength + 1) == EStatus::kFailure) {return EStatus::kFailure;};
                                      pArray [iOldLength] = tElement;
                                      return (EStatus::kSuccess);
                                      };

    T &          AppendEmpty       ()
                                      {
                                      INT  iOldLength = iLength;
                                      SetLength (iOldLength + 1);
                                      return pArray [iOldLength];
                                      };


    EStatus      GetArray          (T       aOut []) const
                                      {
                                      if (iLength == 0) return (EStatus::kFailure);
                                      for (INT  iIndex = 0; iIndex < iLength; ++iIndex) {aOut [iIndex] = pArray [iIndex];};
                                      return (EStatus::kSuccess);
                                      };

                                   // NOTE:  This needs to be non-const so we can return a reference.
                                   //         Const access should be through a non-overloaded operator.
    T &          operator[]        (INT  iIndex)          {if (! BoundsCheck (iIndex)) {if (bAutoGrow) {SetMinLength (iIndex + 1);} else {return (pArray[0]);};}  return (pArray [iIndex]);};

                                   // NOTE:  This function cannot auto-grow
    const T &    GetConstRef       (INT  iIndex) const    {static T  tUndefined; return (BoundsCheck (iIndex)) ? pArray [iIndex] : tUndefined;};

    // NOTE:  We can't have a copy constructor for TArray, because the compiler gets confused and typecasts it wrong
    //  whenever we try to copy the values from old arrays to new when growing the buffer.  Stupid, I know.
    //TArray<T> &  operator=         (const TArray<T> &  arrayIn)  {    DBG_INFO ("Call Copy() from operator="); Copy (arrayIn);  return (*this);};

    VOID         Init              (VOID)
                                      {
                                      pArray     = NULL;
                                      iLength    = 0;
                                      iAllocSize = 0;
                                      iAllocInc  = 10;
                                      bAutoGrow  = FALSE;
                                      };

    EStatus      SetLength         (INT  iLengthIn)
                                      {
                                      //DBG_INFO ("SetLength alloc inc %d", iAllocInc);

                                      INT    iNewAllocSize = iAllocSize;
                                      while (iLengthIn > iNewAllocSize) {iNewAllocSize += iAllocInc;};
                                      if (iNewAllocSize > iAllocSize)
                                        {
                                        //DBG_INFO ("SetLength needs to alloc new memory old %d new %d", iAllocSize, iNewAllocSize);

                                        T *  pNewArray = AllocArray (iNewAllocSize);
                                        if (pNewArray == NULL)  return (EStatus::kFailure);

                                        INT  iOldAllocSize = iAllocSize;  iAllocSize = iNewAllocSize;
                                        T *  pOldArray     = pArray;      pArray = pNewArray;

                                        if (iOldAllocSize > 0)
                                          {
                                          //DBG_INFO ("SetLength Copying %d old values to new array", iOldAllocSize);

                                          for (INT  iIndex = 0; iIndex < iOldAllocSize; ++iIndex)
                                            {
                                            pArray [iIndex] = pOldArray [iIndex];
                                            };
                                          DeleteArray (&pOldArray);
                                          };
                                        };
                                      iLength = iLengthIn;
                                      return (EStatus::kSuccess);
                                      };

                                   /// Make sure the length of the array is at least iLengthIn
    EStatus      SetMinLength      (INT  iLengthIn)           {if (iLengthIn > Length ()) {return (SetLength (iLengthIn));}; return (EStatus::kSuccess);};

    INT          Length            (VOID) const               {return iLength;};

    INT          AllocSize         (VOID) const               {return iAllocSize;};

                                   /// Set length to 0.  Keep allocated array.
    VOID         Clear             (VOID)                     {iLength = 0;};

                                   /// Delete internal array.
    VOID         Reset             (VOID)                     {DeleteArray (&pArray); Init ();};

    EStatus      Remove            (INT  iStartIndex,
                                    INT  iNumToRemove = 1)
                                      {
                                      if ((iLength == 0) || ((iStartIndex + iNumToRemove) > iLength)) return (EStatus::kFailure);
                                      if ((iStartIndex + iNumToRemove) != iLength)
                                        {
                                        CopyValues (pArray, iStartIndex + iNumToRemove, iStartIndex, iLength - iStartIndex - iNumToRemove);
                                        };
                                      iLength -= iNumToRemove;
                                      return (EStatus::kSuccess);
                                      };

                                   /// This routine inserts new entries before the iStartIndex.
    EStatus      Insert            (INT  iStartIndex,
                                    INT  iNumToInsert = 1)
                                      {
                                      INT  iOldLength = iLength;
                                      if ((iStartIndex > iLength) ||
                                          (SetLength (iOldLength + iNumToInsert) == EStatus::kFailure) ||
                                          (Length () < (iOldLength + iNumToInsert)))  {return EStatus::kFailure;};
                                      if ((iStartIndex != iLength) && (iOldLength - iStartIndex > 0))
                                        {
                                        CopyValuesRev (pArray, iStartIndex, iStartIndex + iNumToInsert, iOldLength - iStartIndex);
                                        };
                                      return (EStatus::kSuccess);
                                      };

    VOID         SetSizeIncrement  (INT  iNewIncrement)       {iAllocInc = iNewIncrement; if (iAllocInc <= 0)  iAllocInc = 1;};

    INT          SizeIncrement     (VOID) const               {return (iAllocInc);};

  };

#endif // TARRAY_HPP

