#ifndef TRIANGLE_E2_H
#define TRIANGLE_E2_H

#include "geometry/E2/Point_E2.hpp"
#include "misc/determinant.hpp"

template<typename number_type>
class Triangle_E2
{
    Point_E2<number_type> p0;
    Point_E2<number_type> p1;
    Point_E2<number_type> p2;
public:
    Triangle_E2(const Point_E2<number_type>& q0,
                const Point_E2<number_type>& q1,
                const Point_E2<number_type>& q2)
        : p0(q0), p1(q1), p2(q2)
    {

    }

    Point_E2<number_type> P0() const { return p0; }
    Point_E2<number_type> P1() const { return p1; }
    Point_E2<number_type> P2() const { return p2; }

    Point_E2<number_type> vertex(int i) const
    {
        switch(i) {
        case 0: return p0;
        case 1: return p1;
        case 2: return p2;
        }
    }

    number_type get_signed_area() const
    {
        number_type unity(1.0);
        number_type half(1.0/2.0);
        number_type area = half * determinant(p0.x(), p0.y(), unity,
                                              p1.x(), p1.y(), unity,
                                              p2.x(), p2.y(), unity);
        return area;
    }

private:

};

#endif // TRIANGLE_E2_H
