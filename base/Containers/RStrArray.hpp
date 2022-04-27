/* -----------------------------------------------------------------
                            TEdit String Array

    This module implements a string array class.

                            Michael T. Duffy
   ----------------------------------------------------------------- */

// RStrArray.hpp
// Copyright 2004, 2008, Michael T. Duffy.
// contact:  mduffor@gmail.com

// This file is part of TEdit.
//
// TEdit is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 3 as
// published by the Free Software Foundation.
//
// TEdit is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with TEdit.  If not, see <http://www.gnu.org/licenses/>.


#ifndef RSTRARRAY_HPP
#define RSTRARRAY_HPP

#include <ctype.h>
#include <stdarg.h>   // for variable argument functions (va_*)
#include "Sys/Types.hpp"
#include "Containers/BaseArray.hpp"
#include "Containers/IntArray.hpp"
#include "Util/RStr.hpp"


typedef RStr *  PRStr;

/// An array of RStr instances
//-----------------------------------------------------------------------------
class RStrArray : public BaseArray
  {
  private:

  public:

                 RStrArray      ();
                 RStrArray      (const RStrArray &  arrayIn);
   explicit      RStrArray      (INT   iInitialSize);
                 ~RStrArray     ();

  protected:
    VOID *       AllocArray     (INT      iSizeIn) override;
    VOID         DeleteArray    (PVOID *  pvArrayIn) override;
    VOID         DeleteArrayLocal (PVOID *  pvArrayIn);
    VOID         ZeroValues     (INT      iStartOffset,
                                 INT      iNumToZero) override;
    VOID         FreeValues     (INT      iStartOffset,
                                 INT      iNumToFree) override;
    VOID         FreeValuesLocal (INT      iStartOffset,
                                  INT      iNumToFree);
    VOID         MoveValues     (PVOID    pvSourceData,
                                 INT      iSourceOffsetIn,
                                 INT      iStartOffset,
                                 INT      iNumToCopy) override;
    VOID         MoveValuesRev  (PVOID    pvSourceData,
                                 INT      iSourceOffsetIn,
                                 INT      iStartOffset,
                                 INT      iNumToCopy) override;
    VOID         AverageValues  (INT      iIndexAverage,
                                 INT      iIndexValueOne,
                                 INT      iIndexValueTwo,
                                 FLOAT    fBias = 0.5f) override;
  public:
    VOID         SwapIndexes    (INT  iIndexOne,
                                 INT  iIndexTwo) override;
    INT          CompareIndexes (INT  iIndexOne,
                                 INT  iIndexTwo) override;

    VOID         InitValues     (INT      iStartOffset,
                                 INT      iNumToInit);
    EStatus      Copy           (const RStrArray &  arraySource);
    EStatus      SetAt          (INT  iIndex, RStr          strElement);
    EStatus      SetAt          (INT  iIndex, const char *  szElement);
    EStatus      Append         (RStr  strElement);
    EStatus      Append         (const char *  szElement);
    EStatus      AppendUnique   (RStr  strElement);
    EStatus      AppendUnique   (const char *  szElement);
    EStatus      GetArray       (RStr  []) const;

    RStr         GetAt          (INT  iIndex) const  {return ((*this)[iIndex]);};
    RStr         operator[]     (INT  iIndex) const;
    RStr &       operator[]     (INT  iIndex);
    RStrArray &  operator=      (const  RStrArray & arrayIn);
    BOOL         operator==     (const  RStrArray & arrayIn);
    BOOL         operator!=     (const  RStrArray & arrayIn)   {return (!(*this == arrayIn));};

                                /**
                                @brief Insert another array into this array at a given position
                                @param iIndexIn Position where the new array should be inserted
                                @param arrayToInsertIn The array that you want to insert.
                                @return None
                                */
    EStatus      InsertArray    (INT          iIndexIn,
                                 RStrArray &  arrayToInsertIn);

    INT          Find           (RStr &  strIn);

    INT          Find           (const char *  pszIn);

    VOID         SortIndexes    (IntArray &  arrayOut);

    VOID         DebugPrint     (VOID) const;

    VOID         DebugPrint     (RStr &        strOut) const;

  };




#endif // RSTRARRAY_HPP
