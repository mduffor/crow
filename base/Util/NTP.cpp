/* -----------------------------------------------------------------
                              NTP.CPP

     These routines are for quering the current time from the
   internet's NTP time servers.

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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <errno.h>

#include "Sys/Types.hpp"
#include "Debug.hpp"
ASSERTFILE (__FILE__);

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <Util/NTP.hpp>

#define NTP_TIMESTAMP_DELTA 2208988800ull

//------------------------------------------------------------------------------
typedef struct
  {
  unsigned li   : 2;       // Only two bits. Leap indicator.
  unsigned vn   : 3;       // Only three bits. Version number of the protocol.
  unsigned mode : 3;       // Only three bits. Mode. Client will pick mode 3 for client.

  uint8_t stratum;         // Eight bits. Stratum level of the local clock.
  uint8_t poll;            // Eight bits. Maximum interval between successive messages.
  uint8_t precision;       // Eight bits. Precision of the local clock.

  uint32_t rootDelay;      // 32 bits. Total round trip delay time.
  uint32_t rootDispersion; // 32 bits. Max error aloud from primary clock source.
  uint32_t refId;          // 32 bits. Reference clock identifier.

  uint32_t refTm_s;        // 32 bits. Reference time-stamp seconds.
  uint32_t refTm_f;        // 32 bits. Reference time-stamp fraction of a second.

  uint32_t origTm_s;       // 32 bits. Originate time-stamp seconds.
  uint32_t origTm_f;       // 32 bits. Originate time-stamp fraction of a second.

  uint32_t rxTm_s;         // 32 bits. Received time-stamp seconds.
  uint32_t rxTm_f;         // 32 bits. Received time-stamp fraction of a second.

  uint32_t txTm_s;         // 32 bits and the most important field the client cares about. Transmit time-stamp seconds.
  uint32_t txTm_f;         // 32 bits. Transmit time-stamp fraction of a second.

  } ntp_packet;            // Total: 384 bits or 48 bytes.



//------------------------------------------------------------------------------
EStatus NTP::GetNetworkTimeSec (INT64 &  iTimeOut)
  {
  // NOTE:  Code from http://www.mydailyhacks.org/2014/11/14/get-the-ntp-time-in-c-programm-via-a-simple-socket/
  // Also: https://lettier.github.io/posts/2016-04-26-lets-make-a-ntp-client-in-c.html

  struct hostent *  pServer;
  //char *  pszHostname = (char *) "200.20.186.76";

  iTimeOut = 0;

  // ntp uses port 123
  INT iPortNo = 123;
  INT iSize;

  // buffer for the socket request
  // Create and zero out the packet. All 48 bytes worth.
  ntp_packet packet = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  memset (&packet, 0, sizeof (ntp_packet));

  // Set the first byte's bits to 00,011,011 for li = 0, vn = 3, and mode = 3. The rest will be left set to zero.
  *((char *) &packet + 0) = 0x1b; // Represents 27 in base 10 or 00011011 in base 2.

  //struct in_addr ipaddr;
  struct sockaddr_in  server_addr;
  INT iSocket;

  // open a UDP socket
  iSocket = socket (AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if (iSocket < 0)
    {
    return (EStatus::Failure ("NTP: Unable to open socket."));
    };

  //here you can convert pszHostname to ipaddress if needed
  //$ipaddr = inet_aton($HOSTNAME);
  pServer = gethostbyname ("pool.ntp.org");

  if (pServer == NULL)
    {
    close (iSocket);
    return (EStatus::Failure ("NTP: Unable to access host."));
    };

  memset (&server_addr, 0, sizeof (server_addr));
  server_addr.sin_family      = AF_INET;
  //server_addr.sin_addr.s_addr = inet_addr (pszHostname);
  bcopy ((char*) pServer->h_addr, (char*) &server_addr.sin_addr.s_addr, pServer->h_length);
  server_addr.sin_port        = htons     (iPortNo);

  // send the data to the timing server
  iSize = INT (sendto (iSocket,
                       &packet,
                       sizeof (ntp_packet),
                       0,
                       (struct sockaddr *) &server_addr,
                       sizeof (server_addr)));

  if (iSize < 0)
    {
    close (iSocket);
    return (EStatus::Failure ("NTP: Unable to write to port."));
    };

  // get the data back
  struct sockaddr saddr;
  socklen_t saddr_l = sizeof (saddr);

  // REFACTOR:  The following recvfrom call can lock the program.  Need to rewrite this
  //  class so we can start an NTP query, and then regularly poll it until something
  //  useful comes back.

  // ERROR: The following call WILL lock up if there is no network connection, or if
  //   the connection is valid but a firewall is blocking traffic.  Before release,
  //   this function HAS to be called in a thread other than the main one.

  // here we wait for the reply and fill it into our buffer
  iSize = INT (recvfrom (iSocket,
                         &packet,
                         sizeof (ntp_packet),
                         0, // could be MSG_DONTWAIT
                         &saddr,
                         &saddr_l));

  close (iSocket);

  if (iSize < 0)
    {
    // REFACTOR:  We should check the variable errno to see if it is EAGAIN or EWOULDBLOCK,
    //  after setting the above MSG_DONTWAIT flag, and then return an indication that the
    //  call is still in progress.

    return (EStatus::Failure ("NTP: Unable to read from port."));
    };

  // Handle network-to-host endian conversion

  packet.txTm_s = ntohl (packet.txTm_s); // Time-stamp seconds.
  packet.txTm_f = ntohl (packet.txTm_f); // Time-stamp fraction of a second.

  time_t txTm = ( time_t ) ( packet.txTm_s - NTP_TIMESTAMP_DELTA );

  // We ignore the fractional part, and stick to round seconds.  The fractional
  //  part is (txTm_f / 2^-32) seconds.

  iTimeOut = txTm;
  return (EStatus::kSuccess);


  //long tmit; // the time -- This is a time_t sort of
  //tmit = ntohl ((time_t) ulBuf[4]); // get transmit time
  //tmit -= 2208988800U;
  //std::cout << "NTP time is " << ctime(&tmit) << std::endl;
  //i=time(0);
  //std::cout << "System time is " << (i-tmit) << " seconds off" << std::endl;
  };





