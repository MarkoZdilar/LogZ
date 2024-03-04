#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "fileitemdelegate.h"
#include <QFileDialog>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QInputDialog>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    int percentWidth = 40; //40% max tree view width
    int treeWidth = this->width() * percentWidth / 100;
    ui->treeView->setMaximumWidth(treeWidth);
    model = new QStandardItemModel(this);
    ui->treeView->setModel(model);

    // This delegate allows custom drawing and interaction within the tree view (Drawing of te "X" button).
    auto delegate = new FileItemDelegate(ui->treeView);
    ui->treeView->setItemDelegate(delegate);

    // Connecting signals to slots
    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::onTreeViewClicked);
    connect(delegate, &FileItemDelegate::closeFileRequested, this, &MainWindow::onCloseFileRequested);
    connect(delegate, &FileItemDelegate::closeGroupRequested, this, &MainWindow::onCloseGroupRequested);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_actionOpen_triggered() {
    QString filter = "Text files (*.txt);;Log files (*.log);;Net files (*.net);;All files (*.*)";
    QString filePath = QFileDialog::getOpenFileName(this, "Open File", "", filter);
    if (!filePath.isEmpty()) {
        openAndDisplayFile(filePath);
    }
}

bool MainWindow::openAndDisplayFile(const QString &filePath) {
    QString fileName = QFileInfo(filePath).fileName();

    //Display file in textEditPrimary after handleGrouping is done
    if (handleGrouping(fileName, filePath)) {
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            ui->textEditPrimary->setPlainText(in.readAll());
            currentOpenFilePath = filePath; // Remember the path of the currently opened file
            file.close();
            return true;
        }
    }
    return false;
}

bool MainWindow::handleGrouping(const QString &fileName, const QString &filePath) {
    QStringList groups;
    for (int i = 0; i < model->rowCount(); ++i) {
        QStandardItem *item = model->item(i);
        if (item) groups << item->text();
    }

    QString groupName;
    bool ok;
    do {
        groupName = QInputDialog::getItem(this, tr("Select or enter a group name"),
                                          tr("Group name:"), groups, 0, true, &ok);
        if (!ok) return false;
        if (groupName.isEmpty()) {
            QMessageBox::warning(this, tr("Group Name Required"), tr("You must enter a group name. Please try again."));
        }
    } while (groupName.isEmpty());

    addToGroup(groupName, fileName, filePath);
    return true;
}

void MainWindow::addToGroup(const QString &groupName, const QString &fileName, const QString &filePath) {
    QList<QStandardItem *> found = model->findItems(groupName, Qt::MatchExactly);
    QStandardItem *groupItem = found.isEmpty() ? new QStandardItem(groupName) : found.first();
    if (found.isEmpty()) {
        model->appendRow(groupItem);
        ui->treeView->expand(model->indexFromItem(groupItem)); // Automatically expand the new group
    }

    QStandardItem *fileItem = new QStandardItem(fileName);
    fileItem->setData(filePath, Qt::UserRole); // Store the full file path for later use within UserRole
    groupItem->appendRow(fileItem);
}

void MainWindow::onTreeViewClicked(const QModelIndex &index) {
    QString filePath = model->data(index, Qt::UserRole).toString();
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        ui->textEditPrimary->setPlainText(in.readAll());
        currentOpenFilePath = filePath; // Update the path of the currently opened file
        file.close();
    }
}

void MainWindow::onCloseFileRequested(const QModelIndex &index) {
    if (!index.isValid()) return;

    QString filePath = model->data(index, Qt::UserRole).toString();
    bool clearTextView = filePath == currentOpenFilePath;

    QStandardItem *parentItem = index.parent().isValid() ? model->itemFromIndex(index.parent()) : nullptr;
    if (parentItem) {
        parentItem->removeRow(index.row());
        if (parentItem->rowCount() == 0) {
            // If this is the last file in the group, close the whole group
            int groupRow = model->indexFromItem(parentItem).row();
            model->removeRow(groupRow);
            clearTextView = true;
        }
    } else {
        // If there is no parent (meaning it's group), close the whole group
        model->removeRow(index.row());
    }

    if (model->rowCount() == 0 || clearTextView) {
        ui->textEditPrimary->clear(); // Clear text view if there are no more items left or the opened file was closed
        currentOpenFilePath.clear();
    }
}

void MainWindow::onCloseGroupRequested(const QModelIndex &groupIndex) {
    if (!groupIndex.isValid()) return;

    auto groupItem = model->itemFromIndex(groupIndex);
    for (int i = 0; i < groupItem->rowCount(); ++i) {
        auto fileItem = groupItem->child(i);
        if (fileItem->data(Qt::UserRole).toString() == currentOpenFilePath) {
            ui->textEditPrimary->clear(); // Clear text view if the currently opened file is within the closing group
            currentOpenFilePath.clear();
            break;
        }
    }

    model->removeRow(groupIndex.row()); // Remove the group and its children

    if (model->rowCount() == 0) {
        ui->textEditPrimary->clear(); // Clear text view if there are no more items left in the whole model
    }
}
