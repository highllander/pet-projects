#define BOOST_TEST_MAIN

#include "Helpers.h"
#include "MatchFinder.h"

#include <map>
#include <iostream>
#include <fstream>

#include <boost/test/unit_test.hpp>
#include <boost/filesystem.hpp>


/// 1 SUITE
BOOST_AUTO_TEST_SUITE(Helpers)

BOOST_AUTO_TEST_CASE(RegexToStr)
{
    const std::map<std::string, std::string> tests = {
        {"?add", ".add"},
        {"???", "..."},
        {"", ""},
        {".add", "\\.add"},
        {"1112?aaa", "1112.aaa"},
        {".?*", "\\..\\*"}
    };

    for (const auto& it : tests) {
        BOOST_REQUIRE_EQUAL(HelpersNS::FitStringToBoostRegex(it.first), it.second);
    }
}

BOOST_AUTO_TEST_SUITE_END()

/// 2 SUITE
BOOST_AUTO_TEST_SUITE(MatchFinder)

struct MatchFinderFixture
{
    MatchFinderFixture()
        : m_matchfinder()
    {
    }

    MatchFinderNS::MatchFinder m_matchfinder;
};

BOOST_FIXTURE_TEST_CASE(CheckFile, MatchFinderFixture)
{
    BOOST_CHECK_THROW(m_matchfinder.SetFileName("SOME_NONE_EXIST_FILENAME"), std::runtime_error);
    BOOST_CHECK_NO_THROW(m_matchfinder.SetFileName(boost::filesystem::current_path().string() + "/Release/input1.txt"));
}

BOOST_FIXTURE_TEST_CASE(CheckFinder, MatchFinderFixture)
{
    m_matchfinder.SetFileName(boost::filesystem::current_path().string() + "/Release/input1.txt");
    m_matchfinder.SetMask("?ad");

    m_matchfinder.StartMatchFinding();
    auto matches = m_matchfinder.GetMatches();

    std::ifstream output1;
    output1.open(boost::filesystem::current_path().string() + "/Release/output1.txt", std::ios::in);

    std::string str;
    while (std::getline(output1, str)) {
        BOOST_REQUIRE(matches.find(str) != matches.end());
    }

    output1.close();
}

BOOST_AUTO_TEST_SUITE_END()