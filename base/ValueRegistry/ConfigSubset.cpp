/* -----------------------------------------------------------------
                            Config Subset

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

#include "ValueRegistry/ValueRegistry.hpp"
#include "ValueRegistry/ConfigSubset.hpp"
#include "Script/Expression.hpp"
#include "Util/ParseTools.hpp"



/*
[
  {
  "name" : "config_name",
  "enabled" : "expression",
  "prereq" : "expression",
  "variables" : {
    "type:Var.One" : "string with 'embedded quotes'.",
    "int:Var.Two" : 2
  },
  "segments" : [
    {
      "name" : "SegA",
      "prereq" : "expression",
      "variables" : {
        "int:Var.Two" : 3
      }
    },
    {
      "name" : "SegB",
      "prereq" : "expression",
      "variables" : {
        "int:Var.Two" : 4
      }
    },
  ]
  }
]

*/


// TODO:  Parse nested subsets, which need to be pulled apart at the ConfigLayer parsing level.

//-----------------------------------------------------------------------------
EStatus  ConfigSubset::ParseSubsetList  (RStrParser &            parserIn,
                                         const char *            szPrefixIn,
                                         TList<ConfigSubset*> &  listOut)
  {
  parserIn.SkipWhitespace ();

  // check if this is an array of layers or a single layer.
  if (parserIn.PeekChar () == '[')
    {
    if (! ParseTools::SkipChar (parserIn, '['))  {return (EStatus::Failure("ParseSubsetList missing opening array bracket at line %d.", parserIn.GetLineNumber()));};

    // step through  list
    while (parserIn.PeekChar () != ']')
      {
      ConfigSubset *  pSubset = listOut.PushBack (new ConfigSubset ());

      EStatus  status;
      if ((status = pSubset->ParseSubset (parserIn, szPrefixIn)) == EStatus::kFailure)
        {
        return (status);
        };

      // check for next
      if (! ParseTools::SkipChar (parserIn, ','))  {break;};

      }; // while not end of list

    if (! ParseTools::SkipChar (parserIn, ']'))  {return (EStatus::Failure("ParseSubsetList missing closing array bracket at line %d.", parserIn.GetLineNumber()));};

    } else {
      ConfigSubset *  pSubset = listOut.PushBack (new ConfigSubset ());
      return (pSubset->ParseSubset (parserIn, szPrefixIn));
    };

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
EStatus  ConfigSubset::ParseSubset (RStrParser &   parserIn,
                                    const char *   szPrefixIn)
  {
  RStr     strKey;

  strPrefix.Set (szPrefixIn);

  parserIn.SkipWhitespace ();

  if (parserIn.GetChar () == '{')
    {
    while (parserIn.PeekChar () != '}')
      {
      EStatus  status = EStatus::kSuccess;

      // get key
      if ((status = ParseTools::GetKey (parserIn, ':', strKey)) == EStatus::kFailure)
        {
        return (status);
        };

      if (strKey.Equals (H("name",0)))
        {
        parserIn.GetQuoteString (&strName);
        strName.CalcHash();
        }
      else if (strKey.Equals (H("enabled",0)))
        {
        parserIn.GetQuoteString (&strEnabled);
        }
      else if (strKey.Equals (H("prereq",0)))
        {
        parserIn.GetQuoteString (&strPreReq);
        }
      else if (strKey.Equals (H("prefix",0)))
        {
        parserIn.GetQuoteString (&strPrefix);
        }
      else if (strKey.Equals (H("order",0)))
        {
        iOrder = parserIn.GetInt ();
        }
      else if (strKey.Equals (H("variables",0)))
        {
        status = ParseVariables (parserIn);
        }
      else if (strKey.Equals (H("segments",0)))
        {
        // this is a config layer top level subset.
        status = ParseSubsetList (parserIn, strPrefix.AsChar (), listSegments);
        }
      else
        {
        // unknown key
        RStr  strOut;
        strOut.Format ("Unknown key %s at line %d.", strKey.AsChar (), parserIn.GetLineNumber());
        DBG_ERROR (strOut.AsChar ());
        status = EStatus::Failure(strOut.AsChar());
        };
      if (status.IsFailure ())
        {
        DBG_ERROR ("Error parsing buffer - %s at line %d", status.GetDescription (), parserIn.GetLineNumber());
        return (status);
        };

      if (! ParseTools::SkipChar (parserIn, ','))  {break;};

      }; // while not EOF
    }; // if opening bracket

  // skip ending bracket
  ParseTools::SkipChar (parserIn, '}');

  //printf ("---- After ParseSubset, next is %s\n", parserIn.GetCursorStartPtr());

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
VOID  ConfigSubset::RemoveAll  (VOID)
  {
  // delete all values, thus also breaking their callback connections.
  regVariables.Clear();
  };

//-----------------------------------------------------------------------------
EStatus  ConfigSubset::ParseVariables  (RStrParser &  parserIn)
  {
  // parse the array of values
  /*
    "variables" : {
    "type:Var.One" : "string with 'embedded quotes'.",
    "int:Var.Two" : 2
  },
  */
  RStr     strType;
  RStr     strKey;

  parserIn.SkipWhitespace ();

  if (! ParseTools::SkipChar (parserIn, '{'))  {return (EStatus::Failure("ParseVariables missing opening bracket at line %d.", parserIn.GetLineNumber()));};

  // step through list
  while (parserIn.PeekChar () != '}')
    {
    // read parameters
    if (ParseTools::GetKey (parserIn, ':', strKey) == EStatus::kFailure)
      {
      return (EStatus::Failure ("ParseVariables unable to read dictionary key at line %d", parserIn.GetLineNumber()));
      };

    INT  iSeparator = strKey.FindChar (':');
    if (iSeparator != -1)
      {
      // read the type from the key
      strKey.SubString (0, iSeparator, strType);
      strKey.ChompLeft (iSeparator + 1);

      strType.StripTrailingWhitespace ();
      strKey.StripLeadingWhitespace ();
      }
    else
      {
      // try to deduce the parameter type from the value
      strType.Set (GuessValueType (parserIn));
      };

    //DBG_INFO ("ParseVariables found key %s, which is believed to be of type %s", strKey.AsChar(), strType.AsChar ());

    if (!strType.IsEmpty())
      {
      if (strType.Equals("int"))
        {
        regVariables.SetInt (strPrefix + strKey, parserIn.GetInt ());
        }
      else if (strType.Equals("string"))
        {
        RStr  strOut;
        regVariables.SetString (strPrefix + strKey, parserIn.GetQuoteString (&strOut));
        }
      else if (strType.Equals("bool"))
        {
        RStr  strOut;
        parserIn.GetWord (&strOut, TRUE, "[]{},");
        BOOL  bValue = FALSE;

        if (strOut.Equals ("true")) {bValue = TRUE;};
        regVariables.SetBool (strPrefix + strKey, bValue);

        //regVariables.SetString (strPrefix + strKey, parserIn.GetQuoteString (&strOut));
        }
      else if (strType.Equals("float"))
        {
        regVariables.SetFloat (strPrefix + strKey, parserIn.GetFloat ());
        }
      else if (strType.Equals("double"))
        {
        regVariables.SetDouble (strPrefix + strKey, parserIn.GetDouble ());
        }

      else if (strType.Equals("intarray"))
        {
        IntArray    arrayOut;
        ParseTools::GetIntArray (parserIn, '[', ']', arrayOut);
        regVariables.SetIntArray (strPrefix + strKey, arrayOut);
        }
      else if (strType.Equals("floatarray"))
        {
        FloatArray    arrayOut;
        ParseTools::GetFloatArray (parserIn, '[', ']', arrayOut);
        regVariables.SetFloatArray (strPrefix + strKey, arrayOut);
        }
      else if (strType.Equals("doublearray"))
        {
        DoubleArray    arrayOut;
        ParseTools::GetDoubleArray (parserIn, '[', ']', arrayOut);
        regVariables.SetDoubleArray (strPrefix + strKey, arrayOut);
        }
      else if (strType.Equals("stringarray"))
        {
        RStrArray    arrayOut;
        ParseTools::GetStringArray (parserIn, '[', ']', arrayOut);
        regVariables.SetStringArray (strPrefix + strKey, arrayOut);
        }
      else if (strType.Equals("stringset"))
        {
        RStrArray    arrayOut;
        ParseTools::GetStringArray (parserIn, '[', ']', arrayOut);
        ValueElem *  pElem = regVariables.SetStringArray (strPrefix + strKey, arrayOut);
        if (pElem != NULL)
          {
          pElem->MakeUniqueSet (TRUE);
          };
        }
      else
        {
        return (EStatus::Failure("ParseVariables unknown variable type \"%s\" at line %d", strType.AsChar (), parserIn.GetLineNumber()));
        };
      };

    // check for next
    if (! ParseTools::SkipChar (parserIn, ','))  {break;};
    }; // while not end of list

  if (! ParseTools::SkipChar (parserIn, '}'))  {return (EStatus::Failure("ParseVariables missing closing bracket at line %d", parserIn.GetLineNumber()));};

  return (EStatus::kSuccess);
  };

//-----------------------------------------------------------------------------
const char *  ConfigSubset::GuessValueType (RStrParser &  parserIn)
  {
  const char *  szType = "";

  if (parserIn.PeekChar() == '"')
    {
    szType = "string";
    }
  // NOTE: Probably need to add a case-insensitive search here.
  else if (parserIn.StartsWith("true") || parserIn.StartsWith("false"))
    {
    szType = "bool";
    }
  else if (parserIn.IsFloat (FALSE, TRUE))
    {
    szType = "float";
    }
  else if (parserIn.IsInt (FALSE))
    {
    szType = "int";
    }
  else if (parserIn.PeekChar() == '[')
    {
    // array
    INT  iSavedPos = parserIn.GetCursorStart ();
    parserIn.SkipChars(1);
    parserIn.SkipWhitespace();
    if (parserIn.PeekChar() == '"')
      {
      szType = "stringarray";
      }
    else if (parserIn.IsFloat (FALSE, TRUE))
      {
      szType = "floatarray";
      }
    else if (parserIn.IsInt (FALSE))
      {
      szType = "intarray";
      }
    parserIn.SetCursorStart (iSavedPos);
    }

  return (szType);
  };

//-----------------------------------------------------------------------------
VOID  ConfigSubset::ToJSON  (RStrParser &  parserOut,
                             const char *  szSectionIndentIn, // "    "
                             const char *  szLineIndentIn,    // "  "
                             const char *  szLineEndIn)       // "\n"
  {
  RStr    strSecLineIndent     (szSectionIndentIn);
  RStr    strSecLineLineIndent (szSectionIndentIn);

  strSecLineIndent     += szLineIndentIn;
  strSecLineLineIndent += szLineIndentIn;
  strSecLineLineIndent += szLineIndentIn;

  static const char *   KEY_NAME    = "name";
  static const char *   KEY_PREREQ  = "prereq";
  static const char *   KEY_ENABLED = "enabled";
  static const char *   KEY_VAR     = "variables";
  static const char *   KEY_SEG     = "segments";

  parserOut.AppendFormat ("%s{%s", szSectionIndentIn,
                                   szLineEndIn);

  parserOut.AppendFormat ("%s\"%s\":\"%s\"%s", strSecLineIndent.AsChar(),
                                               KEY_NAME,
                                               strName.AsChar(),
                                               szLineEndIn);

  if (!strEnabled.IsEmpty())
    {
    parserOut.AppendFormat ("%s\"%s\":\"%s\"%s", strSecLineIndent.AsChar(),
                                          KEY_ENABLED,
                                          strEnabled.AsChar(),
                                          szLineEndIn);
    };

  if (!strPreReq.IsEmpty())
    {
    parserOut.AppendFormat ("%s\"%s\":\"%s\"%s", strSecLineIndent.AsChar(),
                                          KEY_PREREQ,
                                          strPreReq.AsChar(),
                                          szLineEndIn);
    };

  parserOut.AppendFormat ("%s\"%s\":{%s", strSecLineIndent.AsChar(),
                                    KEY_VAR,
                                    szLineEndIn);

  // step through variables (NOTE: Could be more efficient, if needed)
  INT  iNumElem = regVariables.Size ();
  for (INT  iIndex = 0; iIndex < iNumElem; ++iIndex)
    {
    ValueElem *  pElem = regVariables.FindByIndex (iIndex);
    if (pElem != NULL)
      {
      const char *  szTypeName = ElemToTypePrefix (*pElem);
      //const char *  szName = ""; // TODO: Map hash to name lookup, or convert Hash to string.

      if (pElem->IsType (ValueElem::kTypeInt)         ||
          pElem->IsType (ValueElem::kTypeFloat)       ||
          pElem->IsType (ValueElem::kTypeDouble)      ||
          pElem->IsType (ValueElem::kTypeIntArray)    ||
          pElem->IsType (ValueElem::kTypeFloatArray)  ||
          pElem->IsType (ValueElem::kTypeDoubleArray) ||
          pElem->IsType (ValueElem::kTypeStringArray))
        {
        parserOut.AppendFormat ("%s\"%s:%s\":%s", strSecLineLineIndent.AsChar(),
                                                  szTypeName,
                                                  pElem->GetName(),
                                                  pElem->GetString());
        }
      else if (pElem->IsType (ValueElem::kTypeBool))
        {
        parserOut.AppendFormat ("%s\"%s:%s\":\"%s\"", strSecLineLineIndent.AsChar(),
                                                      szTypeName,
                                                      pElem->GetName(),
                                                      pElem->GetBool() ? "true" : "false");

        }
      else if (pElem->IsType (ValueElem::kTypeString))
        {
        parserOut.AppendFormat ("%s\"%s:%s\":\"%s\"", strSecLineLineIndent.AsChar(),
                                                      szTypeName,
                                                      pElem->GetName(),
                                                      pElem->GetString());
        };
      };
    if (iIndex != (iNumElem - 1))
      {
      parserOut += ",";
      }
    parserOut += szLineEndIn;
    }; // for each elem

  if (listSegments.Size () > 0)
    {
    parserOut.AppendFormat ("%s\"%s\":[%s", strSecLineIndent.AsChar(),
                                            KEY_SEG,
                                            szLineEndIn);

    for (TListItr<ConfigSubset*>  itrCurr = listSegments.First ();
        itrCurr.IsValid ();
        ++itrCurr)
      {
      // TODO:  Probabaly increase these.   Figure that out in testing.
      (*itrCurr)->ToJSON (parserOut, szSectionIndentIn, szLineIndentIn, szLineEndIn);

      TListItr<ConfigSubset*>  itrNext = itrCurr;
      ++itrNext;
      if (!itrNext.IsValid ())
        {
        parserOut += ",";
        }
      parserOut += szLineEndIn;
      };

    parserOut.AppendFormat ("%s]%s", strSecLineIndent.AsChar(),
                                     szLineEndIn);
    };


  parserOut.AppendFormat ("%s}%s", strSecLineIndent.AsChar(),
                                   szLineEndIn);
  parserOut.AppendFormat ("%s}", szSectionIndentIn);
  };

//-----------------------------------------------------------------------------
const char *  ConfigSubset::ElemToTypePrefix  (ValueElem &  elemIn)
  {
  if (elemIn.IsType (ValueElem::kTypeInt))          return ("int");
  if (elemIn.IsType (ValueElem::kTypeFloat))        return ("float");
  if (elemIn.IsType (ValueElem::kTypeDouble))       return ("double");
  if (elemIn.IsType (ValueElem::kTypeString))       return ("string");
  if (elemIn.IsType (ValueElem::kTypeBool))         return ("bool");
  if (elemIn.IsType (ValueElem::kTypeIntArray))     return ("intarray");
  if (elemIn.IsType (ValueElem::kTypeFloatArray))   return ("floatarray");
  if (elemIn.IsType (ValueElem::kTypeDoubleArray))  return ("doublearray");
  if (elemIn.IsType (ValueElem::kTypeStringArray))
    {
    if (elemIn.IsUniqueSet ())
      {
      return ("stringset");
      }
    else
      {
      return ("stringarray");
      };
    };

  // Not supported yet: blob, ptr, link, vec
  return ("");
  };

//-----------------------------------------------------------------------------
BOOL  ConfigSubset::IsActive  (ValueRegistry &  regIn)
  {
  if (EvaluateExpression (strPreReq, regIn))
    {
    return (EvaluateExpression (strEnabled, regIn));
    }
  return (FALSE);
  };

//-----------------------------------------------------------------------------
BOOL  ConfigSubset::EvaluateExpression (RStr &           strExpressionIn,
                                        ValueRegistry &  regIn)
  {
  // An expression is true if it is blank, it contains the word "true", or
  //   it evaluates to true as an expression.

  if (strPreReq.IsEmpty ()) return (TRUE);

  if (strPreReq.CompareNoCase ("true") == 0)   return (TRUE);
  if (strPreReq.CompareNoCase ("false") == 0)  return (FALSE);

  // TODO: Set the subseg name to a variable for use in seeing if an AB test is activating it.

  Token  tokResult;
  Expression::Execute (strPreReq.AsChar (), &regIn, &tokResult);

  return (tokResult.AsInt () != 0);
  };


//-----------------------------------------------------------------------------
BOOL  ConfigSubset::IsExpressionDriven  (VOID)
  {
  // If the prereq has a string, and that string isn't a bool, then we consider
  //  it to be an expression whose value can change over time.  This will affect
  //  the order of solving the stack.

  if (strPreReq.IsEmpty ())                    return (FALSE);
  if (strPreReq.CompareNoCase ("true") == 0)   return (FALSE);
  if (strPreReq.CompareNoCase ("false") == 0)  return (FALSE);
  return (TRUE);
  };

//-----------------------------------------------------------------------------
VOID  ConfigSubset::MergeInto  (ValueRegistry &  regTargetIn,
                                INT              iOrderIn)
  {
  regTargetIn.Overlay (&regVariables, iOrderIn);
  };
