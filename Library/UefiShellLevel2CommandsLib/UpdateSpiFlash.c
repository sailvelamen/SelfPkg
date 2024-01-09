/** @file
  Main file for Pci shell Debug1 function.

  Copyright (c) 2005 - 2021, Intel Corporation. All rights reserved.<BR>
  (C) Copyright 2013-2015 Hewlett-Packard Development Company, L.P.<BR>
  (C) Copyright 2016 Hewlett Packard Enterprise Development LP<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "UefiShellLevel2CommandsLib.h"

#include <Protocol/SpiFlash.h>

typedef struct {
  SPI_DEVICE                          SpiDevice;
  SPI_FLASH_PROTOCOL                  *SpiFlashProtocol;
  SPI_MASTER_PROTOCOL                 *SpiMasterProtocol;
} FLASH_DEVICE;

SPI_FLASH_PROTOCOL  *SpiFlashProtocol;
SPI_MASTER_PROTOCOL *SpiMasterProtocol;  

FLASH_DEVICE mFlashInstancTemplate = {
  {
    0,    // SpiFlash Transfer Mode
    0,    // SpiFlash Address Size
    NULL, // SpiFlash detailed information
    0,    // SpiFlash RegBase
    0,    // SpiFlash MaxHz
    0,    // SpiFlash PrevHz
    0,    // SpiFlash CaliHz
    0,    // SpiFlash ReadDelay
    0,    // SpiFlash RefClkHz
    0,    // SpiFlash CaliCs
    0,    // SpiFlash IsDecodedCs
    0,    // SpiFlash AbhBase
    0,    // SpiFlash AhbSize
    0,    // SpiFlash FifoDepth
    0,    // SpiFlash FifoWidth
    0,    // SpiFlash TrigAdd
    0,    // SpiFlash DacMode
    0,    // SpiFlash WRDelay
    0,    // SpiFlash Dtr
  }, // SpiDevice
  NULL,
  NULL
};

STATIC
EFI_STATUS
FlashProbe (
  IN FLASH_DEVICE *FlashInstance
  )
{ DEBUG ((DEBUG_INFO, "%a: Entry\n", __FUNCTION__));
  SPI_FLASH_PROTOCOL *SpiFlashProtocol;
  EFI_STATUS Status;

  SpiFlashProtocol = FlashInstance->SpiFlashProtocol;

  // Read SPI flash ID
  Status = SpiFlashProtocol->ReadId (&FlashInstance->SpiDevice, TRUE);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%s: SpiFlashProtocol->ReadId: EFI_NOT_FOUND\n", L"SpiFlashProtocol->ReadId"));
    return EFI_NOT_FOUND;
  }
Print (L"FlashInstanc->SpiDevice.Info->Flags = %d\n", FlashInstance->SpiDevice.Info->Flags);
  Status = SpiFlashProtocol->Init (SpiFlashProtocol, &FlashInstance->SpiDevice);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Cannot initialize flash device\n", __FUNCTION__));
    return EFI_DEVICE_ERROR;
  }
Print (L"FlashInstanc->SpiDevice.Info->Flags = %d\n", FlashInstance->SpiDevice.Info->Flags);
  //
  // SPI flash may require 20ms interval between enabling it and
  // accessing in Direct Mode to its memory mapped content.
  //
  gBS->Stall (20000);

  return EFI_SUCCESS;
}


STATIC
EFI_STATUS
FirmwareUpdate (
  IN FLASH_DEVICE  *FlashInstance,
  IN VOID          *FileBuffer,
  IN UINT64        Addr,
  IN UINTN         FileSize
  ) 
{
  UINT32            Offset, Size;
  VOID              *mFileBuffer;
  EFI_STATUS Status;

  Offset = Addr;
  Size   = 0x20000; 
  mFileBuffer = FileBuffer;
  Print (L"Waiting: %d\n", FileSize/Size);
  for (UINTN i = 0 ; i <= FileSize/Size; i++)
  {
    Print (L"%d ", i);
    // Erase entire region
    Status = FlashInstance->SpiFlashProtocol->Erase (&FlashInstance->SpiDevice, Offset, Size);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%s: Fail!\n", L"SpiFlashProtocol->Erase"));
      return Status;
    }
    Status = FlashInstance->SpiFlashProtocol->Write (&FlashInstance->SpiDevice, Offset, Size, mFileBuffer);
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "%s: Fail!\n", L"SpiFlashProtocol->Write"));
      return Status;
    }
    Offset      += Size;
    mFileBuffer += Size;
  }

  return Status;
}

/**
  Function for 'UpdateSpiFlash' command.

  @param[in] ImageHandle  Handle to the Image (NULL if Internal).
  @param[in] SystemTable  Pointer to the System Table (NULL if Internal).
**/
SHELL_STATUS
EFIAPI
ShellCommandRunUpdateSpiFlash (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS        Status;
  SHELL_STATUS      ShellStatus;
  CONST CHAR16      *Temp;
  LIST_ENTRY        *Package;
  CHAR16            *ProblemParam;
  UINT64            Addr;
  UINT64            RetVal;
  CONST CHAR16      *FileName;
  SHELL_FILE_HANDLE ShellFileHandle;
  UINTN             FileSize;
  VOID              *FileBuffer;
  UINTN             ReadSize;
  SPI_DEVICE        *SpiFlash;
  FLASH_DEVICE      *FlashInstance;

  ProblemParam  = NULL;
  // FileName      = NULL;
  ShellStatus   = SHELL_SUCCESS;
  FlashInstance = NULL;
  SpiFlash      = NULL;
  RetVal        = 0;
  Addr          = 0;

  //
  // initialize the shell lib (we must be in non-auto-init...)
  //
  Status = ShellInitialize ();
  ASSERT_EFI_ERROR (Status);

  //
  // parse the command line
  //
  Status = ShellCommandLineParse (EmptyParamList, &Package, &ProblemParam, TRUE);
  if (EFI_ERROR (Status)) {
    if ((Status == EFI_VOLUME_CORRUPTED) && (ProblemParam != NULL)) {
      ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_PROBLEM), gShellLevel2HiiHandle, L"UpdateSpiFlash", ProblemParam);
      FreePool (ProblemParam);
      ShellStatus = SHELL_INVALID_PARAMETER;
    } else {
      ASSERT (FALSE);
    }
  } else {
    if (ShellCommandLineGetCount (Package) < 3) {
      ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_TOO_FEW), gShellLevel2HiiHandle, L"UpdateSpiFlash");
      ShellStatus = SHELL_INVALID_PARAMETER;
      goto Done;
    }

    if (ShellCommandLineGetCount (Package) > 3) {
      ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_TOO_MANY), gShellLevel2HiiHandle, L"UpdateSpiFlash");
      ShellStatus = SHELL_INVALID_PARAMETER;
      goto Done;
    }
  }

  Temp = ShellCommandLineGetRawValue (Package, 1);
  if (Temp != NULL) {
    //
    // Input converted to hexadecimal number.
    //
    if (!EFI_ERROR (ShellConvertStringToUint64 (Temp, &RetVal, TRUE, TRUE))) {
      Addr = RetVal;    
    } else {
      ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_PARAM_INV_HEX), gShellLevel2HiiHandle, L"UpdateSpiFlash", Temp);
      ShellStatus = SHELL_INVALID_PARAMETER;
      goto Done;
    }
  } 
  Print (L"Addr = 0x%lX\n", Addr);

  Temp = NULL;
  Temp = ShellCommandLineGetRawValue (Package, 2);
  if (Temp != NULL) {
    FileName = Temp;
    Print (L"FileName = %s\n", FileName);
  } else {
    goto Done;
  }

  //
  // We need to open something up to get system information
  //
  Status = gEfiShellProtocol->OpenFileByName (
                                FileName,
                                &ShellFileHandle,
                                EFI_FILE_MODE_READ
                                );

  Status = gEfiShellProtocol->GetFileSize (ShellFileHandle, &FileSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((FileSize == 0) || (FileSize > MAX_UINTN)) {
    return SHELL_UNSUPPORTED;
  }
  
  FileBuffer = AllocateZeroPool (FileSize);
  if (FileBuffer == NULL) {
    goto Done;
  }
  ReadSize = FileSize;
  Status   = gEfiShellProtocol->ReadFile (ShellFileHandle, &ReadSize, FileBuffer);
  if (EFI_ERROR (Status) || (ReadSize < FileSize)) {
    DEBUG ((
      DEBUG_WARN,
      "%a: failed to read initrd file - %r 0x%lx 0x%lx\n",
      __FUNCTION__,
      Status,
      (UINT64)ReadSize,
      FileSize
      ));
    goto Done;
  }

  //
  // Init SpiFlashInstance
  //
  FlashInstance = AllocateCopyPool (sizeof (mFlashInstancTemplate), &mFlashInstancTemplate);
  if (FlashInstance == NULL) {
    DEBUG ((DEBUG_ERROR, "%a: Cannot allocate memory\n", __FUNCTION__));
    return SHELL_OUT_OF_RESOURCES;
  }

  // Locate SPI protocols
  Status = gBS->LocateProtocol (&gJH7110SpiFlashProtocolGuid,
                  NULL,
                  (VOID **)&FlashInstance->SpiFlashProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Cannot locate SpiFlash protocol\n", __FUNCTION__));
    return Status;
  }

  Status = gBS->LocateProtocol (&gJH7110SpiMasterProtocolGuid,
                  NULL,
                  (VOID **)&FlashInstance->SpiMasterProtocol);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a: Cannot locate SpiMaster protocol\n", __FUNCTION__));
    return Status;
  }

  DEBUG ((DEBUG_INFO, "%a: Locate SpiFlash and SpiMaster protocol\n", __FUNCTION__));

  // Setup and probe SPI flash
  SpiFlash = NULL;
  SpiFlash = FlashInstance->SpiMasterProtocol->SetupDevice (
                FlashInstance->SpiMasterProtocol, 
                &FlashInstance->SpiDevice, 
                0);
  if (SpiFlash == NULL) {
    DEBUG ((DEBUG_INFO, "%s: Cannot allocate SpiFlash!\n", L"SpiMasterProtocol->SetupDevice"));
    return SHELL_ABORTED;
  }
  DEBUG ((DEBUG_INFO, "%a: SetupDevice Success\n", __FUNCTION__));  

  //
  // Probe spi flash 
  //
  Status = FlashProbe (FlashInstance);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%s: Fail!\n", L"FlashProbe"));
    goto Done;
  }

  //
  // Update Firmware
  //
  Status = FirmwareUpdate (FlashInstance, FileBuffer, Addr, FileSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%s: Fail!\n", L"FirmwareUpdate"));
    goto Done;
  }

Done:
  if (Package != NULL) {
    ShellCommandLineFreeVarList (Package);
  }
  FreePool (FileBuffer);
  FreePool (FlashInstance);
  return (ShellStatus);
}