#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QObject>
#include <QString>
#include <QFileInfo>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <KZip>
#include <KArchiveFile>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QVector>

struct LogEntry {
    QDateTime dateTime;
    QString text;
    QList<QTextLayout::FormatRange> formats;

    LogEntry(QDateTime dt, const QString& txt, QList<QTextLayout::FormatRange> fmts)
        : dateTime(dt), text(txt), formats(fmts) {}
};

class LogManager : public QObject
{
    Q_OBJECT

public:
    LogManager(QObject *parent = nullptr);
    ~LogManager();

    /**
     * @brief Opens and displays the content of the specified file under a specific group.
     * @param filePath Path of the file to open.
     * @param groupName Name of the group under which the file will be added.
     * @return True if the file is successfully opened and displayed; false otherwise.
     *
     * Opens the specified file and adds it to a specified group in the tree view. If the
     * file opens successfully, its content is displayed in the primary text edit widget.
     */
    bool openAndDisplayFile(const QString &filePath, const QString &groupName);

    /**
     * @brief Sorts logs within a document either in ascending or descending order.
     * @param document Pointer to the QTextDocument that contains the logs to sort.
     * @param ascending Boolean flag indicating sorting order; true for ascending, false for descending.
     * @return Vector of pairs containing sorted logs as plain and HTML text.
     */
    QVector<QPair<QString, QString>> sortLogs(QTextDocument* document, bool ascending);

    /**
     * @brief Extracts a specific file(s) from a ZIP archive and writes it to a temporary location.
     *
     * This function opens a ZIP file, locates the specified files within the archive, and extracts
     * its contents to a temporary file on the disk. It then returns the path to this temporary file.
     * This is useful when you need to access the contents of a file inside a ZIP archive without
     * extracting the entire archive.
     *
     * @param zipFilePath The path to the ZIP file from which to extract.
     * @param fileInsideZip The path of the file inside the ZIP archive to extract.
     * @return QString The path to the extracted temporary file, or an empty QString if extraction failed.
     */
    QString extractFileFromZip(const QString &zipFilePath, const QString &fileInsideZip);

    /**
     * @brief Adds a file to a specified group in the tree view.
     * @param groupName The name of the group under which the file will be added.
     * @param fileName The name of the file to be added.
     * @param filePath The path of the file to be added.
     *
     * Adds a file to an existing or new group in the tree view. If the group does not exist,
     * a new group item is created. The file is then added to this group. Each file in the
     * group is represented as a child node under the group node in the tree view.
     */
    void addToGroup(const QString &groupName, const QString &fileName, const QString &filePath);

signals:
    /**
     * @brief Signal emitted when an error occurs within the LogManager.
     * @param error Description of the error.
     */
    void errorOccurred(const QString &error);

    /**
     * @brief Signal emitted when a file is added to a group.
     * @param groupName Name of the group to which the file is added.
     * @param fileName Name of the file added.
     * @param filePath Path of the file added.
     */
    void fileAddedToGroup(const QString &groupName, const QString &fileName, const QString &filePath);

private:
    /**
     * @brief Writes a list of lines to a temporary file.
     *
     * This function creates a new temporary file and writes the provided list of lines to it.
     * The function returns the path to the created file, allowing further manipulation or display.
     *
     * @param lines QStringList containing all lines to be written to the file.
     * @param fileName The name to be assigned to the temporary file.
     * @return QString The path to the newly created temporary file.
     */
    QString writeLinesToFile(const QStringList &lines, const QString &fileName);};

#endif // LOGMANAGER_H
