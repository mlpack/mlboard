### FAQ

### 1. My data is not logged properly:

- There could be chances that there are race condition where the main thread is exiting before the writer thread, hence you could avoid this using `.close()` method of `mlboard::Filewriter` object.

### 2. Error : Detected out of order event.step:

- If you get the following error:

    ` Detected out of order event.step likely caused by a TensorFlow restart. Purging 1 expired tensor events from Tensorboard display between the previous step: 24 (timestamp: 1594962994.0) and current step: 1 (timestamp: 1594962994.0).`
    `

    There are chances that there are two event files in the `logdir`. Make sure that every logdir has only one event file. On every run create a new logdir either by use timestamp or hash function to avoid such situations.

### 3. Error: Unable to get first event timestamp for run:

- If you get the following error:

    `Unable to get first event timestamp for run .: No event timestamp could be found`

    The most likely reason for this error couble be if the filewriter object was reassigned somewhere, for example :

    ```cpp
    f1 = new mlboard::FileWriter("_templogs");

    mlboard::SummaryWriter<mlboard::FileWriter>::Image(
        "Test Image", 1, image, 512, 512, 3, *f1, "Sample Image",
        "This is a Sample image logged using mlboard.");
    // reassigned 
    f1 = new mlboard::FileWriter("_templogs");
    mlboard::SummaryWriter<mlboard::FileWriter>::Text("add Text support ", 1,
        "mlpack is great", *f1);
    ```

    The above code would probably throw error `No event timestamp could be found`, and hence to avoid such error please avoid reassigning the `Filewriter` object.
