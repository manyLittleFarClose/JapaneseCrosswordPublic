#include "plotwidget.h"

#include <QPainter>

PlotWidget::PlotWidget(QWidget *parent)
    : QWidget{parent}
{

}

void PlotWidget::setImage(const QImage &image)
{
    mImage = image;

    mGrayImage = mImage;
    const int w = mGrayImage.width();
    const int h = mGrayImage.height();
    for (int y = 0; y < h; ++y)
    {
        for (int x = 0; x < w; ++x)
        {
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
    const QRectF targetRect(w * 0.2, h * 0.2, w * 0.8, h * 0.8);

    if (mFlagShowOriginal)
    {
        painter.drawImage(targetRect, mImage, mImage.rect());
    }
    else
    {
        painter.drawImage(targetRect, mMonoImage, mMonoImage.rect());
    }

    const int nColumns = mMonoImage.width();
    const int nRows = mMonoImage.height();
    const double imagePixelWidthOnWidget = w * 0.8 / nColumns;
    const double imagePixelHeightOnWidget = h * 0.8 / nRows;

    painter.save();
    painter.translate(w * 0.2, h * 0.2);

    for (int y = 0; y < nRows; ++y)
    {
        std::vector<int> nBlackPixels;
        for (int x = 0; x < nColumns; ++x)
        {
            if (mMonoImage.pixel(x, y) == qRgb(0, 0, 0))
            {
                int nBlackOnePart = 1;
                ++x;
                for (; x < nColumns && mMonoImage.pixel(x, y) == qRgb(0, 0, 0); ++x)
                {
                    nBlackOnePart++;
                }

                nBlackPixels.push_back(nBlackOnePart);
            }
        }

        painter.save();
        painter.translate(0, imagePixelHeightOnWidget * y);
        for (int i = int(nBlackPixels.size()) - 1; i >= 0; --i )
        {
            painter.translate( -imagePixelWidthOnWidget, 0);
            const QRectF pixelRect(0, 0, imagePixelWidthOnWidget, imagePixelHeightOnWidget);
            const QString str = QString::number(nBlackPixels[i]);
            painter.drawText(pixelRect, Qt::TextDontClip | Qt::AlignCenter, str);
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
