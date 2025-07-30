#include <gtest/gtest.h>
#include "main.hpp"

TEST(MainTest, StripTest) {
  std::string str1 = "   Hello, World!   ";
  strip(str1);
  EXPECT_EQ(str1, "Hello, World!");

  std::string str2 = "\n\t  Leading and trailing whitespace \r\n";
  strip(str2);
  EXPECT_EQ(str2, "Leading and trailing whitespace");

  std::string str3 = "NoWhitespace";
  strip(str3);
  EXPECT_EQ(str3, "NoWhitespace");

  std::string str4 = "    ";
  strip(str4);
  EXPECT_EQ(str4, "");
}

TEST(MainTest, ParseValidInput) {
  std::istringstream input("0.0 1.0 2.0 3.0\n1.5 4.0 5.0 6.0\n");
  auto result = parse(input);
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result->size(), 2);
  EXPECT_DOUBLE_EQ(result->at(0).first, 0.0);
  EXPECT_DOUBLE_EQ(result->at(0).second.x, 1.0);
  EXPECT_DOUBLE_EQ(result->at(0).second.y, 2.0);
  EXPECT_DOUBLE_EQ(result->at(0).second.z, 3.0);
  EXPECT_DOUBLE_EQ(result->at(1).first, 1.5);
  EXPECT_DOUBLE_EQ(result->at(1).second.x, 4.0);
  EXPECT_DOUBLE_EQ(result->at(1).second.y, 5.0);
  EXPECT_DOUBLE_EQ(result->at(1).second.z, 6.0);
}

TEST(MainTest, ParseInvalidInput) {
  std::istringstream input1("0.0 1.0 2.0\n1.5 4.0 5.0 6.0\n");  // Missing z coordinate
  auto result1 = parse(input1);
  ASSERT_FALSE(result1.has_value());

  std::istringstream input2("1.5 4.0 5.0 6.0\n0.0 1.0 2.0 3.0\n");  // Non-decreasing delay
  auto result2 = parse(input2);
  ASSERT_FALSE(result2.has_value());

  std::istringstream input3("-1.0 1.0 2.0 3.0\n1.5 4.0 5.0 6.0\n");  // Negative delay
  auto result3 = parse(input3);
  ASSERT_FALSE(result3.has_value());

  std::istringstream input4("abc def ghi jkl\n");  // Non-numeric input
  auto result4 = parse(input4);
  ASSERT_FALSE(result4.has_value());
}
