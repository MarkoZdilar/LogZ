#include "GroupManager.h"
#include <QMessageBox>
#include <QColorDialog>
#include <QInputDialog>
#include <QCompleter>

GroupManager::GroupManager(QStandardItemModel* model, QObject *parent)
    : QObject(parent), model(model) {}

void GroupManager::addToGroup(const QString &groupName, const QString &fileName, const QString &filePath) {
    QList<QStandardItem *> foundItems = model->findItems(groupName, Qt::MatchExactly);
    QStandardItem *groupItem = foundItems.isEmpty() ? new QStandardItem(groupName) : foundItems.first();

    QColor textColor = groupColors.value(groupName, QColor(Qt::white));  // Use default or existing group color

    if (foundItems.isEmpty()) {
        QFont groupFont;
        groupFont.setBold(true);
        groupItem->setFont(groupFont);
        groupItem->setForeground(QBrush(textColor));
        model->appendRow(groupItem);
    }

    QStandardItem *fileItem = new QStandardItem(fileName);
    QFont fileFont;
    fileFont.setBold(false);
    fileItem->setFont(fileFont);
    fileItem->setForeground(QBrush(textColor));
    fileItem->setData(filePath, Qt::UserRole + 1); // Store file path for opening
    groupItem->appendRow(fileItem);
}


QString GroupManager::promptForGroupNameAndColor() {
    QDialog dialog;
    QVBoxLayout layout(&dialog);

    QLineEdit groupNameEdit;
    groupNameEdit.setPlaceholderText("Enter group name");
    layout.addWidget(&groupNameEdit);

    // Autocompletion options - Fetching all existing groups
    QStringList groups;
    for (int i = 0; i < model->rowCount(); ++i) {
        QStandardItem *item = model->item(i);
        if (item) {
            groups << item->text();
        }
    }
    QCompleter *completer = new QCompleter(groups, &dialog);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    groupNameEdit.setCompleter(completer);

    QPushButton colorButton("Select Color");
    layout.addWidget(&colorButton);

    QColor chosenColor = QColor(Qt::white);  // Default color is white
    connect(&colorButton, &QPushButton::clicked, [&chosenColor, &dialog]() {
        QColor newColor = QColorDialog::getColor(chosenColor, &dialog, "Select Text Color");
        if (newColor.isValid()) {
            chosenColor = newColor;
        }
    });

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout.addWidget(&buttonBox);
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted && !groupNameEdit.text().isEmpty()) {
        QString groupName = groupNameEdit.text();
        if (!groupColors.contains(groupName)) {  // Only update color if it's a new group or color changed
            groupColors[groupName] = chosenColor;
        }
        return groupName;
    }
    return QString();
}
