#ifndef CUSTOMTEXTEDIT_H
#define CUSTOMTEXTEDIT_H

#include <QTextEdit>
#include <QMenu>
#include <QAction>
#include <QColorDialog>
#include <QTextCursor>

class CustomTextEdit : public QTextEdit {
    Q_OBJECT

public:
    /**
     * @brief Constructs a custom QTextEdit with added functionalities.
     *
     * @param parent The parent widget, nullptr if it has no parent.
     */
    explicit CustomTextEdit(QWidget *parent = nullptr);

protected:
    /**
     * @brief Overrides the context menu event to add custom actions.
     *
     * This method creates a custom context menu by adding actions for changing text color,
     * highlighting text, and applying these changes to all occurrences of the selected text.
     *
     * @param event The context menu event.
     */
    void contextMenuEvent(QContextMenuEvent *event) override;

    /**
     * @brief Handles mouse press events, including custom actions for Ctrl+click.
     *
     * Overrides the default mouse press event to emit a signal when a Ctrl+click
     * is detected. This signal is connected to custom slot function designed
     * to handle specific actions, such as deleting the line where the click occurred.
     * The function falls back to the standard QTextEdit mouse press event handling
     * for all other types of clicks.
     *
     * @param event The mouse event information.
     */
    void mousePressEvent(QMouseEvent *event) override;

    /**
     * @brief Handles key press events for custom text manipulation shortcuts.
     *
     * This method intercepts specific key press events to implement custom keyboard
     * shortcuts for text manipulation, such as moving lines up or down within the document.
     * It checks for combinations of the Alt key and arrow keys to trigger these actions,
     * while other key events are passed on to the QTextEdit's default handler.
     *
     * @param event The key event information.
     */
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    /**
     * @brief Applies a selected text color to the current selection.
     *
     * This slot is triggered by a context menu action. It opens a color dialog and applies
     * the selected color to the current text selection.
     */
    void applyTextColor();

    /**
     * @brief Applies a selected highlight color to the current selection.
     *
     * This slot is triggered by a context menu action. It opens a color dialog and applies
     * the selected highlight color to the current text selection.
     */
    void applyHighlightColor();

    /**
     * @brief Applies a selected text color to all occurrences of the selected text.
     *
     * This slot is triggered by a context menu action. It opens a color dialog and applies
     * the selected color to all occurrences of the text currently selected in the document.
     */
    void applyTextColorToAllOccurrences();

    /**
     * @brief Applies a selected highlight color to all occurrences of the selected text.
     *
     * This slot is triggered by a context menu action. It opens a color dialog and applies
     * the selected highlight color to all occurrences of the text currently selected in the document.
     */
    void applyHighlightColorToAllOccurrences();

private:
    /**
     * @brief Sets up the custom context menu for the text edit widget.
     *
     * This method adds custom actions to the context menu for text color change and text highlighting,
     * including options to apply these changes to all occurrences of the selected text.
     */
    void setupContextMenu();

    /**
     * @brief Applies text formatting to the selected text or all its occurrences.
     *
     * Formats the currently selected text or all its occurrences in the document based on the provided
     * text character format. This can include changes in text color, background color, font, etc.
     *
     * @param format The text character format to apply.
     * @param allOccurrences Whether to apply the format to all occurrences of the selected text.
     */
    void formatText(const QTextCharFormat &format, bool allOccurrences = false);

    /**
     * @brief Moves the current line one position down in the document.
     *
     * This function shifts the line where the cursor currently resides one line down,
     * effectively swapping positions with the line immediately below it. The function
     * ensures that the cursor follows the moved line to maintain context for the user.
     * If the cursor is on the last line of the document, no action is taken.
     */
    void moveLineDown();

    /**
     * @brief Moves the current line one position up in the document.
     *
     * This function shifts the line where the cursor currently resides one line up,
     * effectively swapping positions with the line immediately above it. The function
     * ensures that the cursor follows the moved line to maintain context for the user.
     * If the cursor is on the first line of the document, no action is taken.
     */
    void moveLineUp();

signals:
    /**
     * @brief Signal emitted when a line of text is Ctrl+clicked in the CustomTextEdit widget.
     *
     * This signal is used to indicate that the user has performed a Ctrl+click action at a specific position
     * within the text document of the CustomTextEdit widget. It is primarily used to trigger text manipulation
     * actions, such as deleting the line of text at the position of the Ctrl+click. The position is passed as
     * an argument to the slot connected to this signal, enabling direct access to the clicked line of text.
     *
     * @param position The character index in the text document where the Ctrl+click event was triggered.
     */
    void ctrlClickedForDeletion(int position);

};

#endif // CUSTOMTEXTEDIT_H
