#include "LogManager.h"
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <KZip>
#include <KArchiveFile>
#include <QMessageBox>
#include <QRegularExpression>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QFileInfo>

LogManager::LogManager(QObject *parent) : QObject(parent) {}
LogManager::~LogManager() {}

bool LogManager::openAndDisplayFile(const QString &filePath, const QString &groupName) {
    QString fileName = QFileInfo(filePath).fileName();
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit errorOccurred(tr("Unable to open file."));
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
        if (filePartNumber > 1) {
            QString partFileName = QString("%1-%2").arg(filePartNumber).arg(fileName);
            QString tempPath = writeLinesToFile(fileContent, partFileName);
            addToGroup(groupName, partFileName, tempPath);
        } else {
            QString tempPath = writeLinesToFile(fileContent, fileName);
            addToGroup(groupName, fileName, tempPath);
        }
    }

    file.close();
    return true;
}

QString LogManager::writeLinesToFile(const QStringList &lines, const QString &fileName) {
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

QString LogManager::extractFileFromZip(const QString &zipFilePath, const QString &fileInsideZip) {
    KZip archive(zipFilePath);
    if (!archive.open(QIODevice::ReadOnly)) {
        emit errorOccurred(tr("Failed to open ZIP file."));
        return QString();
    }

    const KArchiveDirectory *rootDir = archive.directory();
    const KArchiveEntry *entry = rootDir->entry(fileInsideZip);
    if (!entry || !entry->isFile()) {
        emit errorOccurred(tr("The specified file does not exist within the ZIP archive."));
        return QString();
    }

    const KArchiveFile *file = static_cast<const KArchiveFile *>(entry);
    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/" + fileInsideZip;

    QFile tempFile(tempPath);
    if (!tempFile.open(QIODevice::WriteOnly)) {
        emit errorOccurred(tr("Unable to create temporary file for ZIP contents."));
        return QString();
    }

    tempFile.write(file->data());
    tempFile.close();
    archive.close();

    return tempPath;
}

QVector<QPair<QString, QString>> LogManager::sortLogs(QTextDocument* document, bool ascending) {
    QVector<QPair<QString, QString>> lineData; // HTML and plain text pairs

    for (QTextBlock block = document->begin(); block.isValid(); block = block.next()) {
        QTextCursor cursor(block);
        cursor.select(QTextCursor::LineUnderCursor);
        QString html = cursor.selection().toHtml();
        QString plainText = block.text();
        lineData.append(qMakePair(plainText, html));
    }

    std::sort(lineData.begin(), lineData.end(), [ascending](const QPair<QString, QString>& a, const QPair<QString, QString>& b) {
        return ascending ? a.first < b.first : a.first > b.first;
    });

    return lineData;
}

void LogManager::addToGroup(const QString &groupName, const QString &fileName, const QString &filePath) {
    // This would trigger a signal to update the UI accordingly, instead of directly manipulating UI components here
    emit fileAddedToGroup(groupName, fileName, filePath);
}
