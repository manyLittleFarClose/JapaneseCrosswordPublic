#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <plotwidget.h>

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = nullptr);

    QPushButton *mButton = new QPushButton("Открыть", this);
    PlotWidget *mPlotWidget = new PlotWidget(this);

    void onClicked();
};
#endif // MAINWIDGET_H
