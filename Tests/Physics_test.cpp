#include "gtest/gtest.h"
#include "pch.h"
#include"../16_SFML_Games/Physics.h"

// 1. TEST APPLY THRUST
TEST(PhysicsTest, ApplyThrust_IncreasesVelocityCorrectly)
{
    float dx = 0, dy = 0;

    Physics::applyThrust(dx, dy, 0, 1.0f);

    EXPECT_FLOAT_EQ(dx, 1.0f);
    EXPECT_FLOAT_EQ(dy, 0.0f);
}

// 2. TEST SLOWS DOWN CORRECTLY
TEST(PhysicsTest, ApplyDrag_ReducesVelocityCorrectly)
{
    float dx = 10, dy = -10;

    Physics::applyDrag(dx, dy, 0.5f);

    EXPECT_FLOAT_EQ(dx, 5.0f);
    EXPECT_FLOAT_EQ(dy, -5.0f);
}