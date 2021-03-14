#include "tutorial.h"

#include <gtest/gtest.h>

#ifdef USE_VERSION
TEST(Version, Major) {
    // Arrange

    // Act (empty for this test)

    // Assert
    ASSERT_EQ(HelloWorld_VERSION_MAJOR, 0);
}

TEST(Version, Minor) {
    // Arrange

    // Act (empty for this test)

    // Assert
    ASSERT_EQ(HelloWorld_VERSION_MINOR, 1);
}

TEST(Version, Patch) {
    // Arrange

    // Act (empty for this test)

    // Assert
    ASSERT_GT(HelloWorld_VERSION_PATCH, 0);
}
#endif

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}