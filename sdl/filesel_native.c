#include	<compiler.h>
#include	"np2.h"
#include	<dosio.h>
#include	<codecnv/codecnv.h>
#include	<fdd/diskdrv.h>
#include	<embed/menu/filesel.h>
#include	<nfd.h>
#include	<nfd_sdl2.h>

#ifdef USE_NATIVEFILEDIALOG

extern BOOL nfd_initialized;
extern SDL_Window* s_window;

extern char fddfolder[MAX_PATH];
extern char hddfolder[MAX_PATH];

void
filesel_fdd_native(REG8 drv)
{
    nfdu8char_t *outPath;
	nfdopendialogu8args_t args = {0};
	nfdu8filteritem_t filters[] = {
		{ "FDD Image", "d88,88d,d98,98d,fdi,xdf,hdm,dup,2hd,tfd,nfd,hd4,hd5,hd9,fdd,h01,hdb,ddb,dd6,dcp,dcu,flp,bin,fim,img,ima" },
		{ "All", "*" },
	};
	nfdresult_t result;
#ifdef _WINDOWS
	char fddfolder_u8[MAX_PATH];
	char outPath_SJ[MAX_PATH];
#endif

	if (! nfd_initialized) {
		filesel_fdd(drv);
		return;
	}
	NFD_GetNativeWindowFromSDLWindow(s_window, &args.parentWindow);
	
	args.filterList = filters;
	args.filterCount = 2;
#ifdef _WINDOWS
	codecnv_sjistoutf8(fddfolder_u8, sizeof(fddfolder_u8),
					   fddfolder, strlen(fddfolder));
	args.defaultPath = (nfdu8char_t *)fddfolder_u8;
#else
	args.defaultPath = (nfdu8char_t *)fddfolder;
#endif
	result = NFD_OpenDialogU8_With(&outPath, &args);
	if (result == NFD_OKAY) {
#ifdef _WINDOWS
		codecnv_utf8tosjis(outPath_SJ, sizeof(outPath_SJ),
						   outPath, strlen(outPath));
		file_cpyname(fddfolder, outPath_SJ, NELEMENTS(fddfolder));
		file_cutname(fddfolder);
		diskdrv_setfdd(drv, outPath_SJ, 0);
#else
		file_cpyname(fddfolder, outPath, NELEMENTS(fddfolder));
		file_cutname(fddfolder);
		diskdrv_setfdd(drv, outPath, 0);
#endif
		NFD_FreePathU8(outPath);
	}
}

void
filesel_hdd_native(REG8 drv)
{
    nfdu8char_t *outPath;
	nfdopendialogu8args_t args = {0};
	nfdu8filteritem_t filters[] = {
		{ "HDD/CD Image", "thd,nhd,hdi,vhd,slh,hdn,hdd,iso,cue,ccd,cdm,mds,nrg" },
		{ "All", "*" },
	};
	nfdresult_t result;
#ifdef _WINDOWS
	char hddfolder_u8[MAX_PATH];
	char outPath_SJ[MAX_PATH];
#endif

	if (! nfd_initialized) {
		filesel_hdd(drv);
		return;
	}
	NFD_GetNativeWindowFromSDLWindow(s_window, &args.parentWindow);	
	args.filterList = filters;
	args.filterCount = 2;
#ifdef _WINDOWS
	codecnv_sjistoutf8(hddfolder_u8, sizeof(hddfolder_u8),
					   hddfolder, strlen(hddfolder));
	args.defaultPath = (nfdu8char_t *)hddfolder_u8;
#else
	args.defaultPath = (nfdu8char_t *)hddfolder;
#endif
	result = NFD_OpenDialogU8_With(&outPath, &args);
	if (result == NFD_OKAY) {
#ifdef _WINDOWS
		codecnv_utf8tosjis(outPath_SJ, sizeof(outPath_SJ),
						   outPath, strlen(outPath));
		file_cpyname(fddfolder, outPath_SJ, NELEMENTS(fddfolder));
		file_cutname(fddfolder);
		diskdrv_setsxsi(drv, outPath_SJ);
#else
		file_cpyname(fddfolder, outPath, NELEMENTS(fddfolder));
		file_cutname(fddfolder);
		diskdrv_setsxsi(drv, outPath);
#endif
		NFD_FreePathU8(outPath);
	}
}

#endif /* USE_NATIVEFILEDIALOG */
