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


#### 特技

1.  使用 Readme\_XXX.md 来支持不同的语言，例如 Readme\_en.md, Readme\_zh.md
2.  Gitee 官方博客 [blog.gitee.com](https://blog.gitee.com)
3.  你可以 [https://gitee.com/explore](https://gitee.com/explore) 这个地址来了解 Gitee 上的优秀开源项目
4.  [GVP](https://gitee.com/gvp) 全称是 Gitee 最有价值开源项目，是综合评定出的优秀开源项目
5.  Gitee 官方提供的使用手册 [https://gitee.com/help](https://gitee.com/help)
6.  Gitee 封面人物是一档用来展示 Gitee 会员风采的栏目 [https://gitee.com/gitee-stars/](https://gitee.com/gitee-stars/)
