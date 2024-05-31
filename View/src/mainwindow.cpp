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
#include <QTextBlock>
#include <QDateTime>
#include <QRegularExpression>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    model(new QStandardItemModel(this)),
    logManager(new LogManager),
    groupManager(new GroupManager(model, this)) {
    ui->setupUi(this);
    initializeTreeView();
    initializeFonts();
    setupMenuBar();
    setupDelegatesAndConnections();
    setupTextFormattingActions();
    setupSecondaryTextEditConnections();
    setupTextEdit();
    setupFindDialog();
    setupGroupLogConnections();
}

// Destructor
MainWindow::~MainWindow() {
    delete ui;
    delete logManager;
    delete groupManager;
}

// Initialize the tree view with proper width
void MainWindow::initializeTreeView() {
    int percentWidth = 40;
    int treeWidth = this->width() * percentWidth / 100;
    ui->treeView->setMaximumWidth(treeWidth);
    ui->treeView->setModel(model);  // Samo postavite model, ne stvarajte novi
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
    QString filter = "All supported files (*.txt *.log *.zip);;Text files (*.txt);;Log files (*.log);;ZIP archives (*.zip);;All files (*.*)";
    QStringList filePaths = QFileDialog::getOpenFileNames(this, tr("Open Files"), "", filter);

    if (!filePaths.isEmpty()) {
        QString groupName = groupManager->promptForGroupNameAndColor();
        if (!groupName.isEmpty()) {
            for (const QString &filePath : filePaths) {
                if (filePath.endsWith(".zip")) {
                    ZipViewerDialog viewer(filePath, this);
                    if (viewer.exec() == QDialog::Accepted) {
                        QStringList filesToAdd = viewer.getSelectedFiles();
                        for (const QString &fileInsideZip : filesToAdd) {
                            QString tempPath = logManager->extractFileFromZip(filePath, fileInsideZip);
                            if (!tempPath.isEmpty()) {
                                groupManager->addToGroup(groupName, QFileInfo(fileInsideZip).fileName(), tempPath);
                            }
                        }
                    }
                } else {
                    logManager->openAndDisplayFile(filePath, groupName);
                }
            }
        }
    }
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
    groupManager->addToGroup(groupName, fileName, filePath);
}

void MainWindow::onTreeViewClicked(const QModelIndex &index) {
    if (!index.isValid()) return;

    QStandardItem *item = model->itemFromIndex(index);
    if (!item) return;

    // If parrent exists, that means that file is clicked, else group
    if (item->parent()) {
        QString filePath = item->data(Qt::UserRole + 1).toString();
        QFile file(filePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream in(&file);
            ui->textEditPrimary->setPlainText(in.readAll());
            file.close();
        } else {
            QMessageBox::warning(this, tr("Error"), tr("Cannot open file."));
        }
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

void MainWindow::displayError(const QString &error) {
    QMessageBox::warning(this, tr("Error"), error);
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
    previousContent = ui->textEditSecondary->toHtml();  // Store before sorting
    sortLogs(ascending);
}

void MainWindow::sortLogs(bool ascending) {
    QTextDocument *doc = ui->textEditSecondary->document();
    // Get sorted data from LogManager
    QVector<QPair<QString, QString>> sortedData = logManager->sortLogs(doc, ascending);

    // Clear the existing content and insert the sorted data
    ui->textEditSecondary->clear();
    QTextCursor newCursor(ui->textEditSecondary->document());
    for (const auto &data : sortedData) {
        newCursor.insertHtml(data.second); // Insert HTML content
        newCursor.insertBlock(); // Insert a new block for next line
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
            QString lineText = cursor.block().text();
            formattedResults += "<p>" + lineText + "</p>";
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

void MainWindow::setupGroupLogConnections() {
    connect(logManager, &LogManager::errorOccurred, this, &MainWindow::displayError);
    connect(logManager, &LogManager::fileAddedToGroup, this, &MainWindow::addToGroup);
    connect(groupManager, &GroupManager::groupAdded, this, &MainWindow::addToGroup);
}
