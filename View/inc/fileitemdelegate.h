#ifndef FILEITEMDELEGATE_H
#define FILEITEMDELEGATE_H

#include <QStyledItemDelegate>
#include <QPainter>

/**
 * @brief The FileItemDelegate class provides custom drawing and event handling
 * for items within a view.
 *
 * This delegate is responsible for drawing a close button next to each item in the view
 * and handling the click events on these buttons to emit signals indicating that an item
 * should be closed (either a file or a group).
 */
class FileItemDelegate : public QStyledItemDelegate {
    Q_OBJECT

public:
    /**
     * @brief Constructs a FileItemDelegate with the specified parent.
     * @param parent The parent object.
     */
    explicit FileItemDelegate(QObject *parent = nullptr);

protected:
    /**
     * @brief Paints the delegate using the given painter and style option for the item
     * specified by index.
     *
     * @param painter QPainter to use for painting.
     * @param option The style options for the item.
     * @param index The index of the item in the model.
     */
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    /**
     * @brief Handles editor events, specifically to detect clicks on the close button.
     *
     * @param event The event to handle.
     * @param model The model used by the delegate.
     * @param option The style options for the item.
     * @param index The index of the item in the model.
     * @return True if the event was handled, false otherwise.
     */
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

signals:
    /**
     * @brief Emitted when the close button next to a file item is clicked.
     * @param index The index of the closed file item.
     */
    void closeFileRequested(const QModelIndex &index);

    /**
     * @brief Emitted when the close button next to a group item is clicked.
     * @param index The index of the closed group item.
     */
    void closeGroupRequested(const QModelIndex &index);
};

#endif // FILEITEMDELEGATE_H
