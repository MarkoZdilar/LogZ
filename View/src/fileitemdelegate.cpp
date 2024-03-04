#include "fileitemdelegate.h"
#include <QMouseEvent>

FileItemDelegate::FileItemDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

void FileItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);

    //Drawing X on the right side
    painter->drawText(option.rect.adjusted(option.rect.width() - 20, 0, -5, 0), Qt::AlignRight | Qt::AlignVCenter, "x");
}

bool FileItemDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
    if (event->type() == QEvent::MouseButtonRelease) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QRect closeButtonRect = option.rect.adjusted(option.rect.width() - 20, 0, -5, 0);
        if (closeButtonRect.contains(mouseEvent->pos())) {
            if (index.parent().isValid()) {
                // If parent exists, it is file
                emit closeFileRequested(index);
            } else {
                // If parent don't exist, it's group.
                emit closeGroupRequested(index);
            }
            return true; // Event is done
        }
    }
    return QStyledItemDelegate::editorEvent(event, model, option, index);
}
