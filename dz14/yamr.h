/*! @file yamr.h
 *  @brief Описание n-мерной разреженной матрицы
 */

#ifndef YAMR_H
#define YAMR_H

#include <iostream>
#include <fstream>
#include <string>
#include <numeric>
#include <thread>
#include <vector>
#include <mutex>
#include <string_view>
#include <algorithm>
#include <functional>
#include <sstream>

namespace yamr {

/*!
 * @brief Map
 * @param mnum number of map worker threads
 * @param path source file
 * @return vector containing m vectors of strings
 */
std::vector<std::vector<std::string>> Map(size_t mnum, const std::string& path)
{
    std::vector<std::thread> threads;
    //threads.reserve(mnum);
    std::vector<std::vector<std::string>> output;
    output.resize(mnum);

    std::ifstream fs(path, std::ios::binary);
    if (!fs.is_open())
    {
        std::cerr << "Can't open file!" << std::endl;
        throw;
    }

    size_t start = 0;
    size_t end = 0;
    fs.seekg(0, std::ios::end);
    size_t total_chars = fs.tellg();
    fs.seekg(0, std::ios::beg);
    // Get newline characters size
    char c;
    long long line_dif;
    do
        fs.get(c);
    while(c != '\n' && c != '\r');
    fs.get(c);
    if (c != '\n' && c != '\r')
        line_dif = 1;
    else
        line_dif = 2;

    fs.close();

    fs = std::ifstream(path);
    if (!fs.is_open())
    {
        std::cerr << "Can't open file!" << std::endl;
        throw;
    }

    auto MapWorker = [&path, line_dif](std::vector<std::string>& mapped, long long start, long long end)
    {
        std::ifstream fs(path);
        if (!fs.is_open())
        {
            std::cerr << "Can't open file" << std::endl;
            throw;
        }

        // Read and put in mapped
        fs.seekg(start);
        end -= start;
        while (end > 0)
        {
            std::string str;
            std::getline(fs,str);
            if (str != "")                  // Windows(\r\n new line) files workaround
                mapped.emplace_back(str);
            end -= str.length() + line_dif;
        }
        fs.close();
    };

    for (size_t m = 0; m < mnum; m++)
    {
        if (start >= total_chars - 1)
        {
            mnum = m;
            std::cout << "Mnum set to " << mnum << std::endl;
            break;
        }

        if (m == mnum - 1) // Last part
        {
            threads.emplace_back(MapWorker, std::ref(output[m]), start, total_chars - 1);
        }
        else
        {
            // Search closest new line character
            int sign = -1;
            size_t cnt = 0;
            end = total_chars / mnum * (m + 1);
            do
            {
                end += sign * cnt;
                if (end > start && end < total_chars) // skip if end underflows
                {
                    fs.seekg(end);
                    if (fs.peek() == '\n')
                        break;
                }

                cnt++;
                sign = -sign;
            }
            while (end != total_chars);

            threads.emplace_back(MapWorker, std::ref(output[m]), start, end);
            start = end + 1; // Next character after new line char
        }
    }

    fs.close();
    for (size_t i = 0; i < mnum; i++)
    {
        threads[i].join();
    }

    output.resize(mnum);
    return output;
}

/*!
 * @brief Shuffle
 * @param input vector of vectors of strings
 * @param rnum number of output vectors
 * @return vector containing r continiously sorted vectors of strings
 */
std::vector<std::vector<std::string>> Shuffle(std::vector<std::vector<std::string>> input, size_t rnum)
// Sorts input vectors
{
    std::vector<std::thread> threads;
    threads.reserve(input.size());
    std::vector<std::vector<std::string>> output;

    size_t total_size = std::accumulate(input.begin(), input.end(), 0ull,
                                        [](size_t accum, const std::vector<std::string>& vec)
                                        { return accum + vec.size(); });

    for (size_t i = 0; i < input.size(); ++i)
    {
        threads.emplace_back(std::sort<std::vector<std::string>::iterator>, input[i].begin(), input[i].end());
    }

    for (auto& thr : threads)
        thr.join();

    int r = 0;
    size_t cnt = 0;
    if (rnum > total_size)
    {
        rnum = total_size;
        std::cout << "Rnum set to " << rnum << " due number of elements" << std::endl;
    }
    output.resize(rnum);

    size_t target = total_size * (r + 1) / rnum;
    std::string first_prev = "";    // Elem before first
    std::string& prev = first_prev; // Take first elem
    std::vector<std::vector<std::string>::iterator> min_elems;
    for (auto& vec : input)
    {
        min_elems.emplace_back(vec.begin());
    }
    std::vector<std::string> end_of_vector{"\255"}; // Vector for holding end of vector in min_elems, max char value

    while (cnt < total_size)
    {
        // Get min elem
        auto min_it = std::min_element(min_elems.begin(), min_elems.end(),
                                       [](auto lhs, auto rhs){ return *lhs < *rhs; });
        size_t vec_idx = min_it - min_elems.begin();
        std::string& elem = **min_it;

        // Check if end of vector and increase iterator
        if (++(*min_it) == input[vec_idx].end())
            *min_it = end_of_vector.begin();

        // Check if should fill next container and emplace element
        if (cnt >= target)
        {
            if (elem != prev)
            {
                r++;
                target = total_size * (r + 1) / rnum;
                output[r].emplace_back(elem);
            }
            else // If element is same as previous
            {
                output[r].emplace_back(elem);
            }
        }
        else
        {
            output[r].emplace_back(elem);
        }
        prev = elem;
        cnt++;
    }

    return output;
}

/*!
 * @brief ReduceWorker
 * @param input vector of data
 * @param prev last element before current data
 * @param next element after data
 * @param id thread id, used as filename for saving output
 */
void ReduceWorker(const std::vector<std::string>& input,
            const std::string prev, const std::string next, size_t id, size_t& max_idx)
{
    std::stringstream ss;
    ss << id << ".log";
    std::ofstream os(ss.str());
    size_t idx = 0;
    size_t prev_idx = 0; // Result of comparsion with previous element

    // First elem is special case
    for (; prev_idx < input.front().size(); prev_idx++)
    {
        if (prev_idx < prev.size())
        {
            if (input.front().at(prev_idx) != prev.at(prev_idx)) // Index of different chars
            {
                break;
            }
        }
        else  // Previous string is same, but shorter
        {
            break;
        }
    }

    // Loop over all elements execept last comparing with next element
    for (auto it = input.begin(); it < input.end() - 1; it++)
    {
        // Compare with next
        for (idx = 0; idx < (*it).size(); idx++)
        {
            if (idx < (*it).size())
            {
                if ((*it).at(idx) != (*(it + 1)).at(idx))
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }

        // Compare next and previous results and print with highest result without exceeding size
        //os << std::string_view((*it).data(), std::min(std::max(idx, prev_idx) + 1, (*it).size())) << std::endl;

        max_idx = max_idx > std::min(std::max(idx, prev_idx) + 1, (*it).size()) ? max_idx :
                            std::min(std::max(idx, prev_idx) + 1, (*it).size());
        prev_idx = idx;
    }

    // Last elem is special case too
    if (next != "")
    {
        for (idx = 0; idx < input.back().size(); idx++)
        {
            if (idx < next.size())
            {
                if (input.back().at(idx) != next.at(idx))
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        idx = 0;
    }

    // Compare and print last element
    //os << std::string_view(input.back().data(), std::min(std::max(idx, prev_idx) + 1, input.back().size())) << std::endl;

    max_idx = max_idx > std::min(std::max(idx, prev_idx) + 1, input.back().size()) ? max_idx :
                        std::min(std::max(idx, prev_idx) + 1, input.back().size());
}

/*!
 * @brief Reduce
 * @param input shuffled vectors
 */
size_t Reduce(const std::vector<std::vector<std::string>> input)
{
    std::vector<std::thread> threads;
    threads.reserve(input.size());
    std::vector<size_t> max_thrs;
    max_thrs.resize(input.size(), 0);

    std::string prev = "";
    std::string next;
    for (size_t r = 0; r < input.size(); r++)
    {

        if (r < input.size() - 1)
        {
            if (input[r + 1].size() != 0)
                next = input[r + 1].front();
            else
            {
                threads.emplace_back(ReduceWorker, std::ref(input[r]), prev, "", r, std::ref(max_thrs[r]));
                break;
            }
            threads.emplace_back(ReduceWorker, std::ref(input[r]), prev, next, r, std::ref(max_thrs[r]));
            prev = input[r].back();
        }
        else
        {
            threads.emplace_back(ReduceWorker, std::ref(input[r]), prev, "", r, std::ref(max_thrs[r]));
        }
    }
    for (auto& thr : threads)
    {
        thr.join();
    }

    return *std::max_element(max_thrs.cbegin(), max_thrs.cend());
}

/*!
 * @brief MapReduce
 * @param path source file
 * @param mnum number of map threads
 * @param rnum number of reduce threads
 */
size_t MapReduce(const std::string& path, const int mnum, const int rnum)
{
    return yamr::Reduce(yamr::Shuffle(yamr::Map(mnum, path), rnum));
}

//// First simple solution
//    struct Data
//    {
//        // Char array for all data in file
//        std::vector<std::vector<char>> thrs_data;

//        // Vector of
//        std::vector<std::string_view> sorted_src;
//    };

//    void Map(const std::string& path, size_t start, size_t end, int thr_idx,
//             Data& _data, std::vector<std::vector<std::string_view>>& mapped_)
//    {
//        std::ifstream fs(path);
//        if (!fs.is_open())
//        {
//            std::cerr << "Can't open file" << std::endl;
//            throw;
//        }

//        fs.seekg(start);
//        auto& lines = mapped_[thr_idx];
//        _data.thrs_data[thr_idx].resize(end - start);
//        fs.read(_data.thrs_data[thr_idx].data(), end - start);

//        // Move counters to start position
//        end = end - start;
//        start = 0;
//        while (start < end)
//        {
//            auto pos = std::find(_data.thrs_data[thr_idx].begin() + start,
//                                 _data.thrs_data[thr_idx].end(), '\n');

//            lines.emplace_back(_data.thrs_data[thr_idx].data() + start,
//                                pos - _data.thrs_data[thr_idx].begin() - start);

//            start = pos - _data.thrs_data[thr_idx].begin() + 1;
//        }

//        std::sort(lines.begin(), lines.end());
//    }

//    std::vector<std::string_view> Merge(std::vector<std::vector<std::string_view>>& mapped_)
//    {
//        std::vector<std::string_view> result;
//        size_t total_lines = std::accumulate(mapped_.begin(), mapped_.end(), 0ull,
//                                          [](size_t s, const auto& vec){ return s + vec.size(); });
//        result.reserve(total_lines);

//        // Vector of current iterators for mapped_, filling with begin()
//        std::vector<
//                std::vector<std::string_view>::iterator> current_;

//        for (auto& elem : mapped_)
//        {
//            current_.emplace_back(elem.begin());
//        }
//        // Max string const
//        const std::string_view end("\255", 1);
//        std::vector<std::string_view> end_iter_base{end};

//        // Sorting in result vector
//        do
//        {
//            auto min = std::min_element(current_.begin(), current_.end(),
//                                [](auto& lhs, auto& rhs){ return *lhs < *rhs;});
//            // Exit condition
//            if (**min == end)
//            {
//                break;
//            }

//            result.emplace_back(**min);

//            // Increase iterator and check if at end of vector
//            (*min)++;
//            if (*min == mapped_[min - current_.begin()].end())
//            {
//                *min = end_iter_base.begin();
//            }
//        }
//        while(true);

//        return result;
//    }

//    Data SplitAndMap(const std::string& path, int mnum)
//    {
//        size_t start = 0;
//        size_t end = 0;
//        size_t total_chars = std::filesystem::file_size(path);

//        std::ifstream fs(path);
//        if (!fs.is_open())
//        {
//            std::cerr << "Can't open file" << std::endl;
//            throw;
//        }

//        // Create and resize vectors for m threads
//        Data _data;
//        std::vector<std::thread> thr_vector;
//        _data.thrs_data.resize(mnum);
//        std::vector<std::vector<std::string_view>> mapped_;
//        mapped_.resize(mnum);

//        for (int i = 0; i < mnum; i++)
//        {
//            if (i == mnum - 1) // Last part
//            {
//                //Map(path, start, total_chars, i);
//                thr_vector.emplace_back(std::thread(yamr::Map, std::ref(path), start, total_chars - 1, i,
//                                                    std::ref(_data), std::ref(mapped_)));
//            }
//            else
//            {
//                bool sign = false;
//                size_t cnt = 0;
//                end = total_chars / mnum * (i + 1);
//                do
//                {
//                    if (sign)
//                    {
//                        fs.seekg(end + cnt);
//                        sign = !sign;
//                    }
//                    else
//                    {
//                        fs.seekg(end - cnt);
//                        cnt++;
//                        sign = !sign;
//                    }
//                }
//                while (fs.peek() != '\n');
//                end = sign? end - cnt + 2 : end + cnt + 1;

//                //Map(path, start, end, i);
//                thr_vector.emplace_back(std::thread(yamr::Map, std::ref(path), start, end, i,
//                                                    std::ref(_data), std::ref(mapped_)));
//                start = end;
//            }
//        }
//        for (auto& thr : thr_vector)
//        {
//            thr.join();
//        }
//        _data.sorted_src = Merge(mapped_);
//        return _data;
//    }

//    void Reduce(std::vector<std::string_view>::const_iterator begin,
//                std::vector<std::string_view>::const_iterator end,
//                std::string_view prev, std::string_view next, std::vector<std::string_view>& reduced)
//    {
//        reduced.reserve(end - begin);
//        size_t idx = 0;
//        size_t prev_idx = 0; // Result of comparsion with previous element


//        // First elem is special case
//        for (; prev_idx < (*begin).size() - 1; prev_idx++)
//        {
//            if (prev_idx < prev.size())
//            {
//                if ((*begin).at(prev_idx) != prev.at(prev_idx)) // Index of different chars
//                {
//                    break;
//                }
//            }
//            else  // Previous string is same, but shorter
//            {
//                break;
//            }
//        }

//        // Loop over all elements execept last comparing with next element
//        for (auto it = begin; it < end - 1; it++)
//        {
//            // Compare with next
//            for (idx = 0; idx < (*it).size(); idx++)
//            {
//                if (idx < (*it).size())
//                {
//                    if ((*it).at(idx) != (*(it + 1)).at(idx))
//                    {
//                        break;
//                    }
//                }
//                else
//                {
//                    break;
//                }
//            }

//            // Compare next and previous results and emplace with highest result
//            reduced.emplace_back((*it).data(), std::min(std::max(idx, prev_idx) + 1, (*it).size()));

//            prev_idx = idx;
//        }

//        // Last elem is special case too
//        if (next != "")
//        {
//            for (idx = 0; idx < (*(end - 1)).size(); idx++)
//            {
//                if (idx < next.size())
//                {
//                    if ((*(end - 1)).at(idx) != next.at(idx))
//                    {
//                        break;
//                    }
//                }
//                else
//                {
//                    break;
//                }
//            }
//        }
//        else
//        {
//            idx = 0;
//        }

//        // Compare and emplace last element
//        reduced.emplace_back((*(end - 1)).data(),
//                             std::min(std::max(idx, prev_idx) + 1,
//                                      (*(end - 1)).size()));
//    }

//    std::vector<std::string> SplitAndReduce(const Data& _data, int rnum)
//    {
//        std::vector<std::string> result;
//        result.reserve(_data.sorted_src.size());
//        std::vector<std::thread> threads;
//        threads.reserve(rnum);
//        std::vector<std::vector<std::string_view>> reduced_;
//        reduced_.resize(rnum);

//        size_t pos = 0;
//        size_t oldpos = 0;
//        std::string_view prev(""); // Previous thread last elem, default value for first thread
//        std::string_view next; // Next thread first elem

//        for (int i = 0; i < rnum; ++i)
//        {
//            if (i != rnum - 1)
//            {
//                // Get index of first element greater than at pos(split in equal chunks)
//                pos = _data.sorted_src.size() / (rnum) * (i + 1);
//                auto it = std::upper_bound(_data.sorted_src.begin() + oldpos,
//                                 _data.sorted_src.begin() + pos + _data.sorted_src.size() / rnum,
//                                           _data.sorted_src[pos]);
//                pos = it - _data.sorted_src.begin();

//                // Value from next thread
//                next = _data.sorted_src[pos];

//                // Create threads
//                threads.emplace_back(yamr::Reduce, _data.sorted_src.begin() + oldpos,
//                        _data.sorted_src.begin() + pos, prev, next, std::ref(reduced_[i]));
////                Reduce(_data.sorted_src.begin() + oldpos,
////                        _data.sorted_src.begin() + pos, prev, next, std::ref(reduced_[i]));
//                // Store value for element for next thread
//                prev = _data.sorted_src[pos - 1];
//                oldpos = pos;
//            }
//            else // Last thread
//            {
//                threads.emplace_back(yamr::Reduce, _data.sorted_src.begin() + oldpos, _data.sorted_src.end(),
//                        prev, "", std::ref(reduced_[i]));
////                Reduce(_data.sorted_src.begin() + oldpos, _data.sorted_src.end(),
////                        prev, "", std::ref(reduced_[i]));
//            }
//        }

//        for (auto& thr : threads)
//        {
//            thr.join();
//        }

//        // Combine results into one vector
//        for (const auto& redu : reduced_)
//        {
//            for (const auto& entry : redu)
//            {
//                result.emplace_back(entry);
//            }
//        }

//        return result;
//    }
}

#endif
