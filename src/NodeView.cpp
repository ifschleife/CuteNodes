#include "NodeView.h"

#include <QMouseEvent>
#include <QScrollBar>

#include <algorithm>


namespace
{
    constexpr qreal MinScale           = 0.5;
    constexpr qreal MaxScale           = 3.0;
    constexpr qreal DefaultScaleFactor = 1.10;
}


NodeView::NodeView(QWidget* parent)
    : QGraphicsView(parent)
{
    setDragMode(RubberBandDrag);
    setRenderHints(QPainter::TextAntialiasing | QPainter::HighQualityAntialiasing);
}

NodeView::~NodeView()
{

}


void NodeView::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() == Qt::MiddleButton)
    {
        if (_isPanning)
        {
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() - (event->x() - _panStartPos.x()));
            verticalScrollBar()->setValue(verticalScrollBar()->value() - (event->y() - _panStartPos.y()));
            _panStartPos = event->pos();

            event->accept();
            return;
        }
    }

    QGraphicsView::mouseMoveEvent(event);
}

void NodeView::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() == Qt::MiddleButton) // only start panning when no other button is pressed
    {
        _isPanning = true;
        _panStartPos = event->pos();
        setCursor(Qt::ClosedHandCursor);

        event->accept();
        return;
    }

    QGraphicsView::mousePressEvent(event);
}

void NodeView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton)
    {
        _isPanning = false;
        setCursor(Qt::ArrowCursor);

        event->accept();
        return;
    }

    QGraphicsView::mouseReleaseEvent(event);
}

void NodeView::wheelEvent(QWheelEvent* event)
{
    // Adapted from http://stackoverflow.com/a/20802191/578536
    const QPointF sceneMousePos = mapToScene(event->pos());

    if (event->delta() > 0 && _sceneScale < MaxScale)
    {
        scale(DefaultScaleFactor, DefaultScaleFactor);
        _sceneScale *= DefaultScaleFactor;
    }
    else if (event->delta() < 0 && _sceneScale > MinScale)
    {
        scale(1.0 / DefaultScaleFactor, 1.0 / DefaultScaleFactor);
        _sceneScale /= DefaultScaleFactor;
    }

    const QPointF scaledMousePos = mapFromScene(sceneMousePos);
    const QPointF deltaMouse = scaledMousePos - event->pos();
    horizontalScrollBar()->setValue(static_cast<int>(deltaMouse.x() + horizontalScrollBar()->value()));
    verticalScrollBar()->setValue(static_cast<int>(deltaMouse.y() + verticalScrollBar()->value()));

    event->accept();
}
