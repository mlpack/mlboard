# mlboard
mlpack visualization tool

mlboard is a tool that allows you to log mlpack data in the format that the TensorBoard frontend can render in browsers.

mlboard is still in the development phase. You can track the development ideas [here](https://www.mlpack.org/gsocblog/Jeffin2020CBP.html)

### 0. Contents

  1. [Building mlboard from source](#1-building-the-source-code)
  2. [Supported Summary Types](#2-supported-summary-types)
  3. [Usage](examples/getting-started.md)
  4. [Faq](examples/faq.md)

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

If you wish to keep the logs generated while running the test, you could compile it with the following command:

```
    $ cmake -DKEEP_TEST_LOGS=ON ../
```

### 2. Supported Summary types

Following are the Summary types you could log using mlboard:

- [Scalar Summary](examples/scalar.md)
- [Image Summary](examples/image.md)
- [Histogram Summary](examples/histogram.md)

### 3. Usage

Please glance at [Getting started](examples/getting-started.md) to know more about how to use mlboard.
