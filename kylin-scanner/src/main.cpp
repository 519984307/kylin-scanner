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
#include "singleapplication.h"

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

static void createDir(QString dirPath)
{
    QDir configDir(dirPath);
    if (! configDir.exists()) {
        configDir.mkdir(dirPath);
    }
}

static void createScannerDir()
{
    createDir(g_config_signal->configPath);
    createDir(g_config_signal->scannerPath);
    createDir(g_config_signal->scannerImagePath);
    createDir(g_config_signal->scannerTempPath);
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

    KyInfo() << "Exist user: " << userNow;

    return userNow;
}

static void doWrite(QString userNow, int pidNow)
{
    QFile file(UserExistFile);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        KyInfo() << "open this file error!";
        return;
    }

    // Easy to other user to modify `scanner/user.id` file
    file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner
                        | QFileDevice::ReadUser | QFileDevice::WriteUser
                        | QFileDevice::ReadGroup | QFileDevice::WriteGroup
                        | QFileDevice::ReadOther | QFileDevice::WriteOther);

    QString message = userNow + "," + QString::number(pidNow);
    KyInfo() << "doWrite: message= " << message;

    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();
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

static QString execCmd(QString cmd)
{
    KyInfo() << "cmd = " << cmd;
    QProcess process;
    process.start(QString(cmd));
    process.waitForFinished();
    QByteArray result = process.readAllStandardOutput();
    result = result.left(result.length()-1);
    KyInfo() << "arch = " << result;
    return result;
}
static QString getSystemArchitecture()
{
    QString archResult = execCmd("arch");
    return archResult;
}

static QString getAppVersion()
{
    QString versionResult = execCmd(QString("dpkg-parsechangelog -l %1 --show-field Version").arg(ChangelogFilePath));
    return versionResult;
}

int main(int argc, char *argv[])
{
    int exitCode = 0;

    initUkuiLog4qt("kylin-scanner");

#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 0))
        QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
  QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    SingleApplication app(argc, argv);

    createScannerDir();

    getSystemArchitecture();
    app.setApplicationVersion(getAppVersion());
    app.setWindowIcon (QIcon::fromTheme("kylin-scanner"));

    QCommandLineParser parser;
    parser.setApplicationDescription("An interface-friendly scanner with OCR, Smart rectify and OneClickBeauty.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.process(app);

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

    if (locale == "zh_CN" || locale == "bo_CN") {
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

    if (app.isExitWindow()) {
        return 0;
    }

    if (! app.isRunning()) {
        QString userNow = getCurrentUserName();
        int pidNow = app.applicationPid();

        doWrite(userNow, pidNow);

        MainWidget w;
        app.w = &w;
        w.show();

        return app.exec();
#if 0
        // For restart application
        if (exitCode == MainWidget::EXIT_CODE_REBOOT) {
            QProcess::startDetached(qApp->applicationFilePath(), QStringList());
            return 0;
        }
#endif
    }
}
