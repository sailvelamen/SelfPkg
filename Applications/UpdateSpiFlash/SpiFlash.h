#include <PiDxe.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/UefiBootServicesTableLib.h>    //gST,gBs
#include <Library/UefiRuntimeServicesTableLib.h> //gRT

#include <Library/DebugLib.h>
#include <Library/ShellCommandLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/SpiFlash.h>

typedef struct {
  SPI_DEVICE                          SpiDevice;
  SPI_FLASH_PROTOCOL                  *SpiFlashProtocol;
  SPI_MASTER_PROTOCOL                 *SpiMasterProtocol;
} FLASH_DEVICE;

SPI_FLASH_PROTOCOL  *SpiFlashProtocol;
SPI_MASTER_PROTOCOL *SpiMasterProtocol;  
// SPI_DEVICE          SpiDevice = {
//   0,    // SpiFlash Transfer Mode
//   0,    // SpiFlash Address Size
//   NULL, // SpiFlash detailed information
//   0,    // SpiFlash RegBase
//   0,    // SpiFlash MaxHz
//   0,    // SpiFlash PrevHz
//   0,    // SpiFlash CaliHz
//   0,    // SpiFlash ReadDelay
//   0,    // SpiFlash RefClkHz
//   0,    // SpiFlash CaliCs
//   0,    // SpiFlash IsDecodedCs
//   0,    // SpiFlash AbhBase
//   0,    // SpiFlash AhbSize
//   0,    // SpiFlash FifoDepth
//   0,    // SpiFlash FifoWidth
//   0,    // SpiFlash TrigAdd
//   0,    // SpiFlash DacMode
//   0,    // SpiFlash WRDelay
//   0,    // SpiFlash Dtr
// }; // SpiDevice

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

/**

  Function opens and returns a file handle to the root directory of a volume.

  @param DeviceHandle    A handle for a device

  @return A valid file handle or NULL is returned

**/
EFI_FILE_HANDLE
LibOpenRoot (
  IN EFI_HANDLE  DeviceHandle
  )
{
  EFI_STATUS                       Status;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *Volume;
  EFI_FILE_HANDLE                  File;

  File = NULL;

  //
  // File the file system interface to the device
  //
  Status = gBS->HandleProtocol (
                  DeviceHandle,
                  &gEfiSimpleFileSystemProtocolGuid,
                  (VOID *)&Volume
                  );

  //
  // Open the root directory of the volume
  //
  if (!EFI_ERROR (Status)) {
    Status = Volume->OpenVolume (
                       Volume,
                       &File
                       );
  }

  //
  // Done
  //
  return EFI_ERROR (Status) ? NULL : File;
}

/**

  Function gets the file information from an open file descriptor, and stores it
  in a buffer allocated from pool.

  @param FHand           File Handle.
  @param InfoType        Info type need to get.

  @retval                A pointer to a buffer with file information or NULL is returned

**/
EFI_FILE_INFO *
LibFileInfo (
  IN EFI_FILE_HANDLE  FHand,
  IN EFI_GUID         *InfoType
  )
{
  EFI_STATUS     Status;
  EFI_FILE_INFO  *Buffer;
  UINTN          BufferSize;

  Buffer     = NULL;
  BufferSize = 0;

  Status = FHand->GetInfo (
                    FHand,
                    InfoType,
                    &BufferSize,
                    Buffer
                    );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    Buffer = AllocatePool (BufferSize);
    ASSERT (Buffer != NULL);
  }

  Status = FHand->GetInfo (
                    FHand,
                    InfoType,
                    &BufferSize,
                    Buffer
                    );

  return Buffer;
}

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