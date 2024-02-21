#include "SpiFlash.h"



/**
  UEFI application entry point which has an interface similar to a
  standard C main function.

  The ShellCEntryLib library instance wrappers the actual UEFI application
  entry point and calls this ShellAppMain function.

  @param  Argc             Argument count
  @param  Argv             The parsed arguments

  @retval  0               The application exited normally.
  @retval  Other           An error occurred.

**/
INTN
EFIAPI
ShellAppMain (
  IN UINTN   Argc,
  IN CHAR16  **Argv
  )
{
  EFI_STATUS        Status;
  UINT64            DataOffset;
  CHAR16            *FileName;
  UINTN             NumberOfHandles;
  EFI_HANDLE        *HandleBuffer;
  EFI_FILE_HANDLE   FileHandle;
  EFI_FILE_PROTOCOL *FileProtocol;
  EFI_FILE_INFO     *FileInfo;
  VOID              *FileBuffer;

  FileName   = Argv[2];
  DataOffset = StrHexToUint64 (Argv[1]);
  Print (L"Offset = 0x%lX FileName = %s\n", DataOffset, FileName);

  NumberOfHandles = 0;
  FileHandle      = NULL;
  FileProtocol    = NULL;
  FileInfo        = NULL;
  //
  // Locate Handles that support Simple File System protocol
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "LocateHandleBuffer Status = %r\n", Status));
    return Status;
  } 

  //
  // Find all the instances of the File System prototocol
  //
  for (UINTN Index = 0; Index < NumberOfHandles; Index++) {

    FileHandle = LibOpenRoot (HandleBuffer[Index]);

    if (FileHandle == NULL) {
      Status = EFI_NOT_FOUND;
    } 

    if (!EFI_ERROR (Status)) { 
      Print (L"Locate FileHandle!\n");
      Status = FileHandle->Open (
                            FileHandle,
                            &FileProtocol,
                            FileName,
                            EFI_FILE_MODE_READ,
                            0
                            );
      if (!EFI_ERROR (Status)) {
        DEBUG ((DEBUG_INFO, "%r File Path!\n", Status));
        break;
      } 
      else 
      {
        FileHandle->Close(FileHandle);
        FileHandle = NULL;									
      }
    }
  }

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "%r File Path!\n", Status));
    return Status;
  } 
  FileInfo = LibFileInfo (FileProtocol, &gEfiFileInfoGuid);

  FileBuffer = AllocateZeroPool ((UINTN)FileInfo->FileSize);
  if (FileBuffer == NULL) {
    goto Done;
  }

  Status = FileProtocol->Read (FileProtocol, &FileInfo->FileSize, FileBuffer);
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  FLASH_DEVICE      *FlashInstance;
  SPI_DEVICE        *SpiFlash;
  UINTN             FileSize;

  FlashInstance = NULL;
  SpiFlash      = NULL;
  FileSize      = (UINTN)FileInfo->FileSize;

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
  Status = FirmwareUpdate (FlashInstance, FileBuffer, DataOffset, FileSize);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%s: Fail!\n", L"FirmwareUpdate"));
    goto Done;
  }

Done:
  FreePool (HandleBuffer);
  FreePool (FileBuffer);
  FreePool (FileInfo);

  return 0;
}