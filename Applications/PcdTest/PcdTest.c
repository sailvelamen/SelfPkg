#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>

EFI_STATUS
EFIAPI
PcdTestEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  Print (L"PcdTestEntryPoint Entry!\n");
  
  if (FeaturePcdGet (PcdHelloWorldPrintEnable) == TRUE)
    for (UINT32 Index = 0; Index < PcdGet32 (PcdHelloWorldPrintLoops); Index++)
      Print ((CHAR16 *)PcdGetPtr (PcdHelloWorldPrintString), Index);

  Print (L"PcdTestEntryPoint End!\n");
  return EFI_SUCCESS;
}