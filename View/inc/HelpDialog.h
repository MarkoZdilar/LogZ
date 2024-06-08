#ifndef HELPDIALOG_H
#define HELPDIALOG_H

#include <QDialog>
#include <QVBoxLayout>

/**
 * @class HelpDialog
 * @brief The HelpDialog class provides a dialog for displaying help information.
 *
 * This class is responsible for creating and displaying a help dialog that provides
 * users with detailed information about how to use the application. The dialog includes
 * sections for different parts of the application and an image for visual guidance.
 */
class HelpDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructs a HelpDialog object.
     * @param parent The parent widget, nullptr for a dialog without a parent.
     */
    explicit HelpDialog(QWidget *parent = nullptr);

private:
    /**
     * @brief Sets up the user interface of the help dialog.
     *
     * This function initializes the layout and adds all necessary sections
     * and widgets to the dialog.
     */
    void setupUi();

    /**
     * @brief Sets up the title of the help dialog.
     * @param layout The layout to which the title will be added.
     *
     * This function creates and configures the title label and adds it
     * to the specified layout.
     */
    void setupTitle(QVBoxLayout *layout);

    /**
     * @brief Sets up the image in the help dialog.
     * @param layout The layout to which the image will be added.
     *
     * This function loads the help image, scales it appropriately,
     * and adds it to the specified layout.
     */
    void setupImage(QVBoxLayout *layout);

    /**
     * @brief Sets up the main window section in the help dialog.
     * @param layout The layout to which the main window section will be added.
     *
     * This function creates and configures the labels and descriptions
     * for the main window section and adds them to the specified layout.
     */
    void setupMainWindowSection(QVBoxLayout *layout);

    /**
     * @brief Sets up the secondary window section in the help dialog.
     * @param layout The layout to which the secondary window section will be added.
     *
     * This function creates and configures the labels and descriptions
     * for the secondary window section and adds them to the specified layout.
     */
    void setupSecondaryWindowSection(QVBoxLayout *layout);

    /**
     * @brief Sets up the tree view section in the help dialog.
     * @param layout The layout to which the tree view section will be added.
     *
     * This function creates and configures the labels and descriptions
     * for the tree view section and adds them to the specified layout.
     */
    void setupTreeViewSection(QVBoxLayout *layout);
};

#endif // HELPDIALOG_H
