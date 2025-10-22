#ifndef BOOLEANOPS_H
#define BOOLEANOPS_H

#include "../polygon.h"
#include <vector>
#include <memory>
#include <QString>
#include <QPointF>

namespace BooleanOps {

// 操作类型
enum class Operation {
    Union,
    Intersection,
    Difference
};

// 点结构
struct Point {
    double x;
    double y;
    
    Point() : x(0), y(0) {}
    Point(double x_, double y_) : x(x_), y(y_) {}
    
    static Point fromQPointF(const QPointF& qp) {
        return Point(qp.x(), qp.y());
    }
    
    QPointF toQPointF() const {
        return QPointF(x, y);
    }
    
    Point operator+(const Point& other) const {
        return Point(x + other.x, y + other.y);
    }
    
    Point operator-(const Point& other) const {
        return Point(x - other.x, y - other.y);
    }
    
    Point operator*(double scalar) const {
        return Point(x * scalar, y * scalar);
    }
    
    double length() const {
        return std::sqrt(x * x + y * y);
    }
    
    bool operator==(const Point& other) const {
        return std::abs(x - other.x) < 1e-10 && std::abs(y - other.y) < 1e-10;
    }
};

// 循环（外轮廓或孔）
using Loop = std::vector<Point>;

// 多边形数据（第一个是外轮廓，后续是孔）
using PolygonData = std::vector<Loop>;

// 转换函数
PolygonData fromQtPolygon(const Polygon* polygon);
Polygon* toQtPolygon(const PolygonData& data, const QString& name);

// 主布尔运算函数（使用 Clipper2）
// 注意：返回多个多边形的列表
std::vector<PolygonData> performOperation(
    const PolygonData& poly1,
    const PolygonData& poly2,
    Operation op);

} // namespace BooleanOps

#endif // BOOLEANOPS_H
