# File Abstraction [![Build Status](https://travis-ci.org/ricanontherun/file.svg?branch=master)](https://travis-ci.org/ricanontherun/file)

A very thin wrapper over Linux IO system calls, capable of fast reads.

```cpp
// Create a file object, defaults to normal advice.
File f;
f.Open(path)

// Create a file object, with sequential read advice
File f;

File::STATUS status = f.SetReadAdvice(File::ACCESS_ADVICE::SEQUENTIAL).Open(path);

if (status == File::Status::OK ) { // You can also do f.Ok()
    // Read bytes according to the file's optimal block size.
    File::READ_STATUS status = f.Read();
    
     // Read 1000 bytes into f's internal buffer.
    File::READ_STATUS status = f.Read(1000);
    
    std:string data = f.Get(); // Get the current contents of f's internal buffer.
}
```

## A more practical usage
```cpp
File f;

File::STATUS status = f.Open(File::ACCESS_ADVICE::SEQUENTIAL);

if ( !f.Ok() ) {
    // handle error
} else {
    File::READ_STATUS status;
    
    while ( (status = f.Read(1024)) == File::READ_STATUS::OK ) {
        std::string data = f.Get();
        
        // Do something with data
    }
    
    if ( status == File::READ_STATUS::END_OF_FILE ) {
        // End of file
    } else if ( status == File::READ_STATUS::ERROR ) {
        // An error occured
    }
}
```
