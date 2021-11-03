#ifndef SCANDIALOG_H
#define SCANDIALOG_H

#include <QWidget>
#include <QDialog>
#include <QSize>
#include <QPushButton>
#include <QLabel>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QVariant>

#define ScanDialogWindowSize QSize(380, 164)
#define ScanDialogIconSize QSize(24, 24)
#define ScanDialogLabelHeight 24
#define ScanDialogButtonSize QSize(96, 36)

class ScanDialog : public QDialog
{
    Q_OBJECT
public:
    explicit ScanDialog(QWidget *parent = nullptr);

    void setupGui();
    void initConnect();

    int pageNumber() const;
    void setPageNumber(int pageNumber);

    QString scanMsg() const;
    void setScanMsg(const QString &scanMsg);

signals:

public slots:
    void updatePageNumberWhileScanning();
    void updatePageNumberWhileStopScanning();

private:
    int m_pageNumber = 1;
    QString m_scanMsg;

    QLabel *m_titleLabel;
    QPushButton *m_closeButton;
    QLabel *m_iconLabel;
    QLabel *m_msgLabel;
    QPushButton *m_cancelButton;

    QHBoxLayout *m_titleHBoxLayout;
    QHBoxLayout *m_msgHBoxLayout;
    QHBoxLayout *m_cancelHBoxLayout;
    QVBoxLayout *m_mainVBoxLayout;


};

#endif // SCANDIALOG_H
