#include "NodeView.h"

#include <QMouseEvent>
#include <QScrollBar>


NodeView::NodeView(QWidget* parent)
    : QGraphicsView(parent)
    , _isPanning{false}
    , _panStartPos{0, 0}
{

}

NodeView::~NodeView()
{

}


void NodeView::mouseMoveEvent(QMouseEvent* event)
{
    if (_isPanning)
    {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (event->x() - _panStartPos.x()));
        verticalScrollBar()->setValue(verticalScrollBar()->value() - (event->y() - _panStartPos.y()));
        _panStartPos = event->pos();

        event->accept();
    }
    else
    {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void NodeView::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() == Qt::MiddleButton) // only start panning when no other button is pressed
    {
        _isPanning = true;
        _panStartPos = event->pos();
        setCursor(Qt::ClosedHandCursor);

        event->accept();
    }
    else
    {
        QGraphicsView::mousePressEvent(event);
    }
}

void NodeView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton)
    {
        _isPanning = false;
        setCursor(Qt::ArrowCursor);

        event->accept();
    }
    else
    {
        QGraphicsView::mouseReleaseEvent(event);
    }
}
