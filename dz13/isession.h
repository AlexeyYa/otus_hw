/*! @file join_server.h
 *  @brief Заголовочный файл сервера
 */

#ifndef ISESSION_H
#define ISESSION_H

#include <memory>
#include <string>

class Isession
  : public std::enable_shared_from_this<Isession>
{
public:
    virtual void do_write(const std::string& content) = 0;
};

#endif
