// Copyright 2013-2025 Daniel Parker
// Distributed under Boost license 

#if defined(_MSC_VER)
#include "windows.h" // test no inadvertant macro expansions
#endif
#include <jsoncons/json.hpp>
#include <jsoncons/json_encoder.hpp>
#include <jsoncons/decode_json.hpp>
#include <jsoncons/encode_json.hpp>
#include <catch/catch.hpp>
#include <sstream>
#include <vector>
#include <utility>
#include <ctime>
#include <cstdint>

using namespace jsoncons;

#if defined(JSONCONS_HAS_STATEFUL_ALLOCATOR) && JSONCONS_HAS_STATEFUL_ALLOCATOR == 1

#include <scoped_allocator>
#include <common/mock_stateful_allocator.hpp>

namespace {
namespace ns {

    template <typename Alloc>
    struct book_all_m
    {
        using allocator_type = Alloc;

        using char_allocator_type = typename std::allocator_traits<Alloc>:: template rebind_alloc<char>;
        using string_type = std::basic_string<char, std::char_traits<char>, char_allocator_type>;

        string_type author;
        string_type title;
        double price;

        book_all_m(const Alloc& alloc)
            : author(alloc), title(alloc)
        {
        }

        book_all_m(const book_all_m& other, const Alloc& alloc)
            : author(other.author, alloc), title(other.title, alloc), price(other.price)
        {
        }

        book_all_m(book_all_m&& other, const Alloc& alloc)
            : author(std::move(other.author), alloc), title(std::move(other.title), alloc), price(other.price)
        {
        }
    };

    template <typename Alloc>
    struct book_3_m
    {
        using allocator_type = Alloc;

        using char_allocator_type = typename std::allocator_traits<Alloc>:: template rebind_alloc<char>;
        using string_type = std::basic_string<char, std::char_traits<char>, char_allocator_type>;

        string_type author;
        string_type title;
        double price;
        string_type isbn;

        book_3_m(const Alloc& alloc)
            : author(alloc), title(alloc), isbn(alloc)
        {
        }

        book_3_m(const book_3_m& other, const Alloc& alloc)
            : author(other.author, alloc), title(other.title, alloc), price(other.price), isbn(other.isbn, alloc)
        {
        }

        book_3_m(book_3_m&& other, const Alloc& alloc)
            : author(std::move(other.author), alloc), title(std::move(other.title), alloc), price(other.price), isbn(std::move(other.isbn), alloc)
        {
        }
    };

} // namespace ns
} // namespace 
 
JSONCONS_TPL_ALL_MEMBER_TRAITS(1,ns::book_all_m,author,title,price)
JSONCONS_TPL_N_MEMBER_TRAITS(1,ns::book_3_m,3,author,title,price,isbn)

template <typename T>
using cust_allocator = std::scoped_allocator_adaptor<mock_stateful_allocator<T>>;

TEST_CASE("JSONCONS_ALL_MEMBER_TRAITS using allocator tests")
{

    SECTION("book")
    {
        using book_type = ns::book_all_m<std::scoped_allocator_adaptor<mock_stateful_allocator<char>>>;

        std::string str = R"(
{
    "author" : "Haruki Murakami",  
    "title" : "Kafka on the Shore",
    "price" : 25.17
}
        )";

        cust_allocator<char> alloc(1);
        auto aset = make_alloc_set(alloc);
        auto r = try_decode_json<book_type>(aset, str);

        REQUIRE(r);
    }

    SECTION("vector of book")
    {
        using book_type = ns::book_all_m<cust_allocator<char>>;
        using book_collection_type = std::vector<book_type,cust_allocator<book_type>>;

        std::string str = R"(
[
    {
        "title" : "Kafka on the Shore",
        "author" : "Haruki Murakami",
        "price" : 25.17
    },
    {
        "title" : "Pulp",
        "author" : "Charles Bukowski",
        "price" : 12,  
        "isbn" : "1852272007"  
    },
    {
        "title" : "Cutter's Way",
        "author" : "Ivan Passer",
        "price" : 15.0
    }
]
        )";

        cust_allocator<book_type> alloc(1);
        auto aset = make_alloc_set<cust_allocator<book_type>>(alloc);
        auto r = try_decode_json<book_collection_type>(aset, str);

        REQUIRE(r);
    }
}

TEST_CASE("JSONCONS_N_MEMBER_TRAITS using allocator tests")
{

    SECTION("book")
    {
        using book_type = ns::book_3_m<std::scoped_allocator_adaptor<mock_stateful_allocator<char>>>;

        std::string str = R"(
{
    "author" : "Haruki Murakami",  
    "title" : "Kafka on the Shore",
    "price" : 25.17
}
        )";

        cust_allocator<char> alloc(1);
        auto aset = make_alloc_set(alloc);
        auto r = try_decode_json<book_type>(aset, str);

        REQUIRE(r);
    }

    SECTION("vector of book")
    {
        using book_type = ns::book_3_m<cust_allocator<char>>;
        using book_collection_type = std::vector<book_type,cust_allocator<book_type>>;

        std::string str = R"(
[
    {
        "title" : "Kafka on the Shore",
        "author" : "Haruki Murakami",
        "price" : 25.17
    },
    {
        "title" : "Women: A Novel",
        "author" : "Charles Bukowski",
        "price" : 12.0
    },
    {
        "title" : "Cutter's Way",
        "author" : "Ivan Passer",
        "price" : 15.0
    }
]
        )";

        cust_allocator<book_type> alloc(1);
        auto aset = make_alloc_set<cust_allocator<book_type>>(alloc);
        auto r = try_decode_json<book_collection_type>(aset, str);

        REQUIRE(r);
    }
}

#endif
