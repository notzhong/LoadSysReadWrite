#include "PrintLog.h"
#include <QFile>
#include <QDateTime>

//打印使用信息
void LOGINFO(const char *loginfo)
{
    QFile File("./Log.log");
    File.open(QIODevice::Append);
    QTextStream outLog(&File);
    outLog<<QString().asprintf("%s [INFO] %s\n",QDateTime::currentDateTime().toString("hh:mm:ss.zzz").toStdString().c_str(),
                               loginfo);
    File.close();

}

//打印出错信息
void LOGEORRO(const char *loginfo)
{
    QFile File("./Log.log");
    File.open(QIODevice::Append);
    QTextStream outLog(&File);
    outLog<<QString().asprintf("%s [EORRO] %s\n",QDateTime::currentDateTime().toString("hh:mm:ss.zzz").toStdString().c_str(),
                               loginfo);
    File.close();
}
