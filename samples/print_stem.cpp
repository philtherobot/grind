
// grindtrick import boost_filesystem
// grindtrick import boost_system

#include <iostream>

#include <boost/filesystem.hpp>

int main(int /*argc*/, char ** argv)
{
	boost::filesystem::path pgm(argv[0]);

	std::cout << pgm.stem() << '\n';

	return 0;
}
