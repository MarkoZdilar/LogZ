#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>

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
     * @brief Updates the text view after an item is closed.
     * @param closedIndex Model index of the closed item.
     */
    void updateTextViewAfterClose(const QModelIndex &closedIndex);

    /**
     * @brief Finds the next valid index for selection after an item is closed.
     * @param closedIndex Model index of the closed item.
     * @return Model index of the next valid item.
     */
    QModelIndex findNextValidIndex(const QModelIndex &closedIndex);
};

#endif // MAINWINDOW_H
