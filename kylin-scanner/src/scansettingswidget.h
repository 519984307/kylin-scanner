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
#ifndef SCANSETTINGSWIDGET_H
#define SCANSETTINGSWIDGET_H

#include "include/common.h"
#include "saneobject.h"
#include "globalsignal.h"
#include "sendmail.h"

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QStyleOption>
#include <QColor>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QToolButton>
#include <QSpacerItem>
#include <QSize>
#include <QFontMetrics>
#include <QDir>
#include <QStandardPaths>

#define ScanSettingsWidgetWidth  248
#define ScanSettingsWidgetHeight 636
#define ScanSettingsWidgetScanButtonSize  QSize(216, 36)
#define ScanSettingsWidgetComboboxSize  QSize(166, 36)
#define ScanSettingsWidgetButtonSize  QSize(102, 36)
#define ScanSettingsLabelElideWidth 49
#define ScanSettingsButtonElideWidth 147
#define ScanSettingsMailAndButtonElideWidth 70

class ScanSettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScanSettingsWidget(QWidget *parent = nullptr);

    void setupGui();
    void initConnect();

    void showTimeRow();
    void hideTimeRow();

    /// update scan settings throught sane api
    void updateScanButtonSettings();
    void updateDeviceSettings();
    void updatePageNumberSettings();
    void updateTimeSettings();
    void updateTypeSettings();
    void updateColorSettings();
    void updateResolutionSettings();
    void updateSizeSettings();
    void updateFormatSettings();
    void updateSaveNameTextSettings();
    void updateSaveDirectorySettings();
    void updateSendMailSettings();
    void updateSaveAsSettings();
    void updateSettingsForDetectDevices();
    void updateSettingsForSwitchDevices();
    void updateSettingsStatusForStartScan();
    void updateSettingsStatusForEndScan(int saneStatus);


    void setLabelAttributes(QLabel *label, const QString &text);
    void setSaveButtonAttributes(QPushButton *button, const QString &text, int elideWidth);
    void setComboboxAttributes(QComboBox *combobox, QStringList strList);

    void setNameEditTooltip();

    void warnMsg(QString msg);


protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;


Q_SIGNALS:
    //void scanButtonClickedSignal();

public slots:
    void selectSaveDirectorySlot();

    void deviceCurrentTextChangedSlot(QString text);
    void pageNumberCurrentTextChangedSlot(QString text);
    void timeCurrentTextChangedSlot(QString text);
    void typeCurrentTextChangedSlot(QString text);
    void colorCurrentTextChangedSlot(QString text);
    void resolutionCurrentTextChangedSlot(QString text);
    void sizeCurrentTextChangedSlot(QString text);
    void formatCurrentTextChangedSlot(QString text);
    void nameCurrentTextChangedSlot(QString text);

    void sendMailButtonClickedSlot();

    void scanButtonClickedSlot();

    void fontSizeChanged();
    void themeChangedBlackSlot();
    void themeChangedWhiteSlot();

private:
    QString currentSaveDirectory;
    QString currentSaveAsDirectory;

    QPushButton *m_scanButton;
    QLabel *m_deviceSettingsLabel;
    QLabel *m_deviceLabel;
    QComboBox *m_deviceComboBox;
    QLabel *m_pageNumberLabel;
    QComboBox *m_pageNumberComboBox;
    QLabel *m_timeLabel;
    QComboBox *m_timeComboBox;
    QLabel *m_typeLabel;
    QComboBox *m_typeComboBox;
    QLabel *m_colorLabel;
    QComboBox *m_colorComboBox;
    QLabel *m_resolutionLabel;
    QComboBox *m_resolutionComboBox;
    QLabel *m_fileSettingsLabel;
    QLabel *m_sizeLabel;
    QComboBox *m_sizeComboBox;
    QLabel *m_formatLabel;
    QComboBox *m_formatComboBox;
    QLabel *m_saveNameLabel;
    QLineEdit *m_saveNameEdit;
    QLabel *m_saveDirectoryLabel;
    QPushButton *m_saveDirectoryButton;
    QFormLayout *m_settingsFormLayout;

    QPushButton *m_sendMailButton;
    QPushButton *m_SaveAsButton;
    QHBoxLayout *m_buttonsHLayout;

    QVBoxLayout *m_mainVLayout;
};

#endif // SCANSETTINGSWIDGET_H
