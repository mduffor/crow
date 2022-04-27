/* -----------------------------------------------------------------
                             Color

     This class handles colors and color conversion.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2004-2014, Michael T. Duffy II.  All rights reserved.
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

#include "Debug.hpp"
ASSERTFILE (__FILE__);
#include "Math/RVec.hpp"
#include "Math/RMatrix.hpp"
#include "Gfx/Color8U.hpp"


//-----------------------------------------------------------------------------
//  Color8U
//-----------------------------------------------------------------------------

// Though this file is empty for now, it will at some point contain methods
//  for converting between linear and gamma color space.


//------------------------------------------------------------------------
BOOL operator== (const Color8U & colorOne,
                 const Color8U & colorTwo)
  {
  return (colorOne.uRGBA == colorTwo.uRGBA);
  };

//------------------------------------------------------------------------
BOOL operator!= (const Color8U & colorOne,
                 const Color8U & colorTwo)
  {
  return (colorOne.uRGBA != colorTwo.uRGBA);
  };


//------------------------------------------------------------------------
BOOL operator== (const Color8U & colorOne, UINT32    uColorTwo)
  {
  return (colorOne.uRGBA == uColorTwo);
  };

//------------------------------------------------------------------------
BOOL operator== (UINT32 uColorOne, const Color8U & colorTwo)
  {
  return (uColorOne == colorTwo.uRGBA);
  };

//------------------------------------------------------------------------
INT  Color8U::ReadHex (const char *  szHexIn,
                       INT           iLengthIn,
                       INT &         iIndexInOut)
  {
  // returns hex value.  Updates index value
  INT  iValue = 0;

  if ((iLengthIn - iIndexInOut) >= 2)
    {
    INT  iDigit;

    iDigit = szHexIn [iIndexInOut++];
    iValue = (((iDigit >= '0') && (iDigit <= '9')) ? iDigit - '0':
              ((iDigit >= 'a') && (iDigit <= 'f')) ? iDigit - 'a' + 10:
              ((iDigit >= 'A') && (iDigit <= 'F')) ? iDigit - 'A' + 10 : 0) << 4;

    iDigit = szHexIn [iIndexInOut++];
    iValue |= (((iDigit >= '0') && (iDigit <= '9')) ? iDigit - '0':
               ((iDigit >= 'a') && (iDigit <= 'f')) ? iDigit - 'a' + 10:
               ((iDigit >= 'A') && (iDigit <= 'F')) ? iDigit - 'A' + 10 : 0);
    };
  return (iValue);
  };



//------------------------------------------------------------------------
INT  Color8U::Set (const char *  szHexIn)
  {
  // This function can accept hex color values in the following forms & length
  // 0xRRGGBB
  // 0xRRGGBBAA
  // #RRGGBB
  // #RRGGBBAA
  // RRGGBB
  // RRGGBBAA

  INT  iLength = strlen (szHexIn);
  INT  iIndex = 0;

  uRGBA = 0x000000ff;

  if ((iLength - iIndex) > 2)
    {
    if ((szHexIn [iIndex] == '0') &&
        ((szHexIn [iIndex+1] == 'x') || (szHexIn [iIndex+1] == 'X')))
      {
      iIndex += 2;
      };
    };
  if ((iLength - iIndex) > 1)
    {
    if (szHexIn [iIndex] == '#')
      {
      iIndex += 1;
      };
    };

  INT iR = ReadHex (szHexIn, iLength, iIndex);
  INT iG = ReadHex (szHexIn, iLength, iIndex);
  INT iB = ReadHex (szHexIn, iLength, iIndex);
  INT iA = ((iLength - iIndex) >= 2) ? ReadHex (szHexIn, iLength, iIndex) : 0xff;

  uRGBA = MakeRGBA (iR, iG, iB, iA);
  return (uRGBA);
  };


