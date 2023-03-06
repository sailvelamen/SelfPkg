#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>

EFI_STATUS
EFIAPI
UEFIDriverDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  DEBUG ((DEBUG_ERROR, "fanjixxxxxxxxxxxUEFIDriverDxeEntryPointxxxxxxxxxxxxEntry\n"));
  EFI_TIME curTime;
  //使用BootService和RuntimeService
  SystemTable->BootServices->Stall(2000);  //延时2秒
  SystemTable->RuntimeServices->GetTime(&curTime,NULL);
  DEBUG ((DEBUG_ERROR, "Current Time: %d-%d-%d %02d:%02d:%02d\n",
          curTime.Year, curTime.Month, curTime.Day, curTime.Hour, curTime.Minute, curTime.Second));

  DEBUG ((DEBUG_ERROR, "fanjixxxxxxxxxxxGetHobxxxxxxxxxxxstart\n"));
  extern EFI_GUID  gEfiSelfHobGuid;
  UINT8  *GuidHob = GetFirstGuidHob (&gEfiSelfHobGuid);
  ASSERT (GuidHob != NULL);
  DEBUG ((DEBUG_ERROR, "fanjixxxxxxxxxxxGetHobxxxxxxxxxxxxxend\n"));

  DEBUG ((DEBUG_ERROR, "fanjixxxxxxxxxxxUEFIDriverDxeEntryPointxxxxxxxxxxxxEnd\n"));
  return EFI_SUCCESS;
}
