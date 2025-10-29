#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>
#include <QVector>
#include <QTransform>
#include "polygon.h"

class RenderWidget : public QWidget {
    Q_OBJECT
    
public:
    explicit RenderWidget(QWidget* parent = nullptr);
    
    void setPolygons(const QVector<Polygon*>& polygons,bool isUpdate = true);
    void updatePolygonsVisibility(const QVector<Polygon*>& polygons);
    
protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    
private:
    QVector<Polygon*> m_polygons;
    QRectF m_cachedBounds;
    QTransform m_cachedTransform;
    bool m_boundsNeedsUpdate = true;
    
    void drawPolygon(QPainter& painter, const Polygon* polygon);
    QRectF calculateBounds() const;
    QTransform calculateTransform(const QRectF& bounds) const;
};

#endif // RENDERWIDGET_H
