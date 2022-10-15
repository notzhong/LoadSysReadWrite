#include "sysctrl.h"
#include "PrintLog.h"
#include <QString>

SysCtrl::SysCtrl():m_pDriverName(nullptr)
{
    InitHandle();
}

SysCtrl::~SysCtrl()
{
    delete[]m_pDriverName;
}

//初始化句柄
void SysCtrl::InitHandle()
{
    m_hSCManager=m_hService=nullptr;
    m_hDervice =nullptr;
}

// 打开服务控制管理器数据库
int SysCtrl::QopenSCManager()
{
    // 打开服务控制管理器数据库
    m_hSCManager = OpenSCManagerA(nullptr,nullptr,SC_MANAGER_ALL_ACCESS);
    if(!m_hSCManager)
    {
        LOGEORRO(QString().asprintf("%s 错误码：%d","打开服务控制管理器数据库失败",static_cast<int>(GetLastError())).toStdString().c_str());
        CloseServiceHandle(m_hSCManager);
        return -1;
    }
    return 0;
}

//打开指定驱动服务
int SysCtrl::QopenService()
{
    m_hService = OpenServiceW(m_hSCManager,m_pDriverName,SERVICE_ALL_ACCESS);
    if(!m_hService)
    {
        LOGEORRO(QString().asprintf("%s 错误码：%d","打开服务失败",static_cast<int>(GetLastError())).toStdString().c_str());
        CloseServiceHandle(m_hService);
        CloseServiceHandle(m_hSCManager);
        return -1;
    }
    return 0;
}

//加载驱动
int SysCtrl::LoadSys(const wchar_t*pDriverName, const wchar_t *pDriverPath)
{
    if(QopenSCManager())
        return -1;

    m_pDriverName = new wchar_t[wcslen(pDriverName)+1]{0};
   //设置服务名称
    wmemcpy(m_pDriverName,pDriverName,wcslen(pDriverName));

    if(!wcslen(pDriverName)||!wcslen(pDriverPath))
    {
        LOGEORRO(QString().asprintf("%s 错误码：%d","名称 or 路径不能为空",static_cast<int>(GetLastError())).toStdString().c_str());
        CloseServiceHandle(m_hSCManager);
        return -1;
    }

    //创建服务
    m_hService =CreateServiceW(m_hSCManager, //服务控件管理器数据库的句柄
                               m_pDriverName,//要安装的服务的名称
                               m_pDriverName,//用户界面程序用来显示服务的名称
                               SERVICE_ALL_ACCESS,//对服务的访问权限：所有权限
                               SERVICE_KERNEL_DRIVER,//服务类型：驱动服务
                               SERVICE_DEMAND_START,//服务启动选项：进程调用start service时启动
                               SERVICE_ERROR_IGNORE,//如果无法启动：忽略错误继续运行
                               pDriverPath,//驱动文件的绝对路径
                               nullptr,//服务所属的负载订购组：服务不属于某个组
                               nullptr,//接收订购组唯一标记值：不接受
                               nullptr,//服务加载顺序数组：服务没有依赖项
                               nullptr,//运行服务的账户名：使用local system账户
                               nullptr//local system账户密码
                               );

    if(!m_hService)
    {
        LOGEORRO(QString().asprintf("%s 错误码：%d","创建服务失败",static_cast<int>(GetLastError())).toStdString().c_str());
        CloseServiceHandle(m_hService);
        CloseServiceHandle(m_hSCManager);
        return -1;
    }
    CloseServiceHandle(m_hService);
    CloseServiceHandle(m_hSCManager);
    InitHandle();
    return 0;
}

//启动驱动
int SysCtrl::StartSys()
{
    if(QopenSCManager())
        return -1;
    if(QopenService())
        return -1;
    if(!StartServiceA(m_hService,0,nullptr))
    {
        CloseServiceHandle(m_hService);
        CloseServiceHandle(m_hSCManager);
        LOGEORRO(QString().asprintf("%s 错误码：%d","启动服务失败",static_cast<int>(GetLastError())).toStdString().c_str());
        return -1;
    }
    CloseServiceHandle(m_hService);
    CloseServiceHandle(m_hSCManager);
    InitHandle();
    return 0;
}

//停止驱动
int SysCtrl::StopSys()
{
    if(QopenSCManager())
        return -1;
    if(QopenService())
        return -1;

    //驱动服务信息状态结构体
    SERVICE_STATUS status;
    //获取驱动服务状态
    if(!QueryServiceStatus(m_hService,&status))
    {
        LOGEORRO(QString().asprintf("%s 错误码：%d","获取驱动服务状态失败",static_cast<int>(GetLastError())).toStdString().c_str());
        CloseServiceHandle(m_hService);
        CloseServiceHandle(m_hSCManager);
        return -1;
    }

    //判断驱动服务是否在运行：如果在运行关闭驱动服务
    if(status.dwCurrentState!=SERVICE_STOPPED&&status.dwCurrentState!=SERVICE_STOP_PENDING)
    {
        if(!ControlService(m_hService,SERVICE_CONTROL_STOP,&status))
        {
            LOGEORRO(QString().asprintf("%s 错误码：%d","停止驱动服务状态失败",static_cast<int>(GetLastError())).toStdString().c_str());
            CloseServiceHandle(m_hService);
            CloseServiceHandle(m_hSCManager);
            return -1;
        }
    } else {
        LOGEORRO(QString().asprintf("%s 错误码：%d","驱动服务状态已停止 or 正在停止",static_cast<int>(GetLastError())).toStdString().c_str());
        CloseServiceHandle(m_hService);
        CloseServiceHandle(m_hSCManager);
        return -1;
    }
    int nTime = 0;
    while (status.dwCurrentState!=SERVICE_STOPPED) {
       nTime ++;
       QueryServiceStatus(m_hService,&status);
       Sleep(1000);
       if(nTime == 3)
       {
           LOGEORRO(QString().asprintf("%s 错误码：%d","驱动服务停止超时",static_cast<int>(GetLastError())).toStdString().c_str());
           CloseServiceHandle(m_hService);
           CloseServiceHandle(m_hSCManager);
           return -1;
       }
    }

    CloseServiceHandle(m_hService);
    CloseServiceHandle(m_hSCManager);
    InitHandle();
    return 0;
}

//卸载散出驱动服务
int SysCtrl::RemoveDriver()
{
    if(QopenSCManager())
        return -1;
    if(QopenService())
        return -1;

    if(!DeleteService(m_hService))
    {
        LOGEORRO(QString().asprintf("%s 错误码：%d","驱动服务删除失败",static_cast<int>(GetLastError())).toStdString().c_str());
        CloseServiceHandle(m_hService);
        CloseServiceHandle(m_hSCManager);
        return -1;
    }

    CloseServiceHandle(m_hService);
    CloseServiceHandle(m_hSCManager);
    InitHandle();
    return 0;
}

//创建用于使用的驱动文件设备句柄
int SysCtrl::ReadWriteBegin()
{
    m_hDervice = nullptr;
    m_hDervice = CreateFileW(L"\\\\.\\RW64",
                            GENERIC_READ | GENERIC_WRITE,
                            FILE_SHARE_READ|FILE_SHARE_WRITE,
                            0,
                            OPEN_EXISTING,
                            0,
                            0);
    if(!m_hDervice||reinterpret_cast<PLONG64>(-1)==m_hDervice)
    {
        LOGEORRO(QString().asprintf("%s 错误码：%d","创建设备文件失败",static_cast<int>(GetLastError())).toStdString().c_str());
        return -1;
    }
    return 0;
}

//删除驱动设备句柄
int SysCtrl::ReadWriteEnd()
{
    CloseHandle(m_hDervice);
    return 0;
}

//进行读内存操作
int SysCtrl::ReadVirtualMemory(ULONG64 PID, void *SourceAddr, void *TargetAddr, ULONG64 Size)
{
    LOGINFO(QString().asprintf("SourceAddr %p, TargetAddr %p, *TargetAddr %d, Size %llu", SourceAddr,TargetAddr, *((PINT)TargetAddr), Size).toStdString().c_str());
    PS RW;
    memset(&RW,0,sizeof(RW));
    RW.nPID = PID;
    RW.pSorceAddr = reinterpret_cast<ULONG64>(SourceAddr);
    RW.pTargetAddr = reinterpret_cast<ULONG64>(TargetAddr);
    RW.nSize = Size;

    DeviceIoControl(m_hDervice,ID_Read,&RW,sizeof(RW),0,0,0,0);
    return 0;
}

//进行写内存操作
int SysCtrl::WriteVirtualMemory(ULONG64 PID, void *SourceAddr, void *TargetAddr, ULONG64 Size)
{
    LOGINFO(QString().asprintf("SourceAddr %p, TargetAddr %p, *TargetAddr %d, Size %llu", SourceAddr,TargetAddr, *((PINT)TargetAddr), Size).toStdString().c_str());
    PS RW;
    memset(&RW,0,sizeof(RW));
    RW.nPID = PID;
    RW.pSorceAddr = reinterpret_cast<ULONG64>(SourceAddr);
    RW.pTargetAddr = reinterpret_cast<ULONG64>(TargetAddr);
    RW.nSize = Size;

    DeviceIoControl(m_hDervice,ID_Write,&RW,sizeof(RW),0,0,0,0);
    return 0;
}

//对指定进程实施进程高权限保护
int SysCtrl::ENDISProcessProtect(ULONG64 PID)
{
    PS RW;
    static bool ENDIS = 1;
    memset(&RW,0,sizeof(RW));
    RW.nPID = PID;
    if(ENDIS)
    {
        RW.nSize = ENDIS;
        ENDIS = 0;
    } else {
        RW.nSize = ENDIS;
        ENDIS = 1;
    }
    DeviceIoControl(m_hDervice,ID_Protect,&RW,sizeof(RW),0,0,0,0);
    return 0;
}


//断链进程
int SysCtrl::ProcessLink(ULONG64 PID)
{
    PS RW;
    static bool bLink = 1;
    memset(&RW,0,sizeof(RW));
    RW.nPID = PID;
    if(bLink)
    {
        RW.nSize = bLink;
        bLink = 0;
    } else {
        RW.nSize = bLink;
        bLink = 1;
    }
    DeviceIoControl(m_hDervice,ID_ProcessLink,&RW,sizeof(RW),0,0,0,0);
    return 0;
}
