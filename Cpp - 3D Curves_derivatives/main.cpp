#include "mycadlib.h"

#include <iostream>
#include <cassert>
#include <random>
#include <memory>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <execution> //PSTL CPP17 based on TBB impl using execution:par
#include <chrono>

using namespace mycadlib;
using namespace std::literals;

void TestShapeClass() {
    const double EPSILON = 1e-6;

    Vector3D a; //default constructor
    assert(a.x == 0 && a.y == 0 && a.z == 0);

    Circle3D c(10);
    assert(c.get_r() == 10 && c.getPivot().x == 0 && c.getPivot().y == 0 && c.getPivot().z == 0);
    try {
        Circle3D c1(-1);
    } catch (...) {
        assert(true);
    }
    //test circle points calculation
    const auto pc = c.getPoint(M_PI); //point -10,0,0
    assert(pc.x == -10 && (abs(pc.y - 0) < EPSILON) && pc.z == 0);
    const auto pc1 = c.getPoint(M_PI+M_PI_2); //point -10,-10,0
    assert((abs(pc1.x - 0) < EPSILON) && pc1.y == -10 && pc1.z == 0);

    Ellipse3D e(1, 2, 10, 20, 30);
    assert(e.get_rx() == 1 && e.get_ry() == 2 && e.getPivot().x == 10 && e.getPivot().y == 20 && e.getPivot().z == 30);
    try {
        Ellipse3D e1(-1, 2);
    } catch (...) {
        assert(true);
    }
    try {
        Ellipse3D e2(1, -2);
    } catch (...) {
        assert(true);
    }

    const double step = 5;
    Helix3D h(3, step, 10, 20, 30);
    // check the condition C(t + 2*PI) = C(t) + {0, 0, step}.
    const auto ph1 = h.getPoint(50);
    const auto ph2 = h.getPoint(50+2*M_PI);
    std::cout << ph1 << std::endl;
    std::cout << ph2 << std::endl;
    assert((abs(ph1.x - ph2.x) < EPSILON) && (abs(ph1.y - ph2.y) < EPSILON) && (abs(ph1.z + step - ph2.z) < EPSILON));

    std::cout << "Tests passed!"s << '\n' << '\n';
}

int main() {
    TestShapeClass();

    std::mt19937 engine;
    auto now = std::chrono::high_resolution_clock::now();
    engine.seed(now.time_since_epoch().count());
    //
    std::uniform_int_distribution<short> shape_type(0, 2);          // 0 - circle, 1 - ellipse, 3 - helix
    std::uniform_int_distribution<short> container_size(30, 50);    //size of container
    std::uniform_real_distribution<double> coord(-50, 50);          //coord distribution
    std::uniform_real_distribution<double> radius(5, 50);           //radius distribution
    std::uniform_real_distribution<double> angle(0, 2 * M_PI);      //angle distribution

    //2. Populate a container (e.g. vector or list) of objects of these types created in
    // random manner with random parameters
    std::vector<std::shared_ptr<Curve3D>> curves;
    curves.reserve(container_size(engine));
    for (size_t i = 0; i < curves.capacity(); i++) {
        switch (shape_type(engine)) {
            case 0: //circle
            {
                curves.emplace_back(std::make_shared<Circle3D>(radius(engine),
                                                       coord(engine),
                                                       coord(engine),
                                                       coord(engine)));
                break;
            }
            case 1: //ellipse
            {
                curves.emplace_back(std::make_shared<Ellipse3D>(radius(engine),
                                                        radius(engine),
                                                        coord(engine),
                                                        coord(engine),
                                                        coord(engine)));
                break;
            }
            case 2: //helix
            {
                curves.emplace_back(std::make_shared<Helix3D>(radius(engine),
                                                      coord(engine),
                                                      angle(engine),
                                                      coord(engine),
                                                      coord(engine),
                                                      coord(engine)));
                break;
            }
        }
    }

    std::cout << std::setprecision(2) << std::fixed;

    //3. Print coordinates of points and derivatives of all curves in the container at t=PI/4.
    std::cout << "Output coordinates of points and derivatives of all curves in the container at t=PI/4" << '\n';
    for (const auto &curve : curves) {
        std::cout << curve->getType() << " with t=PI/4 curve point = "s << curve->getPoint(M_PI / 4) <<
                  ", derivative = "s << curve->getDerivative(M_PI / 4) << '\n';
    }
    std::cout << std::endl;

    //4. Populate a second container that would contain only circles from the first container.
    std::cout << "Circles radii"s << '\n';
    std::vector<std::shared_ptr<Circle3D>> circles;
    for (const auto &curve : curves) {
        if (auto circle = std::dynamic_pointer_cast<Circle3D>(curve)) {
            circles.emplace_back(circle);
            std::cout << circle->get_r() << '\n';
        }
    }
    std::cout << std::endl;

    //5. Sort the second container in the ascending order of circles’ radii. That is, the first element has the
    //smallest radius, the last - the greatest.
    std::sort(circles.begin(), circles.end(),
              [](const auto &lhs, const auto &rhs) { return lhs->get_r() < rhs->get_r(); });

    //6. Compute the total sum of radii of all curves in the second container.
    const double sum_radii = transform_reduce(
            std::execution::par, //using PSTL CPP17 based on TBB impl parallel computing
            circles.begin(), circles.end(),
            .0,  // initial value
            std::plus<>{},  // reduce-operation
            [](const auto &circle) { return circle->get_r(); }  // map-operation
    );
    std::cout << "Total sum of radii of all curves in the second container: "s << sum_radii << '\n';

    return 0;
}