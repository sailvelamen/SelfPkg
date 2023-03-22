/** @file
  This is Sample of UEFI library. 
**/
#include <Uefi.h>
#include <Library/UefiLib.h>

#include <Library/DebugLib.h>
#include <Guid/Acpi.h>  // gEfiAcpiTableGuid
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>  // MCFG 
#include <Library/BaseMemoryLib.h>  // CompareMem()

/**
 * Some functions of my library: 2022-10-25 12:14:35 fanjix
 **/

VOID LibFunction(VOID)
{
  DEBUG ((DEBUG_INFO, "LibFunction() is called!(...)\n"));
}


RETURN_STATUS
EFIAPI
MyLibConstructor (VOID)
{
  DEBUG ((DEBUG_INFO, "MyLibConstructor() is called!\n"));
}

RETURN_STATUS
EFIAPI
MyLibDestructor (VOID)
{
  DEBUG ((DEBUG_INFO, "\nMyLibDestructor() is called!\n"));
}

/*++

Routine Description:

  This function prints a GUID to STDOUT.

Arguments:

  Guid    Pointer to a GUID to print.

Returns:

  EFI_SUCCESS             The GUID was printed.
  EFI_INVALID_PARAMETER   The input was NULL.

--*/
EFI_STATUS
PrintGuid (
  IN EFI_GUID *Guid
  )
{
  if (Guid == NULL) {
    // Error (NULL, 0, 2000, "Invalid parameter", "PrintGuidToBuffer() called with a NULL value");
    Print (L"ERROR!\n");
    return EFI_INVALID_PARAMETER;
  }

  Print (
    L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x\n",
    (unsigned) Guid->Data1,
    Guid->Data2,
    Guid->Data3,
    Guid->Data4[0],
    Guid->Data4[1],
    Guid->Data4[2],
    Guid->Data4[3],
    Guid->Data4[4],
    Guid->Data4[5],
    Guid->Data4[6],
    Guid->Data4[7]
    );
  return EFI_SUCCESS;
}

UINT64 GetBaseEcam (VOID)
{
  EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER                         *MmCfgHdr = NULL;
  EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE  *MmCfgBase = NULL;
  EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER                                           *pRsdp10b = NULL;
  EFI_ACPI_6_5_ROOT_SYSTEM_DESCRIPTION_POINTER                                           *pRsdp65 = NULL;
  // Load RSD
  EFI_STATUS Status = EfiGetSystemConfigurationTable ( &gEfiAcpiTableGuid, (VOID **)&pRsdp65 );
  if (Status == EFI_NOT_FOUND)
    EfiGetSystemConfigurationTable ( &gEfiAcpi10TableGuid, (VOID **)&pRsdp10b );
  // Load RSDT or XSDT
  EFI_ACPI_DESCRIPTION_HEADER *XSDT, *RSDT, *Entry;
  if (pRsdp10b->RsdtAddress)
    Entry = RSDT = (EFI_ACPI_DESCRIPTION_HEADER *)pRsdp65->RsdtAddress;
  if (pRsdp65->RsdtAddress)
    Entry = RSDT = (EFI_ACPI_DESCRIPTION_HEADER *)pRsdp65->RsdtAddress;
  if ((pRsdp65->XsdtAddress) && (pRsdp65->Revision >= 2))
    Entry = XSDT = (EFI_ACPI_DESCRIPTION_HEADER *)pRsdp65->XsdtAddress;

  UINT8 AdrLength;
  (!CompareMem (&Entry->Signature, "XSDT", 4)) ? (AdrLength = sizeof(UINT64)) : (AdrLength = sizeof(UINT32));
  UINT8 EntryCount = (Entry->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER)) / AdrLength;
  UINT32 *pEntry32;    
  UINT64 *pEntry64;
  if (AdrLength == sizeof(UINT64)) 
    pEntry64 = (UINT64 *)++Entry;  // 获取第一个Entry的地址
  else
    pEntry32 = (UINT32 *)++Entry;
  while (EntryCount--) {
    if (AdrLength == sizeof(UINT64)) {
      Entry = (EFI_ACPI_DESCRIPTION_HEADER *)(*pEntry64++);  // 获取第一个Entry中存的地址
      // if (Entry->Signature == 0x4746434D)
      if (Entry->Signature == EFI_ACPI_6_5_PCI_EXPRESS_MEMORY_MAPPED_CONFIGURATION_SPACE_BASE_ADDRESS_DESCRIPTION_TABLE_SIGNATURE)
        MmCfgHdr = (EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER *)&Entry->Signature;
        if (MmCfgHdr != NULL) {
          DEBUG ((DEBUG_INFO, "Found MM config address in Xsdt @ 0x%016llX\n", MmCfgHdr));
          MmCfgBase = (EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE *)((UINT8 *)MmCfgHdr + sizeof (*MmCfgHdr));
          DEBUG ((DEBUG_INFO, "ECAM_BASE = 0x%016llX\n", MmCfgBase->BaseAddress));
          return MmCfgBase->BaseAddress;
        }
    }
    else{
      Entry = (EFI_ACPI_DESCRIPTION_HEADER *)(*(pEntry32++));     
      if (Entry->Signature == EFI_ACPI_6_5_PCI_EXPRESS_MEMORY_MAPPED_CONFIGURATION_SPACE_BASE_ADDRESS_DESCRIPTION_TABLE_SIGNATURE)
        MmCfgHdr = (EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER *)&Entry->Signature;
        if (MmCfgHdr != NULL) {
          DEBUG ((DEBUG_INFO, "Found MM config address in Rsdt @ 0x%016llX\n", MmCfgHdr));
          MmCfgBase = (EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE *)((UINT8 *)MmCfgHdr + sizeof (*MmCfgHdr));
          DEBUG ((DEBUG_INFO, "ECAM_BASE = 0x%016llX\n", MmCfgBase->BaseAddress));
          return MmCfgBase->BaseAddress;
        }
    }
  }
}