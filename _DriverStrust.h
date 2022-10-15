#ifndef _DRIVERSTRUST_H
#define _DRIVERSTRUST_H
#include <windows.h>
#include <winioctl.h>

//读内存控制宏
#define ID_Read     CTL_CODE(FILE_DEVICE_UNKNOWN,0x6001,METHOD_OUT_DIRECT,FILE_ANY_ACCESS)

//写内存控制宏
#define ID_Write     CTL_CODE(FILE_DEVICE_UNKNOWN,0x6002,METHOD_OUT_DIRECT,FILE_ANY_ACCESS)

//进程保护控制宏
#define ID_Protect     CTL_CODE(FILE_DEVICE_UNKNOWN,0x6003,METHOD_OUT_DIRECT,FILE_ANY_ACCESS)

//进程断链控制宏
#define ID_ProcessLink     CTL_CODE(FILE_DEVICE_UNKNOWN,0x6004,METHOD_OUT_DIRECT,FILE_ANY_ACCESS)

//传输进程相关信息pid、memory、memory size
typedef struct ProcessStru
{
    ULONG64 nPID;           //进程pid
    ULONG64 pSorceAddr;     //被修改的内存地址
    ULONG64 pTargetAddr;    //需修改的内存地址
    ULONG64 nSize;          //字节大小
}PS,*pPS;



#endif // _DRIVERSTRUST_H
