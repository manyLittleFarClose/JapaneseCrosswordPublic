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

    QPushButton *mButtonOpen = new QPushButton("Открыть", this);
    QPushButton *mButtonZODIAK = new QPushButton("Значок", this);
    PlotWidget *mPlotWidget = new PlotWidget(this);

    void onClicked();
    void onZODIAKClicked();
};
#endif // MAINWIDGET_H
