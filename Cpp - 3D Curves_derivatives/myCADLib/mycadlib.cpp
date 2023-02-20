#include "include/mycadlib.h"

#include <iostream>

namespace mycadlib {

    Vector3D::Vector3D(double x, double y, double z) : x(x), y(y), z(z) {}

    Vector3D Vector3D::operator+(const Vector3D &rh) {
        return {this->x + rh.x, this->y + rh.y, this->z + rh.z};
    }

    std::ostream &operator<<(std::ostream &out, const Vector3D &val) {
        out << '{' << val.x << ',' << val.y << ',' << val.z << '}';
        return out;
    }

    Curve3D::Curve3D(double x, double y, double z) : pivot_point_(x, y, z) {}

    Vector3D Curve3D::getPivot() const {
        return pivot_point_;
    }

    Ellipse3D::Ellipse3D(double rx, double ry, double x, double y, double z)
            : Curve3D(x, y, z),
              rx_(rx),
              ry_(ry) {
        if (rx <= 0 || ry <= 0) {
            throw std::invalid_argument("Radiuses rx and ry must be positive values");
        }
    }

    Vector3D Ellipse3D::getPoint(double t) const {
        return Vector3D(rx_ * std::cos(t), ry_ * std::sin(t), 0) + pivot_point_;
    }

    Vector3D Ellipse3D::getDerivative(double t) const {
        return {-1 * rx_ * std::sin(t), ry_ * std::cos(t), 0};
    }

    std::string Ellipse3D::getType() const {
        using namespace std::literals;
        return "Ellipse"s;
    }

    double Circle3D::get_r() const {
        return rx_;
    }

    double Ellipse3D::get_rx() const {
        return rx_;
    }

    double Ellipse3D::get_ry() const {
        return ry_;
    }

    Circle3D::Circle3D(double r, double x, double y, double z)
            : Ellipse3D(r, r, x, y, z) {}

    std::string Circle3D::getType() const {
        using namespace std::literals;
        return "Circle"s;
    }

    Helix3D::Helix3D(double r, double s, double t_start, double x, double y, double z)
            : Curve3D(x, y, z),
              r_(r),
              s_(s),
              t_start_(t_start) {
        if (r <= 0) {
            throw std::invalid_argument("Helix radius must be positive value");
        }
        if (s == 0) {
            throw std::invalid_argument("Helix step shall be different from zero");
        }
        if (t_start < 0 && t_start > 2*M_PI) {
            std::cout << t_start << '\n';
            throw std::invalid_argument("Angle must be between 0 and 2*PI");
        }
    }

    Vector3D Helix3D::getPoint(double t) const {
        return Vector3D(r_ * std::cos(t), r_ * std::sin(t), (t_start_ + t) / (2 * M_PI) * s_) + pivot_point_;
    }

    Vector3D Helix3D::getDerivative(double t) const {
        return {-1 * r_ * std::sin(t), r_ * std::cos(t), s_ / (2 * M_PI)};
    }

    std::string Helix3D::getType() const {
        using namespace std::literals;
        return "Helix"s;
    }
} // namespace mycadlib