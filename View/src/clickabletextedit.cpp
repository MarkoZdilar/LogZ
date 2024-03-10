#include "clickabletextedit.h"
#include <QTextBlock>

void ClickableTextEdit::mousePressEvent(QMouseEvent *event)
{
    if ((event->modifiers() & Qt::ControlModifier) && event->button() == Qt::LeftButton) {
        QTextCursor cursor = cursorForPosition(event->pos());
        emit ctrlClicked(cursor.block().text());
    } else {
        QTextEdit::mousePressEvent(event);
    }
}
