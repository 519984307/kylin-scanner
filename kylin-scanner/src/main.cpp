/*
* Copyright (C) 2021, KylinSoft Co., Ltd.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 3, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
*
*/
#include "include/common.h"
#include "globalsignal.h"
#include "mainwidget.h"
#include "scandialog.h"
#include <ukui-log4qt.h>

#include <QApplication>
#include <QByteArray>
#include <QDir>
#include <QLibraryInfo>
#include <QLocale>
#include <QProcess>
#include <QTranslator>
#include <X11/Xlib.h> // ought to put this file last
#include <QCommandLineParser>
#include <QIODevice>
#include <QStandardPaths>
#include <QMessageBox>
#include <QObject>
#include <QDir>
#include <QFileInfo>
#include <fstream>
#include <stdio.h>
using namespace std;


typedef struct UsedUsernameAndPid{
    QString usedUsername; 	// Exist app's currently username
    QString usedPid;		// Exist app's pid
}UsedUsernameAndPid;
UsedUsernameAndPid uuap;

static void createScannerDir()
{
    QDir configDir(g_config_signal->configPath);
    if (! configDir.exists()) {
        configDir.mkdir(g_config_signal->configPath);
    }

    QDir scannerDir(g_config_signal->scannerPath);
    if (! scannerDir.exists()) {
        scannerDir.mkdir(g_config_signal->scannerPath);
    }

    QDir scannerImageDir(g_config_signal->scannerImagePath);
    if (! scannerImageDir.exists()) {
        scannerImageDir.mkdir(g_config_signal->scannerImagePath);
    }

    QDir scannerTempDir(g_config_signal->scannerTempPath);
    if (!scannerTempDir.exists()) {
        scannerTempDir.mkdir(g_config_signal->scannerTempPath);
    }
}


static QString getCurrentUserName()
{
    QString cmd(BashType);
    QStringList arglists;

    arglists << "-c";
    arglists << "whoami";

    QProcess *process = new QProcess();
    process->start(cmd, arglists);
    process->waitForFinished();
    QString userNow = QString::fromLocal8Bit(process->readAllStandardOutput().trimmed());
    //QString userNow = QString::fromStdString(process->readAllStandardOutput().toStdString());
    qDebug() << "Exist user: " << userNow;

    return userNow;
}

static void doWrite(QString userNow, int pidNow)
{
    QFile file(UserExistFile);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        qDebug() << "open this file error!";
        return;
    }

    // Easy to other user to modify `scanner/user.id` file
    file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner
                        | QFileDevice::ReadUser | QFileDevice::WriteUser
                        | QFileDevice::ReadGroup | QFileDevice::WriteGroup
                        | QFileDevice::ReadOther | QFileDevice::WriteOther);

    QString message = userNow + "," + QString::number(pidNow);
    qDebug() << "doWrite: message= " << message;

    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();
}

static void doRead()
{
    QString line;

    QFile file(UserExistFile);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "file read error!";
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        line = in.readLine();
    }
    qDebug() << "line: " << line;

    QStringList list = line.split(',', QString::SkipEmptyParts);
    uuap.usedUsername = list.at(0);
    uuap.usedPid = list.at(1);
    qDebug() << "uuap.usedUsername = " << uuap.usedUsername
             << "uuap.usedPid = " << uuap.usedPid;

    file.close();
}

void writeCurrentAppUsernameAndPid(QString userNow, int pidNow)
{
    doWrite(userNow, pidNow);
}

/**
 * @brief doKill
 * @return
 * Cancel: exitCode():127	exitStatus():QProcess::NormalExit	error():QProcess::UnknownError
 * Timeout: exitCode():0 	exitStatus():QProcess::NormalExit	error():QProcess::Timedout => so -1, do not timeout
 * Normal Password Input Correctly: exitCode():1 exitStatus():QProcess::NormalExit
 */
static int doKill()
{
    QString cmd(BashType);
    QStringList arglists;

    QString args = QString("pkexec kill -9 ") + uuap.usedPid;

    arglists << "-c";
    arglists << args;

    QProcess *process = new QProcess();
    process->start(cmd, arglists);
    process->waitForFinished(-1);

    qDebug() << "exitCode: " << process->exitCode()
             << ", exitStatus: " << process->exitStatus()
             << ", error: " << process->error()
             << ", result err: " << process->readAllStandardError()
             << ", result out: " << process->readAllStandardOutput();

    if (process->exitCode() == 1 || process->exitCode() == 0)
        // Password Input is correctly
        return SUCCESS;

    return FAIL;
}

static bool pidIsNotExist()
{
    QString cmd(BashType);
    QStringList arglists;

    arglists << "-c";
    QString args = QString("ps -ef | grep ") + uuap.usedPid + QString(" |grep -v log |grep -v pts |grep -v grep");
    arglists<< args;

    QProcess *process = new QProcess();
    process->start(cmd, arglists);
    process->waitForFinished();
    QString strOutput = QString::fromLocal8Bit(process->readAllStandardOutput().trimmed());
    qDebug() << "strOutput = " << strOutput;

    return strOutput.isEmpty();
}

/**
 * @brief singleWarnMsg
 * 当前xx用户正在使用该软件，打开将会关闭xx用户正在进行的操作
 * @return
 */
int singleWarnMsg()
{
    // normal user: cannot read by `ps -ef`
    //userUsed = getUsedUserName();

    doRead();
    QString userUsed = uuap.usedUsername;
    QString userNow = getCurrentUserName();

#if 1
    // current user straight return
    if (userUsed.compare(userNow) == 0) {
        qDebug() << "user: " << userNow << " " << userUsed;
        return 0;
    }
#endif

    // Q, Because vnc login, so we should message it again ?
    QMessageBox::StandardButton msgBox;
    QString msg = QObject::tr("User ") + userUsed + QObject::tr(" has already opened kylin-scanner, continue open will close ")
            + userUsed + QObject::tr("'s operations.");
    msgBox = QMessageBox::question(0, QObject::tr("Question"), msg);

    if (msgBox == QMessageBox::Yes)
        return 1;
    else
        return 0;
}

int getScreenWidth()
{
    Display *disp = XOpenDisplay(NULL);
    Screen *scrn = DefaultScreenOfDisplay(disp);
    if (NULL == scrn)
        return 0;

    int width = scrn->width;

    if (NULL != disp)
        XCloseDisplay(disp);

    return width;
}

static bool checkStarted()
{
    QString comm;
    QDir dir("/proc/");
    QFileInfo file;
    ifstream in;
    std::string s;

    foreach (QFileInfo f, dir.entryInfoList()) {
        //qDebug() << QString("%1").arg(getpid()) << f.baseName();
        if (f.baseName() == QString("%1").arg(getpid())
                || f.baseName() == "self"
                || f.baseName() == "thread-self")
            continue;

        comm = QString("/proc/%1/comm").arg(f.baseName());
        file.setFile(comm);

        if (file.exists()) {
            in.open(file.absoluteFilePath().toStdString());
            if (in.is_open()) {
                getline(in, s);
                comm = QString::fromStdString(s);
                if (comm == "kylin-scanner") {
                    qDebug() << "exist pid = " << f.baseName();
                    return true;
                }
            }
            in.close();
        }
    }

    return false;
}

static QString getSystemArchitecture()
{
    QProcess process;
    process.start(QString("arch"));
    process.waitForFinished();
    QByteArray result = process.readAllStandardOutput();
    result = result.left(result.length()-1);
    qDebug() << "arch = " << result;
    return result;
}

static QString getAppVersion()
{
    QProcess process;
    process.start(QString("dpkg-parsechangelog -l %1 --show-field Version").arg(ChangelogFilePath));
    process.waitForFinished();
    QByteArray result = process.readAllStandardOutput();
    result = result.left(result.length()-1);
    qDebug() << "version = " << result;
    return result;
}

int main(int argc, char *argv[])
{
    initUkuiLog4qt("kylin-scanner");

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
        QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    QApplication app(argc, argv);

    createScannerDir();

    getSystemArchitecture();
    app.setApplicationVersion(getAppVersion());
    app.setWindowIcon (QIcon::fromTheme("kylin-scanner"));

    QCommandLineParser parser;
    parser.setApplicationDescription("An interface-friendly scanner with OCR, Smart rectify and OneClickBeauty.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);

    // Translation
    QTranslator app_trans;
    QTranslator qt_trans;
    QString locale = QLocale::system().name();
    QString trans_path;
    QString qt_trans_path;

    if (QDir("/usr/share/kylin-scanner/translations").exists()) {
        trans_path = "/usr/share/kylin-scanner/translations";
    } else {
        trans_path = qApp->applicationDirPath();
    }
    qt_trans_path = QLibraryInfo::location(QLibraryInfo::TranslationsPath); // /usr/share/qt5/translations

    if (locale == "zh_CN") {
        if(! app_trans.load("kylin-scanner_" + locale + ".qm", trans_path)){
            KyInfo() << "Load translation file："<< "kylin-scanner_" + locale + ".qm from" << trans_path << "failed!";
        } else {
            app.installTranslator(&app_trans);
        }

        if (! qt_trans.load("qt_" + locale + ".qm", qt_trans_path)){
            KyInfo() << "Load translation file："<< "qt_" + locale + ".qm from" << qt_trans_path << "failed!";
        } else {
            app.installTranslator(&qt_trans);
        }
    }

    bool isRunning = false;
    if (checkStarted()) {
        qDebug() << "App is running, What's your choice ?";
        //QMessageBox::information(NULL, "Cannot start KylinScanner", "KylinScanner has started in current system now.");
        doRead();

        QString userUsed = uuap.usedUsername;
        QMessageBox::StandardButton msgBox;
        QString msg = QObject::tr("Current ") + userUsed + QObject::tr(" User")
                + QObject::tr(" has already opened kylin-scanner, open will close ")
                + userUsed + QObject::tr("'s operations. Are you continue?");
        msgBox = QMessageBox::question(0, QObject::tr("Question"), msg);

        if (msgBox == QMessageBox::Yes) {
            int retKill = doKill();
            if (FAIL == retKill) {
                qDebug() << "Maybe you close this messagebox or choose cancel button!";
                return 0;
            } else {
                // retKill value (0,1) are not fixed, it's reasonable for us to judge whether exist's pid has been killed.
                if (pidIsNotExist()) {
                    qDebug() << "kill " << uuap.usedPid << " success, congratulations!";
                    isRunning = false;
                } else {
                    qDebug() << "kill " << uuap.usedPid << " error!";
                    isRunning = true;
                }
            }
        } else {
            qDebug() << "You choose not open again, so directly close!";
            isRunning = true;
            return 0;
        }
    } else {
         qDebug() << "App is not running, let's start it!";
         isRunning = false;
    }

    if (isRunning) {
        qDebug() << "isRunning.";
    } else {
        QString userNow = getCurrentUserName();
        int pidNow = app.applicationPid();
        qDebug() << "Exist user= " << userNow << ", pid= " << pidNow;

        doWrite(userNow, pidNow);

        MainWidget w;
        w.show();
        return app.exec();
    }
    return 0;
}
