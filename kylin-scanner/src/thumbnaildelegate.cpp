#include "thumbnaildelegate.h"

ThumbnailDelegate::ThumbnailDelegate(QWidget *parent) : QStyledItemDelegate(parent)
{

}

QSize ThumbnailDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    return QSize(40, 40);

}
