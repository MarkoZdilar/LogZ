#include "finddialog.h"
#include <QCheckBox>

FindDialog::FindDialog(QWidget *parent) : QDialog(parent), layout() {
    lineEdit = new QLineEdit(this);
    findNextButton = new QPushButton("Find Next", this);
    findPreviousButton = new QPushButton("Find Previous", this);
    findAllButton = new QPushButton("Find All", this);
    caseSensitiveCheckBox = new QCheckBox("Case Sensitive", this);

    layout.addWidget(lineEdit);
    layout.addWidget(findNextButton);
    layout.addWidget(findPreviousButton);
    layout.addWidget(findAllButton);
    layout.addWidget(caseSensitiveCheckBox);
    setLayout(&layout);

    connect(findNextButton, &QPushButton::clicked, this, [this](){
        emit findNext(lineEdit->text());
    });
    connect(findPreviousButton, &QPushButton::clicked, this, [this](){
        emit findPrevious(lineEdit->text());
    });
    connect(findAllButton, &QPushButton::clicked, this, [this](){
        emit findAll(lineEdit->text());
    });
    connect(caseSensitiveCheckBox, &QCheckBox::stateChanged, this, [this](int state){
        emit caseSensitivityChanged(state == Qt::Checked);
    });
}
