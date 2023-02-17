/** @file
  This is Sample of UEFI library. 
**/
#include <Uefi.h>
#include <Library/UefiLib.h>

#include <stdio.h>
#include <stdlib.h>

/**
 * Some functions of my library: 2022-10-25 12:14:35 fanjix
 **/

VOID LibFunction(VOID)
{
  Print(L"LibFunction() is called!(...)\n");
}


RETURN_STATUS
EFIAPI
MyLibConstructor (VOID )
{
  Print(L"MyLibConstructor() is called!\n");
}

RETURN_STATUS
EFIAPI
MyLibDestructor (VOID)
{
  Print(L"\nMyLibDestructor() is called!\n");
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
    printf("ERROR!\n");
    return EFI_INVALID_PARAMETER;
  }

  printf (
    "%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x\n",
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
