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
    m_loops.clear();
    
    QVector<QPointF> currentLoop;
    bool inLoop = false;
    
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        // 跳过空行
        if (line.isEmpty()) {
            continue;
        }
        
        // 检查是否是 #loop 标记
        if (line.startsWith("#loop")) {
            // 如果之前有循环数据，保存它
            if (!currentLoop.isEmpty()) {
                m_loops.append(currentLoop);
                currentLoop.clear();
            }
            inLoop = true;
            continue;
        }
        
        // 跳过其他注释
        if (line.startsWith("#") || line.startsWith("//")) {
            continue;
        }
        
        // 解析坐标点
        QStringList parts = line.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (parts.size() >= 2) {
            bool okX, okY;
            double x = parts[0].toDouble(&okX);
            double y = parts[1].toDouble(&okY);
            
            if (okX && okY) {
                QPointF point(x, y);
                currentLoop.append(point);
                m_points.append(point);  // 也添加到旧的 m_points 用于向后兼容
            }
        }
    }
    
    // 保存最后一个循环
    if (!currentLoop.isEmpty()) {
        m_loops.append(currentLoop);
    }
    
    // 如果没有 #loop 标记，所有点作为一个循环
    if (m_loops.isEmpty() && !m_points.isEmpty()) {
        m_loops.append(m_points);
    }
    
    file.close();
    return isValid();
}

void Polygon::setLoops(const QVector<QVector<QPointF>>& loops)
{
    m_loops = loops;
    
    // 同步更新 m_points（扁平化所有点）
    m_points.clear();
    for (const QVector<QPointF>& loop : loops) {
        m_points.append(loop);
    }
}
