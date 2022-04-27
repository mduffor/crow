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

#ifndef RANDGENTINYMT_HPP
#define RANDGENTINYMT_HPP

#include "Sys/Types.hpp"
#include "Util/RStrParser.hpp"
#include "Util/RandGen.hpp"

#define TINYMT32_MEXP 127
#define TINYMT32_SH0 1
#define TINYMT32_SH1 10
#define TINYMT32_SH8 8
#define TINYMT32_MASK UINT32_C(0x7fffffff)
#define TINYMT32_MUL (1.0f / 16777216.0f)


//------------------------------------------------------------------------
class RandGenTinyMT : public RandGen
  {
  private:

    uint32_t status[4];
    uint32_t mat1;
    uint32_t mat2;
    uint32_t tmat;


  private:

    UINT32 CalcInt32 (VOID)
      {
      uint32_t t0, t1;
      t0 = status[3];
      #if defined(LINEARITY_CHECK)
        t1 = status[0] ^ (status[2] >> TINYMT32_SH8);
      #else
        t1 = status[0] + (status[2] >> TINYMT32_SH8);
      #endif
      t0 ^= t1;
      if ((t1 & 1) != 0)
        {
        t0 ^= tmat;
        }
      return t0;
      }

    FLOAT CalcFloat (UINT32  uMaskIn)
      {
      // return floating point number r (1.0 <= r < 2.0)
      uint32_t t0, t1;
      union
        {
        uint32_t u;
        float    f;
        } conv;

      t0 = status[3];
      #if defined(LINEARITY_CHECK)
        t1 = status[0] ^ (status[2] >> TINYMT32_SH8);
      #else
        t1 = status[0] + (status[2] >> TINYMT32_SH8);
      #endif
      t0 ^= t1;
      if ((t1 & 1) != 0)
        {
        conv.u  = ((t0 ^ tmat) >> 9) | uMaskIn;
        }
      else
        {
        conv.u  = (t0 >> 9) | uMaskIn;
        }
      return conv.f;
      }

    VOID    CalcNextState        (VOID);

    VOID    PeriodCertification  (VOID);

  public:

            RandGenTinyMT ();

    VOID    Init                 (UINT32    uSeed);

    VOID    InitByArray          (UINT32    auInitKey [],
                                  INT       iKeyLength);


    INT32   Int32                (VOID) override;   // 31-bit positive signed integer r (0 <= r < 2^31)

    UINT32  UInt32               (VOID) override;   // 32-bit unsigned integer r (0 <= r < 2^32)

    FLOAT   Float01              (VOID) override;   // return floating point number r (0.0 <= r < 1.0)

    FLOAT   FloatOC              (VOID) override;   // return floating point number r (0.0 < r <= 1.0)

    FLOAT   FloatOO              (VOID) override;   // return floating point number r (0.0 < r < 1.0)

    DOUBLE  Double               (VOID) override;   // return floating point number r (0.0 <= r < 1.0)

    INT     IntRange             (INT  iMinInclusive,
                                  INT  iMaxInclusive) override;

    FLOAT   FloatRange           (FLOAT  fMinInclusive,
                                  FLOAT  fMaxExclusive) override;

    VOID    Serialize            (RStrParser &  parserOut) override;

    VOID    Deserialize          (RStrParser &  parserIn) override;

  };


#endif // RANDGENTINYMT_HPP

