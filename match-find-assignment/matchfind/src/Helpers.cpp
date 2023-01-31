#include "Helpers.h"

#include <set>

namespace HelpersNS {


std::string FitStringToBoostRegex(const std::string& input)
{
    static const  std::set<char> EXCLUDE_SYMBOLS 
    = {'.', '^', '$', '|', '(', ')', '[', ']', '{', '}', '*', '+', '\\'} ;

    std::string output{};

    for(char s : input)
    {
        auto it = EXCLUDE_SYMBOLS.find(s);
        if(it != EXCLUDE_SYMBOLS.end())
        {
            output += "\\";
            output +=s;
        }
        else if(s == '?')
            output +='.';
        else
            output += s;
    }
    return output;
}



}