/** @file
The Test Lib reference implement

Copyright (c) 2004 - 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "TestUiLib.h"

HII_VENDOR_DEVICE_PATH  mTestHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8)(sizeof (VENDOR_DEVICE_PATH)),
        (UINT8)((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    //
    // {D3867CC2-2E50-C8F0-8E49-A08E24ED095E}
    //
    { 0xD3867CC2, 0x2E50, 0xC8F0, { 0x8E, 0x49, 0xA0, 0x8E, 0x24, 0xED, 0x09, 0x5E }}
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

TEST_MANAGER_CALLBACK_DATA  gTestUILibPrivate = {
  TEST_MANAGER_CALLBACK_DATA_SIGNATURE,
  NULL,
  NULL,
  {
    TestExtractConfig,
    TestRouteConfig,
    TestCallback
  }
};


/**
  This function allows a caller to extract the current configuration for one
  or more named elements from the target driver.


  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Request         A null-terminated Unicode string in <ConfigRequest> format.
  @param Progress        On return, points to a character in the Request string.
                         Points to the string's null terminator if request was successful.
                         Points to the most recent '&' before the first failing name/value
                         pair (or the beginning of the string if the failure is in the
                         first name/value pair) if the request was not successful.
  @param Results         A null-terminated Unicode string in <ConfigAltResp> format which
                         has all values filled in for the names in the Request string.
                         String to be allocated by the called function.

  @retval  EFI_SUCCESS            The Results is filled with the requested values.
  @retval  EFI_OUT_OF_RESOURCES   Not enough memory to store the results.
  @retval  EFI_INVALID_PARAMETER  Request is illegal syntax, or unknown name.
  @retval  EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
TestExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  CONST EFI_STRING                      Request,
  OUT EFI_STRING                            *Progress,
  OUT EFI_STRING                            *Results
  )
{
  if ((Progress == NULL) || (Results == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Request;
  return EFI_NOT_FOUND;
}

/**
  This function processes the results of changes in configuration.

  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Configuration   A null-terminated Unicode string in <ConfigResp> format.
  @param Progress        A pointer to a string filled in with the offset of the most
                         recent '&' before the first failing name/value pair (or the
                         beginning of the string if the failure is in the first
                         name/value pair) or the terminating NULL if all was successful.

  @retval  EFI_SUCCESS            The Results is processed successfully.
  @retval  EFI_INVALID_PARAMETER  Configuration is NULL.
  @retval  EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
TestRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  CONST EFI_STRING                      Configuration,
  OUT EFI_STRING                            *Progress
  )
{
  if ((Configuration == NULL) || (Progress == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Configuration;

  return EFI_NOT_FOUND;
}

/**
  This function is invoked if user selected a interactive opcode from Test's
  Formset. If user set VBIOS, the new value is saved to EFI variable.

  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Action          Specifies the type of action taken by the browser.
  @param QuestionId      A unique value which is sent to the original exporting driver
                         so that it can identify the type of data to expect.
  @param Type            The type of value for the question.
  @param Value           A pointer to the data being sent to the original exporting driver.
  @param ActionRequest   On return, points to the action requested by the callback function.

  @retval  EFI_SUCCESS           The callback successfully handled the action.
  @retval  EFI_INVALID_PARAMETER The setup browser call this function with invalid parameters.

**/
EFI_STATUS
EFIAPI
TestCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL  *This,
  IN  EFI_BROWSER_ACTION                    Action,
  IN  EFI_QUESTION_ID                       QuestionId,
  IN  UINT8                                 Type,
  IN  EFI_IFR_TYPE_VALUE                    *Value,
  OUT EFI_BROWSER_ACTION_REQUEST            *ActionRequest
  )
{
  if (Action != EFI_BROWSER_ACTION_CHANGING) {
    //
    // Do nothing for other UEFI Action. Only do call back when data is changed.
    //
    return EFI_UNSUPPORTED;
  }

  if ((Value == NULL) || (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  Extract device path for given HII handle and class guid.

  @param Handle          The HII handle.

  @retval  NULL          Fail to get the device path string.
  @return  PathString    Get the device path string.

**/
CHAR16 *
TmExtractDevicePathFromHiiHandle (
  IN      EFI_HII_HANDLE  Handle
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  DriverHandle;

  ASSERT (Handle != NULL);

  if (Handle == NULL) {
    return NULL;
  }

  Status = gHiiDatabase->GetPackageListHandle (gHiiDatabase, Handle, &DriverHandle);
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  //
  // Get device path string.
  //
  return ConvertDevicePathToText (DevicePathFromHandle (DriverHandle), FALSE, FALSE);
}

/**
  Dynamic create Hii information for EDKII Menu(Test).

  @param   NextShowFormId     The FormId which need to be show.

**/
VOID
CreateEDKIIManagerForm (
  IN EFI_FORM_ID  NextShowFormId
  )
{
  UINTN               Index;
  EFI_STRING          String;
  EFI_STRING_ID       Token;
  EFI_STRING_ID       TokenHelp;
  EFI_HII_HANDLE      *HiiHandles;
  EFI_GUID            FormSetGuid;
  VOID                *StartOpCodeHandle;
  VOID                *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL  *StartLabel;
  EFI_IFR_GUID_LABEL  *EndLabel;
  CHAR16              *DevicePathStr;
  EFI_STRING_ID       DevicePathId;
  UINT8               ClassGuidNum;
  EFI_GUID            *ClassGuid;
  UINT8               *Ptr;
  EFI_STATUS          Status;

  EFI_HII_HANDLE      HiiHandle    = gTestUILibPrivate.HiiHandle;
  EFI_IFR_FORM_SET    *Buffer      = NULL;
  UINTN               BufferSize   = 0;
  UINTN               TempSize     = 0;

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

  //
  // Get all the Hii handles
  //
  HiiHandles = HiiGetHiiHandles (NULL);
  ASSERT (HiiHandles != NULL);

  //
  // Search for formset of each class type
  //
  for (Index = 0; HiiHandles[Index] != NULL; Index++) {
    Status = HiiGetFormSetFromHiiHandle (HiiHandles[Index], &Buffer, &BufferSize);
    if (EFI_ERROR (Status)) {
      continue;
    }

    Ptr = (UINT8 *)Buffer;
    while (TempSize < BufferSize) {
      TempSize += ((EFI_IFR_OP_HEADER *)Ptr)->Length;
      if (((EFI_IFR_OP_HEADER *)Ptr)->Length <= OFFSET_OF (EFI_IFR_FORM_SET, Flags)) {
        Ptr += ((EFI_IFR_OP_HEADER *)Ptr)->Length;
        continue;
      }

      ClassGuidNum = (UINT8)(((EFI_IFR_FORM_SET *)Ptr)->Flags & 0x3);
      ClassGuid    = (EFI_GUID *)(VOID *)(Ptr + sizeof (EFI_IFR_FORM_SET));
      while (ClassGuidNum-- > 0) {
        if (CompareGuid (&gTestUiLibGuid, ClassGuid) == 0) {        
        // if (CompareGuid (&gEfiHiiPlatformSetupFormsetGuid, ClassGuid) == 0) {
          ClassGuid++;
          continue;
        }

        String = HiiGetString (HiiHandles[Index], ((EFI_IFR_FORM_SET *)Ptr)->FormSetTitle, NULL);
        if (String == NULL) {
          String = HiiGetString (HiiHandle, STRING_TOKEN (STR_MISSING_STRING), NULL);
          ASSERT (String != NULL);
        }

        Token = HiiSetString (HiiHandle, 0, String, NULL);
        FreePool (String);

        String = HiiGetString (HiiHandles[Index], ((EFI_IFR_FORM_SET *)Ptr)->Help, NULL);
        if (String == NULL) {
          String = HiiGetString (HiiHandle, STRING_TOKEN (STR_MISSING_STRING), NULL);
          ASSERT (String != NULL);
        }

        TokenHelp = HiiSetString (HiiHandle, 0, String, NULL);
        FreePool (String);

        CopyMem (&FormSetGuid, &((EFI_IFR_FORM_SET *)Ptr)->Guid, sizeof (EFI_GUID));

        //
        // Not network device process, only need to show at device manger form.
        //
        if (NextShowFormId == TEST_MANAGER_FORM_ID) {
          DevicePathStr = TmExtractDevicePathFromHiiHandle (HiiHandles[Index]);
          DevicePathId  = 0;
          if (DevicePathStr != NULL) {
            DevicePathId =  HiiSetString (HiiHandle, 0, DevicePathStr, NULL);
            FreePool (DevicePathStr);
          }

          HiiCreateGotoExOpCode (
            StartOpCodeHandle,
            0,
            Token,
            TokenHelp,
            0,
            (EFI_QUESTION_ID)(Index + DEVICE_KEY_OFFSET),
            0,
            &FormSetGuid,
            DevicePathId
            );
        }

        break;
      }

      Ptr += ((EFI_IFR_OP_HEADER *)Ptr)->Length;
    }

    FreePool (Buffer);
    Buffer     = NULL;
    TempSize   = 0;
    BufferSize = 0;
  }

  HiiUpdateForm (
    HiiHandle,
    &gTestUiLibGuid,
    NextShowFormId,
    StartOpCodeHandle,
    EndOpCodeHandle
    );

  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);
  FreePool (HiiHandles);
}

/**
  Install Boot Manager Menu driver.

  @param ImageHandle     The image handle.
  @param SystemTable     The system table.

  @retval  EFI_SUCEESS  Install Boot manager menu success.
  @retval  Other        Return error status.

**/
EFI_STATUS
EFIAPI
TestUiLibConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  gTestUILibPrivate.DriverHandle = NULL;
  Status                         = gBS->InstallMultipleProtocolInterfaces (
                                      &gTestUILibPrivate.DriverHandle,
                                      &gEfiDevicePathProtocolGuid,
                                      &mTestHiiVendorDevicePath,
                                      &gEfiHiiConfigAccessProtocolGuid,
                                      &gTestUILibPrivate.ConfigAccess,
                                      NULL
                                      );
  ASSERT_EFI_ERROR (Status);

  //
  // Publish our HII data.
  //
  gTestUILibPrivate.HiiHandle = HiiAddPackages (
                                      &gTestUiLibGuid,
                                      gTestUILibPrivate.DriverHandle,
                                      TestUiLibVfrBin,
                                      TestUiLibStrings,
                                      NULL
                                      );
  ASSERT (gTestUILibPrivate.HiiHandle != NULL);

  //
  // Update boot manager page
  //
  CreateEDKIIManagerForm (TEST_MANAGER_FORM_ID);

  return EFI_SUCCESS;
}

/**
  Unloads the application and its installed protocol.

  @param  ImageHandle     Handle that identifies the image to be unloaded.
  @param  SystemTable     The system table.

  @retval EFI_SUCCESS           The image has been unloaded.
**/
EFI_STATUS
EFIAPI
TestUiLibDestructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = gBS->UninstallMultipleProtocolInterfaces (
                  gTestUILibPrivate.DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mTestHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &gTestUILibPrivate.ConfigAccess,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  HiiRemovePackages (gTestUILibPrivate.HiiHandle);

  return EFI_SUCCESS;
}