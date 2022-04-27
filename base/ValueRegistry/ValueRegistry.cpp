/* -----------------------------------------------------------------
                            Value Registry

     This module implements a registry of Key Value pairs where the
     value can be one of several different base types.

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

#include "Sys/Types.hpp"

#include "Debug.hpp"
ASSERTFILE (__FILE__)

#include "ValueRegistry/ValueRegistry.hpp"


/*
    BOOL                  ValueElem::MatchesName   (const char *  szNameIn,
                                         HASH_T        uHashIn)     {DBG_INFO("Comparing \"%s\" to \"%s\" and \"%s\"", szNameIn, strName.AsChar(), strAltName.AsChar());
                                         return (strName.Equals    (uHashIn, szNameIn) ||
                                                                             strAltName.Equals (uHashIn, szNameIn));};
    BOOL                  ValueElem::MatchesName   (RStr &        strNameIn)   {
                                             DBG_INFO("Comparing \"%s\" to \"%s\" and \"%s\"", strNameIn.AsChar(), strName.AsChar(), strAltName.AsChar());

    return (strName.Equals    (strNameIn) ||
                                                                             strAltName.Equals (strNameIn));};
*/


ValueRegistry *  ValueRegistry::pRoot = NULL;

RStr ValueElem::strStringOut; // used by GetString for string conversion purposes.


//-----------------------------------------------------------------------------
//  ValueElem
//-----------------------------------------------------------------------------

const UINT32   ValueElem::kTypeInt         = MAKE_FOUR_CODE ("INT_");
const UINT32   ValueElem::kTypeFloat       = MAKE_FOUR_CODE ("FLT_");
const UINT32   ValueElem::kTypeDouble      = MAKE_FOUR_CODE ("DBL_");
const UINT32   ValueElem::kTypeVec         = MAKE_FOUR_CODE ("VEC_");
const UINT32   ValueElem::kTypeBool        = MAKE_FOUR_CODE ("BOOL");
const UINT32   ValueElem::kTypeString      = MAKE_FOUR_CODE ("STR_");
const UINT32   ValueElem::kTypeBlob        = MAKE_FOUR_CODE ("BLOB");
const UINT32   ValueElem::kTypeIntArray    = MAKE_FOUR_CODE ("AINT");
const UINT32   ValueElem::kTypeFloatArray  = MAKE_FOUR_CODE ("AFLT");
const UINT32   ValueElem::kTypeDoubleArray = MAKE_FOUR_CODE ("ADBL");
const UINT32   ValueElem::kTypeStringArray = MAKE_FOUR_CODE ("ASTR");
const UINT32   ValueElem::kTypePtr         = MAKE_FOUR_CODE ("PTR_");
const UINT32   ValueElem::kTypeLink        = MAKE_FOUR_CODE ("LINK");

//-----------------------------------------------------------------------------
VOID  ValueElem::SetName  (const char *  szIn)
  {
  // If there is a pipe character in the name, then set both a name and an
  //  alternative name.  Compare against both Name and AltName when searching
  //  for elems.


  strName = szIn;

  // NOTE:  Look to see if you can get rid of alt names, for reasons of
  //         performance and simplicity.  If you can't then implement uAltNameHash

  INT  iPipe = strName.FindChar ('|');
  if (iPipe == -1)
    {
    // no alt name
    strAltName.Empty();
    }
  else
    {
    strName.SubString (iPipe + 1, strName.Length () - iPipe - 1, strAltName);
    strName.TruncateRight (iPipe - 1);
    }

  strName.CalcHash ();
  strAltName.CalcHash ();
  uNameHash = strName.GetHash ();
  bSave = FALSE;
  };

//-----------------------------------------------------------------------------
VOID  ValueElem::SetAltName  (const char *  szIn)
  {
  strAltName = szIn;
  strAltName.CalcHash ();
  };

//-----------------------------------------------------------------------------
VOID  ValueElem::SetByType (ValueElem &  elemIn)
  {
  if      (elemIn.IsType (kTypeInt))          {SetInt (elemIn.GetInt ());}
  else if (elemIn.IsType (kTypeFloat))        {SetFloat (elemIn.GetFloat ());}
  else if (elemIn.IsType (kTypeDouble))       {SetDouble (elemIn.GetDouble ());}
  else if (elemIn.IsType (kTypeVec))          {SetVec (elemIn.GetVec ());}
  else if (elemIn.IsType (kTypeBool))         {SetBool (elemIn.GetBool ());}
  else if (elemIn.IsType (kTypeString))       {SetString (elemIn.GetString ());}
  else if (elemIn.IsType (kTypeBlob))         {RStr  strBlob; elemIn.GetBlob (strBlob); SetBlob (&strBlob);}
  else if (elemIn.IsType (kTypeIntArray))     {SetIntArray (*elemIn.GetIntArray ());}
  else if (elemIn.IsType (kTypeFloatArray))   {SetFloatArray (*elemIn.GetFloatArray ());}
  else if (elemIn.IsType (kTypeDoubleArray))  {SetDoubleArray (*elemIn.GetDoubleArray ());}
  else if (elemIn.IsType (kTypeStringArray))  {SetStringArray (*elemIn.GetStringArray ());}
  else if (elemIn.IsType (kTypePtr))          {SetPtr (elemIn.GetPtr ());}
  else if (elemIn.IsType (kTypeLink))         {SetLink (elemIn.GetLink ());}
  };


//-----------------------------------------------------------------------------
//  ValueRegistry
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
VOID  ValueRegistry::SetFallback  (ValueRegistry *  pRegIn)
  {
  if (this != pRegIn)
    {
    pFallback = pRegIn;
    };
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistry::MarkForSave (const char *  szNameIn,
                                  BOOL          bSaveStatusIn)
  {
  ValueElem *  pElem = Find (szNameIn);
  if (pElem != NULL)
    {
    pElem->MarkForSave (bSaveStatusIn);
    };
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistry::MarkForSave (HASH_T        uNameHashIn,
                                  BOOL          bSaveStatusIn)
  {
  ValueElem *  pElem = Find (uNameHashIn);
  if (pElem != NULL)
    {
    pElem->MarkForSave (bSaveStatusIn);
    };
  };

//-----------------------------------------------------------------------------
VOID  ValueRegistry::Overlay  (ValueRegistry *  pregToOverlay,
                               INT              iOrderIn)
  {
    // TODO:
    // step through incoming registry elems

    // NOTE:  This iteration by index isn't going to be the fastest.
    INT iNumElem = pregToOverlay->Size ();
    for (INT  iIndex = 0; iIndex < iNumElem; ++iIndex)
      {
      ValueElem *  pElem         = pregToOverlay->FindByIndex (iIndex);
      ValueElem *  pElemExisting = Find (pElem->GetNameHash());

      if (pElemExisting != NULL)
        {
        //DBG_INFO ("Merging variable %s into base\n", pElemExisting->GetName());

        if (pElem->IsType (pElemExisting->GetType ()))
          {
          // if type is same, and order is higher, set the new value and order
          if (iOrderIn > pElemExisting->GetOrder ())
            {
            pElemExisting->SetByType (*pElem);
            pElemExisting->SetOrder (pElem->GetOrder ());
            };
          }
        else
          {
          // if type is different, lowest ordered elem determines type
          if (pElem->GetOrder () < pElemExisting->GetOrder ())
            {
            // NOTE: This behavior of having the lowest ordered element setting
            //  the type is needed in case a higher ordered element is solved first
            //  (because it was not expression driven), but it used a predicted type
            //  that was incorrect.  This should hopefully be an edge case, rather
            //  than the norm, and only really a problem on initial solve either at
            //  startup or because the dev/prod environment changed.
            //
            //  To change type we need to delete and recreate the element, and this
            //  will break all OnChange/OnDelete connections.  This should be fine,
            //  as long as the element hookups are coded so they auto-connect as needed.

            // Zero out hash so Find() will ignore it, and a new elem of the same name can be created.
            pElemExisting->ZeroName ();
            ValueElem *  pElemNew = SetClone (pElem);

            // the new item is at the end of the list.  Swap it with the existing
            //  to-be-deleted element so that the indexing isn't messed up.

            SwapIndexes (*pElemExisting, *pElemNew);
            DeleteElem (pElemExisting);
            }
          }
        }
      else
        {
        //DBG_INFO ("SetClone");
        SetClone (pElem);
        };
      };

    // preserve name lookups

  };


//------------------------------------------------------------------------------
const char *  ValueRegistry::ExpandVars (RStrParser &  parserIn) const
  {
  // scan for environment-style variables, and expand as you find them.
  // An environment variable name starts with a dollar sign ($), followed by
  //  a string of alpha_numeric and/or underscore characters.  This string may
  //  optionally be enclosed in either curly brackets {} or parenthesis ()

  // Note that if a match cannot be found, the env_var will be replaced
  //  with an empty string.

  INT32  iSavedCursorPos = parserIn.GetCursorStart ();
  RStr  strKey;
  RStr  strValue;
  RStr  strFullTag;
  while (parserIn.GetNextEnvVar (strKey, &strFullTag))
    {
    strValue.Set (GetString (strKey.AsChar ()));

    if (strValue.IsEmpty ())
      {
      // If we don't find the key, keep the tag in.  This will allow multi-pass replacements.
      parserIn.InsertString (strFullTag, parserIn.GetCursorStart ());
      // increment the cursor so we don't keep finding the same tag.
      parserIn.SkipChars (1);
      }
    else
      {
      parserIn.InsertString (strValue, parserIn.GetCursorStart ());
      };
    strFullTag.Empty();
    };

  parserIn.SetCursorStart (iSavedCursorPos);
  return (parserIn.AsChar ());
  };


