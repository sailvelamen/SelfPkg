#include "ACPI_Demo.h"

#include <IndustryStandard/Bmp.h>
#include <Protocol/GraphicsOutput.h>
#include <Library/BmpSupportLib.h>
#include <Library/MemoryAllocationLib.h>

EFI_STATUS 
EFIAPI
SaveToFile(
  IN UINT8 *FileData, 
  IN UINTN FileDataLength
  )
{
  EFI_STATUS                      Status;
  UINTN                           BufferSize;
  EFI_FILE_PROTOCOL               *Root;
  EFI_FILE_PROTOCOL               *FileHandle;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *SimpleFileSystem;

  Status = gBS->LocateProtocol(
              &gEfiSimpleFileSystemProtocolGuid, 
              NULL,
              (VOID **)&SimpleFileSystem);
              
  if (EFI_ERROR(Status)) {
          Print(L"Cannot find EFI_SIMPLE_FILE_SYSTEM_PROTOCOL \r\n");
          return Status;
  }

  Status = SimpleFileSystem->OpenVolume(SimpleFileSystem, &Root);
  if (EFI_ERROR(Status)) {
      Print(L"OpenVolume error \r\n");
      return Status;
  }
  Status = Root->Open(
              Root, 
              &FileHandle, 
              L"BIOSLogo.bmp",
              EFI_FILE_MODE_READ |
              EFI_FILE_MODE_WRITE | 
              EFI_FILE_MODE_CREATE, 
              0);
  if (EFI_ERROR(Status)){
      Print(L"Error Open NULL  [%r]\n",Status);
      return Status;
  }
  
  BufferSize = FileDataLength;
  Status = FileHandle->Write(FileHandle, &BufferSize, FileData);
  FileHandle->Close(FileHandle);
  
  return Status;
}

/**
  Function Description: dump BGRT Table

  @param  Entry ACPI Table

**/
EFI_STATUS
EFIAPI 
DumpBGRTTable (
  EFI_ACPI_6_5_BOOT_GRAPHICS_RESOURCE_TABLE *Entry
  )
{
  printf("---------------------------------------------------------------------------------\n");
  PrintSignature (Entry->Header.Signature, sizeof (Entry->Header.Signature));
  printf(" @ 0x%016llX\n", Entry);
  printf("       Signature: 0x%-16lX  ", Entry->Header.Signature);
  PrintSignature (Entry->Header.Signature, sizeof (Entry->Header.Signature));
  printf("      @ 0x%016llX\n", &Entry->Header.Signature);
  printf("          Length: %-04u or    0x%04x             @ 0x%016llX\n", Entry->Header.Length, Entry->Header.Length, &Entry->Header.Length);
  printf("        Revision: %-18u            @ 0x%016llX\n", Entry->Header.Revision, &Entry->Header.Revision);
  printf("        Checksum: 0x%-16X            @ 0x%016llX\n", Entry->Header.Checksum, &Entry->Header.Checksum);
  printf("          OEM_ID: 0x");
  for (char i = 5; i >=0; i--)
    printf("%x", Entry->Header.OemId[i]);
  printf("      ");
  for (UINT8 i = 0; i < 6; i++)
    printf("%c", Entry->Header.OemId[i]);
  printf("    @ 0x%016llX\n", Entry->Header.OemId);
  printf("      OemTableId: 0x%016llX  ", Entry->Header.OemTableId);
  UINT64 OemTableId = Entry->Header.OemTableId;
  for (UINT8 i = 1; i <= 8; i++) {
    printf("%c", OemTableId & 0xFF);
    OemTableId >>= 8;
  }
  printf("  @ 0x%016llX\n", &Entry->Header.OemTableId);
  printf("     OemRevision: %-18lu            @ 0x%016llX\n", Entry->Header.OemRevision, &Entry->Header.OemRevision);
  printf("       CreatorId: 0x%-16lX            @ 0x%016llX\n", Entry->Header.CreatorId, &Entry->Header.CreatorId);
  printf(" CreatorRevision: 0x%-16lX            @ 0x%016llX\n\n", Entry->Header.CreatorRevision, &Entry->Header.CreatorRevision);
  printf("         Version: 0x%-16lX            @ 0x%016llX\n", Entry->Version, &Entry->Version);
  printf("          Status: 0x%-16lX            @ 0x%016llX\n", Entry->Status, &Entry->Status);
  printf("       ImageType: 0x%-16lX            @ 0x%016llX\n", Entry->ImageType, &Entry->ImageType);
  printf("    ImageAddress: 0x%-16lX            @ 0x%016llX\n", Entry->ImageAddress, &Entry->ImageAddress);
  printf("    ImageOffsetX: 0x%-16lX            @ 0x%016llX\n", Entry->ImageOffsetX, &Entry->ImageOffsetX);
  printf("    ImageOffsetY: 0x%-16lX            @ 0x%016llX\n\n", Entry->ImageOffsetY, &Entry->ImageOffsetY);

  DumpHex (2, 0, Entry->Header.Length, Entry);  printf("\n");
}

/**
  Function Description: dump ACPI BGRT


  @param  Entry ACPI Entry

**/
EFI_ACPI_6_5_BOOT_GRAPHICS_RESOURCE_TABLE * 
DumpBGRT (
  EFI_ACPI_DESCRIPTION_HEADER *Entry
  )
{
  UINT8  AdrLength;
  UINT8  EntryCount;
  UINT32 *pEntry32;    
  UINT64 *pEntry64;

  (!CompareMem (&Entry->Signature, "XSDT", 4)) ? (AdrLength = sizeof(UINT64)) : (AdrLength = sizeof(UINT32));
  EntryCount = (Entry->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER)) / AdrLength;
  // printf("EntryCount = %d\n", EntryCount);

  if (AdrLength == sizeof(UINT64)) 
    pEntry64 = (UINT64 *)++Entry;  // get first Entry address
  else
    pEntry32 = (UINT32 *)++Entry;
  while (EntryCount--) {
    if (AdrLength == sizeof(UINT64)) {
      Entry = (EFI_ACPI_DESCRIPTION_HEADER *)(*pEntry64++);  // get first Entry address
      if (Entry->Signature == EFI_ACPI_6_5_BOOT_GRAPHICS_RESOURCE_TABLE_SIGNATURE)
        return (EFI_ACPI_6_5_BOOT_GRAPHICS_RESOURCE_TABLE *)Entry;
    }
    else {
      Entry = (EFI_ACPI_DESCRIPTION_HEADER *)(*(pEntry32++));     
      if (Entry->Signature == EFI_ACPI_6_5_BOOT_GRAPHICS_RESOURCE_TABLE_SIGNATURE)
        return (EFI_ACPI_6_5_BOOT_GRAPHICS_RESOURCE_TABLE *)Entry;
    }
  }
}

/**
  Function Description: Display BMP Image on Screem

  @param  BGRT ACPI Table pointer

**/
EFI_STATUS
EFIAPI 
DisplayBMPImage (
  VOID             *LogoBltBuffer,
  BMP_IMAGE_HEADER *mBMP
  )
{
  EFI_STATUS                                Status;
  UINTN                                     Width;
  UINTN                                     Height;
  UINTN                                     GopBltSize;

  BMP_IMAGE_HEADER                          *BMP            = mBMP;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL             *GopBlt         = NULL;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL             *mLogoBltBuffer = LogoBltBuffer;
  EFI_GRAPHICS_OUTPUT_PROTOCOL              *GraphicsOutput;

  Status = gBS->LocateProtocol (
                  &gEfiGraphicsOutputProtocolGuid, 
                  NULL,
                  (VOID **)&GraphicsOutput
                  );
  if (EFI_ERROR (Status)) {
    GraphicsOutput = NULL;
    Print (L"locate protocol gEfiGraphicsOutputProtocolGuid is error!\n");
    return EFI_UNSUPPORTED;
  }

  Status = TranslateBmpToGopBlt (
             (VOID *)mLogoBltBuffer,
             (UINTN)(BMP->Size),
             &GopBlt,
             &GopBltSize,
             &Height,
             &Width
             );
  if (EFI_ERROR (Status)) {
    Print (L"DisplayBMPImage: BMP image is not valid.\n");
    // goto Done;
  }

  Status = GraphicsOutput->Blt (
                            GraphicsOutput,
                            GopBlt,
                            EfiBltBufferToVideo,
                            0,
                            0,
                            0x0,
                            0x0,
                            Width,
                            Height,
                            Width * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                            );
  if (EFI_ERROR (Status)) {
    Print (L"GraphicsOutput Blt is error.\n");
    // goto Done;
  }

  return Status;
}

/***
  Print a welcoming message.

  Establishes the main structure of the application.

  @retval  0         The application exited normally.
  @retval  Other     An error occurred.
***/
int main (IN int Argc, IN char **Argv)
{
  Print(L"Hello,this is Entry of DumpBGRT!(Applications)\n");

  EFI_STATUS                                   Status;
  VOID                                         *mLogoBltBuffer;
  EFI_GUID                                     *Guid = &gEfiAcpiTableGuid;
  EFI_ACPI_DESCRIPTION_HEADER                  *Entry;
  EFI_ACPI_6_5_BOOT_GRAPHICS_RESOURCE_TABLE    *BGRT;
  EFI_ACPI_6_5_ROOT_SYSTEM_DESCRIPTION_POINTER *pRsdp65;
  BMP_IMAGE_HEADER                             *BMP;

  // Load RSD
  if (EfiGetSystemConfigurationTable (Guid, (VOID **)&pRsdp65 ) == EFI_NOT_FOUND)
    return 0;

  if (pRsdp65->RsdtAddress)
    Entry = (EFI_ACPI_DESCRIPTION_HEADER *)pRsdp65->RsdtAddress;
  if ((pRsdp65->XsdtAddress) && (pRsdp65->Revision >= 2))
    Entry = (EFI_ACPI_DESCRIPTION_HEADER *)pRsdp65->XsdtAddress; 
  
  BGRT   = DumpBGRT (Entry);
  Status = DumpBGRTTable (BGRT);
  BMP    = (BMP_IMAGE_HEADER *)(BGRT->ImageAddress);

  //
  // Allocate new logo buffer
  //
  mLogoBltBuffer = AllocateCopyPool (BMP->Size, BMP);
  if (mLogoBltBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  DisplayBMPImage (BMP, mLogoBltBuffer);
  SaveToFile ((UINT8 *)BMP, BMP->Size);
  printf("BMP->Size=%lu BMP->HeaderSize=%lu BMP->ImageSize=%lu\n", BMP->Size, BMP->HeaderSize, BMP->ImageSize);


  if (mLogoBltBuffer != NULL) {
    FreePool (mLogoBltBuffer);
    printf("FreePool (mLogoBltBuffer) !\n");
  }
  return(0);
}