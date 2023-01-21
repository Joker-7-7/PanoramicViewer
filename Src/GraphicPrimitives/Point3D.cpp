#include "Point3D.hpp"

#include <cmath>

namespace GraphicPrimitives
{
    Point3D::Point3D() :
            x(0.0),
            y(0.0),
            z(0.0)
    {
    }

    Point3D::Point3D(double x, double y, double z) :
            x(x),
            y(y),
            z(z)
    {
    }

    Point3D::Point3D(const double* value) :
            x(value[0]),
            y(value[1]),
            z(value[2])
    {
    }

    Point3D operator+(const Point3D& a, const Point3D& b) {
        return {a.x + b.x, a.y + b.y, a.z + b.z};
    }

    Point3D operator-(const Point3D& a, const Point3D& b) {
        return {a.x - b.x, a.y - b.y, a.z - b.z};
    }

    Point3D& Point3D::operator=(const double* value) {
        x = value[0];
        y = value[1];
        z = value[2];
        return *this;
    }

    void Point3D::round() noexcept {
        x = std::round(x);
        y = std::round(y);
        z = std::round(z);
    }
}
