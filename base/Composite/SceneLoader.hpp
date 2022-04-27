/* -----------------------------------------------------------------
                         Raven Node Script Parser

     This module implements reading and writing Nodes and Attrs
       stored in a Renderman-like attribute definition string.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2006-2014, Michael T. Duffy II.  All rights reserved.
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

#ifndef SCENELOADER_HPP
#define SCENELOADER_HPP

#include "Sys/Types.hpp"

#include "Util/RStr.hpp"
#include "Util/RStrParser.hpp"
#include "Composite/Node.hpp"
#include "Composite/World.hpp"
#include "Gfx/Anim.hpp"



//------------------------------------------------------------------------
class SceneLoader
  {
  private:

    static  RStr    strDecryptKey;

  public:

             SceneLoader            ();

             ~SceneLoader           ();

    VOID     PrintError             (EStatus         statusIn,
                                     RStrParser &    parserBufferIn,
                                     const char *    szFilenameForErrorIn);

    EStatus  ReadFile               (const char *    szFilenameIn,
                                     const char *    szHeirarchyPrefixIn,
                                     World *         pwldWorldIn,
                                     BOOL            bLoadAnim    = true);

    EStatus  ReadBuffer             (RStrParser &    parserBufferIn,
                                     const char *    szHeirarchyPrefixIn,
                                     World *         pwldWorldIn,
                                     BOOL            bLoadAnim    = true,
                                     const char *    szFilenameIn = "buffer");

    EStatus  ReadAnimFile           (const char *    szFilenameIn);

    EStatus  ReadAnimBuffer         (RStrParser &    parserBufferIn,
                                     const char *    szFilenameIn = "buffer");

    EStatus  ParseInclude           (RStrParser &          parserBufferIn,
                                     const RStrParser &    parserValueIn,
                                     const char *          szHeirarchyPrefixIn,
                                     World *               pwldWorldIn,
                                     BOOL                  bLoadAnim);

    EStatus  ParseValueRegistry     (const RStr &    strKeyIn,
                                     RStrParser &    parserValueIn);

    EStatus  ReadNode               (RStrParser &    parserBufferIn,
                                     RStrParser &    parserNodeNameIn,
                                     const char *    szHeirarchyPrefixIn,
                                     World *         pwldWorldIn);

    EStatus  ReadComponentParam     (const RStr &    strKeyIn,
                                     RStrParser &    parserValueIn,
                                     Component *     pcompCurr);


    EStatus  ReadAnimClip           (RStrParser &    parserBufferIn,
                                     const char *    szClipTypeIn,
                                     RStrParser &    parserAnimNameIn,
                                     const char *    szHeirarchyPrefixIn,
                                     const char *    szFilenameIn);

    EStatus  ReadAnimClipParams     (const RStr &    strKeyIn,
                                     RStrParser &    parserValueIn,
                                     const char *    szHeirarchyPrefixIn,
                                     AnimClipBase *  pclipCurr);

    EStatus  ReadMappedCurveParams  (const RStr &    strKeyIn,
                                     RStrParser &    parserValueIn,
                                     MappedCurve *   pCurveIn);

    static VOID  SetDecryptKey      (const char *    pszKeyIn)     {strDecryptKey.Set (pszKeyIn);};


  };


#endif // SCENELOADER_HPP


