/**
 * @file filewriter_test.cpp
 * @author Jeffin Sam
 */
#include "catch.hpp"
#include <mlboard/core.hpp>
#include <sstream>
#include <cstdio>
#include <sys/stat.h>

// For windows mkdir.
#ifdef _WIN32
    #include <direct.h>
#endif

class SummaryWriterTestsFixture {
  public:
   static mlboard::FileWriter* f1;
	 static size_t currentSize;
 };

mlboard::FileWriter* SummaryWriterTestsFixture::f1;
size_t SummaryWriterTestsFixture::currentSize = 0;

/**
 * Test the Image summary.
 */
TEST_CASE_METHOD(SummaryWriterTestsFixture, "Writing a Image summary to file", "[SummaryWriter]")
{	
	f1 = new mlboard::FileWriter("_templogs");
	std::ifstream fin("./data/multiple_image.jpg", std::ios::binary);
	std::ostringstream ss;

	ss << fin.rdbuf();
	std::string image(ss.str());
	fin.close();
	mlboard::SummaryWriter<mlboard::FileWriter>::Image(
				"Test Image", 1, image, 512, 512, 3, *f1, "Sample Image",
				"This is a Sample image logged using mlboard.");
}

/**
 * Test the scaler summary.
 */
TEST_CASE_METHOD(SummaryWriterTestsFixture, "Writing a scaler summary to file", "[SummaryWriter]")
{	
  // struct stat results;

  for (int i = 1; i < 25; i++)
  {
		mlboard::SummaryWriter<mlboard::FileWriter>::Scalar("scalerSample_1",
		i, 1.1, *f1);
  }  
}

/**
 * Test multiple Image summary.
 */
TEST_CASE_METHOD(SummaryWriterTestsFixture, "Writing multiple Images summary to file", "[SummaryWriter]")
{	
	arma::Mat<unsigned char> matrix;
	mlpack::data::ImageInfo info;
	std::vector<std::string> files = {"./data/single_image.jpg",
				"./data/single_image.jpg"};

	// Creating the matrix which has image.
	mlpack::data::Load(files, matrix, info, false);

	// Now we can log the matrix.
	mlboard::SummaryWriter<mlboard::FileWriter>::Image(
				"Multiple Image", 1, matrix, info, *f1, "Sample Multiple Image",
				"This is a Sample multiple image logged using mlboard.");

	f1->Close();

	#ifndef KEEP_LOGS
		remove(f1.FileName().c_str());
	#endif

}

