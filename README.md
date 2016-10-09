# Input File Abstraction [![Build Status](https://travis-ci.org/ricanontherun/file.svg?branch=master)](https://travis-ci.org/ricanontherun/file)

An input file abstraction with optimized reads in mind.

```cpp
using File = ricanontherun::File;

// Create a file object, defaults to read-only mode.
File f(path);

// Create a file object, with sequential read advice
File f(path, File::ACCESS_ADVICE::SEQUENTIAL);

if ( f.Ok() ) {
    // Read bytes according to the file's optimal block size.
    File::READ_STATUS status = f.Read();
    
     // Read 1000 bytes into f's internal buffer.
    File::READ_STATUS status = f.Read(1000);
    
    std:string data = f.Get(); // Get the current contents of f's internal buffer.
}
```

## A more practical usage
```cpp
using File = ricanontherun::File;

File f("file.txt", File::ACCESS_ADVICE::SEQUENTIAL);

if ( !f.Ok() ) {
    // handle error
} else {
    File::READ_STATUS status;
    
    while ( (status = f.Read(1024)) == File::READ_STATUS::OK ) {
        std::string data = f.Get();
        
        // Do something with data
    }
    
    if ( status == File::READ_STATUS::EXHAUSTED ) {
        // End of file
    } else if ( status == File::READ_STATUS::ERROR ) {
        // An error occured
    }
}
```
Das it mayne.
