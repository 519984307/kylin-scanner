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
#include "scansettingswidget.h"
#include "include/common.h"

#include <ukui-log4qt.h>

#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QWidgetList>
#include <QApplication>
#include <QListView>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QDateTime>

ScanSettingsWidget::ScanSettingsWidget(QWidget *parent) :
    QWidget(parent),
    m_scanButton(new QPushButton()),
    m_deviceSettingsLabel(new QLabel()),
    m_deviceLabel(new QLabel()),
    m_deviceComboBox(new QComboBox()),
    m_pageNumberLabel(new QLabel()),
    m_pageNumberComboBox(new QComboBox()),
    m_timeLabel(new QLabel()),
    m_timeComboBox(new QComboBox()),
    m_typeLabel(new QLabel()),
    m_typeComboBox(new QComboBox()),
    m_colorLabel(new QLabel()),
    m_colorComboBox(new QComboBox()),
    m_resolutionLabel(new QLabel()),
    m_resolutionComboBox(new QComboBox()),
    m_fileSettingsLabel(new QLabel()),
    m_sizeLabel(new QLabel()),
    m_sizeComboBox(new QComboBox()),
    m_formatLabel(new QLabel()),
    m_formatComboBox(new QComboBox()),
    m_saveNameLabel(new QLabel()),
    m_saveNameEdit(new QLineEdit()),
    m_saveDirectoryLabel(new QLabel()),
    m_saveDirectoryButton(new QPushButton()),
    m_settingsFormLayout(new QFormLayout()),
    m_sendMailButton(new QPushButton()),
    m_SaveAsButton(new QPushButton()),
    m_buttonsHLayout(new QHBoxLayout()),
    m_mainVLayout(new QVBoxLayout(this))
{
    setupGui();
    initConnect();
}

void ScanSettingsWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    QPainterPath rectPath;
    rectPath.addRoundedRect(this->rect(), 0, 0);
    QStyleOption opt;
    opt.init(this);

    QColor mainColor;
    mainColor = opt.palette.color(QPalette::Window);

    p.fillPath(rectPath,QBrush(mainColor));
}

void ScanSettingsWidget::selectSaveDirectorySlot()
{
    if (currentSaveDirectory.isEmpty()) {
        QString scannerPicturePath = g_config_signal->scannerImagePath;
        currentSaveDirectory = scannerPicturePath ;
    }

    QString midPath = currentSaveDirectory;
    QString dlgTitle = tr("Select a directory");
    QString selectedDir = QFileDialog::getExistingDirectory(this, dlgTitle, currentSaveDirectory, QFileDialog::ShowDirsOnly);
    KyInfo() << "selected directory: " << selectedDir;

    if (!selectedDir.isEmpty()) {
        QFileInfo file(selectedDir);
        if (file.permission(QFileDevice::WriteUser | QFileDevice::ReadGroup)) {
            KyInfo() << "The user could read and write " << selectedDir;

            currentSaveDirectory = selectedDir;
            setSaveButtonAttributes(m_saveDirectoryButton, currentSaveDirectory, ScanSettingsButtonElideWidth);
        } else {
            if (file.isWritable()) {
                KyInfo() << "The user can read and write " << selectedDir;

                currentSaveDirectory = selectedDir;
                setSaveButtonAttributes(m_saveDirectoryButton, currentSaveDirectory, ScanSettingsButtonElideWidth);

                return;
            }
            KyInfo() << "The user can't read and write " << selectedDir;

            QString msg = tr("Currently user has no permission to modify directory ") + selectedDir;
            warnMsg(msg);
        }
    }
    g_sane_object->userInfo.saveDirectory = currentSaveDirectory;

    KyInfo() << "saveDirectory = " << currentSaveAsDirectory;
}

void ScanSettingsWidget::deviceCurrentTextChangedSlot(QString text)
{
    KyInfo() << "Sane device name: " << g_sane_object->userInfo.name << "current devece name: " << m_deviceComboBox->currentText();

    if (g_sane_object->userInfo.name.isEmpty()) {
        g_sane_object->userInfo.name = text;

        KyInfo() << "We do not need open device again.";
        return;
    } else {
        KyInfo() << "Device changed, therefore, we need open device again.";
        g_sane_object->userInfo.name = text;
    }

    int curTextLen = m_deviceComboBox->currentText().length();

    if ( curTextLen >= 20) {
        m_deviceComboBox->setToolTip(m_deviceComboBox->currentText());
    } else {
        m_deviceComboBox->setToolTip("");
    }

    // index is chosen by user manually, or default(0)
    int index = m_deviceComboBox->currentIndex();
    if (index == -1) {
        index = 0;
    }

    g_sane_object->userInfo.deviceNameIndex = index;

    g_sane_object->saneClose();

    // while switch scan device, we should open the scan device to get some parameters
    g_sane_object->openDeviceIndex = index;
    g_sane_object->openSaneDeviceForPage(index);

    updateSettingsForSwitchDevices();
}

/**
 * @brief ScanSettingsWidget::pageNumberCurrentTextChangedSlot
 * @param text
 *
 */
void ScanSettingsWidget::pageNumberCurrentTextChangedSlot(QString text)
{
    if (text.compare("Multiple", Qt::CaseInsensitive) == 0
            || text.compare("多页扫描", Qt::CaseInsensitive) == 0) {
        g_sane_object->userInfo.pageNumber = tr("Multiple");

        // Avoid SANE_STATUS_NO_DOC Error to set time not enable.
        g_sane_object->setSaneStatus(true);
        showTimeRow();
    } else {
        g_sane_object->userInfo.pageNumber = tr("Single");
        hideTimeRow();
    }
}

void ScanSettingsWidget::timeCurrentTextChangedSlot(QString text)
{
    g_sane_object->userInfo.time = text;

    KyInfo() << "userInfo.time = " << text;
}

void ScanSettingsWidget::typeCurrentTextChangedSlot(QString text)
{
    g_sane_object->userInfo.type = text;

    KyInfo() << "userInfo.type = " << text;
}

void ScanSettingsWidget::colorCurrentTextChangedSlot(QString text)
{
    if (0 == QString::compare(text, tr("Color"), Qt::CaseInsensitive)) {
        g_sane_object->userInfo.color = "Color";
    } else if(0 == QString::compare(text, tr("Lineart"), Qt::CaseInsensitive)) {
        g_sane_object->userInfo.color = "Lineart";
    } else {
        g_sane_object->userInfo.color = "Gray";
    }

    KyInfo() << "userInfo.color = " << text;
}

void ScanSettingsWidget::resolutionCurrentTextChangedSlot(QString text)
{
    if ((0 == text.compare(tr("4800 dpi"), Qt::CaseInsensitive))
         || (0 == text.compare(tr("2400 dpi"), Qt::CaseInsensitive))
         || (0 == text.compare(tr("1200 dpi"), Qt::CaseInsensitive))) {

        QString msg = tr("This resolution will take a loog time to scan, please choose carelly.");
        warnMsg(msg);
    }

    g_sane_object->userInfo.resolution = text;

    KyInfo() << "userInfo.resolution = " << text;
}

void ScanSettingsWidget::sizeCurrentTextChangedSlot(QString text)
{
    g_sane_object->userInfo.size = text;
    KyInfo() << "userInfo.size = " << text;
}

void ScanSettingsWidget::formatCurrentTextChangedSlot(QString text)
{
    g_sane_object->userInfo.format = text;
    KyInfo() << "userInfo.format = " << text;
}

void ScanSettingsWidget::nameCurrentTextChangedSlot(QString text)
{
    QString msg;
    if (m_saveNameEdit->text().contains(QChar('/'), Qt::CaseInsensitive)) {
        msg = tr("cannot contain '/' character.");
        warnMsg(msg);
        m_saveNameEdit->cursorBackward(true, 1); // move left a character
        m_saveNameEdit->del(); // delete right or selected text
    }
    if (m_saveNameEdit->text().startsWith(QChar('.'), Qt::CaseInsensitive)) {
        msg = tr("cannot save as hidden file.");
        warnMsg(msg);
        m_saveNameEdit->cursorBackward(true, 1);
        m_saveNameEdit->del();
    }

    g_sane_object->userInfo.saveName = m_saveNameEdit->text();
    KyInfo() << "saveName = " << g_sane_object->userInfo.saveName;

    setNameEditTooltip();
}

void ScanSettingsWidget::sendMailButtonClickedSlot()
{
    int retDialog;

    AppList *maillist = getAppIdList(MailType);

    if (! maillist) {
        NoMailDialog *dialog = new NoMailDialog(this);

        retDialog = dialog->exec();
        if (retDialog == QDialog::Accepted) {
            QProcess *process = new QProcess();
            process->start("/usr/bin/kylin-software-center");
        }
        delete dialog;
    } else {
        KyInfo() << "Get mail client list success.";

        SendMailDialog *dialog = new SendMailDialog(this);

        dialog->setMailSelectComboboxItems();

        dialog->exec();
    }

    g_user_signal->sendMailButtonClicked();
}

void ScanSettingsWidget::saveAsButtonClickedSlot()
{
    QString filename;
    QString filepath;
    QString filetype;
    QString msg;
    QString titlename;
    bool flagSave = false;

    while (! flagSave) {
        if (g_sane_object->ocrFlag == 1) {
            if (m_saveNameEdit->text().endsWith(".txt")) {
                filename = m_saveNameEdit->text();
            } else {
                filename = m_saveNameEdit->text() + ".txt";
            }
            titlename = tr("Store text");
        } else {
            if (m_saveNameEdit->text().endsWith(".jpg")
                    || m_saveNameEdit->text().endsWith(".png")
                    || m_saveNameEdit->text().endsWith(".bmp")
                    || m_saveNameEdit->text().endsWith(".pdf")) {
                filename = m_saveNameEdit->text();
            } else {
                filename = m_saveNameEdit->text() + "." + m_formatComboBox->currentText();
            }
            titlename = tr("Save as dialog");
        }
        filepath = currentSaveDirectory;

        KyInfo() << "filepath = " << filepath;


        QWidget *widget = nullptr;
        QWidgetList widgetList = QApplication::allWidgets();
        for (int i=0; i<widgetList.length(); ++i) {
            if (widgetList.at(i)->objectName() == "MainWindow") {
                widget = widgetList.at(i);
            }
        }

        SaveFileDialog *saveDialog = new SaveFileDialog(widget, g_sane_object->ocrFlag, filename, titlename);

        if (widget) {
            QRect rect = widget->geometry();
            int x = rect.x() + rect.width()/2 - saveDialog->width()/2;
            int y = rect.y() + rect.height()/2 - saveDialog->height()/2;
            saveDialog->move(x,y);
        }

        saveDialog->setSaveAsDirectory(filepath);

        if (saveDialog->exec() == QFileDialog::Accepted) {
            filepath = saveDialog->selectedFiles().at(0);
            filename = saveDialog->getFileName();
            KyInfo() << "filepath = " << filepath
                     << "filename = " << filename;

            if (filepath.isNull())
                break;

            if (filename.contains(QChar('/'), Qt::CaseInsensitive)) {
                msg = tr("cannot contain '/' character.");
                warnMsg(msg);
                continue;
            }
            if (filename.startsWith(QChar('.'), Qt::CaseInsensitive)) {
                msg = tr("cannot save as hidden file.");
                warnMsg(msg);
                continue;
            }

            // While not endsWith (".jpg" | ".png" | "pdf" | "bmp"),
            // we neet add suffix behind.
            if (g_sane_object->ocrFlag == 1) {
                if (!filename.endsWith(".txt", Qt::CaseInsensitive)) {
                    filetype = saveDialog->getFileType();
                    filepath = filepath.append(filetype);
                }
            } else {
                if (!filename.endsWith(".jpg", Qt::CaseInsensitive)
                        && !filename.endsWith(".png", Qt::CaseInsensitive)
                        && !filename.endsWith(".pdf", Qt::CaseInsensitive)
                        && !filename.endsWith(".bmp", Qt::CaseInsensitive)) {

                    filetype = saveDialog->getFileType();
                    filepath = filepath.append(filetype);
                }
            }
            flagSave = true;
        } else {
            flagSave = false;
            return;
        }
    }

    KyInfo() << "filepath = " << filepath;

    if (flagSave) {
        g_user_signal->saveAsButtonClicked(filepath);
    }
}

void ScanSettingsWidget::scanButtonClickedSlot()
{
    updateSettingsStatusForStartScan();

    g_user_signal->startScanOperation();
}

void ScanSettingsWidget::fontSizeChanged()
{
    setLabelAttributes(m_deviceLabel, tr("Device"));
    setLabelAttributes(m_pageNumberLabel, tr("Pages"));
    setLabelAttributes(m_timeLabel, tr("Time"));
    setLabelAttributes(m_typeLabel, tr("Type"));
    setLabelAttributes(m_colorLabel, tr("Colour"));
    setLabelAttributes(m_resolutionLabel, tr("Resolution"));
    setLabelAttributes(m_sizeLabel, tr("Size"));
    setLabelAttributes(m_formatLabel, tr("Format"));
    setLabelAttributes(m_saveNameLabel, tr("Name"));
    setLabelAttributes(m_saveDirectoryLabel, tr("Save"));

    setSaveButtonAttributes(m_saveDirectoryButton, currentSaveDirectory, ScanSettingsButtonElideWidth);
    setSaveButtonAttributes(m_sendMailButton, tr("Mail to"), ScanSettingsMailAndButtonElideWidth);
    setSaveButtonAttributes(m_SaveAsButton, currentSaveAsDirectory, ScanSettingsMailAndButtonElideWidth);
}

void ScanSettingsWidget::themeChangedBlackSlot()
{
    m_scanButton->setStyleSheet("QPushButton{background: rgba(55, 144, 250, 1); border-radius: 6px;color:black;}"
                                "QPushButton:hover{background: rgba(55, 144, 250, 0.8);border-radius: 6px; color:black;}"
                                "QPushButton:pressed{background: rgba(36, 109, 212, 1);border-radius: 6px; color:black;}"
                                );

}

void ScanSettingsWidget::themeChangedWhiteSlot()
{
    m_scanButton->setStyleSheet("QPushButton{background: rgba(55, 144, 250, 1); border-radius: 6px; color:white;}"
                                "QPushButton:hover{background: rgba(55, 144, 250, 0.8);border-radius: 6px; color:white;}"
                                "QPushButton:pressed{background: rgba(36, 109, 212, 1);border-radius: 6px; color:white;}"
                                );
}

void ScanSettingsWidget::setupGui()
{
    setMaximumWidth(ScanSettingsWidgetWidth);

    QLabel *scanButtonLeftLabel = new QLabel();
    QLabel *scanButtonRightLabel = new QLabel();
    QHBoxLayout *scanButtonHLayout = new QHBoxLayout();

    scanButtonLeftLabel->setPixmap(QPixmap(":/default-connect-page/begin-scan.svg"));
    scanButtonRightLabel->setText(tr("Begin Scan"));

    scanButtonHLayout->addStretch();
    scanButtonHLayout->addWidget(scanButtonLeftLabel);
    scanButtonHLayout->addSpacing(4);
    scanButtonHLayout->addWidget(scanButtonRightLabel);
    scanButtonHLayout->addStretch();

    m_scanButton->setLayout(scanButtonHLayout);
    m_scanButton->setFixedSize(ScanSettingsWidgetScanButtonSize);
    m_scanButton->setCursor(Qt::PointingHandCursor);

    m_deviceComboBox->setFixedSize(ScanSettingsWidgetComboboxSize);

    m_deviceSettingsLabel->setText(tr("Scanner device"));

    m_pageNumberComboBox->setFixedSize(ScanSettingsWidgetComboboxSize);

    m_timeComboBox->setFixedSize(ScanSettingsWidgetComboboxSize);

    m_typeComboBox->setFixedSize(ScanSettingsWidgetComboboxSize);

    m_colorComboBox->setFixedSize(ScanSettingsWidgetComboboxSize);

    m_resolutionComboBox->setFixedSize(ScanSettingsWidgetComboboxSize);

    m_fileSettingsLabel->setText(tr("File settings"));

    m_sizeComboBox->setFixedSize(ScanSettingsWidgetComboboxSize);

    m_formatComboBox->setFixedSize(ScanSettingsWidgetComboboxSize);

    m_saveNameEdit->setText(tr("scanner01"));
    m_saveNameEdit->setFixedSize(ScanSettingsWidgetComboboxSize);

    m_saveDirectoryButton->setFixedSize(ScanSettingsWidgetComboboxSize);
    if (currentSaveDirectory.isEmpty()) {
        QString scannerPicturePath = g_config_signal->scannerImagePath;
        currentSaveDirectory = scannerPicturePath ;

        g_sane_object->userInfo.saveDirectory = currentSaveDirectory;

        KyInfo() << "saveDirectory = " << currentSaveDirectory;
    }

    m_settingsFormLayout->setSpacing(7);
    m_settingsFormLayout->setVerticalSpacing(8);
    m_settingsFormLayout->addRow(m_scanButton);
    m_settingsFormLayout->addRow(m_deviceSettingsLabel);
    m_settingsFormLayout->addRow(m_deviceLabel, m_deviceComboBox);
    m_settingsFormLayout->addRow(m_pageNumberLabel, m_pageNumberComboBox);
    //m_settingsFormLayout->addRow(m_timeLabel, m_timerComboBox);
    m_settingsFormLayout->addRow(m_typeLabel, m_typeComboBox);
    m_settingsFormLayout->addRow(m_colorLabel, m_colorComboBox);
    m_settingsFormLayout->addRow(m_resolutionLabel, m_resolutionComboBox);
    m_settingsFormLayout->addRow(m_fileSettingsLabel);
    m_settingsFormLayout->addRow(m_sizeLabel, m_sizeComboBox);
    m_settingsFormLayout->addRow(m_formatLabel, m_formatComboBox);
    m_settingsFormLayout->addRow(m_saveNameLabel, m_saveNameEdit);
    m_settingsFormLayout->addRow(m_saveDirectoryLabel, m_saveDirectoryButton);
    m_settingsFormLayout->setLabelAlignment(Qt::AlignRight);
    m_settingsFormLayout->setFormAlignment(Qt::AlignLeft);
    m_settingsFormLayout->setContentsMargins(0, 0, 0, 0);

    m_sendMailButton->setFixedSize(ScanSettingsWidgetButtonSize);

    m_SaveAsButton->setFixedSize(ScanSettingsWidgetButtonSize);
    currentSaveAsDirectory = tr("Save as");

    fontSizeChanged();

    m_buttonsHLayout->setSpacing(0);
    m_buttonsHLayout->addWidget(m_sendMailButton);
    m_buttonsHLayout->addSpacing(12);
    m_buttonsHLayout->addWidget(m_SaveAsButton);
    m_buttonsHLayout->setContentsMargins(0, 0, 0, 0);

    m_mainVLayout->setSpacing(0);
    m_mainVLayout->addSpacing(17);
    m_mainVLayout->addLayout(m_settingsFormLayout);
    m_mainVLayout->addLayout(m_buttonsHLayout);

    setLayout(m_mainVLayout);
}

void ScanSettingsWidget::initConnect()
{
    connect(m_scanButton, &QPushButton::clicked, this, &ScanSettingsWidget::scanButtonClickedSlot);

    connect(m_deviceComboBox, &QComboBox::currentTextChanged, this, &ScanSettingsWidget::deviceCurrentTextChangedSlot);
    connect(m_pageNumberComboBox, &QComboBox::currentTextChanged, this, &ScanSettingsWidget::pageNumberCurrentTextChangedSlot);
    connect(m_timeComboBox, &QComboBox::currentTextChanged, this, &ScanSettingsWidget::timeCurrentTextChangedSlot);
    connect(m_typeComboBox, &QComboBox::currentTextChanged, this, &ScanSettingsWidget::typeCurrentTextChangedSlot);
    connect(m_colorComboBox, &QComboBox::currentTextChanged, this, &ScanSettingsWidget::colorCurrentTextChangedSlot);
    connect(m_resolutionComboBox, &QComboBox::currentTextChanged, this, &ScanSettingsWidget::resolutionCurrentTextChangedSlot);
    connect(m_sizeComboBox, &QComboBox::currentTextChanged, this, &ScanSettingsWidget::sizeCurrentTextChangedSlot);
    connect(m_formatComboBox, &QComboBox::currentTextChanged, this, &ScanSettingsWidget::formatCurrentTextChangedSlot);
    connect(m_saveNameEdit, &QLineEdit::textChanged, this, &ScanSettingsWidget::nameCurrentTextChangedSlot);
    connect(m_saveDirectoryButton, &QPushButton::clicked, this, &ScanSettingsWidget::selectSaveDirectorySlot);

    connect(m_sendMailButton, &QPushButton::clicked, this, &ScanSettingsWidget::sendMailButtonClickedSlot);
    connect(m_SaveAsButton, &QPushButton::clicked, this, &ScanSettingsWidget::saveAsButtonClickedSlot);


    connect(g_user_signal, &GlobalUserSignal::fontSizeChangedSignal, this, &ScanSettingsWidget::fontSizeChanged);
    connect(g_user_signal, &GlobalUserSignal::themeChangedBlackSignal, this, &ScanSettingsWidget::themeChangedBlackSlot);
    connect(g_user_signal, &GlobalUserSignal::themeChangedWhiteSignal, this, &ScanSettingsWidget::themeChangedWhiteSlot);

    connect(g_user_signal, &GlobalUserSignal::scanThreadFinishedSignal, this, &ScanSettingsWidget::updateSettingsStatusForEndScan);

}

void ScanSettingsWidget::showTimeRow()
{
    int countRow = m_settingsFormLayout->rowCount();
    KyInfo() << "countRow = " << countRow;

    if (countRow == 12) {
        if (m_timeLabel) {
            m_timeLabel = new QLabel;
            setLabelAttributes(m_timeLabel, tr("Time"));
        }
        if (m_timeComboBox) {
            m_timeComboBox = new QComboBox;
            m_timeComboBox->setFixedSize(ScanSettingsWidgetComboboxSize);
            connect(m_timeComboBox, &QComboBox::currentTextChanged, this, &ScanSettingsWidget::timeCurrentTextChangedSlot);
        }
        m_settingsFormLayout->insertRow(4, m_timeLabel, m_timeComboBox);

        updateTimeSettings();
    }
}

void ScanSettingsWidget::hideTimeRow()
{
    int countRow = m_settingsFormLayout->rowCount();
    KyInfo() << "countRow = " << countRow;

    if (countRow == 13) {
        m_settingsFormLayout->removeRow(4);
    }
}

void ScanSettingsWidget::updateScanButtonSettings()
{
    bool saneStatus = g_sane_object->getSaneStatus();
    m_scanButton->setEnabled(saneStatus);
}

void ScanSettingsWidget::updateDeviceSettings()
{
    QStringList deviceStringList;

    bool saneStatus = g_sane_object->getSaneStatus();

    if (! saneStatus) {
        /// 1. Not find any scan device
        /// 2. This device open false

        deviceStringList = g_sane_object->getSaneNames();
        if (deviceStringList.count() > 1) {
            // Not only one scan device, just this scanner open false, we need set all sane names
        } else {
            deviceStringList << tr("No available scanners");
        }
    } else {
        deviceStringList = g_sane_object->getSaneNames();
    }

    m_deviceComboBox->setEnabled(true);

    KyInfo() << "Devices :  " << deviceStringList;
    setComboboxAttributes(m_deviceComboBox, deviceStringList);
}

/**
 * @brief ScanSettingsWidget::updatePageNumberSettings
 * Single page:
 *      user choose it, not get from scanners,
 *      only scan once.
 *
 * Multiple page:
 *      user choose it, not get from scanners
 *      scan lots of times, only stop by clicked scan button again or not pages in ADF scanners
 */
void ScanSettingsWidget::updatePageNumberSettings()
{
    QStringList pageNumberStringList;

    bool saneStatus = g_sane_object->getSaneStatus();

    pageNumberStringList << tr("Single") << tr("Multiple");

    KyInfo() << "PageNumber :  " << pageNumberStringList;

    m_pageNumberComboBox->setEnabled(saneStatus);
    setComboboxAttributes(m_pageNumberComboBox, pageNumberStringList);
}

void ScanSettingsWidget::updateTimeSettings()
{
    QStringList timerStringList;

    bool saneStatus = g_sane_object->getSaneStatus();

    timerStringList << tr("3s") << tr("5s") << tr("7s") << tr("10s") << tr("15s");

    KyInfo() << "Timer :  " << timerStringList;

    m_timeComboBox->setEnabled(saneStatus);
    setComboboxAttributes(m_timeComboBox, timerStringList);

}

void ScanSettingsWidget::updateTypeSettings()
{
    QStringList typeStringList;

    bool saneStatus = g_sane_object->getSaneStatus();


    if (! saneStatus) {
        typeStringList << tr("Flatbed") << tr("ADF");
    } else {
        typeStringList = g_sane_object->getSaneTypes();
    }

    KyInfo() << "Type :  " << typeStringList;

    m_typeComboBox->setEnabled(saneStatus);
    if (g_sane_object->haveSourceFlag == 0) {
        // For ESCL scanner device, which not get source type parameter
        //m_typeComboBox->setEnabled(false);
    }
    setComboboxAttributes(m_typeComboBox, typeStringList);
}

void ScanSettingsWidget::updateColorSettings()
{
    QStringList colorStringList;

    bool saneStatus = g_sane_object->getSaneStatus();

    if (! saneStatus) {
        colorStringList << tr("Gray") << tr("Color") << tr("Lineart");
    } else {
        colorStringList = g_sane_object->getSaneColors();
    }


    // Avoid segmentation fault
    if (! colorStringList.isEmpty()) {
        KyInfo() << "Color: " << colorStringList;
    }

    m_colorComboBox->setEnabled(saneStatus);
    setComboboxAttributes(m_colorComboBox, colorStringList);
}

void ScanSettingsWidget::updateResolutionSettings()
{
    QStringList resolutionStringList;

    bool saneStatus = g_sane_object->getSaneStatus();

    if (! saneStatus) {
        resolutionStringList << tr("75 dpi") << tr("100 dpi") << tr("150 dpi");
    } else {
        resolutionStringList = g_sane_object->getSaneResolutions();
    }

    if (! resolutionStringList.isEmpty()) {
        KyInfo() << "resolution: " << resolutionStringList;
    }

    m_resolutionComboBox->setEnabled(saneStatus);
    setComboboxAttributes(m_resolutionComboBox, resolutionStringList);
}

void ScanSettingsWidget::updateSizeSettings()
{
    QStringList sizeStringList;

    bool saneStatus = g_sane_object->getSaneStatus();

    if (! saneStatus) {
        sizeStringList << tr("A4") << tr("A5");
    } else {
        sizeStringList = g_sane_object->getSaneSizes();
    }

    m_sizeComboBox->setEnabled(saneStatus);
    setComboboxAttributes(m_sizeComboBox, sizeStringList);
}

/**
 * @brief ScanSettingsWidget::updateFormatSettings
 * format is choosed by user, so we need set it default.
 */
void ScanSettingsWidget::updateFormatSettings()
{
    QStringList formatStringList;

    bool saneStatus = g_sane_object->getSaneStatus();

    formatStringList << "jpg" << "png" << "pdf" << "bmp";

    m_formatComboBox->setEnabled(saneStatus);
    setComboboxAttributes(m_formatComboBox, formatStringList);

}

/**
 * @brief ScanSettingsWidget::updateSaveNameTextSettings
 * Consider multiple scan, so we set nameEdit timestamp.
 */
void ScanSettingsWidget::updateSaveNameTextSettings()
{
    bool saneStatus = g_sane_object->getSaneStatus();

    QDateTime currentTime = QDateTime::currentDateTime();
    KyInfo() << "current time: " <<currentTime;

    QString currentTimeString = currentTime.toString("yyyy_MM_dd-hh_mm_ss_zzz");
    KyInfo() << currentTimeString;

    m_saveNameEdit->setText(currentTimeString);
    m_saveNameEdit->setAlignment(Qt::AlignLeft);

    m_saveNameEdit->setEnabled(saneStatus);
}

void ScanSettingsWidget::updateSaveDirectorySettings()
{
    bool saneStatus = g_sane_object->getSaneStatus();
    m_saveDirectoryButton->setEnabled(saneStatus);
}

void ScanSettingsWidget::updateSendMailSettings()
{
    bool saneStatus = g_sane_object->getSaneStatus();
    m_sendMailButton->setEnabled(saneStatus);
}

void ScanSettingsWidget::updateSaveAsSettings()
{
    bool saneStatus = g_sane_object->getSaneStatus();
    m_SaveAsButton->setEnabled(saneStatus);
}

void ScanSettingsWidget::updateSaveAsTextStore()
{
    m_SaveAsButton->setText(tr("Store text"));

    KyInfo() << "m_SaveAsButton text: " << m_SaveAsButton->text();
}

void ScanSettingsWidget::updateSaveAsTextRecover()
{
    KyInfo() << "m_SaveAsButton text: " << m_SaveAsButton->text();
    m_SaveAsButton->setText(tr("Save as"));
}


/**
 * @brief ScanSettingsWidget::updateSettings
 * update scan settings while detect and open the default scanner successfully
 */
void ScanSettingsWidget::updateSettingsForDetectDevices()
{
    updateScanButtonSettings();
    updateDeviceSettings();
    updatePageNumberSettings();
    updateTimeSettings();
    updateTypeSettings();
    updateColorSettings();
    updateResolutionSettings();
    updateSizeSettings();
    updateFormatSettings();
    updateSaveNameTextSettings();
    updateSaveDirectorySettings();

    m_sendMailButton->setEnabled(false);
    m_SaveAsButton->setEnabled(false);
}

void ScanSettingsWidget::updateSettingsForSwitchDevices()
{
    updateScanButtonSettings();
    updatePageNumberSettings();
    updateTypeSettings();
    updateColorSettings();
    updateResolutionSettings();
    updateSizeSettings();
    updateFormatSettings();
    updateSaveNameTextSettings();
    updateSaveDirectorySettings();

//    updateSendMailSettings();
//    updateSaveAsSettings();

    m_sendMailButton->setEnabled(false);
    m_SaveAsButton->setEnabled(false);
}

void ScanSettingsWidget::updateSettingsStatusForStartScan()
{
    m_scanButton->setEnabled(false);
    m_deviceComboBox->setEnabled(false);
    m_pageNumberComboBox->setEnabled(false);


    int countRow = m_settingsFormLayout->rowCount();
    if (countRow == 13) {
        m_timeComboBox->setEnabled(false);
    }
    m_typeComboBox->setEnabled(false);
    m_colorComboBox->setEnabled(false);
    m_resolutionComboBox->setEnabled(false);
    m_sizeComboBox->setEnabled(false);
    m_formatComboBox->setEnabled(false);
    m_saveNameEdit->setEnabled(false);
    m_saveDirectoryButton->setEnabled(false);
    m_sendMailButton->setEnabled(false);
    m_SaveAsButton->setEnabled(false);
}

void ScanSettingsWidget::updateSettingsStatusForEndScan(int saneStatus)
{
    KyInfo() << "status: " << saneStatus;
    m_deviceComboBox->setEnabled(true);

    if (saneStatus == SANE_STATUS_NO_DOCS
            || saneStatus == SANE_STATUS_DEVICE_BUSY) {

        m_scanButton->setEnabled(true);
        m_pageNumberComboBox->setEnabled(true);
        int countRow = m_settingsFormLayout->rowCount();
        if (countRow == 13) {
            m_timeComboBox->setEnabled(true);
        }
        m_typeComboBox->setEnabled(true);
        m_colorComboBox->setEnabled(true);
        m_resolutionComboBox->setEnabled(true);
        m_sizeComboBox->setEnabled(true);
        m_formatComboBox->setEnabled(true);
        m_saveNameEdit->setEnabled(true);
        m_saveDirectoryButton->setEnabled(true);
        m_sendMailButton->setEnabled(true);
        m_SaveAsButton->setEnabled(true);

    } else if (saneStatus == SANE_STATUS_INVAL) {
        m_scanButton->setEnabled(true);
        m_pageNumberComboBox->setEnabled(true);

        int countRow = m_settingsFormLayout->rowCount();
        if (countRow == 13) {
            m_timeComboBox->setEnabled(true);
        }

        m_typeComboBox->setEnabled(true);
        m_colorComboBox->setEnabled(true);
        m_resolutionComboBox->setEnabled(true);
        m_sizeComboBox->setEnabled(true);
        m_formatComboBox->setEnabled(true);
        m_saveNameEdit->setEnabled(true);
        m_saveDirectoryButton->setEnabled(true);

        m_sendMailButton->setEnabled(false);
        m_SaveAsButton->setEnabled(false);
    } else {
        bool saneStatusBool = true;
        if (saneStatus == 0) {
            saneStatusBool = true;
        } else {
            saneStatusBool = false;
        }
        m_scanButton->setEnabled(saneStatusBool);
        m_pageNumberComboBox->setEnabled(saneStatusBool);

        int countRow = m_settingsFormLayout->rowCount();
        if (countRow == 13) {
            m_timeComboBox->setEnabled(saneStatusBool);
        }
        m_typeComboBox->setEnabled(saneStatusBool);
        m_colorComboBox->setEnabled(saneStatusBool);
        m_resolutionComboBox->setEnabled(saneStatusBool);
        m_sizeComboBox->setEnabled(saneStatusBool);
        m_formatComboBox->setEnabled(saneStatusBool);
        m_saveNameEdit->setEnabled(saneStatusBool);
        m_saveDirectoryButton->setEnabled(saneStatusBool);
        m_sendMailButton->setEnabled(saneStatusBool);
        m_SaveAsButton->setEnabled(saneStatusBool);
    }
}


void ScanSettingsWidget::setLabelAttributes(QLabel *label, const QString &text)
{
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    label->setFixedWidth(ScanSettingsLabelElideWidth);
    int elideWidth = ScanSettingsLabelElideWidth;

    QFontMetrics fontMetrics(label->font());

    if (fontMetrics.width(text) > label->width()) {
        QString elideText = QFontMetrics(label->font()).elidedText(text, Qt::ElideRight, elideWidth);
        label->setText(elideText);
        label->setToolTip(text);
    } else {
        label->setText(text);
        label->setToolTip("");
    }
}


void ScanSettingsWidget::setSaveButtonAttributes(QPushButton *button, const QString &text, int elideWidth)
{
    QFontMetrics fontMetrics(button->font());

    if (fontMetrics.width(text) > elideWidth) {
        QString elideText = QFontMetrics(button->font()).elidedText(text, Qt::ElideRight, elideWidth);
        button->setText(elideText);
        button->setToolTip(text);
    } else {
        button->setText(text);
        button->setToolTip("");
    }
}

void ScanSettingsWidget::setComboboxAttributes(QComboBox *combobox, QStringList strList)
{
    QListView *listView = new QListView();
    QStandardItemModel *model = new QStandardItemModel();
    int maxTooltipTextLength = 20;

    combobox->clear();

    for (int i = 0; i < strList.size(); ++i) {
        QStandardItem *item = new QStandardItem(strList.at(i));;
        int curTextLen = strList.at(i).length();

        if ( curTextLen >= maxTooltipTextLength) {
            item->setToolTip(strList.at(i));
        } else {
            item->setToolTip("");
        }

        model->appendRow(item);
    }
    combobox->setModel(model);
    combobox->setInsertPolicy(QComboBox::NoInsert);
    combobox->setFocusPolicy(Qt::ClickFocus);
    combobox->setView(listView);
}

void ScanSettingsWidget::setNameEditTooltip()
{
    int lenTextName = m_saveNameEdit->text ().length ();
    int lenImageFormat = 4;
    const int MaxLengthTextName = 256;

    if (lenTextName >= MaxLengthTextName - lenImageFormat) {
        m_saveNameEdit->setToolTip (tr("Scanning images's length cannot be large than 252"));
    } else {
        m_saveNameEdit->setToolTip ("");
    }
}

void ScanSettingsWidget::warnMsg(QString msg)
{
    QMessageBox *msgBox = new QMessageBox();

    msgBox->setText(msg);
    msgBox->setIcon(QMessageBox::Warning);
    msgBox->setWindowIcon(QIcon::fromTheme("kylin-scanner"));
    msgBox->setWindowTitle(tr("Scanner"));
    msgBox->setStandardButtons(QMessageBox::Yes);
    msgBox->setContextMenuPolicy(Qt::NoContextMenu);
    msgBox->button(QMessageBox::Yes)->setText(tr("Yes"));

    QWidget *widget = nullptr;
    QWidgetList widgetList = QApplication::allWidgets();
    for (int i=0; i<widgetList.length(); ++i) {
        if (widgetList.at(i)->objectName() == "MainWindow") {
            widget = widgetList.at(i);
        }
    }
    if (widget) {
        msgBox->setParent(widget);
        QRect rect = widget->geometry();
        int x = rect.x() + rect.width()/2 - msgBox->width()/2;
        int y = rect.y() + rect.height()/2 - msgBox->height()/2;
        msgBox->move(x,y);
    }

    msgBox->exec();
}
