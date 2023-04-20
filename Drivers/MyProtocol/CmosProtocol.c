#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>    //gST,gBs
#include <Library/UefiRuntimeServicesTableLib.h> //gRT

#include "MyProtocol.h"

extern MY_PRIVATE_DATA gMyData;
extern EFI_GUID gEFIMYCmosProtocolGUID;

EFI_STATUS EFIAPI CmosProtocolEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_TIME curTime;

  // BootService and RuntimeService
  SystemTable->BootServices->Stall(2000);  // delay 2s
  SystemTable->RuntimeServices->GetTime(&curTime,NULL);
  Print(L"Current Time: %d-%d-%d %02d:%02d:%02d\n",curTime.Year,curTime.Month,curTime.Day,curTime.Hour,curTime.Minute,curTime.Second);
  //使用SystemTable
  SystemTable->ConOut->OutputString(SystemTable->ConOut,L"Test SystemTable...\n\r");

    EFI_STATUS Status;
    MyCmosProtocolInit();
    Status = gBS->InstallProtocolInterface (
                    &ImageHandle,
                    &gEFIMYCmosProtocolGUID,
                    EFI_NATIVE_INTERFACE,
                    &gMyData.myProtocol
                    );

    EFI_MYCMOS_PRPTOCOL *TestProtocol;
    Status = gBS->LocateProtocol ( 
                    &gEFIMYCmosProtocolGUID, 
                    NULL, 
                    (VOID **) &TestProtocol 
                    );                 
    Status = TestProtocol->MyCmos_Out(TestProtocol);
    Status = TestProtocol->MyCmos_In(TestProtocol, 4);

    return Status;
}