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
#ifndef COMMON_H
#define COMMON_H

#include <QString>
#include <QSize>
#include <QColor>
#include <QDir>
#include <QFileInfo>

/******** Mainwidget ********/
#define MainwidgetWidth 960
#define MainWidgetHeight 676
#define ApplicationName     "Scanner"

/******** TitlebarWidget ********/
#define TitlebarLogoSize  QSize(24,24)
#define TitlebarFontSize  14
#define TitlebarButtonSize  QSize(30,30)
#define TitlebarHeight  40



#define ChineseTranslationFilePath			"/usr/share/kylin-scanner/translations/kylin-scanner."
#define ChangelogFilePath                      "/usr/share/doc/kylin-scanner/changelog.Debian.gz"

#define UserExistFile 		"/tmp/kylin-scanner/user.pid"
#define MailPicture					"/tmp/kylin-scanner/mail.jpg"
#define ScanningPicture				"/tmp/kylin-scanner/scanning.png"

#define BashType 							"/bin/bash"

#define SUCCESS								0
#define FAIL								-1



#endif // COMMON_H
