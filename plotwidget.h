  #ifndef PLOTWIDGET_H
#define PLOTWIDGET_H

#include <QWidget>

class PlotWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PlotWidget(QWidget *parent = nullptr);

    void setImage(const QImage &image);

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
};

#endif // PLOTWIDGET_H
