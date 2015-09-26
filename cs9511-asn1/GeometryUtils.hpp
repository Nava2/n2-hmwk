#ifndef GEOMETRYUTILS_HPP
#define GEOMETRYUTILS_HPP

#include <QPointF>
#include <QPolygon>

namespace GeometryUtils {

/// Calculates the polygon centroid, assuming the polygon does not wrap around.
inline
void polygonCentroid(const QPolygonF& poly, QPointF* const centroid, double* const parea)
{
    double area = 0.;
    double cx = 0., cy = 0.;
    for (int i = 0; i < poly.size(); ++i)
    {
        const QPointF p     = poly[i];
        const QPointF pp1   = poly[(i + 1) % poly.size()];

        const double fact = (p.x() * pp1.y()) - (pp1.x() * p.y());
        area += fact;

        cx += (p.x() + pp1.x()) * fact;
        cy += (p.y() + pp1.y()) * fact;
    }

    if (!!centroid)
    {
        *centroid = QPointF(cx, cy) * (1. / (3. * area));
    }

    if (!!parea)
    {
        *parea = 0.5 * area;
    }
}

inline
qreal distSqr(const QPointF& p1, const QPointF& p2)
{
    const QPointF pt = p2 - p1;
    return pt.x() * pt.x() + pt.y() * pt.y();
}

inline
qreal dist(const QPointF& p1, const QPointF& p2)
{
    return sqrt(distSqr(p1, p2));
}

inline
qreal dot(const QPointF& p1, const QPointF& p2)
{
    return p1.x() * p2.x() + p1.y() * p2.y();
}
/// | a b |
/// | c d |
inline
qreal det(const qreal a, const qreal b, const qreal c, const qreal d)
{
    return a * d - b * c;
}

inline
const QPointF intersection(const QPointF u1, const QPointF u2,
                           const QPointF v1, const QPointF v2)
{
    const qreal denum = det(det(u1.x(), 1, u2.x(), 1), det(u1.y(), 1, u2.y(), 1)
                          , det(v1.x(), 1, v2.x(), 1), det(v1.y(), 1, v2.y(), 1));
    const qreal px = det(det(u1.x(), u1.y(), u2.x(), u2.y()), det(u1.x(), 1, u2.x(), 1)
                       , det(v1.x(), v1.y(), v2.x(), v2.y()), det(v1.x(), 1, v2.x(), 1)) / denum;
    const qreal py = det(det(u1.x(), u1.y(), u2.x(), u2.y()), det(u1.y(), 1, u2.y(), 1)
                       , det(v1.x(), v1.y(), v2.x(), v2.y()), det(v1.y(), 1, v2.y(), 1)) / denum;

    return QPointF(px, py);
}

inline
double angleBtwn(const QPointF& u1, const QPointF& u2,
                 const QPointF& v1, const QPointF& v2)
{
    const QPointF u = u2 - u1;
    const QPointF v = v2 - v1;

    const qreal algDot = dot(u, v);
    const qreal uMag = dist(u2, u1);
    const qreal vMag = dist(v2, v1);

    return std::acos(algDot / (uMag * vMag));
}

inline
qreal ptDistance(const QPointF& p, const QPointF& v, const QPointF& w)
{
    const qreal lenSqr = distSqr(v, w);

    if (lenSqr == 0)
    {   // The same point, thus points are coincident
        return dist(p, v);
    }

    const qreal t = dot(p - v, w - v) / lenSqr;
    if (t < 0.0)
    {
        return dist(p, v);
    }
    else if (t > 1.0)
    {
        return dist(p, w);
    }

    // calculate the projection
    const QPointF proj = v + t * (w - v);

    return dist(p, proj);
}

inline
qreal segDist(const QPointF& s, const QPointF& t, const QPointF& u, const QPointF& v)
{
    const qreal uvmin = std::min(ptDistance(u, s, t), ptDistance(v, s, t));
    const qreal stmin = std::min(ptDistance(s, u, v), ptDistance(t, u, v));

    return std::min(uvmin, stmin);
}

inline
void findParallelLine(const QPolygonF& searchPoly, const QPointF& u, const QPointF& v,
                       QPointF* s, QPointF* t)
{
    const QPointF m_uv = u - v;

    qreal dist = std::numeric_limits<qreal>::max();

    for (int i = 0; i < searchPoly.size(); ++i)
    {
        auto pt1 = searchPoly[i];
        auto pt2 = searchPoly[(i + 1) % searchPoly.size()];

        auto m_pt = pt1 - pt2;

        const qreal newDist = segDist(u, v, pt1, pt2);
        if (   (std::fabs(m_pt.x() - m_uv.x()) < 0.01)
            && (std::fabs(m_pt.y() - m_uv.y()) < 0.01))
        {
            if (newDist < dist)
            {
                *s = pt1;
                *t = pt2;

                dist = newDist;
            }
        }
        else if (   (std::fabs(-m_pt.x() - m_uv.x()) < 0.01)
                 && (std::fabs(-m_pt.y() - m_uv.y()) < 0.01))
        {
            if (newDist < dist)
            {
                *s = pt2;
                *t = pt1;

                dist = newDist;
            }
        }
    }
}

}

#endif // GEOMETRYUTILS_HPP

