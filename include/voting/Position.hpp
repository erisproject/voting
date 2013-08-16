#pragma once
//#include <exception>
#include <stdexcept>
#include <vector>
#include <ostream>

namespace voting {

/** Class representing a geometric position of arbitrary dimensions.
 */
class Position final {
    public:
        Position() = delete;
        /** Creates a new Position object at the given position.  The dimension is determined from
         * the number of values provided to the constructor.  At least one value must be given.
         */
        Position(std::initializer_list<double> position);

        /** Creates a new Position object of the given number of dimensions (>= 1) with initial
         * position of 0 for all dimensions.
         */
        Position(const int &dimensions);

        Position(double) = delete;

        /** Returns the (Euclidean) distance between one position and another.  Throws a
         * std::length_error exception if the two objects are of different dimensions.  The value
         * returned will always be non-negative.
         */
        double distance(const Position &other) const;

        /** Returns the weighted-mean of this Position object and another Position object.
         *
         * \param other the other Position
         * \param weight the weight to apply to this object, defaulting to 0.5.  The weight controls
         * how far along a line from this object to `other` the returned point will be.  The default,
         * 0.5, returns the midpoint between this and `other`.  A value of 1 results in the same
         * Position as `other`; a value of 0 returns the same Position as this object.  Values
         * greater than 1 and less than 0 are allowed: they return a Position along the line through
         * `this` and `other`, but outside the chord between them.
         * \throws std::length_error if the two Position objects are of different dimensions.
         */
        Position mean(const Position &other, const double &weight = 0.5) const;

        /** Accesses the Position's `d`th coordinate, where `d=0` is the first dimension, etc.
         *
         * \throws std::out_of_range exception for `d >= dimensions()`.
         */
        double& operator[](int d);

        /// Const access to Position coordinates.
        const double& operator[](int d) const;

        /** Sets the position to that of the provided position.  The dimensions of the current and
         * new position must be identical.
         */
        Position& operator=(const Position &new_pos);

        /** Returns the number of dimensions of this Position object.  This will always be at least
         * 1.
         */
        int dimensions() const noexcept;

        /** Overloaded so that a Position can be sent to an output stream, resulting in output such
         * as `Position[0.25, 0, -44.3272]`.
         */
        friend std::ostream& operator<<(std::ostream &os, const Position &p);

    private:
        /// The vector
        std::vector<double> pos_;

        /// Throws an exception if the current object and `other` have different dimensions.
        void requireSameDimensions(const Position &other, const std::string &method) const;
};

inline int Position::dimensions() const noexcept {
    return pos_.size();
}

inline void Position::requireSameDimensions(const Position &other, const std::string &method) const {
    if (dimensions() != other.dimensions())
        throw std::length_error(method + "() called with objects of differing dimensions");
}

inline double& Position::operator[](int d) { return pos_.at(d); }
inline const double& Position::operator[](int d) const { return pos_.at(d); }

}
