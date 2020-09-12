#include <gtest/gtest.h>

#include "core/quaternion.h"
#include "core/vector3.h"

TEST(quaternion, basic_constructor)
{
    eng::Quaternion q{ };
    ASSERT_EQ(q, eng::Quaternion(0.0f, 0.0f, 0.0f, 1.0f));
}

TEST(quaternion, axis_angle_constructor)
{
    eng::Quaternion q{ { 1.0f, 0.0f, 0.0f }, 0.5f };
    ASSERT_EQ(q, eng::Quaternion(0.2474039644f, 0.0f, 0.0f, 0.9689124227f));
}

TEST(quaternion, compontent_constructor)
{
    eng::Quaternion q{ 1.0f, 2.0f, 3.0f, 4.0f };
    ASSERT_EQ(q, eng::Quaternion(1.0f, 2.0f, 3.0f, 4.0f));
}

TEST(quaternion, multiply)
{
    eng::Quaternion q1{ { 1.0f, 0.0f, 0.0f }, 0.5f };
    eng::Quaternion q2{ { 0.0f, 0.0f, 1.0f }, 0.2f };
    auto q3 = q1 * q2;

    ASSERT_EQ(q3, eng::Quaternion(0.2461679727f, -0.02469918504f, 0.09672984481f, 0.9640719295f));
}

TEST(quaternion, multiply_assignment)
{
    eng::Quaternion q1{ { 1.0f, 0.0f, 0.0f }, 0.5f };
    eng::Quaternion q2{ { 0.0f, 0.0f, 1.0f }, 0.2f };
    q1 *= q2;

    ASSERT_EQ(q1, eng::Quaternion(0.2461679727f, -0.02469918504f, 0.09672984481f, 0.9640719295f));
}

TEST(quaternion, vector_addition)
{
    eng::Quaternion q1{ { 1.0f, 0.0f, 0.0f }, 0.5f };
    eng::Vector3 v{ 0.0f, 0.0f, 1.0f };
    auto q2 = q1 + v;

    ASSERT_EQ(q2, eng::Quaternion(0.2474039644f, 0.1237019822f, 0.4844562113f, 0.9689124227f));
}

TEST(quaternion, vector_addition_assignment)
{
    eng::Quaternion q{ { 1.0f, 0.0f, 0.0f }, 0.5f };
    eng::Vector3 v{ 0.0f, 0.0f, 1.0f };
    q += v;

    ASSERT_EQ(q, eng::Quaternion(0.2474039644f, 0.1237019822f, 0.4844562113f, 0.9689124227f));
}

TEST(quaternion, scale)
{
    const eng::Quaternion q1{ 1.0f, 2.0f, 3.0f, 4.0f };
    const auto q2 = q1 * 1.5f;

    ASSERT_EQ(q2, eng::Quaternion(1.5f, 3.0f, 4.5f, 6.0f));
}

TEST(quaternion, scale_assignment)
{
    eng::Quaternion q{ 1.0f, 2.0f, 3.0f, 4.0f };
    q *= 1.5f;

    ASSERT_EQ(q, eng::Quaternion(1.5f, 3.0f, 4.5f, 6.0f));
}

TEST(quaternion, subtraction)
{
    const eng::Quaternion q1(1.0f, 2.0f, 3.0f, 4.0f);
    const auto q2 = q1 - eng::Quaternion{ 0.1f, 0.2, 0.3f, 0.4f };

    ASSERT_EQ(q2, eng::Quaternion(0.9f, 1.8f, 2.7f, 3.6f));
}

TEST(quaternion, subtraction_assignment)
{
    eng::Quaternion q(1.0f, 2.0f, 3.0f, 4.0f);
    q -= eng::Quaternion{ 0.1f, 0.2, 0.3f, 0.4f };

    ASSERT_EQ(q, eng::Quaternion(0.9f, 1.8f, 2.7f, 3.6f));
}

TEST(quaternion, addition)
{
    const eng::Quaternion q1(1.0f, 2.0f, 3.0f, 4.0f);
    const auto q2 = q1 + eng::Quaternion{ 0.1f, 0.2, 0.3f, 0.4f };

    ASSERT_EQ(q2, eng::Quaternion(1.1f, 2.2f, 3.3f, 4.4f));
}

TEST(quaternion, addition_assignment)
{
    eng::Quaternion q(1.0f, 2.0f, 3.0f, 4.0f);
    q += eng::Quaternion{ 0.1f, 0.2, 0.3f, 0.4f };

    ASSERT_EQ(q, eng::Quaternion(1.1f, 2.2f, 3.3f, 4.4f));
}

TEST(quaternion, negate)
{
    eng::Quaternion q(1.0f, 2.0f, 3.0f, 4.0f);
    
    ASSERT_EQ(-q, eng::Quaternion(-1.0f, -2.0f, -3.0f, -4.0f));
}

TEST(quaternion, dot)
{
    const eng::Quaternion q1(1.1f, 2.2f, 1.1f, 2.2f);
    const eng::Quaternion q2(0.1f, 0.2f, 0.3f, 0.4f);

    ASSERT_EQ(q1.dot(q2), 1.76f);
}

TEST(quaternion, slerp)
{
    eng::Quaternion q1(1.1f, 2.2f, 1.1f, 2.2f);
    const eng::Quaternion q2(0.1f, 0.2f, 0.3f, 0.4f);

    q1.slerp(q2, 0.5f);

    ASSERT_EQ(q1, eng::Quaternion(0.3007528484f, 0.6015056968f, 0.3508783281f, 0.6516311169f));
}

TEST(quaternion, normalise)
{
    eng::Quaternion q{ 1.0f, 2.0f, 3.0f, 4.0f };
    q.normalise();

    ASSERT_EQ(q, eng::Quaternion(0.1825741827f, 0.3651483655f, 0.5477225184f, 0.730296731f));
}

