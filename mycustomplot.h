#ifndef MYCUSTOMPLOT_H
#define MYCUSTOMPLOT_H

#include "qcustomplot.h"

class MyCustomPlot : public QCustomPlot
{
    double m_x; double m_y;
public:
    MyCustomPlot()
        :QCustomPlot(),m_x(0.0), m_y(0.0){}
   ~MyCustomPlot(){}

    //void mouseMoveEvent(QMouseEvent *event);

    void mouseDoubleClickEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void paintEvent(QPaintEvent *event);    
};

#endif // MYCUSTOMPLOT_H
