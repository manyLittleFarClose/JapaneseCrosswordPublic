#include "plotwidget.h"

#include <QPainter>
#include <vector>

PlotWidget::PlotWidget(QWidget *parent)
    : QWidget{parent}
{}
#include <QImage>
#include <QPainter>
#include <QPainterPath>

QImage generateAriesPathImage()
{
    int size = 500;
    QImage image(size, size, QImage::Format_ARGB32);
    image.fill(Qt::white);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);

    QPen pen(Qt::black);
    pen.setWidth(size / 12);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);

    QPainterPath path;
    double w = size;
    double h = size;

    // --- Левый рог ---
    path.moveTo(w * 0.50, h * 0.85); // Начинаем в нижней центральной части
    // Изгиб вверх и влево
    path.cubicTo(w * 0.10, h * 0.85, w * 0.10, h * 0.20, w * 0.35, h * 0.15);
    // Загиб кончика рога внутрь
    path.cubicTo(w * 0.45, h * 0.12, w * 0.45, h * 0.30, w * 0.35, h * 0.35);

    // --- Правый рог ---
    path.moveTo(w * 0.50, h * 0.85); // Снова начинаем из той же нижней точки
    // Изгиб вверх и вправо
    path.cubicTo(w * 0.90, h * 0.85, w * 0.90, h * 0.20, w * 0.65, h * 0.15);
    // Загиб кончика рога внутрь
    path.cubicTo(w * 0.55, h * 0.12, w * 0.55, h * 0.30, w * 0.65, h * 0.35);

    painter.drawPath(path);

    return image;
}

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

    // Генерируем изображение с сеткой ПОСЛЕ того, как mMonoImage готов
    generateGridImage(mMonoImage);

    //Перерисовываем виджет
    update();
}

void PlotWidget::generateGridImage(const QImage &image)
{
    if (image.isNull() || mMonoImage.isNull()) {
        mImageWithGrid = QImage();
        return;
    }

    // 1. КРИТИЧЕСКИ ВАЖНО: Конвертируем в ARGB32 перед масштабированием.
    // Это предотвращает превращение изображения в белый шум при работе с Mono/Indexed форматами.
    QImage safeImage = image.convertToFormat(QImage::Format_ARGB32);

    // 2. Масштабируем до 1000x1000 (как в вашем исходном наброске).
    // Qt::SmoothTransformation дает более качественный результат при увеличении.
    mImageWithGrid = safeImage.scaled(1000, 1000);

    QPainter painter(&mImageWithGrid);
    painter.setRenderHint(QPainter::Antialiasing); // Делает линии сетки более четкими

    const int nColumns = mMonoImage.width();
    const int nRows = mMonoImage.height();

    // 3. Вычисляем размер клетки исходя из РЕАЛЬНОГО размера получившегося mImageWithGrid
    const double cellWidth = (double) mImageWithGrid.width() / nColumns;
    const double cellHeight = (double) mImageWithGrid.height() / nRows;

    QPen thinPen(Qt::black);
    thinPen.setWidth(1);
    QPen thickPen(Qt::black);
    thickPen.setWidth(3);

    // Вертикальные линии
    for (int i = 0; i <= nColumns; i++) {
        double x = i * cellWidth;
        painter.setPen((i % 5 == 0) ? thickPen : thinPen);
        painter.drawLine(QPointF(x, 0), QPointF(x, mImageWithGrid.height()));
    }

    // Горизонтальные линии
    for (int i = 0; i <= nRows; i++) {
        double y = i * cellHeight;
        painter.setPen((i % 5 == 0) ? thickPen : thinPen);
        painter.drawLine(QPointF(0, y), QPointF(mImageWithGrid.width(), y));
    }
}

void PlotWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);

    const double w = width();
    const double h = height();

    const double offSetX = w * 0.2;
    const double offSetY = h * 0.2;
    const double gridWidth = w * 0.8;
    const double gridHeight = h * 0.8;

    const QRectF targetRect(offSetX, offSetY, gridWidth, gridHeight);

    // Рисуем изображение (с уже встроенной сеткой или оригинал)
    if (mFlagShowOriginal) {
        painter.drawImage(targetRect, mImage, mImage.rect());
    } else if (!mImageWithGrid.isNull()) {
        // Qt автоматически масштабирует mImageWithGrid под targetRect,
        // и сетка идеально совпадет с размерами изображения.
        painter.drawImage(targetRect, mImageWithGrid, mImageWithGrid.rect());
    } else {
        // Fallback на случай, если сетка еще не сгенерирована
        painter.drawImage(targetRect, mImage, mImage.rect());
    }

    // Если изображение не загружено, не рисуем подсказки
    if (mMonoImage.isNull())
        return;

    const int nColumns = mMonoImage.width();
    const int nRows = mMonoImage.height();
    const double imagePixelWidthOnWidget = gridWidth / nColumns;
    const double imagePixelHeightOnWidget = gridHeight / nRows;

    // Настройка шрифта для подсказок
    QFont hintFont = painter.font();
    hintFont.setPixelSize(qMin(imagePixelWidthOnWidget, imagePixelHeightOnWidget) - 2);
    hintFont.setBold(true);
    painter.setFont(hintFont);
    painter.setPen(QPen(Qt::black));
    painter.save();
    painter.translate(offSetX, offSetY);

    // Рисуем подсказки слева от картинки (динамически, не на самой картинке)
    for (int y = 0; y < nRows; ++y) {
        std::vector<int> nBlackPixels;
        for (int x = 0; x < nColumns; ++x) {
            if (mMonoImage.pixel(x, y) == qRgb(0, 0, 0)) {
                int nBlackOnePart = 1;
                ++x;
                for (; x < nColumns && mMonoImage.pixel(x, y) == qRgb(0, 0, 0); ++x) {
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

    // Рисуем подсказки сверху от картинки (динамически, не на самой картинке)
    for (int x = 0; x < nColumns; ++x) {
        std::vector<int> nBlackPixels;
        for (int y = 0; y < nRows; ++y) {
            if (mMonoImage.pixel(x, y) == qRgb(0, 0, 0)) {
                int nBlackOnePart = 1;
                ++y;
                for (; y < nRows && mMonoImage.pixel(x, y) == qRgb(0, 0, 0); ++y) {
                    nBlackOnePart++;
                }
                nBlackPixels.push_back(nBlackOnePart);
            }
        }
        painter.save();
        painter.translate(imagePixelWidthOnWidget * x, 0);
        for (int i = int(nBlackPixels.size()) - 1; i >= 0; --i) {
            painter.translate(0, -imagePixelHeightOnWidget);
            const QRectF pixelRect(0, 0, imagePixelWidthOnWidget, imagePixelHeightOnWidget);
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