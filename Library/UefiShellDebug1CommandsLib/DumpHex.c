/** @file
  Main file for Pci shell Debug1 function.

  Copyright (c) 2005 - 2021, Intel Corporation. All rights reserved.<BR>
  (C) Copyright 2013-2015 Hewlett-Packard Development Company, L.P.<BR>
  (C) Copyright 2016 Hewlett Packard Enterprise Development LP<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "UefiShellDebug1CommandsLib.h"

/**
  Function for 'DumpHex' command.

  @param[in] ImageHandle  Handle to the Image (NULL if Internal).
  @param[in] SystemTable  Pointer to the System Table (NULL if Internal).
**/
SHELL_STATUS
EFIAPI
ShellCommandRunDumpHex (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS    Status;
  LIST_ENTRY    *Package;
  CHAR16        *ProblemParam;
  SHELL_STATUS  ShellStatus;
  CONST CHAR16  *Temp;
  UINT64        Addr;
  UINTN         Length;
  UINT64        RetVal;

  ProblemParam = NULL;
  ShellStatus  = SHELL_SUCCESS;
  Addr         = 0;
  Length       = 0;
  RetVal       = 0;

  //
  // initialize the shell lib (we must be in non-auto-init...)
  //
  Status = ShellInitialize ();
  ASSERT_EFI_ERROR (Status);

  //
  // parse the command line
  //
  Status = ShellCommandLineParse (EmptyParamList, &Package, &ProblemParam, TRUE);
  if (EFI_ERROR (Status)) {
    if ((Status == EFI_VOLUME_CORRUPTED) && (ProblemParam != NULL)) {
      ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_PROBLEM), gShellDebug1HiiHandle, L"DumpHex", ProblemParam);
      FreePool (ProblemParam);
      ShellStatus = SHELL_INVALID_PARAMETER;
    } else {
      ASSERT (FALSE);
    }
  } else {
    if (ShellCommandLineGetCount (Package) < 3) {
      ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_TOO_FEW), gShellDebug1HiiHandle, L"DumpHex");
      ShellStatus = SHELL_INVALID_PARAMETER;
      goto Done;
    }

    if (ShellCommandLineGetCount (Package) > 3) {
      ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_TOO_MANY), gShellDebug1HiiHandle, L"DumpHex");
      ShellStatus = SHELL_INVALID_PARAMETER;
      goto Done;
    }
  }

  // Print (L"ShellCommandLineGetCount = %d\n", ShellCommandLineGetCount (Package));
  Temp = ShellCommandLineGetRawValue (Package, 1);
  if (Temp != NULL) {
    //
    // Input converted to hexadecimal number.
    //
    if (!EFI_ERROR (ShellConvertStringToUint64 (Temp, &RetVal, TRUE, TRUE))) {
      Addr = RetVal;    
    } else {
      ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_PARAM_INV_HEX), gShellDebug1HiiHandle, L"DumpHex", Temp);
      ShellStatus = SHELL_INVALID_PARAMETER;
      goto Done;
    }
  }  

  Temp = ShellCommandLineGetRawValue (Package, 2);
  if (Temp != NULL) {
    //
    // Input converted to hexadecimal number.
    //
    if (!EFI_ERROR (ShellConvertStringToUint64 (Temp, &RetVal, TRUE, TRUE))) {
      Length = RetVal;    
    } else {
      ShellPrintHiiEx (-1, -1, NULL, STRING_TOKEN (STR_GEN_PARAM_INV_HEX), gShellDebug1HiiHandle, L"DumpHex", Temp);
      ShellStatus = SHELL_INVALID_PARAMETER;
      goto Done;
    }
  }
  Print (L"===============================================================================\n");
  Print (L"Addr = 0x%08X, Length = 0x%X\n\n", Addr, Length);
  DumpHex (2, 0, Length, (VOID *)Addr);
  Print (L"===============================================================================\n");
Done:
  if (Package != NULL) {
    ShellCommandLineFreeVarList (Package);
  }


  return (ShellStatus);
}