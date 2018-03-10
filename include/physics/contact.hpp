#pragma once

#include "matrix3.hpp"
#include "rigid_body.hpp"
#include "vector3.hpp"

namespace eng
{

/**
 * This struct encapsulates all the data generated by the collision detector,
 * which is then used by the collision resolver to handle two rigid bodies
 * colliding.
 *
 * Internally this holds two raw pointers to the two colliding bodies. This is
 * ok as this struct is a lightweight data structure for current collisions. So
 * if it only used within the scope of the physics engine where all rigid bodies
 * are in scope then they should never be left dangling.
 */
struct contact final
{
    /**
     * Create a new contact object.
     *
     * @param position
     *   Position in world space of collision point.
     *
     * @param normal
     *   Normal to direction the bodies collided at.
     *
     * @param penetration
     *   How far the bodies have penetrated each other.
     *
     * @param body1
     *   A pointer to the first colliding body.
     *
     * @param body2
     *   A pointer to the second colliding body.
     */
    contact(
        const vector3 &position,
        const vector3 &normal,
        const float penetration,
        rigid_body * const body1,
        rigid_body * const body2);

    /** Default */
    ~contact() = default;
    contact(const contact&) = default;
    contact& operator=(const contact&) = default;
    contact(contact&&) = default;
    contact& operator=(contact&&) = default;

    /*
     * Update the required change in velocity. This is used internally by the
     * physics engine and should never need to be called manually.
     */
    void update_desired_delta_velocity();

    /** Position in world space of collision point. */
    vector3 position;

    /** Normal to direction the bodies collided at. */
    vector3 normal;

    /** How far the bodies have penetrated each other. */
    float penetration;

    /** A pointer to the first colliding body. */
    rigid_body *body1;

    /** A pointer to the second colliding body. */
    rigid_body *body2;

    /**
     * A transformation matrix that is used to transform world coordinates into
     * coordinates relative to the contact coordinates.
     */
    matrix3 contact_basis;

    /** Vector from centre of body1 to point of contact. */
    vector3 relative_contact_position1;

    /**
     * Vector from centre of body2 to point of contact, if body2 is static then
     * this is a zero vector.
     * */
    vector3 relative_contact_position2;

    /** The total velocity of both bodies at the point of contact. */
    vector3 closing_velocity;

    /**
     * Required change in velocity to resolve collision. */
    float desired_delta_velocity;
};

}
