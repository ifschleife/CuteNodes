#pragma once

#include <QGraphicsLineItem>


class CuteDock;


class CuteConnection : public QGraphicsLineItem
{
public:
    explicit CuteConnection(const QLineF& line, CuteDock* startItem);
    ~CuteConnection() override;

    enum { Type = UserType + 3 };
    int type() const override { return Type; }

    void setEndDock(CuteDock* dock) { _connectedDocks.second = dock; }

private:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    bool connectionIsValid() const { return _connectedDocks.first && _connectedDocks.second; }

private:
    std::pair<CuteDock*, CuteDock*> _connectedDocks;
};
