/*! @file collection.h
 *  @brief Заголовочный файл сервера
 */

#ifndef COLLECTION_H
#define COLLECTION_H

#include "conc_queue.h"
#include <memory>
#include <map>
#include <shared_mutex>
#include <string>
#include <sstream>

class Command;

struct Names
{
    std::string nameA;
    std::string nameB;
};

class CollectionsData
{
public:
    ConcurrentQueue<std::shared_ptr<Command>> CommandQueue;

    std::string Insert(const char collection, const int id, const std::string& value)
    {
        std::scoped_lock lk(m_mutex);

        auto it = m_index.find(id);
        if (collection == 'A')
        {
            if (it == m_index.end())
            {
                m_index[id] = {value, ""};
            }
            else if (it->second.nameA == "")
            {
                it->second.nameA = value;
            }
            else
            {
                std::stringstream ss;
                ss << "ERR duplicate " << id << '\n';
                return ss.str();
            }
        }
        else
        {
            if (it == m_index.end())
            {
                m_index[id] = {"", value};
            }
            else if (it->second.nameB == "")
            {
                it->second.nameB = value;
            }
            else
            {
                std::stringstream ss;
                ss << "ERR duplicate " << id << '\n';
                return ss.str();
            }
        }
        return "OK\n";
    }

    std::string Truncate(const char collection)
    {
        std::scoped_lock lk(m_mutex);
        if (collection == 'A')
        {
            for (auto it = m_index.begin(); it != m_index.end(); )
            {
                if (it->second.nameA != "" && it->second.nameB == "")
                {
                    m_index.erase(it++);
                }
                else if (it->second.nameA != "" && it->second.nameB != "")
                {
                    it->second.nameA = "";
                    ++it;
                }
                else
                {
                    ++it;
                }
            }
        }
        else
        {
            for (auto it = m_index.begin(); it != m_index.end(); )
            {
                if (it->second.nameA == "" && it->second.nameB != "")
                {
                    m_index.erase(it++);
                }
                else if (it->second.nameA != "" && it->second.nameB != "")
                {
                    it->second.nameB = "";
                    ++it;
                }
                else
                {
                    ++it;
                }
            }
        }
        return "OK\n";
    }

    std::string Intersect() const
    {
        std::shared_lock lkA(m_mutex);
        std::stringstream ss;

        for (auto elem : m_index)
        {
            if (elem.second.nameA != "" && elem.second.nameB != "")
            {
                ss << elem.first << ',' << elem.second.nameA << ',' << elem.second.nameB << '\n';
            }
        }
        ss << "OK\n";
        std::string result = ss.str();
        return result;
    }

    std::string Difference() const
    {
        std::shared_lock lkA(m_mutex);
        std::stringstream ss;

        for (auto elem : m_index)
        {
            if ((elem.second.nameA != "") && (elem.second.nameB == ""))
            {
                ss << elem.first << ',' << elem.second.nameA << ',' << '\n';
            }
            else if ((elem.second.nameA == "") && (elem.second.nameB != ""))
            {
                ss << elem.first << ',' << ',' << elem.second.nameB << '\n';
            }
        }
        ss << "OK\n";
        std::string result = ss.str();
        return result;
    }
private:
    mutable std::shared_mutex m_mutex;

    std::map<int, Names> m_index;
} CollectionsData_;

#endif
