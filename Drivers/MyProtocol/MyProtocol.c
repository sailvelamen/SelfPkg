#include <Uefi.h>
#include <Library/UefiLib.h>

#include "MyProtocol.h"
// CMOS Library
#include <Library/MyCMOSLib.h>
#include <Library/DebugLib.h>  // MY_PRIVATE_DATA_FROM_THIS

EFI_GUID gEFIMYCmosProtocolGUID = EFI_MYCMOS_PRPTOCOL_GUID;

MY_PRIVATE_DATA gMyData;

EFI_STATUS EFIAPI MyCmos_In (IN EFI_MYCMOS_PRPTOCOL* This, IN UINT8 weekday)
{
    MY_PRIVATE_DATA *mydata;
    mydata          = MY_PRIVATE_DATA_FROM_THIS(This);
    mydata->second  = CmosRead8(0x00);
    mydata->minute  = CmosRead8(0x02);
    mydata->hour    = CmosRead8(0x04);
    mydata->weekday = CmosRead8(0x06);
    mydata->day     = CmosRead8(0x07);
    mydata->month   = CmosRead8(0x08);
    mydata->year    = CmosRead8(0x09);   
    mydata->weekday = weekday;
    Print(L"CMOSDATA = 20%xyear %2xmonth %2xday %xweekday %2x:%2x:%2x\n"
            , mydata->year, mydata->month, mydata->day, mydata->weekday, mydata->hour, mydata->minute, mydata->second);
}

EFI_STATUS EFIAPI MyCmos_Out (IN EFI_MYCMOS_PRPTOCOL* This)
{
    MY_PRIVATE_DATA *mydata;
    mydata          = MY_PRIVATE_DATA_FROM_THIS(This);
    mydata->second  = CmosRead8(0x00);
    mydata->minute  = CmosRead8(0x02);
    mydata->hour    = CmosRead8(0x04);
    mydata->weekday = CmosRead8(0x06);
    mydata->day     = CmosRead8(0x07);
    mydata->month   = CmosRead8(0x08);
    mydata->year    = CmosRead8(0x09);
    Print(L"CMOSDATA = 20%xyear %2xmonth %2xday %xweekday %2x:%2x:%2x\n"
            , mydata->year, mydata->month, mydata->day, mydata->weekday, mydata->hour, mydata->minute, mydata->second);
}

EFI_STATUS MyCmosProtocolInit(VOID)
{
    MY_PRIVATE_DATA *mydata = &gMyData;
    mydata->Signature = MY_PRIVATE_DATA_SIGNATURE;
    mydata->myProtocol.MyCmos_In = MyCmos_In;
    mydata->myProtocol.MyCmos_Out = MyCmos_Out;

    mydata->second  = 0;  // 0x00
    mydata->minute  = 0;  // 0x02
    mydata->hour    = 0;  // 0x04
    mydata->weekday = 0;  // 0x06
    mydata->day     = 0;  // 0x07
    mydata->month   = 0;  // 0x08
    mydata->year    = 0;  // 0x09

    Print (L"MyCmosProtocolInit\n");
    return EFI_SUCCESS;
}




