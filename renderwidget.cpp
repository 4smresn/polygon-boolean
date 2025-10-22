#include "renderwidget.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QPolygonF>
#include <QPainterPath>
#include <QResizeEvent>
#include <limits>

RenderWidget::RenderWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(600, 600);
    setStyleSheet("background-color: white;");
}

void RenderWidget::setPolygons(const QVector<Polygon*>& polygons)
{
    m_polygons = polygons;
    m_boundsNeedsUpdate = true;
    
    // Only recalculate bounds and transform when loading new files
    m_cachedBounds = calculateBounds();
    m_cachedTransform = calculateTransform(m_cachedBounds);
    
    update();
}

void RenderWidget::updatePolygonsVisibility(const QVector<Polygon*>& polygons)
{
    m_polygons = polygons;
    // Do NOT recalculate bounds and transform, just update visibility
    update();
}

void RenderWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    // Recalculate transform when window is resized
    if (!m_cachedBounds.isEmpty()) {
        m_cachedTransform = calculateTransform(m_cachedBounds);
    }
}

void RenderWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    if (m_polygons.isEmpty()) {
        return;
    }
    
    if (m_cachedBounds.isEmpty()) {
        return;
    }
    
    // Apply cached transformation
    painter.setTransform(m_cachedTransform);
    
    // Draw all polygons
    for (const Polygon* polygon : m_polygons) {
        if (polygon && polygon->isValid()) {
            drawPolygon(painter, polygon);
        }
    }
}

QTransform RenderWidget::calculateTransform(const QRectF& bounds) const
{
    if (bounds.isEmpty()) {
        return QTransform();
    }
    
    // Calculate scale to fit the window
    double margin = 50.0;
    double scaleX = (width() - 2 * margin) / bounds.width();
    double scaleY = (height() - 2 * margin) / bounds.height();
    double scale = qMin(scaleX, scaleY);
    
    QTransform transform;
    transform.translate(width() / 2.0, height() / 2.0);
    transform.scale(scale, -scale);  // Y-axis inverted, positive upwards
    transform.translate(-bounds.center().x(), -bounds.center().y());
    
    return transform;
}

void RenderWidget::drawPolygon(QPainter& painter, const Polygon* polygon)
{
    const QVector<QVector<QPointF>>& loops = polygon->getLoops();
    if (loops.isEmpty() || loops[0].size() < 3) {
        return;
    }
    
    // Use QPainterPath to support polygons with holes
    QPainterPath path;
    
    // First loop is the outer contour (counter-clockwise)
    const QVector<QPointF>& outerLoop = loops[0];
    if (!outerLoop.isEmpty()) {
        path.moveTo(outerLoop[0]);
        for (int i = 1; i < outerLoop.size(); ++i) {
            path.lineTo(outerLoop[i]);
        }
        path.closeSubpath();
    }
    
    // Remaining loops are holes (clockwise)
    for (int loopIdx = 1; loopIdx < loops.size(); ++loopIdx) {
        const QVector<QPointF>& hole = loops[loopIdx];
        if (hole.size() >= 3) {
            path.moveTo(hole[0]);
            for (int i = 1; i < hole.size(); ++i) {
                path.lineTo(hole[i]);
            }
            path.closeSubpath();
        }
    }
    
    // Set different colors based on highlight state
    if (polygon->isHighlighted()) {
        // Highlight state: orange edge, yellow fill
        QPen pen(QColor(255, 140, 0));  // Orange edge
        pen.setWidth(5);  // Thicker edge
        pen.setCosmetic(true);
        pen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(pen);
        
        QBrush brush(QColor(255, 215, 0, 200));  // Gold fill
        painter.setBrush(brush);
    } else {
        // Normal state: blue edge, light blue fill
        QPen pen(QColor(0, 80, 200));  // Dark blue edge
        pen.setWidth(3);  // Thicker edge
        pen.setCosmetic(true);
        pen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(pen);
        
        QBrush brush(QColor(173, 216, 255, 180));  // Light blue fill with transparency
        painter.setBrush(brush);
    }
    
    // Draw polygon with holes
    painter.drawPath(path);
}

QRectF RenderWidget::calculateBounds() const
{
    if (m_polygons.isEmpty()) {
        return QRectF();
    }
    
    double minX = std::numeric_limits<double>::max();
    double minY = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double maxY = std::numeric_limits<double>::lowest();
    
    for (const Polygon* polygon : m_polygons) {
        if (!polygon || !polygon->isValid()) {
            continue;
        }
        
        // Traverse all points in all loops
        for (const QVector<QPointF>& loop : polygon->getLoops()) {
            for (const QPointF& point : loop) {
                minX = qMin(minX, point.x());
                minY = qMin(minY, point.y());
                maxX = qMax(maxX, point.x());
                maxY = qMax(maxY, point.y());
            }
        }
    }
    
    if (minX <= maxX && minY <= maxY) {
        return QRectF(QPointF(minX, minY), QPointF(maxX, maxY));
    }
    
    return QRectF();
}
