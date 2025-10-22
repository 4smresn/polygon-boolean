#include "polygon.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

Polygon::Polygon(const QString& name)
    : m_name(name)
{
}

bool Polygon::loadFromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    
    QTextStream in(&file);
    m_points.clear();
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        // 跳过注释和空行
        if (line.isEmpty() || line.startsWith("#") || line.startsWith("//")) {
            continue;
        }
        
        // 解析坐标点
        QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() >= 2) {
            bool okX, okY;
            double x = parts[0].toDouble(&okX);
            double y = parts[1].toDouble(&okY);
            
            if (okX && okY) {
                m_points.append(QPointF(x, y));
            }
        }
    }
    
    file.close();
    return isValid();
}
