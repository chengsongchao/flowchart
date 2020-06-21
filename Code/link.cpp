#include <QtGui>

#include "link.h"
#include "node.h"

Link::Link(Node *fromNode, Node *toNode)
{
    m_fromNode = fromNode;
    m_toNode = toNode;

    m_fromNode->addLink(this);
    m_toNode->addLink(this);

    setFlags(QGraphicsItem::ItemIsSelectable);
    setZValue(-1);

    setColor(Qt::darkRed);
    trackNodes();
}

Link::~Link()
{
    m_fromNode->removeLink(this);
    m_toNode->removeLink(this);
}

Node *Link::fromNode() const
{
    return m_fromNode;
}

Node *Link::toNode() const
{
    return m_toNode;
}

void Link::setColor(const QColor& color)
{
    setPen(QPen(color, 1.0));
}

QColor Link::color() const
{
    return pen().color();
}

void Link::trackNodes()
{
    setLine(QLineF(m_fromNode->pos(), m_toNode->pos()));
}
