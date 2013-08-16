#include "voting/Position.hpp"
#include <cmath>

namespace voting {

Position::Position(std::initializer_list<double> position) : pos_(position) {
    if (pos_.empty())
        throw std::out_of_range("Cannot initialize a Position with 0 dimensions");
}

Position::Position(const int &dimensions) : pos_(dimensions, 0) {
    if (pos_.empty())
        throw std::out_of_range("Cannot initialize a Position with 0 dimensions");
}

double Position::distance(const Position &other) const {
    requireSameDimensions(other, "Position::distance");

    if (dimensions() == 1)
        return fabs(operator[](0) - other[0]);

    if (dimensions() == 2)
        return hypot(operator[](0) - other[0], operator[](1) - other[1]);

    double dsq = 0;
    for (int i = 0; i < dimensions(); ++i)
        dsq += pow(operator[](i) - other[i], 2);
    return sqrt(dsq);
}

Position Position::mean(const Position &other, const double &weight) const {
    requireSameDimensions(other, "Position::mean");

    Position result(dimensions());
    double our_weight = 1.0-weight;

    for (int i = 0; i < dimensions(); i++)
        result[i] = our_weight*operator[](i) + weight*other[i];

    return result;
}

Position& Position::operator=(const Position &new_pos) {
    requireSameDimensions(new_pos, "Position::operator=");

    for (int i = 0; i < dimensions(); i++)
        operator[](i) = new_pos[i];

    return *this;
}

std::ostream& operator<<(std::ostream &os, const Position &p) {
    os << "Position[" << p[0];
    for (int i = 1; i < p.dimensions(); i++)
        os << ", " << p[i];
    os << "]";
    return os;
}

}
