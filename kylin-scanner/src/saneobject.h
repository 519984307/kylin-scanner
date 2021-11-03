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

#ifndef SANEOBJECT_H
#define SANEOBJECT_H

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sane/sane.h>
#include <sane/saneopts.h>
#include <iostream>
#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QtMath>
#include <QVector>
#include <QMap>
#include <QDir>
#include <QFileInfo>
using namespace std;

#include "globalsignal.h"

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

/**
 * @brief The ScanDeviceInfo struct
 * Parameters getted from specific scanner device, which could set in scansetting page
 */
typedef struct  _ScanDeviceInfo {
    bool status;
    bool have_handle;
    bool haveOpenSaneDevice;
    QStringList name;
    QStringList type;
    QStringList color;
    QStringList resolution;
    QStringList size;
}ScanDeviceInfo;

/**
 * @brief The UserSelectedInfo struct
 * Parameters chosen by user
 */
typedef struct _UserSelectedInfo {
    int deviceNameIndex;
    QString name;
    QString pageNumber;
    QString time;
    QString type;
    QString color;
    QString resolution;
    QString size;
    QString format;
    QString saveName;
    QString saveDirectory;
}UserSelectedInfo ;


enum sizes_type {
    A2 = 1,
    A3,
    A4,
    A5,
    A6
};

class  SaneObject: public QObject
{
    Q_OBJECT
public:
    static SaneObject *getInstance();

    bool getSaneStatus();
    bool getSaneHaveOpenDevice();
    bool getSaneHaveHandle();
    QStringList getSaneNames();
    QStringList getSaneTypes();
    QStringList getSaneResolutions();
    int getNumbersFromQString(const QString &qstring);
    QStringList getSaneSizes();
    QStringList getSaneColors();

    void setSaneHaveOpenSaneDevice(bool haveOpenSaneDevice);
    void setSaneHaveHandle(bool have_handle);
    void setSaneStatus(bool status);
    void setSaneNames(QStringList name);
    void setSaneTypes(QStringList type);
    void setSaneResolutions(QStringList resolution);
    void setSaneSizes(QStringList size);
    void setSaneColors(QStringList color);
    void saneExit();
    void saneClose();

    void setSaneAllParametersByUser();
    void setSaneNameByUser();
    void setSanePageNumberByUser();
    void setSaneTimeByUser();
    void setSaneTypeByUser();
    QString getSaneTypeByUser(QString type);
    void setSaneColorByUser();
    QString getSaneColorByUser(QString color);
    void setSaneResolutionByUser();
    int getSaneResolutionByUser(QString resolution);
    void setSaneSizeByUser();
    void setSaneFormatByUser();
    QString getSaneFormatByUser();
    QString getSaneSaveNameByUser();
    QString getSaneSaveDirectoryByUser();


    void detectSaneDeviceForPage();
    void openSaneDeviceForPage(int index);
    QString getFullScanFileNameExceptFormat();
    int startScanning(UserSelectedInfo info);
    bool testScannerIsAlive(QString deviceName);

    SANE_Handle handle;
    UserSelectedInfo userInfo;
    QMap<QString, QString> colorModesMap;
    QMap<QString, QString> sourceModesMap;

    int haveSourceFlag = 0;

    QString openSaneName;

    QString loadFullScanFileName;
    bool haveScanSuccessImage = false;


private:
    static SaneObject *instance;

    explicit SaneObject(QObject *parent = nullptr);
    ~SaneObject();
    SaneObject(const SaneObject &) = delete ;
    SaneObject &operator = (const SaneObject &) = delete ;

    ScanDeviceInfo devicesInfo;
};

#define g_sane_object  SaneObject::getInstance()


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief detectScanDevices
 * detect scan devices to find available devices
 */
void detectSaneDevices();


#ifdef __cplusplus
}
#endif

#endif // SANEOBJECT_H
