#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include "clickabletextedit.h"

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

private:
    Ui::MainWindow *ui; ///< Pointer to the UI elements.
    QStandardItemModel *model; ///< Model for managing tree view items.
    QString currentOpenFilePath; ///< Path of the currently open file.

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
};

#endif // MAINWINDOW_H
