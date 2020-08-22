/**
 * @file tests/callback_test.cpp
 * @author Jeffin Sam
 */
#include "catch.hpp"
#include <mlboard/mlboard.hpp>
#include <mlpack/methods/ann/ffn.hpp>
#include <mlpack/methods/ann/loss_functions/mean_squared_error.hpp>
#include <mlpack/methods/logistic_regression/logistic_regression.hpp>
#include <sstream>
#include <cstdio>
#include <sys/stat.h>

// For windows mkdir.
#ifdef _WIN32
    #include <direct.h>
#endif

using namespace mlpack::ann;
using namespace mlpack;
using namespace mlpack::regression;

class CallBackTestsFixture 
{
 public:
  static mlboard::FileWriter* f1;
  static size_t currentSize;
  static bool deleteLogs;
  CallBackTestsFixture()
  {
    if(deleteLogs)
    {
      #if defined(_WIN32)
        _mkdir("_templogscallback");
      #else
        mkdir("_templogscallback", 0777);
      #endif
      deleteLogs = false;
    }
  };  
};

mlboard::FileWriter* CallBackTestsFixture::f1;
size_t CallBackTestsFixture::currentSize = 0;
bool CallBackTestsFixture::deleteLogs = true;

/**
 * Test callback summary.
 */
TEST_CASE_METHOD(CallBackTestsFixture,
                 "Writing summary using callback to file", "[CallBack]")
{	
  f1 = new mlboard::FileWriter("_templogscallback");
  arma::mat data;
  arma::mat labels;

  data::Load("./data/lab1.csv", data, true);
  data::Load("./data/lab3.csv", labels, true);

  FFN<MeanSquaredError<>, RandomInitialization> model;

  model.Add<Linear<>>(1, 2);
  model.Add<SigmoidLayer<>>();
  model.Add<Linear<>>(2, 1);
  model.Add<SigmoidLayer<>>();

  std::stringstream stream;
  ens::StandardSGD opt(0.1, 1, 1000);
  model.Train(data, labels, opt, ens::MlboardLogger(*f1));
}

/**
 * Test callback via function constructor summary.
 */
TEST_CASE_METHOD(CallBackTestsFixture,
                 "Writing summary using second constructor callback to file",
                 "[CallBack]")
{	
  deleteLogs = true;
  arma::mat data("1 2 3;"
                 "1 2 3");
  arma::Row<size_t> responses("1 1 0");

  ens::StandardSGD sgd(0.1, 1, 50);
  LogisticRegression<> logisticRegression(data, responses, sgd, 0.001);
  std::stringstream stream;

  ens::MlboardLogger cb(*f1, 
        [&]()
      {
        return logisticRegression.ComputeAccuracy(data, responses)/100;
      }, 1,
      "lraccuracy","lrloss"
  );
  // Now train a logistic regression object on it.
  logisticRegression.Train<ens::StandardSGD>(data, responses, sgd,
                                             cb);
  f1->Close();

  #ifndef KEEP_TEST_LOGS
    remove(f1->FileName().c_str());
  #endif
}
