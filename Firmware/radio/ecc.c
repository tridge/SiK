// -*- Mode: C; c-basic-offset: 8; -*-
//
// Copyright (c) 2012 Andrew Tridgell, All Rights Reserved
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//  o Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  o Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in
//    the documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//

///
/// @file	eec.c
///
///

#include <stdarg.h>
#include "radio.h"
#include "golay.h"
#include "ecc.h"

static __xdata uint8_t ebuf[MAX_AIR_PACKET_LENGTH];

// first attempt at error correcting code

bool
ecc_transmit(uint8_t length, __xdata uint8_t * __pdata buf, __pdata uint16_t timeout_ticks)
{
	uint8_t elen;

	elen = 3*((length+1+2)/3);

	if (elen > MAX_AIR_PACKET_LENGTH/2) {
		panic("ecc packet too long");
	}
	buf[elen-1] = length;
	golay_encode(elen, buf, ebuf);
	return radio_transmit(elen*2, ebuf, timeout_ticks);
}


bool
ecc_receive(uint8_t *length, __xdata uint8_t * __pdata buf)
{
	uint8_t elen;

	if (!radio_receive_packet(&elen, ebuf)) {
		return false;
	}
	if (elen > MAX_AIR_PACKET_LENGTH || (elen % 6) != 0) {
		printf("invalid elen %u\n", (unsigned)elen);
		return false;
	}
	golay_decode(elen, ebuf, buf);
	*length = buf[(elen/2)-1];
	return true;
}
