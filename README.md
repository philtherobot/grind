# grind

A tool to compile and link a C++ program made from
a single source file.

Grind has a trick that let you set the dependencies
needed by your program.  Those dependencies are
available for header inclusion and library linking.

Example:

```c++
// grindtrick import boost_filesystem
// grindtrick import boost_system

#include <iostream>

#include <boost/filesystem.hpp>

int main(int argc, char ** argv)
{
    if( argc < 2 )
    {
        std::cout << "no argument given\n";
        return 1;
    }

    boost::filesystem::path pgm(argv[1]);

    std::cout << pgm.stem() << '\n';

    return 0;
}
```

The program can then be compiled and linked:

```
$ grind print_stem.cpp hello.cpp
$ ./print_stem
"hello"
$
```

The dependencies are configured through a .grindconfig
file located in your home directory:

```yaml
libraries:

  yaml-cpp:
    static_library: /usr/local/lib/libyaml-cpp.a

  boost_filesystem:
    library: boost_filesystem

  boost_system:
    library: boost_system

  pstream:
    header: /home/philippe/pstream-1.0.1

  googletest:
    header: /home/philippe/googletest/googletest/include
    static_library: /home/philippe/googletest/googletest/lib/gtest.a
```
