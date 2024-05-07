#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QCheckBox>

/**
 * @brief The FindDialog class provides a dialog for finding text in a document.
 *
 * This dialog allows the user to search for text using various options like
 * find next, find previous, and find all. It also supports case sensitivity.
 */
class FindDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Construct a new Find Dialog object
     *
     * @param parent The parent widget of the dialog, nullptr if it has no parent.
     */
    explicit FindDialog(QWidget *parent = nullptr);

signals:
    /**
     * @brief Signal emitted when the user clicks the "Find Next" button.
     *
     * @param text The text to find.
     */
    void findNext(const QString &text);

    /**
     * @brief Signal emitted when the user clicks the "Find Previous" button.
     *
     * @param text The text to find.
     */
    void findPrevious(const QString &text);

    /**
     * @brief Signal emitted when the user clicks the "Find All" button.
     *
     * @param text The text to find.
     */
    void findAll(const QString &text);

    /**
     * @brief Signal emitted when the case sensitivity option changes.
     *
     * @param enabled True if case sensitivity is enabled, false otherwise.
     */
    void caseSensitivityChanged(bool enabled);

private:
    QLineEdit *lineEdit; ///< Line edit for entering the text to search.
    QPushButton *findNextButton, *findPreviousButton, *findAllButton; ///< Button to find the next/previous/all occurrence of the search term.
    QCheckBox *caseSensitiveCheckBox; ///< Checkbox to toggle case sensitivity in search.
    QVBoxLayout layout; ///< Layout to arrange widgets vertically in the dialog.
};

#endif // FINDDIALOG_H
