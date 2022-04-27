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

#include <stdlib.h>
#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__)

#include "Containers/TList.hpp"
#include "ValueRegistry/ValueRegistry.hpp"
#include "Script/Expression.hpp"
#include "Script/TimerExpression.hpp"








//-----------------------------------------------------------------------------
TimerExpression::TimerExpression  ()
  {
  ccType = TimerExpression::TypeID();
  plistTokExprOnPost = NULL;
  pReg = ValueRegistry::Root();
  };

//-----------------------------------------------------------------------------
TimerExpression::~TimerExpression ()
  {
  if (plistTokExprOnPost != NULL)
    {
    Expression::FreeTokenList (&plistTokExprOnPost);
    };
  };

//-----------------------------------------------------------------------------
VOID  TimerExpression::SetExpression  (const char *  pExpIn)
  {
  strExprOnPost.Set (pExpIn);
  plistTokExprOnPost = Expression::Compile (pExpIn);
  };

//-----------------------------------------------------------------------------
TimerBase *  TimerExpression::Duplicate  (VOID) const
  {
  TimerExpression * pNew = new TimerExpression ();
  pNew->Set (*this); // NOTE:  This will not copy TimerExpression vars, which is fine.
  return pNew;
  };

//-----------------------------------------------------------------------------
BOOL  TimerExpression::Post  (INT64  iMSecondsSinceLast)
  {
  Expression::Execute (plistTokExprOnPost, pReg);
  return (bOneShot);
  };

//-----------------------------------------------------------------------------
VOID  TimerExpression::Serialize  (ValueRegistry &  regIn)
  {
  TimerBase::Serialize (regIn);
  regIn.SetString ("exp", strExprOnPost);
  };

//-----------------------------------------------------------------------------
VOID  TimerExpression::Deserialize  (ValueRegistry &  regIn)
  {
  TimerBase::Deserialize (regIn);
  RStr  strExp;
  regIn.GetString ("exp", strExp);

  SetExpression (strExp.AsChar ());
  };
