#ifndef POINT_E2_H
#define POINT_E2_H

#include <iostream>

template<typename number_type> class Point_E2;
template<typename number_type> class Vector_E2;

template<typename number_type>
std::ostream & operator<< (std::ostream& os, const Point_E2<number_type> & p);

template<typename number_type>
std::istream & operator>> (std::istream& is, Point_E2<number_type> & p);

template<typename number_type>
class Point_E2
{
private:
    number_type _x, _y;
public:
    // Notice that the initialization expressions x() and y() guarantee:
    // 1- that the default constructor for the type number_type will be used
    // 2- if the type number_type is a primitive type,
    //    the member variable is initialized to zero.
    Point_E2()
        : _x(), _y()
    {
    }

    Point_E2(const number_type & x, const number_type & y)
        : _x(x), _y(y)
    {

    }

    number_type x() const { return _x; }
    number_type y() const { return _y; }

    bool operator==(const Point_E2<number_type>& p) const {
        return (this == &p) ||
            ((_x == p._x) && (_y == p._y));
    }

    bool operator!=(const Point_E2<number_type>& p) const {
        return ! operator==(p);
    }

    Point_E2<number_type> operator+=(const Vector_E2<number_type> & v);
    Point_E2<number_type> operator-=(const Vector_E2<number_type> & v);

    friend
    std::ostream& operator <<(std::ostream& os, const Point_E2<number_type>& p);

    friend
    std::istream& operator >>(std::istream & is, Point_E2<number_type> & p);
};

template<typename number_type>
std::ostream & operator <<(std::ostream & os, const Point_E2<number_type> & p)
{
    return os << p._x << " " << p._y;
}

template<typename number_type>
std::istream & operator >>(std::istream & is, Point_E2<number_type> & p)
{
    return is >> p._x >> p._y;
}

template<typename number_type>
number_type
squared_distance(const Point_E2<number_type> & P, const Point_E2<number_type> & Q)
{
    number_type x = P.x() - Q.x();
    number_type y = P.y() - Q.y();
    return x*x + y*y;
}

#endif // POINT_E2_H
