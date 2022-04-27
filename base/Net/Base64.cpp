/* -----------------------------------------------------------------
                           Base64 Codec

    This module implements encoding and decoding in MIME style base64

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

// NOTE: Code inspired by René Nyffenegger's code at http://www.adp-gmbh.ch/cpp/common/base64.html

#include "Sys/Types.hpp"
#include "Debug.hpp"
#include "Util/RStr.hpp"

ASSERTFILE (__FILE__);

#include "Net/Base64.hpp"

//------------------------------------------------------------------------------
VOID Base64::Encode (const char *  pc8ToEncode,
                     INT           iLength,
                     RStr &        strOut)
  {
  static const char szBase64Chars [] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  int iCharsRead = 0;
  unsigned char au8ThreeIn[3];

  // Encode 3 input chars at a time into 4-character encoded output
  while (iLength--)
    {
    au8ThreeIn[iCharsRead++] = *(pc8ToEncode++);
    if (iCharsRead == 3)
      {
      strOut += szBase64Chars[(au8ThreeIn[0] & 0xfc) >> 2];
      strOut += szBase64Chars[((au8ThreeIn[0] & 0x03) << 4) + ((au8ThreeIn[1] & 0xf0) >> 4)];
      strOut += szBase64Chars[((au8ThreeIn[1] & 0x0f) << 2) + ((au8ThreeIn[2] & 0xc0) >> 6)];
      strOut += szBase64Chars[au8ThreeIn[2] & 0x3f];
      iCharsRead = 0;
      };
    };

  // encode remaining in array
  if (iCharsRead)
    {
    for (int iIndex = iCharsRead; iIndex < 3; ++iIndex)
      au8ThreeIn[iIndex] = '\0';

    int iCharsToWrite = iCharsRead;
    strOut += szBase64Chars[(au8ThreeIn[0] & 0xfc) >> 2];
    if (iCharsToWrite--)
      {
      strOut += szBase64Chars[((au8ThreeIn[0] & 0x03) << 4) + ((au8ThreeIn[1] & 0xf0) >> 4)];
      if (iCharsToWrite--)
        {
        strOut += szBase64Chars[((au8ThreeIn[1] & 0x0f) << 2) + ((au8ThreeIn[2] & 0xc0) >> 6)];
        if (iCharsToWrite)
          {
          strOut += szBase64Chars[au8ThreeIn[2] & 0x3f];
          };
        };
      };

    while((iCharsRead++ < 3))
      {
      strOut += '=';
      };
    };
  }

//------------------------------------------------------------------------------
VOID Base64::Decode (const char *  pc8ToDecode,
                     INT           iLength,
                     RStr &        strOut)
  {
  int iCharsRead = 0;
  int iIndex = 0;
  unsigned char au8FourIn[4];

  // Decode 4 encoded chars at a time, to output three unencoded chars
  while (iLength-- && (pc8ToDecode [iIndex] != '=') && Base64::IsBase64Char (pc8ToDecode[iIndex]))
    {
    au8FourIn[iCharsRead++] = (unsigned char) Base64::GetIndex (pc8ToDecode[iIndex]);
    ++iIndex;
    if (iCharsRead == 4)
      {
      strOut += (au8FourIn[0] << 2) + ((au8FourIn[1] & 0x30) >> 4);
      strOut += ((au8FourIn[1] & 0xf) << 4) + ((au8FourIn[2] & 0x3c) >> 2);
      strOut += ((au8FourIn[2] & 0x3) << 6) + au8FourIn[3];
      iCharsRead = 0;
      };
    };

  if (iCharsRead)
    {
    for (iIndex = iCharsRead; iIndex < 4; ++iIndex)
      au8FourIn[iIndex] = 0;

    if (iCharsRead--)
      {
      strOut += (au8FourIn[0] << 2) + ((au8FourIn[1] & 0x30) >> 4);
      if (iCharsRead--)
        {
        strOut += ((au8FourIn[1] & 0xf) << 4) + ((au8FourIn[2] & 0x3c) >> 2);
        if (iCharsRead)
          {
          strOut += ((au8FourIn[2] & 0x3) << 6) + au8FourIn[3];
          };
        };
      };
    };
  };


#ifdef UNITTEST
//------------------------------------------------------------------------------
VOID Base64:UnitTest ()
  {
  BASE64("") = ""
  BASE64("f") = "Zg=="
  BASE64("fo") = "Zm8="
  BASE64("foo") = "Zm9v"
  BASE64("foob") = "Zm9vYg=="
  BASE64("fooba") = "Zm9vYmE="
  BASE64("foobar") = "Zm9vYmFy"
  }
#endif //UNITTEST
