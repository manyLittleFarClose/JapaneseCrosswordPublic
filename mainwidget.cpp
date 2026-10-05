#include "mainwidget.h"

#include <QHBoxLayout>
#include <QFileDialog>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    mButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto *layout = new QHBoxLayout;
    layout->addWidget(mButton);
    layout->addWidget(mPlotWidget);
    setLayout(layout);

    connect(mButton, &QPushButton::clicked, this, &MainWidget::onClicked);
}

void MainWidget::onClicked()
{
    QString fileName = QFileDialog::getOpenFileName(
                    this, "Open File", "", "Images (*.png *.jpg)");

    QImage image(fileName);
    mPlotWidget->setImage(image);
}

