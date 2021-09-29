#ifndef FAILEDPAGEWIDGET_H
#define FAILEDPAGEWIDGET_H

#include "saneobject.h"

#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFont>
#include <QPalette>
#include <QThread>
#include<QSize>

#define FailedPageIconSize  QSize(101, 91)
#define FailedPageButtonSize  QSize(102, 36)


class FailedPageWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FailedPageWidget(QWidget *parent = nullptr);

    void setupGui();
    void initConnect();

signals:
    void scanButtonClicked();

private:
    QLabel *m_failedPageIcon;
    QLabel *m_failedPageText;
    QPushButton *m_failedPageButton;
    QVBoxLayout *m_failedPageVLayout;

};

#endif // FAILEDPAGEWIDGET_H
