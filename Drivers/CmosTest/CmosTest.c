#include <Uefi.h>
#include <Library/UefiLib.h>

// CMOS Library
#include <Library/MyCMOSLib.h>

EFI_STATUS 
EFIAPI 
CmosTestEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_TIME curTime;
  Print(L"Hello,this is Entry of UefiMain!(test)\n");

  //使用BootService和RuntimeService
  SystemTable->BootServices->Stall(2000);  //延时2秒
  SystemTable->RuntimeServices->GetTime(&curTime,NULL);
  Print(L"Current Time: %d-%d-%d %02d:%02d:%02d\n",curTime.Year,curTime.Month,curTime.Day,curTime.Hour,curTime.Minute,curTime.Second);
  //使用SystemTable
  SystemTable->ConOut->OutputString(SystemTable->ConOut,L"Test SystemTable...\n\r");

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
  Print(L"CMOSDATA = 20%xyear %2xmonth %2xday %xweekday %2x:%2x:%2x\n", year, month, day, weekday, hour, minute, second);
  weekday=0x44;
  Print(L"weekday: %x\n", weekday);
  // weekday = CmosWrite8(0x06, weekday);
  CmosWrite8(0x06, weekday);
  Print(L"CMOSDATA = 20%xyear %2xmonth %2xday %xweekday %2x:%2x:%2x\n", year, month, day, weekday, hour, minute, second);

  return 0;
}