/* -----------------------------------------------------------------
                                 HTTP

     This module implements cross-platform routines for dealing with
     file and directory paths

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

#ifndef HTTP_HPP
#define HTTP_HPP

#include "Sys/Types.hpp"
#include "Util/RStr.hpp"
#include "Net/Socket.hpp"
#include "Net/URLBuilder.hpp"
#include "Containers/KVPArray.hpp"

//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------


//------------------------------------------------------------------------
// Forward Declarations
//------------------------------------------------------------------------



//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------

//-----------------------------------------------------------------------------
class HTTP
  {
  private:

    Socket        socket;
    URLBuilder    url;
    RStrParser    parserSend;
    RStrParser    parserReceive;
    static INT    iNumCalls;

  public:

    static const char *     kMimeTypeText;
    static const char *     kMimeTypeWWWForm;

                    HTTP                    ();

                    ~HTTP                   ();

    EStatus         Connect                 (const URLBuilder &  urlIn);

    EStatus         Disconnect              (VOID);

    EStatus         Post                    (const char *  szMimeType,
                                             RStr *        strContent,
                                             const char *  szAdditionalHeader);

    EStatus         Get                     (RStr &        strResultOut);

    VOID            ParseHeader             (RStrParser &    parserIn,
                                             BOOL &          bSuccessOut,
                                             const char * *  pszCodeNameOut,
                                             KVPArray &      kvpAttributesOut);

    BOOL            CheckErrorCode          (INT             iCodeIn,
                                             const char * *  pszCodeNameOut);

    EStatus         XMLHttpRequest          (KVPArray &    arrayIn,
                                             const char *  szAdditionalHeaderIn,
                                             RStr &        strResultOut);

    VOID            BuildMessage            (RStrParser &  parserIn,
                                             const char *  szCommandIn,
                                             const char *  szMimeTypeIn = NULL,
                                             RStr *        strContentIn = NULL,
                                             const char *  szAdditionalHeaderIn = NULL);

    VOID            KVPToURLEncoded         (KVPArray &    arrayIn,
                                             RStrParser &  parserOut);

    VOID            AppendURLEncodedString  (const RStr &  strIn,
                                             RStrParser &  parserTargetOut);

    static RStr &   HeaderBasicAuth         (const char *  szUsername,
                                             const char *  szPassword);

    static const char * NumCallsStr         (VOID);

  };

#endif // HTTP_HPP
