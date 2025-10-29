#include "booleanops.h"
#include "../alg/alg.h"
#include <algorithm>
#include <limits>

namespace BooleanOps
{
    OpPolygon fromQtPolygon(const Polygon *polygon)
    {
        if (!polygon || !polygon->isValid())
        {
            return OpPolygon();
        }

        OpPolygon result;
        const QVector<QVector<QPointF>> &qtLoops = polygon->getLoops();
        result.reserve(qtLoops.size());

        for (const QVector<QPointF> &qtLoop : qtLoops)
        {
            OpLoop loop;
            loop.reserve(qtLoop.size());

            for (const QPointF &qp : qtLoop)
            {
                loop.emplace_back(qp.x(), qp.y());
            }

            if (!loop.empty())
            {
                result.push_back(std::move(loop));
            }
        }

        return result;
    }

    Polygon *toQtPolygon(const OpPolygon &data, const QString &name)
    {
        if (data.empty() || data[0].size() < 3)
        {
            return nullptr;
        }

        Polygon *polygon = new Polygon(name);
        QVector<QVector<QPointF>> qtLoops;
        qtLoops.reserve(data.size());

        for (const OpLoop &loop : data)
        {
            QVector<QPointF> qtLoop;
            qtLoop.reserve(loop.size());

            for (const OpPoint &p : loop)
            {
                qtLoop.append(QPointF(p.first, p.second));
            }

            if (!qtLoop.isEmpty())
            {
                qtLoops.append(qtLoop);
            }
        }

        polygon->setLoops(qtLoops);

        return polygon;
    }

    OpPolygons performOperation(
    const OpPolygon& poly1,
    const OpPolygon& poly2,
    int op,
    const double& tolerance){
        Alg alg(poly1, poly2, op, tolerance);
        alg.excute();
        return alg.getResult();
    }
}


