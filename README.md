# `Mini-Lisp` Interpreter

This is the final project for the course "Practice of Software Design" at Peking University. The project is to implement a simple Lisp interpreter in C++.

## Build
  
```bash
cmake -B build
cmake --build build
```

## Run

REPL mode:
```bash
cd bin
./mini-lisp
```

File mode:
```bash
cd bin
./mini-lisp <path-to-file>
```

## Test
Our TAs provide a test framework for us to test our interpreter. You can find the test framework in `src/rjsj_test.hpp`.

If you want to run the test, please check line 8-9 in `CMakeLists.txt`, and uncomment the line 9 to enable the test mode.
```cmake
add_compile_definitions(__TEST)
```

Then you can build and run the test by:
```bash
cmake -B build
cmake --build build
cd bin
./mini-lisp
```