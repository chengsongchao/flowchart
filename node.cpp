#include <QtGui>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QInputDialog>
#include "link.h"
#include "node.h"
#include "scene.h"

Node::Node()
{
    m_textColor = Qt::darkGreen;
    m_outlineColor = Qt::darkBlue;
    m_backgroundColor = Qt::white;

    setFlags(ItemIsMovable | ItemIsSelectable);
}

Node::~Node()
{
    foreach (Link *link, m_links)
        delete link;
}

void Node::setText(const QString& text)
{
    prepareGeometryChange();
    m_text = text;
    update();
}

QString Node::text() const
{
    return m_text;
}

void Node::setTextColor(const QColor &color)
{
    m_textColor = color;
    update();
}

QColor Node::textColor() const
{
    return m_textColor;
}

void Node::setOutlineColor(const QColor &color)
{
    m_outlineColor = color;
    update();
}

QColor Node::outlineColor() const
{
    return m_outlineColor;
}

void Node::setBackgroundColor(const QColor &color)
{
    m_backgroundColor = color;
    update();
}

QColor Node::backgroundColor() const
{
    return m_backgroundColor;
}

void Node::addLink(Link *link)
{
    m_links.insert(link);
}

void Node::removeLink(Link *link)
{
    m_links.remove(link);
}

QRectF Node::boundingRect() const
{
    const int Margin = 1;
    return outlineRect().adjusted(-Margin, -Margin, +Margin, +Margin);
}

QPainterPath Node::shape() const
{
    QRectF rect = outlineRect();

    QPainterPath path;
    path.addRoundRect(rect, roundness(rect.width()),
                      roundness(rect.height()));
    return path;
}

void Node::paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget * /* widget */)
{
    QPen pen(m_outlineColor);
    if (option->state & QStyle::State_Selected)
    {
        pen.setStyle(Qt::DotLine);
        pen.setWidth(2);
    }
    painter->setPen(pen);
    painter->setBrush(m_backgroundColor);

    QRectF rect = outlineRect();
    painter->drawRoundRect(rect, roundness(rect.width()),
                           roundness(rect.height()));

    painter->setPen(m_textColor);
    painter->drawText(rect, Qt::AlignCenter, m_text);
}

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    QString text = QInputDialog::getText(event->widget(),
                           tr("Edit Text"), tr("Enter new text:"),
                           QLineEdit::Normal, m_text);
    if (!text.isEmpty())
        setText(text);
}

void Node::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    for(auto link : m_links)
    {
        link->trackNodes();
    }

    QGraphicsItem::mouseMoveEvent(event);
}

void Node::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    QPointF curPos = event->pos();
    QRectF rect = outlineRect();
    rect.adjust(3, 3, -3, -3);
    //rect.translate(nodePos);
    if(!rect.contains(curPos))
    {
        auto sc = dynamic_cast<Scene*>(scene());
        if(sc)
        {
            sc->setStart(this);
            return;
        }
    }

    QGraphicsItem::mousePressEvent(event);
}

void Node::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);
}

void Node::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    QGraphicsItem::dragMoveEvent(event);
}

QVariant Node::itemChange(GraphicsItemChange change,
                          const QVariant &value)
{
    if (change == ItemPositionHasChanged)
    {
        foreach (Link *link, m_links)
            link->trackNodes();
    }
    return QGraphicsItem::itemChange(change, value);
}

QRectF Node::outlineRect() const
{
    const int Padding = 8;
    QFontMetrics metrics = QApplication::fontMetrics();
    QRect rect = metrics.boundingRect(m_text);
    rect.adjust(-Padding, -Padding, +Padding, +Padding);
    rect.translate(-rect.center());
    return rect;
}

int Node::roundness(double size) const
{
    const int Diameter = 12;
    return 100 * Diameter / int(size);
}
