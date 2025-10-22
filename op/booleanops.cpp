#include "booleanops.h"
#include "clipper2/clipper.h"
#include <algorithm>
#include <limits>

namespace BooleanOps {

// Scale factor for Clipper2 integer coordinates
// Determined by tolerance precision
inline double getScaleFactorForTolerance(const Tolerance& tol) {
    return tol.getScaleFactor();
}

// ==============================================================================
// Conversion Functions - Qt <-> BooleanOps format
// ==============================================================================

// Clean up degenerate points in a loop based on tolerance
Loop cleanupLoop(const Loop& loop, const Tolerance& tol) {
    if (loop.size() < 3) {
        return loop;
    }
    
    Loop result;
    result.reserve(loop.size());
    
    for (const auto& point : loop) {
        // Skip points that are too close to the last point
        if (result.empty() || 
            point.distance(result.back()) >= tol.minSegmentLength) {
            result.push_back(point);
        }
    }
    
    // Check distance from last to first point
    if (result.size() >= 2 && 
        result.back().distance(result.front()) < tol.minSegmentLength) {
        result.pop_back();
    }
    
    return result;
}

PolygonData fromQtPolygon(const Polygon* polygon)
{
    if (!polygon || !polygon->isValid()) {
        return PolygonData();
    }
    
    PolygonData result;
    const QVector<QVector<QPointF>>& qtLoops = polygon->getLoops();
    result.reserve(qtLoops.size());
    
    for (const QVector<QPointF>& qtLoop : qtLoops) {
        Loop loop;
        loop.reserve(qtLoop.size());
        
        for (const QPointF& qp : qtLoop) {
            loop.emplace_back(Point::fromQPointF(qp));
        }
        
        if (!loop.empty()) {
            result.push_back(std::move(loop));
        }
    }
    
    return result;
}

Polygon* toQtPolygon(const PolygonData& data, const QString& name)
{
    if (data.empty() || data[0].size() < 3) {
        return nullptr;
    }
    
    Polygon* polygon = new Polygon(name);
    QVector<QVector<QPointF>> qtLoops;
    qtLoops.reserve(data.size());
    
    for (const Loop& loop : data) {
        QVector<QPointF> qtLoop;
        qtLoop.reserve(loop.size());
        
        for (const Point& p : loop) {
            qtLoop.append(p.toQPointF());
        }
        
        if (!qtLoop.isEmpty()) {
            qtLoops.append(qtLoop);
        }
    }
    
    polygon->setLoops(qtLoops);
    
    return polygon;
}

// ==============================================================================
// Clipper2 Helper Functions
// ==============================================================================

// Convert our Point to Clipper2's Point64 with tolerance scaling
inline Clipper2Lib::Point64 toClipperPoint(const Point& p, double scaleFactor)
{
    return Clipper2Lib::Point64(
        static_cast<int64_t>(std::round(p.x * scaleFactor)),
        static_cast<int64_t>(std::round(p.y * scaleFactor))
    );
}

// Convert Clipper2's Point64 to our Point with inverse scaling
inline Point fromClipperPoint(const Clipper2Lib::Point64& p, double scaleFactor)
{
    return Point(
        static_cast<double>(p.x) / scaleFactor,
        static_cast<double>(p.y) / scaleFactor
    );
}

// Convert Loop to Clipper2's Path64
Clipper2Lib::Path64 toClipperPath(const Loop& loop, double scaleFactor)
{
    Clipper2Lib::Path64 path;
    path.reserve(loop.size());
    
    for (const Point& p : loop) {
        path.push_back(toClipperPoint(p, scaleFactor));
    }
    
    return path;
}

// Convert Clipper2's Path64 to Loop
Loop fromClipperPath(const Clipper2Lib::Path64& path, double scaleFactor)
{
    Loop loop;
    loop.reserve(path.size());
    
    for (const Clipper2Lib::Point64& p : path) {
        loop.emplace_back(fromClipperPoint(p, scaleFactor));
    }
    
    return loop;
}

// Convert PolygonData to Clipper2's Paths64
Clipper2Lib::Paths64 toClipperPaths(const PolygonData& polyData, double scaleFactor)
{
    Clipper2Lib::Paths64 paths;
    paths.reserve(polyData.size());
    
    for (const Loop& loop : polyData) {
        paths.push_back(toClipperPath(loop, scaleFactor));
    }
    
    return paths;
}

// ==============================================================================
// Path Analysis Functions
// ==============================================================================

// Calculate signed area of a path using shoelace formula
double calculatePathArea(const Clipper2Lib::Path64& path)
{
    if (path.size() < 3) {
        return 0.0;
    }
    
    double area = 0.0;
    size_t n = path.size();
    
    for (size_t i = 0; i < n; ++i) {
        size_t j = (i + 1) % n;
        const auto& p1 = path[i];
        const auto& p2 = path[j];
        area += static_cast<double>(p1.x) * static_cast<double>(p2.y);
        area -= static_cast<double>(p2.x) * static_cast<double>(p1.y);
    }
    
    return area / 2.0;
}

// Check if a point is inside a path using ray casting algorithm
bool isPointInsidePath(const Clipper2Lib::Point64& point, const Clipper2Lib::Path64& path)
{
    if (path.size() < 3) {
        return false;
    }
    
    int intersections = 0;
    size_t n = path.size();
    
    for (size_t i = 0; i < n; ++i) {
        size_t j = (i + 1) % n;
        const auto& p1 = path[i];
        const auto& p2 = path[j];
        
        // Check if the ray crosses this edge
        if ((p1.y > point.y) != (p2.y > point.y)) {
            // Calculate x-coordinate of intersection
            int64_t xIntersection = p1.x + 
                (point.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);
            if (point.x < xIntersection) {
                intersections++;
            }
        }
    }
    
    return (intersections % 2) == 1;
}

// ==============================================================================
// Polygon Assembly - Match outer contours with holes
// ==============================================================================

std::vector<PolygonData> assemblePolygonsWithHoles(
    const Clipper2Lib::Paths64& paths,
    double scaleFactor)
{
    if (paths.empty()) {
        return std::vector<PolygonData>();
    }
    
    // Separate outer contours and holes based on area sign
    std::vector<size_t> outerIndices;
    std::vector<size_t> holeIndices;
    
    for (size_t i = 0; i < paths.size(); ++i) {
        double area = calculatePathArea(paths[i]);
        if (area > 0) {
            outerIndices.push_back(i);
        } else if (area < 0) {
            holeIndices.push_back(i);
        }
        // area == 0 means degenerate path, skip it
    }
    
    std::vector<PolygonData> result;
    result.reserve(outerIndices.size());
    
    std::vector<bool> assignedHoles(paths.size(), false);
    
    // For each outer contour, find its holes
    for (size_t outerIdx : outerIndices) {
        PolygonData polygon;
        polygon.push_back(fromClipperPath(paths[outerIdx], scaleFactor));
        
        // Find holes that belong to this outer contour
        for (size_t holeIdx : holeIndices) {
            if (assignedHoles[holeIdx]) {
                continue;
            }
            
            // Check if hole is inside this outer contour
            if (!paths[holeIdx].empty() && 
                isPointInsidePath(paths[holeIdx][0], paths[outerIdx])) {
                polygon.push_back(fromClipperPath(paths[holeIdx], scaleFactor));
                assignedHoles[holeIdx] = true;
            }
        }
        
        result.push_back(std::move(polygon));
    }
    
    return result;
}

// ==============================================================================
// Main Boolean Operation Function
// ==============================================================================

std::vector<PolygonData> performOperation(
    const PolygonData& poly1,
    const PolygonData& poly2,
    Operation op,
    const Tolerance& tolerance)
{
    // Validate input
    if (poly1.empty() || poly2.empty()) {
        return std::vector<PolygonData>();
    }
    
    if (poly1[0].empty() || poly2[0].empty()) {
        return std::vector<PolygonData>();
    }
    
    // Get scale factor based on tolerance
    double scaleFactor = tolerance.getScaleFactor();
    
    // Convert to Clipper2 format
    Clipper2Lib::Paths64 subject = toClipperPaths(poly1, scaleFactor);
    Clipper2Lib::Paths64 clip = toClipperPaths(poly2, scaleFactor);
    
    // Determine clip type
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
    
    // Execute boolean operation using Clipper2
    Clipper2Lib::Clipper64 clipper;
    clipper.AddSubject(subject);
    clipper.AddClip(clip);
    
    Clipper2Lib::Paths64 solution;
    clipper.Execute(clipType, Clipper2Lib::FillRule::NonZero, solution);
    
    // Convert back to our format with proper hole association
    auto result = assemblePolygonsWithHoles(solution, scaleFactor);
    
    // Clean up degenerate geometry based on tolerance
    for (auto& polygon : result) {
        for (auto& loop : polygon) {
            loop = cleanupLoop(loop, tolerance);
        }
    }
    
    return result;
}

} // namespace BooleanOps
