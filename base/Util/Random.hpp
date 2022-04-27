/* -----------------------------------------------------------------
                             Random

     This module implements an organized pseudo random number
   generation system.

   ----------------------------------------------------------------- */

// Modified BSD License:
//
// Copyright (c) 2020, Michael T. Duffy II.  All rights reserved.
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

#ifndef RANDOM_HPP
#define RANDOM_HPP


#include "Sys/Types.hpp"
#include "Util/RStr.hpp"
#include "Util/RStrParser.hpp"
#include "Containers/TList.hpp"

#include "Util/RandGen.hpp"

// RANDNBR generates a number between 0.0 and 1.0
// RANDSNBR generates a number between -1.0 and 1.0

// TODO: Write a better RNG
#define RANDMASK 0x7fffffff
#define RANDMASKHALF (0x7fffffff / 2)
#define RANDNBR ((rand() & RANDMASK)/(double) RANDMASK)
#define RANDSNBR ((double (rand() & RANDMASK)/double (RANDMASKHALF)) - double (1.0f))




//------------------------------------------------------------------------
class RandSequencer
  {
  private:

    RandGen *  pGenerator;

  public:

    UINT32  uId;  // an ID to identify this sequencer.  Can use Hash.

  public:

            RandSequencer (UINT32  uIdIn,
                           UINT32  uSeedIn);

    INT32   Int32         (VOID)                  {return (pGenerator->Int32());};

    UINT32  UInt32        (VOID)                  {return (pGenerator->UInt32());};

    FLOAT   Float01       (VOID)                  {return (pGenerator->Float01());};

    FLOAT   FloatOC       (VOID)                  {return (pGenerator->FloatOC());};

    FLOAT   FloatOO       (VOID)                  {return (pGenerator->FloatOO());};

    DOUBLE  Double        (VOID)                  {return (pGenerator->Double());};

    INT     IntRange      (INT    iMinInclusive,
                           INT    iMaxInclusive)  {return (pGenerator->IntRange(iMinInclusive, iMaxInclusive));};

    FLOAT   FloatRange    (FLOAT  fMinInclusive,
                           FLOAT  fMaxExclusive)  {return (pGenerator->FloatRange(fMinInclusive, fMaxExclusive));};

    VOID    Serialize     (RStrParser &  parserOut);

    VOID    Deserialize   (RStrParser &  parserIn);

  };


//------------------------------------------------------------------------
class Random
  {
  private:

    TList<RandSequencer *>   listSequencers;


  public:


    RandSequencer *  FindSequencer (UINT  uIdIn);

    RandSequencer *  NewSequencer  (UINT    uIdIn,
                                    UINT32  uSeedIn);

    INT32            Int32         (UINT  uIdIn);

    UINT32           UInt32        (UINT  uIdIn);

    FLOAT            Float01       (UINT  uIdIn);

    FLOAT            FloatOC       (UINT  uIdIn);

    FLOAT            FloatOO       (UINT  uIdIn);

    DOUBLE           Double        (UINT  uIdIn);

    INT              IntRange      (UINT  uIdIn,
                                    INT   iMinInclusive,
                                    INT   iMaxInclusive);

    FLOAT            FloatRange    (UINT   uIdIn,
                                    FLOAT  fMinInclusive,
                                    FLOAT  fMaxExclusive);

    //BOOL             Chance        (FLOAT  fChanceIn);

    VOID             Serialize     (RStrParser &  parserOut);

    VOID             Deserialize   (RStrParser &  parserIn);


  };



#endif // RANDOM_HPP
