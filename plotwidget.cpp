#include "plotwidget.h"

#include <QPainter>

PlotWidget::PlotWidget(QWidget *parent)
    : QWidget{parent}
{}

void PlotWidget::setImage(const QImage &image)
{
    mImage = image;

    mGrayImage = mImage;
    const int w = mGrayImage.width();
    const int h = mGrayImage.height();
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const auto pixel = mGrayImage.pixel(x, y);

            const auto red = qRed(pixel);
            const auto green = qGreen(pixel);
            const auto blue = qBlue(pixel);
            const auto grey = (red + green + blue) / 3;

            mGrayImage.setPixel(x, y, qRgb(grey, grey, grey));
        }
    }

    // масштабируем
    mGrayScaledImage = mGrayImage.scaled(50, 50);

    // превращаем все цвета либо в белый, либо в черный цвет
    mMonoImage = mGrayScaledImage.convertToFormat(QImage::Format_Mono, Qt::MonoOnly);
}

void PlotWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);

    const double w = width();
    const double h = height();
    //Специальные переменные размерности окна
    const double offSetX = w * 0.2;
    const double offSetY = h * 0.2;
    const double gridWidth = w * 0.8;
    const double gridHight = h * 0.8;

    const QRectF targetRect(offSetX, offSetY, gridWidth, gridHight);

    if (mFlagShowOriginal) {
        painter.drawImage(targetRect, mImage, mImage.rect());
    } else {
        painter.drawImage(targetRect, mMonoImage, mMonoImage.rect());
    }

    //Проверка  если изображение не загружено не рисуем его
    if (mMonoImage.isNull())
        return;

    const int nColumns = mMonoImage.width();
    const int nRows = mMonoImage.height();
    const double imagePixelWidthOnWidget = gridWidth / nColumns;
    const double imagePixelHeightOnWidget = gridHight / nRows;

    //настройка шрифта текста, для того чтобы тект нормально влизал
    QFont hintFont = painter.font();
    //Делаем размеры шрифта чуть меньше размера клетки
    hintFont.setPixelSize(qMin(imagePixelWidthOnWidget, imagePixelHeightOnWidget) - 2);
    hintFont.setBold(true); //Делаем шрифт полужирным
    painter.setFont(hintFont);

    painter.save();
    painter.translate(offSetX, offSetY);

    //Создаем Pen для рисования
    QPen thinPen(Qt::black);
    thinPen.setWidth(1);
    QPen thickPen (Qt::black);
    thickPen.setWidth(3);

    //Рисуем линии
    //Вертикальные линии
    for(int i = 0; i <=nColumns; i++){
        double x =  i * imagePixelWidthOnWidget;

        painter.setPen((i % 5 == 0) ? thickPen : thinPen);
        painter.drawLine(x, 0, x, gridHight);
    }
    //Горизонтальные линии
    for(int i = 0; i <=nRows; i++){
        double y = i * imagePixelHeightOnWidget;
        painter.setPen((i % 5 == 0) ? thickPen : thinPen);
        painter.drawLine(0, y, gridWidth, y);
    }

    //Рисуем подсказки слева от картинки
    for (int y = 0; y < nRows; ++y) {
        std::vector<int> nBlackPixels;
        for (int x = 0; x < nColumns; ++x) {
            if (mMonoImage.pixel(x, y) == qRgb(0, 0, 0)) {
                int nBlackOnePart = 1;
                ++x;
                for (; x < nColumns && mMonoImage.pixel(x, y) == qRgb(0,0,0); ++x) {
                    nBlackOnePart++;
                }

                nBlackPixels.push_back(nBlackOnePart);
                x--;
            }
        }

        painter.save();
        painter.translate(0, imagePixelHeightOnWidget * y);
        for (int i = int(nBlackPixels.size()) - 1; i >= 0; --i) {
            painter.translate(-imagePixelWidthOnWidget, 0);
            const QRectF pixelRect(0, 0, imagePixelWidthOnWidget, imagePixelHeightOnWidget);
            const QString str = QString::number(nBlackPixels[i]);
            painter.drawText(pixelRect, Qt::TextDontClip | Qt::AlignCenter, str);
        }
        painter.restore();
    }
    //Рисуем подсказки сверху от картинки
    for(int x = 0; x < nColumns; ++x){
        std::vector<int> nBlackPixels;
        for(int y = 0; y < nRows; ++y){
            if(mMonoImage.pixel(x, y) == qRgb(0, 0 , 0)){
                int nBlackOnePart = 1;
                ++y;
                for(; y <nRows && mMonoImage.pixel(x, y) == qRgb(0,0,0); ++y){
                    nBlackOnePart++;
                }
                nBlackPixels.push_back(nBlackOnePart);
            }
        }
        painter.save();
        painter.translate(imagePixelWidthOnWidget * x, 0);
        for(int i = int(nBlackPixels.size()) - 1; i >=0; --i){
            painter.translate(0, -imagePixelHeightOnWidget);
            const QRect pixelRect(0, 0, imagePixelWidthOnWidget, imagePixelHeightOnWidget);
            const QString str = QString::number(nBlackPixels[i]);
            painter.drawText(pixelRect, Qt::TextDontClip | Qt::AlignHCenter, str);
        }
        painter.restore();
    }


    painter.restore();
}

void PlotWidget::mousePressEvent(QMouseEvent *event)
{
    mFlagShowOriginal = !mFlagShowOriginal;
    update();

    QWidget::mousePressEvent(event);
}
