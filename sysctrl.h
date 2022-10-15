#ifndef SYSCTRL_H
#define SYSCTRL_H

#include "_DriverStrust.h"

class SysCtrl
{
private:
    SC_HANDLE m_hSCManager;
    SC_HANDLE m_hService;
    HANDLE m_hDervice;
    wchar_t*m_pDriverName;
    int QopenSCManager();
    int QopenService();

public:
    SysCtrl();
    ~SysCtrl();
    //初始话句柄
    void InitHandle();
    //加载驱动
    int LoadSys(const wchar_t* pDriverName = nullptr,const wchar_t*pDriverPath = nullptr);
    //启动驱动
    int StartSys();
    //停止驱动
    int StopSys();
    //卸载驱动
    int RemoveDriver();
    //创建驱动设备会话句柄
    int ReadWriteBegin();
    //关闭驱动设备会话句柄
    int ReadWriteEnd();
    //读内存
    int ReadVirtualMemory(ULONG64 PID,void*SourceAddr,void*TargetAddr,ULONG64 Size);
    //写内存
    int WriteVirtualMemory(ULONG64 PID,void*SourceAddr,void*TargetAddr,ULONG64 Size);
    //开启进程保护
    int ENDISProcessProtect(ULONG64 PID);
    //启动进程断链
    int ProcessLink(ULONG64);

};

#endif // SYSCTRL_H
