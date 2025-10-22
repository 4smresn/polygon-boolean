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
    bool isValid() const { return m_points.size() >= 3; }
    
    void setHighlighted(bool highlighted) { m_highlighted = highlighted; }
    bool isHighlighted() const { return m_highlighted; }
    
private:
    QString m_name;
    QVector<QPointF> m_points;
    bool m_highlighted = false;
};

#endif // POLYGON_H
