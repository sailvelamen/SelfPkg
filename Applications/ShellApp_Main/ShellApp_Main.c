/** @file
  This is Sample of UEFI aplication. 
**/
#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/ShellCEntryLib.h>

#include <Library/UefiBootServicesTableLib.h>  //gST,gBs
#include <Library/UefiRuntimeServicesTableLib.h> //gRT
/***
  Print a welcoming message.

  Establishes the main structure of the application.

  @retval  0         The application exited normally.
  @retval  Other     An error occurred.
***/
INTN
EFIAPI
ShellAppMain (
  IN UINTN Argc,    // 参数的数量
  IN CHAR16 **Argv  // 参数数组
  )
{
  EFI_TIME curTime;
  Print(L"Hello,this is Entry of ShellAppMain!(Applications)\n");
  
  Print(L"Total: %d parameters\n",Argc);
  for (short i = 0; i < Argc; i++)
  {
    Print(L"parameters %d: %s\n", i+1, Argv[i]);
  }
  

  //使用BootService和RuntimeService
  // gST->BootServices->Stall(2000);  //延时2秒
  gBS->Stall(2000);
  // gST->RuntimeServices->GetTime(&curTime,NULL);
  gRT->GetTime(&curTime,NULL);
  Print(L"Current Time: %d-%d-%d %02d:%02d:%02d\n",curTime.Year,curTime.Month,curTime.Day,curTime.Hour,curTime.Minute,curTime.Second);

  //使用SystemTable
  gST->ConOut->OutputString(gST->ConOut,L"Test SystemTable...\n\r");

  return(0);
}
