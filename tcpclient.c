/*
 * Copyright (c) 2001 - 2003 Bernd Walter Computer Technology
 * http://www.bwct.de
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $URL$
 * $Date$
 * $Author$
 * $Rev$
 */

#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/uio.h>

#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

// TODO: timeout handling

int main(int argc, char *argv[]);
void usage(void);

static int fd;

static uint8_t packet[256];
static uint8_t header[6];
static uint8_t packetlen;

ssize_t
readn(int fd, const void *vptr, size_t n) {
	char *ptr = (char*)vptr;
	fd_set fds;
	size_t nleft = n;
	while (nleft > 0) {
		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		select(fd + 1, &fds, NULL, NULL, NULL);
		ssize_t nread;
		if ((nread = read(fd, ptr, nleft)) < 0) {
			if (errno != EAGAIN && errno != EINTR)
				return(nread);
			nread = 0;
		}
		nleft -= nread;
		ptr += nread;
	}
	return (n);
}

ssize_t
writen(int fd, const void *vptr, size_t n) {
	char *ptr = (char*)vptr;
	fd_set fds;
	size_t nleft = n;
	while (nleft > 0) {
		FD_ZERO(&fds);
		FD_SET(fd, &fds);
		select(fd + 1, NULL, &fds, NULL, NULL);
		ssize_t nwritten;
		if ((nwritten = write(fd, ptr, nleft)) < 0) {
			if (errno != EAGAIN && errno != EINTR)
				return(nwritten);
			nwritten = 0;
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return (n);
}

void
sendpacket() {
	header[0] = 0x00; // transaction identifier
	header[1] = 0x00;
	header[2] = 0x00; // protocol identifier
	header[3] = 0x00;
	header[4] = 0x00;
	header[5] = packetlen;
	writen(fd, header, sizeof(header));
	writen(fd, packet, packetlen);
}

void
getpacket() {
	readn(fd, header, sizeof(header));
	packetlen = header[5];
	readn(fd, packet, packetlen);
}

int
main(int argc, char *argv[]) {
	uint8_t address;
	uint16_t number;
	char *cmd;
	int error;
	const char *cause = NULL;
	struct addrinfo hints, *res, *res0;

	if (argc <5)
		usage();

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = PF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	error = getaddrinfo(argv[1], argv[2], &hints, &res0);
	if (error) {
		printf("getaddrinfo failed: %s\n", gai_strerror(error));
		exit(1);
	}
	fd = -1;
	cause = "no addresses";
	errno = EADDRNOTAVAIL;
	for (res = res0; res; res = res->ai_next) {
		fd = socket(res->ai_family, res->ai_socktype,
		res->ai_protocol);
		if (fd < 0) {
			cause = "socket";
			continue;
		}
		if (connect(fd, res->ai_addr, res->ai_addrlen) < 0) {
			cause = "connect";
			close(fd);
			fd = -1;
			continue;
		}
		break;  /* okay we got one */
	}
	if (fd < 0) {
		printf("%s failed\n", cause);
		exit(1);
	}
	freeaddrinfo(res0);

	address = atol(argv[3]);
	cmd = argv[4];

	if (strcmp(cmd, "read_coil") == 0)  {
		if (argc !=6)
			usage();
		number = atol(argv[5]);
		packet[0] = address;
		packet[1] = 0x01;
		packet[2] = number >> 8;
		packet[3] = number & 0xff;
		packet[4] = 0x00;
		packet[5] = 0x01;
		packetlen = 6;
		sendpacket();
		getpacket();
		if (packetlen == 0)
			exit(1);
		if (packet[1] & 0x80)
			exit(2);
		printf("%i@[%s]:%s coil %i=%i\n",
		    address, argv[1], argv[2], number, packet[3] ? 1 : 0);
	} else if (strcmp(cmd, "write_coil") == 0)  {
		if (argc !=7)
			usage();
		number = atol(argv[5]);
		packet[0] = address;
		packet[1] = 0x05;
		packet[2] = number >> 8;
		packet[3] = number & 0xff;
		packet[4] = (strcmp(argv[6], "0") == 1) ? 0xff : 0x00;
		packet[5] = 0x00;
		packetlen = 6;
		sendpacket();
		getpacket();
		if (packetlen == 0)
			exit(1);
		if (packet[1] & 0x80)
			exit(2);
	} else if (strcmp(cmd, "identification") == 0)  {
		if (argc !=3)
			usage();
		//  TODO
	} else
		usage();
	return 0;
}

void
usage(void) {

	printf("usage: serclient ip port slaveaddress cmd [cmddata] \n");
	exit(1);
}

