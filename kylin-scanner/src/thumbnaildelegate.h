#ifndef THUMBNAILDELEGATE_H
#define THUMBNAILDELEGATE_H

#include <QWidget>
#include <QObject>
#include <QStyledItemDelegate>
#include <QModelIndex>
#include <QStyleOptionViewItem>
#include <QSize>

class ThumbnailDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ThumbnailDelegate(QWidget *parent = nullptr);

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

signals:

};

#endif // THUMBNAILDELEGATE_H
