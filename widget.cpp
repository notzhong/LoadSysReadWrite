#include "widget.h"
#include "ui_widget.h"
#include "PrintLog.h"


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    m_pid = 0;
    ui->setupUi(this);
    resize(1050,788);
    setMaximumSize(1050,788);
    setWindowTitle(QString("峰造内存读写遍历工具"));
    //setWindowIcon()

    //获取系统进程
    GetProcessInfo();
}

Widget::~Widget()
{
    delete ui;
}

//根据进程名称获取pid
ULONG64 Widget::GetProcessId(const QString&ProcessName)
{
    for (auto it = m_ProcessNames.begin();it != m_ProcessNames.end();it++) {
        if(it->second == ProcessName)
            return it->first;
    }
    return 0;
}

void Widget::GetProcessInfo()
{
    //此结构描述了列表中的条目，该条目枚举了拍摄快照时驻留在系统地址空间中的进程。
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    //创建系统进程快照
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

    //获取第一个进程信息
    Process32First(hProcessSnap,&pe32);

    //遍历进程名
    do
    {
        //创建用于接收系统进程的对象
        std::wstringstream sProcessNames;

        //从进程快照中将进程名称传入接收的对象中
        sProcessNames << pe32.szExeFile;

        //创建string对象，接收系统进程名称
        std::wstring Pop_ProcessNames;
        Pop_ProcessNames = sProcessNames.str();

        //将系统进程名称格式化输入QString中
        QString Name = QString::fromStdWString(Pop_ProcessNames);
        LOGINFO(QString(QString("当前获得进程：")+Name).toStdString().c_str());
        //将系统进程PID，名称存放入ProcessName中
        ui->ProcessComboBox->addItem(Name);
        m_ProcessNames[pe32.th32ProcessID] = Name;

    }while(Process32Next(hProcessSnap,&pe32));//获取下一个进程信息

    CloseHandle(hProcessSnap);
}

//计算需进行内存读取的内存地址
ULONG64 Widget::pTargetAddressAddNum()
{
    ULONG64 pTargetAddress = ui->SourceAddr->text().toULongLong(nullptr,16);
    pTargetAddress+=ui->one_offset->text().toULongLong(nullptr,16);
    pTargetAddress+=ui->two_offset->text().toULongLong(nullptr,16);
    pTargetAddress+=ui->three_offset->text().toULongLong(nullptr,16);
    pTargetAddress+=ui->four_offset->text().toULongLong(nullptr,16);
    pTargetAddress+=ui->five_offset->text().toULongLong(nullptr,16);
    pTargetAddress+=ui->six_offset->text().toULongLong(nullptr,16);
    pTargetAddress+=ui->seven_offset->text().toULongLong(nullptr,16);
    pTargetAddress+=ui->eight_offset->text().toULongLong(nullptr,16);

    return pTargetAddress;
}

//读char类型内容的函数
void Widget::ReadMemoryChar(PVOID64 SourceAddress, PVOID64 pGetAddress)
{
    m_RWProtcet.ReadVirtualMemory(m_pid,SourceAddress,pGetAddress,sizeof(char));
}

void Widget::ReadMemory2Char(PVOID64 SourceAddress, PVOID64 pGetAddress)
{
    m_RWProtcet.ReadVirtualMemory(m_pid,SourceAddress,pGetAddress,sizeof(short));
}

void Widget::ReadMemory4Char(PVOID64 SourceAddress, PVOID64 pGetAddress)
{
    m_RWProtcet.ReadVirtualMemory(m_pid,SourceAddress,pGetAddress,sizeof(int));
}


void Widget::ReadMemoryFloatChar(PVOID64 SourceAddress, PVOID64 pGetAddress)
{
    m_RWProtcet.ReadVirtualMemory(m_pid,SourceAddress,pGetAddress,sizeof(float));
}

void Widget::ReadMemoryDoubleChar(PVOID64 SourceAddress, PVOID64 pGetAddress)
{
    m_RWProtcet.ReadVirtualMemory(m_pid,SourceAddress,pGetAddress,sizeof(double));
}

void Widget::ReadMemory10StringChar(PVOID64 SourceAddress, PVOID64 pGetAddress)
{
    m_RWProtcet.ReadVirtualMemory(m_pid,SourceAddress,pGetAddress,10);
}


//返回选中的单选按钮
int Widget::GetRadioNum()
{
    if(ui->char_radio->isChecked())
    {
        return 1;
    } else if(ui->short_radio->isChecked())
    {
        return 2;
    } else if(ui->int_radio->isChecked())
    {
        return 4;
    } else if(ui->float_radio->isChecked())
    {
        return sizeof(float)*2;
    } else if(ui->double_radio->isChecked())
    {
        return sizeof(double)*2;
    } else if(ui->string_radio->isChecked())
    {
        return 10;
    }
    return 0;
}

void Widget::FindInt()
{
    int nBeginRow = 0;
    int number = ui->textEdit->toPlainText().toInt();
    if(!ui->tableWidget->selectedItems().isEmpty())
    {
        int nRow = ui->tableWidget->selectedItems().back()->row()+1;
        //QMessageBox::information(this,tr("信息："),QString().asprintf("%d",nRow));
        nBeginRow = nRow;
    }
    for (int nCountRow = ui->tableWidget->rowCount(); nBeginRow < nCountRow; ++nBeginRow) {
        if(ui->tableWidget->item(nBeginRow,3)->text().toInt()==number)
        {
            ui->tableWidget->setCurrentCell(nBeginRow,QItemSelectionModel::Select);
            break;
        }
    }
}

//选择驱动文件打开
void Widget::on_SysButton_clicked()
{
    //打开文件选择对话框
    QString sPath = QFileDialog::getOpenFileName(nullptr,
                                                 "选择文件",
                                                 nullptr,
                                                 tr("*.sys;;*.*"),
                                                 nullptr,
                                                 QFileDialog::ShowDirsOnly);
    //判断是否选择了文件
    if(sPath.isEmpty())
    {
        LOGINFO("没有选择文件");
        return;
    }
    LOGINFO((QString("选择的路径")+sPath).toStdString().c_str());
    //设置编辑框显示字体的大小，及选择的路径
    ui->LoadPath->setFontPointSize(6);
    ui->LoadPath->setText(sPath);
}

//注册驱动
void Widget::on_RegisterSys_clicked()
{
    LOGINFO("点击注册驱动");
    if(m_RWProtcet.LoadSys(L"RWSys",ui->LoadPath->toPlainText().toStdWString().c_str()))
    {
        LOGEORRO("注册驱动服务失败");
    } else {
        LOGINFO("注册驱动服务成功");
    }
}

//启动驱动
void Widget::on_StartSys_clicked()
{
    LOGINFO("点击启动驱动");
    if(m_RWProtcet.StartSys())
    {
        LOGEORRO("服务启动失败");
    } else {
        LOGINFO("驱动服务启动成功");
    }
}

//停止驱动
void Widget::on_StopSys_clicked()
{
    LOGINFO("点击停止驱动");
    if(m_RWProtcet.StopSys())
    {
        LOGEORRO("服务停止失败");
    } else {
        LOGINFO("驱动服务停止成功");
    }
}

//卸载驱动
void Widget::on_RemoteSys_clicked()
{
    LOGINFO("点击卸载驱动");
    if(m_RWProtcet.RemoveDriver())
    {
        LOGEORRO("服务卸载失败");
    } else {
        LOGINFO("驱动服务卸载成功");
    }
}

//读内存，并将读内存得到的数据打印在Table控件里
void Widget::on_BeginOffset_clicked()
{
    //判断Table是否存在内容，如果有就清空
    if(ui->tableWidget->rowCount())
    {
        int nRow = ui->tableWidget->rowCount();
        while(nRow--)
        {
            ui->tableWidget->removeRow(nRow);
        }
    }

    //选择需要读取内存内容的进程
    //QString ProcessName = ui->ProcessComboBox->currentText();

    //判断得到的进程PID是否为空
    if(!m_pid)
    {
        QMessageBox::warning(this,tr("警告："),tr("未选择进程 or 进程不存在"),QMessageBox::Ok,QMessageBox::Ok);
        return;
    }
    int nRowCount = 0;
    int nCount= 0;

    //得到开始遍历的内存地址
    PVOID64 nSourceAddress = reinterpret_cast<PVOID64>(pTargetAddressAddNum());

    //开始进行内存读
    if(m_RWProtcet.ReadWriteBegin())
    {
        QMessageBox::warning(this,"警告：","驱动服务未加载 or 加载失败");
        return;
    }

    //循环将读到的数据打印在表格里
    while (nRowCount ++ < ui->Offset_Num->text().toInt()) {
        //获取控件存在多少行内容
        nCount = ui->tableWidget->rowCount();
        //递增读取的内存地址
        PVOID64 nAddress=reinterpret_cast<PVOID64>(reinterpret_cast<ULONG64>(nSourceAddress)+nCount);
        //在第nCount行插入新行
        ui->tableWidget->insertRow(nCount);
        //设置nCount行的第一个数据的内容
        ui->tableWidget->setItem(nCount,0,new QTableWidgetItem(QString().asprintf("%p",nAddress)));
        char p = 0;
        //读取一个字节的内容
        ReadMemoryChar(nAddress,&p);
        ui->tableWidget->setItem(nCount,1,new QTableWidgetItem(QString().asprintf("%c",p)));

        short pTwo = 0;
        ReadMemory2Char(nAddress,&pTwo);
        ui->tableWidget->setItem(nCount,2,new QTableWidgetItem(QString().asprintf("%u",pTwo)));

        int pThree = 0;
        ReadMemory4Char(nAddress,&pThree);
        ui->tableWidget->setItem(nCount,3,new QTableWidgetItem(QString().asprintf("%u",pThree)));

        float pFour = 0.0f;
        ReadMemoryFloatChar(nAddress,&pFour);
        ui->tableWidget->setItem(nCount,4,new QTableWidgetItem(QString().asprintf("%f",pFour)));

        double pFive = 0.0f;
        ReadMemoryDoubleChar(nAddress,&pFive);
        ui->tableWidget->setItem(nCount,5,new QTableWidgetItem(QString().asprintf("%lf",pFive)));

        char pChar [10];
        _strset_s(pChar,strlen(pChar),0);
        ReadMemory10StringChar(nAddress,&pChar);
        ui->tableWidget->setItem(nCount,6,new QTableWidgetItem(QString().asprintf("%s",pChar)));


    }
    m_RWProtcet.ReadWriteEnd();
}

//刷新系统进程快照
void Widget::on_RefreshProcess_clicked()
{
    if(!m_ProcessNames.empty())
        m_ProcessNames.clear();
    ui->ProcessComboBox->clear();
    GetProcessInfo();
}

//选择进程后，获取进程PID
void Widget::on_ProcessComboBox_currentTextChanged(const QString &arg1)
{
    m_pid = GetProcessId(arg1);
}

//写内存操作
void Widget::on_Write_Button_clicked()
{
    PVOID64 pTatgerAddress=nullptr;
    int nContrl = GetRadioNum();

    if(m_RWProtcet.ReadWriteBegin())
    {
        LOGEORRO("创建驱动设备失败！！！");
        return;
    }

    switch (nContrl) {
    case 1:
    {   qDebug()<<1;
        pTatgerAddress = (PVOID64)(ui->textEdit->toPlainText().toStdString().c_str());
        m_RWProtcet.WriteVirtualMemory(m_pid,
                                       reinterpret_cast<PVOID64>(ui->WritAddress->text().toULongLong(nullptr,16)),
                                       pTatgerAddress,
                                       sizeof(char));
        break;
    }
    case 2:
    {
        qDebug()<<2;
        auto value = ui->textEdit->toPlainText().toShort();
        pTatgerAddress = &value;
        m_RWProtcet.WriteVirtualMemory(m_pid,
                                       reinterpret_cast<PVOID64>(ui->WritAddress->text().toULongLong(nullptr,16)),
                                       pTatgerAddress,
                                       sizeof(short));
        break;
    }
    case 4:
    {
        qDebug()<<4;
        auto value = ui->textEdit->toPlainText().toInt();
        pTatgerAddress = &value;
        m_RWProtcet.WriteVirtualMemory(m_pid,
                                       reinterpret_cast<PVOID64>(ui->WritAddress->text().toULongLong(nullptr,16)),
                                       pTatgerAddress,
                                       sizeof(int));
        break;
    }
    case 8:
    {
        qDebug()<<8;
        auto value = ui->textEdit->toPlainText().toFloat();
        pTatgerAddress = &value;
        m_RWProtcet.WriteVirtualMemory(m_pid,
                                       reinterpret_cast<PVOID64>(ui->WritAddress->text().toULongLong(nullptr,16)),
                                       pTatgerAddress,
                                       sizeof(float));
        break;
    }
    case 16:
    {
        qDebug()<<16;
        auto value = ui->textEdit->toPlainText().toDouble();
        pTatgerAddress = &value;
        m_RWProtcet.WriteVirtualMemory(m_pid,
                                       reinterpret_cast<PVOID64>(ui->WritAddress->text().toULongLong(nullptr,16)),
                                       pTatgerAddress,
                                       sizeof(double));
        break;
    }
    case 10:
    {
        qDebug()<<10;
        pTatgerAddress = (PVOID64)(ui->textEdit->toPlainText().toStdString().c_str());
        m_RWProtcet.WriteVirtualMemory(m_pid,
                                       reinterpret_cast<PVOID64>(ui->WritAddress->text().toULongLong(nullptr,16)),
                                       pTatgerAddress,
                                       10);
        break;
    }
    default:
        break;
    }

    m_RWProtcet.ReadWriteEnd();

}

//开启进程保护
void Widget::on_EnableProcessProtect_clicked()
{
    if(m_RWProtcet.ReadWriteBegin())
    {
        LOGEORRO("创建驱动设备失败！！！");
        return;
    }
    m_RWProtcet.ENDISProcessProtect(m_pid);
    m_RWProtcet.ReadWriteEnd();
}


void Widget::on_ProcessLinkUp_clicked()
{
    if(m_RWProtcet.ReadWriteBegin())
    {
        LOGEORRO("创建驱动设备失败！！！");
        return;
    }
    m_RWProtcet.ProcessLink(m_pid);
    m_RWProtcet.ReadWriteEnd();
}


void Widget::on_Find_Button_clicked()
{
    int nCtrlNum = GetRadioNum();
    switch (nCtrlNum) {
    case 4:
    {
        FindInt();
        break;
    }
    default:
        break;
    }
}

