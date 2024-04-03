#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "clickabletextedit.h"
#include <QDateTime>
#include <QList>
#include <QTextLayout>
#include <QTextDocument>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct LogEntry {
    QDateTime dateTime;
    QString text;
    QList<QTextLayout::FormatRange> formats;

    // Constructor
    LogEntry(QDateTime dt, const QString& txt, QList<QTextLayout::FormatRange> fmts)
        : dateTime(dt), text(txt), formats(fmts) {}
};

/**
 * @brief The MainWindow class provides the main application window and manages user interactions.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a MainWindow object.
     * @param parent The parent widget, nullptr for a window without a parent.
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destroys the MainWindow object.
     */
    ~MainWindow();

private slots:
    /**
     * @brief Slot triggered by the Open action.
     *        Opens a file dialog, and displays the selected file.
     */
    void on_actionOpen_triggered();

    /**
     * @brief Slot triggered when a tree view item is clicked.
     *        Displays the content of the selected file.
     * @param index The model index of the clicked item.
     */
    void onTreeViewClicked(const QModelIndex &index);

    /**
     * @brief Slot triggered to close a file or a group.
     * @param index The model index of the item to close.
     */
    void onCloseFileRequested(const QModelIndex &index);

    /**
     * @brief Slot triggered to close a group.
     * @param groupIndex The model index of the group to close.
     */
    void onCloseGroupRequested(const QModelIndex &index);

    /**
     * @brief Handles the Ctrl+click action on a line of text in the primary text edit widget.
     *
     * This slot is triggered when the user Ctrl+clicks on a line of text in the primary text edit.
     * It appends the clicked line of text to the secondary text edit widget.
     * @param lineText The text of the clicked line to be added to the secondary text edit.
     */
    void onTextEditPrimaryCtrlClicked(const QString &lineText);

    /**
     * @brief Slot triggered to change the font size of the text edits and tree view.
     *        Presents a font dialog to the user and applies the chosen font.
     */
    void onChangeFontSizeTriggered();

    /**
     * @brief Changes the color of the selected text in textEditSecondary.
     */
    void changeTextColorInSecondary();

    /**
     * @brief Highlights the selected text in textEditSecondary.
     */
    void highlightTextInSecondary();

    /**
     * @brief Opens a previously saved editable log for further editing.
     */
    void on_openEditableLog_triggered();

    /**
     * @brief Saves the current state of the content in textEditSecondary to a file.
     */
    void on_save_triggered();

    /**
     * @brief Confirms the action to sort logs in ascending order.
     *
     * This method is a slot that gets triggered when the user selects
     * the option to sort logs in ascending order. It calls `sortLogs` with
     * the ascending parameter set to true. This method is typically connected
     * to a signal from a menu action or button click.
     */
    void sortAscendingConfirmed();

    /**
     * @brief Confirms the action to sort logs in descending order.
     *
     * This method is a slot that gets triggered when the user selects
     * the option to sort logs in descending order. It calls `sortLogs` with
     * the ascending parameter set to false. This method is typically connected
     * to a signal from a menu action or button click.
     */
    void sortDescendingConfirmed();

    /**
     * @brief Reverts the text in the secondary text edit widget to its state before sorting.
     *
     * This method restores the HTML content of the secondary text edit widget to
     * its previous state, before the last sorting operation was performed. This allows
     * the user to undo the sorting action and recover any formatting or highlighting
     * that was applied to the text. This method is typically connected to a signal
     * from an undo action in a menu.
     */
    void undoChanges();

    /**
     * @brief Deletes the line of text where the Ctrl+click event was triggered in the secondary text edit widget.
     *
     * This method removes a specific line of text in the secondary text edit widget, based on the position
     * provided as an argument. The position corresponds to the character index in the text document where
     * the Ctrl+click event occurred. This allows the user to directly delete lines of text by using a
     * Ctrl+click shortcut, enhancing the editing capabilities of the application.
     *
     * @param position The character index in the text document of the secondary text edit widget, indicating
     *                 where the Ctrl+click event was triggered.
     */
    void deleteSelectedLineInSecondary(int position);

private:
    Ui::MainWindow *ui; ///< Pointer to the UI elements.
    QStandardItemModel *model; ///< Model for managing tree view items.
    QString currentOpenFilePath; ///< Path of the currently open file.
    QList<LogEntry> logEntries;

    /**
     * @brief Opens and displays the content of the specified file.
     * @param filePath Path of the file to open and display.
     * @return True if successful, false otherwise.
     */
    bool openAndDisplayFile(const QString &filePath);

    /**
     * @brief Handles the creation of groups for files.
     * @param fileName Name of the file to be grouped.
     * @param filePath Path of the file to be grouped.
     * @return True if successful, false otherwise.
     */
    bool handleGrouping(const QString &fileName, const QString &filePath);

    /**
     * @brief Adds a file to a group, creating the group if necessary.
     * @param groupName Name of the group.
     * @param fileName Name of the file to add.
     * @param filePath Path of the file to add.
     */
    void addToGroup(const QString &groupName, const QString &fileName, const QString &filePath);

    /**
     * @brief Initializes the tree view with proper width and model.
     */
    void initializeTreeView();

    /**
     * @brief Initializes the fonts for textEdit widgets with monospace font.
     */
    void initializeFonts();

    /**
     * @brief Sets up the menu bar with File and View menus and associated actions.
     */
    void setupMenuBar();

    /**
     * @brief Sets up delegates for custom drawing in the tree view and connects signals to slots.
     */
    void setupDelegatesAndConnections();

    /**
     * @brief Sets up text formatting actions for textEditSecondary.
     *
     * This function adds actions for changing text color and highlighting text
     * to the context menu of textEditSecondary, enabling the user to modify
     * the appearance of selected text.
     */
    void setupTextFormattingActions();

    /**
     * @brief Sets up signal-slot connections specific to the secondary text edit widget.
     *
     * This method establishes connections between the secondary text edit widget's signals
     * and the corresponding slots within the MainWindow. It includes connecting the
     * ctrlClickedForDeletion signal to handle the deletion of selected lines through
     * a Ctrl+click interaction.
     */
    void setupSecondaryTextEditConnections();

    /**
     * @brief Sorts the logs in the secondary text edit widget.
     *
     * Sorts the logs displayed in the secondary text edit widget either in ascending
     * or descending order based on their timestamps. This sorting does not
     * take into account any formatting or highlighting previously applied to the text.
     *
     * @param ascending If true, logs are sorted in ascending order; if false, in descending order.
     */
    void sortLogs(bool ascending);

    /**
     * @brief Prompts the user for confirmation before sorting logs.
     *
     * Displays a confirmation dialog asking the user if they want to proceed with
     * sorting the logs. This is because sorting will remove all formatting and
     * highlighting from the text. If the user confirms, `sortLogs` is called
     * with the specified sorting order.
     *
     * @param ascending Specifies the sorting order. True for ascending, false for descending.
     */
    void promptForSortConfirmation(bool ascending);

    /**
     * @brief Holds the HTML content of the secondary text edit widget before any sort operation.
     *
     * This member stores the entire HTML content of the secondary text edit widget
     * right before a sort operation is performed. This allows for the content
     * to be restored if the user decides to undo the sort operation, preserving
     * all formatting and highlighting that was previously applied.
     */
    QString previousContent;

};

#endif // MAINWINDOW_H
