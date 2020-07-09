/**
 * @file util_test.cpp
 * @author Jeffin Sam
 */
#include <mlboard/core.hpp>
#include "catch.hpp"


/**
 * Test EncodeImage utitlity function.
 */
TEST_CASE("Test EncodeImage utitlity function", "[UtilFunction]")
{
	std::vector<std::string> filePaths = {"data/multiple_image.jpg",
			"data/single_image.jpg"};
	std::vector<std::string> encodeImage;
	mlboard::util::EncodeImage(filePaths,encodeImage);
	REQUIRE(encodeImage.size() == 2);
	REQUIRE(encodeImage[0].length() > 0);
	REQUIRE(encodeImage[1].length() > 0);
}
