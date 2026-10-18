#ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>
QImage generateAriesPathImage();
class PlotWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlotWidget(QWidget *parent = nullptr);

    void setImage(const QImage &image);
    void generateGridImage(const QImage &image);

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

    bool mFlagShowOriginal = false;

    // исходная
    QImage mImage;
    // черно-белая картинка
    QImage mGrayImage;
    // скалированная картинка
    QImage mGrayScaledImage;
    // монохромная картинка
    QImage mMonoImage;
    //монохром с сеткой
    QImage mImageWithGrid;
};

#endif // PLOTWIDGET_H
