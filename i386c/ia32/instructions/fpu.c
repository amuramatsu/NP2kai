/*
 * Copyright (c) 2012 NONAKA Kimihiro
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

#include <compiler.h>
#include <pccore.h>
#include <ia32/cpu.h>
#include "ia32/ia32.mcr"
#include "ia32/inst_table.h"

#include <ia32/instructions/fpu/fp.h>
#include "ia32/instructions/fpu/fpumem.h"

#if defined(USE_FPU) && !defined(SUPPORT_FPU_DOSBOX) && !defined(SUPPORT_FPU_DOSBOX2) && !defined(SUPPORT_FPU_SOFTFLOAT)
#error No FPU detected. Please define SUPPORT_FPU_DOSBOX, SUPPORT_FPU_DOSBOX2 or SUPPORT_FPU_SOFTFLOAT.
#endif

#if 1
#undef	TRACEOUT
#define	TRACEOUT(s)	_TRACEOUT s
#include <stdarg.h>
#include <stdio.h>
static void _TRACEOUT(const char *format, ...)
{
	FILE *f;
	va_list ap;
	f = fopen("fpu.log", "a+");
	va_start(ap, format);
	vfprintf(f, format, ap);
	va_end(ap);
	fputc('\n', f);
	fclose(f);
}
#else
#undef	TRACEOUT
#define	TRACEOUT(s)	(void)(s)
#endif	/* 0 */

void
fpu_initialize(void)
{
#if defined(USE_FPU)
	if(i386cpuid.cpu_feature & CPU_FEATURE_FPU){
		switch(i386cpuid.fpu_type){
#if defined(SUPPORT_FPU_DOSBOX)
		case FPU_TYPE_DOSBOX:
			insttable_2byte[0][0xae] = insttable_2byte[1][0xae] = DB_FPU_FXSAVERSTOR;
			insttable_1byte[0][0xd8] = insttable_1byte[1][0xd8] = DB_ESC0;
			insttable_1byte[0][0xd9] = insttable_1byte[1][0xd9] = DB_ESC1;
			insttable_1byte[0][0xda] = insttable_1byte[1][0xda] = DB_ESC2;
			insttable_1byte[0][0xdb] = insttable_1byte[1][0xdb] = DB_ESC3;
			insttable_1byte[0][0xdc] = insttable_1byte[1][0xdc] = DB_ESC4;
			insttable_1byte[0][0xdd] = insttable_1byte[1][0xdd] = DB_ESC5;
			insttable_1byte[0][0xde] = insttable_1byte[1][0xde] = DB_ESC6;
			insttable_1byte[0][0xdf] = insttable_1byte[1][0xdf] = DB_ESC7;
			DB_FPU_FINIT();
			break;
#endif
#if defined(SUPPORT_FPU_DOSBOX2)
		case FPU_TYPE_DOSBOX2:
			insttable_2byte[0][0xae] = insttable_2byte[1][0xae] = DB2_FPU_FXSAVERSTOR;
			insttable_1byte[0][0xd8] = insttable_1byte[1][0xd8] = DB2_ESC0;
			insttable_1byte[0][0xd9] = insttable_1byte[1][0xd9] = DB2_ESC1;
			insttable_1byte[0][0xda] = insttable_1byte[1][0xda] = DB2_ESC2;
			insttable_1byte[0][0xdb] = insttable_1byte[1][0xdb] = DB2_ESC3;
			insttable_1byte[0][0xdc] = insttable_1byte[1][0xdc] = DB2_ESC4;
			insttable_1byte[0][0xdd] = insttable_1byte[1][0xdd] = DB2_ESC5;
			insttable_1byte[0][0xde] = insttable_1byte[1][0xde] = DB2_ESC6;
			insttable_1byte[0][0xdf] = insttable_1byte[1][0xdf] = DB2_ESC7;
			DB2_FPU_FINIT();
			break;
#endif
#if defined(SUPPORT_FPU_SOFTFLOAT)
		case FPU_TYPE_SOFTFLOAT:
			insttable_2byte[0][0xae] = insttable_2byte[1][0xae] = SF_FPU_FXSAVERSTOR;
			insttable_1byte[0][0xd8] = insttable_1byte[1][0xd8] = SF_ESC0;
			insttable_1byte[0][0xd9] = insttable_1byte[1][0xd9] = SF_ESC1;
			insttable_1byte[0][0xda] = insttable_1byte[1][0xda] = SF_ESC2;
			insttable_1byte[0][0xdb] = insttable_1byte[1][0xdb] = SF_ESC3;
			insttable_1byte[0][0xdc] = insttable_1byte[1][0xdc] = SF_ESC4;
			insttable_1byte[0][0xdd] = insttable_1byte[1][0xdd] = SF_ESC5;
			insttable_1byte[0][0xde] = insttable_1byte[1][0xde] = SF_ESC6;
			insttable_1byte[0][0xdf] = insttable_1byte[1][0xdf] = SF_ESC7;
			SF_FPU_FINIT();
			break;
#endif
		default:
#if defined(SUPPORT_FPU_SOFTFLOAT)
			insttable_2byte[0][0xae] = insttable_2byte[1][0xae] = SF_FPU_FXSAVERSTOR;
			insttable_1byte[0][0xd8] = insttable_1byte[1][0xd8] = SF_ESC0;
			insttable_1byte[0][0xd9] = insttable_1byte[1][0xd9] = SF_ESC1;
			insttable_1byte[0][0xda] = insttable_1byte[1][0xda] = SF_ESC2;
			insttable_1byte[0][0xdb] = insttable_1byte[1][0xdb] = SF_ESC3;
			insttable_1byte[0][0xdc] = insttable_1byte[1][0xdc] = SF_ESC4;
			insttable_1byte[0][0xdd] = insttable_1byte[1][0xdd] = SF_ESC5;
			insttable_1byte[0][0xde] = insttable_1byte[1][0xde] = SF_ESC6;
			insttable_1byte[0][0xdf] = insttable_1byte[1][0xdf] = SF_ESC7;
			SF_FPU_FINIT();
#elif defined(SUPPORT_FPU_DOSBOX)
			insttable_2byte[0][0xae] = insttable_2byte[1][0xae] = DB_FPU_FXSAVERSTOR;
			insttable_1byte[0][0xd8] = insttable_1byte[1][0xd8] = DB_ESC0;
			insttable_1byte[0][0xd9] = insttable_1byte[1][0xd9] = DB_ESC1;
			insttable_1byte[0][0xda] = insttable_1byte[1][0xda] = DB_ESC2;
			insttable_1byte[0][0xdb] = insttable_1byte[1][0xdb] = DB_ESC3;
			insttable_1byte[0][0xdc] = insttable_1byte[1][0xdc] = DB_ESC4;
			insttable_1byte[0][0xdd] = insttable_1byte[1][0xdd] = DB_ESC5;
			insttable_1byte[0][0xde] = insttable_1byte[1][0xde] = DB_ESC6;
			insttable_1byte[0][0xdf] = insttable_1byte[1][0xdf] = DB_ESC7;
			DB_FPU_FINIT();
#elif defined(SUPPORT_FPU_DOSBOX2)
			insttable_2byte[0][0xae] = insttable_2byte[1][0xae] = DB2_FPU_FXSAVERSTOR;
			insttable_1byte[0][0xd8] = insttable_1byte[1][0xd8] = DB2_ESC0;
			insttable_1byte[0][0xd9] = insttable_1byte[1][0xd9] = DB2_ESC1;
			insttable_1byte[0][0xda] = insttable_1byte[1][0xda] = DB2_ESC2;
			insttable_1byte[0][0xdb] = insttable_1byte[1][0xdb] = DB2_ESC3;
			insttable_1byte[0][0xdc] = insttable_1byte[1][0xdc] = DB2_ESC4;
			insttable_1byte[0][0xdd] = insttable_1byte[1][0xdd] = DB2_ESC5;
			insttable_1byte[0][0xde] = insttable_1byte[1][0xde] = DB2_ESC6;
			insttable_1byte[0][0xdf] = insttable_1byte[1][0xdf] = DB2_ESC7;
			DB2_FPU_FINIT();
#else
			insttable_2byte[0][0xae] = insttable_2byte[1][0xae] = NOFPU_FPU_FXSAVERSTOR;
			insttable_1byte[0][0xd8] = insttable_1byte[1][0xd8] = NOFPU_ESC0;
			insttable_1byte[0][0xd9] = insttable_1byte[1][0xd9] = NOFPU_ESC1;
			insttable_1byte[0][0xda] = insttable_1byte[1][0xda] = NOFPU_ESC2;
			insttable_1byte[0][0xdb] = insttable_1byte[1][0xdb] = NOFPU_ESC3;
			insttable_1byte[0][0xdc] = insttable_1byte[1][0xdc] = NOFPU_ESC4;
			insttable_1byte[0][0xdd] = insttable_1byte[1][0xdd] = NOFPU_ESC5;
			insttable_1byte[0][0xde] = insttable_1byte[1][0xde] = NOFPU_ESC6;
			insttable_1byte[0][0xdf] = insttable_1byte[1][0xdf] = NOFPU_ESC7;
			NOFPU_FPU_FINIT();
#endif
			break;
		}
	}else{
#endif
		insttable_2byte[0][0xae] = insttable_2byte[1][0xae] = NOFPU_FPU_FXSAVERSTOR;
		insttable_1byte[0][0xd8] = insttable_1byte[1][0xd8] = NOFPU_ESC0;
		insttable_1byte[0][0xd9] = insttable_1byte[1][0xd9] = NOFPU_ESC1;
		insttable_1byte[0][0xda] = insttable_1byte[1][0xda] = NOFPU_ESC2;
		insttable_1byte[0][0xdb] = insttable_1byte[1][0xdb] = NOFPU_ESC3;
		insttable_1byte[0][0xdc] = insttable_1byte[1][0xdc] = NOFPU_ESC4;
		insttable_1byte[0][0xdd] = insttable_1byte[1][0xdd] = NOFPU_ESC5;
		insttable_1byte[0][0xde] = insttable_1byte[1][0xde] = NOFPU_ESC6;
		insttable_1byte[0][0xdf] = insttable_1byte[1][0xdf] = NOFPU_ESC7;
		NOFPU_FPU_FINIT();
#if defined(USE_FPU)
	}
#endif
}

static char *
fpu_REG80_str(REG80 value)
{
	static char buf[128];
	char tmp[128];
	snprintf(tmp, sizeof(tmp), "%08x,%08x,%02x",
			 value.d.l[0], value.d.l[1], value.d.h);
	strcpy(buf, tmp);
#ifdef SUPPORT_FPU_SOFTFLOAT
	struct extFloat80M f80;
	sw_float64_t fval;
	f80.signif = LOADINTELQWORD(&value.w[0]);
	f80.signExp = LOADINTELWORD(&value.w[4]);
	fval = extF80M_to_f64((sw_extFloat80_t *)&f80);
	snprintf(tmp, sizeof(tmp), " / %f", *((double *)&fval));
	strcat(buf, tmp);
#endif
	return buf;
}

char *
fpu_reg2str(void)
{
	static char buf[4096];
	char tmp[128];
	int i;
	
	buf[0] = '\0';
	for (i = 0; i < FPU_REG_NUM+1; i++) {
		snprintf(tmp, sizeof(tmp), "st=%d ", i);
		strcat(buf, tmp);
		snprintf(tmp, sizeof(tmp), " TAG: %d ", FPU_STAT.tag[i]);
		strcat(buf, tmp);
#ifdef SUPPORT_FPU_SOFTFLOAT
		sw_float64_t fval = extF80M_to_f64(&FPU_STAT.reg[i].d);
		snprintf(tmp, sizeof(tmp), " F80: %lf", *((double *)&fval));
		strcat(buf, tmp);
#endif
		snprintf(tmp, sizeof(tmp), " F64: %lf", FPU_STAT.reg[i].d64);
		strcat(buf, tmp);
		snprintf(tmp, sizeof(tmp), " BCD: %s\n",
				 fpu_REG80_str(*((REG80 *)FPU_STAT.reg[i].b)));
		strcat(buf, tmp);
	}

	snprintf(tmp, sizeof(tmp),
	    "ctrl=%04x  status=%04x\n"
	    "inst=%04x:%08x  data=%04x:%08x  op=%03x\n",
	    FPU_CTRLWORD, FPU_STATUSWORD,
	    FPU_INSTPTR_SEG, FPU_INSTPTR_OFFSET,
	    FPU_DATAPTR_SEG, FPU_DATAPTR_OFFSET,
	    FPU_LASTINSTOP);
	strcat(buf, tmp);

	return buf;
}

void
fpu_dump()
{
	TRACEOUT(("%s", fpu_reg2str()));
}

/*
 * FPU memory access function
 */
#if defined(USE_FPU)
UINT8 MEMCALL
fpu_memoryread_b(UINT32 address)
{
	UINT16 seg;

	FPU_DATAPTR_SEG = seg = CPU_INST_SEGREG_INDEX;
	FPU_DATAPTR_OFFSET = address;
#if 1
	UINT8 value = cpu_vmemoryread_b(seg, address);
	TRACEOUT(("MRD_B: %02x:%08x -> %02x", seg, address, value));
	return value;
#else
	return cpu_vmemoryread_b(seg, address);
#endif
}

UINT16 MEMCALL
fpu_memoryread_w(UINT32 address)
{
	UINT16 seg;

	FPU_DATAPTR_SEG = seg = CPU_INST_SEGREG_INDEX;
	FPU_DATAPTR_OFFSET = address;
#if 1
	UINT16 value = cpu_vmemoryread_w(seg, address);
	TRACEOUT(("MRD_W: %02x:%08x -> %04x", seg, address, value));
	return value;
#else
	return cpu_vmemoryread_w(seg, address);
#endif
}

UINT32 MEMCALL
fpu_memoryread_d(UINT32 address)
{
	UINT16 seg;

	FPU_DATAPTR_SEG = seg = CPU_INST_SEGREG_INDEX;
	FPU_DATAPTR_OFFSET = address;
#if 1
	UINT32 value = cpu_vmemoryread_d(seg, address);
	TRACEOUT(("MRD_D: %02x:%08x -> %08x", seg, address, value));
	return value;
#else
	return cpu_vmemoryread_d(seg, address);
#endif
}

UINT64 MEMCALL
fpu_memoryread_q(UINT32 address)
{
	UINT16 seg;

	FPU_DATAPTR_SEG = seg = CPU_INST_SEGREG_INDEX;
	FPU_DATAPTR_OFFSET = address;
#if 1
	UINT64 value = cpu_vmemoryread_q(seg, address);
	TRACEOUT(("MRD_Q: %02x:%08x -> %016lx", seg, address, value));
	return value;
#else
	return cpu_vmemoryread_q(seg, address);
#endif
}

REG80 MEMCALL
fpu_memoryread_f(UINT32 address)
{
	UINT16 seg;

	FPU_DATAPTR_SEG = seg = CPU_INST_SEGREG_INDEX;
	FPU_DATAPTR_OFFSET = address;
#if 1
	REG80 value = cpu_vmemoryread_f(seg, address);
	//REG80 value;
	//value.d.l[0] = cpu_vmemoryread_d(seg, address);
	//value.d.l[1] = cpu_vmemoryread_d(seg, address+4);
	//value.d.h = cpu_vmemoryread_w(seg, address+8);
	TRACEOUT(("MRD_F: %02x:%08x -> %s", seg, address, fpu_REG80_str(value)));
	return value;
#else
	return cpu_vmemoryread_f(seg, address);
#endif
}

float MEMCALL
fpu_memoryread_f32(UINT32 address)
{
	UINT16 seg;
	union {
		float f;
		UINT32 l;
	} val;

	FPU_DATAPTR_SEG = seg = CPU_INST_SEGREG_INDEX;
	FPU_DATAPTR_OFFSET = address;
	val.l = cpu_vmemoryread_d(seg, address);
#if 1
	TRACEOUT(("MRD_F32: %02x:%08x -> %f", seg, address, val.f));
#endif
	return val.f;
}

double MEMCALL
fpu_memoryread_f64(UINT32 address)
{
	UINT16 seg;
	union {
		double f;
		UINT64 q;
	} val;

	FPU_DATAPTR_SEG = seg = CPU_INST_SEGREG_INDEX;
	FPU_DATAPTR_OFFSET = address;
	val.q = cpu_vmemoryread_q(seg, address);
#if 1
	TRACEOUT(("MRD_F64: %02x:%08x -> %lf", seg, address, val.f));
#endif
	return val.f;
}

void MEMCALL
fpu_memorywrite_b(UINT32 address, UINT8 value)
{
	UINT16 seg;

	FPU_DATAPTR_SEG = seg = CPU_INST_SEGREG_INDEX;
	FPU_DATAPTR_OFFSET = address;
	cpu_vmemorywrite_b(seg, address, value);
#if 1
	TRACEOUT(("MWR_B: %02x:%08x <- %02x", seg, address, value));
#endif
}

void MEMCALL
fpu_memorywrite_w(UINT32 address, UINT16 value)
{
	UINT16 seg;

	FPU_DATAPTR_SEG = seg = CPU_INST_SEGREG_INDEX;
	FPU_DATAPTR_OFFSET = address;
	cpu_vmemorywrite_w(seg, address, value);
#if 1
	TRACEOUT(("MWR_W: %02x:%08x <- %04x", seg, address, value));
#endif
}

void MEMCALL
fpu_memorywrite_d(UINT32 address, UINT32 value)
{
	UINT16 seg;

	FPU_DATAPTR_SEG = seg = CPU_INST_SEGREG_INDEX;
	FPU_DATAPTR_OFFSET = address;
	cpu_vmemorywrite_d(seg, address, value);
#if 1
	TRACEOUT(("MWR_D: %02x:%08x <- %08x", seg, address, value));
#endif
}

void MEMCALL
fpu_memorywrite_q(UINT32 address, UINT64 value)
{
	UINT16 seg;

	FPU_DATAPTR_SEG = seg = CPU_INST_SEGREG_INDEX;
	FPU_DATAPTR_OFFSET = address;
	cpu_vmemorywrite_q(seg, address, value);
#if 1
	TRACEOUT(("MWR_Q: %02x:%08x <- %016lx", seg, address, value));
#endif
}

void MEMCALL
fpu_memorywrite_f(UINT32 address, REG80 *value)
{
	UINT16 seg;

	FPU_DATAPTR_SEG = seg = CPU_INST_SEGREG_INDEX;
	FPU_DATAPTR_OFFSET = address;
	cpu_vmemorywrite_f(seg, address, value);
#if 1
	TRACEOUT(("MWR_F: %02x:%08x <- %s", seg, address, fpu_REG80_str(*value)));
#endif
}

void MEMCALL
fpu_memorywrite_f32(UINT32 address, float value)
{
	UINT16 seg;
	union {
		float f;
		UINT32 l;
	} val;

	val.f = value;

	FPU_DATAPTR_SEG = seg = CPU_INST_SEGREG_INDEX;
	FPU_DATAPTR_OFFSET = address;
	cpu_vmemorywrite_d(seg, address, val.l);
#if 1
	TRACEOUT(("MWR_F32: %02x:%08x <- %f", seg, address, value));
#endif
}

void MEMCALL
fpu_memorywrite_f64(UINT32 address, double value)
{
	UINT16 seg;
	union {
		double f;
		UINT64 q;
	} val;

	val.f = value;

	FPU_DATAPTR_SEG = seg = CPU_INST_SEGREG_INDEX;
	FPU_DATAPTR_OFFSET = address;
	cpu_vmemorywrite_q(seg, address, val.q);
#if 1
	TRACEOUT(("MWR_F64: %02x:%08x <- %f", seg, address, value));
#endif
}
#endif

void
FPU_FWAIT(void)
{
#if defined(USE_FPU)
	// FPUなしなら何もしない
	if(!(i386cpuid.cpu_feature & CPU_FEATURE_FPU)){
		return;
	}
	// タスクスイッチまたはMPでNM(デバイス使用不可例外)を発生させる
	if ((CPU_CR0 & (CPU_CR0_MP|CPU_CR0_TS))==(CPU_CR0_MP|CPU_CR0_TS)) {
		EXCEPTION(NM_EXCEPTION, 0);
	}
	
	// Check exception
	if((FPU_STATUSWORD & ~FPU_CTRLWORD) & 0x3F){
		EXCEPTION(MF_EXCEPTION, 0);
	}
#endif
}
