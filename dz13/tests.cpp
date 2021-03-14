/*! @file tests.cpp
 *  @brief Тесты
 */

#define _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING

#include <gtest/gtest.h>
#include "server.h"
#include <sstream>

class TestSession : public Isession
{
public:
    void do_write(const std::string &content) override
    {
        out_value = content;
    }

    std::string out_value;
} TestSession_;

std::shared_ptr test_session = std::make_shared<TestSession>();

/*! @brief Тест на смену размера
 */
TEST(COLLECTIONSDATA, Insert) {
    // Arrange

    // Act
    std::string str = CollectionsData_.Insert('A', 1, "Val");

    // Assert
    ASSERT_EQ(str, "OK\n");
}

TEST(COLLECTIONSDATA, InsertDuplicate) {
    // Arrange

    // Act
    std::string str = CollectionsData_.Insert('A', 1, "Val");

    // Assert
    ASSERT_EQ(str, "ERR duplicate 1\n");
}

TEST(COLLECTIONSDATA, SymmetricDifference) {
    // Arrange

    // Act
    std::string str = CollectionsData_.Difference();

    // Assert
    ASSERT_EQ(str, "1,Val,\nOK\n");
}

TEST(COLLECTIONSDATA, Intersection) {
    // Arrange

    // Act
    std::string str1 = CollectionsData_.Intersect();
    CollectionsData_.Insert('B', 1, "Val2");
    std::string str2 = CollectionsData_.Intersect();

    // Assert
    ASSERT_EQ(str1, "OK\n");
    ASSERT_EQ(str2, "1,Val,Val2\nOK\n");
}

TEST(COLLECTIONSDATA, SymmetricDifference2) {
    // Arrange

    // Act
    std::string str = CollectionsData_.Difference();

    // Assert
    ASSERT_EQ(str, "OK\n");
}

TEST(COMMAND, Insert) {
    // Arrange
    InsertCommand cmd(test_session, 'A', 2, "A2");

    // Act
    cmd.Execute();
    std::string str = test_session->out_value;

    // Assert
    ASSERT_EQ(str, "OK\n");
}

//
//

TEST(PARSE, InvalidCmd){
    // Arrange
    Parse("Invalid", test_session);

    // Act
    std::shared_ptr<Command> cmd;
    CollectionsData_.CommandQueue.wpop(cmd);
    cmd->Execute();
    std::string str = test_session->out_value;

    // Assert
    ASSERT_EQ(str, "ERR invalid command\n");
}

int main(int argc, char** argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
