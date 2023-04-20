/** @file
  PC/AT CMOS access routines

  Copyright (c) 2006 - 2009, Intel Corporation. All rights reserved.<BR>
  Copyright (c) 2019, Citrix Systems, Inc.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/IoLib.h>

/**
  Reads 8-bits of CMOS data.

  Reads the 8-bits of CMOS data at the location specified by Index.
  The 8-bit read value is returned.

  @param  Index  The CMOS location to read.

  @return The value read.

**/
UINT8
EFIAPI
CmosRead8 (
  IN      UINTN  Index
  )
{
  UINT8 flag = IoRead8(0x70);
  flag = flag & 0x80;  // save 70 port bit7
  Index = Index & 0x7F;
  Index = Index | flag;
  
  IoWrite8 (0x70, (UINT8)Index);
  return IoRead8 (0x71);
}

/**
  Writes 8-bits of CMOS data.

  Writes 8-bits of CMOS data to the location specified by Index
  with the value specified by Value and returns Value.

  @param  Index  The CMOS location to write.
  @param  Value  The value to write to CMOS.

  @return The value written to CMOS.

**/
VOID CmosWrite8 (
  IN      UINTN  Index,
  IN      UINT8  Value
  )
{
  Index = (Index & 0x7F) | (IoRead8(0x70) & 0x80);

  IoWrite8 (0x70, (UINT8)Index);
  IoWrite8 (0x71, Value);
}
