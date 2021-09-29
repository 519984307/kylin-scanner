#ifndef SHOWIMAGEWIDGET_H
#define SHOWIMAGEWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QMenu>
#include <QAction>

#define ShowImageWidgetMinimumSize QSize(387, 536)

class ShowImageWidget : public QLabel
{
    Q_OBJECT
public:
    explicit ShowImageWidget(QWidget *parent = nullptr);

    void setupGui();
    void initConnect();

public slots:

signals:

private:
//    QMenu *m_imageMenu;
//    QAction *m_imageSaveAction;


};

#endif // SHOWIMAGEWIDGET_H
