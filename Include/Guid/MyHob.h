#ifndef __MYHOB_GUID_H__
#define __MYHOB_GUID_H__

#include <Uefi.h>

#define EFI_SELF_HOB_GUID { 0x3E0481ED, 0xF593, 0x6976, { 0x43, 0xD9, 0x06, 0x10, 0x0E, 0xE4, 0x34, 0xA0 }}

// EFI_GUID  gEfiSelfHobGuid;

typedef struct {        
  UINT16    ID;
  UINT16    Type;
  CHAR16    Mesg[12];
} EFI_SELF_HOB;

#endif