/* -----------------------------------------------------------------
                       Expression Default Functions

     This module implements some default functions that can be called
     by the expression solver

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

#ifndef EXPRESSIONDEFAULTFN_HPP
#define EXPRESSIONDEFAULTFN_HPP

#include "Sys/Types.hpp"
#include "Containers/TList.hpp"
#include "ValueRegistry/ValueRegistry.hpp"
#include "Script/Expression.hpp"

//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------

VOID RegisterDefaultExpressionFn (VOID);

// TODO: Should all the default functions be PascalCase?

//-----------------------------------------------------------------------------
class FnMath
  {
  public:

    static VOID  Min        (Token *          ptokParamsIn,
                             ValueRegistry *  pRegistryIn,
                             Token *          ptokResultOut);
    static VOID  Max        (Token *          ptokParamsIn,
                             ValueRegistry *  pRegistryIn,
                             Token *          ptokResultOut);
    static VOID  Lerp       (Token *          ptokParamsIn,
                             ValueRegistry *  pRegistryIn,
                             Token *          ptokResultOut);
    static VOID  Rand       (Token *          ptokParamsIn,
                             ValueRegistry *  pRegistryIn,
                             Token *          ptokResultOut);
    static VOID  Choose     (Token *          ptokParamsIn,
                             ValueRegistry *  pRegistryIn,
                             Token *          ptokResultOut);
  };

//-----------------------------------------------------------------------------
class FnUtil
  {
  public:
    static VOID  PrintRegistry  (Token *          ptokParamsIn,
                                 ValueRegistry *  pRegistryIn,
                                 Token *          ptokResultOut);
    static VOID  Print          (Token *          ptokParamsIn,
                                 ValueRegistry *  pRegistryIn,
                                 Token *          ptokResultOut);
    static VOID  GetInt         (Token *          ptokParamsIn,
                                 ValueRegistry *  pRegistryIn,
                                 Token *          ptokResultOut);
    static VOID  GetString      (Token *          ptokParamsIn,
                                 ValueRegistry *  pRegistryIn,
                                 Token *          ptokResultOut);
    static VOID  Eval           (Token *          ptokParamsIn,
                                 ValueRegistry *  pRegistryIn,
                                 Token *          ptokResultOut);
  };

//-----------------------------------------------------------------------------
class FnOpenURL : public ExpressionFn
  {
  public:

                   FnOpenURL ()  {SetName ("OpenURL");};

    virtual       ~FnOpenURL ()  {};

    virtual VOID  OnCall        (Token *          ptokParamsIn,
                                 ValueRegistry *  pRegistryIn,
                                 Token *          ptokResultOut) override;
  };

//-----------------------------------------------------------------------------
class FnOpenTwitter : public ExpressionFn
  {
  public:

                  FnOpenTwitter  ()  {SetName ("OpenTwitter");};

    virtual       ~FnOpenTwitter ()  {};

    virtual VOID  OnCall         (Token *          ptokParamsIn,
                                  ValueRegistry *  pRegistryIn,
                                  Token *          ptokResultOut) override;
  };


//-----------------------------------------------------------------------------
class FnOpenFacebook : public ExpressionFn
  {
  public:

                  FnOpenFacebook  ()  {SetName ("OpenFacebook");};

    virtual       ~FnOpenFacebook ()  {};

    virtual VOID  OnCall          (Token *          ptokParamsIn,
                                   ValueRegistry *  pRegistryIn,
                                   Token *          ptokResultOut) override;
  };

//-----------------------------------------------------------------------------
class FnSystem
  {
  public:

    static VOID  CloseApplication  (Token *          ptokParamsIn,
                                    ValueRegistry *  pRegistryIn,
                                    Token *          ptokResultOut);
    static VOID  ShowNotification  (Token *          ptokParamsIn,
                                    ValueRegistry *  pRegistryIn,
                                    Token *          ptokResultOut);
  };

//-----------------------------------------------------------------------------
class FnTouch
  {
  public:

    static VOID  Click      (Token *          ptokParamsIn,
                             ValueRegistry *  pRegistryIn,
                             Token *          ptokResultOut);
    static VOID  Press      (Token *          ptokParamsIn,
                             ValueRegistry *  pRegistryIn,
                             Token *          ptokResultOut);
    static VOID  Release    (Token *          ptokParamsIn,
                             ValueRegistry *  pRegistryIn,
                             Token *          ptokResultOut);
    static VOID  Action     (Token *          ptokParamsIn,
                             ValueRegistry *  pRegistryIn,
                             Token *          ptokResultOut);
  };


//-----------------------------------------------------------------------------
class FnTimeDate
  {
  public:

    static VOID  LocalBeforeDate    (Token *          ptokParamsIn,
                                     ValueRegistry *  pRegistryIn,
                                     Token *          ptokResultOut);

    static VOID  LocalAfterDate     (Token *          ptokParamsIn,
                                     ValueRegistry *  pRegistryIn,
                                     Token *          ptokResultOut);

    static VOID  LocalBetweenDates  (Token *          ptokParamsIn,
                                     ValueRegistry *  pRegistryIn,
                                     Token *          ptokResultOut);

    static VOID  AddToDate          (Token *          ptokParamsIn,
                                     ValueRegistry *  pRegistryIn,
                                     Token *          ptokResultOut);

    static VOID  LocalTimeRemaining (Token *          ptokParamsIn,
                                     ValueRegistry *  pRegistryIn,
                                     Token *          ptokResultOut);


    static VOID  SecondsToString    (Token *          ptokParamsIn,
                                     ValueRegistry *  pRegistryIn,
                                     Token *          ptokResultOut);

    static VOID  LocalCountdown     (Token *          ptokParamsIn,
                                     ValueRegistry *  pRegistryIn,
                                     Token *          ptokResultOut);

  };

/*
- bool local_before_date (dateIn, currentTimeIn = now)
- bool local_after_date (dateIn, currentTimeIn = now)
- bool local_between_dates (startDateIn, endDateIn, currentTimeIn = now)
- date add_to_date (startDateIn, deltaTimeIn)
*/


// TODO: add a "Choose" function for strings  "ChooseStr"





#endif // EXPRESSIONDEFAULTFN_HPP
