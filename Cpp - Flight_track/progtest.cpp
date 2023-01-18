#include <stdio.h>
#include <string.h>
#include <unordered_map>
#include <string>
#include <limits>
#include <tuple>

typedef char Carrier[3];        // ��� ������������
typedef char FlightNo[5];    // ����� �����
typedef char Point[4];        // ��� ������
typedef long Fare;            // �����
const double CONST_DISCOUNT = 0.8;      //������ ����� ������ �����������

// ����� ��������
struct RoutePoint {
    RoutePoint *next;
    Point point;

    RoutePoint() : next(0) {}
};

// �������
class Route {
    RoutePoint *first;

public:
    Route();

    ~Route();

    // ������ �� �����
    int read(const char *fileName);    // 0 - OK, !=0 - ������

    // �������� �������� ��:
    //   ������������ �������� �������
    //   �� ����� ���� ������� � ��������
    int check() const;    // 0 - OK, !=0 - ������

    // ��������
    RoutePoint *iterator(RoutePoint *&iter) const;

    // ������ �� stdout
    void print(const char *prefix) const;
};

// ����
struct Flight {
    Carrier carrier;        // ����������
    FlightNo flightNo;    // ����� �����
    Point depPoint;    // ����� �����������
    Point arrPoint;    // ����� ����������
    Fare fare;            // �����

    void print() const;
};

//
class ScheduleItem : public Flight {
    friend class Schedule;

    ScheduleItem *next;

public:
    ScheduleItem() : next(0) {}
};

// ����������
class Schedule {
    ScheduleItem *firstFlight;
public:
    Schedule();

    ~Schedule();

    // ������ �� �����
    int read(const char *fileName);    // 0 - OK, !=0 - ������

    // ��������
    ScheduleItem *iterator(ScheduleItem *&iter) const;

    // ������ �� stdout
    void print() const;

};

// ������� ���������
struct TransLeg {
    TransLeg *next;
    Flight flight;

    TransLeg() : next(0) {}
};

// ���������
class Transportation {
    TransLeg *firstLeg;
    double total_fare;
public:
    Transportation();

    ~Transportation();

    void flush();

    int buildCheapest(const Route &route, const Schedule &schedule);

    void print() const;

private:
    std::unordered_map<std::string, Flight *> findLegFlight(const Schedule &schedule,
                                                            const char *depPoint,
                                                            const char *arrPoint);
};

//___ ���������� _________________________________

//___ Route ______________________________________

Route::Route()
        : first(0) {
}

Route::~Route() {
    for (RoutePoint *item = first; item;) {
        RoutePoint *toDelete = item;
        item = item->next;
        delete toDelete;
    }
}

int Route::read(const char *fileName) {
    RoutePoint *lastItem = 0;

    FILE *f = fopen(fileName, "r");
    if (!f) return 1;

    Point readPoint;
    while (fscanf(f, "%3s", readPoint) == 1) {
        RoutePoint *newItem = new RoutePoint;
        strcpy(newItem->point, readPoint);
        if (lastItem) {
            lastItem->next = newItem;
        } else
            first = newItem;
        lastItem = newItem;
    }

    fclose(f);
    return 0;
}

int Route::check() const {
    if (!first || !first->next)
        return 1;

    RoutePoint *iter = 0;
    while (iterator(iter)) {
        if (iter->next && 0 == strcmp(iter->point, iter->next->point))
            return 1;
    }
    return 0;
}

RoutePoint *Route::iterator(RoutePoint *&iter) const {
    if (iter)
        iter = iter->next;
    else
        iter = first;
    return iter;
}

void Route::print(const char *prefix) const {
    if (prefix)
        printf(prefix);

    RoutePoint *iter = 0;
    while (iterator(iter)) {
        printf("%s ", iter->point);
    }

    printf("\n");
}

//___ ���������� ___________________________________________

void Flight::print() const {
    printf("%-2s %-4s %-3s %-3s %10ld",
           carrier,
           flightNo,
           depPoint,
           arrPoint,
           fare);
}

Schedule::Schedule()
        : firstFlight(0) {
}

Schedule::~Schedule() {
    for (ScheduleItem *flight = firstFlight; flight;) {
        ScheduleItem *toDelete = flight;
        flight = flight->next;
        delete toDelete;
    }
}

int Schedule::read(const char *fileName) {
    ScheduleItem *lastFlight = 0;

    FILE *f = fopen(fileName, "r");
    if (!f) return 1;

    Flight fl;
    while (fscanf(f, "%2s %4s %3s %3s %ld", fl.carrier, fl.flightNo, fl.depPoint, fl.arrPoint, &fl.fare) == 5) {
        ScheduleItem *newFlight = new ScheduleItem;
        *(Flight *) newFlight = fl;
        if (lastFlight) {
            lastFlight->next = newFlight;
        } else
            firstFlight = newFlight;
        lastFlight = newFlight;
    }

    fclose(f);
    return 0;
}

ScheduleItem *Schedule::iterator(ScheduleItem *&iter) const {
    if (iter)
        iter = iter->next;
    else
        iter = firstFlight;
    return iter;
}

void Schedule::print() const {
    ScheduleItem *f = 0;
    while (iterator(f)) {
        f->print();
        printf("\n");
    }
}

//___ Transportation ______________________________________________

Transportation::Transportation()
        : firstLeg(0), total_fare(0) {
}

Transportation::~Transportation() {
    flush();
}

void Transportation::flush() {
    for (TransLeg *leg = firstLeg; leg;) {
        TransLeg *toDelete = leg;
        leg = leg->next;
        delete toDelete;
    }
    firstLeg = 0;
    total_fare = 0;
}

std::unordered_map<std::string, Flight *> Transportation::findLegFlight(const Schedule &schedule,
                                                                        const char *depPoint,
                                                                        const char *arrPoint) {
    Flight *flightWithMinimalFare = 0;
    std::unordered_map<std::string, Flight *> carrier_to_legs;

    ScheduleItem *schedItem = 0;
    while (schedule.iterator(schedItem)) {
        if (0 != strcmp(schedItem->depPoint, depPoint) ||
            0 != strcmp(schedItem->arrPoint, arrPoint))
            continue;
        if (!flightWithMinimalFare || flightWithMinimalFare->fare > schedItem->fare) {
            flightWithMinimalFare = schedItem;
            carrier_to_legs["MinFare"] = schedItem;
        }
        carrier_to_legs[schedItem->carrier] = schedItem;
    }

    return std::move(carrier_to_legs);
}

int Transportation::buildCheapest(const Route &route, const Schedule &schedule) {
    RoutePoint *routePoint = 0;

    // ������� �� ����� ������� �������� ��� ������� �����������, ���� ��� ���� ���� ��� �������� �
    // ����������� ������� �� ��������� ������ ������������ � carrier_transportation
    //������ - ����� ���������, ������ ��������� fistLag, ��������� ����� lastLag
    std::unordered_map<std::string, std::tuple<Fare, TransLeg *, TransLeg *>> carrier_transportation;

    while (route.iterator(routePoint) && routePoint->next) {
        std::unordered_map<std::string, Flight *> Legs = findLegFlight(schedule, routePoint->point,
                                                                       routePoint->next->point);
        if (Legs.empty()) return 1;

        //
        if (carrier_transportation.empty()) {
            //��� ����� ������ ������� � ��������� ������ ���������
            for (const auto&[carrier, flight] : Legs) {
                TransLeg *newLeg = new TransLeg;
                newLeg->flight = *flight;
                carrier_transportation[carrier] = std::make_tuple(newLeg->flight.fare, newLeg, newLeg);
            }
        } else {
            for (auto it = carrier_transportation.begin(); it != carrier_transportation.end();) {
                //������ ������ ������ ������������ ��� ������
                //������ ��, ��� ������� �� ������� �������� �������� - ��������� ������ �� ����������
                auto it_f = Legs.find(it->first);
                if (it_f == Legs.end()) {
                    it = carrier_transportation.erase(it);
                    continue;
                }
                //������� ������� � ����� ��������� � �������� �����
                TransLeg *newLeg = new TransLeg;
                newLeg->flight = *it_f->second;
                auto&[sum, f_lag, lastlag] = it->second;
                lastlag->next = newLeg;         //��������� �������
                lastlag = newLeg;               //������� �����
                sum += newLeg->flight.fare;    //����������� ����� �������� ��� ������, ����� �� ������ �����������
                it++;

            }
            //���� ���� �����-�� ���������� ������� �������� �������� ������ ��� � �������� �������� -
            // ��� �� ������ � ���������� ����, �� ��� ��� � �� �����
        }
    }
    //� ������� ������ ��������� ����� ��� ������������, � ������� ���� ��� ����������� ��������
    //� ����� ����������� ������� �� ��������� ������ ������������
    //������ ����� ����� ����������� ������� ����� ���� � ������ ������ 80% ��� ������ ����������� �� ����� ��������
    flush();

    total_fare = std::numeric_limits<Fare>::max();
    for (const auto &ct : carrier_transportation) {
        auto&[ct_sum, f_lag, lastlag] = ct.second;
        if ((ct.first == "MinFare") && (total_fare > ct_sum)) {
            total_fare = ct_sum;
            firstLeg = f_lag;
        } else if ((ct.first != "MinFare") && (total_fare > ct_sum * 0.8) && f_lag->next) {
            total_fare = ct_sum * CONST_DISCOUNT;
            firstLeg = f_lag;
        }
    }

    return 0;
}

void Transportation::print() const {
    int legNo = 0;
    for (TransLeg *leg = firstLeg; leg; leg = leg->next) {
        printf("% 2d: ", legNo++);
        leg->flight.print();
        printf("\n");
    }
    printf("Total fare: %.4f\n", total_fare); //format change
}


//___

int main() {
    // ������ �������
    Route route;
    if (route.read("route.txt")) {
        fprintf(stderr, "cannot read route\n");
        return 1;
    }
    route.print("Route read: ");
    if (route.check()) {
        fprintf(stderr, "route is invalid\n");
        return 1;
    }

    // ������ ����������
    Schedule schedule;
    if (schedule.read("schedule.txt")) {
        fprintf(stderr, "cannot read schedule\n");
        return 1;
    }
    printf("\nSchedule read:\n");
    schedule.print();

    // ������ ���������
    Transportation trans;
    if (trans.buildCheapest(route, schedule)) {
        fprintf(stderr, "cannot build transportation\n");
        return 1;
    }

    printf("\nCheapest transportation:\n");
    trans.print();

    return 0;
}