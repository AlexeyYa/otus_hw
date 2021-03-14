/*! @file bulk.h
 *  @brief Пакетная обработка команд
 */

#ifndef BULK_H
#define BULK_H

#include "conc_queue.h"

#include <vector>
#include <queue>
#include <string>
#include <chrono>
#include <sstream>
#include <fstream>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <numeric>
#include <mutex>
#include <functional>

/*!
 *  @brief Обработчик комманд для пакетной обработки
 */
template <typename T>
struct Bulk
{
    using Block_with_time = std::pair<std::vector<T>,
                                      std::chrono::system_clock::time_point>;

    struct Thread_log
    {
        Thread_log()
            : block_count(0), cmd_count(0) {}
        size_t block_count, cmd_count;
    };

    /*!
     * @brief Конструктор
     * @param default_size размер по умолчанию
     */
    Bulk(std::ostream& os, std::mutex& os_mutex, size_t bulk_id, size_t default_size=3, size_t fs_threads_n=2)
        : default_size(default_size), os(os), m_os_mutex(os_mutex), bulk_id(bulk_id), m_console_thr(&Bulk::ConsoleOutput, this),
          m_open_brackets(0), m_line_count(0), m_block_count(0), m_cmd_count(0), m_is_running(0)

    {
        m_fs_thrs.reserve(fs_threads_n);
        m_fs_thrs_data.resize(fs_threads_n);
        for (size_t i = 0; i < fs_threads_n; i++)
            m_fs_thrs.emplace_back(&Bulk::SaveToLog, this, i);
    }

    /*!
     * @brief Конструктор
     * @param default_size размер по умолчанию
     */
    Bulk(std::function<void(const std::string&)> callback, std::mutex& os_mutex, size_t bulk_id, size_t default_size=3, size_t fs_threads_n=2)
        : default_size(default_size), m_callback(callback), os(std::cout), m_os_mutex(os_mutex), bulk_id(bulk_id),
          m_console_thr(&Bulk::ConsoleOutput, this),
          m_open_brackets(0), m_line_count(0), m_block_count(0), m_cmd_count(0), m_is_running(0)

    {
        m_use_callback = true;
        m_fs_thrs.reserve(fs_threads_n);
        m_fs_thrs_data.resize(fs_threads_n);
        for (size_t i = 0; i < fs_threads_n; i++)
            m_fs_thrs.emplace_back(&Bulk::SaveToLog, this, i);
    }

    Bulk()
    {
        os = std::cout;
        default_size = 3;
        m_fs_thrs.reserve(2);
        m_fs_thrs_data.resize(2);
        for (size_t i = 0; i < 2; i++)
        {
            m_fs_thrs.emplace_back(&Bulk::SaveToLog, this, i);
        }
    }
    Bulk(Bulk&& b) = default;

    ~Bulk()
    {
        if (m_open_brackets == 0 && m_data.first.size() != 0)
            ExecuteCmds();

        m_console_queue.exit();
        m_console_thr.join();

        m_fs_queue.exit();
        for (auto& thr : m_fs_thrs)
            thr.join();
        //Exit();
    }

    /*!
     * @brief Обработка введенной комманды
     * @param команда
     */
    void Parse(const std::string& input)
    {
        m_line_count++;
        if (input == "{")
        {
            if (m_data.first.size() != 0 && m_open_brackets == 0)
            {
                ExecuteCmds();
                m_time = std::chrono::system_clock::now();
            }
            m_open_brackets++;
        }
        else if (input == "}")
        {
            if (m_open_brackets == 0)
                return;
            if (--m_open_brackets == 0)
            {
                ExecuteCmds();
            }
        }
        else
        {
            m_cmd_count++;
            if (m_data.first.size() == 0)
                m_time = std::chrono::system_clock::now();
            m_data.first.emplace_back(input);
            if (m_data.first.size() == default_size && m_open_brackets == 0)
            {
                ExecuteCmds();
            }
        }
    }

    size_t default_size;
private:
    /*!
     * @brief вывод счетчиков в конце программы
     */
    void Exit()
    {
        std::unique_lock<std::mutex> lk(m_os_mutex);
        os << "thread\t" << "line count\t" << "cmd count\t" << "block count\n";
        os << "main\t" << m_line_count << "\t\t" << m_cmd_count << "\t\t" << m_block_count << '\n';

        os << "log\t\t\t" << m_console_thr_data.cmd_count << "\t\t" << m_console_thr_data.block_count << '\n';
        for (size_t i = 0; i < m_fs_thrs_data.size(); i++)
            os << "file" << i << "\t\t\t" << m_fs_thrs_data[i].cmd_count << "\t\t" << m_fs_thrs_data[i].block_count << '\n';
    }

    /*!
     * @brief выполнение всех записанных комманд
     */
    void ExecuteCmds()
    {
        m_block_count++;
        m_data.second = std::move(m_time);
        m_console_queue.push(std::move(m_data));
        m_data = Block_with_time{};
    }

    /*!
     * @brief вывод комманд в консоль
     */
    void ConsoleOutput()
    {
        Thread_log& thread_data = m_console_thr_data;
        Block_with_time block;
        while (m_console_queue.wpop(block))
        {
            std::unique_lock<std::mutex> lk(m_os_mutex);
            if (m_use_callback)
            {
                std::stringstream ss;
                ss << "Bulk: ";
                bool first = true;
                for (auto cmd : block.first)
                {
                    if (first)
                        first = false;
                    else
                        ss << ", ";
                    cmd(ss);
                    thread_data.cmd_count++;
                }
                ss << std::endl;
                m_callback(ss.str());
            }
            else
            {

                os << "Bulk: ";
                bool first = true;
                for (auto cmd : block.first)
                {
                    if (first)
                        first = false;
                    else
                        os << ", ";
                    cmd(os);
                    thread_data.cmd_count++;
                }
                os << std::endl;
            }
            thread_data.block_count++;

            m_fs_queue.push(std::move(block));
        }
    }

    /*!
     * @brief сохранение всех записанных комманд
     */
    void SaveToLog(size_t index)
    {
        Thread_log& thread_data = m_fs_thrs_data[index];
        size_t counter = 0;
        std::ofstream fs;
        Block_with_time block;
        while (m_fs_queue.wpop(block))
        {
            std::stringstream filename;
            filename << "bulk" << std::chrono::duration_cast<std::chrono::seconds>(block.second.time_since_epoch()).count()
                    << 't' << index << 'b' << bulk_id << 'c' << counter << ".log";
            fs.open(filename.str(), std::ios::out);
            if (fs.is_open())
            {
                bool first = true;
                for (auto cmd : block.first)
                {
                    if (first)
                        first = false;
                    else
                        fs << ", ";
                    fs << cmd.ToString();
                    thread_data.cmd_count++;
                }
                fs << std::endl;
                fs.close();
                thread_data.block_count++;
            }
            counter++;
        }
    }

    /*!
     * @brief вектор записанных комманд
     */
    Block_with_time m_data;
    std::function<void(const std::string&)> m_callback;
    std::ostream& os;
    std::mutex& m_os_mutex;
    size_t bulk_id;

    // Очереди
    ConcurrentQueue<Block_with_time> m_fs_queue;
    ConcurrentQueue<Block_with_time> m_console_queue;
    // Потоки
    std::thread m_console_thr;
    Thread_log m_console_thr_data;
    std::vector<std::thread> m_fs_thrs;
    std::vector<Thread_log> m_fs_thrs_data;

    size_t m_open_brackets;
    size_t m_line_count, m_block_count, m_cmd_count;
    bool m_is_running, m_use_callback = false;

    std::chrono::system_clock::time_point m_time;
};


#endif
