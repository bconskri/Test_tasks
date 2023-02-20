#pragma once

#include <optional>
#include <stdexcept>
#include <cmath>

namespace mycadlib {
    struct Vector3D {
        double x = 0, y = 0, z = 0;

        Vector3D() = default;

        Vector3D(double x, double y, double z);

        Vector3D operator+(const Vector3D &rh);

        friend std::ostream &operator<<(std::ostream &out, const Vector3D &value_to_output);
    };

    class Curve3D {
    protected:
        Vector3D pivot_point_;

        Curve3D() = default;

        explicit Curve3D(double x, double y, double z);

        virtual ~Curve3D() = default;

    public:
        virtual Vector3D getPoint(double t) const = 0;

        virtual Vector3D getDerivative(double t) const = 0;

        virtual std::string getType() const = 0;

        Vector3D getPivot() const;
    };

    class Ellipse3D : public Curve3D {
    public:
        explicit Ellipse3D(double rx, double ry, double x = 0, double y = 0, double z = 0);

        Vector3D getPoint(double t) const override;

        Vector3D getDerivative(double t) const override;

        std::string getType() const override;

        double get_rx() const;
        double get_ry() const;

    protected:
        double rx_, ry_;
    };

    class Circle3D : public Ellipse3D {
    public:
        explicit Circle3D(double r, double x = 0, double y = 0, double z = 0);

        std::string getType() const override;

        double get_r() const;
    };

    class Helix3D : public Curve3D {
    public:
        explicit Helix3D(double r, double s, double t_start = 0, double x = 0, double y = 0, double z = 0);

        Vector3D getPoint(double t) const override;

        Vector3D getDerivative(double t) const override;

        std::string getType() const override;

    protected:
        double r_, s_, t_start_; //radius, step, t_start - start angle
    };
} // namespace mycadlib