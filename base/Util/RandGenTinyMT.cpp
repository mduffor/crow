
/* -----------------------------------------------------------------
                             RandGen TinyMT

     This class refactors the TinyMT code into RandGen format.

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

#include <stdint.h>
#include <inttypes.h>
#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__)

#include "Util/RandGenTinyMT.hpp"


// Header from tinymt32.c, where the guts of this code originate.

/**
 * @file tinymt32.c
 *
 * @brief Tiny Mersenne Twister only 127 bit internal state
 *
 * @author Mutsuo Saito (Hiroshima University)
 * @author Makoto Matsumoto (The University of Tokyo)
 *
 * Copyright (C) 2011 Mutsuo Saito, Makoto Matsumoto,
 * Hiroshima University and The University of Tokyo.
 * All rights reserved.
 *
 * The 3-clause BSD License is applied to this software, see
 * LICENSE.txt
 */



// Code is based on the TinyMT version of Mersenne Twister
//  http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/TINYMT/index.html
//  License: http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/TINYMT/LICENSE.txt
//-----------------------------------------------------------------------------
//  RandGen TinyMT
//-----------------------------------------------------------------------------

#define MIN_LOOP 8
#define PRE_LOOP 8

/**
 * This function represents a function used in the initialization
 * by init_by_array
 * @param x 32-bit integer
 * @return 32-bit integer
 */
//-----------------------------------------------------------------------------
static uint32_t ini_func1 (uint32_t x)
  {
  return (x ^ (x >> 27)) * UINT32_C(1664525);
  }

/**
 * This function represents a function used in the initialization
 * by init_by_array
 * @param x 32-bit integer
 * @return 32-bit integer
 */
//-----------------------------------------------------------------------------
static uint32_t ini_func2 (uint32_t x)
  {
  return (x ^ (x >> 27)) * UINT32_C(1566083941);
  }



//-----------------------------------------------------------------------------
RandGenTinyMT::RandGenTinyMT ()
  {
    status[0] = status[1] = status[2] = status[3] = 0;
    mat1 = mat2 = tmat = 0;
  };



//-----------------------------------------------------------------------------
VOID RandGenTinyMT::PeriodCertification (VOID)
  {
  // This function certificate the period of 2^127-1.

  if ((status[0] & TINYMT32_MASK) == 0 &&
        status[1] == 0 &&
        status[2] == 0 &&
        status[3] == 0)
    {
    status[0] = 'T';
    status[1] = 'I';
    status[2] = 'N';
    status[3] = 'Y';
    }
  }

//-----------------------------------------------------------------------------
VOID RandGenTinyMT::Init (UINT32  uSeed)
  {
  // Initialize the internal state with a 32-bit unsigned int seed
  status[0] = uSeed;
  status[1] = mat1;
  status[2] = mat2;
  status[3] = tmat;
  for (unsigned int i = 1; i < MIN_LOOP; i++)
    {
    status[i & 3] ^= i + UINT32_C(1812433253)
          * (status[(i - 1) & 3]
              ^ (status[(i - 1) & 3] >> 30));
    }
  PeriodCertification ();
  for (unsigned int i = 0; i < PRE_LOOP; i++)
    {
    CalcNextState ();
    }
  }


//-----------------------------------------------------------------------------
VOID RandGenTinyMT::InitByArray (UINT32  auInitKey[],
                                 INT     iKeyLength)
  {
  // Initialize the internal state with an array of 32 bit unsigned int seeds

  const unsigned int lag = 1;
  const unsigned int mid = 1;
  const unsigned int size = 4;
  unsigned int i, j;
  unsigned int count;
  uint32_t r;
  uint32_t * st = &status[0];

  st[0] = 0;
  st[1] = mat1;
  st[2] = mat2;
  st[3] = tmat;
  if (iKeyLength + 1 > MIN_LOOP)
    {
    count = (unsigned int)iKeyLength + 1;
    }
  else
    {
    count = MIN_LOOP;
    }
  r = ini_func1 (st[0] ^ st[mid % size] ^ st[(size - 1) % size]);
  st[mid % size] += r;
  r += (unsigned int)iKeyLength;
  st[(mid + lag) % size] += r;
  st[0] = r;
  count--;
  for (i = 1, j = 0; (j < count) && (j < (unsigned int)iKeyLength); j++)
    {
    r = ini_func1 (st[i % size] ^ st[(i + mid) % size] ^ st[(i + size - 1) % size]);
    st[(i + mid) % size] += r;
    r += auInitKey[j] + i;
    st[(i + mid + lag) % size] += r;
    st[i % size] = r;
    i = (i + 1) % size;
    }
  for (; j < count; j++)
    {
    r = ini_func1 (st[i % size] ^ st[(i + mid) % size] ^ st[(i + size - 1) % size]);
    st[(i + mid) % size] += r;
    r += i;
    st[(i + mid + lag) % size] += r;
    st[i % size] = r;
    i = (i + 1) % size;
    }
  for (j = 0; j < size; j++)
    {
    r = ini_func2 (st[i % size] + st[(i + mid) % size] + st[(i + size - 1) % size]);
    st[(i + mid) % size] ^= r;
    r -= i;
    st[(i + mid + lag) % size] ^= r;
    st[i % size] = r;
    i = (i + 1) % size;
    }
  PeriodCertification();

  for (i = 0; i < PRE_LOOP; i++)
    {
    CalcNextState ();
    }
  }

//-----------------------------------------------------------------------------
VOID RandGenTinyMT::CalcNextState (VOID)
  {
  uint32_t x;
  uint32_t y;

  y = status[3];
  x = (status[0] & TINYMT32_MASK)
      ^ status[1]
      ^ status[2];
  x ^= (x << TINYMT32_SH0);
  y ^= (y >> TINYMT32_SH0) ^ x;
  status[0] = status[1];
  status[1] = status[2];
  status[2] = x ^ (y << TINYMT32_SH1);
  status[3] = y;
  int32_t const a = -((int32_t)(y & 1)) & (int32_t)mat1;
  int32_t const b = -((int32_t)(y & 1)) & (int32_t)mat2;
  status[1] ^= (uint32_t)a;
  status[2] ^= (uint32_t)b;
  }

//-----------------------------------------------------------------------------
INT32  RandGenTinyMT::Int32  (VOID)
  {
  CalcNextState ();
  return INT32 (CalcInt32 () & UINT32_C(0x7FFFFFFF));
  };

//-----------------------------------------------------------------------------
UINT32  RandGenTinyMT::UInt32  (VOID)
  {
  CalcNextState ();
  return CalcInt32 ();
  };

//-----------------------------------------------------------------------------
FLOAT  RandGenTinyMT::Float01  (VOID)
  {
  CalcNextState ();

  // This is faster on Intel
  //return (FLOAT)(CalcInt32() >> 8) * TINYMT32_MUL;

  return CalcFloat(UINT32_C(0x3f800000)) - 1.0f;
  };

//-----------------------------------------------------------------------------
FLOAT  RandGenTinyMT::FloatOC  (VOID)
  {
  return 1.0f - Float01();
  };

//-----------------------------------------------------------------------------
FLOAT  RandGenTinyMT::FloatOO  (VOID)
  {
  CalcNextState ();
  return CalcFloat(UINT32_C(0x3f800001)) - 1.0f;
  };

//-----------------------------------------------------------------------------
DOUBLE  RandGenTinyMT::Double  (VOID)
  {
  CalcNextState ();
  return CalcInt32() * (1.0 / 4294967296.0);
  };

//-----------------------------------------------------------------------------
INT  RandGenTinyMT::IntRange  (INT  iMinInclusive,
                               INT  iMaxInclusive)
  {
  INT  iRandom = Int32();
  INT  iRange  = iMaxInclusive - iMinInclusive + 1;

  return (iMinInclusive + (iRandom % iRange));
  };

//-----------------------------------------------------------------------------
FLOAT  RandGenTinyMT::FloatRange  (FLOAT  fMinInclusive,
                                   FLOAT  fMaxExclusive)
  {
  FLOAT  fRandom = Float01();
  FLOAT  fRange = fMaxExclusive - fMinInclusive;

  return (fMinInclusive + (fRange * fRandom));

  };

//-----------------------------------------------------------------------------
VOID  RandGenTinyMT::Serialize  (RStrParser &  parserOut)
  {
  parserOut.SetU4_LEnd (0x01); // version, just in case

  parserOut.SetU4_LEnd ((UINT32)status[0]);
  parserOut.SetU4_LEnd ((UINT32)status[1]);
  parserOut.SetU4_LEnd ((UINT32)status[2]);
  parserOut.SetU4_LEnd ((UINT32)status[3]);

  parserOut.SetU4_LEnd ((UINT32)mat1);
  parserOut.SetU4_LEnd ((UINT32)mat2);
  parserOut.SetU4_LEnd ((UINT32)tmat);
  };

//-----------------------------------------------------------------------------
VOID  RandGenTinyMT::Deserialize  (RStrParser &  parserIn)
  {
  if (parserIn.IsEmpty ()) return;

  UINT  uVersion = parserIn.GetU4_LEnd ();

  // check version.  If we don't understand it, exit out.
  if (uVersion != 0x01) return;

  status[0] = parserIn.GetU4_LEnd ();
  status[1] = parserIn.GetU4_LEnd ();
  status[2] = parserIn.GetU4_LEnd ();
  status[3] = parserIn.GetU4_LEnd ();

  mat1 = parserIn.GetU4_LEnd ();
  mat2 = parserIn.GetU4_LEnd ();
  tmat = parserIn.GetU4_LEnd ();
  };


















































