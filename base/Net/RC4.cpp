/* -----------------------------------------------------------------
                           RC4 Codec

    This module implements encoding and decoding in RC4

   ----------------------------------------------------------------- */

// Authors:  Michael T. Duffy  (mduffor@gmail.com)

// Modified BSD License:
//
// Copyright (c) 2016, Michael T. Duffy II.  All rights reserved.
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

// NOTE: Code based on information at https://en.wikipedia.org/wiki/RC4

#include "Sys/Types.hpp"
#include "Debug.hpp"
#include "Util/RStr.hpp"

ASSERTFILE (__FILE__);

#include "Net/RC4.hpp"

//------------------------------------------------------------------------------
RC4::RC4  ()
  {
  memset (acKeyedBuffer, 0, sizeof (acKeyedBuffer));
  };

//------------------------------------------------------------------------------
RC4::~RC4 ()
  {
  };

//------------------------------------------------------------------------------
VOID  RC4::InitWithKey  (const char *  szKeyIn,
                         INT           iKeyLength)
  {
  INT  iI;
  for (iI = 0; iI < iKeyedBufferSize; ++iI)
    {
    acKeyedBuffer [iI] = iI;
    };

  INT    iJ = 0;
  for (iI = 0; iI < iKeyedBufferSize; ++iI)
    {
    iJ = (iJ + acKeyedBuffer[iI] + szKeyIn [iI % iKeyLength]) % iKeyedBufferSize;

    UINT8  uTemp;
    uTemp              = acKeyedBuffer [iI];
    acKeyedBuffer [iI] = acKeyedBuffer [iJ];
    acKeyedBuffer [iJ] = uTemp;
    };
  };



//------------------------------------------------------------------------------
VOID RC4::Encode (const char *  pc8ToEncode,
                  INT           iLength,
                  RStr &        strOut)
  {
  INT    iI = 0;
  INT    iJ = 0;

  //DBG_INFO ("RC4 Encode (%d bytes)", iLength);
  strOut.GrowAbsolute (iLength);

  for (INT  iIndex = 0; iIndex < iLength; ++iIndex)
    {
    iI = (iI + 1) % iKeyedBufferSize;
    iJ = (iJ + acKeyedBuffer[iI]) % iKeyedBufferSize;

    UINT8  uTemp;
    uTemp              = acKeyedBuffer [iI];
    acKeyedBuffer [iI] = acKeyedBuffer [iJ];
    acKeyedBuffer [iJ] = uTemp;

    strOut.SetAt (iIndex, pc8ToEncode[iIndex] ^ acKeyedBuffer[(acKeyedBuffer[iI] + acKeyedBuffer[iJ]) % iKeyedBufferSize]);

    //if (iIndex < 100)
    //  {
    //  DBG_INFO ("%c ^ %c = %c",
    //            pc8ToEncode[iIndex],
    //            acKeyedBuffer[(acKeyedBuffer[iI] + acKeyedBuffer[iJ]) % iKeyedBufferSize],
    //            strOut.GetAt (iIndex));
    //  };


    };
  };


//------------------------------------------------------------------------------
VOID RC4::Decode (const char *  pc8ToDecode,
                  INT           iLength,
                  RStr &        strOut)
  {
  // Encoding and decoding is symmetric

  Encode (pc8ToDecode, iLength, strOut);
  };

