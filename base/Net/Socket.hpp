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


#ifndef SOCKET_HPP
#define SOCKET_HPP

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#ifdef ANDROID_NDK
  #define LINUX
#endif

#ifdef LINUX
  #include <sys/socket.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <unistd.h>
  #include <errno.h>
  #include <fcntl.h>
  #include <sys/ioctl.h>
  #define INVALID_SOCKET -1
  #define SOCKET_ERROR   -1
  typedef int SOCKET;

#endif

#ifdef WIN32
#include <winsock.h>
#endif


#include <memory.h>

#include "Sys/Types.hpp"
#include "Util/RStr.hpp"


//------------------------------------------------------------------------
class  Socket
  {
  private:

    int                 iSocketfd;
    int                 iPort;
    struct sockaddr_in  serv_addr; // Note: This is an IPv4 structure.  We will need to revisit for IPv6 support.
    struct hostent *    server;

    static int          iRefCount;
    BOOL                bConnected;

  public:


                  Socket           ();

                  Socket           (const char *  szClientIn,
                                    int           iPortIn);

                  ~Socket          ();

    EStatus       Init             (VOID);


    EStatus       ClientConnect    (const char *  szClientIn,
                                    int           iPortIn);



    EStatus       ClientDisconnect (VOID);

                                   /** @brief Queries the IOCTL for number of bytes available to read from the socket
                                       @return Number of bytes that can be read.
                                   */
    INT           GetBytesToRead   (VOID);

    EStatus       Write            (const RStr &  strIn);

                                   /** @brief Reads from the connected socket
                                       @param strOut String to receive the read.  The read data will be appended, and the size of the string grown to accomodate the read.
                                       @return Whether the read succeeded
                                   */
    EStatus       Read             (RStr & strOut);

    BOOL          IsConnected      (VOID)                   {return bConnected;};

    VOID          Block            (BOOL  bBlockStatusIn);

    const char *  GetErrorString (VOID);

  };


#endif // SOCKET_HPP
