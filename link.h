#ifndef LINK_H
#define LINK_H

#include <QGraphicsItem>

class Node;

class TempLink : public QGraphicsLineItem
{
public:
    TempLink(){}
    void setStart(const QPointF& pt){}
    void start(const QPointF& pt){}
};

class Link : public QGraphicsLineItem
{
public:
    Link(Node* fromNode, Node* toNode);
    ~Link();

    Node* fromNode() const;
    Node* toNode() const;

    void setColor(const QColor& color);
    QColor color() const;

    void trackNodes();

private:
    Node* m_fromNode;
    Node* m_toNode;
};

#endif
