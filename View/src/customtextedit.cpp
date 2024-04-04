#include "customtextedit.h"
#include <QContextMenuEvent>
#include <QTextBlock>
#include <QKeyEvent>

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

void CustomTextEdit::mousePressEvent(QMouseEvent *event) {
    if ((event->modifiers() & Qt::ControlModifier) && event->button() == Qt::LeftButton) {
        QTextCursor cursor = cursorForPosition(event->pos());
        emit ctrlClickedForDeletion(cursor.position());
    } else {
        QTextEdit::mousePressEvent(event);
    }
}

void CustomTextEdit::keyPressEvent(QKeyEvent *event) {
    if (event->modifiers() & Qt::AltModifier) {
        switch (event->key()) {
        case Qt::Key_Up:
            moveLineUp();
            break;
        case Qt::Key_Down:
            moveLineDown();
            break;
        default:
            QTextEdit::keyPressEvent(event);
        }
    } else {
        QTextEdit::keyPressEvent(event);
    }
}

void CustomTextEdit::moveLineUp() {
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();

    // Check if the cursor is at the beginning of the text.
    cursor.movePosition(QTextCursor::StartOfLine);
    if (cursor.position() == 0) {
        cursor.endEditBlock();
        return; // Do nothing if the cursor is at the start of the document.
    }

    // Select the entire line including the newline character.
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    QString currentLineText = cursor.selectedText() + "\n";
    cursor.removeSelectedText();

    // Remove the newline character after the selected line if not at the end of the document.
    if (!cursor.atEnd()) {
        cursor.deleteChar();
    }

    // Move cursor up and insert the selected line.
    cursor.movePosition(QTextCursor::Up);
    int startOfPreviousLine = cursor.position();
    cursor.movePosition(QTextCursor::StartOfLine);
    cursor.insertText(currentLineText);

    // Set cursor position to the beginning of the inserted line.
    cursor.setPosition(startOfPreviousLine, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    setTextCursor(cursor);

    cursor.endEditBlock();
}

void CustomTextEdit::moveLineDown() {
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();

    // Check if the cursor is at the last line of the text.
    cursor.movePosition(QTextCursor::StartOfLine);
    int startOfLine = cursor.position();
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    if(cursor.position() == document()->characterCount() - 1) {
        cursor.endEditBlock();
        return; // Do nothing if the cursor is at the end of the document.
    }

    // Save and remove the current line text.
    QString currentLineText = cursor.selectedText();
    cursor.removeSelectedText();
    if(!cursor.atEnd()) {
        cursor.deleteChar(); // Remove the newline character after the current line.
    }

    // Move down and insert the selected line text.
    if (!cursor.movePosition(QTextCursor::Down)) {
        cursor.movePosition(QTextCursor::End);
        cursor.insertText("\n"); // Add a newline at the end if on the penultimate line.
    }

    cursor.movePosition(QTextCursor::StartOfLine);
    int newPos = cursor.position();
    cursor.insertText(currentLineText);
    if(cursor.position() < document()->characterCount() - 1) {
        cursor.insertText("\n"); // Add a newline after inserting the text if not at the end of the document.
    }

    // Update the cursor position to follow the moved line.
    cursor.setPosition(newPos + currentLineText.length(), QTextCursor::MoveAnchor);
    setTextCursor(cursor);

    cursor.endEditBlock();
}

