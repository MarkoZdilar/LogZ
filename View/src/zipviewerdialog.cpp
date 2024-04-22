#include "zipviewerdialog.h"
#include <KZip>
#include <KArchiveEntry>
#include <KArchiveDirectory>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QMessageBox>

// Konstruktor za ZipViewerDialog
ZipViewerDialog::ZipViewerDialog(const QString &zipPath, QWidget *parent)
    : QDialog(parent), treeWidget(new QTreeWidget(this)) {
    setWindowTitle("Pregled sadržaja ZIP datoteke");
    treeWidget->setHeaderLabel("Datoteke i direktoriji");
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(treeWidget);

    // Gumbi za potvrdu i otkazivanje
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addWidget(buttonBox);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &ZipViewerDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &ZipViewerDialog::reject);

    // Učitaj sadržaj ZIP datoteke
    loadZipContents(zipPath);
}

// Učitavanje sadržaja ZIP datoteke
void ZipViewerDialog::loadZipContents(const QString &zipPath) {
    KZip archive(zipPath);
    if (archive.open(QIODevice::ReadOnly)) {
        const KArchiveDirectory* rootDir = archive.directory();
        populateTreeWidget(rootDir, nullptr);  // Popunjavanje treeWidget-a s sadržajem
        archive.close();
    } else {
        QMessageBox::critical(this, "Greška", "Nije moguće otvoriti ZIP datoteku.");
    }
}

// Rekurzivno popunjavanje QTreeWidget-a
void ZipViewerDialog::populateTreeWidget(const KArchiveDirectory* directory, QTreeWidgetItem* parentItem) {
    QStringList entries = directory->entries();
    foreach (const QString &entry, entries) {
        const KArchiveEntry *e = directory->entry(entry);
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, e->name());
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable); // Omogućuje checkbox
        item->setCheckState(0, Qt::Unchecked); // Postavlja checkbox u neoznačeno stanje

        if (e->isDirectory()) {
            item->setIcon(0, QIcon(":/icons/folder.png"));  // Pretpostavlja se da imate ikonu direktorija
        } else {
            item->setIcon(0, QIcon(":/icons/file.png"));  // Pretpostavlja se da imate ikonu datoteke
        }
        if (parentItem) {
            parentItem->addChild(item);
        } else {
            treeWidget->addTopLevelItem(item);
        }
        if (e->isDirectory()) {
            populateTreeWidget(static_cast<const KArchiveDirectory *>(e), item);
        }
    }
}

// Funkcija accept će se pozvati kada korisnik klikne "OK"
void ZipViewerDialog::accept() {
    selectedFiles.clear();
    collectSelectedFiles(treeWidget->invisibleRootItem()); // Početak prikupljanja odabranih datoteka
    QDialog::accept();
}

// Rekurzivna funkcija za prikupljanje odabranih datoteka
void ZipViewerDialog::collectSelectedFiles(QTreeWidgetItem* item) {
    if (item->checkState(0) == Qt::Checked) {
        selectedFiles.append(item->text(0)); // Dodaje putanju datoteke u listu
    }
    for (int i = 0; i < item->childCount(); ++i) {
        collectSelectedFiles(item->child(i)); // Rekurzivno prikuplja odabrane datoteke
    }
}

QStringList ZipViewerDialog::getSelectedFiles() const {
    return selectedFiles;
}
