#include "customtextedit.h"
#include <QContextMenuEvent>

CustomTextEdit::CustomTextEdit(QWidget *parent) : QTextEdit(parent) {
}

void CustomTextEdit::contextMenuEvent(QContextMenuEvent *event) {
    QMenu *contextMenu = createStandardContextMenu();
    contextMenu->addSeparator();

    QAction *textColorAction = contextMenu->addAction("Change Text Color");
    connect(textColorAction, &QAction::triggered, this, &CustomTextEdit::applyTextColor);

    QAction *highlightAction = contextMenu->addAction("Highlight Text");
    connect(highlightAction, &QAction::triggered, this, &CustomTextEdit::applyHighlightColor);

    QAction *textColorAllAction = contextMenu->addAction("Change Text Color (All Occurrences)");
    connect(textColorAllAction, &QAction::triggered, this, &CustomTextEdit::applyTextColorToAllOccurrences);

    QAction *highlightAllAction = contextMenu->addAction("Highlight Text (All Occurrences)");
    connect(highlightAllAction, &QAction::triggered, this, &CustomTextEdit::applyHighlightColorToAllOccurrences);

    contextMenu->exec(event->globalPos());
    delete contextMenu;
}

void CustomTextEdit::applyTextColor() {
    QColor color = QColorDialog::getColor(Qt::black, this, "Choose Text Color");
    if (color.isValid()) {
        QTextCharFormat fmt;
        fmt.setForeground(color);
        formatText(fmt);
    }
}

void CustomTextEdit::applyHighlightColor() {
    QColor color = QColorDialog::getColor(Qt::yellow, this, "Choose Highlight Color");
    if (color.isValid()) {
        QTextCharFormat fmt;
        fmt.setBackground(color);
        formatText(fmt);
    }
}

void CustomTextEdit::applyTextColorToAllOccurrences() {
    QColor color = QColorDialog::getColor(Qt::black, this, "Choose Text Color for All Occurrences");
    if (color.isValid()) {
        QTextCharFormat fmt;
        fmt.setForeground(color);
        formatText(fmt, true);
    }
}

void CustomTextEdit::applyHighlightColorToAllOccurrences() {
    QColor color = QColorDialog::getColor(Qt::yellow, this, "Choose Highlight Color for All Occurrences");
    if (color.isValid()) {
        QTextCharFormat fmt;
        fmt.setBackground(color);
        formatText(fmt, true);
    }
}

void CustomTextEdit::formatText(const QTextCharFormat &format, bool allOccurrences) {
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection() && !allOccurrences) {
        cursor.select(QTextCursor::WordUnderCursor);
    }

    if (allOccurrences) {
        QString selectedText = cursor.selectedText();
        QTextCursor highlightCursor(document());
        QTextCursor backupCursor = cursor;
        while (!highlightCursor.isNull() && !highlightCursor.atEnd()) {
            highlightCursor = document()->find(selectedText, highlightCursor);
            if (!highlightCursor.isNull()) {
                highlightCursor.mergeCharFormat(format);
            }
        }
        setTextCursor(backupCursor);
    } else {
        cursor.mergeCharFormat(format);
    }
}
