/*! @file cmd.h
 *  @brief Простая текстовая команда
 */

#ifndef COMMAND_H
#define COMMAND_H

#include <string>
#include <iostream>

/*!
 *  @brief Простая текстовая команда
 */
struct Command
{
    /*!
     * @brief Конструктор
     * @param команда
     */
    Command(std::string command) : m_command(command)
	{}
	
    /*!
     * @brief Выполнение команды(вывод в консоль)
     */
    void operator()() const
    {
		std::cout << m_command;
    }

    /*!
     * @brief Перевод в строку
     * @return команда
     */
    std::string ToString()
    {
        return m_command;
    }
private:
    std::string m_command;
};



#endif
