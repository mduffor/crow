/* -----------------------------------------------------------------
                        Attr Binary Serializer

     This module implements a class using the Visitor pattern to 
   serialize attrs into a binary blob.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2015, Michael T. Duffy II.  All rights reserved.
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

#ifndef ATTRBINARYSERIALIZER_HPP
#define ATTRBINARYSERIALIZER_HPP

#include "Sys/Types.hpp"
#include "Util/RStr.hpp"
#include "Util/RStrParser.hpp"
#include "Util/Signal.h"
#include "Composite/AttrInt.hpp"
#include "Composite/AttrFloat.hpp"
#include "Composite/AttrString.hpp"
#include "Composite/AttrStringArray.hpp"
#include "Composite/AttrIntArray.hpp"
#include "Composite/AttrFloatArray.hpp"

/**

  */


//-----------------------------------------------------------------------------
class AttrBinarySerializer
  {
  private:
  
  
  public:
             AttrBinarySerializer  ();
              
             ~AttrBinarySerializer ();

                                   /*
                                    * 
                                    */
    EStatus  LoadList              (RStrParser &       parserIn,
                                    TList<Attr*> *     plistAttr);

    EStatus  SaveList              (RStrParser &       parserIn,
                                    TList<Attr*> *     plistAttr);

  private:
    
    EStatus  LoadInt               (RStrParser &       parserIn,
                                    Attr * *           ppattrIn);
                                         
    EStatus  SaveInt               (RStrParser &       parserIn,
                                    AttrInt &          attrIn);

    EStatus  LoadFloat             (RStrParser &       parserIn,
                                    Attr * *           ppattrIn);

    EStatus  SaveFloat             (RStrParser &       parserIn,
                                    AttrFloat &        attrIn);

    EStatus  LoadString            (RStrParser &       parserIn,
                                    Attr * *           ppattrIn);

    EStatus  SaveString            (RStrParser &       parserIn,
                                    AttrString &       attrIn);

    EStatus  LoadStringArray       (RStrParser &       parserIn,
                                    Attr * *           ppattrIn);

    EStatus  SaveStringArray       (RStrParser &       parserIn,
                                    AttrStringArray &  attrIn);

    EStatus  LoadIntArray          (RStrParser &       parserIn,
                                    Attr * *           ppattrIn);

    EStatus  SaveIntArray          (RStrParser &       parserIn,
                                    AttrIntArray &     attrIn);

    EStatus  LoadFloatArray        (RStrParser &       parserIn,
                                    Attr * *           ppattrIn);

    EStatus  SaveFloatArray        (RStrParser &       parserIn,
                                    AttrFloatArray &   attrIn);
  };
  
  
  
#endif // ATTRBINARYSERIALIZER  