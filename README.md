# mlboard
mlpack visualization tool

mlboard is a tool that allows you to log mlpack data in the format that the TensorBoard frontend can render in browsers.

Mlboard is still in the development phase. You can track the development ideas [here](https://www.mlpack.org/gsocblog/Jeffin2020CBP.html)

### 0. Contents

  1. [Building mlboard from source](#1-building-mlboard-from-source)
  2. [Usage](#2-usage)

### 1. Building the source code 

For mlboard to run, you have to install protobuf

- For Mac OS 

`brew install protobuf`

- For debian-based systems

`sudo apt-get install libprotobuf-dev protobuf-compiler`

would be sufficient. 

Make a build directory.  The directory can have any name, but 'build' is
sufficient.

    $ mkdir build
    $ cd build

The next step is to run CMake to configure the project.  Running CMake is the
equivalent to running `./configure` with autotools. 

    $ cmake ../

Once CMake is configured, building the library is as simple as typing 'make'.

    $ make

If you wish to install mlboard to `/usr/local/include/mlboard/`, `/usr/local/lib/`,
and `/usr/local/bin/`, make sure you have root privileges (or write permissions 
to those three directories), and simply type

    $ make install

and the mlboard headers are found in `/usr/local/include/mlpack/`.

### 2. Usage

To generate summary data you can make use of `fileWriter` class and `SummaryWriter` class.

For creating a summary, you have to call the specific summary type function from `SummaryWriter` class, for example to log a scaler summary you can call the scaler function as `mlboard::SummaryWriter<mlboard::fileWriter>::scalar(tag,step,value,filewriterobject);`. 

Irrespective of the summary type, you always have to pass the filewriter object that is responsible for first creating events using this summary and then putting that into a queue and then finally writing those events into the file through the logger, which is running asynchronously.

Following is a snippet that would log scaler for 4 steps in temp directory.

You can compile the following snippet of code using : `g++ main.cpp -lproto -lprotobuf -lpthread` 

```
#include <mlboard/mlboard.hpp>
#include <iostream>
#include <chrono> 
#include <ctime> 
#include <future>

using namespace std;
using namespace mlboard;
int main()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    // creating a filewriter object that is repnsible for logging the summary.
    fileWriter f1("temp");
    std::chrono::time_point<std::chrono::system_clock> start, end; 
    start = std::chrono::system_clock::now(); 
    
    // Creating a scaler summary
    mlboard::SummaryWriter<mlboard::fileWriter>::scalar("tag",1,1.1,f1);
    mlboard::SummaryWriter<mlboard::fileWriter>::scalar("tag",2,1.2,f1);
    mlboard::SummaryWriter<mlboard::fileWriter>::scalar("tag",3,1.4,f1);
    mlboard::SummaryWriter<mlboard::fileWriter>::scalar("tag",5,1.5,f1);
    end = std::chrono::system_clock::now(); 
  
    std::chrono::duration<double> elapsed_seconds = end - start; 
    std::time_t end_time = std::chrono::system_clock::to_time_t(end); 
  
    std::cout << "finished computation at " << std::ctime(&end_time) 
              << "elapsed time: " << elapsed_seconds.count() << "s\n"; 

    // This will allow you to indicate that you have logged all your data.
    f1.close();
    google::protobuf::ShutdownProtobufLibrary();
}
```

You can then use tensorboard to visualize the scaler by using a simple command: `tensorboard --logdir .`

The above snippet of code would print the time taken for the scaler to log

```
elapsed time: 40.0163s
```

Alternatively you can execute the logging in an async manner which would be faster at many instances by using `std::async`, here is a snippet which allows you to do the same operation but much faster as compared to the previous code

```
#include <mlboard/mlboard.hpp>
#include <iostream>
#include <chrono> 
#include <ctime> 
#include <future>

using namespace std;
using namespace mlboard;
int main()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
    fileWriter f1("temp");
    std::chrono::time_point<std::chrono::system_clock> start, end; 
    start = std::chrono::system_clock::now(); 
    
    std::future<void> result1 = async(std::launch::async,  SummaryWriter<fileWriter>::scalar,"tag",1,1.1,std::ref(f1));
    std::future<void> result2 = async(std::launch::async,  SummaryWriter<fileWriter>::scalar, "tag",2,1.2,std::ref(f1));
    std::future<void> result3 = async(std::launch::async,  SummaryWriter<fileWriter>::scalar,"tag",3,1.3,std::ref(f1));
    std::future<void> result4 = async(std::launch::async, SummaryWriter<fileWriter>::scalar, "tag",4,1.4,std::ref(f1));
    result1.get();
    result2.get();
    result3.get();
    result4.get();
    end = std::chrono::system_clock::now(); 
  
    std::chrono::duration<double> elapsed_seconds = end - start; 
    std::time_t end_time = std::chrono::system_clock::to_time_t(end); 
  
    std::cout << "finished computation at " << std::ctime(&end_time) 
              << "elapsed time: " << elapsed_seconds.count() << "s\n"; 
    f1.close();
    google::protobuf::ShutdownProtobufLibrary();
}

```

The total time of execution would be reduced to 10 sec since they all are executing parallelly.

```
elapsed time: 10.0004s
```

Note: Just to benchmark, a waiting time of 10 sec was added using `std::this_thread::sleep_for( std::chrono::seconds(10));` inside the scaler function (to mock a behavior of writing a summary which has a lot of data), so that there could be a clear difference between the two codes 
