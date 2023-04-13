/** @file

  This file contains a sample implementation of the Platform Runtime Mechanism (PRM)
  SSDT Install library.

  Copyright (c) Microsoft Corporation
  Copyright (c) 2020, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/
#include <Uefi.h>
#include <PiDxe.h>  // EFI_SECTION_RAW
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/DxeServicesLib.h>  // GetSectionFromFv
#include <Library/UefiBootServicesTableLib.h>  //gST,gBs
#include <Protocol/AcpiTable.h>  // EFI_ACPI_TABLE_PROTOCOL
#include <IndustryStandard/Acpi.h>  //EFI_ACPI_DESCRIPTION_HEADER

#include "Fanx.h"

#define _DBGMSGID_  "[FANXSSDTINSTALL]"
extern GUID  gEfiCallerIdGuid;

EFI_STATUS
InstallFanxSsdt ( VOID )
{
  EFI_STATUS                   Status;
  UINTN                        TableSize;
  UINTN                        TableKey = 0;
  EFI_ACPI_TABLE_PROTOCOL      *mAcpiTableProtocol;
  EFI_ACPI_DESCRIPTION_HEADER  *Table;
  UINTN                        SectionInstance = 0;

  DEBUG ((DEBUG_INFO, "%a %a - Entry.\n", _DBGMSGID_, __FUNCTION__));

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&mAcpiTableProtocol);
  if (!EFI_ERROR (Status)) {
    //
    // Discover the SSDT
    //
    Status =  GetSectionFromFv (
                &gEfiCallerIdGuid, // 当前drive的GUID,就是在.inf文件的FILE_GUID
                EFI_SECTION_RAW,
                SectionInstance,
                (VOID **)&Table,
                &TableSize
                );
    if (EFI_ERROR (Status))
    {
      DEBUG ((DEBUG_INFO, "%a %a: SSDT loaded...\n", _DBGMSGID_, __FUNCTION__));
      Status   = mAcpiTableProtocol->InstallAcpiTable (
                                        mAcpiTableProtocol,
                                        (VOID **)&FANX_aml,
                                        FANX_aml_size,
                                        &TableKey
                                        );      
      ASSERT_EFI_ERROR (Status);
      DEBUG ((DEBUG_INFO, "%a %a - End.\n", _DBGMSGID_, __FUNCTION__));
      return Status;
    }
    DEBUG ((DEBUG_INFO, "%a gEfiCallerIdGuid: %g \n", _DBGMSGID_, gEfiCallerIdGuid));
    // DEBUG ((DEBUG_INFO, "%a EFI_SECTION_RAW:  0x%x \n", _DBGMSGID_, EFI_SECTION_RAW));
    if (Table->OemTableId == SIGNATURE_64 ('J', 'i', ',', 'F', 'a', 'n', 'X', ' ')) {
      DEBUG ((DEBUG_INFO, "%a %a: SSDT loaded...\n", _DBGMSGID_, __FUNCTION__));
      //
      // Publish the SSDT. Table is re-checksummed.
      //
      Status   = mAcpiTableProtocol->InstallAcpiTable (
                                        mAcpiTableProtocol,
                                        Table,
                                        TableSize,
                                        &TableKey
                                        );
      ASSERT_EFI_ERROR (Status);
    }
  }
  DEBUG ((DEBUG_INFO, "%a %a - End.\n", _DBGMSGID_, __FUNCTION__));

  return Status;
}

/**
  The entry point for this module.

  @param[in]  ImageHandle    The firmware allocated handle for the EFI image.
  @param[in]  SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS    The entry point is executed successfully.
  @retval Others         An error occurred when executing this entry point.

**/
EFI_STATUS
EFIAPI
InstallACPIEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  Status = InstallFanxSsdt ();
  ASSERT_EFI_ERROR (Status);

  return Status;
}
