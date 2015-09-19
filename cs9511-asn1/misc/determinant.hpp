#ifndef DETERMINANT_HPP
#define DETERMINANT_HPP

template<typename number_type>
inline
number_type
determinant(const number_type & a, const number_type & b,
            const number_type & c, const number_type & d)
{
    return a * d - b * c;
}

template<typename number_type>
inline
number_type
determinant(const number_type & a, const number_type & b, const number_type & c,
            const number_type & d, const number_type & e, const number_type & f,
            const number_type & g, const number_type & h, const number_type & i)
{
    return a * determinant(e,f,h,i)
        -  b * determinant(d,f,g,i)
        +  c * determinant(d,e,g,h);
}

template<typename number_type>
inline
number_type
determinant(const number_type & a, const number_type & b, const number_type & c, const number_type & d,
            const number_type & e, const number_type & f, const number_type & g, const number_type & h,
            const number_type & i, const number_type & j, const number_type & k, const number_type & l,
            const number_type & m, const number_type & n, const number_type & o, const number_type & p)
{
    return a * determinant(f,g,h, j,k,l, n,o,p)
        -  b * determinant(e,g,h, i,k,l, m,o,p)
        +  c * determinant(e,f,h, i,j,l, m,n,p)
        -  d * determinant(e,f,g, i,j,k, m,n,o);
}

template<typename number_type>
inline
number_type
inner_product(const number_type & x1, const number_type & y1,
              const number_type & x2, const number_type & y2)
{
    return x1 * x2 + y1 * y2;
}

template<typename number_type>
inline
number_type
inner_product(const number_type & x1, const number_type & y1, const number_type & z1,
              const number_type & x2, const number_type & y2, const number_type & z2)
{
    return x1 * x2 + y1 * y2 + z1 * z2;
}

template<typename number_type>
inline
number_type
inner_product(const number_type & x1, const number_type & y1, const number_type & z1, const number_type & w1,
              const number_type & x2, const number_type & y2, const number_type & z2, const number_type & w2)
{
    return x1 * x2 + y1 * y2 + z1 * z2 + w1 * w2;
}

template<typename number_type>
inline
bool
are_dependent(
              const number_type & a1, const number_type & b1, const number_type & c1,
              const number_type & a2, const number_type & b2, const number_type & c2 )
{
    number_type zero = static_cast<number_type>(0);
    return (
            determinant(a1, b1, a2, b2) == zero &&
            determinant(a1, c1, a2, c2) == zero &&
            determinant(b1, c1, b2, c2) == zero );
}

template<typename number_type>
inline
bool
are_dependent(
              const number_type & a1, const number_type & b1, const number_type & c1, const number_type & d1,
              const number_type & a2, const number_type & b2, const number_type & c2, const number_type & d2 )
{
    number_type zero = static_cast<number_type>(0);
    return (
            determinant(a1, b1, a2, b2) == zero &&
            determinant(a1, c1, a2, c2) == zero &&
            determinant(a1, d1, a2, d2) == zero &&
            determinant(b1, c1, b2, c2) == zero &&
            determinant(b1, d1, b2, d2) == zero &&
            determinant(c1, d1, c2, d2) == zero );
}

template<class number_type>
inline
void
cross_product(const number_type & v1x, const number_type & v1y, const number_type & v1z,
              const number_type & v2x, const number_type & v2y, const number_type & v2z,
              number_type & vx, number_type & vy, number_type & vz)
{
    vx = + determinant( v1y, v2y, v1z, v2z );
    vy = - determinant( v1x, v2x, v1z, v2z );
    vz = + determinant( v1x, v2x, v1y, v2y );
}

#endif // DETERMINANT_HPP
