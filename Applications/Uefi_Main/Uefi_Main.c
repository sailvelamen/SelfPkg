/** @file
  This is Sample of UEFI aplication. 
**/
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the application.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
Uefi_Main (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_TIME curTime;
  Print(L"Hello,this is Entry of UefiMain!(Applications)\n");

  // BootService and RuntimeService
  SystemTable->BootServices->Stall(2000);  // delay 2s
  SystemTable->RuntimeServices->GetTime(&curTime,NULL);
  Print(L"Current Time: %d-%d-%d %02d:%02d:%02d\n",curTime.Year,curTime.Month,curTime.Day,curTime.Hour,curTime.Minute,curTime.Second);
  DEBUG ((DEBUG_INFO,    "fanjixxxxxxxxxxxDEBUG_INFOxxxxxxxxxxxxx\n"));
  DEBUG ((DEBUG_VERBOSE, "fanjixxxxxxxxxxxDEBUG_VERBOSExxxxxxxxxx\n"));
  DEBUG ((DEBUG_ERROR,   "fanjixxxxxxxxxxxDEBUG_ERRORxxxxxxxxxxxx\n"));
  // SystemTable
  SystemTable->ConOut->OutputString(SystemTable->ConOut,L"Test SystemTable...\n\r");
  Print(L"\rrxxxxxxxxxxxxxxxxxxxxxxxxxxxxxn\n");
  Print(L"xxxxxxxxxxxxxxxn\nxxxxxxxxxxxxxxn\n");
  SystemTable->ConOut->OutputString(SystemTable->ConOut,L"xxxxxxxxxxxxxxxxnr\n\r");
  SystemTable->ConOut->OutputString(SystemTable->ConOut,L"xxxxxxxxxxxxxxxxrn\r\n");
  Print(L"\nxxxxxxxn\nxxxxxxxxxxxxr\r.......\n");

  Print (L"16: %#x\n", 0x1A);

  return EFI_SUCCESS;
}
