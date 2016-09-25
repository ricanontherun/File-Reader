# File Abstraction

File Abstraction with fast reads in mind.

```cpp
using File = ricanontherun::File;

// Create a file object, defaults to read-only mode.
File f(path);

// Create a file object, read-only with sequential read advice
File f(path, O_RDONLY, File::ACCESS_ADVICE::SEQUENTIAL);

if ( f.Ok() ) {
    File::READ_STATUS status = f.Read(); // Read BUFSIZ bytes into f's internal buffer.
    File::READ_STATUS status = f.Read(1000) // Read 1000 bytes into f's internal buffer.
    
    std:string data = f.Get(); // Get the current contents of f's internal buffer.
}
```

Das it mayne.