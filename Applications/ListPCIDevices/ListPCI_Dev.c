#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/ShellCEntryLib.h>
#include <Library/UefiBootServicesTableLib.h>    //gST,gBs
#include <Library/UefiRuntimeServicesTableLib.h> //gRT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Library/IoLib.h>

// #include <Protocol/PciIo.h>			    //获取PciIO protocol所需
// #include <Protocol/PciRootBridgeIo.h>	//获取PciRootBridgeIO protocol所需
// #include <IndustryStandard/Pci.h>       // PCI_TYPE00

// EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *gPCIRootBridgeIO;
// EFI_PCI_IO_PROTOCOL *gPCIIOArray[256];

// EFI_STATUS PciDevicePresent (
//   IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *PciRootBridgeIo,
//   OUT PCI_TYPE00                       *Pci,
//   IN  UINT8                            Bus,
//   IN  UINT8                            Device,
//   IN  UINT8                            Func
//   )
// {
//   UINT64      Address;
//   EFI_STATUS  Status;

//   //
//   // Create PCI address map in terms of Bus, Device and Func
//   //
//   Address = EFI_PCI_ADDRESS (Bus, Device, Func, 0);

//   //
//   // Read the Vendor ID register
//   //
//   Status = PciRootBridgeIo->Pci.Read (
//                                   PciRootBridgeIo,
//                                   EfiPciWidthUint32,
//                                   Address,
//                                   1,
//                                   Pci
//                                   );

//   if (!EFI_ERROR (Status) && ((Pci->Hdr).VendorId != 0xffff)) {
//     //
//     // Read the entire config header for the device
//     //
//     Status = PciRootBridgeIo->Pci.Read (
//                                     PciRootBridgeIo,
//                                     EfiPciWidthUint32,
//                                     Address,
//                                     sizeof (PCI_TYPE00) / sizeof (UINT32),
//                                     Pci
//                                     );

//     return EFI_SUCCESS;
//   }

//   return EFI_NOT_FOUND;
// }

// EFI_STATUS LocatePCI_Bridge(void)
// {
//     EFI_STATUS Status;
//     EFI_HANDLE *PciHandleBuffer;
//     UINTN      HandleCount = 0;
//     // 获取Handle
//     Status = gBS->LocateHandleBuffer(
//         ByProtocol,
//         &gEfiPciRootBridgeIoProtocolGuid,
//         NULL,
//         &HandleCount,
//         &PciHandleBuffer
//     );
//     if (EFI_ERROR(Status))
//         return Status;
//     printf("Find PCI Root Bridge: %d\n", HandleCount);
//     // 获取Protocol实例
//     for (UINTN HandleIndex = 0; HandleIndex < HandleCount; HandleIndex++)
//     {
//         Status = gBS->HandleProtocol(
//             PciHandleBuffer[HandleIndex],
//             &gEfiPciRootBridgeIoProtocolGuid,
//             (VOID**)&gPCIRootBridgeIO
//         );
//         if (EFI_ERROR(Status))
//             continue;
//         else
//             return EFI_SUCCESS;
//     }
//     return Status;
// }

// EFI_STATUS ListPCI_Devices_Bridge(void)
// {
//     EFI_STATUS Status;
//     PCI_TYPE00 Pci;
//     UINT16 count = 0;
//     for (UINT8 i = 0; i < PCI_MAX_BUS; i++)
//         for (UINT8 j = 0; j < PCI_MAX_DEVICE; j++)
//             for (UINT8 k = 0; k < PCI_MAX_FUNC; k++)
//             {   // 判断设备是否存在
//                 Status = PciDevicePresent (gPCIRootBridgeIO, &Pci, i , j, k);
//                 if (Status == EFI_SUCCESS)  // 设备存在
//                 {
//                     ++count;
//                     Print(L"%02d %03d %02d %d :",count,i,j,k);
//                     Print(L"\tVendorID-%x DeviceID-%x\n", Pci.Hdr.VendorId, Pci.Hdr.DeviceId);
//                 }
//             }
//     return EFI_SUCCESS;
// }

#define PCI_CONFIGURATION_ADDRESS_PORT  0xCF8
#define PCI_CONFIGURATION_DATA_PORT     0xCFC

#define PCI_ECAM_ADDRESS(Bus, Device, Function, Offset) \
  (((Bus & 0xff) << 20) | ((Device & 0x1f) << 15) | ((Function & 0x07) << 12) | (Offset & 0xfff))

UINT32 SetIOAddress (UINT8 Bus, UINT8 Device, UINT8 Function, UINT16 Offset) 
{
    return (0x80000000 | (((Bus) & 0xff) << 16) | (((Device) & 0x1f) << 11) | (((Function) & 0x07) << 8) | Offset);
}
// 一次读32位
UINT32 GetRegistValue32(UINT8 Bus, UINT8 Device, UINT8 Function, UINT16 Offset)
{
    IoWrite32 (PCI_CONFIGURATION_ADDRESS_PORT, SetIOAddress (Bus, Device, Function, Offset));
    return IoRead32 (PCI_CONFIGURATION_DATA_PORT);    
}
// 一次读8位
UINT8 GetRegistValue8(UINT8 Bus, UINT8 Device, UINT8 Function, UINT16 Offset)
{
    UINT32 Data = GetRegistValue32(Bus, Device, Function, (Offset & 0xFC));
    // printf("\n---%x\n",Data);
    // Data = (Data >> ((Offset - (Offset & 0xFC)) * 8)) & 0x000000FF;
    Data = (Data >> ((Offset & 0x03) * 8)) & 0x000000FF;

    return (UINT8)Data;    
}
VOID DeviceType(UINT8 Bus, UINT8 Device, UINT8 Function)
{
    UINT8 Offset = 0x08; 
    UINT32 Data  = GetRegistValue32(Bus, Device, Function, Offset);
    // Data = (Data & 0xFFFFFF00) >> 24;
    // printf("Class Code: %08x\n", Data);
    switch ((Data & 0xFFFFFF00) >> 24)
    {
        case 0x01:   
            printf("Mass storage controller ");  
            // if (((Data & 0x00FFFF00) >> 16) == 0x01)
            //     printf("- IDE controller\n");
            // else
            //     printf("\n");
            (((Data & 0x00FFFF00) >> 16) == 0x01) ? printf("- IDE controller\n") : printf("\n");    
            break;
        case 0x02:   
            printf("Network controller ");  
            (((Data & 0x00FFFF00) >> 8) == 0x0000) ? printf("- Ethernet controller\n") : printf("\n");
            break;
        case 0x03:   
            printf("Display controller ");  
            (((Data & 0x00FFFF00) >> 16) == 0x00) ? printf("- VGA/8514 controller\n") : printf("\n");
            break;
        case 0x04:   
            printf("Multimedia device ");  
            (((Data & 0x00FFFF00) >> 16) == 0x01) ? printf("- Audio device\n") : printf("\n");
            break;
        case 0x05:    printf("Memory controller\n");    break;
        case 0x06:   
            printf("Bridge device ");  
            switch ((Data & 0x00FFFF00) >> 16)
            {
                case 0x00:    printf("- Host/PCI bridge\n");      break;
                case 0x01:    printf("- PCI/ISA bridge\n");       break;
                case 0x80:    printf("- Other bridge device\n");  break;
                default:      printf("\n");                       break;
            }
            break;
        case 0x07:    printf("Simple communication controllers\n");    break;
        case 0x08:
            printf("Base system peripherals ");
            (((Data & 0x00FFFF00) >> 8) == 0x8000) ? printf("- Other system peripheral\n") : printf("\n");
            break;
        case 0x09:    printf("Input devices\n");     break;
        case 0x0a:    printf("Docking stations\n");  break;
        case 0x0b:    printf("Processors\n");        break;
        case 0x0c:   
            printf("Serial bus controllers ");  
            switch ((Data & 0x00FFFF00) >> 8)
            {
                case 0x0310:    printf("- USB\n");   break;
                case 0x0320:    printf("- USB2\n");  break;  
                default:        printf("\n");        break;
            }
            break;
        case 0x0d:    printf("Wireless controller\n");                                 break;
        case 0x0e:    printf("Intelligent I/O controllers\n");                         break;
        case 0x0f:    printf("Satellite communication controllers\n");                 break;
        case 0x10:    printf("Encryption/Decryption controllers\n");                   break;
        case 0x11:    printf("Data acquisition and signal processing controllers\n");  break;
        default:      printf("Unrecognized device\n");                                 break;
    }
}

VOID ASPMSupport(UINT8 Bus, UINT8 Device, UINT8 Function)
{
    UINT8 offset = 0x34;
    UINT8 LinkAddress = GetRegistValue8(Bus, Device, Function, offset);
    UINT8 LinkNext;
    // printf("LinkAddress = %x\n",LinkAddress);
    UINT8 CapID;

    for (;;)
    {
        CapID = GetRegistValue8(Bus, Device, Function, LinkAddress);
        
        if (CapID == 0x10)
        {
            UINT32 LinkCap = GetRegistValue32(Bus, Device, Function, LinkAddress + 0x0C);
            if (LinkCap != 0)
            {
                printf("\tLinkCap = %08X ",LinkCap);
                UINT8 Sup = ((LinkCap >> 10) & 0x00000003);
                switch (Sup)
                {
                    case 0: printf("\tNo ASPM Support\n");       break;
                    case 1: printf("\tL0s Supported\n");         break;
                    case 2: printf("\tL1 Supported\n");          break;
                    case 3: printf("\tL0s and L1 Supported\n");  break;
                    default:printf("\tERROR!\n");                break;
                }
                break;
            }
            else
            {
                printf("\tNo ASPM Support\n");
                break;
            }
        }
        else
        {
            // printf("LinkAddress = %x\t",LinkAddress);
            LinkNext = GetRegistValue8(Bus, Device, Function, LinkAddress+1);
            LinkAddress = LinkNext;
            // printf("LinkAddressNext = %x\n",LinkAddress);
            if (LinkAddress == 0)
            {
                printf("\tNo ASPM Support\n");
                break;
            }
        }
    }   
}

int main(IN int Argc, IN char *Argv[])
{
    // printf("Hello,this is Entry of main!()\n");

    // LocatePCI_Bridge();
    // ListPCI_Devices_Bridge();

    UINT32 Bus, Device, Function, Offset, Data;

    switch (Argc)
    {
        case 1:
            Print (L"Bus:  Device:  Function: \n");
            Print (L"----  -------  --------- \n");
            for (Bus = 0; Bus < 256; Bus++)
            {
                for (Device = 0; Device < 32; Device++)
                {                    
                    for (Function = 0; Function < 8; Function++)
                    {
                        // Data = GetRegistValue32((UINT8)Bus, (UINT8)Device, (UINT8)Function, 0x00);
                        // // 判断设备是否存在
                        // if (Data == 0xFFFFFFFF)
                        //     continue; 
                        if (GetRegistValue32((UINT8)Bus, (UINT8)Device, (UINT8)Function, 0x00) == 0xFFFFFFFF)
                            continue;                             
                        
                        printf("  %02x\t%02X\t%02x\t==>  ", Bus, Device, Function);
                        DeviceType((UINT8)Bus, (UINT8)Device, (UINT8)Function);

                        Data = GetRegistValue32((UINT8)Bus, (UINT8)Device, (UINT8)Function, 0x00);
                        printf("\tVendor ID: %X\tDevice ID: %04X ", (Data & 0x0000FFFF), ((Data & 0xFFFF0000) >> 16));

                        Data = GetRegistValue32((UINT8)Bus, (UINT8)Device, (UINT8)Function, 0x08);
                        printf(" Prog Interface %X\n", ((Data & 0x0000FF00) >> 8));
                        ASPMSupport((UINT8)Bus, (UINT8)Device, (UINT8)Function);
                    } // End Function
                    // if (Device == 31)    break;
                } // End Device
                // if (Bus == 255)    break;
            } // End Bus
            break;

        case 4:
            // char 转 十六进制 int
            sscanf(Argv[1], "%x", &Bus);
            sscanf(Argv[2], "%x", &Device);
            sscanf(Argv[3], "%x", &Function);
            if ((Bus <= 0xff) && (Device <= 0x1f) && (Function <= 7))
            {   
                Print (L"Bus: %X Device: %X Function: %X \n", Bus, Device, Function);
                Print (L"Data:\n");
                for (Offset = 0x00; Offset < 0x3F; Offset += 0x04)
                {
                    Data = GetRegistValue32((UINT8)Bus, (UINT8)Device, (UINT8)Function, (UINT16)Offset);
                    printf("    0x%02X: %08X \n", Offset, Data);
                }
                // for (Offset = 0x00; Offset < 0x3F; Offset += 0x04)
                // {
                //     Data = MmioRead32(0x80000000 + PCI_ECAM_ADDRESS((UINT8)Bus, (UINT8)Device, (UINT8)Function, (UINT16)Offset));
                //     printf("    0x%02X: %08X \n", Offset, Data);
                // }

            }
            else
                printf("Address ERROR!!!\n");
            break;

        case 5:
            // char 转 int
            sscanf(Argv[1], "%x", &Bus);
            sscanf(Argv[2], "%x", &Device);
            sscanf(Argv[3], "%x", &Function);
            sscanf(Argv[4], "%x", &Offset);
            if ((Bus <= 0xff) && (Device <= 0x1f) && (Function <= 7) && (Offset <= 0xfff))
            {   
                Print (L"Bus: %X Device: %X Function: %X Offset: 0x%X\n", Bus, Device, Function, Offset);
                Print (L"Data(8-bit): ");
                UINT32 Data8 = GetRegistValue8((UINT8)Bus, (UINT8)Device, (UINT8)Function, (UINT16)Offset);
                // UINT32 Data8 = MmioRead8(0x80000000 + PCI_ECAM_ADDRESS((UINT8)Bus, (UINT8)Device, (UINT8)Function, (UINT16)Offset));
                printf("0x%02X\n",Data8);
            }
            break;
    default:
        printf("Parameter error!!!\n");
        break;
    }

    return 0;
}

