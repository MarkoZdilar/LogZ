#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "QToolBar"
#include <QFileDialog>
#include <QFile>
#include <QIODevice>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Setiraj max sirinu 35% (Ne moze se to setirati kroz qt designer.
    int percentWidth = 35;
    int treeWidth = this->width() * percentWidth / 100;
    ui->treeView->setMaximumWidth(treeWidth);

    // Inicijalizacija modela
    model = new QStandardItemModel(this);
    ui->treeView->setModel(model);  //Setiranje modela u treeView

    // Povezivanje signala i slota
    connect(ui->treeView, &QTreeView::clicked, this, &MainWindow::onTreeViewClicked); //Kada se klikne na naziv filea, njega prikazi.
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered() {
    QString filter = "Text files (*.txt);;Log files (*.log);;Net files (*.net);;All files (*.*)";
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), filter);
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            ui->textEditPrimary->setPlainText(stream.readAll());
            file.close();

            // Izvuci samo file name
            QFileInfo fileInfo(file.fileName());
            QString justFileName = fileInfo.fileName();

            // Add file name to the tree view
            QStandardItem *item = new QStandardItem(justFileName);
            item->setData(fileInfo.absoluteFilePath(), Qt::UserRole);  // UserRole - koristimo za spremanje podataka koje nećemo koristiti u UI-ju(fetchamo full path kada kliknemo na element), tj. nemaju svoj model
            model->appendRow(item);
        }
    }
}

void MainWindow::onTreeViewClicked(const QModelIndex &index) {
    QString filePath = model->data(index, Qt::UserRole).toString(); //Dohvati puni file path iz modela
    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&file);
        ui->textEditPrimary->setPlainText(stream.readAll());
        file.close();
    }
}
