#ifndef SCENE_H
#define SCENE_H
#include <QGraphicsScene>

class Node;
class QGraphicsLineItem;
class Scene : public QGraphicsScene
{
public:
    Scene(QObject* parent = nullptr);
    Scene(qreal x, qreal y, qreal width, qreal height, QObject *parent = nullptr);
    void setStart(Node* node);

protected:
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

    Node* m_start{nullptr};
    Node* m_end{nullptr};
    QGraphicsLineItem* m_tempLink{nullptr};
};

#endif // SCENE_H
