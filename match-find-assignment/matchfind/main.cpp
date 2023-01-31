#include "MatchFinder.h"

#include <string>
#include <iostream>
#include <boost/filesystem.hpp>

#include <boost/program_options.hpp>

int main(int argc, char** argv)
{
    namespace bpo = boost::program_options;
    try
    {
        bpo::options_description options{ "options" };
        options.add_options()
            ("help,h", "Description of mtfind")
            ("path,p", bpo::value<std::string>(), "path of the target file")
            ("mask,m", bpo::value<std::string>(), "mask for match words");

        bpo::variables_map vmap;
        store(parse_command_line(argc, argv, options), vmap);

        if (vmap.count("help"))
        {
            std::cout << options;
            return 0;
        }

        std::string path{};
        std::string mask{};
        
        if (vmap.count("path") == 0 || vmap.count("mask") == 0)
        {
            std::cout << "incorrect arguments!";
            return 0;
        }
        else
        {
            path = vmap["path"].as<std::string>();
            mask = vmap["mask"].as<std::string>();

            std::cout << "Target file is: " << path << '\n';
            std::cout << "Applied mask is: " << mask << '\n';
        }

        MatchFinderNS::MatchFinder matchFinder(path, mask);

        matchFinder.StartMatchFinding();
        matchFinder.PritnAllMatches(std::cout);       
    }
    catch (const std::exception& ex)
    {
        std::cout << "incorrect arguments! ex=" << ex.what();
    }


    return 0;
}
