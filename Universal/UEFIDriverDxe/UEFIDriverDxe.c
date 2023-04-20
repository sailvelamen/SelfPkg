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
  // BootService and RuntimeService
  SystemTable->BootServices->Stall(2000);  //delay 2s
  SystemTable->RuntimeServices->GetTime(&curTime,NULL);
  DEBUG ((DEBUG_ERROR, "fanjix: Current Time: %d-%d-%d %02d:%02d:%02d\n",
          curTime.Year, curTime.Month, curTime.Day, curTime.Hour, curTime.Minute, curTime.Second));

  DEBUG ((DEBUG_ERROR, "fanjixxxxxxxxxxxGetHobxxxxxxxxxxxstart\n"));
  EFI_HOB_GUID_TYPE  *GuidHob = GetFirstGuidHob (&gEfiSelfHobGuid);
  ASSERT (GuidHob != NULL);
  EFI_SELF_HOB *MySelfHob = GET_GUID_HOB_DATA (GuidHob);
  DEBUG ((DEBUG_INFO, "fanjix: MySelfHob->ID = 0x%X\n", MySelfHob->ID));
  DEBUG ((DEBUG_INFO, "fanjix: MySelfHob->Type = %d\n", MySelfHob->Type));
  DEBUG ((DEBUG_INFO, "fanjix: MySelfHob->Mesg = %s\n", MySelfHob->Mesg));
  DEBUG ((DEBUG_INFO, "fanjix: sizeof (MySelfHob) = %d\n", sizeof (*MySelfHob)));
  DEBUG ((DEBUG_ERROR, "fanjixxxxxxxxxxxGetHobxxxxxxxxxxxxxend\n"));

  DEBUG ((DEBUG_ERROR, "fanjixxxxxxxxxxxUEFIDriverDxeEntryPointxxxxxxxxxxxxEnd\n\n"));
  return EFI_SUCCESS;
}
