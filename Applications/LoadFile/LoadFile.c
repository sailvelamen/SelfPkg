#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/UefiBootServicesTableLib.h>    //gST,gBs
#include <Library/UefiRuntimeServicesTableLib.h> //gRT
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/FileExplorerLib.h>

#include <Guid/FileInfo.h>


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


INTN
EFIAPI
ShellAppMain (
  IN UINTN Argc,
  IN CHAR16 *Argv[]
  )
{
  DEBUG ((DEBUG_INFO, "fanjix: %a\n", __FUNCTION__));
  Print (L"===============================================================================\n");

  CHAR16      *Filename;
  VOID       *Buffer;
  // UINTN      BufferSize;

  EFI_STATUS        Status;
  UINTN             NumberOfHandles;
  EFI_HANDLE        *HandleBuffer;
  UINTN             Index;
  EFI_FILE_HANDLE   FileHandle;
  EFI_FILE_PROTOCOL *FileProtocol;
  EFI_FILE_INFO     *FileInfo;

  DEBUG ((DEBUG_INFO, "Argc = %d\n", Argc));
  switch (Argc)
  {
  case 2:
    Print (L"Argv[1]: %s\n", Argv[1]);
        
    Filename = Argv[1];

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
    for (Index = 0; Index < NumberOfHandles; Index++) {

      FileHandle = LibOpenRoot (HandleBuffer[Index]);

      if (FileHandle == NULL) {
        Status = EFI_NOT_FOUND;
      } 

      if (!EFI_ERROR (Status)) { 
        Print (L"Locate FileHandle!\n");
        Status = FileHandle->Open (
                              FileHandle,
                              &FileProtocol,
                              Filename,
                              EFI_FILE_MODE_READ,
                              0
                              );
        if (!EFI_ERROR (Status)) {
          break;
        } 
        else 
        {
          FileHandle->Close(FileHandle);
          FileHandle = NULL;									
        }
      }
    }

    FileInfo = LibFileInfo (FileProtocol, &gEfiFileInfoGuid);

    Buffer = AllocateZeroPool ((UINTN)FileInfo->FileSize);
    if (Buffer == NULL) {
      goto Done;
    }

    Status = FileProtocol->Read (FileProtocol, &FileInfo->FileSize, Buffer);
    if (EFI_ERROR (Status)) {
      FreePool (Buffer);
      goto Done;
    }
    Print (L"FileSize = 0x%X, FileAddr = 0x%X\n", FileInfo->FileSize, Buffer);

    break;
  
  default:   
    Print (L"\ne.g. LoadFile.efi jh7110-starfive-visionfive-2-v1.3b.dtb\n");
    break;
  }

  Print (L"===============================================================================\n");

Done:
  FreePool (HandleBuffer);
  FreePool (FileInfo);
  return EFI_SUCCESS;
}

