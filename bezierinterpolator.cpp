#include "bezierinterpolator.h"
#include <QtCore/qmath.h>
#include <QHash>

const unsigned BezierInterpolator::curveRecursionLimit = 32;
const double BezierInterpolator::curveCollinearityEpsilon = 1e-30;
const double BezierInterpolator::curveAngleToleranceEpsilon = 0.01;
const double BezierInterpolator::AngleTolerance = 0.0;
const double BezierInterpolator::CuspLimit = 0.0;

BezierInterpolator::BezierInterpolator() : DistanceTolerance(0.5) {}

// InterpolateBezier - interpolates points with bezier curve.
// Algorithm is based on article "Adaptive Subdivision of Bezier Curves" by
// Maxim Shemanarev.
// http://www.antigrain.com/research/adaptive_bezier/index.html
void BezierInterpolator::InterpolateBezier(double x1, double y1,
                                           double x2, double y2,
                                           double x3, double y3,
                                           double x4, double y4,
                                           QPolygonF &interpolatedPoints,
                                           unsigned level) const {
  if(level > curveRecursionLimit) {
    return;
  }

  // Calculate all the mid-points of the line segments
  double x12   = (x1 + x2) / 2;
  double y12   = (y1 + y2) / 2;
  double x23   = (x2 + x3) / 2;
  double y23   = (y2 + y3) / 2;
  double x34   = (x3 + x4) / 2;
  double y34   = (y3 + y4) / 2;
  double x123  = (x12 + x23) / 2;
  double y123  = (y12 + y23) / 2;
  double x234  = (x23 + x34) / 2;
  double y234  = (y23 + y34) / 2;
  double x1234 = (x123 + x234) / 2;
  double y1234 = (y123 + y234) / 2;

  if(level > 0) {
    // Enforce subdivision first time

    // Try to approximate the full cubic curve by a single straight line
    double dx = x4-x1;
    double dy = y4-y1;

    double d2 = qAbs(((x2 - x4) * dy - (y2 - y4) * dx));
    double d3 = qAbs(((x3 - x4) * dy - (y3 - y4) * dx));

    double da1, da2;

    if(d2 > curveCollinearityEpsilon && d3 > curveCollinearityEpsilon) {
      // Regular care
      if((d2 + d3)*(d2 + d3) <= DistanceTolerance * (dx*dx + dy*dy)) {
        // If the curvature doesn't exceed the distance_tolerance value
        // we tend to finish subdivisions.
        if(AngleTolerance < curveAngleToleranceEpsilon) {
          interpolatedPoints.push_back(QPointF(x1234, y1234));
          return;
        }

        // Angle & Cusp Condition
        double a23 = qAtan2(y3 - y2, x3 - x2);
        da1 = fabs(a23 - qAtan2(y2 - y1, x2 - x1));
        da2 = fabs(qAtan2(y4 - y3, x4 - x3) - a23);
        if(da1 >= M_PI) da1 = 2*M_PI - da1;
        if(da2 >= M_PI) da2 = 2*M_PI - da2;

        if(da1 + da2 < AngleTolerance) {
          // Finally we can stop the recursion
          interpolatedPoints.push_back(QPointF(x1234, y1234));
          return;
        }

        if(CuspLimit != 0.0) {
          if(da1 > CuspLimit) {
            interpolatedPoints.push_back(QPointF(x2, y2));
            return;
          }

          if(da2 > CuspLimit) {
            interpolatedPoints.push_back(QPointF(x3, y3));
            return;
          }
        }
      }
    } else {
      if(d2 > curveCollinearityEpsilon) {
        // p1,p3,p4 are collinear, p2 is considerable
        if(d2 * d2 <= DistanceTolerance * (dx*dx + dy*dy)) {
          if(AngleTolerance < curveAngleToleranceEpsilon) {
            interpolatedPoints.push_back(QPointF(x1234, y1234));
            return;
          }

          // Angle Condition
          da1 = fabs(qAtan2(y3 - y2, x3 - x2) - qAtan2(y2 - y1, x2 - x1));
          if(da1 >= M_PI)
            da1 = 2*M_PI - da1;

          if(da1 < AngleTolerance) {
            interpolatedPoints.push_back(QPointF(x2, y2));
            interpolatedPoints.push_back(QPointF(x3, y3));
            return;
          }

          if(CuspLimit != 0.0) {
            if(da1 > CuspLimit) {
              interpolatedPoints.push_back(QPointF(x2, y2));
              return;
            }
          }
        }
      } else if(d3 > curveCollinearityEpsilon) {
        // p1,p2,p4 are collinear, p3 is considerable
        if(d3 * d3 <= DistanceTolerance * (dx*dx + dy*dy)) {
          if(AngleTolerance < curveAngleToleranceEpsilon) {
            interpolatedPoints.push_back(QPointF(x1234, y1234));
            return;
          }

          // Angle Condition
          da1 = fabs(qAtan2(y4 - y3, x4 - x3) - qAtan2(y3 - y2, x3 - x2));
          if(da1 >= M_PI) da1 = 2*M_PI - da1;

          if(da1 < AngleTolerance) {
            interpolatedPoints.push_back(QPointF(x2, y2));
            interpolatedPoints.push_back(QPointF(x3, y3));
            return;
          }

          if(CuspLimit != 0.0) {
            if(da1 > CuspLimit) {
              interpolatedPoints.push_back(QPointF(x3, y3));
              return;
            }
          }
        }
      } else {
        // Collinear case
        dx = x1234 - (x1 + x4) / 2;
        dy = y1234 - (y1 + y4) / 2;
        if(dx*dx + dy*dy <= DistanceTolerance) {
          interpolatedPoints.push_back(QPointF(x1234, y1234));
          return;
        }
      }
    }
  }

  // Continue subdivision
  InterpolateBezier(x1, y1, x12, y12, x123, y123, x1234, y1234,
                    interpolatedPoints, level + 1);
  InterpolateBezier(x1234, y1234, x234, y234, x34, y34, x4, y4,
                    interpolatedPoints, level + 1);
}

void BezierInterpolator::InterpolateBezier(const QPointF &p1, const QPointF &p2,
                                           const QPointF &p3, const QPointF &p4,
                                           QPolygonF &interpolatedPoints,
                                           unsigned level) const {
  InterpolateBezier(p1.x(), p1.y(), p2.x(), p2.y(), p3.x(), p3.y(), p4.x(),
                    p4.y(), interpolatedPoints, level);
}

// CalculateBoorNet - inserts new control points with de Boor algorithm for
// transformation of B-spline into composite Bezier curve.
void BezierInterpolator::CalculateBoorNet(const QVector<QPointF *> &controlPoints,
    const QVector<qreal> &knotVector,
    QPolygonF &boorNetPoints) const {
  Q_ASSERT(controlPoints.size() > 2);
  Q_ASSERT(knotVector.size() > 4);
  // We draw uniform cubic B-spline that passes through endpoints, so we assume
  // that multiplicity of first and last knot is 4 and 1 for knots between.

  QVector<qreal> newKnotVector = knotVector;
  boorNetPoints.clear();
  for (int counter = 0; counter < controlPoints.size(); ++counter)
    boorNetPoints.push_back(*controlPoints[counter]);

  // Insert every middle knot 2 times to increase its multiplicity from 1 to 3.
  const int curveDegree = 3;
  const int increaseMultiplicity = 2;

  for (int knotCounter = 4; knotCounter < newKnotVector.size() - 4;
       knotCounter += 3) {
    QHash< int, QHash<int, QPointF> > tempPoints;
    for (int counter = knotCounter - curveDegree; counter <= knotCounter;
         ++counter)
      tempPoints[counter][0] = boorNetPoints[counter];

    for (int insertCounter = 1; insertCounter <= increaseMultiplicity;
         ++insertCounter)
      for (int i = knotCounter - curveDegree + insertCounter; i < knotCounter;
           ++i) {
        double coeff = (newKnotVector[knotCounter] - newKnotVector[i]) /
            (newKnotVector[i + curveDegree - insertCounter + 1] - newKnotVector[i]);
        QPointF newPoint = (1.0 - coeff) * tempPoints[i - 1][insertCounter - 1] +
                           coeff * tempPoints[i][insertCounter - 1];
        tempPoints[i][insertCounter] = newPoint;
      }

    for (int counter = 0; counter < increaseMultiplicity; ++counter)
      newKnotVector.insert(knotCounter, newKnotVector[knotCounter]);

    // Fill new control points.
    QPolygonF newBoorNetPoints;
    for (int counter = 0; counter <= knotCounter - curveDegree; ++counter)
      newBoorNetPoints.push_back(boorNetPoints[counter]);

    for (int counter = 1; counter <= increaseMultiplicity; ++counter) {
      QPointF &newP = tempPoints[knotCounter - curveDegree + counter][counter];
      newBoorNetPoints.push_back(newP);
    }

    for (int counter = -curveDegree + increaseMultiplicity + 1; counter <= -1;
         ++counter) {
      QPointF &newP = tempPoints[knotCounter + counter][increaseMultiplicity];
      newBoorNetPoints.push_back(newP);
    }

    for (int counter = increaseMultiplicity - 1; counter >= 1; --counter)
      newBoorNetPoints.push_back(tempPoints[knotCounter - 1][counter]);

    for (int counter = knotCounter - 1; counter < boorNetPoints.size(); ++counter)
      newBoorNetPoints.push_back(boorNetPoints[counter]);

    boorNetPoints = newBoorNetPoints;
  }
}

void BezierInterpolator::SetDistanceTolerance(double value) {
  DistanceTolerance = value;
}
