# SelfPkg

#### 介绍
EDK II Self Package

#### 软件架构
自定义 EDK II 项目

SelfPkg\Applications # 都是 UEFI_APPLICATION 项目 

SelfPkg\Drivers      # 都是 UEFI_DRIVER 项目

SelfPkg\Universal    # 这些项目需要放入平台的DSC文件中进行编译

SelfPkg\Library      # 自定义库

SelfPkg\Include      # 包含 Guid 以及配合自定义的库


#### 安装教程

1.  安装 ASL NASM
2.  下载 EDK II 代码
3.  Copy 到 工作目录下

#### 使用说明

1.  build -a X64 -p SelfPkg\SelfPkg.dsc # 直接编译
2.  build -a X64 -p SelfPkg\SelfPkg.dsc -m SelfPkg\Applications\ShellApp_Main\ShellApp_Main.inf # 编译具体 Moudle