/* -----------------------------------------------------------------
                             Sockets

    This module implements sockets for TCP/IP communications.


   ----------------------------------------------------------------- */

// Authors:  Michael T. Duffy  (mduffor@gmail.com)

// Modified BSD License:
//
// Copyright (c) 2004-2016, Michael T. Duffy II.  All rights reserved.
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


// REFERENCE:
//  http://www.beej.us/guide/bgnet/output/print/bgnet_A4.pdf

#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include "Net/Socket.hpp"

#ifdef LINUX
  #define LINUX_OR_ANDROID
#elif defined ANDROID_NDK
  #define LINUX_OR_ANDROID
#endif

//void error(char *msg)
//{
//    perror(msg);
//    exit(0);
//}


int Socket::iRefCount      = 0;

//------------------------------------------------------------------------
Socket::Socket ()
  {
  Init ();


  };


//------------------------------------------------------------------------
Socket::Socket  (const char *  szClientIn,
                 int           iPortIn)
  {
  Init ();
  ClientConnect (szClientIn, iPortIn);
  };


//------------------------------------------------------------------------
Socket::~Socket ()
  {


  if (iRefCount != 0) --iRefCount;
  if (iRefCount == 0)
    {
    #ifdef WIN32
    WSACleanup ();
    #endif // WIN32
    };

  };



//------------------------------------------------------------------------
EStatus  Socket::Init  (VOID)
  {
  iSocketfd  = 0;
  iPort      = 0;
  server     = NULL;
  bConnected = FALSE;

  memset (&serv_addr, 0, sizeof (serv_addr));


  // perform class-wide initialization when the first instance is created.
  ++iRefCount;
  if (iRefCount == 1)
    {
    #ifdef WIN32

    WSADATA wsaDat;
    int     iReturnCode;

    iReturnCode = WSAStartup (MAKEWORD(1, 1), &wsaDat);

    if (iReturnCode != 0)
      {
      fprintf (stderr, "WSA Initialization failed.");

      if (iReturnCode == WSASYSNOTREADY)
        {
        fprintf (stderr, "Network is not ready.");
        }
      else if (iReturnCode == WSAVERNOTSUPPORTED)
        {
        fprintf (stderr, "Implementation does not provide requested WinSock version.");
        }
      else if (iReturnCode == WSAEINVAL)
        {
        fprintf (stderr, "DLL does not support requested WinSock version.");
        }
      else
        {
        fprintf (stderr, "unknown error.");
        };
      return (EStatus::kFailure);
      }
    #endif // WIN32
    };
  return (EStatus::kSuccess);
  };


//------------------------------------------------------------------------
EStatus Socket::ClientConnect (const char *   szClientIn,
                               int            iPortIn)
  {
  EStatus  errorStatus = EStatus::kFailure;
  iPort = iPortIn;

  if (bConnected)
    {
    ClientDisconnect ();
    };

  // initialize the serv_addr structure
  memset (&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons (iPort);


  // find the server
  // NOTE:  This digit test will not work with with IPv6
  if (isdigit (szClientIn [0]))
    {
    // server is an IP number
    if (inet_pton (AF_INET, szClientIn, &(serv_addr.sin_addr)) <= 0)
      {
      // unable to parse IP address in presentation mode
      RStr  strErrorOut ("Socket::ClientConnect Failure - Unable to parse IP address in presentation mode : ");

      strErrorOut += szClientIn;
      errorStatus.SetDescription (strErrorOut);
      DBG_ESTATUS (errorStatus);

      // error connecting
      return (errorStatus);

      };
    }
  else
    {
    // server is a name.  Look it up
    if ((server = gethostbyname (szClientIn)) == NULL)
      {
      // error: no such host.  This is an early-out failure so we don't create a socket we can't use.
      RStr  strErrorOut ("Socket::ClientConnect Failure - No Host found for ");

      strErrorOut += szClientIn;
      errorStatus.SetDescription (strErrorOut);
      DBG_ESTATUS (errorStatus);

      // error connecting
      return (errorStatus);
      };

    // the following two lines do the same thing for IPv4.  We likely need to revisit for IPv6 due to a different address size.
    serv_addr.sin_addr.s_addr = *((unsigned long*) &server->h_addr);
    //memcpy (server->h_addr, &(serv_addr.sin_addr.s_addr), server->h_length);
    };

  DBG_INFO ("Socket::ClientConnect - Creating socket for client %s:%i", szClientIn, iPortIn);
  // create the socket
  iSocketfd = socket (AF_INET, SOCK_STREAM, 0);
  if (iSocketfd < 0)
    {
    // error opening socket
    errorStatus.SetDescription ("Socket::ClientConnect Failure - Unable to create socket.");
    DBG_ESTATUS (errorStatus);
    return (errorStatus);
    };

  // connect to the server
  if (connect (iSocketfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    {
    RStr  strErrorOut ("Socket::ClientConnect Failure - ");

    strErrorOut += GetErrorString ();
    errorStatus.SetDescription (strErrorOut);
    DBG_ESTATUS (errorStatus);

    // error connecting
    return (errorStatus);
    };
  DBG_INFO ("Socket::ClientConnect - Connected");
  bConnected = TRUE;
  return (EStatus::kSuccess);
  };


//------------------------------------------------------------------------
EStatus Socket::ClientDisconnect (VOID)
  {

  #ifdef WIN32
  closesocket (iSocketfd);
  #endif // WIN32

  #ifdef LINUX_OR_ANDROID
  close (iSocketfd);
  #endif // LINUX_OR_ANDROID

  iSocketfd  = 0;
  bConnected = FALSE;

  //shutdown (iSocketfd ,SHUT_RDWR);
  DBG_INFO ("Socket::ClientDisconnect - Disconnected");

  return (EStatus::kSuccess);
  };


//------------------------------------------------------------------------
EStatus Socket::Write (const RStr &  strIn)
  {
  //int  iBytesWritten = write (iSocketfd, strIn.AsChar (), strIn.Length ());

  DBG_INFO ("Socket::Write - Sending %d bytes", strIn.Length ());
  int  iBytesRemaining = strIn.Length ();

  while (iBytesRemaining > 0)
    {
    int  iBytesWritten = send (iSocketfd, strIn.AsChar (), iBytesRemaining, 0);

    iBytesRemaining -= iBytesWritten;

    if (iBytesWritten < 0)
      {
      RStr  strErrorOut ("Socket::Write Failure - ");
      strErrorOut += GetErrorString ();

      EStatus errorStatus  = EStatus::Failure (strErrorOut);
      DBG_ESTATUS (errorStatus);
      return (errorStatus);
      };
    };
  return (EStatus::kSuccess);
  };


//------------------------------------------------------------------------
INT  Socket::GetBytesToRead (VOID)
  {
  // NOTE:  Not sure if I need to perform an empty read before checking
  //  for number of bytes available for reading.

  #ifdef LINUX_OR_ANDROID
  int            iNumBytesToRead;
  #endif // LINUX_OR_ANDROID

  #ifdef WIN32
  unsigned long  iNumBytesToRead;
  #endif // WIN32


  #ifdef LINUX_OR_ANDROID
  ioctl (iSocketfd, FIONREAD, &iNumBytesToRead);
  #endif // LINUX_OR_ANDROID

  #ifdef WIN32
  ioctlsocket (iSocketfd, FIONREAD, &iNumBytesToRead);
  #endif // WIN32

  return (INT(iNumBytesToRead));
  }

//------------------------------------------------------------------------
EStatus Socket::Read (RStr & strOut)
  {
  int       iBytesRead;
  int       iNumBytesToRead;



  // Set socket to blocking so we wait for the message
  Block (true);

  // block for the entire message
  int  iFlags = MSG_PEEK;
  #ifdef LINUX_OR_ANDROID
    iFlags |= MSG_WAITALL;
  #endif

  // block until data is ready to recv
  recv (iSocketfd, NULL, 0, iFlags);

  // get the number of bytes in that message
  iNumBytesToRead = GetBytesToRead ();

  // size the string so we can read the entire message

  strOut.Grow (iNumBytesToRead + 1);
  int  iStartingLength = strOut.Length();

  // now read the actual data
  iBytesRead = recv (iSocketfd, strOut.GetBufferPtr (iStartingLength), strOut.GetAllocSize () - strOut.Length(), 0);

  if (iBytesRead < 0)
    {
    #ifdef LINUX_OR_ANDROID
    if (errno == EAGAIN)
      {
      // no message waiting
      return (EStatus::kSuccess);
      };
    #endif // LINUX_OR_ANDROID

    #ifdef WIN32


    #endif // WIN32

    // error.
    return (EStatus::kFailure);
    };

  // by setting the null, we also set the string length and null terminate the read bytes.
  strOut.SetAt (iStartingLength + iBytesRead, '\0');

  return (EStatus::kSuccess);
  };


//------------------------------------------------------------------------
VOID Socket::Block (BOOL  bBlockStatusIn)
  {

  #ifdef WIN32
  unsigned long lNoBlock = (bBlockStatusIn ? 0L : 1L);
  int rc;

  rc = ioctlsocket (iSocketfd, FIONBIO, &lNoBlock);

  if (rc) {errno = WSAGetLastError();}

  //return (rc);
  #endif


  #ifdef LINUX_OR_ANDROID
  unsigned long lBlock = (bBlockStatusIn ? 0L : O_NONBLOCK);
  //int rc = (fcntl (iSocketfd, F_SETFL, lBlock) == -1);
  fcntl (iSocketfd, F_SETFL, lBlock);

  //return(rc);
  #endif


  };


//------------------------------------------------------------------------
const char *  Socket::GetErrorString (VOID)
  {
  static char  szOut [255];

  #ifdef LINUX_OR_ANDROID
    INT32  iError = errno;

    switch (iError)
      {
      case EACCES:       {return ("Write permission denied.");};
      //case EAGAIN:       // fall through
      case EWOULDBLOCK:  {return ("Blocking operation on a nonblocking socket.");};
      case EBADF:        {return ("Bad File Descriptor");};
      case ECONNRESET:   {return ("Connection reset by peer.");};
      case EDESTADDRREQ: {return ("Socket is not connection-mode, and no peer address is set.");};
      case EFAULT:       {return ("Socket structure address outside of user address space.");};
      case EINTR:        {return ("A signal occurred before data was transmitted.");};
      case EINVAL:       {return ("Invalid argument passed.");};
      case EISCONN:      {return ("The socket is already connected.");};
      case EMSGSIZE:     {return ("Size of message prevents automatic sending.");};
      case ENOBUFS:      {return ("The output queue was full.  It is possible the interface has stopped sending, or that there is transient congestion. ");};
      case ENOMEM:       {return ("No memory available..");};
      case ENOTCONN:     {return ("The socket is not connected, and no target has been given.");};
      case ETIMEDOUT:    {return ("Connection timed out.");};
      case ECONNREFUSED: {return ("Connection refused.");};
      case EHOSTDOWN:    {return ("Host is down.");};
      case EHOSTUNREACH: {return ("Host is unreachable.");};
      case ENOTSOCK:     {return ("File Descriptor is not a Socket.");};
      case EOPNOTSUPP:   {return ("Some bit in the flags argument is inappropriate for the socket type.");};
      case EPIPE:        {return ("The local end has been shut down on a connection oriented socket.");};
      default: break;
      };
  #endif // LINUX_OR_ANDROID


  #ifdef WIN32
    INT32  iError = WSAGetLastError ();

    switch (iError)
      {
      //case WSAEBADF:        {return ("Bad File Descriptor");
      case WSAEFAULT:       {return ("Socket structure address outside of user address space");};
      case WSAENOTSOCK:     {return ("File Descriptor is not a Socket");};
      case WSAEISCONN:      {return ("The socket is already connected");};
      case WSAECONNREFUSED: {return ("Connection Refused");};
      case WSAETIMEDOUT:    {return ("Timed Out");};
      default: break;
      };
  #endif // WIN32

  sprintf (szOut, "Unknown errno (%d);", iError);

  return (szOut);
  };

