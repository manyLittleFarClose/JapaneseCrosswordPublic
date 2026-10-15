#include "mainwidget.h"

#include <QFileDialog>
#include <QHBoxLayout>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent)
{
    mButtonOpen->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    auto *buttonsLayout = new QVBoxLayout;
    buttonsLayout->addWidget(mButtonOpen);
    buttonsLayout->addWidget(mButtonZODIAK);
    buttonsLayout->addStretch(); // Опционально: этот "распорка" прижмет кнопки к верху, а не растянет их по всей высоте


    auto *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(buttonsLayout); // Добавляем колонку с кнопками
    mainLayout->addWidget(mPlotWidget);   // Добавляем виджет отрисовки справа от кнопок


    mainLayout->setStretchFactor(mPlotWidget, 1);

    setLayout(mainLayout);

    
    connect(mButtonOpen, &QPushButton::clicked, this, &MainWidget::onClicked);
    connect(mButtonZODIAK, &QPushButton::clicked, this, &MainWidget::onZODIAKClicked);
}

void MainWidget::onZODIAKClicked()
{
    QImage ariesImage = generateAriesPathImage();


    mPlotWidget->setImage(ariesImage);
}

void MainWidget::onClicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open File", "", "Images (*.png *.jpg)");
    //Проверка не пустой ли объект?
    if (fileName.isEmpty()) {
        return;
    }
    QImage image(fileName);
    // проверка не загрузилось ли не то?
    if (image.isNull()) {
        QMessageBox::warning(this,
                             "Ошибка",
                             "Не удалось загрузить изображение. Проверьте формат файла.");
        return;
    }
    mPlotWidget->setImage(image);
}
