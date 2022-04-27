/* -----------------------------------------------------------------
                             URL Builder

     This module implements a class for manipulating the different
     elements of a URI.

   ----------------------------------------------------------------- */

// Authors:  Michael T. Duffy  (mduffor@gmail.com)

// Modified BSD License:
//
// Copyright (c) 2004-2014, Michael T. Duffy II.  All rights reserved.
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

#include "Net/URLBuilder.hpp"

#define PREFIX_TERMINATOR ':'
#define SERVER_PREFIX     "://"
#define SERVER_POSTFIX    '/'
#define PORT_PREFIX       ':'

//------------------------------------------------------------------------------
URLBuilder::URLBuilder      ()
  {
  Init ();
  };

//------------------------------------------------------------------------------
URLBuilder::URLBuilder (const char *  szURLIn)
  {
  Init ();
  SetFullURL (szURLIn);
  };

//------------------------------------------------------------------------------
URLBuilder::~URLBuilder     ()
  {
  };

//------------------------------------------------------------------------------
VOID URLBuilder::Init (VOID)
  {
  strPrefix.Reset ();
  strServer.Reset ();
  iPort = -1;
  strPath.Reset ();

  parserFull.Reset ();
  };

//------------------------------------------------------------------------------
URLBuilder &  URLBuilder::operator =  (const URLBuilder &  urlIn)
  {
  strPrefix  = urlIn.strPrefix;
  strServer  = urlIn.strServer;
  iPort      = urlIn.iPort;
  strPath    = urlIn.strPath;
  parserFull = urlIn.parserFull;
  return (*this);
  };

//------------------------------------------------------------------------------
const char *  URLBuilder::GetFullURL  (VOID)
  {
  return (parserFull.AsChar ());
  };

//------------------------------------------------------------------------------
const char *  URLBuilder::GetPrefix  (VOID)
  {
  return (strPrefix.AsChar ());
  };

//------------------------------------------------------------------------------
const char *  URLBuilder::GetServer  (VOID)
  {
  return (strServer.AsChar ());
  };

//------------------------------------------------------------------------------
INT           URLBuilder::GetPort   (VOID)
  {
  return (iPort);
  };

//------------------------------------------------------------------------------
const char *  URLBuilder::GetPath   (VOID)
  {
  return (strPath.AsChar ());
  };

//------------------------------------------------------------------------------
VOID          URLBuilder::SetFullURL  (const char *  szIn)
  {
  parserFull = szIn;

  // parse out the individual elements
  //DBG_INFO ("Parsing out path %s", szIn);

  // prefix
  INT  iPrefixEnd = parserFull.FindChar (PREFIX_TERMINATOR, parserFull.GetCursorStart ());
  parserFull.ReadChars  (strPrefix, iPrefixEnd);
  //DBG_INFO ("Prefix %s", strPrefix.AsChar ());

  // separator ://
  INT  iServerPrefixLength = strlen (SERVER_PREFIX);
  if (parserFull.Compare (SERVER_PREFIX, iServerPrefixLength, iPrefixEnd) != 0) return;
  parserFull.SkipChars (iServerPrefixLength);

  // server
  INT  iServerEnd = parserFull.FindChar (SERVER_POSTFIX, parserFull.GetCursorStart ());
  if (iServerEnd == -1)
    {
    iServerEnd = parserFull.Length ();
    }
  INT  iServerLength = iServerEnd - parserFull.GetCursorStart ();

  parserFull.ReadChars (strServer, iServerLength);
  //DBG_INFO ("Server %s", strServer.AsChar ());

  // port
  INT  iPortPrefix = strServer.FindChar (PORT_PREFIX);
  //DBG_INFO ("Port Offset %d", iPortPrefix);
  if (iPortPrefix == -1)
    {
    iPort = -1;
    }
  else
    {
    RStrParser  parserServer (strServer);
    parserServer.SetCursorStart (iPortPrefix + 1);
    iPort = parserServer.GetInt ();
    strServer.TruncateRight (iPortPrefix - 1);
    };
  //DBG_INFO ("Port %i", iPort);

  // path
  strPath.Empty ();
  parserFull.GetRight (parserFull.Length () - iServerEnd, strPath);
  //DBG_INFO ("Path %s", strPath.AsChar ());
  };

//------------------------------------------------------------------------------
VOID  URLBuilder::SetPrefix   (const char *  szIn)
  {
  strPrefix = szIn;
  BuildFullURL ();
  };

//------------------------------------------------------------------------------
VOID  URLBuilder::SetServer  (const char *  szIn)
  {
  strServer = szIn;
  BuildFullURL ();
  };

//------------------------------------------------------------------------------
VOID  URLBuilder::SetPort  (int           iIn)
  {
  iPort = iIn;
  BuildFullURL ();
  };

//------------------------------------------------------------------------------
VOID  URLBuilder::SetPath  (const char *  szIn)
  {
  strPath = szIn;
  BuildFullURL ();
  };

//------------------------------------------------------------------------------
VOID  URLBuilder::ClearParams  (VOID)
  {
  kvpParams.Clear ();
  BuildFullURL ();
  };

//------------------------------------------------------------------------------
VOID  URLBuilder::SetParam  (const char *  szKeyIn,
                             const char *  szValueIn)
  {
  kvpParams.SetAt (szKeyIn, szValueIn);
  BuildFullURL ();
  };

//------------------------------------------------------------------------------
VOID  URLBuilder::BuildFullURL  (VOID)
  {
  parserFull.Empty ();

  parserFull.AppendString (strPrefix);
  parserFull.AppendString (SERVER_PREFIX);
  parserFull.AppendString (strServer);

  //DBG_INFO ("Prefix: %s %d", strPrefix.AsChar (), strPrefix.Length ());
  //DBG_INFO ("Server Prefix: %s", SERVER_PREFIX);
  //DBG_INFO ("Server: %s %d", strServer.AsChar (), strServer.Length ());

  if (iPort >= 0)
    {
    parserFull.AppendChar (PORT_PREFIX);
    parserFull.AppendFormat ("%d", iPort);
    }
  parserFull.AppendString (strPath);

  INT  iNumParams = kvpParams.Length ();
  if (iNumParams > 0)
    {
    parserFull.AppendChar ('?');

    // NOTE:  Still need to handle percent encoding to get rid of spaces and other characters that URLs can't handle.

    // NOTE: You may want to move this string building into KVPArray itself.
    for (INT  iIndex = 0; iIndex < iNumParams; ++iIndex)
      {
      parserFull.AppendString (kvpParams.KeyAtIndex (iIndex));
      parserFull.AppendChar ('=');
      parserFull.AppendString (kvpParams.ValueAtIndex (iIndex));
      if (iIndex + 1 < iNumParams)
        {
        parserFull.AppendChar ('&');
        };
      };
    };
  //DBG_INFO ("BuildFullURL: %s %d", parserFull.AsChar (), parserFull.Length ());
  };
