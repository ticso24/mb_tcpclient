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

#include <bwctmb/bwctmb.h>

int main(int argc, char *argv[]);
void usage(void);

int
main(int argc, char *argv[]) {
	uint8_t address;
	uint16_t number;
	uint16_t val;
	uint16_t count;
	int i;
	String host;
	String port;
	String cmd;

	if (argc < 3)
		usage();
	host = argv[1];
	port = argv[2];
	Modbus mb(host, port);
	argc -= 3;
	argv += 3;

	while (argc > 0) {
		if (argc < 2)
			usage();
		address = atol(argv[0]);
		cmd = argv[1];
		argc -= 2;
		argv += 2;

		try {
			if (cmd == "read_inputs")  {
				if (argc < 2)
					usage();
				number = atol(argv[0]);
				count = atol(argv[1]);
				SArray<bool> vals;
				vals = mb.read_discrete_inputs(address, number, count);
				printf("%i@[%s]:%s inputs %i=",
				    address, host.c_str(), port.c_str(), number);
				for (i = 0; i <= vals.max; i++) {
					printf("%i", vals[i] ? 1 : 0);
					if (i != vals.max) {
						printf(":");
					}
				}
				printf("\n");
				argc -= 2;
				argv += 2;
			} else if (cmd == "read_input")  {
				if (argc < 1)
					usage();
				number = atol(argv[0]);
				bool res = mb.read_discrete_input(address, number);
				printf("%i@[%s]:%s input %i=%i\n",
				    address, host.c_str(), port.c_str(), number, res ? 1 : 0);
				argc -= 1;
				argv += 1;
			} else if (cmd == "read_coils")  {
				if (argc < 2)
					usage();
				number = atol(argv[0]);
				count = atol(argv[1]);
				SArray<bool> vals;
				vals = mb.read_coils(address, number, count);
				printf("%i@[%s]:%s coils %i=",
				    address, host.c_str(), port.c_str(), number);
				for (i = 0; i <= vals.max; i++) {
					printf("%i", vals[i] ? 1 : 0);
					if (i != vals.max) {
						printf(":");
					}
				}
				printf("\n");
				argc -= 2;
				argv += 2;
			} else if (cmd == "read_coil")  {
				if (argc < 1)
					usage();
				number = atol(argv[0]);
				bool res = mb.read_coil(address, number);
				printf("%i@[%s]:%s coil %i=%i\n",
				    address, host.c_str(), port.c_str(), number, res ? 1 : 0);
				argc -= 1;
				argv += 1;
			} else if (cmd == "write_coils")  {
				if (argc < 2)
					usage();
				number = atol(argv[0]);
				SArray<bool> vals;
				char *pos = argv[1];
				char *tmpstr;
				int i = 0;
				while ((tmpstr = strsep(&pos, ":")) != NULL) {
					if (*tmpstr != '\0') {
						vals[i++] = atol(tmpstr);
					}
				}
				mb.write_coils(address, number, vals);
				argc -= 2;
				argv += 2;
			} else if (cmd == "write_coil")  {
				if (argc < 2)
					usage();
				number = atol(argv[0]);
				val = atol(argv[1]);
				mb.write_coil(address, number, val);
				argc -= 2;
				argv += 2;
			} else if (cmd == "read_input_registers")  {
				if (argc < 2)
					usage();
				number = atol(argv[0]);
				count = atol(argv[1]);
				SArray<uint16_t> vals;
				vals = mb.read_input_registers(address, number, count);
				printf("%i@[%s]:%s inputs %i=",
				    address, host.c_str(), port.c_str(), number);
				for (i = 0; i <= vals.max; i++) {
					printf("%i", vals[i]);
					if (i != vals.max) {
						printf(":");
					}
				}
				printf("\n");
				argc -= 2;
				argv += 2;
			} else if (cmd == "read_input_register")  {
				if (argc < 1)
					usage();
				number = atol(argv[0]);
				uint16_t res = mb.read_input_register(address, number);
				printf("%i@[%s]:%s input %i=%i\n",
				    address, host.c_str(), port.c_str(), number, res);
				argc -= 1;
				argv += 1;
			} else if (cmd == "read_holding_registers")  {
				if (argc < 2)
					usage();
				number = atol(argv[0]);
				count = atol(argv[1]);
				SArray<uint16_t> vals;
				vals = mb.read_holding_registers(address, number, count);
				printf("%i@[%s]:%s registers %i=",
				    address, host.c_str(), port.c_str(), number);
				for (i = 0; i <= vals.max; i++) {
					printf("%i", vals[i]);
					if (i != vals.max) {
						printf(":");
					}
				}
				printf("\n");
				argc -= 2;
				argv += 2;
			} else if (cmd == "read_holding_register")  {
				if (argc < 1)
					usage();
				number = atol(argv[0]);
				uint16_t res = mb.read_holding_register(address, number);
				printf("%i@[%s]:%s register %i=%i\n",
				    address, host.c_str(), port.c_str(), number, res);
				argc -= 1;
				argv += 1;
			} else if (cmd == "write_register")  {
				if (argc < 2)
					usage();
				number = atol(argv[0]);
				val = atol(argv[1]);
				mb.write_register(address, number, val);
				argc -= 2;
				argv += 2;
			} else if (cmd == "write_registers")  {
				if (argc < 2)
					usage();
				number = atol(argv[0]);
				SArray<uint16_t> vals;
				char *pos = argv[1];
				char *tmpstr;
				int i = 0;
				while ((tmpstr = strsep(&pos, ":")) != NULL) {
					if (*tmpstr != '\0') {
						vals[i++] = atol(tmpstr);
					}
				}
				mb.write_registers(address, number, vals);
				argc -= 2;
				argv += 2;
			} else if (cmd == "identification")  {
				if (argc < 1)
					usage();
				String res = mb.identification(address, atol(argv[0]));
				printf("%i@[%s]:%s String %i=%s\n",
				    address, host.c_str(), port.c_str(), (int)atol(argv[0]), res.c_str());
				argc -= 1;
				argv += 1;
			} else if (cmd == "bwct_set_address")  {
				if (argc < 2)
					usage();
				mb.bwct_set_address(address, atol(argv[0]), String(argv[1]));
				argc -= 2;
				argv += 2;
			} else if (cmd == "bwct_read_magcard")  {
				String res = mb.bwct_read_magcard(address);
				printf("%i@[%s]:%s Cardreader (%s)\n",
				    address, host.c_str(), port.c_str(), res.c_str());
			} else if (cmd == "bwct_fw_update")  {
				if (argc < 1)
					usage();
				mb.bwct_fw_update(address, argv[0]);
				argc -= 1;
				argv += 1;
			} else
				usage();
		} catch (std::exception& e) {
			fprintf(stderr, "Error: %s\n", e.what());
			exit(1);
		}
	}
	return 0;
}

void
usage(void) {

	printf("usage: mb_tcpclient ip port slaveaddress cmd [cmddata] \n");
	printf(" read_input inputnumber\n");
	printf(" read_inputs inputnumber count\n");
	printf(" read_coil coilnumber\n");
	printf(" read_coils coilnumber count\n");
	printf(" write_coil coilnumber value\n");
	printf(" write_coils coilnumber value1:value2[:value3[...]]\n");
	printf(" read_input_register registernumber\n");
	printf(" read_input_registers registernumber count\n");
	printf(" read_holding_register registernumber\n");
	printf(" read_holding_registers registernumber count\n");
	printf(" write_registers registernumber value1:value2[:value3[...]]\n");
	printf(" write_register registernumber value\n");
	printf(" identification stringnumber\n");
	printf("  string 0 vendor\n");
	printf("  string 1 product\n");
	printf("  string 2 version\n");
	printf("  string 3 url\n");
	printf("  string 128 bwct device serial\n");
	printf("  string 129 bwct firmware serial\n");
	printf(" bwct_set_address newaddress serial\n");
	printf(" bwct_read_magcard\n");
	printf(" bwct_fw_update firmwarefile\n");
	printf(" nothing\n");
	exit(1);
}

