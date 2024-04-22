#ifndef ZIPVIEWERDIALOG_H
#define ZIPVIEWERDIALOG_H

#include <QDialog>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <KArchiveDirectory>

/**
 * @class ZipViewerDialog
 * @brief The ZipViewerDialog class provides a dialog window to view the contents of a ZIP archive.
 *
 * This dialog is designed to display the hierarchical structure of files and directories
 * within a ZIP archive using a QTreeWidget. It allows users to view and possibly select
 * files or directories within the archive for further processing.
 */
class ZipViewerDialog : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for ZipViewerDialog.
     * @param zipPath Path to the ZIP file to be opened and displayed.
     * @param parent Parent widget, typically the main window.
     */
    ZipViewerDialog(const QString &zipPath, QWidget *parent = nullptr);

    QStringList getSelectedFiles() const;

private slots:
    void accept() override;

private:
    QTreeWidget *treeWidget; ///< Tree widget to display the contents of the ZIP file.
    QStringList selectedFiles;

    /**
     * @brief Loads the contents of the ZIP file into the tree widget.
     * @param zipPath Path to the ZIP file.
     */
    void loadZipContents(const QString &zipPath);

    /**
     * @brief Recursively populates the tree widget with items from the KArchiveDirectory.
     * @param directory Pointer to the current directory in the archive being processed.
     * @param parentItem The parent tree widget item under which new items will be added.
     */
    void populateTreeWidget(const KArchiveDirectory* directory, QTreeWidgetItem* parentItem = nullptr);

    void collectSelectedFiles(QTreeWidgetItem* item);
};

#endif // ZIPVIEWERDIALOG_H
