
// grindtrick import boost_filesystem
// grindtrick import boost_system
// grindtrick import yaml-cpp
// grindtrick import pstream

#include <algorithm>
#include <iostream>
#include <fstream>
#include <regex>
#include <set>

#include <yaml-cpp/yaml.h>

#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/filesystem.hpp>

#include <pstream.h>

using namespace std;
using namespace boost::algorithm;
using namespace boost::filesystem;

typedef set<string> string_set;

int g_verbose = 0;

void help()
{
	cout << "Usage: grind [-v] <source>\n";
	cout << "Compiles and links a C++ program from a single source file.\n";
	cout << "\n";
	cout << "-v  verbose\n";
	cout << "\n";
}

void exit_with_message( string const & msg )
{
	cerr << msg << '\n';
	exit(1);
}

template <typename A>
void talk(int lvl, A a)
{
	if( lvl <= g_verbose )
	{
		cout << a;
	}
}

template <typename A, typename ... Args>
void talk(int lvl, A a, Args ... args)
{
	talk(lvl, a);
	talk(lvl, args...);
}

bool is_older(path const & a, path const & b)
{
	return last_write_time(a) < last_write_time(b);
}

string get_pragma_library(string const & pragma)
{
	istringstream is(pragma);
	string s;
	is >> s; // the comment
	is >> s; // grindtrick
	is >> s; // import
	is >> s; // library - keep this one
	return s;
}

string_set get_imported_libraries(path const & sourcefn)
{
	string_set libraries;

	std::ifstream is(sourcefn.string());
	regex re("// grindtrick\\s+import\\s.*");
	string line;
	while( getline(is, line) )
	{
		erase_all( line, "\r");
		smatch mr;
		if( regex_match(line, mr, re) )
		{
			libraries.insert(get_pragma_library(line));
		}
	}

	return libraries;
}

string_set library_part(string_set const & libraries, 
	YAML::Node const & config, string const & part)
{
	string_set r;

	auto libraries_node = config["libraries"];

	for(auto lib: libraries)
	{
		auto node = libraries_node[lib];
		if( !node )
		{
			exit_with_message("library not found");
		}

		auto s = node[part];
		if( s ) 
		{
			auto v = s.as<string>();
			r.insert( v );
		}
	}

	return r;
}

string_set headers(string_set const & libraries, YAML::Node const & config)
{
	return library_part(libraries, config, "header");
}

string_set static_libraries(string_set const & libraries, YAML::Node const & config)
{
	return library_part(libraries, config, "static_library");
}

string_set system_libraries(string_set const & libraries, YAML::Node const & config)
{
	return library_part(libraries, config, "library");
}

int execute(string const & cmd, vector<string> const & args)
{
	auto t = args;
	t.insert(t.begin(), cmd);

	string verbo = "executing: " + join( t, " " );
	talk(1, verbo, "\n");

	redi::pstream process(cmd, t);
	process.close();

	auto st = process.rdbuf()->status();

	talk(1, "exit status: ", st, "\n");

	return st;
}

void compile(path const & sourcefn, path const & outputfn)
{
	auto libraries = get_imported_libraries(sourcefn);

	auto configpath = path(getenv("HOME")) /= ".grindconfig";

	auto config = YAML::LoadFile(configpath.string());

	auto     hdrs =          headers(libraries, config);
	auto statlibs = static_libraries(libraries, config);
	auto  syslibs = system_libraries(libraries, config);

	syslibs.insert("stdc++");

	string compiler("g++");

	using namespace boost::assign;

	vector<string> args;

	args += "-Wall", "-Wextra", "-Wpedantic";
	args += "-pthread";
	args += "--std=gnu++17";

	for_each(hdrs.begin(), hdrs.end(), [&args] (string const & h) { args += "-I" + h; });

	args += "-o" + outputfn.string();

	args += sourcefn.string();

	args.insert(args.end(), statlibs.begin(), statlibs.end());

	for_each(syslibs.begin(), syslibs.end(), [&args] (string const & s) { args += "-l" + s; });

	auto st = execute(compiler, args);
	if( st ) exit(1);
}

int main(int argc, char ** argv)
{
	if( argc < 2 || argc > 3 )
	{
		help();
		return 1;
	}

	int i = 1;

	string arg(argv[i]);
	if( arg == "-v" )
	{
		g_verbose = 1;
		++i;
	}

	path sourcefn(argv[i]);

	if( !exists(sourcefn) )
	{
		exit_with_message("source not found");
	}

	path outputfn( sourcefn.stem() ); // strip path and extension

	if( !exists(outputfn) || is_older(outputfn, sourcefn) )
	{
		compile(sourcefn, outputfn);
	}
	else
	{
		talk(1, "target up to date\n");
	}

	return 0;
}
