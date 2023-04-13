#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/UefiBootServicesTableLib.h>    //gST,gBs
#include <Library/UefiRuntimeServicesTableLib.h> //gRT

#include <stdio.h>
// #include <stdlib.h>

#include <Library/DebugLib.h>
#include <Guid/Acpi.h>  // gEfiAcpiTableGuid
#include <Protocol/AcpiTable.h>  // EFI_ACPI_TABLE_PROTOCOL
#include <Library/BaseMemoryLib.h>  // CompareMem()
#include <Library/ShellCommandLib.h>  // DumpHex()

VOID PrintSignature (UINT64 Signature, UINT8 Length)
{
  for (UINT8 i = 0; i < Length; i++)
    ( printf("%c", Signature & 0xFF), Signature >>= 8 );  //逗号运算符
}

VOID *DumpRSDP65 (IN EFI_GUID *Guid)
{
  printf("\n---------------------------------------------------------------------------------\n");
  EFI_ACPI_6_5_ROOT_SYSTEM_DESCRIPTION_POINTER * pRsdp65;
  // Load RSD
  if (EfiGetSystemConfigurationTable ( Guid, (VOID **)&pRsdp65 ) == EFI_NOT_FOUND)
    return NULL;
  printf("RSD_65 @ 0x%016llX\n", (UINT64)pRsdp65);

  printf("        Signature: 0x%llX  ", pRsdp65->Signature);
  PrintSignature (pRsdp65->Signature, sizeof (pRsdp65->Signature));
  printf("  @ 0x%016llX\n", &pRsdp65->Signature);
  printf("         Checksum: 0x%-16X            @ 0x%016llX\n", pRsdp65->Checksum, &pRsdp65->Checksum);
  printf("           OEM_ID: 0x");
  for (char i = 5; i >=0; i--)
    printf("%x", pRsdp65->OemId[i]);
  printf("      ");
  for (UINT8 i = 0; i < 6; i++)
    printf("%c", pRsdp65->OemId[i]);
  printf("    @ 0x%016llX\n", pRsdp65->OemId);
  printf("         Revision: %-18u            @ 0x%016llX\n", pRsdp65->Revision, &pRsdp65->Revision);
  printf("             RSDT: 0x%016llX            @ 0x%016llX\n", pRsdp65->RsdtAddress, &pRsdp65->RsdtAddress);
  printf("           Length: %-18u            @ 0x%016llX\n", pRsdp65->Length, &pRsdp65->Length);
  printf("             XSDT: 0x%016llX            @ 0x%016llX\n", pRsdp65->XsdtAddress, &pRsdp65->XsdtAddress);
  printf(" ExtendedChecksum: 0x%-16X            @ 0x%016llX\n\n", pRsdp65->ExtendedChecksum, &pRsdp65->ExtendedChecksum);
  DumpHex (2, 0, pRsdp65->Length, pRsdp65);   

  return pRsdp65;
}

VOID DumpACPIHeader (EFI_ACPI_DESCRIPTION_HEADER *Entry)
{
  printf("---------------------------------------------------------------------------------\n");
  PrintSignature (Entry->Signature, sizeof (Entry->Signature));
  printf(" @ 0x%016llX\n", Entry);
  printf("       Signature: 0x%-16lX  ", Entry->Signature);
  PrintSignature (Entry->Signature, sizeof (Entry->Signature));
  printf("      @ 0x%016llX\n", &Entry->Signature);
  printf("          Length: %-18u            @ 0x%016llX\n", Entry->Length, &Entry->Length);
  printf("        Revision: %-18u            @ 0x%016llX\n", Entry->Revision, &Entry->Revision);
  printf("        Checksum: 0x%-16X            @ 0x%016llX\n", Entry->Checksum, &Entry->Checksum);
  printf("          OEM_ID: 0x");
  for (char i = 5; i >=0; i--)
    printf("%x", Entry->OemId[i]);
  printf("      ");
  for (UINT8 i = 0; i < 6; i++)
    printf("%c", Entry->OemId[i]);
  printf("    @ 0x%016llX\n", Entry->OemId);
  printf("      OemTableId: 0x%016llX  ", Entry->OemTableId);
  UINT64 OemTableId = Entry->OemTableId;
  for (UINT8 i = 1; i <= 8; i++) {
    printf("%c", OemTableId & 0xFF);
    OemTableId >>= 8;
  }
  printf("  @ 0x%016llX\n", &Entry->OemTableId);
  printf("     OemRevision: %-18lu            @ 0x%016llX\n", Entry->OemRevision, &Entry->OemRevision);
  printf("       CreatorId: 0x%-16lX            @ 0x%016llX\n", Entry->CreatorId, &Entry->CreatorId);
  printf(" CreatorRevision: 0x%-16lX            @ 0x%016llX\n\n", Entry->CreatorRevision, &Entry->CreatorRevision);
  DumpHex (2, 0, Entry->Length, Entry);  printf("\n");
}

VOID DumpDSDT(EFI_ACPI_DESCRIPTION_HEADER *Entry)
{
  EFI_ACPI_DESCRIPTION_HEADER *DSDT;
  EFI_ACPI_6_5_FIXED_ACPI_DESCRIPTION_TABLE *FACP = (EFI_ACPI_6_5_FIXED_ACPI_DESCRIPTION_TABLE *)Entry;
  if (FACP->XDsdt) {
    // Print (L"FACP->XDsdt: 0x%016llX\n", FACP->XDsdt);
    DSDT = (EFI_ACPI_DESCRIPTION_HEADER *)FACP->XDsdt;
    DumpACPIHeader (DSDT);
  }
  else {
    // Print (L"FACP->Dsdt: 0x%016llX\n", FACP->Dsdt);
    DSDT = (EFI_ACPI_DESCRIPTION_HEADER *)FACP->Dsdt;
    DumpACPIHeader (DSDT);
  }
}

VOID DumpACPI(EFI_ACPI_DESCRIPTION_HEADER *Entry)
{
  UINT8 AdrLength;
  (!CompareMem (&Entry->Signature, "XSDT", 4)) ? (AdrLength = sizeof(UINT64)) : (AdrLength = sizeof(UINT32));
  UINT8 EntryCount = (Entry->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER)) / AdrLength;
  // printf("EntryCount = %d\n", EntryCount);
  UINT32 *pEntry32;    
  UINT64 *pEntry64;
  if (AdrLength == sizeof(UINT64)) 
    pEntry64 = (UINT64 *)++Entry;  // 获取第一个Entry的地址
  else
    pEntry32 = (UINT32 *)++Entry;
  while (EntryCount--) {
    if (AdrLength == sizeof(UINT64)) {
      Entry = (EFI_ACPI_DESCRIPTION_HEADER *)(*pEntry64++);  // 获取第一个Entry中存的地址
      DumpACPIHeader (Entry);
      if (Entry->Signature == EFI_ACPI_6_5_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE)
        DumpDSDT (Entry);
    }
    else {
      Entry = (EFI_ACPI_DESCRIPTION_HEADER *)(*(pEntry32++));     
      DumpACPIHeader (Entry);
      if (Entry->Signature == EFI_ACPI_6_5_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE)
        DumpDSDT (Entry);
    }
  }
}