#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QHash>
#include <QTimer>
#include "bezierinterpolator.h"

class MovingEllipseItem;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private slots:
  // Slots for ui (bells and whistles).
  void on_startStopButton_clicked();
  void on_checkBox_stateChanged(int arg1);
  void on_checkBox_2_stateChanged(int arg1);
  void on_checkBox_3_stateChanged(int arg1);
  void on_checkBox_4_stateChanged(int arg1);
  void on_checkBox_5_stateChanged(int arg1);
  void on_RandomButton_clicked();
  void on_AddPointButton_clicked();
  void on_DelPointButton_clicked();
  void on_AntialiasingSlider_sliderMoved(int position);
  void on_SpeedSlider_sliderMoved(int position);

  /// updateView - calculate content of the scene based on control points and
  /// show it in graphicsView.
  void updateView(QPointF *skipPoint = 0);

  /// moveCurve - moves control points according to its speed and updates view.
  void moveCurve();

  /// updateFPS - show FPS in gui.
  void updateFPS();

  void on_AntialiasingSlider_valueChanged(int value);

  void on_SpeedSlider_valueChanged(int value);

  void on_horizontalSlider_sliderMoved(int position);

  void on_horizontalSlider_valueChanged(int value);

private:
  // Crunch. After moving of control point scene must be rerendered and I am too
  // lazy to create public function for it.
  friend class MovingEllipseItem;

  // User interface.
  Ui::MainWindow *ui;

  // Main scene for spline visualization.
  QGraphicsScene *scene;

  QTimer *animationTimer; // Control points will be moved by this timer.
  QTimer *fpsTimer; // On this timer fpsLabel is updated.
  unsigned framesNumber; // Number of frames rendered so far.

  // Speed of control point is multiplied by this value before moving.
  double speedMultiplicator;

  QVector<QPointF*> controlPoints;
  QVector<qreal> knotVector;
  QPolygonF boorNetPoints;
  QPolygonF interpolatedPoints;

  // Mapping of items on the scene to points in \var controlPoints.
  QHash<MovingEllipseItem*, QPointF*> itemToPoint;

  QVector<QPointF> controlPointsSpeed;

  // Object with interface to boor net calculator and Bezier interpolation.
  BezierInterpolator bezierInterpolator;

  int pointsNumber;

  /// showRandomSpline - generate random control points and show them.
  void showRandomSpline();

  /// interpolateCurve - calculate new control points with de Boor algorithm,
  /// break curve into multiple Bezier curves and interpolate each Bezier curve.
  void interpolateCurve();

  /// clearPoints - delete all control points properly.
  void clearPoints();

  /// fillKnotVector - fill \var knotVector with knots for uniform cubic
  /// B-spline that passes through endpoints.
  void fillKnotVector();

  /// addControlPoint - adds control point within borders of \var graphicsView.
  void addControlPoint();

  struct DisplaySettings {
    DisplaySettings() : showInterpolatedPoints(false), showControlPoints(true),
      showBoorPoints(false), showControlLines(true), showBoorLines(false) {}

    bool showInterpolatedPoints;
    bool showControlPoints;
    bool showBoorPoints;
    bool showControlLines;
    bool showBoorLines;
  } displaySettings;
};

#endif // MAINWINDOW_H
