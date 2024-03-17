#ifndef CLICKABLETEXTEDIT_H
#define CLICKABLETEXTEDIT_H

#include <QTextEdit>
#include <QMouseEvent>

/**
 * @class ClickableTextEdit
 * @brief The ClickableTextEdit class extends QTextEdit to provide clickable functionality with Ctrl key.
 *
 * Enables text lines within a QTextEdit widget to be clicked with the Ctrl key pressed,
 * emitting a signal with the text of the clicked line.
 */
class ClickableTextEdit : public QTextEdit {
    Q_OBJECT

public:
    /**
     * Inherits QTextEdit constructor.
     */
    using QTextEdit::QTextEdit;

protected:
    /**
     * Overrides mousePressEvent to handle Ctrl+click on text lines.
     * @param event Mouse event information.
     */
    void mousePressEvent(QMouseEvent *event) override;

signals:
    /**
     * Signal emitted when a line of text is Ctrl+clicked.
     * @param lineText The text of the clicked line.
     */
    void ctrlClicked(const QString &lineText);
};

#endif // CLICKABLETEXTEDIT_H
