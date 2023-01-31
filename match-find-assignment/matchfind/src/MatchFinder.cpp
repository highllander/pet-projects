#include "MatchFinder.h"
#include "Helpers.h"

#include <functional>
#include <iterator>

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <mutex>
#include <stdexcept>

namespace MatchFinderNS  {

/// --------PUBLIC-----------

MatchFinder::MatchFinder(unsigned int size) :
    m_threadsCount(size)
{
 
}

MatchFinder::MatchFinder(std::string path, std::string mask, unsigned int size)
    : m_mask(HelpersNS::FitStringToBoostRegex(mask))
    , m_threadsCount(size)
{
    SetFileName(path);
}

void MatchFinder::StartMatchFinding()
{
    if(!m_needReload)
        return;

    m_needReload = false;

    runFileParse();

    for(auto& thread : m_pool)
    {
        if(thread.joinable())
            thread.join();
    }
    m_pool.clear();
}

void MatchFinder::PritnAllMatches(std::ostream& out)
{
    for (const auto& match : getSortedMathces())
        out << match.first << ' ' << match.second.first << ' ' << match.second.second << '\n';
}

void MatchFinder::SetFileName(const std::string& name)
{
    if(name == m_fileName)
        return;

    if (!boost::filesystem::exists(name))
        throw std::runtime_error("[MatchFinder] SetFileName, file doesn't exist: " + name); 

    m_fileName = name;
    m_needReload = true;
};

void MatchFinder::SetMask(const std::string& mask)
{
    if(m_mask == mask)
        return;

    m_mask = HelpersNS::FitStringToBoostRegex(mask);
    m_needReload = true;
};

MatchMap MatchFinder::GetMatches()
{
    std::unique_lock<std::mutex> lock(m_fileMutex);
    return m_matchMap;
}

/// --------PRIVATE-------------

void MatchFinder::matchFind(MatchFinderNS::LineT parsLine, const std::string& mask, MatchFinderNS::MatchMap& mapOut)
{
    boost::smatch match;
    boost::regex reg(mask);

    while(boost::regex_search(parsLine.second, match, reg))
    {
        mapOut[match.str(0)] = { parsLine.first, match.position() + 1};
        parsLine.second = match.suffix().str();
    }
}

size_t MatchFinder::calcBytePerThread()
{
    return boost::filesystem::file_size(m_fileName) / m_threadsCount;
}

void MatchFinder::readAndParse(std::ifstream& infile, unsigned int readSize, int& lineCounter)
{  
    std::unique_lock<std::mutex> lock(m_fileMutex);

    size_t alreadyRead = 0;
    std::string line;
    MatchFinderNS::ChunksVec chunks;
    
    while(!infile.eof() && alreadyRead < readSize)
    {
        if (!std::getline(infile, line))
            break;
        alreadyRead += line.size();
        chunks.push_back({++lineCounter, std::move(line)});
    }
    m_endOfData = infile.eof();

    lock.unlock();
    MatchFinderNS::MatchMap resMap;

    for (const auto& chunk : chunks)
    {
        matchFind(chunk, m_mask, resMap);
    }

    lock.lock();
    m_matchMap.insert(std::make_move_iterator(std::begin(resMap)), std::make_move_iterator(std::end(resMap)));
}

void MatchFinder::parseCycle(std::ifstream& file, unsigned int readSize, int& lineCounter)
{
    while(!m_endOfData)
    {
        readAndParse(file, readSize, lineCounter);
        std::this_thread::yield();
    }
}

void MatchFinder::runFileParse()
{
    if (m_fileName.empty())
        return;

    std::ifstream file(m_fileName);

    auto readByte = calcBytePerThread();
    int lineCounter = 0;

    while(m_pool.size() < m_threadsCount )
    {
        m_pool.push_back(std::thread(&MatchFinder::parseCycle, this, std::ref(file), readByte, std::ref(lineCounter)));
    }
}

MatchMapSorted MatchFinder::getSortedMathces()
{
    MatchMapSorted mathcesSorted;
    std::unique_lock<std::mutex> lock(m_fileMutex);
    for (const auto& match : m_matchMap)
        mathcesSorted.insert({ match.second.first, {match.second.second, match.first } });
    return mathcesSorted;
}

}