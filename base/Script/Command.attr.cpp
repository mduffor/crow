/* -----------------------------------------------------------------
                             Command

     This module implements a command system, so that functions
   can be run by name.

   ----------------------------------------------------------------- */

// contact:  mduffor@gmail.com

// Modified BSD License:
//
// Copyright (c) 2015, Michael T. Duffy II.  All rights reserved.
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

#include "Debug.hpp"
ASSERTFILE (__FILE__);
#include "Script/Command.hpp"
#include "Composite/AttrInt.hpp"
#include "Composite/AttrFloat.hpp"
#include "Composite/AttrString.hpp"

//Deprecated
#ifdef REFERENCE

static TList<Command *>  listCommands;

//-----------------------------------------------------------------------------
Command::Command  ()
  {
  strName.Empty ();
  listCommands.PushBack (this);
  };

//-----------------------------------------------------------------------------
Command::Command  (const char *  szNameIn)
  {
  SetName (szNameIn);
  listCommands.PushBack (this);
  };

//-----------------------------------------------------------------------------
Command::~Command  ()
  {
  FreeParams ();
  };

//-----------------------------------------------------------------------------
VOID  Command::FreeParams (VOID)
  {
  for (TListItr<Attr*> itrCurr = listParameters.First (); itrCurr.IsValid (); ++itrCurr)
    {
    delete (*itrCurr);
    };
  listParameters.Empty ();
  };

//-----------------------------------------------------------------------------
Attr *  Command::SetParam  (const char *  szParamNameIn,
                             INT           iValueIn)
  {
  Attr *  pAttr = GetParam (szParamNameIn);
  if (pAttr->IsType (AttrInt::Identifier ()))
    {
    ((AttrInt *) pAttr)->Set (iValueIn);
    return (pAttr);
    }

  return (listParameters.PushBack (new AttrInt (szParamNameIn, iValueIn)));
  };

//-----------------------------------------------------------------------------
Attr *  Command::SetParam  (const char *  szParamNameIn,
                            FLOAT         fValueIn)
  {
  Attr *  pAttr = GetParam (szParamNameIn);
  if (pAttr->IsType (AttrFloat::Identifier ()))
    {
    ((AttrFloat *) pAttr)->Set (fValueIn);
    return (pAttr);
    }

  return (listParameters.PushBack (new AttrFloat (szParamNameIn, fValueIn)));
  };

//-----------------------------------------------------------------------------
Attr *  Command::SetParam  (const char *  szParamNameIn,
                            const char *  szValueIn)
  {
  Attr *  pAttr = GetParam (szParamNameIn);
  if (pAttr->IsType (AttrString::Identifier ()))
    {
    ((AttrString *) pAttr)->Set (szValueIn);
    return (pAttr);
    }

  return (listParameters.PushBack (new AttrString (szParamNameIn, szValueIn)));
  };

//-----------------------------------------------------------------------------
Attr *  Command::GetParam  (const char *  szParamNameIn)
  {
  for (TListItr<Attr*> itrCurr = listParameters.First (); itrCurr.IsValid (); ++itrCurr)
    {
    if (streq ((*itrCurr)->Name (), szParamNameIn))
      {
      return (*itrCurr);
      };
    };
  return (NULL);
  };

//-----------------------------------------------------------------------------
VOID  Command::ClearParams  (VOID)
  {
  for (TListItr<Attr*> itrCurr = listParameters.First (); itrCurr.IsValid (); ++itrCurr)
    {
    (*itrCurr)->Clear ();
    };
  };

//-----------------------------------------------------------------------------
VOID  Command::Invoke  (const char *    szNameIn,
                        TList<Attr*> *  pParamsIn)
  {
  for (TListItr<Command*> itrCurr = listCommands.First (); itrCurr.IsValid (); ++itrCurr)
    {
    if (streq ((*itrCurr)->Name (), szNameIn))
      {
      (*itrCurr)->ClearParams ();
      // copy parameters that will be sent to the command.
      if (pParamsIn != NULL)
        {
        for (TListItr<Attr*> itrParam = pParamsIn->First (); itrParam.IsValid (); ++itrParam)
          {
          if ((*itrParam)->IsType (AttrInt::Identifier ()))
            {
            (*itrCurr)->SetParam ((*itrParam)->Name (), ((AttrInt*)(*itrParam))->Value ());
            }
          else if ((*itrParam)->IsType (AttrFloat::Identifier ()))
            {
            (*itrCurr)->SetParam ((*itrParam)->Name (), ((AttrFloat*)(*itrParam))->Value ());
            }
          else if ((*itrParam)->IsType (AttrString::Identifier ()))
            {
            (*itrCurr)->SetParam ((*itrParam)->Name (), ((AttrString*)(*itrParam))->Value ().AsChar ());
            }
          };
        };
      (*itrCurr)->Invoke ();
      return;
      };
    };
  DBG_WARNING ("Command::Invoke unable to find command: %s", szNameIn);
  };

#endif // REFERENCE
