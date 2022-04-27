/* -----------------------------------------------------------------
                              XML Tools

     This module contains tools and utilities for in-place parsing 
   of files in XML format.

   ----------------------------------------------------------------- */

// Modified BSD License:
//
// Copyright (c) 2017, Michael T. Duffy II.  All rights reserved.
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

#ifndef XMLTOOLS_HPP
#define XMLTOOLS_HPP


#include "Sys/Types.hpp"
#include "Util/RStr.hpp"
#include "Util/RStrParser.hpp"

//#include "Containers/RStrArray.hpp"
//#include "Containers/BaseArray.hpp"
//#include "Containers/IntArray.hpp"
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



//------------------------------------------------------------------------
class XmlTools
  {
  public:

    RStr          strTagName;
    KVPArray      arrayAttributes;
    INT32         iTagEnd;
    INT32         iDataStart;
    INT32         iDataEnd;  
    RStrParser *  pparserCachedBuffer;   
                                    
  public:
  
                XmlTools       (VOID);
  
    VOID        Reset          (VOID);
  
    EStatus     FindTag        (RStrParser &  parserBufferIn);

    RStr &      GetTag         (VOID)               {return strTagName;};
    
    INT         GetNumAttrs    (VOID)               {return arrayAttributes.Length();};
    
    RStr &      GetAttr        (const char *  szKeyIn)   {return arrayAttributes[szKeyIn];};

    BOOL        HasAttr        (const char *  szKeyIn)  {return (arrayAttributes.FindKey (szKeyIn) != -1);};
    
    INT         GetAttrInt     (const char *  szKeyIn,
                                INT           iDefaultIn = 0);
    
    FLOAT       GetAttrFloat   (const char *  strKeyIn,
                                FLOAT         fDefaultIn = 0.0f);
    
    VOID        GetAttrAtIndex (INT     iIndexIn,
                                RStr &  strNameOut,
                                RStr &  strValueOut);
    
    BOOL        HasBody        (VOID)   {return (iDataStart <= iDataEnd);};
    
    BOOL        GetBody        (RStrParser &  parserOut);
  
    EStatus     ReadAttributes (RStrParser &  parserBufferIn,
                                INT32         iAttrStart,
                                INT32         iAttrEnd,
                                KVPArray &    arrayAttributesOut);
  

  };


#endif // XMLTOOLS_HPP











