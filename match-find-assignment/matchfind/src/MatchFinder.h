#include <string>
#include <vector>
#include <thread>
#include <iostream>
#include <mutex>
#include <map>
#include <set>
#include <fstream>
#include <atomic>

#include <boost/regex.hpp>

namespace MatchFinderNS 
{

using LineT = std::pair<int /*lineNumber*/, std::string /*line*/>;
using ChunksVec = std::vector<LineT>;
using MatchMap = std::map<std::string, std::pair<int,int>>;
using MatchMapSorted = std::multimap<int, std::pair<int, std::string>>;

class MatchFinder 
{
    std::vector<std::thread> m_pool;
    bool m_needReload = true;

    std::string m_fileName;
    std::string m_mask;
    size_t  m_threadsCount;

    std::mutex m_fileMutex;

    MatchMap m_matchMap;
    ChunksVec m_chunksVec;

    std::atomic_bool m_endOfData = false;

public:
    MatchFinder(unsigned int size = std::thread::hardware_concurrency());
    MatchFinder(std::string path, std::string mask, unsigned int size = std::thread::hardware_concurrency());

    void StartMatchFinding();
    void PritnAllMatches(std::ostream& out);
    void SetFileName(const std::string& name);
    void SetMask(const std::string& mask);
    MatchMap GetMatches();

private:
    void matchFind(LineT parsLine, const std::string& mask, MatchMap& mapOut);
    size_t calcBytePerThread();
    void readAndParse(std::ifstream& infile, unsigned int readSize, int& lineCounter);
    void parseCycle(std::ifstream& file, unsigned int readSize, int& lineCounter);
    void runFileParse();
    MatchMapSorted getSortedMathces();
};
}
