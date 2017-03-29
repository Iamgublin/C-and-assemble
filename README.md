C-and-assemble
=============
主要内容
---------------
1.[NDIS 6.30](https://github.com/Iamgublin/C-and-assemble/tree/master/NDIS%206.30)：一个基于NDIS 6.30的数据包过滤函数库,支持数据包过滤分析以及自定义数据包API（尚未文档化），包括基于NDIS 6.30核心代码(MY NDIS 6.0)，数据分析(RawPacketAnalysis)，以及驱动控制的API（NdisCoreApi）。`NDIS 6.30的驱动只能够在win8和以上的操作系统运行！`</br>


2.[VirtualDisk](https://github.com/Iamgublin/C-and-assemble/tree/master/VirtualDisk):基于Windows-Driver-Sample的VirtualDisk改写（源代码为WDF框架的驱动，改写为WDM框架的），文件系统为FAT12，FAT16。基于内存虚拟化的磁盘驱动程序。

3[Get LoadLibraryA shellcode](https://github.com/Iamgublin/C-and-assemble/tree/master/Get%20LoadLibraryA%20shellcode(x64)):分32位和64位两个版本，主要的区别就是32位是从fs寄存器里面取PBE,而64位是从gs寄存器中取。通过lrd字段寻找kernel32地址，内存定位装载PE地址，通过输出表找到Loadlibrary函数并调用。
