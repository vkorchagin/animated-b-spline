#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "movingellipseitem.h"
#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow), framesNumber(0),
    speedMultiplicator(1.0), pointsNumber(6) {
  ui->setupUi(this);

  fillKnotVector();

  animationTimer = new QTimer();
  connect(animationTimer, SIGNAL(timeout()), SLOT(moveCurve()));

  fpsTimer = new QTimer();
  connect(fpsTimer, SIGNAL(timeout()), SLOT(updateFPS()));

  scene = new QGraphicsScene();
  ui->graphicsView->setScene(scene);
  showRandomSpline();

  ui->ControlPointsLabel->setText(QString::number(pointsNumber));
}

MainWindow::~MainWindow() {
  delete ui;
  delete animationTimer;
  delete fpsTimer;
  delete scene;
  clearPoints();
}

/// fillKnotVector - fill \var knotVector with knots for uniform cubic B-spline
/// that passes through endpoints.
void MainWindow::fillKnotVector() {
  int middleKnotNumber = pointsNumber - 4;
  knotVector.clear();
  for (int counter = 0; counter < 4; ++counter)
    knotVector.push_back(0.0);
  for (int counter = 1; counter <= middleKnotNumber; ++counter)
    knotVector.push_back(1.0 / (middleKnotNumber + 1) * counter);
  for (int counter = 0; counter < 4; ++counter)
    knotVector.push_back(1.0);
}

/// interpolateCurve - calculate new control points with de Boor algorithm,
/// break curve into multiple Bezier curves and interpolate each Bezier curve.
void MainWindow::interpolateCurve() {
  interpolatedPoints.clear();
  bezierInterpolator.CalculateBoorNet(controlPoints, knotVector, boorNetPoints);
  interpolatedPoints.push_back(*(controlPoints.first()));
  for (int counter = 0; counter < boorNetPoints.size() - 3; counter += 3)
    bezierInterpolator.InterpolateBezier(boorNetPoints[counter],
                                         boorNetPoints[counter + 1],
                                         boorNetPoints[counter + 2],
                                         boorNetPoints[counter + 3],
                                         interpolatedPoints);
  interpolatedPoints.push_back(*(controlPoints.last()));
}

/// clearPoints - delete all control points properly.
void MainWindow::clearPoints() {
  for (int counter = 0; counter < controlPoints.size(); ++counter)
    delete controlPoints[counter];
  controlPoints.clear();
}

/// addControlPoint - adds control point within borders of \var graphicsView.
void MainWindow::addControlPoint()
{
  int x_border = ui->graphicsView->width();;
  int y_border = ui->graphicsView->height();
  int x = qrand() % x_border;
  int y = qrand() % y_border;
  controlPoints.push_back(new QPointF(x, y));
}

/// showRandomSpline - generate random control points and show them.
void MainWindow::showRandomSpline() {
  scene->clear();
  clearPoints();  

  for (int counter = 0; counter < pointsNumber; ++counter) {
    addControlPoint();
  }

  updateView();
}

void MainWindow::on_startStopButton_clicked() {
  showRandomSpline();
}

/// updateView - calculate content of the scene based on control points and show
/// it in graphicsView.
void MainWindow::updateView(QPointF *skipPoint) {
  QTime t;
  t.start();
  interpolateCurve();

  // Show interpolated curve.
  for (QPolygonF::iterator pointIt = interpolatedPoints.begin(),
       pointEnd = interpolatedPoints.end(); pointIt != pointEnd; ++pointIt) {
    if (pointIt != interpolatedPoints.end() - 1)
     scene->addLine(QLineF(*pointIt, *(pointIt + 1)), QPen("black"));
    if (displaySettings.showInterpolatedPoints)
      scene->addEllipse(pointIt->x() - 2, pointIt->y() - 2, 4, 4, QPen("black"),
                        QBrush("black"));
  }
  // Show control points.
  for (QVector<QPointF*>::iterator pointIt = controlPoints.begin(),
       pointEnd = controlPoints.end(); pointIt != pointEnd; ++pointIt) {
    if (displaySettings.showControlLines && pointIt != controlPoints.end() - 1)
      scene->addLine(QLineF(**pointIt, **(pointIt + 1)), QPen("blue"));
    if ((skipPoint && skipPoint == *pointIt) ||
        !displaySettings.showControlPoints) {
      continue;
    }
    // On Android control points must be larger for moving them with fingers.
#ifdef Q_OS_ANDROID
    const int controlPointSize = 50;
#else
    const int controlPointSize = 10;
#endif
    MovingEllipseItem *pointItem = new MovingEllipseItem(
                                     (*pointIt)->x() - controlPointSize / 2,
                                     (*pointIt)->y() - controlPointSize / 2,
                                     controlPointSize, controlPointSize, this);
    itemToPoint[pointItem] = *pointIt;
    pointItem->setBrush(QBrush("blue"));
    pointItem->setFlag(QGraphicsItem::ItemIsMovable, true);
    scene->addItem(pointItem);
  }
  // Show boor net points.
  for (QPolygonF::iterator pointIt = boorNetPoints.begin(),
       pointEnd = boorNetPoints.end(); pointIt != pointEnd; ++pointIt) {
    if (displaySettings.showBoorLines && pointIt != boorNetPoints.end() - 1)
      scene->addLine(QLineF(*pointIt, *(pointIt + 1)), QPen("red"));
    if (displaySettings.showBoorPoints)
      scene->addEllipse(pointIt->x() - 2, pointIt->y() - 2, 4, 4, QPen("green"),
                        QBrush("green"));
  }

  ui->InterpolatedPointsLabel->setText(
        QString::number(interpolatedPoints.size()));
  ++framesNumber;
}

void MainWindow::on_checkBox_stateChanged(int arg1) {
  displaySettings.showInterpolatedPoints = arg1;
  scene->clear();
  updateView();
}

void MainWindow::on_checkBox_2_stateChanged(int arg1) {
  displaySettings.showControlPoints = arg1;
  scene->clear();
  updateView();
}

void MainWindow::on_checkBox_3_stateChanged(int arg1) {
    displaySettings.showBoorPoints = arg1;
    scene->clear();
    updateView();
}

void MainWindow::on_checkBox_4_stateChanged(int arg1) {
    displaySettings.showControlLines = arg1;
    scene->clear();
    updateView();
}

void MainWindow::on_checkBox_5_stateChanged(int arg1) {
    displaySettings.showBoorLines = arg1;
    scene->clear();
    updateView();
}

/// moveCurve - moves control points according to its speed and updates view.
void MainWindow::moveCurve() {
  if (controlPointsSpeed.size() != controlPoints.size()) {
    // Randomly create speed.
    controlPointsSpeed.clear();
    const int speedLimit = 5;
    for (int counter = 0; counter < controlPoints.size(); ++counter) {
      controlPointsSpeed.push_back(QPointF(qrand() % speedLimit,
                                           qrand() % speedLimit));
    }
  }

  // Move each control point.
  for (int counter = 0; counter < controlPoints.size(); ++counter) {
    QPointF &point = *controlPoints[counter];
    QPointF &speed = controlPointsSpeed[counter];
    double xSpeed = speedMultiplicator * speed.x();
    double ySpeed = speedMultiplicator * speed.y();
    point.setX(point.x() + xSpeed);
    point.setY(point.y() + ySpeed);
    if (point.x() < 0 || point.x() > ui->graphicsView->width()) {
      point.setX(point.x() - 2 * xSpeed);
      speed.setX(-speed.x());
    }
    if (point.y() < 0 || point.y() > ui->graphicsView->height()) {
      point.setY((point.y() - 2 * ySpeed));
      speed.setY(-speed.y());
    }
    if (point.x() < 0 || point.x() > ui->graphicsView->width() ||
        point.y() < 0 || point.y() > ui->graphicsView->height()) {
      // Looks like view was resized and point is out of view.
      point.setX(qrand() % ui->graphicsView->width());
      point.setY(qrand() % ui->graphicsView->height());
    }
  }

  scene->clear();
  updateView();
}

void MainWindow::on_RandomButton_clicked() {
  if (animationTimer->isActive()) {
    animationTimer->stop();
    fpsTimer->stop();
  } else {
    animationTimer->start(30);
    fpsTimer->start(1000);
  }
}

void MainWindow::on_AddPointButton_clicked() {
  ++pointsNumber;
  addControlPoint();
  fillKnotVector();
  scene->clear();
  ui->ControlPointsLabel->setText(QString::number(pointsNumber));
  updateView();
}

void MainWindow::on_DelPointButton_clicked() {
  if (pointsNumber < 4)
    return;
  --pointsNumber;
  delete controlPoints.last();
  controlPoints.pop_back();
  if (!controlPointsSpeed.empty())
    controlPointsSpeed.pop_back();
  fillKnotVector();
  scene->clear();
  ui->ControlPointsLabel->setText(QString::number(pointsNumber));
  updateView();
}

/// updateFPS - show FPS in gui.
void MainWindow::updateFPS() {
  ui->fpsLabel_2->setText(QString::number(framesNumber));
  framesNumber = 0;
}

void MainWindow::on_AntialiasingSlider_sliderMoved(int position) {
  Q_ASSERT(position >= 0 && position <= 2);
  switch (position) {
    case 0:
      // Disable antialiasing.
      ui->graphicsView->setRenderHint(QPainter::Antialiasing, false);
      ui->graphicsView->setRenderHint(QPainter::HighQualityAntialiasing, false);
      ui->AntialiasingLabel->setText("Antialiasing: None");
      break;
    case 1:
      // Enable antialiasing.
      ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);
      ui->graphicsView->setRenderHint(QPainter::HighQualityAntialiasing, false);
      ui->AntialiasingLabel->setText("Antialiasing: Medium");
      break;
    case 2:
      // Maximum quality - maximum lags.
      ui->graphicsView->setRenderHint(QPainter::Antialiasing, true);
      ui->graphicsView->setRenderHint(QPainter::HighQualityAntialiasing, true);
      ui->AntialiasingLabel->setText("Antialiasing: High");
      break;
  }
}

void MainWindow::on_SpeedSlider_sliderMoved(int position) {
  Q_ASSERT(position >= 1 && position <= 20);
  speedMultiplicator = (double) position / 10.0;
  ui->SpeedLabel->setText("Speed: " +
                          QString::number(speedMultiplicator, 'f', 1) + "x");
}

void MainWindow::on_AntialiasingSlider_valueChanged(int value) {
  on_AntialiasingSlider_sliderMoved(value);
}

void MainWindow::on_SpeedSlider_valueChanged(int value) {
  on_SpeedSlider_sliderMoved(value);
}

void MainWindow::on_horizontalSlider_sliderMoved(int position) {
  int max = ui->horizontalSlider->maximum();
  double distanceTolerance = (double) (max - position) + 0.5;
  bezierInterpolator.SetDistanceTolerance(distanceTolerance);

  QString prefix = "Interp Quality: ";
  QString postfix;
  // Divide quality into 4 ranges: Best, Good, Bad, Worst
  if (position >= 0 && position < max / 4)
    postfix = "Worst";
  else if (position >= max / 4 && position < max / 2)
    postfix = "Bad";
  else if (position >= max / 2 && position < 3 * max / 4)
    postfix = "Good";
  else
    postfix = "Best";

  ui->QualityLabel->setText(prefix + postfix);

  scene->clear();
  updateView();
}

void MainWindow::on_horizontalSlider_valueChanged(int value) {
  on_horizontalSlider_sliderMoved(value);
}
