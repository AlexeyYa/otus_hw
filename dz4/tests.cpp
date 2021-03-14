/*! @file tests.cpp
 *  @brief file with tests
 */

#include "ip_struct.h"

#include <gtest/gtest.h>

/*! @brief test for creating from int with only first byte
 */
TEST(IP_ADDRESS, FromInt) {
    // Arrange

    // Act
    ip_address ip = make_ip_address(44);

    // Assert
    ASSERT_EQ(ip.to_string(), "0.0.0.44");
}

/*! @brief test for creating from int
 */
TEST(IP_ADDRESS, FromIntFull) {
    // Arrange

    // Act
    ip_address ip = make_ip_address(0xFFFFFFFF);

    // Assert
    ASSERT_EQ(ip.to_string(), "255.255.255.255");
}

/*! @brief test for creating from char array
 */
TEST(IP_ADDRESS, FromCharAst) {
    // Arrange

    // Act
    ip_address ip = make_ip_address("44c");

    // Assert
    ASSERT_EQ(ip.to_string(), "44c");
}

/*! @brief test for creating from string
 */
TEST(IP_ADDRESS, FromString) {
    // Arrange
    std::string s{"www"};

    // Act
    ip_address ip = make_ip_address(s);

    // Assert
    ASSERT_EQ(ip.to_string(), "www");
}

/*! @brief test for creating from tuple
 */
TEST(IP_ADDRESS, FromTuple) {
    // Arrange
    std::tuple<unsigned char, unsigned char, unsigned char> s{255, 1, 200};

    // Act
    ip_address ip = make_ip_address(s);

    // Assert
    ASSERT_EQ(ip.to_string(), "255.1.200");
}

/*! @brief test for creating from vector
 */
TEST(IP_ADDRESS, FromVector) {
    // Arrange
    std::vector<unsigned char> s{255, 1, 200};

    // Act
    ip_address ip = make_ip_address(s);

    // Assert
    ASSERT_EQ(ip.to_string(), "255.1.200");
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
