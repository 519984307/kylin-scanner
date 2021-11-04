#ifndef WATERMARKDIALOG_H
#define WATERMARKDIALOG_H

#include <QDialog>
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
#include <QStyleOption>
#include <QColor>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QString>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QProcess>
#include <QPixmap>
#include <QFileInfo>
#include <QFileIconProvider>

#include  <ukui-log4qt.h>

#include "utils/xatom-helper.h"
#include "globalsignal.h"
#include "include/common.h"
#include "include/theme.h"


class WatermarkDialog : public QDialog
{
    Q_OBJECT
public:
    explicit WatermarkDialog(QWidget *parent = nullptr);

    void initWindow();
    void initLayout();
    void initConnect();

    QString getLineEditText();

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;


public slots:
    void confirmButtonClickedSlot();

private:
    QLabel *m_titleLable;
    QPushButton *m_closeButton;
    QHBoxLayout *m_titleHBoxLayout;

    QLabel *m_infoTextLabel;
    QLineEdit *m_watermarkLineEdit;
    QHBoxLayout *m_watermarkHBoxLayout;

    QPushButton *m_cancelButton;
    QPushButton *m_confirmButton;
    QHBoxLayout *m_buttonsHBoxLayout;

    QVBoxLayout *m_mainVBoxLayout;

};

#endif // WATERMARKDIALOG_H
