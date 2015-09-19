#ifndef GEOMETRY_E2_H
#define GEOMETRY_E2_H

#include "geometry/E2/Point_E2.hpp"

template<typename number_type> class Line_E2;
template<typename number_type> class Segment_E2;
template<typename number_type> class Vector_E2;
template<typename number_type> class Triangle_E2;
template<typename number_type> class Line_E2;
template<typename number_type> class Oriented_Circle_E2;
template<typename number_type> class Point_Polygon_E2;
template<typename number_type> class Line_Polygon_E2;
template<typename number_type> class Sub_hyperplane_E2;
template<typename number_type> class Transformation_E2;
template<typename number_type> class Circular_Arc_E2;

template<typename number_type>
struct Geometry_E2
{
    typedef Point_E2<number_type>           Point;
    typedef Vector_E2<number_type>          Vector;
    typedef Line_E2<number_type>            Line;
    typedef Line_E2<number_type>            Hyperplane;
    typedef Segment_E2<number_type>         Segment;
    typedef Triangle_E2<number_type>        Triangle;
    typedef Oriented_Circle_E2<number_type> Circle;

    typedef Point_Polygon_E2<number_type>   Point_Polygon;
    typedef Point_Polygon_E2<number_type>   Point_Polytope;

    typedef Line_Polygon_E2<number_type>    Line_Polygon;
    typedef Line_Polygon_E2<number_type>    Hyperplane_Polytope;
    typedef Sub_hyperplane_E2<number_type>  Sub_hyperplane;
    typedef Transformation_E2<number_type>  Transformation;
    typedef Circular_Arc_E2<number_type>    Circular_Arc;
};

#endif // GEOMETRY_E2_H
