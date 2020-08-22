/**
 * @file tests/summarywriter_test.cpp
 * @author Jeffin Sam
 */
#include "catch.hpp"
#include <mlboard/mlboard.hpp>
#include <sstream>
#include <cstdio>
#include <sys/stat.h>
#include <random>

// For windows mkdir.
#ifdef _WIN32
    #include <direct.h>
#endif

class SummaryWriterTestsFixture 
{
 public:
  static mlboard::FileWriter* f1;
  static size_t currentSize;
  static bool deleteLogs;
  SummaryWriterTestsFixture()
  {
    if(deleteLogs)
    {
      #if defined(_WIN32)
        _mkdir("_templogs");
      #else
        mkdir("_templogs", 0777);
      #endif
      deleteLogs = false;
    }
  };  
};

mlboard::FileWriter* SummaryWriterTestsFixture::f1;
size_t SummaryWriterTestsFixture::currentSize = 0;
bool SummaryWriterTestsFixture::deleteLogs = true;

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
TEST_CASE_METHOD(SummaryWriterTestsFixture, "Writing a scaler summary to file",
                 "[SummaryWriter]")
{	
  for (int i = 1; i < 25; i++)
  {
    mlboard::SummaryWriter<mlboard::FileWriter>::Scalar("scalerSample_1",
      i, 1.1, *f1);
  }
}

/**
 * Test the PRCurve summary.
 */
TEST_CASE_METHOD(SummaryWriterTestsFixture, "Writing a PrCurve summary to file",
                 "[SummaryWriter]")
{
  std::vector<double> labels = {1, 1, 1, 1, 1, 1, 1, 1, 0, 1};
  std::vector<double> predictions = {0.6458941, 0.3843817, 0.4375872,
      0.2975346, 0.891773, 0.05671298, 0.96366274, 0.2726563,
      0.3834415, 0.47766513};
  mlboard::SummaryWriter<mlboard::FileWriter>::PRCurve("test_pr_curve",
      labels, predictions, *f1);
}

/**
 * Test the PRCurve summary using arma vec.
 */
TEST_CASE_METHOD(SummaryWriterTestsFixture,
    "Writing a PrCurve summary using arma vec file", "[SummaryWriter]")
{
  arma::rowvec labels = {1, 1, 1, 1, 1, 1, 1, 1, 0, 1};
  arma::rowvec predictions = {0.6458941, 0.3843817, 0.4375872,
      0.2975346, 0.891773, 0.05671298, 0.96366274, 0.2726563,
      0.3834415, 0.47766513};
  mlboard::SummaryWriter<mlboard::FileWriter>::PRCurve("test_pr_curve_arma_vec",
      labels, predictions, *f1);
}

/**
 * Test text summary.
 */
TEST_CASE_METHOD(SummaryWriterTestsFixture, "Writing text summary to file",
                 "[SummaryWriter]")
{	
  mlboard::SummaryWriter<mlboard::FileWriter>::Text("add Text support ", 1,
      "Test case for checking text support in mlboard.", *f1);
  mlboard::SummaryWriter<mlboard::FileWriter>::Text("add Text support", 2,
      " Project developed during GSoc 2020 ", *f1);
}

/**
 * Test Histogram summary.
 */
TEST_CASE_METHOD(SummaryWriterTestsFixture, "Writing histogram summary to file",
                 "[SummaryWriter]")
{	
  std::default_random_engine generator;
  std::normal_distribution<double> default_distribution(0, 1.0);
  for (int i = 0; i < 10; ++i)
  {
    std::normal_distribution<double> distribution(i * 0.1, 1.0);
    std::vector<double> values;
    for (int j = 0; j < 10000; ++j)
    {
      values.push_back(distribution(generator));
    }
    mlboard::SummaryWriter<mlboard::FileWriter>::Histogram("SampleHistogram",
        i, values, *f1);
  }
}

/**
 * Test Histogram summary using arma rowvec.
 */
TEST_CASE_METHOD(SummaryWriterTestsFixture,
                "Writing histogram summary using arma rowvec to file",
                "[SummaryWriter]")
{	
  std::default_random_engine generator;
  std::normal_distribution<double> default_distribution(0, 1.0);
  for (int i = 0; i < 10; ++i)
  {
    std::normal_distribution<double> distribution(i * 0.1, 1.0);
    std::vector<double> values;
    for (int j = 0; j < 10000; ++j)
    {
      values.push_back(distribution(generator));
    }

    // create arma vec.
    arma::rowvec tempValues(values);
    mlboard::SummaryWriter<mlboard::FileWriter>::Histogram("ArmaHistogram",
        i, tempValues, *f1);
  }
}

 /**
 * Test embedding support.
 */
TEST_CASE_METHOD(SummaryWriterTestsFixture,
                 "Writing embedding summary to file", "[SummaryWriter]")
{
  arma::mat temp;
  mlpack::data::Load("./data/vecs.tsv", temp);
  std::vector<std::string> meta;
  std::string line;
  std::ifstream meta_file("./data/meta.tsv");
  while (getline(meta_file, line))
  {
    meta.push_back(line);
  }
  meta_file.close();

  // Make sure you pass the correct dimensions here.
  // Here we transpose the matrix since our dataset was loaded according to mlpack convention.
  // Row is feature and column is data point.
  mlboard::SummaryWriter<mlboard::FileWriter>::Embedding("vocab", temp.t(), meta, *f1);
}

/**
 * Test multiple Image summary.
 */
TEST_CASE_METHOD(SummaryWriterTestsFixture, "Writing multiple Images summary to file",
                 "[SummaryWriter]")
{	
  deleteLogs = true;
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

  #ifndef KEEP_TEST_LOGS
    remove(f1->FileName().c_str());
  #endif
}
