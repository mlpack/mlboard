## Logging Scalar Values 

These examples help you to understand `mlboard::SummaryWriter::scalar()` API in depth.

### 0. API 

  1. [Log a scalar value](#1-scalar-value)

### 1. Scalar Value

A scalar value could be logged using the following API:

```cpp
template<typename Filewriter>
void SummaryWriter<Filewriter>::Scalar(const std::string& tag,
                                       int step,
                                       double value,
                                       mlboard::Filewriter& fw)
```

The API accepts `tag`, `step`, `value` (scalar value) and `mlboard::Filewriter` object.

Following is a snippet that would log some scalar values.

```cpp
#include <mlboard/mlboard.hpp>
#include <iostream>
#include <chrono> 
#include <ctime> 
#include <future>

int main()
{
  // Creating a FileWriter object that is responsible for logging the summary.
  std::chrono::time_point<std::chrono::system_clock> start, end; 
  start = std::chrono::system_clock::now(); 
  mlboard::FileWriter f1("temp");
  // Creating a scalar summary.
  mlboard::SummaryWriter<mlboard::FileWriter>::Scalar("tag", 1, 1.1, f1);
  mlboard::SummaryWriter<mlboard::FileWriter>::Scalar("tag", 2, 2.1, f1);

  // This will allow you to indicate that you have logged all your data.
  f1.Close();
  end = std::chrono::system_clock::now(); 
  std::chrono::duration<double> elapsed_seconds = end - start; 
  std::time_t end_time = std::chrono::system_clock::to_time_t(end); 

  std::cout << "finished computation at " << std::ctime(&end_time) 
            << "elapsed time: " << elapsed_seconds.count() << "s\n"; 
}
```
