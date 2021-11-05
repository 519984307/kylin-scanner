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

#include "daemondbus.h"

int KYCDaemonDbusObject::daemonIsNotRunning()
{
    char service_name[SERVICE_NAME_SIZE];
    memset(service_name, 0, SERVICE_NAME_SIZE);
    snprintf(service_name, SERVICE_NAME_SIZE, "%s_%d", KYLIN_USER_GUIDE_SERVICE, getuid());
    QDBusConnection conn = QDBusConnection::sessionBus();
    if (!conn.isConnected())
        return -1;

    QDBusReply<QString> reply = conn.interface()->call("GetNameOwner", service_name);
    return reply.value() == "";
}

void KYCDaemonDbusObject::showGuide(QString appName)
{
    KyInfo() << Q_FUNC_INFO << appName;
    bool bRet  = false;

    char service_name[SERVICE_NAME_SIZE];
    memset(service_name, 0, SERVICE_NAME_SIZE);
    snprintf(service_name, SERVICE_NAME_SIZE, "%s_%d", KYLIN_USER_GUIDE_SERVICE, getuid());

    KyInfo() << "service_name  " << service_name;
    QDBusMessage msg = QDBusMessage::createMethodCall(QString(service_name), KYLIN_USER_GUIDE_PATH,
                                                      KYLIN_USER_GUIDE_INTERFACE, "showGuide");
    msg << appName;

    QDBusMessage response = QDBusConnection::sessionBus().call(msg);
    if (response.type() == QDBusMessage::ReplyMessage) {
        //bRet = response.arguments().at(0).toBool();
        KyInfo() << "showGuide is success!\n";
    } else {
        KyInfo() << "showGuide is fail!\n";
    }

    KyInfo() << "bRet:" << bRet;
}
