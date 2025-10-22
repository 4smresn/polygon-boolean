#include "booleanops.h"
#include "clipper2/clipper.h"
#include <cmath>
#include <algorithm>
#include <limits>

namespace BooleanOps {

// 缩放因子，Clipper2 使用整数坐标
constexpr double SCALE_FACTOR = 1000000.0;

// ============================================================================
// 转换函数
// ============================================================================

// 从 Qt Polygon 转换到标准库格式
PolygonData fromQtPolygon(const Polygon* polygon)
{
    if (!polygon || !polygon->isValid()) {
        return PolygonData();
    }
    
    PolygonData result;
    const QVector<QVector<QPointF>>& qtLoops = polygon->getLoops();
    
    for (const QVector<QPointF>& qtLoop : qtLoops) {
        Loop loop;
        loop.reserve(qtLoop.size());
        
        for (const QPointF& qp : qtLoop) {
            loop.push_back(Point::fromQPointF(qp));
        }
        
        result.push_back(loop);
    }
    
    return result;
}

// 转换回 Qt Polygon
Polygon* toQtPolygon(const PolygonData& data, const QString& name)
{
    if (data.empty() || data[0].size() < 3) {
        return nullptr;
    }
    
    Polygon* polygon = new Polygon(name);
    
    QVector<QVector<QPointF>> qtLoops;
    
    for (const Loop& loop : data) {
        QVector<QPointF> qtLoop;
        qtLoop.reserve(loop.size());
        
        for (const Point& p : loop) {
            qtLoop.append(p.toQPointF());
        }
        
        qtLoops.append(qtLoop);
    }
    
    polygon->setLoops(qtLoops);
    
    return polygon;
}

// ============================================================================
// Clipper2 辅助函数
// ============================================================================

// 将我们的 Point 转换为 Clipper2 的 Point64
Clipper2Lib::Point64 toClipperPoint(const Point& p)
{
    return Clipper2Lib::Point64(
        static_cast<int64_t>(p.x * SCALE_FACTOR),
        static_cast<int64_t>(p.y * SCALE_FACTOR)
    );
}

// 将 Clipper2 的 Point64 转换为我们的 Point
Point fromClipperPoint(const Clipper2Lib::Point64& p)
{
    return Point(
        static_cast<double>(p.x) / SCALE_FACTOR,
        static_cast<double>(p.y) / SCALE_FACTOR
    );
}

// 将 Loop 转换为 Clipper2 的 Path64
Clipper2Lib::Path64 toClipperPath(const Loop& loop)
{
    Clipper2Lib::Path64 path;
    path.reserve(loop.size());
    
    for (const Point& p : loop) {
        path.push_back(toClipperPoint(p));
    }
    
    return path;
}

// 将 Clipper2 的 Path64 转换为 Loop
Loop fromClipperPath(const Clipper2Lib::Path64& path)
{
    Loop loop;
    loop.reserve(path.size());
    
    for (const Clipper2Lib::Point64& p : path) {
        loop.push_back(fromClipperPoint(p));
    }
    
    return loop;
}

// 将 PolygonData 转换为 Clipper2 的 Paths64
Clipper2Lib::Paths64 toClipperPaths(const PolygonData& polyData)
{
    Clipper2Lib::Paths64 paths;
    paths.reserve(polyData.size());
    
    for (const Loop& loop : polyData) {
        paths.push_back(toClipperPath(loop));
    }
    
    return paths;
}

// 计算多边形面积（带符号）
double pathArea(const Clipper2Lib::Path64& path)
{
    double area = 0.0;
    size_t n = path.size();
    
    for (size_t i = 0; i < n; ++i) {
        size_t j = (i + 1) % n;
        area += static_cast<double>(path[i].x) * static_cast<double>(path[j].y);
        area -= static_cast<double>(path[j].x) * static_cast<double>(path[i].y);
    }
    
    return area / 2.0;
}

// 判断点是否在路径内
bool isPointInPath(const Clipper2Lib::Point64& point, const Clipper2Lib::Path64& path)
{
    int intersections = 0;
    size_t n = path.size();
    
    for (size_t i = 0; i < n; ++i) {
        size_t j = (i + 1) % n;
        const auto& p1 = path[i];
        const auto& p2 = path[j];
        
        if ((p1.y > point.y) != (p2.y > point.y)) {
            int64_t xIntersection = (p2.x - p1.x) * (point.y - p1.y) / (p2.y - p1.y) + p1.x;
            if (point.x < xIntersection) {
                intersections++;
            }
        }
    }
    
    return (intersections % 2) == 1;
}

// 将 Clipper2 的 Paths64 转换为多个 PolygonData
// 需要识别哪些是外轮廓，哪些是孔
std::vector<PolygonData> fromClipperPathsWithHoles(const Clipper2Lib::Paths64& paths)
{
    if (paths.empty()) {
        return std::vector<PolygonData>();
    }
    
    // 按面积排序，面积大的是外轮廓
    std::vector<size_t> indices(paths.size());
    for (size_t i = 0; i < paths.size(); ++i) {
        indices[i] = i;
    }
    
    std::sort(indices.begin(), indices.end(), [&paths](size_t a, size_t b) {
        return std::abs(pathArea(paths[a])) > std::abs(pathArea(paths[b]));
    });
    
    std::vector<PolygonData> result;
    std::vector<bool> assigned(paths.size(), false);
    
    // 遍历每个路径
    for (size_t idx : indices) {
        if (assigned[idx]) continue;
        
        double area = pathArea(paths[idx]);
        
        // 正面积 = 外轮廓
        if (area > 0) {
            PolygonData polygon;
            polygon.push_back(fromClipperPath(paths[idx]));
            assigned[idx] = true;
            
            // 查找属于这个外轮廓的孔
            for (size_t holeIdx : indices) {
                if (assigned[holeIdx]) continue;
                
                double holeArea = pathArea(paths[holeIdx]);
                
                // 负面积 = 孔
                if (holeArea < 0) {
                    // 检查孔是否在当前外轮廓内
                    if (!paths[holeIdx].empty() && 
                        isPointInPath(paths[holeIdx][0], paths[idx])) {
                        polygon.push_back(fromClipperPath(paths[holeIdx]));
                        assigned[holeIdx] = true;
                    }
                }
            }
            
            result.push_back(polygon);
        }
    }
    
    return result;
}

// ============================================================================
// 主布尔运算函数
// ============================================================================

std::vector<PolygonData> performOperation(
    const PolygonData& poly1,
    const PolygonData& poly2,
    Operation op)
{
    if (poly1.empty() || poly2.empty()) {
        return std::vector<PolygonData>();
    }
    
    // 转换为 Clipper2 格式
    Clipper2Lib::Paths64 subject = toClipperPaths(poly1);
    Clipper2Lib::Paths64 clip = toClipperPaths(poly2);
    
    // 确定裁剪类型
    Clipper2Lib::ClipType clipType;
    switch (op) {
        case Operation::Union:
            clipType = Clipper2Lib::ClipType::Union;
            break;
        case Operation::Intersection:
            clipType = Clipper2Lib::ClipType::Intersection;
            break;
        case Operation::Difference:
            clipType = Clipper2Lib::ClipType::Difference;
            break;
        default:
            return std::vector<PolygonData>();
    }
    
    // 执行布尔运算
    Clipper2Lib::Clipper64 clipper;
    clipper.AddSubject(subject);
    clipper.AddClip(clip);
    
    Clipper2Lib::Paths64 solution;
    clipper.Execute(clipType, Clipper2Lib::FillRule::NonZero, solution);
    
    // 转换回我们的格式，正确处理多个多边形和孔
    std::vector<PolygonData> result = fromClipperPathsWithHoles(solution);
    
    return result;
}

} // namespace BooleanOps
