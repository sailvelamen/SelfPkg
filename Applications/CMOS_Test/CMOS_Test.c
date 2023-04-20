#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/UefiBootServicesTableLib.h>    //gST,gBs
#include <Library/UefiRuntimeServicesTableLib.h> //gRT

#include <stdio.h>
#include <stdlib.h>
// CMOS Library
#include <Library/MyCMOSLib.h>
#include <Library/IoLib.h>

int main(IN int Argc, IN char **Argv)
{
  EFI_TIME curTime;
  printf("Hello,this is Entry of main!(test)\n");

  gBS->Stall(2000);  // delay 2s
  // gST->RuntimeServices->GetTime(&curTime,NULL);
  gRT->GetTime(&curTime, NULL);
  printf("Current Time: %d-%d-%d %02d:%02d:%02d\n", curTime.Year, curTime.Month, curTime.Day, curTime.Hour, curTime.Minute, curTime.Second);
  gST->ConOut->OutputString(gST->ConOut, L"Test SystemTable...\n\r");

  UINT8 second = 0;  // 0x00
  UINT8 minute = 0;  // 0x02
  UINT8 hour = 0;    // 0x04
  UINT8 weekday = 0; // 0x06
  UINT8 day = 0;     // 0x07
  UINT8 month = 0;   // 0x08
  UINT8 year = 0;    // 0x09

  second = CmosRead8(0x00);
  minute = CmosRead8(0x02);
  hour = CmosRead8(0x04);
  weekday = CmosRead8(0x06);
  day = CmosRead8(0x07);
  month = CmosRead8(0x08);
  year = CmosRead8(0x09);
  printf("CMOSDATA = 20%xyear %2xmonth %2xday %xweekday %2x:%2x:%2x\n", year, month, day, weekday, hour, minute, second);
  printf("0x06(weekday) = %x\n", IoRead8(0x70));
  printf("input weekday: ");
  scanf("%x", &weekday);
  CmosWrite8(0x06, weekday);
  printf("CMOSDATA = 20%xyear %2xmonth %2xday %xweekday %2x:%2x:%2x\n", year, month, day, weekday, hour, minute, second);

  return 0;
}