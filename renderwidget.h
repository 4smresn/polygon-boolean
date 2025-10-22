#ifndef RENDERWIDGET_H
#define RENDERWIDGET_H

#include <QWidget>
#include <QVector>
#include "polygon.h"

class RenderWidget : public QWidget {
    Q_OBJECT
    
public:
    explicit RenderWidget(QWidget* parent = nullptr);
    
    void setPolygons(const QVector<Polygon*>& polygons);
    
protected:
    void paintEvent(QPaintEvent* event) override;
    
private:
    QVector<Polygon*> m_polygons;
    
    void drawPolygon(QPainter& painter, const Polygon* polygon);
    QRectF calculateBounds() const;
};

#endif // RENDERWIDGET_H
