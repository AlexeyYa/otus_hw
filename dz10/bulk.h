/*! @file bulk.h
 *  @brief Пакетная обработка команд
 */

#ifndef BULK_H
#define BULK_H

#include "conc_queue.h"

#include <vector>
#include <list>
#include <queue>
#include <string>
#include <chrono>
#include <sstream>
#include <fstream>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <numeric>

/*!
 *  @brief Обработчик комманд для пакетной обработки
 */
template <typename T>
struct Bulk
{
    using Block_with_time = std::pair<std::vector<T>,
                                      std::chrono::system_clock::time_point>;

    struct Thread_w_log
    {
        template <class ...Args>
        Thread_w_log(Args&&... args)
            : thread(args...), block_count(0), cmd_count(0) {}
        std::thread thread;
        size_t block_count, cmd_count;
    };

    /*!
     * @brief Конструктор
     * @param default_size размер по умолчанию
     */
    Bulk(std::ostream& os, size_t default_size=3, size_t fs_threads_n=2)
        : default_size(default_size), os(os), m_console_thr(&Bulk::ConsoleOutput, this),
          m_open_brackets(0), m_line_count(0), m_block_count(0), m_cmd_count(0), m_is_running(0)

    {
//        std::lock_guard lk(m_fs_delay);
//        m_fs_thrs.reserve(fs_threads_n);
        for (size_t i = 0; i < fs_threads_n; i++)
            m_fs_thrs.emplace_back(&Bulk::SaveToLog, this, i);
	}

    ~Bulk()
    {
        if (m_open_brackets == 0 && m_data.first.size() != 0)
            ExecuteCmds();

        m_console_queue.exit();
        m_console_thr.thread.join();

        m_fs_queue.exit();
        for (auto& thr : m_fs_thrs)
            thr.thread.join();
        Exit();
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
        os << "thread\t" << "line count\t" << "cmd count\t" << "block count\n";
        os << "main\t" << m_line_count << "\t\t" << m_cmd_count << "\t\t" << m_block_count << '\n';

        os << "log\t\t\t" << m_console_thr.cmd_count << "\t\t" << m_console_thr.block_count << '\n';
        int i = 0;
        for (auto& thr : m_fs_thrs)
            os << "file" << i++ << "\t\t\t" << thr.cmd_count << "\t\t" << thr.block_count << '\n';
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
        Thread_w_log& thread_data = m_console_thr;
        Block_with_time block;
        while (m_console_queue.wpop(block))
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
            thread_data.block_count++;

            m_fs_queue.push(std::move(block));
        }
	}
	
    /*!
     * @brief сохранение всех записанных комманд
     */
    void SaveToLog(size_t index)
    {
//        {
//            std::lock_guard lk(m_fs_delay);
//        }
        auto it = m_fs_thrs.begin();
        if (index)
            std::advance(it, index);
        Thread_w_log& thread_data = *it;
        size_t counter = 0;
        std::ofstream fs;
        Block_with_time block;
        while (m_fs_queue.wpop(block))
		{
			std::stringstream filename;
            filename << "bulk" << std::chrono::duration_cast<std::chrono::seconds>(block.second.time_since_epoch()).count()
                    << "t" << index << "c" << counter << ".log";
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
    std::ostream& os;

    // Очереди
    ConcurrentQueue<Block_with_time> m_fs_queue;
    ConcurrentQueue<Block_with_time> m_console_queue;
	// Потоки
    Thread_w_log m_console_thr;
    std::list<Thread_w_log> m_fs_thrs;
    std::mutex m_fs_delay;

    size_t m_open_brackets;
    size_t m_line_count, m_block_count, m_cmd_count;
    bool m_is_running;

    std::chrono::system_clock::time_point m_time;
};


#endif
