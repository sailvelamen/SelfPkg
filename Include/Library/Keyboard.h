//code by luobing  
#ifndef _KEYBOARD_H
#define _KEYBOARD_H
#include  <Uefi.h>
// #include  <Library/UefiLib.h>
// // #include  <Library/ShellCEntryLib.h>
// // #include  <Library/DebugLib.h>

// #include <Library/BaseMemoryLib.h>

// #include <Library/UefiBootServicesTableLib.h>  //gST,gBS
// #include <Library/BaseLib.h>


EFI_STATUS GetKeyEx(UINT16 *ScanCode, UINT16 *UniChar, UINT32 *ShiftState, EFI_KEY_TOGGLE_STATE * ToggleState);
EFI_STATUS GetKey(EFI_INPUT_KEY *key);
VOID FlushKeyBuffer(VOID);
VOID WaitKey(VOID);

#endif