#ifndef MYPROTOCOL_H
#define MYPROTOCOL_H

#include <Uefi.h>

// {DEBC5904-0AA3-4396-9361-B832350E5328}
#define EFI_MYCMOS_PRPTOCOL_GUID { 0xdebc5904, 0xaa3, 0x4396, { 0x93, 0x61, 0xb8, 0x32, 0x35, 0xe, 0x53, 0x28 } };
#define MY_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('C', 'M', 'O', 'S')
// #define _CR(Record, TYPE, Field)  ((TYPE *) ((CHAR8 *) (Record) - (CHAR8 *) &(((TYPE *) 0)->Field)))
#define MY_PRIVATE_DATA_FROM_THIS(a) CR (a, MY_PRIVATE_DATA, myProtocol, MY_PRIVATE_DATA_SIGNATURE)

typedef struct _EFI_MYCMOS_PRPTOCOL EFI_MYCMOS_PRPTOCOL;

typedef EFI_STATUS (EFIAPI* EFI_MYCMOS_IN) (IN EFI_MYCMOS_PRPTOCOL* This, IN UINT8 weekday);

typedef EFI_STATUS (EFIAPI* EFI_MYCMOS_OUT) (IN EFI_MYCMOS_PRPTOCOL* This);

struct _EFI_MYCMOS_PRPTOCOL
{
    EFI_MYCMOS_IN   MyCmos_In;
    EFI_MYCMOS_OUT  MyCmos_Out;
};

typedef struct 
{
    UINTN                Signature;
    EFI_MYCMOS_PRPTOCOL  myProtocol;
    UINT8                second;  // 0x00
    UINT8                minute;  // 0x02
    UINT8                hour;    // 0x04
    UINT8                weekday; // 0x06
    UINT8                day;     // 0x07
    UINT8                month;   // 0x08
    UINT8                year;    // 0x09
} MY_PRIVATE_DATA;

EFI_STATUS MyCmosProtocolInit(VOID);


#endif