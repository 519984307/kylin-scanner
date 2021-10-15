/*
* Copyright (C) 2020, KylinSoft Co., Ltd.
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

#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QObject>
#include <QApplication>
#include <QtNetwork/QLocalServer>

#include "mainwidget.h"

class SingleApplication : public QApplication
{
    Q_OBJECT
public:
    SingleApplication(int &argc, char **argv);

    bool isRunning();
    bool isExitWindow();
    MainWidget *w;

private slots:
    void _newLocalConnection();

private:
    void _initLocalConnection();
    void _newLocalServer();
    void _activateWindow();

    bool _isRunning;
    bool _isExitWindow;
    QLocalServer *_localServer;
    QString _serverName;
};


#endif // SINGLEAPPLICATION_H
