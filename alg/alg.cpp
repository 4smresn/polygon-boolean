#include "alg.h"
#include <algorithm>
#include <iostream>

Alg::Alg(stdPolygon a, stdPolygon b, int ops)
{
    this->ops = ops;
    for (auto &l : a)
    {
        Loop loop2;
        for (int i = 0; i < l.size(); i++)
        {
            loop2.push_back(new Point(l[i].first, l[i].second, i));
        }
        A.push_back(loop2);
    }
    for (auto &l : b)
    {
        Loop loop2;
        for (int i = 0; i < l.size(); i++)
        {
            loop2.push_back(new Point(l[i].first, l[i].second, i));
        }
        B.push_back(loop2);
    }
}

Alg::stdPolygons Alg::getResult()
{
    stdPolygons res;
    for (auto &polygon : result)
    {
        stdPolygon loops;
        for (auto &loop : polygon)
        {
            stdLoop loop2;
            for (auto &pt : loop)
            {
                loop2.push_back({pt->x, pt->y});
            }
            loops.push_back(loop2);
        }
        res.push_back(loops);
    }
    return res;
}
double Alg::computeArea(Loop &loop)
{
    double area = 0;
    for (int i = 0; i < loop.size(); i++)
    {
        int j = (i + 1) % loop.size();
        area += loop[i]->x * loop[j]->y - loop[j]->x * loop[i]->y;
    }
    return area / 2;
}
bool Alg::isOuterLoop(Loop &loop)
{
    return computeArea(loop) > 0;
}

bool Alg::crossProduct(Point *p1, Point *p2, Point *p3, Point *p4)
{
    return (p2->x - p1->x) * (p4->y - p3->y) - (p2->y - p1->y) * (p4->x - p3->x) > 0;
}

bool Alg::isPointInLoop(Point *p, Loop &loop)
{
    int count = 0;
    for (int i = 0; i < loop.size(); i++)
    {
        int j = (i + 1) % loop.size();
        if ((loop[i]->y > p->y) != (loop[j]->y > p->y) && p->x < (loop[j]->x - loop[i]->x) * (p->y - loop[i]->y) / (loop[j]->y - loop[i]->y) + loop[i]->x)
        {
            count++;
        }
    }
    return count % 2 == 1;
}

bool Alg::isLoopAContainsLoopB(Loop &loopA, Loop &loopB)
{
    for (auto &p : loopB)
    {
        if (!isPointInLoop(p, loopA))
        {
            return false;
        }
    }
    return true;
}

bool Alg::calculateIntersection(Point *startA, Point *endA, Point *startB, Point *endB, Point *&intersection)
{
    double x1 = startA->x;
    double y1 = startA->y;
    double x2 = endA->x;
    double y2 = endA->y;
    double x3 = startB->x;
    double y3 = startB->y;
    double x4 = endB->x;
    double y4 = endB->y;

    double d = (x1 - x2) * (y3 - y4) - (y1 - y2) * (x3 - x4);
    if (d == 0)
    {
        return false;
    }
    double t = ((x1 - x3) * (y3 - y4) - (y1 - y3) * (x3 - x4)) / d;
    double u = ((x1 - x3) * (y1 - y2) - (y1 - y3) * (x1 - x2)) / d;
    if (t < 0 || t > 1 || u < 0 || u > 1)
    {
        return false;
    }

    intersection = new Point();
    intersection->x = x1 + t * (x2 - x1);
    intersection->y = y1 + t * (y2 - y1);
    intersection->isIntersection = true;
    intersection->scaleInA = t;
    intersection->scaleInB = u;
    intersection->isEnterA = crossProduct(startA, endA, startB, endB);
    return true;
}

void Alg::calculateIntersections()
{
    for (int loopAIndex = 0; loopAIndex < A.size(); loopAIndex++)
    {
        for (int loopBIndex = 0; loopBIndex < B.size(); loopBIndex++)
        {
            for (int startAIndex = 0; startAIndex < A[loopAIndex].size(); startAIndex++)
            {
                auto startA = A[loopAIndex].begin() + startAIndex;
                auto endA = std::next(startA);
                if (endA == A[loopAIndex].end())
                {
                    endA = A[loopAIndex].begin();
                }
                for (int startBIndex = 0; startBIndex < B[loopBIndex].size(); startBIndex++)
                {
                    auto startB = B[loopBIndex].begin() + startBIndex;
                    auto endB = std::next(startB);
                    if (endB == B[loopBIndex].end())
                    {
                        endB = B[loopBIndex].begin();
                    }
                    Point *intersection = nullptr;

                    if (calculateIntersection(*startA, *endA, *startB, *endB, intersection))
                    {
                        intersection->loopIndexInA = loopAIndex;
                        intersection->loopIndexInB = loopBIndex;
                        intersection->startIndexInA = startAIndex;
                        intersection->startIndexInB = startBIndex;
                        intersections.push_back(intersection);
                    }
                }
            }
        }
    }
}

void Alg::insertIntersections()
{
    std::sort(intersections.begin(), intersections.end(), [](const Point *a, const Point *b)
              {
            if(a->loopIndexInA!=b->loopIndexInA){
                return a->loopIndexInA < b->loopIndexInA;
            }
            if(a->startIndexInA!=b->startIndexInA){
                return a->startIndexInA < b->startIndexInA;
            }
            return a->scaleInA < b->scaleInA; });
    for (int loopIndex = 0, i = 0; loopIndex < A.size(); loopIndex++)
    {
        std::vector<Point *> newLoop;
        int j = 0;
        while (i < intersections.size() && intersections[i]->loopIndexInA == loopIndex)
        {
            int startIndex = intersections[i]->startIndexInA;
            while (j <= startIndex)
            {
                newLoop.push_back(A[loopIndex][j]);
                j++;
            }
            while (i < intersections.size() && intersections[i]->loopIndexInA == loopIndex && intersections[i]->startIndexInA == startIndex)
            {
                intersections[i]->indexInA = newLoop.size();
                newLoop.push_back(intersections[i]);
                i++;
            }
        }
        while (j < A[loopIndex].size())
        {
            newLoop.push_back(A[loopIndex][j]);
            j++;
        }
        _A.push_back(newLoop);
    }

    std::sort(intersections.begin(), intersections.end(), [](const Point *a, const Point *b)
              {
            if(a->loopIndexInB!=b->loopIndexInB){
                return a->loopIndexInB < b->loopIndexInB;
            }
            if(a->startIndexInB!=b->startIndexInB){
                return a->startIndexInB < b->startIndexInB;
            }
            return a->scaleInB < b->scaleInB; });
    for (int loopIndex = 0, i = 0; loopIndex < B.size(); loopIndex++)
    {
        std::vector<Point *> newLoop;
        int j = 0;
        while (i < intersections.size() && intersections[i]->loopIndexInB == loopIndex)
        {
            int startIndex = intersections[i]->startIndexInB;
            while (j <= startIndex)
            {
                newLoop.push_back(B[loopIndex][j]);
                j++;
            }
            while (i < intersections.size() && intersections[i]->loopIndexInB == loopIndex && intersections[i]->startIndexInB == startIndex)
            {
                intersections[i]->indexInB = newLoop.size();
                newLoop.push_back(intersections[i]);
                i++;
            }
        }
        while (j < B[loopIndex].size())
        {
            newLoop.push_back(B[loopIndex][j]);
            j++;
        }
        _B.push_back(newLoop);
    }
}

void Alg::Union()
{
    for (auto &i : intersections)
    {
        auto startPointPointer = _A[i->loopIndexInA].begin() + i->indexInA;
        auto startLoopPointer = _A.begin() + i->loopIndexInA;
        if (!(*startPointPointer)->isEnterA || (*startPointPointer)->isUsed)
        {
            continue;
        }

        Polygon p;
        std::vector<Point *> loop;

        bool onA = 1;
        while (!(*startPointPointer)->isUsed)
        {
            loop.push_back(*startPointPointer);
            (*startPointPointer)->isUsed = true;
            startPointPointer++;
            if (startPointPointer == (*startLoopPointer).end())
            {
                startPointPointer = (*startLoopPointer).begin();
            }
            if ((*startPointPointer)->isIntersection)
            {
                if (onA)
                {
                    startLoopPointer = _B.begin() + (*startPointPointer)->loopIndexInB;
                    startPointPointer = (*startLoopPointer).begin() + (*startPointPointer)->indexInB;
                    onA = 0;
                }
                else
                {
                    startLoopPointer = _A.begin() + (*startPointPointer)->loopIndexInA;
                    startPointPointer = (*startLoopPointer).begin() + (*startPointPointer)->indexInA;
                    onA = 1;
                }
            }
        }

        p.push_back(loop);
        if (isOuterLoop(loop))
        {
            result.push_back(p);
        }
        else
        {
            innerLoops.push_back(p);
        }
    }
}

void Alg::excuteUnion()
{
    calculateIntersections();
    insertIntersections();
    Union();

    bool AContainsB = isLoopAContainsLoopB(A[0], B[0]);
    bool BContainsA = isLoopAContainsLoopB(B[0], A[0]);
    if (A[0].size() == _A[0].size())
    {
        if (AContainsB)
        {
            result.push_back(A);
        }
        else if (BContainsA)
        {
            result.push_back(B);
        }
        else
        {
            result.push_back(A);
            result.push_back(B);
        }
    }

    for (auto &inner : innerLoops)
    {
        for (auto &outer : result)
        {
            if (isPointInLoop(inner[0][0], outer[0]))
            {
                outer.push_back(inner[0]);
                break;
            }
        }
    }
}

void Alg::excute()
{
    if (ops == 1)
    {
        excuteUnion();
    }
}
