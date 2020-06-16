# mlboard
mlpack visualization tool

mlboard is a tool that allows you to log mlpack data in the format that the TensorBoard frontend can render in browsers.

mlboard is still in the development phase. You can track the development ideas [here](https://www.mlpack.org/gsocblog/Jeffin2020CBP.html)

### 0. Contents

  1. [Building mlboard from source](#1-building-mlboard-from-source)
  2. [Usage](#2-usage)
  3. [Supported Summary Types](#3-supported-summary-types)

### 1. Building the source code 

For mlboard to run, you have to install protobuf

- For Mac OS 

```
brew install protobuf
```

- For debian-based systems

```
sudo apt-get install libprotobuf-dev protobuf-compiler
```

would be sufficient. 

Make a build directory.  The directory can have any name, but 'build' is
sufficient.

```
    $ mkdir build
    $ cd build
```

The next step is to run CMake to configure the project.  Running CMake is the
equivalent to running `./configure` with autotools. 

```
    $ cmake ../
```

Note : The test build are by default `ON`, you can avoid building test by providing args to CMake as `cmake -DBUILD_TESTS=OFF ../`

Once CMake is configured, building the library is as simple as typing 'make'. This will build all library components as well as 'mlboard_tests'.

```
    $ make
```

Note : If you only want to build test you can hit

```
    $ make mlboard_tests
```

If you wish to install mlboard to `/usr/local/include/mlboard/`, `/usr/local/lib/`,
and `/usr/local/bin/`, make sure you have root privileges (or write permissions 
to those three directories), and simply type

```
    $ make install
```

and the mlboard headers are found in `/usr/local/include/mlpack/`.

To run test, you can issue the following command:

```
    $ ./mlboard_tests
```

### 2. Usage

To generate summary data you can make use of `FileWriter` class and `SummaryWriter` class.

For creating a summary, you have to call the specific summary type function from `SummaryWriter` class, for example to log a scaler summary you can call the scaler function as:
`mlboard::SummaryWriter<mlboard::FileWriter>::Scalar(tag,step,value,filewriterobject);`. 

Irrespective of the summary type, you always have to pass the filewriter object that is responsible for first creating events using this summary and then putting that into a queue and then finally writing those events into the file through the logger, which is running asynchronously.

Following is a snippet that would log scaler for 4 steps in temp directory.

You can compile the following snippet using : `g++ main.cpp -lproto -lprotobuf -lpthread` 

```
#include <mlboard/mlboard.hpp>
#include <iostream>
#include <chrono> 
#include <ctime> 
#include <future>

using namespace std;
using namespace mlboard;

// A function to mock that summary creation takes 10 sec.
void mockfunc(const std::string& tag,
              int step,
              double value,
              FileWriter& fw)
{
    std::this_thread::sleep_for( std::chrono::seconds(10));
    mlboard::SummaryWriter<mlboard::FileWriter>::Scalar(tag, step, value, fw);
}

int main()
{
    // Creating a FileWriter object that is responsible for logging the summary.
    std::chrono::time_point<std::chrono::system_clock> start, end; 
    start = std::chrono::system_clock::now(); 
    FileWriter f1("temp");
    // Creating a scaler summary.
    mockfunc("tag", 1, 1.1, f1);
    mockfunc("tag", 2, 2.1, f1);
    mockfunc("tag", 3, 3.1, f1);
    mockfunc("tag", 4, 4.1, f1);

    // This will allow you to indicate that you have logged all your data.
    f1.Close();
    end = std::chrono::system_clock::now(); 
    std::chrono::duration<double> elapsed_seconds = end - start; 
    std::time_t end_time = std::chrono::system_clock::to_time_t(end); 
  
    std::cout << "finished computation at " << std::ctime(&end_time) 
              << "elapsed time: " << elapsed_seconds.count() << "s\n"; 

    google::protobuf::ShutdownProtobufLibrary();
}
```

You can then use tensorboard to visualize the scaler by using a simple command: `tensorboard --logdir .`

The above snippet of code would print the time taken for the scaler to log.

```
elapsed time: 44.6872s
```

Above timing is 40s (summary creation time) + 4s (flush timing i.e time taken to write event files).

Alternatively you can execute the logging in an async manner which would be faster at many instances by using `std::async`, here is a snippet which allows you to do the same operation but much faster as compared to the previous code.

```
#include <mlboard/mlboard.hpp>
#include <iostream>
#include <chrono> 
#include <ctime> 
#include <future>

using namespace std;
using namespace mlboard;

// A function to mock that summary creation takes 10 sec.
void mockfunc(const std::string& tag,
              int step,
              double value,
              FileWriter& fw)
{
    std::this_thread::sleep_for( std::chrono::seconds(10));
    mlboard::SummaryWriter<mlboard::FileWriter>::Scalar(tag, step, value, fw);
}

int main()
{
    std::chrono::time_point<std::chrono::system_clock> start, end; 
    start = std::chrono::system_clock::now(); 
    FileWriter f1("temp");

    std::future<void> result1 = async(std::launch::async, mockfunc, "tag", 1, 1.1, std::ref(f1));
    std::future<void> result2 = async(std::launch::async, mockfunc, "tag", 2, 1.2, std::ref(f1));
    std::future<void> result3 = async(std::launch::async, mockfunc, "tag", 3, 1.3, std::ref(f1));
    std::future<void> result4 = async(std::launch::async, mockfunc, "tag", 4, 1.4, std::ref(f1));
    result1.get();
    result2.get();
    result3.get();
    result4.get();

    f1.Close();  

    end = std::chrono::system_clock::now(); 
    std::chrono::duration<double> elapsed_seconds = end - start; 
    std::time_t end_time = std::chrono::system_clock::to_time_t(end); 
  
    std::cout << "finished computation at " << std::ctime(&end_time) 
              << "elapsed time: " << elapsed_seconds.count() << "s\n"; 
}

```

The total time of execution would be reduced to 14 sec since they all are executing parallelly.

```
elapsed time: 14.0004s
```

Above timing is 10s (summary creation time) + 4s (flush timing i.e time taken to write event files).

As you can see from both the snippet, the flush timing remains same since deafult flush timing are `5000 milliseconds` and we are able to get approx same flush time, for both of the above code. The significant reduction was in summary creation time becuase of async function calling.

Note: Just to benchmark, a waiting time of 10 sec was added using `std::this_thread::sleep_for( std::chrono::seconds(10));` inside the `mockfunc` (to mock a behavior of writing a summary which has a lot of data), so that there could be a clear difference between the two codes.

### 3. Supported Summary types 

Following are the Summary you could log using mlboard:

- [Scaler Summary](examples/scaler.md)
- [Image Summary](examples/image.md)
