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

#ifndef ABOUT_H
#define ABOUT_H

#include <QDesktopServices>
#include <QDialog>
#include <QScreen>
#include <QScrollBar>
#include <QStyle>

namespace Ui {
class KYCAboutDialog;
}

#include "../include/theme.h"
#include "../utils/xatom-helper.h"

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

    void initWindow();
    void initLayout();
    void initStyle();
    void initConnect();
    void setFontStyle();
    void moveCenter();

private:
    Ui::KYCAboutDialog *ui;
    int m_iHeight;

    QStringList stylelist;
    QStringList iconthemelist;
    QGSettings *style_settings;
    QGSettings *icon_theme_settings;

private slots:
    void titlebar_icon_theme_changed(QString);
    void titlebar_style_changed(QString);
};

#endif // ABOUT_H
