
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/IoLib.h>

#define EFI_ACPI_ENABLE_SW_SMI 0x38;

EFI_STATUS
EFIAPI
TestDemoEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  UINT8 SW_SMI_Port = EFI_ACPI_ENABLE_SW_SMI;
  IoWrite8 (0xB2, SW_SMI_Port);
  if (IoRead8(0xB2) == SW_SMI_Port)
  {
    Print (L"0xB2 = %x\n", IoRead8(0xB2));
    Print (L"SwSmi Write Successful!\n");
  }

  return EFI_SUCCESS;
}
