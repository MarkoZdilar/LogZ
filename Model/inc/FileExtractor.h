#ifndef FILEEXTRACTOR_H
#define FILEEXTRACTOR_H

#include <QObject>
#include <QString>

class FileExtractor : public QObject {
    Q_OBJECT //This macro is for signals and slots

public:
    FileExtractor(QObject *parent = nullptr);
    ~FileExtractor();

    /**
     * @brief Extracts a specific file from a ZIP archive and writes it to a temporary location.
     *
     * This function opens a ZIP file, locates the specified file within the archive, and extracts
     * its contents to a temporary file on the disk. It then returns the path to this temporary file.
     * This is useful when you need to access the contents of a file inside a ZIP archive without
     * extracting the entire archive.
     *
     * @param zipFilePath The path to the ZIP file from which to extract.
     * @param fileInsideZip The path of the file inside the ZIP archive to extract.
     * @return QString The path to the extracted temporary file, or an empty QString if the extraction fails.
    */
    QString extractFileFromZip(const QString &zipFilePath, const QString &fileInsideZip);

};

#endif // FILEEXTRACTOR_H
