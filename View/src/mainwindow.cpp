#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "fileitemdelegate.h"
#include "zipviewerdialog.h"
#include <QFileDialog>
#include <QFile>
#include <QIODevice>
#include <QTextStream>
#include <QInputDialog>
#include <QMessageBox>
#include <QFontDialog>
#include <QColorDialog>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextBlock>
#include <QTextCharFormat>
#include <utility>
#include <algorithm>
#include <KArchive>
#include <KZip>
#include <KTar>
#include <QStandardPaths>
#include <QTextDocumentFragment>

// Constructor initialization
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    initializeTreeView();
    initializeFonts();
    setupMenuBar();
    setupDelegatesAndConnections();
    setupTextFormattingActions();
    setupSecondaryTextEditConnections();
    setupTextEdit();
    setupFindDialog();
}

// Destructor
MainWindow::~MainWindow() {
    delete ui;
}

// Initialize the tree view with proper width
void MainWindow::initializeTreeView() {
    int percentWidth = 40;
    int treeWidth = this->width() * percentWidth / 100;
    ui->treeView->setMaximumWidth(treeWidth);
    model = new QStandardItemModel(this);
    ui->treeView->setModel(model);
}

// Initialize font settings for text edits
void MainWindow::initializeFonts() {
    QFont monospaceFont("Courier New", 14);
    ui->textEditPrimary->setFont(monospaceFont);
    ui->textEditSecondary->setFont(monospaceFont);
}

// Setup menu bar and actions
void MainWindow::setupMenuBar() {
    // Access the existing menu bar or create a new one if none exists
    QMenuBar *menuBar = this->menuBar();

    // File Menu
    QMenu *fileMenu = menuBar->addMenu(tr("&File"));
    QAction *openLogsAction = new QAction(tr("&Open Logs"), this);
    fileMenu->addAction(openLogsAction);
    connect(openLogsAction, &QAction::triggered, this, &MainWindow::on_actionOpen_triggered);

    QAction *openEditableLogAction = new QAction(tr("Open &Editable Log"), this);
    fileMenu->addAction(openEditableLogAction);
    connect(openEditableLogAction, &QAction::triggered, this, &MainWindow::on_openEditableLog_triggered);

    QAction *saveAction = new QAction(tr("&Save"), this);
    saveAction->setShortcut(QKeySequence::Save);
    fileMenu->addAction(saveAction);
    connect(saveAction, &QAction::triggered, this, &MainWindow::on_save_triggered);

    QAction *undoAction = fileMenu->addAction(tr("&Undo"));
    connect(undoAction, &QAction::triggered, this, &MainWindow::undoChanges);

    // View Menu
    QMenu *viewMenu = menuBar->addMenu(tr("&View"));
    QAction *changeFontSizeAction = new QAction(tr("&Change Font Size"), this);
    viewMenu->addAction(changeFontSizeAction);
    connect(changeFontSizeAction, &QAction::triggered, this, &MainWindow::onChangeFontSizeTriggered);

    QAction *toggleViewAction = new QAction(tr("Toggle Find Results"), this);
    toggleViewAction->setCheckable(true);
    connect(toggleViewAction, &QAction::toggled, this, &MainWindow::toggleFindResults);
    viewMenu->addAction(toggleViewAction);

    // Themes
    QMenu *themeMenu = viewMenu->addMenu(tr("&Themes"));
    QAction *darkThemeAction = new QAction(tr("&Dark Theme"), this);
    QAction *blueThemeAction = new QAction(tr("&Blue Theme"), this);
    QAction *lightThemeAction = new QAction(tr("&Light Theme"), this);

    themeMenu->addAction(darkThemeAction);
    themeMenu->addAction(blueThemeAction);
    themeMenu->addAction(lightThemeAction);

    connect(darkThemeAction, &QAction::triggered, this, &MainWindow::setDarkTheme);
    connect(blueThemeAction, &QAction::triggered, this, &MainWindow::setBlueTheme);
    connect(lightThemeAction, &QAction::triggered, this, &MainWindow::setLightTheme);

    // Sort Menu
    QMenu *sortMenu = menuBar->addMenu(tr("&Sort"));
    QAction *sortAscendingAction = new QAction(tr("Sort Ascending"), this);
    QAction *sortDescendingAction = new QAction(tr("Sort Descending"), this);
    sortMenu->addAction(sortAscendingAction);
    sortMenu->addAction(sortDescendingAction);
    connect(sortAscendingAction, &QAction::triggered, [this]() { promptForSortConfirmation(true); });
    connect(sortDescendingAction, &QAction::triggered, [this]() { promptForSortConfirmation(false); });
}

// Setup delegates for custom drawing and connections for signals and slots
void MainWindow::setupDelegatesAndConnections() {
    auto delegate = new FileItemDelegate(ui->treeView);
    ui->treeView->setItemDelegate(delegate);

    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::onTreeViewClicked);
    connect(delegate, &FileItemDelegate::closeFileRequested, this, &MainWindow::onCloseFileRequested);
    connect(delegate, &FileItemDelegate::closeGroupRequested, this, &MainWindow::onCloseGroupRequested);
    connect(ui->textEditPrimary, &ClickableTextEdit::ctrlClicked, this, &MainWindow::onTextEditPrimaryCtrlClicked);
}

void MainWindow::setupFindDialog() {
    findDialog = new FindDialog(this);
    findDialog->hide(); // Hide dialog after initialization

    connect(findDialog, &FindDialog::findNext, this, &MainWindow::findNext);
    connect(findDialog, &FindDialog::findPrevious, this, &MainWindow::findPrevious);
    connect(findDialog, &FindDialog::caseSensitivityChanged, this, &MainWindow::updateCaseSensitivity);
    connect(findDialog, &FindDialog::findAll, this, &MainWindow::findAllInDocument);

    // Action to open dialog on Ctrl+F
    QAction *openFindDialogAction = new QAction(this);
    openFindDialogAction->setShortcut(QKeySequence("Ctrl+F"));
    connect(openFindDialogAction, &QAction::triggered, findDialog, &QWidget::show);
    addAction(openFindDialogAction);
}

void MainWindow::on_actionOpen_triggered() {
    // Filter for file dialog to restrict files that user can open.
    QString filter = "All supported files (*.txt *.log *.zip);;Text files (*.txt);;Log files (*.log);;ZIP archives (*.zip);;All files (*.*)";
    QStringList filePaths = QFileDialog::getOpenFileNames(this, tr("Open Files"), "", filter);

    // Check if at least one file has been selected.
    if (!filePaths.isEmpty()) {
        // Prompt user to enter or select a group for files.
        QString groupName = promptForGroupName();
        if (!groupName.isEmpty()) {
            // Process each selected file.
            for (const QString &filePath : filePaths) {
                // If it is a ZIP file, show the ZIP Viewer Dialog.
                if (filePath.endsWith(".zip")) {
                    ZipViewerDialog viewer(filePath, this);
                    // Execute the dialog and check if the user accepted (clicked OK).
                    if (viewer.exec() == QDialog::Accepted) {
                        // Extract the selected files from the ZIP.
                        QStringList filesToAdd = viewer.getSelectedFiles();
                        for (const QString &fileInsideZip : filesToAdd) {
                            // Extract and temporarily store the selected file from the ZIP.
                            QString tempPath = extractFileFromZip(filePath, fileInsideZip);
                            // If the file was successfully extracted, add it to the group.
                            if (!tempPath.isEmpty()) {
                                addToGroup(groupName, QFileInfo(fileInsideZip).fileName(), tempPath);
                            }
                        }
                    }
                } else {
                    // For other file types, open and display them as usual.
                    openAndDisplayFile(filePath, groupName);
                }
            }
        }
    }
}

QString MainWindow::extractFileFromZip(const QString &zipFilePath, const QString &fileInsideZip) {
    // Open the ZIP file using KZip.
    KZip archive(zipFilePath);
    if (!archive.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to open ZIP file."));
        return QString();
    }

    // Access the root directory of the ZIP archive.
    const KArchiveDirectory *rootDir = archive.directory();
    // Find the requested file in the ZIP archive.
    const KArchiveEntry *entry = rootDir->entry(fileInsideZip);
    if (!entry || !entry->isFile()) {
        QMessageBox::warning(this, tr("Error"), tr("The specified file does not exist within the ZIP archive."));
        return QString();
    }

    // Cast the entry to a file (since we know it is a file at this point).
    const KArchiveFile *file = static_cast<const KArchiveFile *>(entry);
    // Create a temporary path where the file's contents will be written.
    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/" + fileInsideZip;

    // Open or create the temporary file.
    QFile tempFile(tempPath);
    if (!tempFile.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Error"), tr("Unable to create temporary file for ZIP contents."));
        return QString();
    }

    // Write the file contents to the temporary file.
    tempFile.write(file->data());
    tempFile.close();
    // Close the archive as it's no longer needed.
    archive.close();

    // Return the path to the temporary file.
    return tempPath;
}

bool MainWindow::openAndDisplayFile(const QString &filePath, const QString &groupName) {
    QString fileName = QFileInfo(filePath).fileName();
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("Error"), tr("Unable to open file."));
        return false;
    }

    QTextStream in(&file);
    QStringList fileContent;
    QString line;
    int lineCount = 0;
    const int maxLinesPerFile = 100000; // Max number of lines per file
    int filePartNumber = 1;

    while (!in.atEnd()) {
        line = in.readLine();
        fileContent.append(line);
        lineCount++;

        if (lineCount >= maxLinesPerFile) {
            QString partFileName = QString("%1-%2").arg(filePartNumber).arg(fileName);
            QString tempPath = writeLinesToFile(fileContent, partFileName);
            addToGroup(groupName, partFileName, tempPath);

            fileContent.clear(); // reset for next file
            lineCount = 0;
            filePartNumber++;
        }
    }

    if (!fileContent.isEmpty()) {
        if (filePartNumber > 1) {  // If file is being "broken" into parts, then add numbers before name and add all parts to group
            QString partFileName = QString("%1-%2").arg(filePartNumber).arg(fileName);
            QString tempPath = writeLinesToFile(fileContent, partFileName);
            addToGroup(groupName, partFileName, tempPath);
        } else {
            // If file have less than 100 000 lines, add it to group without number as filename prefix
            QString tempPath = writeLinesToFile(fileContent, fileName);
            addToGroup(groupName, fileName, tempPath);
        }
    }

    file.close();
    return true;
}

QString MainWindow::writeLinesToFile(const QStringList &lines, const QString &fileName) {
    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/" + fileName;
    QFile tempFile(tempPath);
    if (tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&tempFile);
        for (const QString &line : lines) {
            out << line << "\n";
        }
        tempFile.close();
    }
    return tempPath;
}

QString MainWindow::promptForGroupName() {
    QStringList groups;
    for (int i = 0; i < model->rowCount(); ++i) {
        QStandardItem *item = model->item(i);
        if (item) groups << item->text();
    }
    bool ok;
    QString groupName = QInputDialog::getItem(this, tr("Select or enter a group name"),
                                              tr("Group name:"), groups, 0, true, &ok);
    if (ok && !groupName.isEmpty()) {
        return groupName;
    }
    return "";
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
    fileItem->setData(filePath, Qt::UserRole);
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

void MainWindow::onTextEditPrimaryCtrlClicked(const QString &lineText) {
    ui->textEditSecondary->append(lineText); // Append - to the bottom of textEditSecondary
}

void MainWindow::onChangeFontSizeTriggered() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, this);
    if (ok) {
        ui->textEditPrimary->setFont(font);
        ui->textEditSecondary->setFont(font);
    }
}

void MainWindow::setupTextFormattingActions() {
    QAction *changeColorAction = new QAction(tr("Change Text Color"), this);
    QAction *highlightAction = new QAction(tr("Highlight Text"), this);

    connect(changeColorAction, &QAction::triggered, this, &MainWindow::changeTextColorInSecondary);
    connect(highlightAction, &QAction::triggered, this, &MainWindow::highlightTextInSecondary);

    ui->textEditSecondary->addAction(changeColorAction);
    ui->textEditSecondary->addAction(highlightAction);
}

void MainWindow::changeTextColorInSecondary() {
    QColor color = QColorDialog::getColor(Qt::black, this, "Choose Text Color");
    if (color.isValid()) {
        QTextCursor cursor = ui->textEditSecondary->textCursor();
        QTextCharFormat fmt;
        fmt.setForeground(color);
        cursor.mergeCharFormat(fmt);
    }
}

void MainWindow::highlightTextInSecondary() {
    QColor color = QColorDialog::getColor(Qt::yellow, this, "Choose Highlight Color");
    if (color.isValid()) {
        QTextCursor cursor = ui->textEditSecondary->textCursor();
        QTextCharFormat fmt;
        fmt.setBackground(color);
        cursor.mergeCharFormat(fmt);
    }
}

void MainWindow::on_openEditableLog_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open HTML Log"), "", tr("HTML Files (*.html);;All Files (*)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            ui->textEditSecondary->setHtml(file.readAll());
            file.close();
        }
    }
}

void MainWindow::on_save_triggered() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save HTML Log"), "", tr("HTML Files (*.html);;All Files (*)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << ui->textEditSecondary->toHtml();
            file.close();
        }
    }
}

void MainWindow::sortAscendingConfirmed() {
    sortLogs(true);
}

void MainWindow::sortDescendingConfirmed() {
    sortLogs(false);
}

void MainWindow::promptForSortConfirmation(bool ascending) {
    // Store everything from textEditSecondary for later Undo use.
    previousContent = ui->textEditSecondary->toHtml();
    sortLogs(ascending);
}

void MainWindow::sortLogs(bool ascending) {
    QTextDocument *doc = ui->textEditSecondary->document();
    QVector<QPair<QString, QString>> lineData; // HTML and plain text pairs


    for (QTextBlock block = doc->begin(); block.isValid(); block = block.next()) {
        QTextCursor cursor(block);
        cursor.select(QTextCursor::LineUnderCursor);
        QString html = cursor.selection().toHtml();
        QString plainText = block.text();
        lineData.append(qMakePair(plainText, html));
    }

    // Sorting plain text, preserving HTML
    std::sort(lineData.begin(), lineData.end(), [ascending](const QPair<QString, QString> &a, const QPair<QString, QString> &b) {
        return ascending ? a.first < b.first : a.first > b.first;
    });

    // Rebuild new file with preserved HTML editings.
    ui->textEditSecondary->clear();
    QTextCursor newCursor(ui->textEditSecondary->document());
    for (const auto &data : lineData) {
        newCursor.insertHtml(data.second); // Insert HTML
        newCursor.insertBlock(); // Add line
    }
}


void MainWindow::undoChanges() {
    ui->textEditSecondary->setHtml(previousContent);
}

void MainWindow::setupSecondaryTextEditConnections() {
    connect(ui->textEditSecondary, &CustomTextEdit::ctrlClickedForDeletion, this, &MainWindow::deleteSelectedLineInSecondary);
}

void MainWindow::deleteSelectedLineInSecondary(int position) {
    QTextCursor cursor(ui->textEditSecondary->document());
    cursor.setPosition(position);
    cursor.select(QTextCursor::LineUnderCursor);
    cursor.removeSelectedText();
    if (!cursor.atEnd()) {
        cursor.deleteChar(); // Removes newline char if it's not the last line
    }
}

void MainWindow::setupTextEdit() {
    ui->textEditPrimary->setStyleSheet(
        "QTextEdit {"
        "   selection-background-color: green;" // Selected text color and background
        "   selection-color: white;"
        "}"
        );
}

void MainWindow::findNext(const QString &text) {
    QTextDocument::FindFlags flags;
    if (caseSensitiveSearch) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    QTextCursor cursor = ui->textEditPrimary->textCursor();
    QTextCursor foundCursor = ui->textEditPrimary->document()->find(text, cursor, flags);

    if (!foundCursor.isNull()) {
        ui->textEditPrimary->setTextCursor(foundCursor);
    } else {
        QMessageBox::information(this, tr("Text Not Found"), tr("The specified text was not found."));
        ui->textEditPrimary->setTextCursor(cursor);
    }
}

void MainWindow::findPrevious(const QString &text) {
    QTextDocument::FindFlags flags = QTextDocument::FindBackward;
    if (caseSensitiveSearch) {
        flags |= QTextDocument::FindCaseSensitively;
    }

    QTextCursor cursor = ui->textEditPrimary->textCursor();
    QTextCursor foundCursor = ui->textEditPrimary->document()->find(text, cursor, flags);

    if (!foundCursor.isNull()) {
        ui->textEditPrimary->setTextCursor(foundCursor);
    } else {
        QMessageBox::information(this, tr("Text Not Found"), tr("No previous occurrence found."));
        ui->textEditPrimary->setTextCursor(cursor);
    }
}

void MainWindow::updateCaseSensitivity(bool enabled) {
    caseSensitiveSearch = enabled;
}

void MainWindow::toggleFindResults() {
    if (isFindResultsDisplayed) {
        // userContent in textEditSecondary
        ui->textEditSecondary->setHtml(userContent);
        ui->textEditSecondary->setReadOnly(false);
        isFindResultsDisplayed = false;
    } else {
        // Find results in textEditSecondary
        userContent = ui->textEditSecondary->toHtml();
        ui->textEditSecondary->setHtml(findResults);
        ui->textEditSecondary->setReadOnly(true);
        isFindResultsDisplayed = true;
    }
}

void MainWindow::findAllInDocument(const QString &text) {
    QTextDocument *document = ui->textEditPrimary->document();
    QTextCursor cursor(document);
    QString formattedResults;
    bool found = false;

    // CSS needed because of newLines(Now, logs aren't separated with empty new lines).
    QString style = "<style>p { margin: 0; padding: 0; }</style>";

    while (!cursor.isNull() && !cursor.atEnd()) {
        cursor = document->find(text, cursor);
        if (!cursor.isNull() && !cursor.atEnd()) {
            found = true;
            int linePos = cursor.blockNumber();
            QString lineText = cursor.block().text();
            formattedResults += "<p>" + QString::number(linePos + 1) + ": " + lineText + "</p>";
        }
    }

    if (found) {
        findResults = style + formattedResults;
        if (!isFindResultsDisplayed) {
            userContent = ui->textEditSecondary->toHtml();
            isFindResultsDisplayed = true;
        }
        ui->textEditSecondary->setHtml(findResults);
        ui->textEditSecondary->setReadOnly(true);
    } else {
        QMessageBox::information(this, tr("No Matches"), tr("No matches found for the specified text."));
    }
}

void MainWindow::setDarkTheme() {
    //QString windowStyle = "background-color: #1f1f1f; color: #434343;";
    QString textStyle = "QTextEdit { background-color: #121212; color: #FFFFFF; }";
    QString treeStyle = "QTreeView { background-color: #121212; color: #FFFFFF; }";

    //this->setStyleSheet(windowStyle);
    ui->textEditPrimary->setStyleSheet(textStyle);
    ui->textEditSecondary->setStyleSheet(textStyle);
    ui->treeView->setStyleSheet(treeStyle);
}

void MainWindow::setBlueTheme() {
    QString styleSheet = "background-color: #06013d; color: #FFFFFF;";
    ui->textEditPrimary->setStyleSheet(styleSheet);
    ui->textEditSecondary->setStyleSheet(styleSheet);
    ui->treeView->setStyleSheet(styleSheet);
}

void MainWindow::setLightTheme() {
    QString styleSheet = "background-color: #FFFFFF; color: #000000;";
    ui->textEditPrimary->setStyleSheet(styleSheet);
    ui->textEditSecondary->setStyleSheet(styleSheet);
    ui->treeView->setStyleSheet(styleSheet);
}
