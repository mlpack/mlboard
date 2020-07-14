/**
 * @file test/summarywriter_test.cpp
 * @author Jeffin Sam
 */
#include "catch.hpp"
#include <mlboard/mlboard.hpp>
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
 * Test the scaler summary.
 */
TEST_CASE_METHOD(SummaryWriterTestsFixture, "Writing a scaler summary to file",
                 "[SummaryWriter]")
{	
  f1 = new mlboard::FileWriter("_templogs");
  for (int i = 1; i < 25; i++)
  {
    mlboard::SummaryWriter<mlboard::FileWriter>::Scalar("scalerSample_1",
      i, 1.1, *f1);
  }
}

/**
 * Test multiple Image summary.
 */
TEST_CASE_METHOD(SummaryWriterTestsFixture, "Writing text summary to file",
                 "[SummaryWriter]")
{	
  mlboard::SummaryWriter<mlboard::FileWriter>::Text("add Text support ", 1,
      "mlpack is great", *f1);
  mlboard::SummaryWriter<mlboard::FileWriter>::Text("add Text support", 2,
      " Project developed during GSoc 2020 ", *f1);
  f1->Close();

  #ifndef KEEP_LOGS
    remove(f1->FileName().c_str());
  #endif
}
