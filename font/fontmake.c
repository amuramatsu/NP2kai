#include	<compiler.h>
#include	<common/bmpdata.h>
#include	<common/parts.h>
#if defined(OSLANG_EUC) || defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
#include	"oemtext.h"
#endif
#include	<dosio.h>
#include	<fontmng.h>
#include	<font/font.h>
#include	"fontdata.h"
#include	"fontmake.h"


typedef struct {
	UINT16	jis1;
	UINT16	jis2;
} JISPAIR;

static const BMPDATA fntinf = {2048, 2048, 1};
static const UINT8 fntpal[8] = {0x00,0x00,0x00,0x00, 0xff,0xff,0xff,0x00};

static const UINT8 deltable[] = {
		//     del         del         del         del         del
			0x0f, 0x5f, 0,
			0x01, 0x10, 0x1a, 0x21, 0x3b, 0x41, 0x5b, 0x5f, 0,
			0x54, 0x5f, 0,
			0x57, 0x5f, 0,
			0x19, 0x21, 0x39, 0x5f, 0,
			0x22, 0x31, 0x52, 0x5f, 0,
			0x01, 0x5f, 0,
			0x01, 0x5f, 0,
			0x01, 0x5f, 0,
			0x01, 0x5f, 0,
			0x01, 0x5f, 0,
			0x1f, 0x20, 0x37, 0x3f, 0x5d, 0x5f, 0};

static const JISPAIR jis7883[] = {
			{0x3646, 0x7421}, /* Â∞≠:Â†Ø */	{0x4b6a, 0x7422}, /* Êßô:Êßá */
			{0x4d5a, 0x7423}, /* ÈÅ•:ÈÅô */	{0x596a, 0x7424}, /* Êêñ:Áë§ */ };

static const JISPAIR jis8390[] = {
			{0x724d, 0x3033}, /* È∞∫:ÈØµ */	{0x7274, 0x3229}, /* È∂Ø:È¥¨ */
			{0x695a, 0x3342}, /* Ë†£:Ëõé */	{0x5978, 0x3349}, /* Êî™:Êíπ */
			{0x635e, 0x3376}, /* Á´à:Á´É */	{0x5e75, 0x3443}, /* ÁÅå:ÊΩÖ */
			{0x6b5d, 0x3452}, /* Ë´´:Ë´å */	{0x7074, 0x375b}, /* È†∏:È†ö */
			{0x6268, 0x395c}, /* Á§¶:Á†ø */	{0x6922, 0x3c49}, /* ËòÇ:Ëïä */
			{0x7057, 0x3f59}, /* Èù±:Èù≠ */	{0x6c4d, 0x4128}, /* Ë≥§:Ë≥é */
			{0x5464, 0x445b}, /* Â£∫:Â£∑ */	{0x626a, 0x4557}, /* Á§™:Á†∫ */
			{0x5b6d, 0x456e}, /* Ê™Æ:Ê¢º */	{0x5e39, 0x4573}, /* Êø§:Ê∂õ */
			{0x6d6e, 0x4676}, /* ÈÇá:Ëø© */	{0x6a24, 0x4768}, /* Ë†Ö:Ëùø */
			{0x5b58, 0x4930}, /* Ê™ú:Ê°ß */	{0x5056, 0x4b79}, /* ÂÑò:‰æ≠ */
			{0x692e, 0x4c79}, /* Ëó™:ËñÆ */	{0x6446, 0x4f36}, /* Á±†:ÁØ≠ */ };


static UINT16 cnvjis(UINT16 jis, const JISPAIR *tbl, UINT tblsize) {

const JISPAIR	*tblterm;

	tblterm = (JISPAIR *)(((UINT8 *)tbl) + tblsize);
	while(tbl < tblterm) {
		if (jis == tbl->jis1) {
			return(tbl->jis2);
		}
		else if (jis == tbl->jis2) {
			return(tbl->jis1);
		}
		tbl++;
	}
	return(jis);
}

static BOOL ispc98jis(UINT16 jis) {

const UINT8	*p;
	UINT	tmp;

	switch(jis >> 8) {
		case 0x22:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:
		case 0x28:
		case 0x29:
		case 0x2a:
		case 0x2b:
		case 0x2c:
		case 0x2d:
			p = deltable;
			tmp = (jis >> 8) - 0x22;
			while(tmp) {
				tmp--;
				while(*p++) { }
			}
			tmp = (jis & 0xff) - 0x20;
			while(*p) {
				if ((tmp >= (UINT)p[0]) && (tmp < (UINT)p[1])) {
					return(FALSE);
				}
				p += 2;
			}
			break;

		case 0x4f:
			tmp = jis & 0xff;
			if (tmp >= 0x54) {
				return(FALSE);
			}
			break;

		case 0x7c:
			tmp = jis & 0xff;
			if ((tmp == 0x6f) || (tmp == 0x70)) {
				return(FALSE);
			}
			break;

		case 0x2e:
		case 0x2f:
		case 0x74:
		case 0x75:
		case 0x76:
		case 0x77:
		case 0x78:
		case 0x7d:
		case 0x7e:
		case 0x7f:
			return(FALSE);
	}
	return(TRUE);
}

static void setank(UINT8 *ptr, void *fnt, UINT from, UINT to) {

	char	work[2];
	FNTDAT	dat;
const UINT8	*p;
	UINT8	*q;
	int		width;
	int		height;
	UINT8	bit;
	int		i;
#if defined(OSLANG_EUC) || defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
	OEMCHAR	oemwork[4];
#endif

	ptr += (2048 * (2048 / 8)) + from;
	work[1] = '\0';
	while(from < to) {
		work[0] = (char)from;
#if defined(OSLANG_EUC) || defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
		oemtext_sjistooem(oemwork, NELEMENTS(oemwork), work, -1);
		dat = fontmng_get(fnt, oemwork);
#else
		dat = fontmng_get(fnt, work);
#endif
		if (dat) {
			width = MIN(dat->width, 8);
			height = MIN(dat->height, 16);
			p = (UINT8 *)(dat + 1);
			q = ptr;
			while(height > 0) {
				height--;
				q -= (2048 / 8);
				bit = 0xff;
				for (i=0; i<width; i++) {
					if (p[i]) {
						bit ^= (0x80 >> i);
					}
				}
				*q = bit;
				p += dat->width;
			}
		}
		from++;
		ptr++;
	}
}

static void patchank(UINT8 *ptr, const UINT8 *fnt, UINT from) {

	int		r;
	int		y;

	ptr += (2048 * (2048 / 8)) + from;
	r = 0x20;
	do {
		y = 16;
		do {
			ptr -= (2048 / 8);
			*ptr = ~(*fnt++);
		} while(--y);
		ptr += (16 * (2048 / 8)) + 1;
	} while(--r);
}

static void setjis(UINT8 *ptr, void *fnt) {

	char	work[4];
	UINT16	h;
	UINT16	l;
	UINT16	jis;
	UINT	sjis;
	FNTDAT	dat;
const UINT8	*p;
	UINT8	*q;
	int		width;
	int		height;
	UINT16	bit;
	int		i;
#if defined(OSLANG_EUC) || defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
	OEMCHAR	oemwork[4];
#endif

	work[2] = '\0';
	ptr += ((0x80 - 0x21) * 16 * (2048 / 8)) + 2;
	for (h=0x2100; h<0x8000; h+=0x100) {
		for (l=0x21; l<0x7f; l++) {
			jis = h + l;
			if (ispc98jis(jis)) {
				jis = cnvjis(jis, jis7883, sizeof(jis7883));
				jis = cnvjis(jis, jis8390, sizeof(jis8390));
				sjis = jis2sjis(jis);
				work[0] = (UINT8)(sjis >> 8);
				work[1] = (UINT8)sjis;
#if defined(OSLANG_EUC) || defined(OSLANG_UTF8) || defined(OSLANG_UCS2)
				oemtext_sjistooem(oemwork, NELEMENTS(oemwork), work, -1);
				dat = fontmng_get(fnt, oemwork);
#else
				dat = fontmng_get(fnt, work);
#endif
				if (dat) {
					width = MIN(dat->width, 16);
					height = MIN(dat->height, 16);
					p = (UINT8 *)(dat + 1);
					q = ptr;
					while(height > 0) {
						height--;
						q -= (2048 / 8);
						bit = 0xffff;
						for (i=0; i<width; i++) {
							if (p[i]) {
								bit ^= (0x8000 >> i);
							}
						}
						q[0] = (UINT8)(bit >> 8);
						q[1] = (UINT8)bit;
						p += dat->width;
					}
				}
			}
			ptr -= 16 * (2048 / 8);
		}
		ptr += ((0x7f - 0x21) * 16 * (2048 / 8)) + 2;
	}
}

static void patchextank(UINT8 *ptr, const UINT8 *fnt, UINT pos) {

	UINT	r;

	ptr += ((0x80 - 0x21) * 16 * (2048 / 8)) + (pos * 2);
	r = 0x5e * 16;
	do {
		ptr -= (2048 / 8);
		*ptr = ~(*fnt++);
	} while(--r);
}

static void patchextfnt(UINT8 *ptr, const UINT8 *fnt) {			// 2c24-2c6f

	UINT	r;

	ptr += ((0x80 - 0x24) * 16 * (2048 / 8)) + (0x0c * 2);
	r = 0x4c * 16;
	do {
		ptr -= (2048 / 8);
		ptr[0] = (UINT8)(~fnt[0]);
		ptr[1] = (UINT8)(~fnt[1]);
		fnt += 2;
	} while(--r);
}

#define COPYGLYPH_LEFTPART		0x1		// 16 -> 8ÇÃÇ∆Ç´èkè¨ÇπÇ∏ç∂îºï™ÇÉRÉsÅ[
#define COPYGLYPH_RIGHTPART		0x2		// 16 -> 8ÇÃÇ∆Ç´èkè¨ÇπÇ∏âEîºï™ÇÉRÉsÅ[
#define COPYGLYPH_ADJUSTPART	0x4		// 16 -> 8ÇÃÇ∆Ç´ÉfÅ[É^Ç™Ç†ÇÈXç¿ïWÇãNì_Ç…ÉRÉsÅ[
#define COPYGLYPH_ADJUSTPARTR	0x8		// 16 -> 8ÇÃÇ∆Ç´ÉfÅ[É^Ç™Ç†ÇÈXç¿ïWÇãNì_Ç…ÉRÉsÅ[ÅiâEëµÇ¶Åj
#define COPYGLYPH_WITHVMARK		0x10	// ÉRÉsÅ[å„Ç…ë˜ì_ÇïtÇØÇÈ
#define COPYGLYPH_WITHPMARK		0x20	// ÉRÉsÅ[å„Ç…îºë˜ì_ÇïtÇØÇÈ
#define COPYGLYPH_REVERSEXSH1	0x40	// ç∂âEîΩì]ÇµÇƒ1pxÉVÉtÉgÇ∑ÇÈ
#define COPYGLYPH_7TO6			0x80	// ïùÇ1pxèkè¨

// ï∂éöÉRÉsÅ[
static void copyglyph(UINT8* ptr, UINT srccode, int srcwidth, UINT dstcode, int dstwidth, int copyflag) {

	int i;
	int stride = 2048 / 8;
	int stridedst = 2048 / 8;
	UINT8 reversebuf[16 * 2];
	UINT8* srcptr;
	UINT8* dstptr;
	UINT8* dstptrbase;
	if (srccode <= 0xff) {
		srcptr = ptr + (2048 / 16 - 1) * (2048 / 8 * 16) + srccode;
	}
	else {
		int jisy = (srccode & 0xff);
		int jisx = ((srccode >> 8) & 0xff) - 0x20;
		srcptr = ptr + (2048 / 16 - 1 - jisy) * (2048 / 8 * 16) + jisx * 2;
	}
	if (dstcode <= 0xff) {
		dstptr = ptr + (2048 / 16 - 1) * (2048 / 8 * 16) + dstcode;
	}
	else {
		int jisy = (dstcode & 0xff);
		int jisx = ((dstcode >> 8) & 0xff) - 0x20;
		dstptr = ptr + (2048 / 16 - 1 - jisy) * (2048 / 8 * 16) + jisx * 2;
	}
	dstptrbase = dstptr;
	if (copyflag & COPYGLYPH_REVERSEXSH1) {
		memset(reversebuf, 0xff, sizeof(reversebuf));
		if (srcwidth == 8) {
			UINT8 tmp;
			UINT8 bitstate = 0xff;
			for (i = 0; i < 16; i++) {
				tmp = *srcptr;
				tmp = ((tmp & 0x0f) << 4) | ((tmp >> 4) & 0x0f);
				tmp = ((tmp & 0x33) << 2) | ((tmp >> 2) & 0x33);
				tmp = ((tmp & 0x55) << 1) | ((tmp >> 1) & 0x55);
				bitstate &= tmp;
				reversebuf[i * 2] = tmp;
				srcptr += stride;
			}
			if (bitstate & 0x01) {
				// âEí[Ç™ãÛÇ¢ÇƒÇ¢ÇÍÇŒ1pxÇ∏ÇÁÇ∑
				for (i = 0; i < 16; i++) {
					*(UINT8*)(reversebuf + i * 2) = (*(UINT8*)(reversebuf + i * 2) >> 1) | 0x80;
				}
			}
		}
		else if (srcwidth == 16) {
			UINT16 tmp;
			UINT16 bitstate = 0xffff;
			for (i = 0; i < 16; i++) {
				tmp = *((UINT16*)srcptr);
				tmp = ((tmp & 0x00ff) << 8) | ((tmp >> 8) & 0x00ff);
				tmp = ((tmp & 0x0f0f) << 4) | ((tmp >> 4) & 0x0f0f);
				tmp = ((tmp & 0x3333) << 2) | ((tmp >> 2) & 0x3333);
				tmp = ((tmp & 0x5555) << 1) | ((tmp >> 1) & 0x5555);
				bitstate &= tmp;
				reversebuf[i * 2] = (tmp >> 8) & 0xff;
				reversebuf[i * 2 + 1] = tmp & 0xff;
				srcptr += stride;
			}
			if (bitstate & 0x0001) {
				// âEí[Ç™ãÛÇ¢ÇƒÇ¢ÇÍÇŒ1pxÇ∏ÇÁÇ∑
				for (i = 0; i < 16; i++) {
					*((UINT16*)(reversebuf + i * 2)) = (*((UINT16*)(reversebuf + i * 2)) >> 1) | 0x8000;
				}
			}
		}
		srcptr = reversebuf;
		stride = 2;
	}
	if (srcwidth == 8 && dstwidth == 8) {
		// îºäpÅ®îºäp
		for (i = 0; i < 16; i++) {
			*dstptr = *srcptr;
			srcptr += stride;
			dstptr += stridedst;
		}
	}
	else if (srcwidth == 16 && dstwidth == 16) {
		// ëSäpÅ®ëSäp
		for (i = 0; i < 16; i++) {
			*dstptr = *srcptr;
			*(dstptr + 1) = *(srcptr + 1);
			srcptr += stride;
			dstptr += stridedst;
		}
	}
	else if (srcwidth == 8 && dstwidth == 16) {
		// îºäpÅ®ëSäpÅiâEë§ãÛîíÅj
		for (i = 0; i < 16; i++) {
			*dstptr = *srcptr;
			*(dstptr + 1) = 0xff;
			srcptr += stride;
			dstptr += stridedst;
		}
	}
	else if (srcwidth == 16 && dstwidth == 8) {
		// ëSäpÅ®îºäp
		if (copyflag & COPYGLYPH_LEFTPART) {
			// ç∂ë§ÇÃÇ›
			for (i = 0; i < 16; i++) {
				*dstptr = *srcptr;
				srcptr += stride;
				dstptr += stridedst;
			}
		}
		else if(copyflag & COPYGLYPH_RIGHTPART) {
			// âEë§ÇÃÇ›
			for (i = 0; i < 16; i++) {
				*dstptr = *(srcptr + 1);
				srcptr += stride;
				dstptr += stridedst;
			}
		}
		else if (copyflag & (COPYGLYPH_ADJUSTPART | COPYGLYPH_ADJUSTPARTR)) {
			// ÉfÅ[É^Ç™Ç†ÇÈXç¿ïWÇãNì_
			int beginX = 0;
			int endX = 0;
			UINT16 bitstate = 0xffff;
			UINT16 bitstateTmp;
			UINT8* srcptr2 = srcptr;
			for (i = 0; i < 16; i++) {
				bitstate &= (UINT16)(*srcptr2) << 8 | *(srcptr2 + 1);
				srcptr2 += stride;
			}
			bitstateTmp = bitstate;
			for (beginX = 0; beginX < 16; beginX++) {
				if (!(bitstateTmp & 0x8000)) break;
				bitstateTmp <<= 1;
			}
			endX = 16;
			bitstateTmp = bitstate;
			for (endX = 16; endX > beginX; endX--) {
				if (!(bitstateTmp & 0x1)) break;
				bitstateTmp >>= 1;
			}
			if (copyflag & COPYGLYPH_ADJUSTPARTR) {
				beginX -= 8 - (endX - beginX); // âEëµÇ¶Ç…Ç∑ÇÈ
				if (beginX < 0) beginX = 0;
			}
			else {
				if (endX - beginX < 8 && beginX > 0) beginX--; // àÍî‘ç∂í[ÇÕîÇØÇÈ
			}
			for (i = 0; i < 16; i++) {
				UINT8 src1 = *srcptr;
				UINT8 src2 = *(srcptr + 1);
				if (beginX < 8) {
					src1 = src1 << beginX;
					src1 |= src2 >> (8 - beginX);
				}
				else {
					src1 = src2 << (beginX - 8);
					src1 |= 0xff >> (8 - (beginX - 8));
				}
				*dstptr = src1;
				srcptr += stride;
				dstptr += stridedst;
			}
		}
		else {
			// îºïùâª
			for (i = 0; i < 16; i++) {
				UINT8 src1 = *srcptr;
				UINT8 src2 = *(srcptr + 1);
				src1 &= src1 >> 1;
				src1 &= 0x55;
				src2 &= src2 >> 1;
				src2 &= 0x55;
				*dstptr = ((src1 << 1) & 0x80) | ((src1 << 2) & 0x40) | ((src1 << 3) & 0x20) | ((src1 << 4) & 0x10) |
					((src2 >> 3) & 0x08) | ((src2 >> 2) & 0x04) | ((src2 >> 1) & 0x02) | ((src2 >> 0) & 0x01);
				srcptr += stride;
				dstptr += stridedst;
			}
		}
	}
	dstptr = dstptrbase;
	if (copyflag & COPYGLYPH_WITHVMARK) {
		dstptr += stridedst * 14;
		if (dstwidth == 16) {
			dstptr++; // âEë§Ç…Ç¬ÇØÇÈ
		}
		*dstptr &= 0xfa;
		*dstptr |= 0x02;
		dstptr += stridedst;
		*dstptr &= 0xfa;
		*dstptr |= 0x02;
	}
	else if (copyflag & COPYGLYPH_WITHPMARK) {
		dstptr += stridedst * 12;
		if (dstwidth == 16) {
			dstptr++; // âEë§Ç…Ç¬ÇØÇÈ
		}
		*dstptr &= 0xfd;
		*dstptr |= 0x05;
		dstptr += stridedst;
		*dstptr &= 0xfa;
		*dstptr |= 0x02;
		dstptr += stridedst;
		*dstptr &= 0xfa;
		*dstptr |= 0x02;
		dstptr += stridedst;
		*dstptr &= 0xfd;
		*dstptr |= 0x05;
	}
	dstptr = dstptrbase;
	if (copyflag & COPYGLYPH_7TO6) {
		memset(reversebuf, 0xff, sizeof(reversebuf));
		if (dstwidth == 8) {
			UINT8 tmp;
			UINT8 bitstate = 0xff;
			UINT8 bitsame = 0x00;
			for (i = 0; i < 16; i++) {
				tmp = *dstptr;
				bitstate &= tmp;
				bitsame |= tmp ^ (tmp << 1);
				dstptr += stridedst;
			}
			if (bitstate & 0x01) {
				// âEí[Ç™ãÛÇ¢ÇƒÇ¢ÇÍÇŒâΩÇ‡ÇµÇ»Ç≠ÇƒÇÊÇ¢
			}
			else if ((bitstate & 0xc0) == 0xc0) {
				// ç∂í[Ç™2pxãÛÇ¢ÇƒÇ¢ÇÍÇŒç∂Ç÷1pxÇ∏ÇÁÇ∑
				dstptr = dstptrbase;
				for (i = 0; i < 16; i++) {
					*dstptr = (*dstptr << 1) | 0x01;
					dstptr += stridedst;
				}
			}
			else {
				// íÜâõÇ…ìØÇ∂ì‡óeÇ™òAë±ÇµÇƒÇ¢ÇΩÇÁÇªÇ±ÇçÌÇÈ
				dstptr = dstptrbase;
				if (!(bitsame & 0x08)) {
					for (i = 0; i < 16; i++) {
						tmp = *dstptr;
						tmp = (tmp & 0xf0) | ((tmp & 0x7) << 1) | 0x01;
						*dstptr = tmp;
						dstptr += stridedst;
					}
				}
				else if (!(bitsame & 0x10)) {
					for (i = 0; i < 16; i++) {
						tmp = *dstptr;
						tmp = (tmp & 0xe0) | ((tmp & 0xf) << 1) | 0x01;
						*dstptr = tmp;
						dstptr += stridedst;
					}
				}
			}
		}
		else if (srcwidth == 16) {
			// ñ¢é¿ëï
		}
	}
}

void makepc98bmp(const OEMCHAR *filename, const OEMCHAR* fontface) {

	void	*fnt;
	BMPFILE	bf;
	UINT	size;
	BMPINFO	bi;
	UINT8	*ptr;
	FILEH	fh;
	BOOL	r;
	int		i;

#if defined(FDAT_SHIFTJIS)
	fnt = fontmng_create(16, FDAT_SHIFTJIS, fontface);
#else
	fnt = fontmng_create(16, 0, fontface);
#endif
	if (fnt == NULL) {
		goto mfnt_err1;
	}
	size = bmpdata_setinfo(&bi, &fntinf);
	bmpdata_sethead(&bf, &bi);
	ptr = (UINT8 *)_MALLOC(size, filename);
	if (ptr == NULL) {
		goto mfnt_err2;
	}
	FillMemory(ptr, size, 0xff);
	setank(ptr, fnt, 0x20, 0x7f);
	setank(ptr, fnt, 0xa1, 0xe0);
	patchank(ptr, fontdata_16 + 0*32*16, 0x00);
	patchank(ptr, fontdata_16 + 1*32*16, 0x80);
	patchank(ptr, fontdata_16 + 2*32*16, 0xe0);
	setjis(ptr, fnt);
	patchextank(ptr, fontdata_2b, 0x0b);
	patchextfnt(ptr, fontdata_2c);
#ifdef USE_BUILTIN_FONT
	// ÇÀÇ±Å[ì‡ë†ë„ë÷ÉtÉHÉìÉg
	patchextank(ptr, fontdata_29, 0x09);
	patchextank(ptr, fontdata_2a, 0x0a);
#else
	// éwíËÉtÉHÉìÉgÇ©ÇÁã[éóìIÇ…ê∂ê¨
	copyglyph(ptr, 0x315f, 16, 0xf1, 8, 0); // â~
	copyglyph(ptr, 0x472f, 16, 0xf2, 8, 0); // îN
	copyglyph(ptr, 0x376e, 16, 0xf3, 8, 0); // åé
	copyglyph(ptr, 0x467c, 16, 0xf4, 8, 0); // ì˙
	copyglyph(ptr, 0x3b7e, 16, 0xf5, 8, 0); // éû
	copyglyph(ptr, 0x4a2c, 16, 0xf6, 8, 0); // ï™
	copyglyph(ptr, 0x4943, 16, 0xf7, 8, 0); // ïb
	for (i = 0; i < 0x7f - 0x21; i++) {
		copyglyph(ptr, 0x21 + i, 8, 0x2921 + i, 8, 0); // êîéöâpéöÇ»Ç«
	}
	for (i = 0x20; i < 0x39; i++) {
		copyglyph(ptr, 0x21 + i, 8, 0x2921 + i, 8, COPYGLYPH_7TO6); // asciiïîï™ÇæÇØâ¬î\Ç»ÇÁã∑ïùÇ…Ç∑ÇÈ
	}
	for (i = 0x40; i < 0x59; i++) {
		copyglyph(ptr, 0x21 + i, 8, 0x2921 + i, 8, COPYGLYPH_7TO6); // asciiïîï™ÇæÇØâ¬î\Ç»ÇÁã∑ïùÇ…Ç∑ÇÈ
	}
	for (i = 0; i < 0x60 - 0x21; i++) {
		copyglyph(ptr, 0xa1 + i, 8, 0x2a21 + i, 8, 0); // ÉJÉiÇ»Ç«
	}
	copyglyph(ptr, 0x2570, 16, 0x2a60, 8, 0); // Éê
	copyglyph(ptr, 0x2571, 16, 0x2a61, 8, 0); // Éë
	copyglyph(ptr, 0x256e, 16, 0x2a62, 8, 0); // Éé
	copyglyph(ptr, 0x2575, 16, 0x2a63, 8, 0); // Éï
	copyglyph(ptr, 0x2576, 16, 0x2a64, 8, 0); // Éñ
	copyglyph(ptr, 0xb3, 8, 0x2a65, 8, COPYGLYPH_WITHVMARK); // Éî
	for (i = 0; i < 15; i++) {
		// ÉKçsÅAÉUçsÅAÉ_çs
		copyglyph(ptr, 0xb6 + i, 8, 0x2a66 + i, 8, COPYGLYPH_WITHVMARK);
	}
	for (i = 0; i < 5; i++) {
		// ÉoçsÅAÉpçs
		copyglyph(ptr, 0xca + i, 8, 0x2a75 + i * 2, 8, COPYGLYPH_WITHVMARK);
		copyglyph(ptr, 0xca + i, 8, 0x2a75 + i * 2 + 1, 8, COPYGLYPH_WITHPMARK);
	}
	copyglyph(ptr, 0x214c, 16, 0x2b74, 8, COPYGLYPH_ADJUSTPARTR); // Åk
	copyglyph(ptr, 0x214d, 16, 0x2b75, 8, COPYGLYPH_ADJUSTPART); // Ål
	copyglyph(ptr, 0x2152, 16, 0x2b76, 8, COPYGLYPH_ADJUSTPARTR); // Åq
	copyglyph(ptr, 0x2153, 16, 0x2b77, 8, COPYGLYPH_ADJUSTPART); // År
	copyglyph(ptr, 0x2154, 16, 0x2b78, 8, COPYGLYPH_ADJUSTPARTR); // Ås
	copyglyph(ptr, 0x2155, 16, 0x2b79, 8, COPYGLYPH_ADJUSTPART); // Åt
	copyglyph(ptr, 0x2158, 16, 0x2b7a, 8, COPYGLYPH_ADJUSTPARTR); // Åw
	copyglyph(ptr, 0x2159, 16, 0x2b7b, 8, COPYGLYPH_ADJUSTPART); // Åx
	copyglyph(ptr, 0x215a, 16, 0x2b7c, 8, COPYGLYPH_ADJUSTPARTR); // Åy
	copyglyph(ptr, 0x215b, 16, 0x2b7d, 8, COPYGLYPH_ADJUSTPART); // Åz
	copyglyph(ptr, '-', 8, 0x2b7e, 8, 0); // -
	copyglyph(ptr, '/', 8, 0xfc, 8, COPYGLYPH_REVERSEXSH1); // ÉoÉbÉNÉXÉâÉbÉVÉÖ
#endif

	fh = file_create(filename);
	if (fh == FILEH_INVALID) {
		goto mfnt_err3;
	}
	r = (file_write(fh, &bf, sizeof(bf)) == sizeof(bf)) &&
		(file_write(fh, &bi, sizeof(bi)) == sizeof(bi)) &&
		(file_write(fh, fntpal, sizeof(fntpal)) == sizeof(fntpal)) &&
		(file_write(fh, ptr, size) == size);
	file_close(fh);
	if (!r) {
		file_delete(filename);
	}

mfnt_err3:
	_MFREE(ptr);

mfnt_err2:
	fontmng_destroy(fnt);

mfnt_err1:
	return;
}

