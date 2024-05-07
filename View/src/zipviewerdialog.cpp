#include "zipviewerdialog.h"
#include <KZip>
#include <KArchiveEntry>
#include <KArchiveDirectory>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QMessageBox>

ZipViewerDialog::ZipViewerDialog(const QString &zipPath, QWidget *parent)
    : QDialog(parent), treeWidget(new QTreeWidget(this)) {
    setWindowTitle("ZIP Content Viewer");
    treeWidget->setHeaderLabel("Files and Directories");
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(treeWidget);

    // Dialog buttons for confirmation and cancellation.
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &ZipViewerDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &ZipViewerDialog::reject);

    // Load the contents of the ZIP file.
    loadZipContents(zipPath);
}

void ZipViewerDialog::loadZipContents(const QString &zipPath) {
    KZip archive(zipPath);
    if (archive.open(QIODevice::ReadOnly)) {
        const KArchiveDirectory* rootDir = archive.directory();
        populateTreeWidget(rootDir, nullptr);  // Populate the tree widget with the contents.
        archive.close();
    } else {
        QMessageBox::critical(this, "Error", "Unable to open ZIP file.");
    }
}

void ZipViewerDialog::populateTreeWidget(const KArchiveDirectory* directory, QTreeWidgetItem* parentItem) {
    QStringList entries = directory->entries();
    foreach (const QString &entry, entries) {
        const KArchiveEntry *e = directory->entry(entry);
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, e->name());
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // Enables checkbox.
        item->setCheckState(0, Qt::Unchecked); // Sets checkbox to unchecked by default.

        if (e->isDirectory()) {
            item->setIcon(0, QIcon(":/icons/folder.png"));  // Assuming an icon for directories is available.
        } else {
            item->setIcon(0, QIcon(":/icons/file.png"));  // Assuming an icon for files is available.
        }
        if (parentItem) {
            parentItem->addChild(item);
        } else {
            treeWidget->addTopLevelItem(item);
        }
        if (e->isDirectory()) {
            populateTreeWidget(static_cast<const KArchiveDirectory *>(e), item);
        }
    }
}

void ZipViewerDialog::accept() {
    selectedFiles.clear();
    collectSelectedFiles(treeWidget->invisibleRootItem()); // Start collecting selected files.
    QDialog::accept();
}

void ZipViewerDialog::collectSelectedFiles(QTreeWidgetItem* item) {
    if (item->checkState(0) == Qt::Checked) {
        selectedFiles.append(item->text(0)); // Adds the file path to the list.
    }
    for (int i = 0; i < item->childCount(); ++i) {
        collectSelectedFiles(item->child(i)); // Recursively collect selected files.
    }
}

QStringList ZipViewerDialog::getSelectedFiles() const {
    return selectedFiles;
}
