#include "CuteNodeWidget.h"

#include <QBrush>
#include <QGraphicsGridLayout>
#include <QGraphicsProxyWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPainter>


CuteNodeWidget::CuteNodeWidget(QGraphicsItem* parent, Qt::WindowFlags flags)
    : QGraphicsWidget(parent, flags)
{
    setFlag(QGraphicsItem::ItemIsMovable);
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

void CuteNodeWidget::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    QBrush brush(Qt::darkGray);
    painter->setBrush(brush);
    painter->drawRect(boundingRect());
}
