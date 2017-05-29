C-and-assemble
=============
主要内容
---------------
1. [VirtualDisk](https://github.com/Iamgublin/C-and-assemble/tree/master/VirtualDisk):基于Windows-Driver-Sample的VirtualDisk改写（源代码为WDF框架的驱动，改写为WDM框架的），文件系统为FAT12，FAT16。基于内存虚拟化的磁盘驱动程序。
![效果图](https://github.com/Iamgublin/C-and-assemble/blob/master/VirtualDisk/效果图.png)


2. [Get LoadLibraryA shellcode](https://github.com/Iamgublin/C-and-assemble/tree/master/Get%20LoadLibraryA%20shellcode(x64)):分32位和64位两个版本，主要的区别就是32位是从fs寄存器里面取PBE,而64位是从gs寄存器中取。通过PEB的ldr字段寻找kernel32地址，内存定位装载PE地址，通过输出表找到Loadlibrary函数并调用。
