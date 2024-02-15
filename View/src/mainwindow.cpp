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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionOpen_triggered() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QString(), tr("Text Files (*.txt);;All Files (*)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);
            ui->textEditPrimary->setPlainText(stream.readAll()); //Setiramo sve u textEditPrimary (ne-editabilni prozor)
            file.close();
        }
    }
}
