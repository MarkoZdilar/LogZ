#ifndef GROUPMANAGER_H
#define GROUPMANAGER_H

#include <QDialog>
#include <QStandardItemModel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QColor>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QMap>
#include <QCheckBox>

/**
 * @brief A dialog for selecting a group name and its associated color.
 */
class ColorSelector : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Retrieves the group name from the input field.
     * @return The group name as a QString.
     */
    QString getGroupName() const;

    /**
     * @brief Retrieves the selected color.
     * @return The selected QColor.
     */
    QColor getSelectedColor() const;

private:
    QLineEdit *groupNameEdit;                   ///< Line edit for entering the group name.
    QDialogButtonBox *buttonBox;                ///< Dialog buttons.
    QVector<QPushButton*> colorButtons;         ///< Buttons for color selection.
    QColor selectedColor;                       ///< Currently selected color.
    static const QVector<QColor> colors;        ///< Predefined set of colors.
};

/**
 * @brief Manages groups within a QStandardItemModel.
 */
class GroupManager : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructor for the GroupManager.
     * @param model Pointer to the QStandardItemModel that stores group data.
     * @param parent The parent QObject.
     */
    explicit GroupManager(QStandardItemModel* model, QObject *parent = nullptr);

    /**
     * @brief Adds a file to a group, creating the group if it does not exist.
     * @param groupName The name of the group.
     * @param fileName The name of the file.
     * @param filePath The path to the file.
     */
    void addToGroup(const QString &groupName, const QString &fileName, const QString &filePath);

    /**
     * @brief Displays a dialog to prompt for a group name and color, and returns the selected group name.
     * @return The selected group name, or an empty string if cancelled.
     */
    QString promptForGroupNameAndColor();

private:
    QStandardItemModel* model;                  ///< The model that holds the items.
    QMap<QString, QColor> groupColors;          ///< Maps group names to their associated colors.

signals:
    /**
     * @brief Signal emitted when a new group is added.
     * @param groupName The name of the group.
     * @param fileName The name of the file added to the group.
     * @param filePath The path to the file.
     */
    void groupAdded(const QString &groupName, const QString &fileName, const QString &filePath);

};

#endif // GROUPMANAGER_H
