/* -----------------------------------------------------------------
                  Function : Push To / Pop From Array

     This command pushes and pops values in and out of array value
   elems.  This can be used, for example, to save and restore UI state.

   ----------------------------------------------------------------- */

// Authors:  Michael T. Duffy  (mduffor@gmail.com)

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

#include "Sys/Types.hpp"
#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "GameTech/FnValueRegistry.hpp"

//-----------------------------------------------------------------------------
VOID RegisterValueRegistryCommands (VOID)
  {
  Expression::RegisterFunction (new FnPushToArray);
  Expression::RegisterFunction (new FnPopFromArray);
  Expression::RegisterFunction (new FnRemoveFromArray);
  Expression::RegisterFunction (new FnClearArray);
  Expression::RegisterFunction (new FnPushToArrayIfEmpty);
  Expression::RegisterFunction (new FnToggle);
  };


//-----------------------------------------------------------------------------
//  Push To / Pop From Array Function
//-----------------------------------------------------------------------------


//------------------------------------------------------------------------------
VOID FnPushToArray::OnCall  (Token *          ptokParamsIn,
                             ValueRegistry *  pRegistryIn,
                             Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 2)
    {
    DBG_ERROR ("Wrong number of parameters passed to PushToArray(strValue, strArrayName)");
    return;
    };

  RStr  strValueName = ptokParamsIn->AsString (pRegistryIn);
  RStr  strArrayName = ptokParamsIn->Next()->AsString (pRegistryIn);

  pRegistryIn->SetStringInArray (strArrayName.AsChar (),
                                 strValueName.AsChar (),
                                 -1); // append
  };

//------------------------------------------------------------------------------
VOID FnPopFromArray::OnCall  (Token *          ptokParamsIn,
                              ValueRegistry *  pRegistryIn,
                              Token *          ptokResultOut)
  {
  ASSERT (ptokResultOut != NULL);
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 1)
    {
    DBG_ERROR ("Wrong number of parameters passed to PopFromArray(strArrayName)");
    return;
    };

  RStr  strArrayName = ptokParamsIn->AsString (pRegistryIn);
  INT   iArraySize = pRegistryIn->GetArrayLength (strArrayName.AsChar ());

  ptokResultOut->eType = TokenType::kString;

  if (iArraySize > 0)
    {
    ptokResultOut->Set (pRegistryIn->GetStringInArray (strArrayName.AsChar (), iArraySize - 1), pRegistryIn);
    pRegistryIn->DeleteArrayElem (strArrayName.AsChar (),
                                  -1); // delete last elem
    }
  else
    {
    DBG_ERROR ("Unable PopFromArray() because the Array is empty!");
    };
  };


//------------------------------------------------------------------------------
VOID FnRemoveFromArray::OnCall  (Token *          ptokParamsIn,
                                 ValueRegistry *  pRegistryIn,
                                 Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 2)
    {
    DBG_ERROR ("Wrong number of parameters passed to RemoveFromArray(strValue, strArrayName)");
    return;
    };

  RStr  strValueName = ptokParamsIn->AsString (pRegistryIn);
  RStr  strArrayName = ptokParamsIn->Next()->AsString (pRegistryIn);

  pRegistryIn->DeleteStringInArray (strArrayName.AsChar (),
                                    strValueName.AsChar ());
  };

//------------------------------------------------------------------------------
VOID FnClearArray::OnCall  (Token *          ptokParamsIn,
                            ValueRegistry *  pRegistryIn,
                            Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 1)
    {
    DBG_ERROR ("Wrong number of parameters passed to ClearArray(strArrayName)");
    return;
    };

  RStr  strArrayName = ptokParamsIn->AsString (pRegistryIn);

  pRegistryIn->ClearArray (strArrayName.AsChar ());
  };

//------------------------------------------------------------------------------
VOID FnPushToArrayIfEmpty::OnCall  (Token *          ptokParamsIn,
                                    ValueRegistry *  pRegistryIn,
                                    Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 2)
    {
    DBG_ERROR ("Wrong number of parameters passed to PushToArrayIfEmpty(strValue, strArrayName)");
    return;
    };

  RStr  strValueName = ptokParamsIn->AsString (pRegistryIn);
  RStr  strArrayName = ptokParamsIn->Next()->AsString (pRegistryIn);

  if (pRegistryIn->GetArrayLength (strArrayName.AsChar ()))
    {
    pRegistryIn->SetStringInArray (strArrayName.AsChar (),
                                   strValueName.AsChar (),
                                   -1); // append
    };
  };

//------------------------------------------------------------------------------
VOID FnToggle::OnCall  (Token *          ptokParamsIn,
                        ValueRegistry *  pRegistryIn,
                        Token *          ptokResultOut)
  {
  INT  iNumTokens = ptokParamsIn->ArrayCount ();

  if (iNumTokens != 1)
    {
    DBG_ERROR ("Wrong number of parameters passed to Toggle(strValueKey)");
    return;
    };

  RStr  strValueKey;
  strValueKey.Set (ptokParamsIn->AsString (pRegistryIn), TRUE);

  pRegistryIn->SetBool (strValueKey, !pRegistryIn->GetBool (strValueKey));

  DBG_INFO ("Toggle value of %s is now %s", ptokParamsIn->AsString (pRegistryIn), pRegistryIn->GetBool (strValueKey) ? "true" : "false");

  };
