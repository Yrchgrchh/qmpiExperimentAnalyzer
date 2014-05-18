#include "mycustomplot.h"

/*void MyCustomPlot::mouseMoveEvent(QMouseEvent *event)
{
    //emit mouseMove(event);

    // call event of affected layout element:
    //if (mMouseEventElement)
    //  mMouseEventElement->mouseMoveEvent(event);

    //QWidget::mouseMoveEvent(event);

    m_x = event->pos().x();
    m_y = event->pos().y();

    QString str = QString("x : %1 y : %2")
            .arg(QString::number(xAxis->pixelToCoord(m_x))).arg(QString::number(yAxis->pixelToCoord(m_y)));

    setToolTip(str);
    repaint();
}*/

void MyCustomPlot::mouseDoubleClickEvent(QMouseEvent *event)
{
    m_x = event->pos().x();
    m_y = event->pos().y();

    repaint();
}

void MyCustomPlot::keyPressEvent(QKeyEvent *event)
{
    if ( event->key() == Qt::Key_Escape )
    {
        m_x = m_y = 0;
    }

    repaint();
}

void MyCustomPlot::paintEvent(QPaintEvent *event)
{
    (void)event;

    QPainter painter(this);
    painter.drawPixmap(0, 0, mPaintBuffer);

    painter.setPen(Qt::black);
    painter.drawLine(0, m_y, this->size().width(), m_y);
    painter.drawLine(m_x, 0, m_x, this->size().height());

    QString str = QString("x : %1 y : %2")
            .arg(QString::number(xAxis->pixelToCoord(m_x))).arg(QString::number(yAxis->pixelToCoord(m_y)));

    painter.drawText(0, 0, 200, 30, Qt::AlignVCenter | Qt::AlignHCenter,str);
}
