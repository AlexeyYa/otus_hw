/*! @file cmd.h
 *  @brief Заголовочный файл сервера
 */

#ifndef CMD_H
#define CMD_H

#include "server.h"
#include "collections.h"
#include <string>
#include <memory>

class session;

class Command
{
public:
    Command(std::shared_ptr<Isession> session) : m_session(session) {}
    virtual ~Command() = default;
    virtual void Execute() const = 0;

protected:
    std::shared_ptr<Isession> m_session;
};

class InsertCommand : public Command
{
public:
    InsertCommand(std::shared_ptr<Isession> session, char c, int id, const std::string& value)
        : Command(session), m_c(c), m_id(id), m_value(value)
    {}

    virtual void Execute() const override
    {
        m_session->do_write(CollectionsData_.Insert(m_c, m_id, m_value));
    }
private:
    char m_c;
    int m_id;
    std::string m_value;
};

class IntersectCommand : public Command
{
public:
    IntersectCommand(std::shared_ptr<Isession> session) : Command(session)
    {}

    virtual void Execute() const override
    {
        m_session->do_write(CollectionsData_.Intersect());
    }
};

class DifferenceCommand : public Command
{
public:
    DifferenceCommand(std::shared_ptr<Isession> session) : Command(session)
    {}

    virtual void Execute() const override
    {
        m_session->do_write(CollectionsData_.Difference());
    }
};

class TruncateCommand : public Command
{
public:
    TruncateCommand(std::shared_ptr<Isession> session, char c) : Command(session), m_c(c)
    {}

    virtual void Execute() const override
    {
        m_session->do_write(CollectionsData_.Truncate(m_c));
    }
private:
    char m_c;
};

class InvalidCommand : public Command
{
public:
    InvalidCommand(std::shared_ptr<Isession> session, const std::string& message) : Command(session), m_message(message)
    {}

    virtual void Execute() const override
    {
        m_session->do_write("ERR " + m_message + "\n");
    }
private:
    std::string m_message;
};
#endif
