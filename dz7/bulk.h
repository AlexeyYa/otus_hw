/*! @file bulk.h
 *  @brief Пакетная обработка команд
 */

#ifndef BULK_H
#define BULK_H

#include <vector>
#include <string>
#include <chrono>
#include <sstream>
#include <fstream>
#include <iostream>

/*!
 *  @brief Обработчик комманд для пакетной обработки
 */
template <typename T>
struct Bulk
{
    /*!
     * @brief Конструктор
     * @param default_size размер по умолчанию
     */
    Bulk(size_t default_size=3) : default_size(default_size), m_open_brackets(0)
    {}

    ~Bulk()
    {
        if (m_open_brackets == 0 && m_data.size() != 0)
            ExecuteCmds();
    }

    /*!
     * @brief Обработка введенной комманды
     * @param команда
     */
    void Parse(std::string input)
    {
        if (input == "{")
		{
            if (m_data.size() != 0 && m_open_brackets == 0)
            {
                ExecuteCmds();
                time = std::chrono::system_clock::now();
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
            if (m_data.size() == 0)
                time = std::chrono::system_clock::now();
            m_data.emplace_back(input);
            if (m_data.size() == default_size && m_open_brackets == 0)
			{
                ExecuteCmds();
			}
		}
    }

    size_t default_size;
private:
    /*!
     * @brief выполнение всех записанных комманд
     */
	void ExecuteCmds()
	{
		std::cout << "Bulk: ";
		bool first = true;
		for (auto cmd : m_data)
		{
			if (first)
				first = false;
			else
                std::cout << ", ";
			cmd();
		}
        std::cout << std::endl;
        SaveToLog();
        m_data.clear();
	}
	
    /*!
     * @brief сохранение всех записанных комманд
     */
	void SaveToLog()
	{
        std::ofstream fs;
        std::stringstream filename;
        filename << "bulk" << std::chrono::duration_cast<std::chrono::seconds>(time.time_since_epoch()).count()
                 << ".log";
        fs.open(filename.str(), std::ios::out);
        if (fs.is_open())
        {
            bool first = true;
            for (auto cmd : m_data)
            {
                if (first)
                    first = false;
                else
                    fs << ", ";
                fs << cmd.ToString();
            }
            fs << std::endl;
            fs.close();
        }
	}

    /*!
     * @brief вектор записанных комманд
     */
    std::vector<T> m_data;
    std::chrono::system_clock::time_point time;
    size_t m_open_brackets;
};


#endif
