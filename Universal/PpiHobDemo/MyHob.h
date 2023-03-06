#ifndef __MYHOB_GUID_H__
#define __MYHOB_GUID_H__

#include <Uefi.h>

#define EFI_SELF_HOB_GUID { 0xb48cb6fb, 0x13db, 0xc88d, { 0xe2, 0x96, 0xcb, 0x42, 0xf7, 0xa6, 0xce, 0x36 }}

extern EFI_GUID  gEfiSelfHobGuid;

typedef struct {
  UINT32    Type;   
  GUID      *Signature;        
  CHAR16    *Mesg;
} EFI_SELF_HOB;

#endif