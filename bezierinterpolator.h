#ifndef BEZIERINTERPOLATOR_H
#define BEZIERINTERPOLATOR_H

#include <QPolygonF>
#include <QPointF>

class BezierInterpolator {
public:
  BezierInterpolator();

  // InterpolateBezier - interpolates points with bezier curve.
  // Algorithm is based on article "Adaptive Subdivision of Bezier Curves" by
  // Maxim Shemanarev.
  // http://www.antigrain.com/research/adaptive_bezier/index.html
  void InterpolateBezier(double x1, double y1, double x2, double y2, double x3,
                         double y3, double x4, double y4,
                         QPolygonF &interpolatedPoints,
                         unsigned level = 0) const;

  void InterpolateBezier(const QPointF &p1, const QPointF &p2,
                         const QPointF &p3, const QPointF &p4,
                         QPolygonF &interpolatedPoints,
                         unsigned level = 0) const;


  // CalculateBoorNet - inserts new control points with de Boor algorithm for
  // transformation of B-spline into composite Bezier curve.
  void CalculateBoorNet(const QVector<QPointF*> &controlPoints,
                        const QVector<qreal> &knotVector,
                        QPolygonF &boorNetPoints) const;

  void SetDistanceTolerance(double value);

private:
  // Casteljau algorithm (interpolating Bezier curve) parameters.
  static const unsigned curveRecursionLimit;
  static const double curveCollinearityEpsilon;
  static const double curveAngleToleranceEpsilon;
  static const double AngleTolerance;  
  static const double CuspLimit;

  double DistanceTolerance;
};

#endif // BEZIERINTERPOLATOR_H
