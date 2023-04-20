#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/IoLib.h>

#include <stdio.h>
#include <stdlib.h>

#include <IndustryStandard/SmBios.h>  // SMBIOS_TABLE_ENTRY_POINT
#include <Guid/SmBios.h>              // gEfiSmbiosTableGuid
#include <Library/BaseMemoryLib.h>    // CompareMem
#include <Library/DebugLib.h>         // ASSERT

// #define INVALID_HANDLE  (UINT16) (-1)

STATIC SMBIOS_TABLE_ENTRY_POINT    *SmbiosTable = NULL;  // ESP
STATIC SMBIOS_STRUCTURE_POINTER    m_SmbiosStruct;
STATIC SMBIOS_STRUCTURE_POINTER    *mSmbiosStruct = &m_SmbiosStruct;


CHAR8 * LibGetSmbiosString (
  IN  SMBIOS_STRUCTURE_POINTER  *Smbios,
  IN  UINT16                    StringNumber
  )
{
  UINT16  Index;
  CHAR8   *String;

  ASSERT (Smbios != NULL);

  //
  // Skip over formatted section
  //
  String = (CHAR8 *)(Smbios->Raw + Smbios->Hdr->Length);

  //
  // Look through unformated section
  //
  for (Index = 1; Index <= StringNumber; Index++) {
    if (StringNumber == Index) {
      return String;
    }

    //
    // Skip string
    //
    for ( ; *String != 0; String++) {
    }

    String++;

    if (*String == 0) {
      //
      // If double NULL then we are done.
      //  Return pointer to next structure in Smbios.
      //  if you pass in a -1 you will always get here
      //
      Smbios->Raw = (UINT8 *)++String;
      return NULL;
    }
  }

  return NULL;
}


VOID DisplayESPInfo (SMBIOS_TABLE_ENTRY_POINT  *SmbiosTable)
{
    printf("==============================================\n");
    printf("Anchor String:         %c%c%c%c\n", 
        SmbiosTable->AnchorString[0], 
        SmbiosTable->AnchorString[1], 
        SmbiosTable->AnchorString[2], 
        SmbiosTable->AnchorString[3]);
    printf("EPS Checksum:          0x%02X\n", SmbiosTable->EntryPointStructureChecksum);
    printf("Entry Point Len:       %d\n", SmbiosTable->EntryPointLength);
    printf("Version:               %d.%d\n", SmbiosTable->MajorVersion, SmbiosTable->MinorVersion);
    printf("Number of Structures:  %d\n", SmbiosTable->NumberOfSmbiosStructures);
    printf("Max Struct size:       %d\n", SmbiosTable->MaxStructureSize);
    printf("Table Address:         0x%08X\n", SmbiosTable->TableAddress);
    printf("Table Length:          %d\n", SmbiosTable->TableLength);
    printf("Entry Point revision:  0x%02X\n", SmbiosTable->EntryPointRevision);
    printf("SMBIOS BCD Revision:   0x%02X\n", SmbiosTable->SmbiosBcdRevision);
    printf("Inter Anchor:          %c%c%c%c%c\n", 
        SmbiosTable->IntermediateAnchorString[0],
        SmbiosTable->IntermediateAnchorString[1],
        SmbiosTable->IntermediateAnchorString[2],
        SmbiosTable->IntermediateAnchorString[3],
        SmbiosTable->IntermediateAnchorString[4]);
    printf("Inter Checksum:        0x%02X\n", SmbiosTable->IntermediateChecksum);
    printf("FormattedArea:         %02x %02x %02x %02x %02x\n",
        SmbiosTable->FormattedArea[0],
        SmbiosTable->FormattedArea[1],
        SmbiosTable->FormattedArea[2],
        SmbiosTable->FormattedArea[3],
        SmbiosTable->FormattedArea[4]);
}

VOID DisplayDataInfo (
    SMBIOS_STRUCTURE_POINTER Smbios, 
    SMBIOS_STRUCTURE_POINTER SmbiosNext, 
    UINT8 *Raw, 
    UINT16 Length, 
    UINT32 Index
    )
{
    printf("Type=%d, Handle=0x%x\n", Smbios.Hdr->Type, Smbios.Hdr->Handle);
    LibGetSmbiosString (&SmbiosNext, (UINT16)(-1));
    Raw = Smbios.Raw;
    Length = (UINT16)(SmbiosNext.Raw - Raw);
    printf("Index=%d,Length=0x%X,Addr=0x%08X\n", Index, Length, (UINTN)Raw);  

    for (UINT8 i = 1 ; Raw < SmbiosNext.Raw; Raw++)
    {
        printf("%02X ", MmioRead8((UINTN)Raw));
        if ((i++ % 16) == 0)
            printf("\n");
    }
    printf("\nStructure Type: ");
    switch (Smbios.Hdr->Type)
    {
        case 0:    printf("BIOS Information\n");                      break;
        case 1:    printf("System Information\n");                    break;
        case 2:    printf("Base Board Information\n");                break;
        case 3:    printf("System Enclosure\n");                      break;
        case 4:    printf("Processor Information\n");                 break;
        case 5:    printf("Memory Controller Information\n");         break;
        case 6:    printf("Memory Module Information\n");             break;
        case 7:    printf("Cache Information\n");                     break;
        case 8:    printf("Port Connector Information\n");            break;
        case 9:    printf("System Slots\n");                          break;
        case 10:   printf("On Board Devices Information\n");          break;
        case 11:   printf("OEM Strings\n");                           break;
        case 12:   printf("System Configuration Options\n");          break;
        case 13:   printf("BIOS Language Information\n");             break;
        case 14:   printf("Group Associations\n");                    break;
        case 15:   printf("System Event Log\n");                      break;
        case 16:   printf("Physical Memory Array\n");                 break;
        case 17:   printf("Memory Device\n");                         break;
        case 18:   printf("32-bit Memory Error Information\n");       break;
        case 19:   printf("Memory Array Mapped Address\n");           break;
        case 20:   printf("Memory Device Mapped Address\n");          break;
        case 21:   printf("Built-in Pointing Device\n");              break;
        case 22:   printf("Portable Battery\n");                      break;
        case 23:   printf("System Reset\n");                          break;
        case 24:   printf("Hardware Security\n");                     break;
        case 25:   printf("System Power Controls\n");                 break;
        case 26:   printf("Voltage Probe\n");                         break;
        case 27:   printf("Cooling Device\n");                        break;
        case 28:   printf("Temperature Probe\n");                     break;
        case 29:   printf("Electrical Current Probe\n");              break;
        case 32:   printf("System Boot Information\n");               break;
        case 33:   printf("64-bit Memory Channel Type\n");            break;
        case 34:   printf("Management Device\n");                     break;
        case 35:   printf("Management Device Component\n");           break;
        case 36:   printf("Management Device Threshold Data\n");      break;
        case 38:   printf("IPMI Device Information\n");               break;
        case 41:   printf("Onboard Devices Extended Information\n");  break;
        case 126:  printf("Inactive\n");                              break;
        case 127:  printf("End-of-Table\n");                          break;
        default:   printf("Undefined Value\n");                       break;
    }
    printf("Format part Len : %d\n", Smbios.Hdr->Length);
    printf("Structure Handle: %d\n", Smbios.Hdr->Handle);
}

VOID EnumSmbiosTable(
    SMBIOS_STRUCTURE_POINTER *mSmbiosStruct, 
    UINT16 NumberOfSmbiosStructures, 
    UINT8 Type, 
    BOOLEAN flag
    )
{
    SMBIOS_STRUCTURE_POINTER  Smbios, SmbiosNext;
    UINT16                    Length = 0;
    UINT8                     *Raw;

    SmbiosNext.Raw = Smbios.Raw = mSmbiosStruct->Raw;
    printf("===============================================\n");
    for (UINT32 Index = 0; Index < NumberOfSmbiosStructures; Index++)
    {   
        if (!flag)
        {
            DisplayDataInfo (Smbios, SmbiosNext, Raw, Length, Index);
            LibGetSmbiosString (&SmbiosNext, (UINT16)(-1));
            Smbios.Raw = SmbiosNext.Raw;
            printf("\n===============================================\n");
        }   
        else
        {
            if (Smbios.Hdr->Type == Type)
            {
                DisplayDataInfo (Smbios, SmbiosNext, Raw, Length, Index);
                LibGetSmbiosString (&SmbiosNext, (UINT16)(-1));
                Smbios.Raw = SmbiosNext.Raw;
                printf("\n===============================================\n");
            }
            else
            {
                LibGetSmbiosString (&SmbiosNext, (UINT16)(-1));
                Smbios.Raw = SmbiosNext.Raw;
            }
        }  
    }
}

int main(IN int Argc, IN char **Argv)
{
    // printf("Hello,this is Entry of main!()\n");

    EFI_STATUS Status;

    // get ESP Table
    Status = EfiGetSystemConfigurationTable (&gEfiSmbiosTableGuid, (VOID **)&SmbiosTable); 
    if ((Status != EFI_SUCCESS) || (SmbiosTable == NULL) || (CompareMem (SmbiosTable->AnchorString, "_SM_", 4) != 0))
    {
        printf("SmBios Table is not found!");
        return Status;
    }

    SMBIOS_STRUCTURE_POINTER   SmBiosStruct;
    BOOLEAN flag = FALSE;
    UINT8 Type = 0;
    switch (Argc)
    {
        case 1:
            DisplayESPInfo(SmbiosTable);
            // Get Start Address
            mSmbiosStruct->Raw = (UINT8 *)(UINTN)(SmbiosTable->TableAddress);
            SmBiosStruct.Raw = mSmbiosStruct->Raw;
            // Traverse SmBios
            EnumSmbiosTable(&SmBiosStruct, SmbiosTable->NumberOfSmbiosStructures, Type, flag);
            break;
        case 3:
            if (CompareMem(Argv[1], "-t", 2) == 0)
            {
                flag = TRUE;
                sscanf(Argv[2], "%d", &Type);
                DisplayESPInfo(SmbiosTable);
                // Get Start Address
                mSmbiosStruct->Raw = (UINT8 *)(UINTN)(SmbiosTable->TableAddress);
                SmBiosStruct.Raw = mSmbiosStruct->Raw;
                // Traverse SmBios
                EnumSmbiosTable(&SmBiosStruct, SmbiosTable->NumberOfSmbiosStructures, Type, flag);
            }
    }
    

    return 0;
}