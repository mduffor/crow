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

#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__)

#include "Util/RandGenTinyMT.hpp"
#include "Util/Random.hpp"
#include "Util/RandGenTinyMT.hpp"

//-----------------------------------------------------------------------------
//  Rand Sequencer
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
RandSequencer::RandSequencer (UINT32  uIdIn,
                              UINT32  uSeedIn)
  {
  uId = uIdIn;

  // NOTE:  In the future you can support different types of generators
  pGenerator = new RandGenTinyMT;

  dynamic_cast<RandGenTinyMT*>(pGenerator)->Init (uSeedIn);

  };

//-----------------------------------------------------------------------------
VOID  RandSequencer::Serialize  (RStrParser &  parserOut)
  {
  parserOut.SetU4_LEnd (0x01); // version, just in case

  parserOut.SetU4_LEnd (uId);  // identifier

  // hard-code the generator type, but at least identify it for future proofing.
  parserOut.SetU4_LEnd (MAKE_FOUR_CODE("TnMT"));

  pGenerator->Serialize (parserOut);
  };

//-----------------------------------------------------------------------------
VOID  RandSequencer::Deserialize  (RStrParser &  parserIn)
  {

  if (parserIn.IsEmpty ()) return;

  UINT  uVersion = parserIn.GetU4_LEnd ();

  // check version.  If we don't understand it, exit out.
  if (uVersion != 0x01) return;

  uId = parserIn.GetU4_LEnd ();

  UINT32  uType = parserIn.GetU4_LEnd ();

  if (uType != MAKE_FOUR_CODE("TnMT"))
    {
    // unknown type.  Return and use whatever was created in the constructor.
    };

  // NOTE: This assumes the generator is hard-coded as RangGenTinyMT
  pGenerator->Deserialize (parserIn);
  };


//-----------------------------------------------------------------------------
//  Random
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
RandSequencer *  Random::FindSequencer  (UINT  uIdIn)
  {
  for (TListItr<RandSequencer *> itrCurr = listSequencers.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    if ((*itrCurr)->uId == uIdIn)
      {
      return (*itrCurr);
      };
    }
  return (NULL);
  };

//-----------------------------------------------------------------------------
RandSequencer *  Random::NewSequencer   (UINT    uIdIn,
                                         UINT32  uSeedIn)
  {
  RandSequencer *  pNew = FindSequencer (uIdIn);

  if (pNew == NULL)
    {
    listSequencers.PushBack (new RandSequencer (uIdIn, uSeedIn));
    };
  return (pNew);
  };

//-----------------------------------------------------------------------------
INT32  Random::Int32  (UINT  uIdIn)
  {
  RandSequencer *  pSeq = FindSequencer (uIdIn);
  return ((pSeq != NULL) ? pSeq->Int32 () : 0);
  };

//-----------------------------------------------------------------------------
UINT32  Random::UInt32  (UINT  uIdIn)
  {
  RandSequencer *  pSeq = FindSequencer (uIdIn);
  return ((pSeq != NULL) ? pSeq->UInt32 () : 0);
  };

//-----------------------------------------------------------------------------
FLOAT  Random::Float01  (UINT  uIdIn)
  {
  RandSequencer *  pSeq = FindSequencer (uIdIn);
  return ((pSeq != NULL) ? pSeq->Float01 () : 0.0f);
  };

//-----------------------------------------------------------------------------
FLOAT  Random::FloatOC  (UINT  uIdIn)
  {
  RandSequencer *  pSeq = FindSequencer (uIdIn);
  return ((pSeq != NULL) ? pSeq->FloatOC () : 0.0f);
  };

//-----------------------------------------------------------------------------
FLOAT  Random::FloatOO  (UINT  uIdIn)
  {
  RandSequencer *  pSeq = FindSequencer (uIdIn);
  return ((pSeq != NULL) ? pSeq->FloatOO () : 0.0f);
  };

//-----------------------------------------------------------------------------
DOUBLE  Random::Double  (UINT  uIdIn)
  {
  RandSequencer *  pSeq = FindSequencer (uIdIn);
  return ((pSeq != NULL) ? pSeq->Double () : 0.0);
  };

//-----------------------------------------------------------------------------
INT  Random::IntRange   (UINT  uIdIn,
                         INT   iMinInclusive,
                         INT   iMaxInclusive)
  {
  RandSequencer *  pSeq = FindSequencer (uIdIn);
  return ((pSeq != NULL) ? pSeq->IntRange (iMinInclusive, iMaxInclusive) : 0);
  };

//-----------------------------------------------------------------------------
FLOAT  Random::FloatRange  (UINT   uIdIn,
                            FLOAT  fMinInclusive,
                            FLOAT  fMaxExclusive)
  {
  RandSequencer *  pSeq = FindSequencer (uIdIn);
  return ((pSeq != NULL) ? pSeq->FloatRange (fMinInclusive, fMaxExclusive) : 0.0f);
  };

//-----------------------------------------------------------------------------
VOID  Random::Serialize  (RStrParser &  parserOut)
  {
  parserOut.SetU4_LEnd (0x01); // version, just in case


  parserOut.SetU4_LEnd (listSequencers.Size ());
  for (TListItr<RandSequencer *> itrCurr = listSequencers.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    (*itrCurr)->Serialize (parserOut);
    }
  };

//-----------------------------------------------------------------------------
VOID  Random::Deserialize  (RStrParser &  parserIn)
  {
  if (parserIn.IsEmpty ()) return;

  UINT  uVersion = parserIn.GetU4_LEnd ();

  // check version.  If we don't understand it, exit out.
  if (uVersion != 0x01) return;

  INT  iNumSequencers = parserIn.GetU4_LEnd ();

  for (INT  iIndex = 0; iIndex < iNumSequencers; ++iIndex)
    {
    RandSequencer *  pNew = NewSequencer (0, 0);

    pNew->Deserialize (parserIn);
    };
  };


