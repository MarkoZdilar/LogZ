#include "HelpDialog.h"
#include <QLabel>
#include <QPixmap>
#include <QScrollArea>

HelpDialog::HelpDialog(QWidget *parent)
    : QDialog(parent) {
    setupUi();
}

void HelpDialog::setupUi() {
    QVBoxLayout *layout = new QVBoxLayout(this);

    // Create a scroll area
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);

    // Create a widget that will be the container for the scroll area
    QWidget *scrollWidget = new QWidget(this);
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollWidget);

    setupTitle(scrollLayout);
    setupImage(scrollLayout);
    setupMainWindowSection(scrollLayout);
    setupSecondaryWindowSection(scrollLayout);
    setupTreeViewSection(scrollLayout);

    scrollWidget->setLayout(scrollLayout);
    scrollArea->setWidget(scrollWidget);

    layout->addWidget(scrollArea);
    setLayout(layout);
}

void HelpDialog::setupTitle(QVBoxLayout *layout) {
    QLabel *title = new QLabel(tr("LogZ - One place to handle all your logs"), this);
    title->setAlignment(Qt::AlignCenter);
    QFont titleFont = title->font();
    titleFont.setPointSize(16);
    titleFont.setFamily("Cursive");
    title->setFont(titleFont);

    layout->addWidget(title);
}

void HelpDialog::setupImage(QVBoxLayout *layout) {
    QLabel *imageLabel = new QLabel(this);
    QPixmap pixmap(":/icons/LogZ/Resources/Icons/LogZ.png");

    int maxWidth = 400;
    QPixmap scaledPixmap = pixmap.scaledToWidth(maxWidth, Qt::SmoothTransformation);

    imageLabel->setPixmap(scaledPixmap);
    imageLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(imageLabel);
}

void HelpDialog::setupMainWindowSection(QVBoxLayout *layout) {
    QLabel *mainWindowLabel = new QLabel(tr("<div><b>MAIN WINDOW</b></div>"), this);
    QFont mainWindowFont = mainWindowLabel->font();
    mainWindowFont.setPointSize(18);
    mainWindowLabel->setFont(mainWindowFont);
    mainWindowLabel->setStyleSheet("color: orange;");
    layout->addWidget(mainWindowLabel);

    QLabel *mainWindowDescription = new QLabel(this);
    mainWindowDescription->setTextFormat(Qt::RichText);
    mainWindowDescription->setText(tr("<div style='color:orange;'>ALT + LMB:</div> Add a line of text from MAIN WINDOW to SECONDARY WINDOW<br>"
                                      "<div style='color:orange;'>CTRL + F:</div> Open the search dialog with 3 options:"
                                      "<div style='margin-left: 20px;'>- find next<br>"
                                      "- find previous<br>"
                                      "- find all - all search results will be displayed in the secondary window (SECONDARY)</div>"));
    mainWindowDescription->setWordWrap(true);
    layout->addWidget(mainWindowDescription);

    layout->addSpacing(20);
}

void HelpDialog::setupSecondaryWindowSection(QVBoxLayout *layout) {
    QLabel *secondaryWindowLabel = new QLabel(tr("<div><b>SECONDARY WINDOW</b></div>"), this);
    QFont secondaryWindowFont = secondaryWindowLabel->font();
    secondaryWindowFont.setPointSize(18);
    secondaryWindowLabel->setFont(secondaryWindowFont);
    secondaryWindowLabel->setStyleSheet("color: green;");
    layout->addWidget(secondaryWindowLabel);

    QLabel *secondaryWindowDescription = new QLabel(this);
    secondaryWindowDescription->setTextFormat(Qt::RichText);
    secondaryWindowDescription->setText(tr("<div style='color:green;'>CTRL+LMB:</div> Delete a line of text<br>"
                                           "<div style='color:green;'>ALT+ARROW UP:</div> Move a line of text up<br>"
                                           "<div style='color:green;'>ALT + ARROW DOWN:</div> Move a line of text down<br>"
                                           "<div style='color:green;'>TEXT SELECTION + RMB:</div> Open a context menu with options:"
                                           "<div style='margin-left: 20px;'>- Change text color: Change the color of selected text<br>"
                                           "- Highlight text: Highlight the selected text<br>"
                                           "- Change text color (all occurrences): Change the color of all occurrences of selected text<br>"
                                           "- Highlight text (all occurrences): Highlight all occurrences of selected text</div>"));
    secondaryWindowDescription->setWordWrap(true);
    layout->addWidget(secondaryWindowDescription);

    layout->addSpacing(20);
}

void HelpDialog::setupTreeViewSection(QVBoxLayout *layout) {
    QLabel *treeViewLabel = new QLabel(tr("<div><b>TREE VIEW</b></div>"), this);
    QFont treeViewFont = treeViewLabel->font();
    treeViewFont.setPointSize(18);
    treeViewLabel->setFont(treeViewFont);
    treeViewLabel->setStyleSheet("color: #1E90FF;");
    layout->addWidget(treeViewLabel);

    QLabel *treeViewDescription = new QLabel(tr("The place where opened files organized in groups are located."), this);
    treeViewDescription->setWordWrap(true);
    layout->addWidget(treeViewDescription);
}
