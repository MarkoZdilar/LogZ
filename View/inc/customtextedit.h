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
};

#endif // CUSTOMTEXTEDIT_H
