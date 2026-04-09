/*
 * Copyright (c) 2026 MURAMATSU Atsushi
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
 */

#if !defined(__LIBRETRO__) && !defined(EMSCRIPTEN)

#include "compiler.h"

#include "pccore.h"
#include "np2.h"
#include "commng.h"
#include "codecnv/codecnv.h"
#if defined(SUPPORT_NP2_TICKCOUNT)
#include <np2_tickcount.h>
#endif

#if defined(_WINDOWS)
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <termios.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

void
CMPara::check_timeout()
{
	if (phase != CLOSED) {
		if (prev_output + timeout > current_time) {
			if (phase == OPEN)
				close_session();
			phase = CLOSED;
		}
	}
}

void
CMPara::write(UINT8 data)
{
	if (phase == CLOSED) {
		output = open_session();
		if (output)
			phase = OPEN;
		else
			phase = OPEN_BUT_NOPEER
	}
	if (phase == OPEN)
		fputc(data, output);
	if (phase != CLOSED)
		prev_output = thistime;
}

bool
CMPipe::open_session()
{
	if (notexist, PIPE_NAME)
		mkfifo(PIPE_NAME);
	int fd = open(PIPE_NAME, WRONLY | NONBLOCK);
	if (fd < 0)
		return false;
	output = fdopen(fd, "wb");
}

void
CMPipe::close_session()
{
	if (output) {
		fclose(output);
		output = NULL;
	}
}


#endif	/* __LIBRETRO__ */
