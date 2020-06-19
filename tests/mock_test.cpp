/**
 * @file mock_test.cpp
 */
#include "catch.hpp"
#include "mlboard.hpp"

/**
 * Test the MOCK.
 */
TEST_CASE("MOCK_TEST", "MOCK")
{
    mlboard::FileWriter f1("jeffin");
    f1.close(); 
    REQUIRE(1 <= 2);
}