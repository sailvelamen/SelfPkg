#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/UefiBootServicesTableLib.h>    //gST,gBs
#include <Library/UefiRuntimeServicesTableLib.h> //gRT

#include <Library/DebugLib.h>
#include <Library/ShellCommandLib.h>

// #include <stdio.h>
// #include <stdlib.h>

/***
  Print a welcoming message.

  Establishes the main structure of the application.

  @retval  0         The application exited normally.
  @retval  Other     An error occurred.
***/
INTN
EFIAPI
ShellAppMain (
  IN UINTN Argc,
  IN CHAR16 **Argv
  )
{
  Print (L"DumpHex: %a\n===============================================================================\n", __FUNCTION__);

  UINT64 Addr;
  UINTN  Length;

  DEBUG ((DEBUG_INFO, "Argc = %d\n", Argc));

  switch (Argc)
  {
  case 3:
    Addr   = StrHexToUint64 (Argv[1]);
    Length = StrHexToUint64 (Argv[2]);
    Print (L"Addr = 0x%08X, Length = 0x%X\n\n", Addr, Length);
    DumpHex (2, 0, Length, (VOID *)Addr);
    break;
  
  default:   
    Print (L"\ne.g. DumpHex.efi 0xE0000000 0x10\n");
    break;
  }

  Print (L"===============================================================================\nDumpHex: %s\n", L"End!");
  return 0;
}
