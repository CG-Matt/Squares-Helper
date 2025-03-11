# Squares helper

This is a [squares](https://squares.org/) helper utility written in C.  
It allows the user to provide the program with a squares grid,
after which it generates a file (`output.txt`) which contains
a list of less common words.    
Disclainer: These words aren't always recognised as valid
by squares 

## Dependencies
The following dependencies are required for this program:
```
cmake >= 3.18
python3 >= 3.11.2
libcurl >= 7.88.1
```

## Build Instructions
To build this program do the following:

### Linux
```console
$ git clone https://www.github.com/CG-Matt/squares-helper.git
$ cd squares-helper
$ mkdir build
$ cd build
$ cmake ..
```

### Windows
Support is not currently available.

### MacOS
Support is not currently available.

## Using the program
For the program to function properly it must be ran from the project directory not the build directory.
For example on linux you would run the program as so:
```console
$ ./build/squares-helper
```

To use the program, first fill in the `GRID` file with the letters from the squares grid you would like help with. They can be lowercase, uppercase, on one line, on two lines or four lines. The program does not care about line count, it only wants 16 characters.  
You can also configure the max word length from the `settings.conf` file. This can speed up execution of the max word length is small for the day.