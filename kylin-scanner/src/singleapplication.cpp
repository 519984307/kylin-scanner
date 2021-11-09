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

#include "singleapplication.h"
#include "include/common.h"
#include <ukui-log4qt.h>
#include <QtNetwork/QLocalSocket>
#include <QFileInfo>
#include <QLatin1String>
#include <QFileInfo>
#include <QIODevice>
#include <QStandardPaths>
#include <QMessageBox>
#include <QObject>
#include <QLibraryInfo>
#include <QTranslator>
#include <QProcess>
#include <QStandardPaths>
#include <KF5/KWindowSystem/kwindowsystem.h>
#include <fstream>
#include <stdio.h>

#define TIME_OUT                (15000)    // 500ms

typedef struct UsedUsernameAndPid{
    QString usedUsername; 	// Exist app's currently username
    QString usedPid;		// Exist app's pid
}UsedUsernameAndPid;
UsedUsernameAndPid uuap;

static bool checkStarted()
{
    QString comm;
    QDir dir("/proc/");
    QFileInfo file;
    ifstream in;
    std::string s;

    foreach (QFileInfo f, dir.entryInfoList()) {
        //KyInfo() << QString("%1").arg(getpid()) << f.baseName();
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
                    KyInfo() << "exist pid = " << f.baseName();
                    return true;
                }
            }
            in.close();
        }
    }

    return false;
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
    KyInfo() << "Exist user: " << userNow;

    return userNow;
}

static void doRead()
{
    QString line;

    QFile file(UserExistFile);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        KyInfo() << "file read error!";
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        line = in.readLine();
    }
    KyInfo() << "line: " << line;

    QStringList list = line.split(',', QString::SkipEmptyParts);
    uuap.usedUsername = list.at(0);
    uuap.usedPid = list.at(1);
    KyInfo() << "uuap.usedUsername = " << uuap.usedUsername
             << "uuap.usedPid = " << uuap.usedPid;

    file.close();
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

    KyInfo() << "exitCode: " << process->exitCode()
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
    KyInfo() << "strOutput = " << strOutput;

    return strOutput.isEmpty();
}


SingleApplication::SingleApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , w(NULL)
    , _isRunning(false)
    , _isExitWindow(false)
    , _localServer(NULL)
{
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    qApp->installTranslator(&qtTranslator);
    // For translations with different language environments
    QTranslator translator;
    QString localePath("/usr/share/kylin-scanner/translations/kylin-scanner_");
    QString locale = localePath + QLocale::system().name();

    translator.load(locale);
    qApp->installTranslator(&translator);

    QLocale lang;
    if (lang.language() == QLocale::English) {
        qDebug() << "English";
    } else if (lang.language() == QLocale::Chinese){
        qDebug() << "Chinese";
    }

    KyInfo() << "DISPLAY: " << QLatin1String(getenv ("DISPLAY"));
    _serverName = QFileInfo(QCoreApplication::applicationFilePath()).fileName()
                  + QLatin1String(getenv("DISPLAY"));

    _initLocalConnection();
}

/**
 * @brief SingleApplication::isRunning
 * Check whether an instance is running: true(isRunning), false(notRunning)
 * @return
 */
bool SingleApplication::isRunning()
{
    return _isRunning;
}

bool SingleApplication::isExitWindow()
{
    return _isExitWindow;
}

/**
 * @brief SingleApplication::_newLocalConnection
 * Through socket to accomplish single application, while has new connect to call this function
 */
void SingleApplication::_newLocalConnection()
{
    QLocalSocket *socket = _localServer->nextPendingConnection();
    if (socket) {
        socket->waitForReadyRead(2 * TIME_OUT);
        delete socket;

        // Other handles, such as read start arguments

        _activateWindow();
    }
}

/**
 * @brief SingleApplication::_initLocalConnection
 * Init local connect, if not connect to server, we should create a socket
 */
void SingleApplication::_initLocalConnection()
{
    _isRunning = false;

    _isRunning = checkStarted();
    if (_isRunning) {
        KyInfo() << "App is running, What's your choice ?";

        doRead();
        QString userUsed = uuap.usedUsername;
        QString userNow = getCurrentUserName();

        // current user straight return
        if (userUsed.compare(userNow) == 0) {
            KyInfo() << "user: " << userNow << " " << userUsed;

            QLocalSocket socket;
            socket.connectToServer(_serverName);

            if (socket.waitForConnected(TIME_OUT)) {
                fprintf(stderr, "%s already running.\n",
                        _serverName.toLocal8Bit().constData());
                _isRunning = true;
                KyInfo() << "isRunnig = " << _isRunning;

                // Other handles, such as send start arguments to server
                return;
            } else {
                KyInfo() << "isRunnig = " << _isRunning;
            }

            _newLocalServer();

            return;
        } else {
            // Different users to open `kylin-scanner`, wo display messagebos
            QMessageBox::StandardButton msgBox;
            QString msg = QObject::tr("Current ") + userUsed + QObject::tr(" User")
                    + QObject::tr(" has already opened kylin-scanner, open will close ")
                    + userUsed + QObject::tr("'s operations. Are you continue?");
            msgBox = QMessageBox::question(0, QObject::tr("Question"), msg);

            if (msgBox == QMessageBox::Yes) {
                int retKill = doKill();
                if (FAIL == retKill) {
                    KyInfo() << "Maybe you close this messagebox or choose cancel button!";
                    _isExitWindow = true;
                    _isRunning = true;
                    return;
                } else {
                    // retKill value (0,1) are not fixed, it's reasonable for us to judge whether exist's pid has been killed.
                    if (pidIsNotExist()) {
                        KyInfo() << "kill " << uuap.usedPid << " success, congratulations!";
                        _isRunning = false;

                        QLocalSocket socket;
                        socket.connectToServer(_serverName);

                        if (socket.waitForConnected(TIME_OUT)) {
                            fprintf(stderr, "%s already running.\n",
                                    _serverName.toLocal8Bit().constData());
                            _isRunning = true;
                            KyInfo() << "isRunnig = " << _isRunning;

                            // Other handles, such as send start arguments to server

                            return;
                        } else {
                            KyInfo() << "isRunnig = " << _isRunning;
                        }

                        // Cannot to connect to server, so create it
                        _newLocalServer();
                    } else {
                        KyInfo() << "kill " << uuap.usedPid << " error!";
                        _isExitWindow = true;
                        _isRunning = true;
                        return;
                    }
                }
            } else {
                KyInfo() << "You choose not open again, so directly close!";
                _isExitWindow = true;
                _isRunning = true;
                return;
            }
        }
    } else {
        KyInfo() << "App is not running, let's start it!";
        _isRunning = false;

        QLocalSocket socket;
        socket.connectToServer(_serverName);

        if (socket.waitForConnected(TIME_OUT)) {
            fprintf(stderr, "%s already running.\n",
                    _serverName.toLocal8Bit().constData());
            _isRunning = true;
            KyInfo() << "isRunnig = " << _isRunning;

            // Other handles, such as send start arguments to server

            return;
        } else {
            KyInfo() << "isRunnig = " << _isRunning;
        }

        _newLocalServer();
    }

}

/**
 * @brief SingleApplication::_newLocalServer
 * Create LocalServer
 */
void SingleApplication::_newLocalServer()
{
    _localServer = new QLocalServer(this);
    connect(_localServer, SIGNAL(newConnection()), this, SLOT(_newLocalConnection()));

    if (!_localServer->listen(_serverName)) {
        // Fail to listen, maybe application crashed, leave process server lead this
        if (_localServer->serverError() == QAbstractSocket::AddressInUseError) {
            QLocalServer::removeServer(_serverName);
            _localServer->listen(_serverName);
        }
    }
}

/**
 * @brief SingleApplication::_activateWindow
 * Activate mainwindow
 */
void SingleApplication::_activateWindow()
{
    if (w) {
        KyInfo() << "activateWindow";
        w->show();
        w->raise();

        KWindowSystem::setState(w->winId(), NET::KeepAbove); // Always show this application

        QTimer::singleShot(200, [this](){
            w->activateWindow(); // activateWindow maybe not work, so need KWindowSystem
            KWindowSystem::clearState(w->winId(), NET::KeepAbove);
        });
    }
}
