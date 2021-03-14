#include "ip.h"

#define BOOST_TEST_MODULE ip
#include <boost/test/included/unit_test.hpp>

#define RANDOM_TEST_COUNT 1000
#define RANDOM_FILTER_TEST_COUNT 100
#define RANDOM_FILTER_VECTOR_SIZE 1000

ip_address ip_from_digits(unsigned char b1, unsigned char b2, unsigned char b3, unsigned char b4);

#ifdef USE_BOOST

ip_address ip_from_digits(unsigned char b1, unsigned char b2, unsigned char b3, unsigned char b4){
    typedef boost::asio::detail::array<unsigned char, 4> bytes_type;

    ip_address ip_result(bytes_type{b1, b2, b3, b4});
    return ip_result;
}
#else
#ifdef WIN32

bool operator==(ip_address lhs, ip_address rhs){
    return lhs.S_un.S_addr == rhs.S_un.S_addr;
}

ip_address ip_from_digits(unsigned char b1, unsigned char b2, unsigned char b3, unsigned char b4){
    ip_address ip_result;
    ip_result.S_un = {b1, b2, b3, b4};
    return ip_result;
}
#elif __linux__

bool operator==(ip_address lhs, ip_address rhs){
    return lhs.s_addr == rhs.s_addr;
}

ip_address ip_from_digits(unsigned char b1, unsigned char b2, unsigned char b3, unsigned char b4){
    ip_address ip_result;
    uint32_t converted_to_int = (b1 << 24) + (b2 << 16) + (b3 << 8) + b4;
    ip_result.s_addr = ntohl(converted_to_int);
    return ip_result;
}

#endif
#endif

BOOST_AUTO_TEST_CASE(SplitLineEdgeCase){
    ip_address ip0 = ip_from_digits(0, 0, 0, 0);
    BOOST_TEST(split("0.0.0.0") == ip0);

    ip_address ip255 = ip_from_digits(255, 255, 255, 255);
    BOOST_TEST(split("255.255.255.255") == ip255);
}

BOOST_AUTO_TEST_CASE(SplitLineRandom){
    for (int i = 0; i < RANDOM_TEST_COUNT; i++){
        u_char b1 = static_cast<u_char>(rand());
        u_char b2 = static_cast<u_char>(rand());
        u_char b3 = static_cast<u_char>(rand());
        u_char b4 = static_cast<u_char>(rand());
        ip_address ip_random = ip_from_digits(b1, b2, b3, b4);
        std::string textValue = std::to_string(b1) + '.'
                + std::to_string(b2) + '.'
                + std::to_string(b3) + '.'
                + std::to_string(b4);
        BOOST_TEST(split(textValue) == ip_random);
    }
}

BOOST_AUTO_TEST_CASE(Filter1_Simple){
    ip_address ip = ip_from_digits(0, 0, 0, 0);
    std::vector<ip_address> ip_vector{ip};
    std::vector<ip_address> ip_nonempty = filter(0, ip_vector);
    std::vector<ip_address> ip_empty = filter(1, ip_vector);
    BOOST_TEST(ip_nonempty == ip_vector);
    BOOST_TEST(ip_empty == std::vector<ip_address>{});
}

BOOST_AUTO_TEST_CASE(Filter1_Random){
    for (int i = 0; i < RANDOM_FILTER_TEST_COUNT; i++){
        std::vector<ip_address> ip_random_vector;
        std::vector<ip_address> ip_filtered_check;
        u_char filter_b1 = static_cast<u_char>(rand());
        for (int j = 0; j < RANDOM_FILTER_VECTOR_SIZE; j++){
            u_char b1 = static_cast<u_char>(rand());
            u_char b2 = static_cast<u_char>(rand());
            u_char b3 = static_cast<u_char>(rand());
            u_char b4 = static_cast<u_char>(rand());
            ip_address ip_random = ip_from_digits(b1, b2, b3, b4);
            ip_random_vector.emplace_back(ip_random);

            if (b1 == filter_b1) ip_filtered_check.emplace_back(ip_random);
        }
        std::vector<ip_address> ip_filtered = filter(filter_b1, ip_random_vector);
        BOOST_TEST(ip_filtered == ip_filtered_check);
    }
}

BOOST_AUTO_TEST_CASE(Filter12_Simple){
    ip_address ip = ip_from_digits(0, 0, 0, 0);
    std::vector<ip_address> ip_vector{ip};
    std::vector<ip_address> ip_nonempty = filter(0, 0, ip_vector);
    std::vector<ip_address> ip_empty1 = filter(0, 1, ip_vector);
    std::vector<ip_address> ip_empty2 = filter(1, 0, ip_vector);
    std::vector<ip_address> ip_empty3 = filter(1, 1, ip_vector);
    BOOST_TEST(ip_nonempty == ip_vector);
    BOOST_TEST(ip_empty1 == std::vector<ip_address>{});
    BOOST_TEST(ip_empty2 == std::vector<ip_address>{});
    BOOST_TEST(ip_empty3 == std::vector<ip_address>{});
}

BOOST_AUTO_TEST_CASE(Filter12_Random){
    for (int i = 0; i < RANDOM_FILTER_TEST_COUNT; i++){
        std::vector<ip_address> ip_random_vector;
        std::vector<ip_address> ip_filtered_check;
        u_char filter_b1 = static_cast<u_char>(rand());
        u_char filter_b2 = static_cast<u_char>(rand());
        for (int j = 0; j < RANDOM_FILTER_VECTOR_SIZE; j++){
            u_char b1 = static_cast<u_char>(rand());
            u_char b2 = static_cast<u_char>(rand());
            u_char b3 = static_cast<u_char>(rand());
            u_char b4 = static_cast<u_char>(rand());
            ip_address ip_random = ip_from_digits(b1, b2, b3, b4);
            ip_random_vector.emplace_back(ip_random);

            if (b1 == filter_b1 && b2 == filter_b2) ip_filtered_check.emplace_back(ip_random);
        }
        std::vector<ip_address> ip_filtered = filter(filter_b1, filter_b2, ip_random_vector);
        BOOST_TEST(ip_filtered == ip_filtered_check);
    }
}


BOOST_AUTO_TEST_CASE(FilterAny_Simple){
    ip_address ip1 = ip_from_digits(0, 1, 1, 1);
    std::vector<ip_address> ip_vector1{ip1};
    ip_address ip2 = ip_from_digits(1, 0, 1, 1);
    std::vector<ip_address> ip_vector2{ip2};
    ip_address ip3 = ip_from_digits(1, 1, 0, 1);
    std::vector<ip_address> ip_vector3{ip3};
    ip_address ip4 = ip_from_digits(1, 1, 1, 0);
    std::vector<ip_address> ip_vector4{ip4};
    ip_address ip_fail = ip_from_digits(1, 1, 1, 1);
    std::vector<ip_address> ip_vector_fail{ip_fail};
    std::vector<ip_address> ip_nonempty1 = filter_any(0, ip_vector1);
    std::vector<ip_address> ip_nonempty2 = filter_any(0, ip_vector2);
    std::vector<ip_address> ip_nonempty3 = filter_any(0, ip_vector3);
    std::vector<ip_address> ip_nonempty4 = filter_any(0, ip_vector4);
    std::vector<ip_address> ip_empty = filter_any(0, ip_vector_fail);
    BOOST_TEST(ip_nonempty1 == ip_vector1);
    BOOST_TEST(ip_nonempty2 == ip_vector2);
    BOOST_TEST(ip_nonempty3 == ip_vector3);
    BOOST_TEST(ip_nonempty4 == ip_vector4);
    BOOST_TEST(ip_empty == std::vector<ip_address>{});
}

BOOST_AUTO_TEST_CASE(FilterAny_Random){
    for (int i = 0; i < RANDOM_FILTER_TEST_COUNT; i++){
        std::vector<ip_address> ip_random_vector;
        std::vector<ip_address> ip_filtered_check;
        u_char filter_b_any = static_cast<u_char>(rand());
        for (int j = 0; j < RANDOM_FILTER_VECTOR_SIZE; j++){
            u_char b1 = static_cast<u_char>(rand());
            u_char b2 = static_cast<u_char>(rand());
            u_char b3 = static_cast<u_char>(rand());
            u_char b4 = static_cast<u_char>(rand());
            ip_address ip_random = ip_from_digits(b1, b2, b3, b4);
            ip_random_vector.emplace_back(ip_random);

            if (b1 == filter_b_any || b2 == filter_b_any || b3 == filter_b_any || b4 == filter_b_any)
                ip_filtered_check.emplace_back(ip_random);
        }
        std::vector<ip_address> ip_filtered = filter_any(filter_b_any, ip_random_vector);
        BOOST_TEST(ip_filtered == ip_filtered_check);
    }
}
