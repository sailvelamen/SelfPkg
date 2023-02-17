# SelfPkg

#### Description
EDK II Self Package

#### Software Architecture
Self EDK II Project

SelfPkg\Applications # All UEFI_APPLICATION Projects

SelfPkg\Drivers # All UEFI_DRIVER Projects

SelfPkg\Universal # These projects need to be compiled in the platform's DSC file

SelfPkg\Library # Self Libraries

SelfPkg\Include # Include Guid and customized Libraries

#### Installation

1.  Install ASL NASM
2.  Download EDK II source code
3.  Copy to Workspace

#### Instructions

1.  build -a X64 -p SelfPkg\SelfPkg.dsc  # Direct compilation
2.  build -a X64 -p SelfPkg\SelfPkg.dsc -m SelfPkg\Applications\ShellApp_Main\ShellApp_Main.inf  # Compile concrete module