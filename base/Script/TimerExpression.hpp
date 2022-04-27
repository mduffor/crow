/* -----------------------------------------------------------------
                             Timer Expression

     This module implements a timer that calls an expression when
   it posts.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

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

#ifndef TIMEREXPRESSION_HPP
#define TIMEREXPRESSION_HPP


#include "Sys/Types.hpp"
#include "Containers/TList.hpp"
#include "ValueRegistry/ValueRegistry.hpp"
#include "Util/RStr.hpp"
#include "Script/Expression.hpp"
#include "Sys/Timer.hpp"

//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------

//------------------------------------------------------------------------------
class TimerExpression : public TimerBase
  {
  public:

    RStr              strExprOnPost;
    TList<Token*>  *  plistTokExprOnPost;
    ValueRegistry *   pReg;

  public:

                         TimerExpression  ();

                         ~TimerExpression ();

    static UINT32        TypeID           (VOID)                        {return MAKE_FOUR_CODE ("TEXP");};

    VOID                 SetExpression    (const char *  pExpIn);

    VOID                 SetRegistry      (ValueRegistry *  pRegIn)     {pReg = pRegIn;};

    virtual VOID         Copy             (const TimerBase &  timerIn) override {return;};

    virtual TimerBase *  Duplicate        (VOID) const override ;

    virtual BOOL         Post             (INT64  iMSecondsSinceLast)  override;

    virtual VOID         Serialize        (ValueRegistry &  regIn)     override;

    virtual VOID         Deserialize      (ValueRegistry &  regIn)     override;



  };



#endif // TIMEREXPRESSION_HPP
