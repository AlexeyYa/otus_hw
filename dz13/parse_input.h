/*! @file parse_input.h
 *  @brief Заголовочный файл сервера
 */

#ifndef PARSEINPUT_H
#define PARSEINPUT_H

#include "collections.h"
#include "cmd.h"
#include "server.h"

#include <string>
#include <string_view>
#include <algorithm>
#include <memory>
#include <iostream>

void Parse(std::string str, std::shared_ptr<Isession> session)
{
    auto space_iter = std::find(str.cbegin(), str.cend(), ' ');
    std::string_view s_v(str.c_str(), space_iter - str.cbegin());

    if (s_v == "INSERT")
    {
        auto space_iter2 = std::find(space_iter + 1, str.cend(), ' ');
        if (space_iter2 == str.cend())
        {
            CollectionsData_.CommandQueue.push(std::make_shared<InvalidCommand>(session, "invalid insert synatax"));
            return;
        }
        auto s_v_col = std::string_view(str.c_str() + (space_iter - str.cbegin()) + 1
                               , space_iter2 - space_iter - 1);
        auto space_iter3 = std::find(space_iter2 + 1, str.cend(), ' ');
        if (space_iter3 == str.cend())
        {
            CollectionsData_.CommandQueue.push(std::make_shared<InvalidCommand>(session, "invalid INSERT synatax"));
            return;
        }
        auto s_v_id = std::string(str.c_str() + (space_iter2 - str.cbegin()) + 1
                               , space_iter3 - space_iter2 - 1);
        int id;
        try {
            id = std::stoi(s_v_id);
        } catch (...) {
            CollectionsData_.CommandQueue.push(std::make_shared<InvalidCommand>(session, "invalid INSERT synatax"));
            return;
        }

        auto s_v_value = std::string(str.c_str() + (space_iter3 - str.cbegin()) + 1
                               , str.cend() - space_iter3 - 2);

        if ((s_v_col == "A" || s_v_col == "B") &&
                (s_v_value != ""))
            CollectionsData_.CommandQueue.push(std::make_shared<InsertCommand>(session, *s_v_col.begin(), id, s_v_value));
        else
            CollectionsData_.CommandQueue.push(std::make_shared<InvalidCommand>(session, "invalid table name or empty value"));
        return;
    }
    if (s_v == "TRUNCATE")
    {
        auto s_v_col = std::string_view(str.c_str() + (space_iter - str.cbegin()) + 1
                               , str.cend() - space_iter - 1);
        if (s_v_col == "A\n")
            CollectionsData_.CommandQueue.push(std::make_shared<TruncateCommand>(session, 'A'));
        else if (s_v_col == "B\n")
            CollectionsData_.CommandQueue.push(std::make_shared<TruncateCommand>(session, 'B'));
        else
            CollectionsData_.CommandQueue.push(std::make_shared<InvalidCommand>(session, "invalid table name"));
        return;
    }
    if (s_v == "INTERSECTION\n")
    {
        CollectionsData_.CommandQueue.push(std::make_shared<IntersectCommand>(session));
        return;
    }
    if (s_v == "SYMMETRIC_DIFFERENCE\n")
    {
        CollectionsData_.CommandQueue.push(std::make_shared<DifferenceCommand>(session));
        return;
    }

    CollectionsData_.CommandQueue.push(std::make_shared<InvalidCommand>(session, "invalid command"));
}

#endif
