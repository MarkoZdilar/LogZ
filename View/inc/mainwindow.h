#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "clickabletextedit.h"
#include "finddialog.h"
#include "LogManager.h"
#include "GroupManager.h"
#include <QDateTime>
#include <QList>
#include <QTextLayout>
#include <QTextDocument>
#include <KArchive>
#include <KZip>
#include <KTar>
#include <QStandardPaths>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

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
     * @brief Triggers the file open dialog allowing the user to select multiple files.
     *
     * This slot is connected to an action in the File menu. When triggered, it opens
     * a file dialog that allows users to select multiple files to open. After selecting
     * the files, users are prompted to enter or select a group name under which the
     * files will be grouped in the tree view.
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

    /**
     * @brief Finds and selects the next occurrence of the text in the primary text edit widget.
     *
     * This method searches for the next occurrence of the specified text in the primary text edit widget,
     * starting from the current cursor position. If the text is found, it selects the text and updates
     * the cursor position to the end of the found text.
     *
     * @param text The text string to search for in the primary text edit widget.
     */
    void findNext(const QString &text);

    /**
     * @brief Finds and selects the previous occurrence of the text in the primary text edit widget.
     *
     * This method searches for the previous occurrence of the specified text in the primary text edit widget,
     * starting from the current cursor position and searching backwards. If the text is found, it selects
     * the text and updates the cursor position to the beginning of the found text.
     *
     * @param text The text string to search for in the primary text edit widget.
     */
    void findPrevious(const QString &text);

    /**
     * @brief Updates the case sensitivity setting for text search operations.
     *
     * This method sets the case sensitivity for the text search operations in the application.
     * When enabled, the search operations will distinguish between upper and lower case letters.
     *
     * @param enabled True to enable case sensitivity, false to disable it.
     */
    void updateCaseSensitivity(bool enabled);

    /**
     * @brief Toggles the display between user content and find results in the secondary text edit widget.
     *
     * This method switches the content displayed in the secondary text edit widget between the user's
     * original content and the results of a find operation. It also adjusts the editability of the widget
     * based on the type of content being displayed.
     */
    void toggleFindResults();

    /**
     * @brief Searches for all occurrences of the specified text in the primary text edit widget and displays the results.
     *
     * This method finds all occurrences of the specified text within the primary text edit widget. It formats
     * each found line with its line number and the matched text, and displays these results in the secondary
     * text edit widget. If no matches are found, it informs the user via a message box.
     *
     * @param text The text string to search for in the primary text edit widget.
     */
    void findAllInDocument(const QString &text);

    /**
     * @brief Sets the dark theme for the application.
     *
     * This function applies a dark color scheme to the main window
     * and its components. It sets the background to dark gray and
     * the text color to white to ensure readability.
     */
    void setDarkTheme();

    /**
     * @brief Sets the blue theme for the application.
     *
     * This function applies a blue color scheme to the main window
     * and its components. It uses a deep blue color for the background
     * and a light color for the text, providing a high contrast
     * and a professional look.
     */
    void setBlueTheme();

    /**
     * @brief Sets the light theme for the application.
     *
     * This function applies a light color scheme to the main window
     * and its components. It sets the background to white and the
     * text color to black, which is ideal for brightly lit environments
     * or users who prefer a more traditional look.
     */
    void setLightTheme();

    /**
     * @brief Displays an error message in a message box.
     *
     * This slot is connected to the errorOccurred signal from the LogManager.
     * When an error occurs, this slot is triggered and displays the error using a QMessageBox.
     *
     * @param error The error message to display.
     */
    void displayError(const QString &error);

    /**
     * @brief Sorts the logs in the secondary text editor.
     *
     * @param ascending Specifies the sorting order. If true, logs are sorted in ascending order; if false, in descending order.
     */
    void sortLogs(bool ascending);

private:
    Ui::MainWindow *ui; ///< Pointer to the UI elements.
    QStandardItemModel *model; ///< Model for managing tree view items.
    QString currentOpenFilePath; ///< Path of the currently open file.
    QList<LogEntry> logEntries; ///< List of log entries displayed in the application.
    FindDialog *findDialog; ///< Pointer to the find dialog used for text searches.
    bool caseSensitiveSearch = false; ///< Indicates if the search should be case-sensitive.
    QString findResults; ///< Stores the search results formatted as HTML.
    QString userContent; ///< Stores the user content displayed in the secondary text editor when not showing find results.
    QAction *toggleViewAction; ///< Action associated with toggling between user content and find results.
    bool isFindResultsDisplayed = false; ///< Flag to indicate if the find results are currently displayed in the secondary text editor.
    LogManager* logManager; ///< Pointer to an instance of LogManager which handles the logic for managing log files.
    GroupManager* groupManager; ///< Pointer to an instance of GroupManager which manages group creation and color settings for the groups.

    /**
     * @brief Prompts the user to enter or select a group name.
     * @return The name of the group if provided; an empty string otherwise.
     *
     * This function displays a dialog box prompting the user to either select an existing
     * group or enter a new group name. If the user provides a valid name and confirms the dialog,
     * the name is returned. If the user cancels the dialog or enters an invalid name, an empty
     * string is returned.
     */
    QString promptForGroupName();

    /**
     * @brief Handles the creation of groups for files.
     * @param fileName Name of the file to be grouped.
     * @param filePath Path of the file to be grouped.
     * @return True if successful, false otherwise.
     */
    bool handleGrouping(const QString &fileName, const QString &filePath);

    /**
     * @brief Adds a file to a specified group in the tree view.
     * @param groupName The name of the group under which the file will be added.
     * @param fileName The name of the file to be added.
     * @param filePath The path of the file to be added.
     *
     * Adds a file to an existing or new group in the tree view. If the group does not exist,
     * a new group item is created. The file is then added to this group. Each file in the
     * group is represented as a child node under the group node in the tree view.
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
     * @brief Sets up the find dialog and associated actions.
     *
     * This function initializes the find dialog, configures its visibility, and connects
     * the necessary signals and slots for the search functionality within the application.
     * It also sets up the shortcut and action for triggering the find dialog.
     */
    void setupFindDialog();

    /**
     * @brief Sets up the item delegate for styling tree view items.
     */
    void setupItemDelegate();

    /**
     * @brief Sets up the connections between signals and slot of LogManager and GroupManager.
     */
    void setupGroupLogConnections();

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

    /**
     * @brief Configures the appearance of the primary text edit widget.
     *
     * This method sets up the visual styling for the primary text edit widget in the application.
     * It applies a custom style sheet to change the appearance of selected text, setting the background
     * color to green and the text color to white. This enhances the visibility of selected text,
     * making it easier for users to identify their selections within the text.
     */
    void setupTextEdit();

    /**
     * @brief Decompresses and processes files within an archive file.
     *
     * This method opens an archive file, decompresses its contents, and
     * optionally saves them to a specified output directory or directly uses them.
     *
     * @param filePath Path to the archive file.
     * @param outputPath Directory where the decompressed content should be saved.
     */
    void processArchiveFile(const QString &filePath, const QString &outputPath);
};

#endif // MAINWINDOW_H
