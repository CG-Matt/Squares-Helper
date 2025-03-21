# Squares helper

This is a [squares](https://squares.org/) helper utility written in C.  
It allows the user to provide the program with a squares grid,
after which it generates a file (`output.txt`) which contains
a list of less common words with the help of a python script.
It uses the wordfreq python library for word frequency checking.    
Disclainer: These words aren't always recognised as valid
by squares 

## Dependencies
The following dependencies are required for this program:
```
cmake >= 3.18
python3 >= 3.11.2
libcurl >= 7.88.1
wordfreq >= 3.1.1
```

## Build Instructions
To build this program do the following:

First install the python wordfreq library using the following command
```console
$ pip install wordfreq
```

### Linux
```console
$ git clone https://www.github.com/CG-Matt/squares-helper.git
$ cd squares-helper
$ mkdir build
$ cd build
$ cmake ..
```

### Windows
- Clone the repo. If you have git installed simply do
```console
$ git clone https://www.github.com/CG-Matt/squares-helper.git
```
- Open the folder with Visual Studio and let VS configure the build environment
- Build the project

### MacOS
Support is not currently available.

## Using the program
To use the program, first fill in the `GRID` file with the letters from the squares grid you would like help with. They can be lowercase, uppercase, on one line, on two lines or four lines. The program does not care about line count, it only wants 16 characters.  
You can also configure the max word length from the `settings.conf` file. This can speed up execution of the max word length is small for the day.

## Additional configuration
If you would like to use a different python environment that the defualt one on your system you can edit the configuration file `src/config.h` and specify a custom python executable path. Note this requires the program to be re-compiled.