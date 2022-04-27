/* -----------------------------------------------------------------
                             URL Builder

     This module implements a class for manipulating the different
     elements of a URI.

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

#ifndef URLBUILDER_HPP
#define URLBUILDER_HPP

#include "Sys/Types.hpp"
#include "Util/RStr.hpp"
#include "Util/RStrParser.hpp"
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
class URLBuilder
  {
  private:

    RStr        strPrefix;
    RStr        strServer;
    INT         iPort;
    RStr        strPath;
    KVPArray    kvpParams;

    RStrParser   parserFull;

  public:

                  URLBuilder      ();

    explicit      URLBuilder      (const char *  szURLIn);

                  ~URLBuilder     ();

    VOID          Init           (VOID);

    URLBuilder &  operator =     (const URLBuilder &  urlIn);

    const char *  GetFullURL     (VOID);

    const char *  GetPrefix      (VOID);

    const char *  GetServer      (VOID);

    INT           GetPort        (VOID);

    const char *  GetPath        (VOID);

    VOID          SetFullURL     (const char *  szIn);

    VOID          SetPrefix      (const char *  szIn);

    VOID          SetServer      (const char *  szIn);

    VOID          SetPort        (int           iIn);

    VOID          SetPath        (const char *  szIn);

    VOID          ClearParams    (VOID);

    VOID          SetParam       (const char *  szKeyIn,
                                  const char *  szValueIn);

    VOID          BuildFullURL   (VOID);



  };

#endif //define URLBUILDER_HPP
