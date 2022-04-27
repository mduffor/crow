/* -----------------------------------------------------------------
                             RC4 Codec

    This module implements encoding and decoding in RC4

   ----------------------------------------------------------------- */

// Authors:  Michael T. Duffy  (mduffor@gmail.com)

// Modified BSD License:
//
// Copyright (c) 2018, Michael T. Duffy II.  All rights reserved.
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

#ifndef RC4_HPP
#define RC4_HPP

#include "Sys/Types.hpp"
#include "Util/RStr.hpp"

/**

  This class implements RC4 encoding and decoding of strings / buffers.

  */


/// RC4 codec
//------------------------------------------------------------------------
class RC4
  {
  private:
    const static INT    iKeyedBufferSize = 256;
    UINT8               acKeyedBuffer [iKeyedBufferSize];

  public:

           RC4  ();

           ~RC4 ();

    VOID   InitWithKey  (const char *  szKeyIn,
                         INT           iKeyLength);

    VOID   InitWithKey  (const RStr &  strKeyIn)     {InitWithKey (strKeyIn.AsChar (), strKeyIn.Length ());};

    VOID   Encode       (const char *  pc8ToEncode,
                         INT           iLength,
                         RStr &        strOut);

    VOID   Decode       (const char *  pc8ToDecode,
                         INT           iLength,
                         RStr &        strOut);
  };

#endif // RC4_HPP
