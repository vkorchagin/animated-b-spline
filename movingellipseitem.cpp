#include "movingellipseitem.h"
#include <QGraphicsSceneMouseEvent>

MovingEllipseItem::MovingEllipseItem(qreal x, qreal y, qreal width,
                                     qreal height, MainWindow *mainWindow,
                                     QGraphicsItem *parent) :
  QGraphicsEllipseItem(x, y, width, height, parent), mainWindow(mainWindow) {}

/// mouseMoveEvent - moves control point on QGraphicsView, updates control
/// point position in \var mainWindow->controlPoints and rerenders scene.
void MovingEllipseItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event) {
  // Check for borders.
  // FIXME: Correctly determine borders of view.
  //if (pos.x() > 0 && pos.x() < 1500 && pos.y() > 0 && pos.y() < 1500)
  if (true)
    QGraphicsEllipseItem::mouseMoveEvent(event);
  else
    return;
  const QPointF &pos = event->scenePos();
  // FIXME: position this on center of mouse position.
  //this->setPos(pos.x(), pos.y());
  QPointF *point = mainWindow->itemToPoint[this];
  *point = pos;
  const QList<QGraphicsItem*> &items = mainWindow->scene->items();
  for (QList<QGraphicsItem*>::const_iterator itemIt = items.begin();
       itemIt != items.end(); ++itemIt)
    if (*itemIt != this) {
      mainWindow->scene->removeItem(*itemIt);
      //mainWindow->itemToPoint.remove(static_cast<MovingEllipseItem*>(*itemIt));
      delete *itemIt;
    }
  mainWindow->updateView(point);
}
