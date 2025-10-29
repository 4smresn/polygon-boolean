#ifndef BOOLEANOPS_H
#define BOOLEANOPS_H

#include "../polygon.h"
#include <vector>
#include <memory>
#include <QString>
#include <QPointF>
#include <cmath>


namespace BooleanOps {

using OpPoint = std::pair<double, double>;
// Loop/Path type - sequence of points
using OpLoop = std::vector<OpPoint>;

// Polygon type - first loop is outer contour, rest are holes
using OpPolygon = std::vector<OpLoop>;
using OpPolygons = std::vector<OpPolygon>;

// Conversion functions between Qt and BooleanOps formats
OpPolygon fromQtPolygon(const Polygon* polygon);
Polygon* toQtPolygon(const OpPolygon& data, const QString& name);

// Main boolean operation function with tolerance control
// Returns a vector of result polygons (each can have holes)
// tolerance: Precision/tolerance settings for the operation
OpPolygons performOperation(
    const OpPolygon& poly1,
    const OpPolygon& poly2,
    int op,
    const double& tolerance = 1e-9);

} // namespace BooleanOps

#endif // BOOLEANOPS_H
