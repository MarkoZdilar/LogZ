#include "customtextedit.h"
#include <QContextMenuEvent>
#include <QTextBlock>
#include <QKeyEvent>
#include <QTextDocumentFragment>

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

    // Determine the start and end of the current line
    cursor.movePosition(QTextCursor::StartOfLine);
    if (cursor.position() == 0) {
        cursor.endEditBlock();
        return; // Exit if the cursor is at the start of the document
    }

    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);
    QString currentLineHtml = cursor.selection().toHtml(); // Retrieve the HTML of the current line
    cursor.removeSelectedText();

    // If not at the beginning of the document
    if (cursor.position() != 0) {
        cursor.deletePreviousChar(); // Remove the newline character above the current line
        cursor.movePosition(QTextCursor::StartOfLine); // Go to the start of the line above
    }

    // Insert the deleted HTML above
    cursor.insertHtml(currentLineHtml);
    cursor.insertText("\n"); // Add a newline after the inserted line

    // Reset the cursor position to the start of the inserted line
    cursor.movePosition(QTextCursor::Up);
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    setTextCursor(cursor);

    cursor.endEditBlock();
}

void CustomTextEdit::moveLineDown() {
    QTextCursor cursor = textCursor();
    cursor.beginEditBlock();

    int originalPosition = cursor.positionInBlock(); // Store the cursor's position within the line

    // Determine the start and end of the current line
    cursor.movePosition(QTextCursor::StartOfLine);
    int currentLineStart = cursor.position();
    cursor.movePosition(QTextCursor::EndOfLine, QTextCursor::KeepAnchor);

    // Check if the current line is the last line of the document
    if (cursor.atEnd() && !cursor.movePosition(QTextCursor::Down)) {
        cursor.endEditBlock();
        return; // Exit if it's the last line, do nothing
    }

    // Retrieve the HTML of the current line and remove it
    QString currentLineHtml = cursor.selection().toHtml();
    cursor.removeSelectedText();
    if (!cursor.atEnd()) {
        cursor.deleteChar(); // Remove the newline character following the current line
    }

    // Move down to the next line
    cursor.movePosition(QTextCursor::Down);
    cursor.movePosition(QTextCursor::StartOfLine);
    int nextLineStart = cursor.position();

    // Insert the current line below
    cursor.insertHtml(currentLineHtml);
    if (!cursor.atEnd()) {
        cursor.insertText("\n"); // Ensure there's a newline after the inserted line
    }

    // Calculate the new cursor position based on the original position
    int newPosition = nextLineStart + originalPosition;
    cursor.setPosition(newPosition, QTextCursor::MoveAnchor);
    setTextCursor(cursor);

    cursor.endEditBlock();
}

