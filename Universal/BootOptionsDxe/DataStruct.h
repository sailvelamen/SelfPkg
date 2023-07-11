/** @file

Copyright (c) 2007 - 2018, Intel Corporation. All rights reserved.<BR>*
(C) Copyright 2020 Hewlett Packard Enterprise Development LP<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

Module Name:

  DataStruc.h

Abstract:

  Data structure used by the sample driver

Revision History:


**/


#define BOOT_OPTIONS_FORMSET_GUID  { 0x3b79d625, 0x32d4, 0xbb02, { 0x25, 0x2c, 0xe6, 0xe0, 0x9a, 0x93, 0x1a, 0x96 }}
#define TEST_MANAGER_FORMSET_GUID  { 0xa9dd1a1c, 0xa671, 0x9753, { 0x3e, 0xa9, 0x90, 0xfa, 0x1d, 0xa7, 0x61, 0xa2 }}

#define DEVICE_MANAGER_FORM_ID     0x1000

#define LABEL_DEVICES_LIST         0x1100
#define LABEL_FORM_ID_OFFSET       0x1234
#define LABEL_END                  0xffff