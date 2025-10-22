#ifndef BOOLEANOPS_H
#define BOOLEANOPS_H

#include "../polygon.h"
#include <vector>
#include <memory>
#include <QString>
#include <QPointF>
#include <cmath>

namespace BooleanOps {

// Enum for boolean operation types
enum class Operation {
    Union,           // Union of two polygons
    Intersection,    // Intersection of two polygons
    Difference       // Difference of two polygons (A - B)
};

// Tolerance/precision settings for boolean operations
// Corresponds to Clipper2's precision model
struct Tolerance {
    // Precision level (decimal places)
    // Similar to Clipper2's precision parameter
    // 0 = integer, 1 = 0.1, 2 = 0.01, etc.
    int precision = 6;
    
    // Point merge tolerance for degenerate points
    // Points closer than this distance are considered the same
    double pointMergeTolerance = 1e-10;
    
    // Minimum segment length (degenerate segments are removed)
    double minSegmentLength = 1e-10;
    
    // Get the scale factor for this precision
    double getScaleFactor() const {
        return std::pow(10.0, precision);
    }
    
    // Convert to integer coordinate with this precision
    int64_t toInteger(double value) const {
        return static_cast<int64_t>(std::round(value * getScaleFactor()));
    }
    
    // Convert from integer coordinate to double
    double toDouble(int64_t value) const {
        return static_cast<double>(value) / getScaleFactor();
    }
};

// Point structure with vector operations
struct Point {
    double x;
    double y;
    
    Point() : x(0), y(0) {}
    Point(double x_, double y_) : x(x_), y(y_) {}
    
    // Conversion functions
    static Point fromQPointF(const QPointF& qp) {
        return Point(qp.x(), qp.y());
    }
    
    QPointF toQPointF() const {
        return QPointF(x, y);
    }
    
    // Vector operations
    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y);
    }
    
    Point operator-(const Point& other) const {
        return Point(x - other.x, y - other.y);
    }
    
    Point operator*(double scalar) const {
        return Point(x * scalar, y * scalar);
    }
    
    double dot(const Point& other) const {
        return x * other.x + y * other.y;
    }
    
    double cross(const Point& other) const {
        return x * other.y - y * other.x;
    }
    
    double length() const {
        return std::sqrt(x * x + y * y);
    }
    
    double distance(const Point& other) const {
        return (*this - other).length();
    }
    
    bool operator==(const Point& other) const {
        constexpr double EPSILON = 1e-10;
        return std::abs(x - other.x) < EPSILON && std::abs(y - other.y) < EPSILON;
    }
    
    // Compare with custom tolerance
    bool equals(const Point& other, double tolerance) const {
        return distance(other) < tolerance;
    }
};

// Loop/Path type - sequence of points
using Loop = std::vector<Point>;

// Polygon type - first loop is outer contour, rest are holes
using PolygonData = std::vector<Loop>;

// Conversion functions between Qt and BooleanOps formats
PolygonData fromQtPolygon(const Polygon* polygon);
Polygon* toQtPolygon(const PolygonData& data, const QString& name);

// Main boolean operation function with tolerance control
// Returns a vector of result polygons (each can have holes)
// tolerance: Precision/tolerance settings for the operation
std::vector<PolygonData> performOperation(
    const PolygonData& poly1,
    const PolygonData& poly2,
    Operation op,
    const Tolerance& tolerance = Tolerance());

} // namespace BooleanOps

#endif // BOOLEANOPS_H
