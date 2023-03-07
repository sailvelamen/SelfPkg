#include "SWSMIDemo.h"

EFI_STATUS
EFIAPI
SmmCommunicationCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  DEBUG ((DEBUG_INFO, "fanjix: SmmCommunicationCallback Enter\n"));
  // AsmWriteMsr64 ();
  DEBUG ((DEBUG_INFO, "fanjix: SmmCommunicationCallback End\n"));
  return EFI_SUCCESS;
}

EFI_STATUS EFIAPI SwSmiDemoMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  DEBUG ((DEBUG_INFO, "\nfanjixxxxxxxxxSwSmiDemoMainxxxxxxxxxEnter\n"));

  EFI_STATUS                     Status;
  EFI_SMM_SW_DISPATCH2_PROTOCOL  *SmmSwDispatch2;
  EFI_SMM_SW_REGISTER_CONTEXT    SmmSwDispatchContext;
  EFI_HANDLE                     DispatchHandle;
  //
  // Locate PI SMM CPU protocol
  //
  Status = gSmst->SmmLocateProtocol (&gEfiSmmCpuProtocolGuid, NULL, (VOID **)&mSmmCpuProtocol);
  ASSERT_EFI_ERROR (Status);

  //
  // Register a SMM handler to handle subsequent SW SMIs.
  // 
  Status = gSmst->SmiHandlerRegister (SmmSwDispatcher, NULL, &DispatchHandle);
  ASSERT_EFI_ERROR (Status);
  //
  // Publish PI SMM SwDispatch2 Protocol
  //
  ImageHandle = NULL;
  Status      = gSmst->SmmInstallProtocolInterface (
                         &ImageHandle,
                         &gEfiSmmSwDispatch2ProtocolGuid,
                         EFI_NATIVE_INTERFACE,
                         &gSmmSwDispatch2
                         );
  ASSERT_EFI_ERROR (Status);

  Status      = gSmst->SmmLocateProtocol (
                        &gEfiSmmSwDispatch2ProtocolGuid,
                        NULL,
                        (VOID **)&SmmSwDispatch2
                        );
  ASSERT_EFI_ERROR (Status);

  SmmSwDispatchContext.SwSmiInputValue = EFI_ACPI_ENABLE_SW_SMI; // Sw Smi Port
  Status      = SmmSwDispatch2->Register (
                                  SmmSwDispatch2,
                                  SmmCommunicationCallback,
                                  &SmmSwDispatchContext,
                                  &DispatchHandle
                                  );  
  DEBUG ((DEBUG_INFO, "fanjixxxxxxxxxSwSmiDemoMainxxxxxxxxxEnd\n\n"));
  return EFI_SUCCESS;
}
