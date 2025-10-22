#ifndef POLYGON_H
#define POLYGON_H

#include <QString>
#include <QVector>
#include <QPointF>

class Polygon {
public:
    Polygon(const QString& name);
    
    bool loadFromFile(const QString& filename);
    
    const QString& getName() const { return m_name; }
    const QVector<QPointF>& getPoints() const { return m_points; }
    const QVector<QVector<QPointF>>& getLoops() const { return m_loops; }
    bool isValid() const { return !m_loops.isEmpty() && m_loops[0].size() >= 3; }
    bool hasHoles() const { return m_loops.size() > 1; }
    
    // 设置多边形数据（用于布尔运算结果）
    void setLoops(const QVector<QVector<QPointF>>& loops);
    
    void setHighlighted(bool highlighted) { m_highlighted = highlighted; }
    bool isHighlighted() const { return m_highlighted; }
    
private:
    QString m_name;
    QVector<QPointF> m_points;  // 保留用于向后兼容
    QVector<QVector<QPointF>> m_loops;  // 多个循环：第一个是外轮廓，其余是孔
    bool m_highlighted = false;
};

#endif // POLYGON_H
