#include <list>
#include <vector>

class Alg
{
    struct Point
    {
        double x;
        double y;
        bool isIntersection = false;
        double scaleInA = 0.0;
        double scaleInB = 0.0;
        bool isEnterA = false;
        bool isUsed = false;

        int loopIndexInA = -1;
        int loopIndexInB = -1;
        int startIndexInA = -1;
        int startIndexInB = -1;
        int indexInA = -1;
        int indexInB = -1;

        int index = -1;

        Point(double x, double y, int index)
        {
            this->x = x;
            this->y = y;
            this->index = index;
        }
        Point() {}
    };

    using Loop = std::vector<Point *>;
    using Polygon = std::vector<Loop>;
    using Polygons = std::vector<Polygon>;

    using stdLoop = std::vector<std::pair<double, double>>;
    using stdPolygon = std::vector<stdLoop>;
    using stdPolygons = std::vector<stdPolygon>;

private:
    int ops;
    Polygon A;
    Polygon B;
    Loop intersections;
    Polygon _A;
    Polygon _B;
    Polygons result;
    Polygons innerLoops;

public:
    Alg(stdPolygon a, stdPolygon b, int ops);

    stdPolygons getResult();

    double computeArea(Loop &loop);

    bool isOuterLoop(Loop &loop);

    bool crossProduct(Point *p1, Point *p2, Point *p3, Point *p4);

    bool isPointInLoop(Point *p, Loop &loop);

    bool isLoopAContainsLoopB(Loop &loopA, Loop &loopB);

    bool calculateIntersection(Point *startA, Point *endA, Point *startB, Point *endB, Point *&intersection);

    void calculateIntersections();

    void insertIntersections();

    void Union();

    void excuteUnion();

    void excute();
};
