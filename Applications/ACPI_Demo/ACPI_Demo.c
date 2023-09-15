#include "ACPI_Demo.h"

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
    DEBUG ((DEBUG_INFO, "VendorGuid[%02d]: %g\n", Index, EfiConfigurationTable[Index].VendorGuid));

  EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER * pRsdp10b;
  EFI_ACPI_6_5_ROOT_SYSTEM_DESCRIPTION_POINTER * pRsdp65 = DumpRSDP65(Guid); 
  if (pRsdp65 == NULL)
    goto Rsdp10;
  EFI_ACPI_DESCRIPTION_HEADER *XSDT, *RSDT, *Entry;
  if (pRsdp65->RsdtAddress)
    Entry = RSDT = (EFI_ACPI_DESCRIPTION_HEADER *)pRsdp65->RsdtAddress;
  if ((pRsdp65->XsdtAddress) && (pRsdp65->Revision >= 2))
    Entry = XSDT = (EFI_ACPI_DESCRIPTION_HEADER *)pRsdp65->XsdtAddress;
  
  DumpACPIHeader (RSDT);
  DumpACPIHeader (XSDT);
  DumpACPI (Entry);
  printf("=================================================================================\n");
  
  return 0;

Rsdp10:
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