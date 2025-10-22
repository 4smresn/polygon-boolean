#include "renderwidget.h"
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QPolygonF>

RenderWidget::RenderWidget(QWidget* parent)
    : QWidget(parent)
{
    setMinimumSize(600, 600);
    setStyleSheet("background-color: white;");
}

void RenderWidget::setPolygons(const QVector<Polygon*>& polygons)
{
    m_polygons = polygons;
    update();
}

void RenderWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    if (m_polygons.isEmpty()) {
        return;
    }
    
    // 计算所有多边形的边界
    QRectF bounds = calculateBounds();
    if (bounds.isEmpty()) {
        return;
    }
    
    // 计算缩放和平移以适应窗口
    double margin = 50.0;
    double scaleX = (width() - 2 * margin) / bounds.width();
    double scaleY = (height() - 2 * margin) / bounds.height();
    double scale = qMin(scaleX, scaleY);
    
    // 设置变换
    painter.translate(width() / 2.0, height() / 2.0);
    painter.scale(scale, -scale);  // Y轴翻转，使其向上为正
    painter.translate(-bounds.center().x(), -bounds.center().y());
    
    // 绘制所有多边形
    for (const Polygon* polygon : m_polygons) {
        if (polygon && polygon->isValid()) {
            drawPolygon(painter, polygon);
        }
    }
}

void RenderWidget::drawPolygon(QPainter& painter, const Polygon* polygon)
{
    const QVector<QPointF>& points = polygon->getPoints();
    if (points.size() < 3) {
        return;
    }
    
    // 创建QPolygonF
    QPolygonF qpolygon;
    for (const QPointF& point : points) {
        qpolygon.append(point);
    }
    
    // 根据是否高亮设置不同的颜色
    if (polygon->isHighlighted()) {
        // 高亮状态：橙色边缘，黄色填充
        QPen pen(QColor(255, 140, 0));  // 橙色边缘线
        pen.setWidth(5);  // 更粗的边缘线
        pen.setCosmetic(true);
        pen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(pen);
        
        QBrush brush(QColor(255, 215, 0, 200));  // 金黄色填充
        painter.setBrush(brush);
    } else {
        // 普通状态：蓝色边缘，淡蓝色填充
        QPen pen(QColor(0, 80, 200));  // 深蓝色边缘线
        pen.setWidth(3);  // 加粗边缘线
        pen.setCosmetic(true);
        pen.setJoinStyle(Qt::RoundJoin);
        painter.setPen(pen);
        
        QBrush brush(QColor(173, 216, 255, 180));  // 淡蓝色填充，带透明度
        painter.setBrush(brush);
    }
    
    // 绘制多边形
    painter.drawPolygon(qpolygon);
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
        
        for (const QPointF& point : polygon->getPoints()) {
            minX = qMin(minX, point.x());
            minY = qMin(minY, point.y());
            maxX = qMax(maxX, point.x());
            maxY = qMax(maxY, point.y());
        }
    }
    
    if (minX <= maxX && minY <= maxY) {
        return QRectF(QPointF(minX, minY), QPointF(maxX, maxY));
    }
    
    return QRectF();
}
