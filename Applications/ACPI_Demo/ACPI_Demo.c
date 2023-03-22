#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/UefiBootServicesTableLib.h>    //gST,gBs
#include <Library/UefiRuntimeServicesTableLib.h> //gRT

#include <stdio.h>
// #include <stdlib.h>

#include <Library/DebugLib.h>
#include <Guid/Acpi.h>  // gEfiAcpiTableGuid
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
  EfiGetSystemConfigurationTable ( Guid, (VOID **)&pRsdp65 );
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
  printf("      OemTableId: 0x%016llX            @ 0x%016llX\n", Entry->OemTableId, &Entry->OemTableId);
  printf("     OemRevision: %-18lu            @ 0x%016llX\n", Entry->OemRevision, &Entry->OemRevision);
  printf("       CreatorId: 0x%-16lX            @ 0x%016llX\n", Entry->CreatorId, &Entry->CreatorId);
  printf(" CreatorRevision: 0x%-16lX            @ 0x%016llX\n\n", Entry->CreatorRevision, &Entry->CreatorRevision);
  DumpHex (2, 0, Entry->Length, Entry);  printf("\n");
}

VOID DumpACPI(EFI_ACPI_DESCRIPTION_HEADER *Entry)
{
  UINT8 AdrLength;
  (!CompareMem (&Entry->Signature, "XSDT", 4)) ? (AdrLength = sizeof(UINT64)) : (AdrLength = sizeof(UINT32));
  UINT8 EntryCount = (Entry->Length - sizeof (EFI_ACPI_DESCRIPTION_HEADER)) / AdrLength;
  printf("EntryCount = %d\n", EntryCount);
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
    }
    else{
      Entry = (EFI_ACPI_DESCRIPTION_HEADER *)(*(pEntry32++));     
      DumpACPIHeader (Entry);
    }
  }
}

int main (IN int Argc, IN char **Argv)
{
  printf("\nHello,this is Entry of main!()\n");
  
  EFI_CONFIGURATION_TABLE  *EfiConfigurationTable = gST->ConfigurationTable;
  EFI_GUID  *Guid = &gEfiAcpiTableGuid;
  Print (L"gEfiAcpiTableGuid:   %g\n", Guid);
  Print (L"gEfiAcpi10TableGuid: %g\n", &gEfiAcpi10TableGuid);
  Print (L"gEfiAcpi20TableGuid: %g\n", &gEfiAcpi20TableGuid);
  printf("gST->NumberOfTableEntries = %d\n", gST->NumberOfTableEntries);
  for (UINTN Index = 0; Index < gST->NumberOfTableEntries; Index++) 
    DEBUG ((DEBUG_INFO, "VendorGuid[%d]: %g\n", Index, EfiConfigurationTable[Index].VendorGuid));

  EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER * pRsdp10b;
  EFI_ACPI_6_5_ROOT_SYSTEM_DESCRIPTION_POINTER * pRsdp65 = DumpRSDP65(Guid); 

  EFI_ACPI_DESCRIPTION_HEADER *XSDT, *RSDT, *Entry;
  if (pRsdp65->RsdtAddress)
    Entry = RSDT = (EFI_ACPI_DESCRIPTION_HEADER *)pRsdp65->RsdtAddress;
  if ((pRsdp65->XsdtAddress) && (pRsdp65->Revision >= 2))
    Entry = XSDT = (EFI_ACPI_DESCRIPTION_HEADER *)pRsdp65->XsdtAddress;
  
  DumpACPIHeader (RSDT);
  DumpACPIHeader (XSDT);
  DumpACPI (Entry);

  printf("=================================================================================\n");

  EfiGetSystemConfigurationTable ( &gEfiAcpi10TableGuid, (VOID **)&pRsdp10b );
  printf("\nRSD_10b @ 0x%016llX\n", (UINT64)pRsdp10b);
  printf("  Signature: 0x%llX  ", pRsdp10b->Signature);
  PrintSignature (pRsdp10b->Signature, sizeof (pRsdp10b->Signature));
  printf("  @ 0x%016llX\n", &pRsdp65->Signature);
  printf("   Checksum: 0x%-16X            @ 0x%016llX\n", pRsdp10b->Checksum, &pRsdp65->Checksum);
  printf("     OEM_ID: 0x");
  for (char i = 5; i >=0; i--)
    printf("%x", pRsdp10b->OemId[i]);
  printf("      ");
  for (UINT8 i = 0; i < 6; i++)
    printf("%c", pRsdp10b->OemId[i]);
  printf("    @ 0x%016llX\n", pRsdp10b->OemId);
  printf("       RSDT: 0x%016llX            @ 0x%016llX\n\n", pRsdp10b->RsdtAddress, &pRsdp10b->RsdtAddress);
  DumpHex (2, 0, sizeof(EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER), pRsdp10b);

  Entry = RSDT = (EFI_ACPI_DESCRIPTION_HEADER *)pRsdp10b->RsdtAddress;
  DumpACPI (Entry);

  return 0;
}