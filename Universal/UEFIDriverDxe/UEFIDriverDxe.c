#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Pi/PiHob.h>  // GET_GUID_HOB_DATA
#include <Guid/MyHob.h>

EFI_STATUS
EFIAPI
UEFIDriverDxeEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  DEBUG ((DEBUG_ERROR, "\nfanjixxxxxxxxxxxUEFIDriverDxeEntryPointxxxxxxxxxxxxEntry\n"));
  EFI_TIME curTime;
  //使用BootService和RuntimeService
  SystemTable->BootServices->Stall(2000);  //延时2秒
  SystemTable->RuntimeServices->GetTime(&curTime,NULL);
  DEBUG ((DEBUG_ERROR, "fanjix: Current Time: %d-%d-%d %02d:%02d:%02d\n",
          curTime.Year, curTime.Month, curTime.Day, curTime.Hour, curTime.Minute, curTime.Second));

  DEBUG ((DEBUG_ERROR, "fanjixxxxxxxxxxxGetHobxxxxxxxxxxxstart\n"));
  UINT8  *GuidHob = GetFirstGuidHob (&gEfiSelfHobGuid);
  ASSERT (GuidHob != NULL);
  EFI_SELF_HOB *MySelfHob = (EFI_SELF_HOB *)(*(UINTN *)(GET_GUID_HOB_DATA (GuidHob)));
  DEBUG ((DEBUG_INFO, "fanjix: MySelfHob->ID = 0x%X\n", MySelfHob->ID));
  DEBUG ((DEBUG_INFO, "fanjix: MySelfHob->Mesg = %s\n", MySelfHob->Mesg));
  DEBUG ((DEBUG_ERROR, "fanjixxxxxxxxxxxGetHobxxxxxxxxxxxxxend\n"));

  DEBUG ((DEBUG_ERROR, "fanjixxxxxxxxxxxUEFIDriverDxeEntryPointxxxxxxxxxxxxEnd\n\n"));
  return EFI_SUCCESS;
}
