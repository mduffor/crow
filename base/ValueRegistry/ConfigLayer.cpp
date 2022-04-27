/* -----------------------------------------------------------------
                            Config Layer

     This module implements a value registry composed of overlapping
     layers of key-value pairs.

   ----------------------------------------------------------------- */

// Authors:  Michael T. Duffy  (mduffor@gmail.com)

// Modified BSD License:
//
// Copyright (c) 2021, Michael T. Duffy II.  All rights reserved.
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

#include "ValueRegistry/ConfigLayer.hpp"

/*
[
  {
  "name" : "config_name",
  "enabled" : "expression",
  "prereq" : "expression",
  "variables" : [
    "type:Var.One" : "string with 'embedded quotes'.",
    "int:Var.Two" : 2
  ],
  "segments" : [
    {
    "name" : "SegA",
    "prereq" : "expression",
    "variables" : [
      "int:Var.Two" : 3
    ]
    },
    {
    "name" : "SegB",
    "prereq" : "expression",
    "variables" : [
      "int:Var.Two" : 4
    ]
    },
  ]
  }
]

*/



//-----------------------------------------------------------------------------
EStatus  ConfigLayer::Deserialize  (RStrParser &  parserIn,
                                    const char *  szSourceIn)
  {

  RemoveBaseSegment();
  RemoveAllSegments();

  pBase = new ConfigSubset ();

  EStatus  status = pBase->ParseSubset (parserIn, "");

  //printf ("Deserialized layer %s\n", pBase->strName.AsChar());

  //pBase->regVariables.DebugPrint();
  //printf (" layer %s done\n", pBase->strName.AsChar());

  return (status);
  };

//-----------------------------------------------------------------------------
VOID  ConfigLayer::ToJSON (RStrParser &  parserOut,
                           const char *  strSectionIndent,
                           const char *  strLineIndent,
                           const char *  strLineEnd)
  {
  if (pBase == NULL) return;
  pBase->ToJSON (parserOut, strSectionIndent, strLineIndent, strLineEnd);
  };

//-----------------------------------------------------------------------------
BOOL  ConfigLayer::IsActive          (ValueRegistry &  regIn)
  {
  // TODO:  Set the layer name into the registry according to ABTest rules.

  if (pBase != NULL)
    {
    return (pBase->IsActive (regIn));
    }
  return (TRUE);
  };

//-----------------------------------------------------------------------------
BOOL  ConfigLayer::IsExpressionDriven (VOID)
  {
  if (pBase == NULL) return (FALSE);

  if (pBase->IsExpressionDriven()) return (TRUE);

  for (TListItr<ConfigSubset*>  itrCurr = pBase->listSegments.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    if ((*itrCurr)->IsExpressionDriven ()) return (TRUE);
    };
  return (FALSE);
  };

//-----------------------------------------------------------------------------
ConfigSubset *  ConfigLayer::GetBaseSegment   (VOID)
  {
  return (pBase);
  };

//-----------------------------------------------------------------------------
ConfigSubset *  ConfigLayer::GetActiveSegment   (ValueRegistry &  regIn)
  {
  // TODO:  Set the layer name into the registry according to ABTest rules.
  if (pBase == NULL) return (NULL);

  for (TListItr<ConfigSubset*>  itrCurr = pBase->listSegments.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    if ((*itrCurr)->IsActive (regIn)) return (*itrCurr);
    };
  return (NULL);
  };

//-----------------------------------------------------------------------------
VOID  ConfigLayer::RemoveBaseSegment  (VOID)
  {
  if (pBase != NULL)
    {
    delete (pBase);
    pBase = NULL;
    };
  };

//-----------------------------------------------------------------------------
VOID  ConfigLayer::RemoveAllSegments  (VOID)
  {
  if (pBase == NULL) return;
  for (TListItr<ConfigSubset*>  itrCurr = pBase->listSegments.First ();
       itrCurr.IsValid ();
       ++itrCurr)
    {
    delete (*itrCurr);
    };
  pBase->listSegments.Empty();
  };

//-----------------------------------------------------------------------------
VOID  ConfigLayer::MergeInto          (ValueRegistry &  regTargetIn)
  {

  ConfigSubset *  psetBase   = GetBaseSegment();
  ConfigSubset *  psetActive = GetActiveSegment(regTargetIn);
  INT             iOrder     = GetOrder();

  if (psetBase != NULL)   psetBase->MergeInto   (regTargetIn, iOrder);
  if (psetActive != NULL) psetActive->MergeInto (regTargetIn, iOrder);
  };
