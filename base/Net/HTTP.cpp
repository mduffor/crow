/* -----------------------------------------------------------------
                                 HTTP

     This module implements cross-platform routines for dealing with
     file and directory paths

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

#include "Net/HTTP.hpp"
#include "Net/Base64.hpp"


INT  HTTP::iNumCalls = 0;

// NOTE:  Some code in here is inspired by:
//  http://fm4dd.com/openssl/sslconnect.htm
//  https://www.w3.org/Library/src/HTTP.c


/* ------------------------------------------------------------ *
 * file:        sslconnect.c                                    *
 * purpose:     Example code for building a SSL connection and  *
 *              retrieving the server certificate               *
 * author:      06/12/2012 Frank4DD                             *
 *                                                              *
 * gcc -lssl -lcrypto -o sslconnect sslconnect.c                *
 * ------------------------------------------------------------ */

 /*
#include <sys/socket.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>
*/

#define DEFAULT_PORT_HTTP    80  // enable for HTTP
#define DEFAULT_PORT_HTTPS  443 // enable for HTTPS


const char *  HTTP::kMimeTypeText = "text/ISO-8859-1";
const char *  HTTP::kMimeTypeWWWForm = "application/x-www-form-urlencoded";


//------------------------------------------------------------------------------
HTTP::HTTP ()
  {
  }

//------------------------------------------------------------------------------
HTTP::~HTTP ()
  {
  }

//------------------------------------------------------------------------------
EStatus  HTTP::Connect (const URLBuilder &  urlIn)
  {
  EStatus  status;

  url = urlIn;

  if (url.GetPort () < 0)
    {
    if (strcmp (url.GetPrefix (), "http") == 0)
      {
      url.SetPort (DEFAULT_PORT_HTTP);
      }
    else
      {
      url.SetPort (DEFAULT_PORT_HTTPS);
      };
    };

  status = socket.ClientConnect (url.GetServer (), url.GetPort ());
  //if (status == EStatus::kFailure) {return status;};
  return (status);
  };

//------------------------------------------------------------------------------
EStatus  HTTP::Disconnect (VOID)
  {
  return (socket.ClientDisconnect ());
  };

//------------------------------------------------------------------------------
EStatus  HTTP::Post (const char *  szMimeType,
                     RStr *        strContent,
                     const char *  szAdditionalHeader)
  {
  // build the header
  // NOTE:  https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html
  // strContent is an RStr so we can better handle binary data.

  RStrParser    parserSend;

  parserSend.Empty ();
  BuildMessage (parserSend, "POST", szMimeType, strContent, szAdditionalHeader);

  //DBG_INFO ("Post message");
  //DBG_INFO (parserSend.AsChar ());

  ++iNumCalls;
  return (socket.Write (parserSend));
  };

//------------------------------------------------------------------------------
EStatus  HTTP::Get (RStr &      strResultOut)
  {
  RStrParser    parserSend;

  parserSend.Empty ();
  BuildMessage (parserSend, "GET");

  //DBG_INFO ("HTTP::Get");
  //DBG_INFO (parserSend.AsChar ());

  ++iNumCalls;
  EStatus  status = socket.Write (parserSend);

  if (status == EStatus::kFailure) return (status);

  // now read in the return
  // NOTE:  Pause or sleep here?

  parserReceive.Empty ();
  status = socket.Read (parserReceive);

  // TODO: Split this out to its own method
  parserReceive.ResetCursor ();

  BOOL          bSuccess;
  const char *  szCodeName = NULL;
  KVPArray      kvpAttributes;

  ParseHeader (parserReceive, bSuccess, &szCodeName, kvpAttributes);

  parserReceive.SubString (parserReceive.GetCursorStart (), parserReceive.Length () - parserReceive.GetCursorStart (), strResultOut);

  /*
  // can search for Content-Type: Content-Length: Connection: keep-alive

  INT32  iContentStart = parserReceive.Find ("\r\n\r\n");
  if (iContentStart != -1)
    {
    // HACK: skip the header
    //strResultOut = parserReceive;

    parserReceive.SubString (iContentStart + 4, parserReceive.Length () - iContentStart - 4, strResultOut);
    }
  */

  //DBG_INFO ("HTTP:Get Returned");
  //DBG_INFO (strResultOut.AsChar ());

  return (status);
  };

//------------------------------------------------------------------------------
VOID  HTTP::ParseHeader (RStrParser &    parserIn,
                         BOOL &          bSuccessOut,
                         const char * *  pszCodeNameOut,
                         KVPArray &      kvpAttributesOut)
  {
  RStr  strStatusText;
  INT   iCode;

  parserIn.GetWord (); // skip HTTP/1.1
  iCode = parserIn.GetInt ();
  parserIn.GetLine (strStatusText);

  RStr  strKey;
  RStr  strValue;

  while (parserIn.GetLineLength () > 0)
    {
    // find a colon, but make sure it is on this line.
    INT32  iColon = parserIn.FindChar (':', parserIn.GetCursorStart ());
    if ((iColon != -1) && (iColon < parserIn.GetLineEnd ()))
      {
      parserIn.SubString (parserIn.GetCursorStart (), iColon - parserIn.GetCursorStart (), strKey);
      parserIn.SetCursorStart (iColon + 1);
      parserIn.SkipWhitespace (FALSE);
      parserIn.SubString (parserIn.GetCursorStart (), parserIn.GetLineEnd () - parserIn.GetCursorStart (), strValue);
      kvpAttributesOut.SetAt (strKey.AsChar (), strValue.AsChar ());

      DBG_INFO ("ParseHeader Key: %s", strKey.AsChar ());
      DBG_INFO ("ParseHeader Value: %s", strValue.AsChar ());
      }
    else
      {
      RStr  strLine;
      parserIn.GetLine (strLine);
      DBG_ERROR ("HTTP::ParseHeader found non-standard header line : %s", strLine.AsChar ());
      continue;
      };
    parserIn.GotoNextLine ();
    };
  parserIn.SkipEOL ();

  const char *  szCodeName = "Undefined";
  bSuccessOut = CheckErrorCode (iCode, &szCodeName);

  if (bSuccessOut)
    {
    DBG_INFO ("HTTP::ParseHeader - %s", szCodeName);
    }
  else
    {
    DBG_ERROR ("HTTP::ParseHeader - %s", szCodeName);
    }
  };


//------------------------------------------------------------------------------
BOOL  HTTP::CheckErrorCode (INT             iCodeIn,
                            const char * *  pszCodeNameOut)
  {
  INT  iClass     = iCodeIn / 100;
  *pszCodeNameOut = "Unknown";
  BOOL bSuccess   = FALSE;

  switch (iClass)
    {
    case 1: // Informational
      switch (iCodeIn)
        {
        case 100:  {*pszCodeNameOut = "100 Continue"; bSuccess = TRUE;}; break;
        case 101:  {*pszCodeNameOut = "101 Switching Protocols"; bSuccess = TRUE;}; break;
        default: break;
        };
      break;

    case 0: // fallthrough
    case 2: // Successful
      switch (iCodeIn)
        {
        case 200:  {*pszCodeNameOut = "200 OK"; bSuccess = TRUE;}; break;
        case 201:  {*pszCodeNameOut = "201 Created"; bSuccess = TRUE;}; break;
        case 202:  {*pszCodeNameOut = "202 Accepted"; bSuccess = TRUE;}; break;
        case 203:  {*pszCodeNameOut = "203 Non-Authoritative Information"; bSuccess = TRUE;}; break;
        case 204:  {*pszCodeNameOut = "204 No Content"; bSuccess = TRUE;}; break;
        case 205:  {*pszCodeNameOut = "205 Reset Content"; bSuccess = TRUE;}; break;
        case 206:  {*pszCodeNameOut = "206 Partial Content"; bSuccess = TRUE;}; break;
        default: break;
        };
      break;

    case 3: // Redirectional (we don't currently handle these, so marked as error)
      switch (iCodeIn)
        {
        case 300:  {*pszCodeNameOut = "300 Multiple Choices"; bSuccess = FALSE;}; break;
        case 301:  {*pszCodeNameOut = "301 Moved Permanently"; bSuccess = FALSE;}; break;
        case 302:  {*pszCodeNameOut = "302 Found"; bSuccess = FALSE;}; break;
        case 303:  {*pszCodeNameOut = "303 See Other"; bSuccess = FALSE;}; break;
        case 304:  {*pszCodeNameOut = "304 Not Modified"; bSuccess = FALSE;}; break;
        case 305:  {*pszCodeNameOut = "305 Use Proxy"; bSuccess = FALSE;}; break;
        case 307:  {*pszCodeNameOut = "307 Temporary Redirect"; bSuccess = FALSE;}; break;
        default: break;
        };
      break;

    case 4: // Client Error
      switch (iCodeIn)
        {
        case 400:  {*pszCodeNameOut = "400 Bad Request"; bSuccess = FALSE;}; break;
        case 401:  {*pszCodeNameOut = "401 Unauthorized"; bSuccess = FALSE;}; break;
        case 402:  {*pszCodeNameOut = "402 Payment Required"; bSuccess = FALSE;}; break;
        case 403:  {*pszCodeNameOut = "403 Forbidden"; bSuccess = FALSE;}; break;
        case 404:  {*pszCodeNameOut = "404 Not Found"; bSuccess = FALSE;}; break;
        case 405:  {*pszCodeNameOut = "405 Method Not Allowed"; bSuccess = FALSE;}; break;
        case 406:  {*pszCodeNameOut = "406 Not Acceptable"; bSuccess = FALSE;}; break;
        case 407:  {*pszCodeNameOut = "407 Proxy Authentication Required"; bSuccess = FALSE;}; break;
        case 408:  {*pszCodeNameOut = "408 Request Timeout"; bSuccess = FALSE;}; break;
        case 409:  {*pszCodeNameOut = "409 Conflict"; bSuccess = FALSE;}; break;
        case 410:  {*pszCodeNameOut = "410 Gone"; bSuccess = FALSE;}; break;
        case 411:  {*pszCodeNameOut = "411 Length Required"; bSuccess = FALSE;}; break;
        case 412:  {*pszCodeNameOut = "412 Precondition Failed"; bSuccess = FALSE;}; break;
        case 413:  {*pszCodeNameOut = "413 Request Entity Too Large"; bSuccess = FALSE;}; break;
        case 414:  {*pszCodeNameOut = "414 Request-URI Too Long"; bSuccess = FALSE;}; break;
        case 415:  {*pszCodeNameOut = "415 Unsupported Media Type"; bSuccess = FALSE;}; break;
        case 416:  {*pszCodeNameOut = "416 Requested Range Not Satisfiable"; bSuccess = FALSE;}; break;
        case 417:  {*pszCodeNameOut = "417 Expectation Failed"; bSuccess = FALSE;}; break;
        default: break;
        };
      break;

    case 5: // Server Error
      switch (iCodeIn)
        {
        case 500:  {*pszCodeNameOut = "500 Internal Server Error"; bSuccess = FALSE;}; break;
        case 501:  {*pszCodeNameOut = "501 Not Implemented"; bSuccess = FALSE;}; break;
        case 502:  {*pszCodeNameOut = "502 Bad Gateway"; bSuccess = FALSE;}; break;
        case 503:  {*pszCodeNameOut = "503 Service Unavailable"; bSuccess = FALSE;}; break;
        case 504:  {*pszCodeNameOut = "504 Gateway Timeout"; bSuccess = FALSE;}; break;
        case 505:  {*pszCodeNameOut = "505 HTTP Version Not Supported"; bSuccess = FALSE;}; break;
        default: break;
        };
      break;
    default: break;
    }
  return (bSuccess);
  };

//------------------------------------------------------------------------------
EStatus  HTTP::XMLHttpRequest (KVPArray &    arrayIn,
                               const char *  szAdditionalHeaderIn,
                               RStr &        strResultOut)
  {
  RStr          strAdditionalHeader ("X-Requested-With: XMLHttpRequest\r\n");
  RStrParser    parserParams;
  BOOL          bSuccess;
  const char *  szCodeName = NULL;
  KVPArray      kvpAttributes;


  KVPToURLEncoded  (arrayIn, parserParams);

  strAdditionalHeader += szAdditionalHeaderIn;
  EStatus status = Post (HTTP::kMimeTypeWWWForm, &parserParams, strAdditionalHeader.AsChar ());

  if (status == EStatus::kFailure) return (status);


  // NOTE:  I will need to set up the reading as an ASync polling routine.
  //  Store the request, with the URL and a user-provided ID.
  //  Call the poll routine regularly.  If no requests are queued, return.
  //  Read socket if data available.  Append to current read buffer.
  //  If no curr request, pull top of queue and set as active
  //  Read header, and store state
  //  Parse as much body as is present.
  //  If entire request has been fulfilled, clear it.
  //    Call associated callback (or delegate object) to process read data


  //  Read socket if data available.  Append to current read buffer.
  //if (socket.GetBytesToRead () == 0) return (EStatus::kFailure); // currently blocks.  use this for polling.
  ++iNumCalls;
  parserReceive.Empty ();
  status = socket.Read (parserReceive);

  //  If no curr request, pull top of queue and set as active
  //  Read header, and store state

  DBG_INFO ("HTTP:XMLHttpRequest Raw");
  DBG_INFO (parserReceive.AsChar ());

  parserReceive.ResetCursor ();  // This line needs to move when you fully set up async
  ParseHeader (parserReceive, bSuccess, &szCodeName, kvpAttributes);

  //DBG_INFO ("HTTP:XMLHttpRequest Parsed Attributes");
  //kvpAttributes.DebugPrint ();


  //  Parse as much body as is present.

  INT  iContentLengthIndex    = kvpAttributes.FindKey ("Content-Length");
  INT  iTransferEncodingIndex = kvpAttributes.FindKey ("Transfer-Encoding");

  if (iContentLengthIndex != -1)
    {
    //DBG_INFO ("------- Attempting content length");
    INT  iContentSize = strtol (kvpAttributes.ValueAtIndex (iContentLengthIndex).AsChar (), NULL, 10);

    parserReceive.SubString (parserReceive.GetCursorStart (), iContentSize, strResultOut);
    parserReceive.SkipChars (iContentSize);
    }
  else if (iTransferEncodingIndex != -1)
    {
    DBG_INFO ("------- Attempting transfer encoding");
    if (kvpAttributes.ValueAtIndex (iTransferEncodingIndex).Compare ("chunked") == 0)
      {
      INT  iContentSize = parserReceive.GetInt (FALSE);
      parserReceive.GotoNextLine ();

      DBG_INFO (" chunk size : %d", iContentSize);
      DBG_INFO (parserReceive.GetCursorStartPtr ());

      while (iContentSize != 0)
        {
        strResultOut.AppendChars (parserReceive.GetCursorStartPtr (), iContentSize);
        parserReceive.SkipChars (iContentSize);

        iContentSize = parserReceive.GetInt ();
        };
      };
    }
  else
    {
    //DBG_INFO ("------- Attempting raw read");
    INT  iContentSize = parserReceive.Length () - parserReceive.GetCursorStart ();
    parserReceive.SubString (parserReceive.GetCursorStart (), iContentSize, strResultOut);
    parserReceive.SkipChars (iContentSize);
    };

  //  If entire request has been fulfilled, clear it.
  //    Call associated callback (or delegate object) to process read data

  DBG_INFO ("HTTP:XMLHttpRequest Returned");
  DBG_INFO (strResultOut.AsChar ());

  return (status);
  };

//------------------------------------------------------------------------------
VOID  HTTP::BuildMessage (RStrParser &  parserIn,
                          const char *  szCommandIn,
                          const char *  szMimeTypeIn,
                          RStr *        strContentIn,
                          const char *  szAdditionalHeaderIn)
  {
  parserIn.AppendFormat ("%s %s HTTP/1.1\r\n", szCommandIn, url.GetPath ());
  if (szMimeTypeIn != NULL)
    {
    parserIn.AppendFormat ("Content-Type: %s\r\n", szMimeTypeIn);
    };
  parserIn.AppendString ("User-Agent: Crow/1.0\r\n");

  if (url.GetPort () < 0)
    {
    parserIn.AppendFormat ("Host: %s\r\n", url.GetServer ());
    }
  else
    {
    parserIn.AppendFormat ("Host: %s:%d\r\n", url.GetServer (), url.GetPort ());
    };
  parserIn.AppendString ("Accept: *\\*\r\nAccept-Charset: ISO-8859-1,utf-8;\r\nAccept-Language: en-us,en\r\n");
  // TODO: Accept-Encoding: gzip

  if (szAdditionalHeaderIn != NULL)
    {
    parserIn.AppendString (szAdditionalHeaderIn);
    }

  if ((szMimeTypeIn != NULL) && (strContentIn->Length () != 0))
    {
    parserIn.AppendFormat ("Content-Length: %d\r\n", strContentIn->Length ());
    };
  parserIn.AppendString ("Connection: close\r\n"); // NOTE: You can re-do this if you implement multi-send.
  parserIn.AppendString ("\r\n");
  if ((szMimeTypeIn != NULL) && (strContentIn->Length () != 0))
    {
    parserIn.AppendString (*strContentIn);
    };
  };


//------------------------------------------------------------------------------
VOID  HTTP::KVPToURLEncoded  (KVPArray &    arrayIn,
                              RStrParser &  parserOut)
  {

  // convert to application/x-www-form-urlencoded
  for (INT  iArrayIndex = 0; iArrayIndex < arrayIn.Length (); ++iArrayIndex)
    {
    const RStr &  strKey   = arrayIn.KeyAtIndex (iArrayIndex);
    const RStr &  strValue = arrayIn.ValueAtIndex (iArrayIndex);

    parserOut.Grow (strKey.Length () + strValue.Length () + 2);

    AppendURLEncodedString  (strKey, parserOut);
    parserOut.AppendChar ('=');
    AppendURLEncodedString  (strValue, parserOut);
    if (iArrayIndex + 1 < arrayIn.Length ())
      {
      // TODO:  This should come from the Separator field
      parserOut.AppendChar ('&');
      };
    };
  };



//------------------------------------------------------------------------------
VOID  HTTP::AppendURLEncodedString  (const RStr &  strIn,
                                     RStrParser &  parserTargetOut)
  {
  static char const   acReservedChar [] = {' ',   ';',   '/',   '?',   ':',   '@',   '&',   '=',   '+',   '$',   ',',     '\n'};
  static char const * szReservedCode [] = {"+", "%3B", "%2F", "%3F", "%3A", "%40", "%26", "%3D", "%2B", "%24", "%2C", "%0D%0A"};
  INT  iIndex;
  INT  iCodeIndex;

  for (iIndex = 0; iIndex < (INT) strIn.Length (); ++iIndex)
    {
    INT  iChar = strIn[iIndex];

    if ((iChar >= 'a' && iChar <= 'z') || (iChar >= 'A' && iChar <= 'Z') ||
        (iChar == '-'	|| iChar == '_'	|| iChar ==	'.'	|| iChar == '~'))
      {
      // literal char
      parserTargetOut.AppendChar (iChar);
      }
    else
      {
      for (iCodeIndex = 0; iCodeIndex < (INT) sizeof (acReservedChar); ++iCodeIndex)
        {
        if (iChar == acReservedChar [iCodeIndex])
          {
          parserTargetOut.AppendString (szReservedCode [iCodeIndex]);
          break;
          };
        };
      };
    };
  };

//------------------------------------------------------------------------------
RStr &  HTTP::HeaderBasicAuth (const char *  szUsername,
                               const char *  szPassword)
  {
  // NOTE: Using a static string is not thread-safe, but should be ok for our client-side purposes.
  static RStr  strAuthOut;
  RStr         strUserPass;
  RStr         strUserPassEnc;

  strUserPass = szUsername;
  strUserPass += ":";
  strUserPass += szPassword;

  Base64::Encode (strUserPass.AsChar (), strUserPass.Length (), strUserPassEnc);

  strAuthOut = "Authorization: Basic ";
  strAuthOut += strUserPassEnc;
  strAuthOut += "\r\n";

  return (strAuthOut);
  };

//------------------------------------------------------------------------------
const char *  HTTP::NumCallsStr  (VOID)
  {
  static RStr  strOut;

  strOut.Format ("%d", iNumCalls);
  return (strOut.AsChar ());
  };


/*
//------------------------------------------------------------------------------
VOID XMLParser::Encode ()
  {
  RStrParser  parserOut;

  parserOut.Empty ();

  parserOut.AppendString ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");

  // append the DOM

  }
*/

/*
GET (Supported by Internet Explorer 7 (and later), Mozilla 1+)
POST (Supported by Internet Explorer 7 (and later), Mozilla 1 (and later))
HEAD (Supported by Internet Explorer 7 (and later))
PUT
DELETE
*/
/*
sprintf(buffer,"HTTP/1.1 200 OK\nContent-Type: text/xml; charset=utf-8\nContent-Length: 0\nConnection: close\n\n");
sprintf(buffer,"HTTP/1.1 200 OK\r\nContent-Type: text/xml; charset=utf-8\r\nContent-Length: 0\r\nConnection: close\r\n\r\n");

request.setRequestHeader('X-Requested-With', 'XMLHttpRequest');  // Tells server that this call is made for ajax purposes.

POST /xmlEcho.asp HTTP/1.1
Content-Type: text/xml
User-Agent: Chilkat/1.0.0 (+http://www.chilkatsoft.com/ChilkatHttpUA.asp)
Host: www.chilkatsoft.com
Accept-Charset: ISO-8859-1,utf-8;q=0.7,*; q=0.7
Accept-Language: en-us,en;q=0.5
Accept-Encoding: gzip
Content-Length: 81

<?xml version="1.0" encoding="UTF-8"?><appXml><something>123</something></appXml>


*/

/*
Content-Type:application/x-www-form-urlencoded; charset=UTF-8

  var jqxhr = $.ajax ({type: "POST",
                      async: false,
                      url: FileUtil.urlServer + "workspace.php",
                      data: {project: szProjectNameIn,
                             scene: szSceneNameIn,
                             data: szBufferIn,
                             action: "writescene"}
                     });

*/




/*
// NOTE:  The below code sets up the TCP conneciton to be encoded with SSL.
//        Use this as a guide for when you add support for HTTPS
int main() {

  char           dest_url[] = "https://www.hp.com";
  BIO              *certbio = NULL;
  BIO               *outbio = NULL;
  X509                *cert = NULL;
  X509_NAME       *certname = NULL;
  const SSL_METHOD *method;
  SSL_CTX *ctx;
  SSL *ssl;
  int server = 0;
  int ret, i;

  // ---------------------------------------------------------- *
  // These function calls initialize openssl for correct work.  *
  // ---------------------------------------------------------- *
  OpenSSL_add_all_algorithms();
  ERR_load_BIO_strings();
  ERR_load_crypto_strings();
  SSL_load_error_strings();

  // ---------------------------------------------------------- *
  // Create the Input/Output BIO's.                             *
  // ---------------------------------------------------------- *
  certbio = BIO_new(BIO_s_file());
  outbio  = BIO_new_fp(stdout, BIO_NOCLOSE);

  // ---------------------------------------------------------- *
  // initialize SSL library and register algorithms             *
  // ---------------------------------------------------------- *
  if(SSL_library_init() < 0)
    BIO_printf(outbio, "Could not initialize the OpenSSL library !\n");

  // ---------------------------------------------------------- *
  // Set SSLv2 client hello, also announce SSLv3 and TLSv1      *
  // ---------------------------------------------------------- *
  method = SSLv23_client_method();

  // ---------------------------------------------------------- *
  // Try to create a new SSL context                            *
  // ---------------------------------------------------------- *
  if ( (ctx = SSL_CTX_new(method)) == NULL)
    BIO_printf(outbio, "Unable to create a new SSL context structure.\n");

  // ---------------------------------------------------------- *
  // Disabling SSLv2 will leave v3 and TSLv1 for negotiation    *
  // ---------------------------------------------------------- *
  SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);

  // ---------------------------------------------------------- *
  // Create new SSL connection state object                     *
  // ---------------------------------------------------------- *
  ssl = SSL_new(ctx);

  // ---------------------------------------------------------- *
  // Make the underlying TCP socket connection                  *
  // ---------------------------------------------------------- *
  server = create_socket(dest_url, outbio);
  if(server != 0)
    BIO_printf(outbio, "Successfully made the TCP connection to: %s.\n", dest_url);

  // ---------------------------------------------------------- *
  // Attach the SSL session to the socket descriptor            *
  // ---------------------------------------------------------- *
  SSL_set_fd(ssl, server);

  // ---------------------------------------------------------- *
  // Try to SSL-connect here, returns 1 for success             *
  // ---------------------------------------------------------- *
  if ( SSL_connect(ssl) != 1 )
    BIO_printf(outbio, "Error: Could not build a SSL session to: %s.\n", dest_url);
  else
    BIO_printf(outbio, "Successfully enabled SSL/TLS session to: %s.\n", dest_url);

  // ---------------------------------------------------------- *
  // Get the remote certificate into the X509 structure         *
  // ---------------------------------------------------------- *
  cert = SSL_get_peer_certificate(ssl);
  if (cert == NULL)
    BIO_printf(outbio, "Error: Could not get a certificate from: %s.\n", dest_url);
  else
    BIO_printf(outbio, "Retrieved the server's certificate from: %s.\n", dest_url);

  // ---------------------------------------------------------- *
  // extract various certificate information                    *
  // -----------------------------------------------------------*
  certname = X509_NAME_new();
  certname = X509_get_subject_name(cert);

  // ---------------------------------------------------------- *
  // display the cert subject here                              *
  // -----------------------------------------------------------*
  BIO_printf(outbio, "Displaying the certificate subject data:\n");
  X509_NAME_print_ex(outbio, certname, 0, 0);
  BIO_printf(outbio, "\n");

  // ---------------------------------------------------------- *
  // Free the structures we don't need anymore                  *
  // -----------------------------------------------------------*
  SSL_free(ssl);
  close(server);
  X509_free(cert);
  SSL_CTX_free(ctx);
  BIO_printf(outbio, "Finished SSL/TLS connection with server: %s.\n", dest_url);
  return(0);
}
*/



