/* -----------------------------------------------------------------
                      Function : GUI Commands

     These commands are general purpose for controlling the GUI.

   ----------------------------------------------------------------- */

// Authors:  Michael T. Duffy  (mduffor@gmail.com)

// Modified BSD License:
//
// Copyright (c) 2018, Michael T. Duffy II.  All rights reserved.
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

#ifndef FNGUI_HPP
#define FNGUI_HPP

#include "Sys/Types.hpp"
#include "Script/Expression.hpp"

//------------------------------------------------------------------------
// Defines
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// Class Definitions
//------------------------------------------------------------------------
/**
  @addtogroup gametech
  @{
*/

//-----------------------------------------------------------------------------
class FnShowScreen : public ExpressionFn
  {
  public:

                  FnShowScreen  ()  {SetName ("ShowScreen");};

    virtual       ~FnShowScreen ()  {};

    virtual VOID  OnCall        (Token *          ptokParamsIn,
                                 ValueRegistry *  pRegistryIn,
                                 Token *          ptokResultOut);
  };

//-----------------------------------------------------------------------------
class FnTransPopScreen : public ExpressionFn
  {
  public:

                  FnTransPopScreen  ()  {SetName ("TransitionPopScreen");};

    virtual       ~FnTransPopScreen ()  {};

    virtual VOID  OnCall            (Token *          ptokParamsIn,
                                     ValueRegistry *  pRegistryIn,
                                     Token *          ptokResultOut);
  };

//-----------------------------------------------------------------------------
class FnPopScreen : public ExpressionFn
  {
  public:

                  FnPopScreen  ()  {SetName ("PopScreen");};

    virtual       ~FnPopScreen ()  {};

    virtual VOID  OnCall       (Token *          ptokParamsIn,
                                ValueRegistry *  pRegistryIn,
                                Token *          ptokResultOut);
  };
/** @} */ // end of gametech group


#endif // FNGUI_HPP

