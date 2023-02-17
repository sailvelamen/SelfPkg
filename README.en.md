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


#### Gitee Feature

1.  You can use Readme\_XXX.md to support different languages, such as Readme\_en.md, Readme\_zh.md
2.  Gitee blog [blog.gitee.com](https://blog.gitee.com)
3.  Explore open source project [https://gitee.com/explore](https://gitee.com/explore)
4.  The most valuable open source project [GVP](https://gitee.com/gvp)
5.  The manual of Gitee [https://gitee.com/help](https://gitee.com/help)
6.  The most popular members  [https://gitee.com/gitee-stars/](https://gitee.com/gitee-stars/)
