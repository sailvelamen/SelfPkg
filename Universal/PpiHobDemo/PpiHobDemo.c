#include "PpiHobDemo.h"

EFI_STATUS EFIAPI SelfTestFunction (VOID)
{
  DEBUG ((DEBUG_INFO, "fanjix: SelfTestFunction Enter\n"));

  DEBUG ((DEBUG_INFO, "fanjix: Creat EFI_SELF_HOB Start\n"));
  EFI_SELF_HOB  MySelfHob;
  MySelfHob.Type = 0x02;
  MySelfHob.ID   = 0x135246;
  DEBUG ((DEBUG_INFO, "fanjix: MySelfHob.Type = %d\n", MySelfHob.Type));
  DEBUG ((DEBUG_INFO, "fanjix: MySelfHob.ID = 0x%X\n", MySelfHob.ID));
  BuildGuidDataHob (&gEfiSelfHobGuid, &MySelfHob, sizeof (MySelfHob));
  DEBUG ((DEBUG_INFO, "fanjix: Creat EFI_SELF_HOB Start End\n"));

  DEBUG ((DEBUG_INFO, "fanjix: SelfTestFunction End\n"));
  return EFI_SUCCESS;
}

SELF_TEST_INIT_PPI mSelfTestPpi = {
  SelfTestFunction
};

CONST EFI_PEI_PPI_DESCRIPTOR  mPeiSelfInitPpi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gSelfTestInitFunctionPpiGuid,
  &mSelfTestPpi
};

EFI_STATUS
EFIAPI
PeimInitEntryPoint (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  DEBUG ((DEBUG_INFO, "\nfanjix: PeimInitEntryPoint Enter\n")); 

  EFI_STATUS  Status;
  Status = PeiServicesInstallPpi (&mPeiSelfInitPpi);
  DEBUG ((DEBUG_INFO, "fanjix: PeiServicesInstallPpi: %r\n", Status));
  SELF_TEST_INIT_PPI *SelfTestPpi;
  Status = PeiServicesLocatePpi (
            &gSelfTestInitFunctionPpiGuid,
            0,
            NULL,
            (VOID **)&SelfTestPpi
            );
  DEBUG ((DEBUG_INFO, "fanjix: PeiServicesLocatePpi: %r\n", Status));
  Status = SelfTestPpi->Self_Test();
  DEBUG ((DEBUG_INFO, "fanjix: Self_Test: %r\n", Status));

  DEBUG ((DEBUG_INFO, "fanjix: PeimInitEntryPoint End\n\n")); 
  return Status;
}