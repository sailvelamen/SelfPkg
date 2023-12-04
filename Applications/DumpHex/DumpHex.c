#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/UefiBootServicesTableLib.h>    //gST,gBs
#include <Library/UefiRuntimeServicesTableLib.h> //gRT

#include <stdio.h>
// #include <stdlib.h>

#include <Library/DebugLib.h>
#include <Library/ShellCommandLib.h>

int main (IN int Argc, IN char **Argv)
{
  printf ("DumpHex: %s\n===============================================================================\n", __func__);

  UINT64 Addr;
  UINTN  Length;

  DEBUG ((DEBUG_INFO, "Argc = %d\n", Argc));

  switch (Argc)
  {
  case 3:
    sscanf (Argv[1], "%x", &Addr);
    sscanf (Argv[2], "%x", &Length);
    printf ("Addr = 0x%08X, Length = 0x%X\n\n", Addr, Length);
    DumpHex (2, 0, Length, (VOID *)Addr);
    break;
  
  default:   
    printf ("\ne.g. DumpHex.efi 0xE0000000 0x10\n");
    break;
  }

  printf ("===============================================================================\nDumpHex: %s\n", "End!");
  return 0;
}
