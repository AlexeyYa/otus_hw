/*! @file ip_struct.h
 *  @brief ip_struct header file
 */

#ifndef IPFILTER_H
#define IPFILTER_H

#include <string>
#include <sstream>
#include <iostream>
#include <type_traits>
#include <vector>
#include <deque>
#include <list>
#include <array>
#include <forward_list>
#include <memory>
#include <algorithm>

/*! @struct ip_address_base ip_struct.h "ip_struct.h"
 *  @brief This is a ip_address interface.
 */
struct ip_address_base {
    virtual ~ip_address_base() = default;

    /*! @brief Conversion to string function
     *  @return a string.
     */
    virtual std::string to_string() const noexcept = 0;
};

/*! @struct ip_address_str ip_struct.h "ip_struct.h"
 *  @brief This is a ip_address data struct for text information.
 */
struct ip_address_str : ip_address_base {
    std::string addr;

    /*! @brief Conversion to string function
     *  @return a string.
     */
    virtual std::string to_string() const noexcept override
    {
        return addr;
    }
};

/*! @struct ip_address_num ip_struct.h "ip_struct.h"
 *  @brief This is a ip_address data struct for numeric types.
 */
struct ip_address_num : ip_address_base {
    std::vector<unsigned char> addr;

    /*! @brief Conversion to string function
     *  @return a string.
     */
    virtual std::string to_string() const noexcept override
    {
        std::stringstream ss;
        bool first = true;
        for (auto b : addr){
            if (first){
                first = false;
            } else
            {
                ss << '.';
            }
            ss << static_cast<unsigned int>(b);
        }
        return ss.str();
    }
};

/*! @struct ip_address ip_struct.h "ip_struct.h"
 *  @brief This is a ip_address struct.
 *
 *  Mediator struct storing pointer to implementation of ip_address.
 */
struct ip_address {

    std::shared_ptr<ip_address_base> addr; /**< shared_ptr to underlying struct. */

    /*! Empty default constructor disabled.*/
    ip_address() = delete;

    /*! The constructor from shared ptr.*/
    ip_address(std::shared_ptr<ip_address_base> in)
    {
        addr = in;
    }

    virtual ~ip_address() = default;

    /*! @brief Conversion to string function
     *  @return a string.
     */
    virtual std::string to_string() const noexcept //! convert to string function.
    {
        return (*addr).to_string();
    }
};

/*! @namespace container_tools
 *  @brief structs for SFINAE for numeric standart containers
 */
namespace  container_tools{
    template <typename T>
    struct is_container : std::false_type{};

    template <typename T>
    struct is_container<std::vector<T>> : std::true_type{};

    template <typename T, size_t N>
    struct is_container<std::array<T, N>> : std::true_type{};

    template <typename T>
    struct is_container<std::deque<T>> : std::true_type{};

    template <typename T>
    struct is_container<std::forward_list<T>> : std::true_type{};

    template <typename T>
    struct is_container<std::list<T>> : std::true_type{};

    template <typename T>
    constexpr size_t container_size (T container) { return container.size(); }

    template <typename T,
          typename Fake1 = typename std::enable_if<
              is_container<T>::value,
              void>::type,
          typename Fake2 = typename std::enable_if<
              std::is_arithmetic<typename T::value_type>::value,
              void>::type>
    struct is_arithmetic_container : std::true_type{};

    template <typename T, size_t N>
    constexpr size_t size(T(&)[N]) noexcept { return N; }
}


/*! @fn ip_address make_ip_address(T addr)
 *  @brief A ip_address creation for numeric type argument.
 *  @param addr an arithmetic type.
 *  @return a ip_address struct.
 */
template<typename T,
         typename Fake = typename std::enable_if<
             std::is_arithmetic<T>::value,
             void>::type, size_t T_Size = sizeof(T)>
ip_address make_ip_address(T addr) noexcept
{
    ip_address_num ip_num;
    // Using mask to extract bytes from most significant byte
    for (size_t i = 0; i < T_Size; i++)
    {
        ip_num.addr.emplace_back( (addr >> (8* (T_Size - i - 1))) & 0xff );
    }
    return ip_address { std::make_shared<ip_address_num>(ip_num) };
}


/*! @fn ip_address make_ip_address(const T& addr)
 *  @brief A ip_address creation for stl container types argument.
 *  @param addr a vector, list, forward_list, deque, array.
 *  @return a ip_address struct.
 */
template<typename T,
         typename Fake = typename std::enable_if<
             container_tools::is_arithmetic_container<T>::value,
             void>::type>
ip_address make_ip_address(const T& addr) noexcept
{
    ip_address_num ip_num;
    std::for_each(begin(addr), end(addr),
                  [&ip_num](auto byte) mutable
                  { ip_num.addr.emplace_back(static_cast<unsigned char>(byte)); });

    return ip_address { std::make_shared<ip_address_num>(ip_num) };
}

/*! @fn ip_address_num make_ip_address_helper(const std::tuple<T...>& addr)
 *  @brief A helper function for processing tuple.
 *  @param addr a tuple.
 *  @return a ip_address_num struct.
 */
template<size_t I = 0, typename ...T>
ip_address_num make_ip_address_helper(const std::tuple<T...>& addr) noexcept
{
    ip_address_num ip_num;
    if constexpr(I + 1 != sizeof...(T))
        ip_num = make_ip_address_helper<I + 1>(addr);
    ip_num.addr.emplace(begin(ip_num.addr), std::get<I>(addr) );
    return ip_num;
}

/*! @fn ip_address make_ip_address(const std::tuple<T...>& addr)
 *  @brief A ip_address creation for tuple argument.
 *  @param addr a tuple.
 *  @return a ip_address struct.
 */
template<size_t I = 0, typename ...T>
ip_address make_ip_address(const std::tuple<T...>& addr) noexcept
{
    ip_address_num ip_num = make_ip_address_helper<0>(addr);
    return ip_address { std::make_shared<ip_address_num>(ip_num) };
}

/*! @fn ip_address make_ip_address(const std::string& addr)
 *  @brief A ip_address creation for string argument.
 *  @param addr a string.
 *  @return a ip_address struct.
 */
ip_address make_ip_address(const std::string& addr) noexcept
{
    ip_address_str ip_str;
    ip_str.addr = addr;
    return ip_address { std::make_shared<ip_address_str>(ip_str) };
}

/*! @brief Prints ip stored in input.
 *  @param input a standart container, tuple, numeric type or string storing ip address or text.
 */
template <typename T>
void print_ip(T input)
{
    std::cout << make_ip_address(input).to_string() << std::endl;
}
#endif
