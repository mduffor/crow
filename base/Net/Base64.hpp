/* -----------------------------------------------------------------
                           Base64 Codec

    This module implements encoding and decoding in MIME style base64

   ----------------------------------------------------------------- */

// Authors:  Michael T. Duffy  (mduffor@gmail.com)

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

#ifndef BASE64_HPP
#define BASE64_HPP

#include "Sys/Types.hpp"
#include "Util/RStr.hpp"

/**

  This class implements MIME type Base 64 encoding and decoding of strings / buffers.


  */


/// Base64 codec
//------------------------------------------------------------------------
class Base64
  {
  public:

           Base64  ()  {};
           ~Base64 ()  {};

    static bool   IsBase64Char (unsigned char cIn)  {return ((cIn >= 'A' && cIn <= 'Z') ||
                                                             (cIn >= 'a' && cIn <= 'z') ||
                                                             (cIn >= '0' && cIn <= '9') ||
                                                              cIn == '+' || cIn == '/');};

    static INT    GetIndex     (unsigned char cIn)  {if (cIn >= 'A' && cIn <= 'Z') return (cIn - 'A');
                                                     if (cIn >= 'a' && cIn <= 'z') return (cIn - 'a' + 26);
                                                     if (cIn >= '0' && cIn <= '9') return (cIn - '0' + 52);
                                                     if (cIn == '+') return (62);
                                                     if (cIn == '/') return (63);
                                                     return (-1);};

    static VOID   Encode       (const char *  pc8ToEncode,
                                INT           iLength,
                                RStr &        strOut);

    static VOID   Decode       (const char *  pc8ToDecode,
                                INT           iLength,
                                RStr &        strOut);
  };

#endif // BASE64_HPP
