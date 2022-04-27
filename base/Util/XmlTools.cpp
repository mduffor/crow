/* -----------------------------------------------------------------
                              XML Tools

     This module contains tools and utilities for parsing files
   in the XML format.

   ----------------------------------------------------------------- */

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
ASSERTFILE (__FILE__)

#include "XmlTools.hpp"


//-----------------------------------------------------------------------------
XmlTools::XmlTools ()
  {
  Reset ();
  };


//-----------------------------------------------------------------------------
VOID  XmlTools::Reset  (VOID)
  {
  strTagName.Empty ();
  arrayAttributes.Clear ();
  iTagEnd = 0;
  iDataStart = 0;
  iDataEnd = 0;
  pparserCachedBuffer = NULL;
  };

//-----------------------------------------------------------------------------
INT  XmlTools::GetAttrInt  (const char *  szKeyIn,
                            INT           iDefaultIn)
  {
  RStrParser  parserValue = arrayAttributes[szKeyIn];
  if (parserValue.IsEmpty ()) return (iDefaultIn);
  return (parserValue.GetInt ());
  };

//-----------------------------------------------------------------------------
FLOAT  XmlTools::GetAttrFloat  (const char *  szKeyIn,
                                FLOAT         fDefaultIn)
  {
  RStrParser  parserValue = arrayAttributes[szKeyIn];
  if (parserValue.IsEmpty ()) return (fDefaultIn);
  return (parserValue.GetFloat ());
  };

//-----------------------------------------------------------------------------
BOOL  XmlTools::GetBody  (RStrParser &  parserOut)
  {
  // NOTE:  This would be more efficient if you crate a sub-string of the middle, rather
  //         than a copy.

  parserOut.Empty ();
  if (HasBody ())
    {
    pparserCachedBuffer->GetMiddle (UINT32 (iDataStart),
                                    UINT32  (iDataEnd - iDataStart + 1),
                                    parserOut);
    parserOut.ResetCursor ();
    return (TRUE);
    }
  return (FALSE);
  };

//-----------------------------------------------------------------------------
EStatus  XmlTools::FindTag (RStrParser &  parserBufferIn)
  {
  INT32  iCloseBracket;
  RStrParser   parserCurrTag;

  Reset ();
  pparserCachedBuffer = &parserBufferIn;

  //DBG_INFO ("FindTag Buffer offset %d", parserBufferIn.GetCursorStart ());
  //DBG_INFO ("Buffer:\n%s", parserBufferIn.GetCursorStartPtr ());

  parserBufferIn.SkipWhitespace();
  if (parserBufferIn.IsEOF ()) return (EStatus::kSuccess);

  // scan for the open tag
  while (! parserBufferIn.IsEOF ())
    {
    INT32  iOpenBracket = parserBufferIn.FindChar ('<', parserBufferIn.GetCursorStart ());
    if (iOpenBracket != -1)
      {
      //DBG_INFO ("Open Bracket %d : %s", iOpenBracket, parserBufferIn.GetBufferPtr (iOpenBracket));
      parserBufferIn.SetCursorStart (iOpenBracket + 1);

      // skip xml comments
      if (parserBufferIn.Compare ("!--", 3, iOpenBracket + 1) == 0)
        {
        // XML comment
        if ((iCloseBracket = parserBufferIn.Find (RStr ("-->"), iOpenBracket + 4)) == -1)
          {
          // comment never closed
          return (EStatus::Failure ("Comment missing closing tag."));
          };
        parserBufferIn.SetCursorStart (iCloseBracket + 3);
        }

      // skip xml declarations
      else if (parserBufferIn.Compare ("?", 1, iOpenBracket + 1) == 0)
        {
        if ((iCloseBracket = parserBufferIn.Find (RStr ("?>"), iOpenBracket + 2)) == -1)
          {
          // declarations never closed
          return (EStatus::Failure ("Declaration missing closing tag."));
          };
        parserBufferIn.SetCursorStart (iCloseBracket + 2);
        }

      // extract the tag name
      else
        {
        strTagName = parserBufferIn.GetAlphaNum ();
       // DBG_INFO ("Extracted Tag %s", strTagName.AsChar ());
        break;
        };
      }
    else
      {
      // unable to find opening bracket
      return (EStatus::Failure ("Unable to find opening bracket."));
      };
    };

  if (parserBufferIn.IsEOF ())
    {
    return (EStatus::Failure ("Unable to find tag name."));
    };

  // scan for the end of the open tag, getting the attr text bounds as you go.
  parserBufferIn.SkipWhitespace ();

  INT32  iAttrStart = parserBufferIn.GetCursorStart ();
  INT32  iAttrEnd   = parserBufferIn.FindChar ('>', parserBufferIn.GetCursorStart ());

  if (iAttrEnd == -1)
    {
    return (EStatus::Failure ("Unable to find closing bracket of tag" + strTagName));
    };
  parserBufferIn.SetCursorStart (iAttrEnd + 1);

  iDataStart = parserBufferIn.GetCursorStart ();
  // start with a zero sized buffer
  iDataEnd = iDataStart - 1;

  // check to see if this is a self-contained tag.
  --iAttrEnd;

  if (parserBufferIn.PeekChar (iAttrEnd) == '/')
    {
    // self contained tag.
    //DBG_INFO ("Self Contained Tag");
    --iAttrEnd;
    }
  else
    {
    // scan for the tag close, tracking tag opening and closing as you go.

    //DBG_INFO ("");
    //DBG_INFO ("Searching for closing tag of %s", strTagName.AsChar ());

    INT32  iLevels = 0;
    while (! parserBufferIn.IsEOF ())
      {
      iDataEnd      = parserBufferIn.GetCursorStart () - 1;
      INT32  iNextChar = parserBufferIn.GetChar ();

      if (iNextChar == '<')
        {

        // open bracket.
        // skip xml comments
        if (parserBufferIn.Compare ("!--", 3, parserBufferIn.GetCursorStart ()) == 0)
          {
          // XML comment
          if ((iCloseBracket = parserBufferIn.Find (RStr ("-->"), parserBufferIn.GetCursorStart () + 3)) == -1)
            {
            // comment never closed
            return (EStatus::Failure ("Comment missing closing tag."));
            };
          parserBufferIn.SetCursorStart (iCloseBracket + 3);
          }

        // skip xml declarations
        else if (parserBufferIn.Compare ("?", 1, parserBufferIn.GetCursorStart ()) == 0)
          {
          if ((iCloseBracket = parserBufferIn.Find (RStr ("?>"), parserBufferIn.GetCursorStart () + 2)) == -1)
            {
            // declarations never closed
            return (EStatus::Failure ("Declaration missing closing tag."));
            };
          parserBufferIn.SetCursorStart (iCloseBracket + 2);
          }

        // skip the tag name
        else
          {
          if (parserBufferIn.PeekChar () == '/')
            {
            // closing tag
            parserBufferIn.SkipChars (1);
            --iLevels;

            // read the tag in order to skip it
            parserCurrTag = parserBufferIn.GetAlphaNum ();

            //DBG_INFO ("Considering closing tag %s == %s level %d", parserCurrTag.AsChar (), strTagName.AsChar (), iLevels);

            if ((iLevels == -1) && (parserCurrTag == strTagName))
              {
              // found the closing tag
              // skip ending bracket

              parserBufferIn.SkipWhitespace ();
              parserBufferIn.SkipChars (1);
              break;
              };
            }
          else
            {
            // opening tag
            RStrParser  parserOpeningTag = parserBufferIn.GetAlphaNum ();

            ++iLevels;
            };

          if ((iCloseBracket = parserBufferIn.FindChar ('>', parserBufferIn.GetCursorStart ())) == -1)
            {
            // tag never closed
            return (EStatus::Failure ("Missing closing tag."));
            }
          else
            {
            if (parserBufferIn.PeekChar (iCloseBracket - 1) == '/')
              {
              // self-contained tag.  Undo the above level increment.
              --iLevels;
              }
            };

          parserBufferIn.SetCursorStart (iCloseBracket + 1);
          };
        };
      };

    if ((parserBufferIn.IsEOF ()) && (iLevels != -1))
      {
      strDbgBuffer.Format ("Missing closing tag for %s", strTagName.AsChar ());
      return (EStatus::Failure (strDbgBuffer.AsChar ()));
      };
    };


  //DBG_INFO ("Tag %s", strTagName.AsChar ());

  //RStrParser  parserBody;
  //if (GetBody (parserBody))
  //  {
  //  DBG_INFO ("Body:\n%s", parserBody.AsChar ());
  //  };



  // convert attribute string into an attr list.
  return (ReadAttributes (parserBufferIn, iAttrStart, iAttrEnd, arrayAttributes));
  };


//-----------------------------------------------------------------------------
EStatus  XmlTools::ReadAttributes (RStrParser &  parserBufferIn,
                                   INT32         iAttrStart,
                                   INT32         iAttrEnd,
                                   KVPArray &    arrayAttributesOut)
  {
  INT32  iSavedBufferPos = parserBufferIn.GetCursorStart ();

  parserBufferIn.SetCursorStart (iAttrStart);
  parserBufferIn.SkipWhitespace ();

  while (parserBufferIn.GetCursorStart () < iAttrEnd)
    {
    RStr  strKey = parserBufferIn.GetAlphaNum ();
    if (parserBufferIn.GetChar () != '=')
      {
      parserBufferIn.SetCursorStart (iSavedBufferPos);
      return (EStatus::Failure ("Attribute " + strKey + "is not followed by an equals sign."));
      };
    RStr  strValue;
    parserBufferIn.GetQuoteString (&strValue);

    arrayAttributesOut.Append (strKey.AsChar(), strValue.AsChar());
    };

  // restore the cursor pos before returning.
  parserBufferIn.SetCursorStart (iSavedBufferPos);

  return (EStatus::kSuccess);
  };


//-----------------------------------------------------------------------------
VOID  XmlTools::GetAttrAtIndex (INT     iIndexIn,
                                RStr &  strNameOut,
                                RStr &  strValueOut)
  {
  strNameOut  = arrayAttributes.KeyAtIndex   (iIndexIn);
  strValueOut = arrayAttributes.ValueAtIndex (iIndexIn);
  };














