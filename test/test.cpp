#include <gtest/gtest.h>

int function()
{
    return 0;
}

TEST(DemoTest, firstTest){
    EXPECT_EQ(function(), 0);
}