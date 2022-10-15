#ifndef WIDGET_H
#define WIDGET_H
#include <QDebug>
#include <QWidget>
#include <QFileDialog>
#include <windows.h>
#include <tlhelp32.h>
#include <QComboBox>
#include <QMessageBox>
#include <QGroupBox>
#include <QRadioButton>

#include "sysctrl.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    //获取进程pid
    ULONG64 GetProcessId(const QString&ProcessName);
    //获取进程信息
    void GetProcessInfo();
    //基址+偏移
    ULONG64 pTargetAddressAddNum();
    //读内存操作
    void ReadMemoryChar(PVOID64 SourceAddress,PVOID64 pGetAddress);
    void ReadMemory2Char(PVOID64 SourceAddress,PVOID64 pGetAddress);
    void ReadMemory4Char(PVOID64 SourceAddress,PVOID64 pGetAddress);
    void ReadMemoryFloatChar(PVOID64 SourceAddress,PVOID64 pGetAddress);
    void ReadMemoryDoubleChar(PVOID64 SourceAddress,PVOID64 pGetAddress);
    void ReadMemory10StringChar(PVOID64 SourceAddress,PVOID64 pGetAddress);
    //获取写内存类型
    int GetRadioNum();

    //查找int类型数据
    void FindInt();

private slots:
    //选择驱动
    void on_SysButton_clicked();
    //注册驱动
    void on_RegisterSys_clicked();
    //启动驱动
    void on_StartSys_clicked();
    //停止驱动
    void on_StopSys_clicked();
    //卸载驱动
    void on_RemoteSys_clicked();
    //读内存操作
    void on_BeginOffset_clicked();
    //刷新系统进程快照
    void on_RefreshProcess_clicked();

    //根据选择进程获取pid
    void on_ProcessComboBox_currentTextChanged(const QString &arg1);
    //写内存操作
    void on_Write_Button_clicked();
    //开启进程保护
    void on_EnableProcessProtect_clicked();
    //开启进程断链
    void on_ProcessLinkUp_clicked();
    //查找数据
    void on_Find_Button_clicked();

private:
    //存储进程PID、Name
    std::map<ULONG64,QString> m_ProcessNames;
    //进程PID
    unsigned long long m_pid;
    Ui::Widget *ui;
    //驱动对象
    SysCtrl m_RWProtcet;
};
#endif // WIDGET_H
