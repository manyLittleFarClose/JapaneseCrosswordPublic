#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QMessageBox>
#include <QPushButton>
#include <QWidget>
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
