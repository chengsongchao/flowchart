#include "scene.h"
#include "node.h"
#include "link.h"
#include <QGraphicsSceneMouseEvent>

Scene::Scene(QObject* parent)
    : QGraphicsScene(parent)
{

}

Scene::Scene(qreal x, qreal y, qreal width, qreal height, QObject *parent)
    : QGraphicsScene(x, y, width, height)
{

}

void Scene::setStart(Node* node)
{
    m_start = node;
}

void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
    if(m_start)
    {
        auto childs = items(event->scenePos());
        if(!childs.empty())
        {
            for(auto child : childs)
            {
                m_end = dynamic_cast<Node*>(child);
                if(m_end && m_start != m_end)
                {
                    Link *link = new Link(m_start, m_end);
                    addItem(link);
                    break;
                }
            }
        }
        if(m_tempLink)
            removeItem(m_tempLink);

        m_start = nullptr;
        m_end = nullptr;
        m_tempLink = nullptr;
    }
    QGraphicsScene::mouseReleaseEvent(event);
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
    if(event->type() == QEvent::GraphicsSceneMouseMove)
    {
        auto sPos =event->scenePos();
        auto childs = items(event->scenePos());
        if(!childs.empty())
        {
            for(auto child : childs)
            {
                auto item = dynamic_cast<Node*>(child);
                if(item)
                {
                    sendEvent(item, event);
                    break;
                }
            }
        }
    }

    if(m_start)
    {
        if(!m_tempLink)
        {
            m_tempLink = new QGraphicsLineItem;
            m_tempLink->setPen(QPen(QColor(Qt::darkRed), 1.0));
            addItem(m_tempLink);
        }
        m_tempLink->setLine(QLineF(m_start->pos(), event->scenePos()));
        return;
    }
    QGraphicsScene::mouseMoveEvent(event);
}
