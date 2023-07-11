/** @file
This is an example of how a driver might export data to the HII protocol to be
later utilized by the Setup Protocol

Copyright (c) 2004 - 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "BootOptionsDxe.h"

HII_VENDOR_DEVICE_PATH  mBootVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8)(sizeof (VENDOR_DEVICE_PATH)),
        (UINT8)((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    BOOT_OPTIONS_FORMSET_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8)(END_DEVICE_PATH_LENGTH),
      (UINT8)((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

BOOT_OPTIONS_CALLBACK_DATA  *mPrivateData = NULL;

/**
  Dynamic create Hii information for EDKII Menu(Test).

  @param   NextShowFormId     The FormId which need to be show.

**/
VOID
CreateBootOptionsForm (
  IN EFI_FORM_ID  NextShowFormId
  )
{
  // UINTN               Index;
  // EFI_STRING          String;
  // EFI_STRING_ID       Token;
  // EFI_STRING_ID       TokenHelp;
  // EFI_HII_HANDLE      *HiiHandles;
  // EFI_GUID            FormSetGuid;
  VOID                *StartOpCodeHandle;
  VOID                *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL  *StartLabel;
  EFI_IFR_GUID_LABEL  *EndLabel;
  // CHAR16              *DevicePathStr;
  // EFI_STRING_ID       DevicePathId;
  // UINT8               ClassGuidNum;
  // EFI_GUID            *ClassGuid;
  // UINT8               *Ptr;
  // EFI_STATUS          Status;

  EFI_HII_HANDLE      HiiHandle    = mPrivateData->HiiHandle;
  // EFI_IFR_FORM_SET    *Buffer      = NULL;
  // UINTN               BufferSize   = 0;
  // UINTN               TempSize     = 0;  

  //
  // Allocate space for creation of UpdateData Buffer
  //
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (EndOpCodeHandle != NULL);

  //
  // Create Hii Extend Label OpCode as the start opcode
  //
  StartLabel               = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  //
  // According to the next show Form id(mNextShowFormId) to decide which form need to update.
  //
  StartLabel->Number = (UINT16)(LABEL_FORM_ID_OFFSET + NextShowFormId);

  //
  // Create Hii Extend Label OpCode as the end opcode
  //
  EndLabel               = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_END;


  HiiUpdateForm (
    HiiHandle,                 // HII handle
    &gBootOptionsFormsetGuid,  // Formset GUID
    NextShowFormId,            // Form ID
    StartOpCodeHandle,         // Label for where to insert opcodes
    EndOpCodeHandle            // Replace data
    );

  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);
  // FreePool (HiiHandles);  
}


/**
  Main entry for this driver.

  @param ImageHandle     Image handle this driver.
  @param SystemTable     Pointer to SystemTable.

  @retval EFI_SUCESS     This function always complete successfully.

**/
EFI_STATUS
EFIAPI
BootOptionsInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  //
  // Initialize driver private data
  //
  mPrivateData = AllocateZeroPool (sizeof (BOOT_OPTIONS_CALLBACK_DATA));
  if (mPrivateData == NULL)
    return EFI_OUT_OF_RESOURCES;

  mPrivateData->Signature    = BOOT_OPTIONS_CALLBACK_DATA_SIGNATURE;
  mPrivateData->HiiHandle    = NULL;
  mPrivateData->DriverHandle = NULL;
  mPrivateData->ConfigAccess.ExtractConfig = TestExtractConfig;
  mPrivateData->ConfigAccess.RouteConfig   = TestRouteConfig;
  mPrivateData->ConfigAccess.Callback      = TestCallback;


  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mPrivateData->DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mBootVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &mPrivateData->ConfigAccess,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Publish our HII data.
  //
  mPrivateData->HiiHandle = HiiAddPackages (
                                      &gBootOptionsFormsetGuid,
                                      mPrivateData->DriverHandle,
                                      BootOptionsVfrBin,
                                      BootOptionsDxeStrings,
                                      NULL
                                      );
  if (mPrivateData->HiiHandle == NULL) {
    BootOptionsUnload (ImageHandle);
    return EFI_OUT_OF_RESOURCES;
  }
  // ASSERT (mPrivateData->HiiHandle != NULL);

  //
  // Update boot manager page
  //
  CreateBootOptionsForm (DEVICE_MANAGER_FORM_ID);

  return EFI_SUCCESS;
}

/**
  Unloads the application and its installed protocol.

  @param[in]  ImageHandle       Handle that identifies the image to be unloaded.

  @retval EFI_SUCCESS           The image has been unloaded.
**/
EFI_STATUS
EFIAPI
BootOptionsUnload (
  IN EFI_HANDLE  ImageHandle
  )
{
  ASSERT (mPrivateData != NULL);

  if (mPrivateData->DriverHandle != NULL) {
    gBS->UninstallMultipleProtocolInterfaces (
           mPrivateData->DriverHandle,
           &gEfiDevicePathProtocolGuid,
           &mBootVendorDevicePath,
           &gEfiHiiConfigAccessProtocolGuid,
           &mPrivateData->ConfigAccess,
           NULL
           );
    mPrivateData->DriverHandle = NULL;
  }

  if (mPrivateData->HiiHandle != NULL) {
    HiiRemovePackages (mPrivateData->HiiHandle);
  }

  FreePool (mPrivateData);
  mPrivateData = NULL;

  return EFI_SUCCESS;  
}