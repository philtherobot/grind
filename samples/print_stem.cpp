
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
