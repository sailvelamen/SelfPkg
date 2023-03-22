//code by luobing 2020-5-17 13:11:15
#ifndef _MYLIBRARY_H
#define _MYLIBRARY_H	
#include <Uefi.h>

VOID LibFunction(VOID);

EFI_STATUS PrintGuid (IN EFI_GUID *Guid);

UINT64 GetBaseEcam (VOID);

#endif