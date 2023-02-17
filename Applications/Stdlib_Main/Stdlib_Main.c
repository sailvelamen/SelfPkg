#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/UefiBootServicesTableLib.h>    //gST,gBs
#include <Library/UefiRuntimeServicesTableLib.h> //gRT

#include <stdio.h>
// #include <stdlib.h>

#include <Library/DebugLib.h>


int main (IN int Argc, IN char **Argv)
{
  printf("Hello,this is Entry of main!()\n");
  DEBUG ((EFI_D_ERROR, "fanjixxxxxxxxxxxxxxxxxx\n"));
  DEBUG ((DEBUG_ERROR, "fanjixxxxxxxxxxxxxxxxxx\n"));
  printf("16: %#x\n", 0x1A);

  return 0;
}

