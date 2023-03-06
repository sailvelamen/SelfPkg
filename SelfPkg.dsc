[Defines]
  PLATFORM_NAME                  = SelfPkg
  PLATFORM_GUID                  = 0eb07ab9-53d4-45c0-90b3-ddbd17efc885
  PLATFORM_VERSION               = 0.1
  DSC_SPECIFICATION              = 0x00010006
  OUTPUT_DIRECTORY               = Build/SelfPkg
  SUPPORTED_ARCHITECTURES        = IA32|X64|EBC|ARM|AARCH64|RISCV64|LOONGARCH64
  BUILD_TARGETS                  = DEBUG|RELEASE|NOOPT
  SKUID_IDENTIFIER               = DEFAULT

[PcdsFeatureFlag.common]
  gEfiSelfPkgTokenSpaceGuid.PcdHelloWorldPrintEnable|FALSE

[PcdsFixedAtBuild.common]
  gEfiSelfPkgTokenSpaceGuid.PcdHelloWorldPrintString|L"UEFI Hello World! Intex = %d\n"
  gEfiMdePkgTokenSpaceGuid.PcdDebugPropertyMask|0xFF

[PcdsFixedAtBuild]
  gEfiSelfPkgTokenSpaceGuid.PcdHelloWorldPrintLoops|5

[PcdsPatchableInModule]
  gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x8000004F
  # gEfiMdePkgTokenSpaceGuid.PcdDebugPrintErrorLevel|0x807B55FF
  # DEBUG_INIT      0x00000001  // Initialization
  # DEBUG_WARN      0x00000002  // Warnings
  # DEBUG_LOAD      0x00000004  // Load events
  # DEBUG_FS        0x00000008  // EFI File system
  # DEBUG_POOL      0x00000010  // Alloc & Free (pool)
  # DEBUG_PAGE      0x00000020  // Alloc & Free (page)
  # DEBUG_INFO      0x00000040  // Informational debug messages
  # DEBUG_DISPATCH  0x00000080  // PEI/DXE/SMM Dispatchers
  # DEBUG_VARIABLE  0x00000100  // Variable
  # DEBUG_BM        0x00000400  // Boot Manager
  # DEBUG_BLKIO     0x00001000  // BlkIo Driver
  # DEBUG_NET       0x00004000  // SNP Driver
  # DEBUG_UNDI      0x00010000  // UNDI Driver
  # DEBUG_LOADFILE  0x00020000  // LoadFile
  # DEBUG_EVENT     0x00080000  // Event messages
  # DEBUG_GCD       0x00100000  // Global Coherency Database changes
  # DEBUG_CACHE     0x00200000  // Memory range cachability changes
  # DEBUG_VERBOSE   0x00400000  // Detailed debug messages that may significantly impact boot performance
  # DEBUG_ERROR     0x80000000  // Error

!include MdePkg/MdeLibs.dsc.inc

[LibraryClasses.common]
#
# Entry Point Libraries
#
  UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
  ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf
  UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
#
# Base Libraries
#
  UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
  BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
  PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
  UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
  UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
  PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
  RegisterFilterLib|MdePkg/Library/RegisterFilterLibNull/RegisterFilterLibNull.inf
  DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLibOptionalDevicePathProtocol.inf  #
  BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf  #
  MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf  #
  HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
  UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf

!if $(TARGET) == RELEASE  #
  DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
!else
  DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
  # DebugLib|MdePkg/Library/UefiDebugLibConOut/UefiDebugLibConOut.inf  #
!endif
  DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
  SerialPortLib|PcAtChipsetPkg/Library/SerialIoLib/SerialIoLib.inf  #
  IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf  #
  
# Self Library
  MyLibraryLib|SelfPkg/Library/MyLibrary/MyLibrary.inf
  MyCMOSLib|SelfPkg/Library/MyCMOSLib/MyCMOSLib.inf
  MyKeyboardLib|SelfPkg/Library/MyKeyboardLib/MyKeyboard.inf

[LibraryClasses.common.PEIM]
  PeimEntryPoint|MdePkg/Library/PeimEntryPoint/PeimEntryPoint.inf
  PeiServicesLib|MdePkg/Library/PeiServicesLib/PeiServicesLib.inf
  PeiServicesTablePointerLib|MdePkg/Library/PeiServicesTablePointerLibIdt/PeiServicesTablePointerLibIdt.inf
  MemoryAllocationLib|MdePkg/Library/PeiMemoryAllocationLib/PeiMemoryAllocationLib.inf
  HobLib|MdePkg/Library/PeiHobLib/PeiHobLib.inf

[LibraryClasses.common.DXE_SMM_DRIVER]
  SmmServicesTableLib|MdePkg/Library/SmmServicesTableLib/SmmServicesTableLib.inf

[LibraryClasses.common.UEFI_DRIVER, LibraryClasses.common.UEFI_APPLICATION]
  PcdLib|MdePkg/Library/DxePcdLib/DxePcdLib.inf

[Components]
#### Uefi Applications.
  SelfPkg/Applications/Uefi_Main/Uefi_Main.inf
  SelfPkg/Applications/ShellApp_Main/ShellApp_Main.inf
  SelfPkg/Applications/Stdlib_Main/Stdlib_Main.inf
  SelfPkg/Applications/CMOS_Test/CMOS_Test.inf
  SelfPkg/Applications/GetAll_Variables/GetAll_Variables.inf
  SelfPkg/Applications/ListPCIDevices/ListPCI_Dev.inf
  SelfPkg/Applications/MySMBios/MySmBios.inf
  SelfPkg/Applications/TestDemo/TestDemo.inf
  SelfPkg/Applications/PcdTest/PcdTest.inf
#### Uefi Drivers.
  SelfPkg/Drivers/UefiMain/UefiMain.inf
  SelfPkg/Drivers/CmosTest/CmosTest.inf
  SelfPkg/Drivers/MyEvent/MyEvent.inf
  SelfPkg/Drivers/MyProtocol/MyProtocol.inf
  SelfPkg/Universal/DriverSampleDxe/DriverSampleDxe.inf
  SelfPkg/Universal/SwSmiDemo/SwSmiDemo.inf
  SelfPkg/Universal/PpiHobDemo/PpiHobDemo.inf
#### Application Library.
  SelfPkg/Applications/MyLibApp/MyLibApp.inf

##############################################################################
#
#  Include Boilerplate text required for building with the Standard Libraries.
#
##############################################################################
!include StdLib/StdLib.inc