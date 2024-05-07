#include "FileExtractor.h"
#include <KZip>
#include <KArchiveFile>
#include <QMessageBox>
#include <QStandardPaths>
#include <QFile>

FileExtractor::FileExtractor(QObject *parent) : QObject(parent) {}

FileExtractor::~FileExtractor() {}

QString FileExtractor::extractFileFromZip(const QString &zipFilePath, const QString &fileInsideZip) {
    KZip archive(zipFilePath);
    if (!archive.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(nullptr, "Error", "Failed to open ZIP file.");
        return QString();
    }

    const KArchiveDirectory *rootDir = archive.directory();
    const KArchiveEntry *entry = rootDir->entry(fileInsideZip);
    if (!entry || !entry->isFile()) {
        QMessageBox::warning(nullptr, "Error", "The specified file does not exist within the ZIP archive.");
        return QString();
    }

    const KArchiveFile *file = static_cast<const KArchiveFile *>(entry);
    QString tempPath = QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/" + fileInsideZip;

    QFile tempFile(tempPath);
    if (!tempFile.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(nullptr, "Error", "Unable to create temporary file for ZIP contents.");
        return QString();
    }

    tempFile.write(file->data());
    tempFile.close();
    archive.close();

    return tempPath;
}
