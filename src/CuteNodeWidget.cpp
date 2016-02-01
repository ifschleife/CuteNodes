#include "CuteNodeWidget.h"

#include <QBrush>
#include <QGraphicsGridLayout>
#include <QGraphicsProxyWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>


CuteNodeWidget::CuteNodeWidget(QGraphicsItem* parent, Qt::WindowFlags winFlags)
    : QGraphicsWidget(parent, winFlags)
{
    setFlags(flags() | ItemContainsChildrenInShape | ItemIsMovable | ItemIsSelectable);

    QGraphicsGridLayout* layout = new QGraphicsGridLayout;

    QGraphicsProxyWidget* proxy = new QGraphicsProxyWidget;
    proxy->setWidget(new QLabel("in"));
    layout->addItem(proxy, 0, 0);

    QGraphicsProxyWidget* proxy2 = new QGraphicsProxyWidget;
    proxy2->setWidget(new QLineEdit("out"));
    layout->addItem(proxy2, 0, 1);

    setLayout(layout);

    // draw on top of everything
    setZValue(-1.0);
}

CuteNodeWidget::~CuteNodeWidget()
{

}


QVariant CuteNodeWidget::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemSelectedHasChanged)
    {
        const bool nodeSelected = value.toBool() == true;
        _pen.setColor(nodeSelected ? Qt::red : Qt::black);
    }

    return QGraphicsWidget::itemChange(change, value);
}

void CuteNodeWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    QBrush brush(Qt::darkGray);
    painter->setPen(_pen);
    painter->setBrush(brush);
    painter->drawRect(boundingRect());
}
