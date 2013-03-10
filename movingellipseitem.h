#ifndef MOVINGELLIPSEITEM_H
#define MOVINGELLIPSEITEM_H

#include "mainwindow.h"
#include <QGraphicsEllipseItem>
#include <QHash>

/// MovingEllipseItem - this class represents control point on QGraphicsView on
/// \var mainWindow.
class MovingEllipseItem : public QGraphicsEllipseItem {
public:
  MovingEllipseItem(qreal x, qreal y, qreal width, qreal height,
                    MainWindow *mainWindow, QGraphicsItem *parent = 0);
  
private:
  MainWindow *mainWindow;

  /// mouseMoveEvent - moves control point on QGraphicsView, updates control
  /// point position in \var mainWindow->controlPoints and rerenders scene.
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

};

#endif // MOVINGELLIPSEITEM_H
