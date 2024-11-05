#pragma once

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>
#include <vector>
#include <sstream>

namespace hlsdk_utils
{
    enum class CompareOptions
    {
        None = 0,
        IgnoreCase = 1
    };

    //
    //  REGISTER FUNCTIONS
    //

    inline void to_lower(std::string& s)
    {
        std::for_each(s.begin(), s.end(), [](char& c)
        {
            c = ::tolower(c);
        });
    }

    inline void to_upper(std::string& s)
    {
        std::for_each(s.begin(), s.end(), [](char& c)
        {
            c = ::toupper(c);
        });
    }

    //
    //  TRIM FUNCTIONS
    //

    inline void ltrim(std::string& s, const std::function<bool(unsigned char)>& condition)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), condition));
    }

    inline void ltrim(std::string& s)
    {
        ltrim(s, [](unsigned char ch) { return !std::isspace(ch); });
    }

    inline void ltrim(std::string& s, char trim_char)
    {
        ltrim(s, [trim_char](unsigned char ch) { return !trim_char; });
    }


    inline void rtrim(std::string& s, const std::function<bool(unsigned char)>& condition)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(), condition).base(), s.end());
    }

    inline void rtrim(std::string& s)
    {
        rtrim(s, [](unsigned char ch) { return !std::isspace(ch); });
    }

    inline void rtrim(std::string& s, char trim_char)
    {
        rtrim(s, [trim_char](unsigned char ch) { return !trim_char; });
    }


    inline void trim(std::string& s)
    {
        ltrim(s);
        rtrim(s);
    }

    inline void trim(std::string& s, char trim_char)
    {
        ltrim(s, trim_char);
        rtrim(s, trim_char);
    }

    inline void trim(std::string& s, const std::function<bool(unsigned char)>& condition)
    {
        ltrim(s, condition);
        rtrim(s, condition);
    }

    //
    //  REPLACE FUNCTIONS
    //

    inline void replace_nth(std::string& s, const std::string& search, int nth, const std::string& format)
    {
        size_t start_pos = 0;
        int i = 0;
        while ((start_pos = s.find(search, start_pos)) != std::string::npos)
        {
            if (i == nth)
            {
                s.replace(start_pos, search.length(), format);
                break;
            }

            start_pos += search.size();
            i++;
        }
    }

    // from https://en.cppreference.com/w/cpp/string/basic_string/replace
    inline std::size_t replace_all(std::string& inout, std::string_view what, std::string_view with)
    {
        std::size_t count{};
        for (std::string::size_type pos{};
             std::string::npos != (pos = inout.find(what.data(), pos, what.length()));
             pos += with.length(), ++count)
        {
            inout.replace(pos, what.length(), with.data(), with.length());
        }
        return count;
    }

    [[nodiscard]] inline std::string replace_all_copy(std::string_view in, std::string_view what, std::string_view with)
    {
        std::string result(in);
        replace_all(result, what, with);
        return result;
    }

    //
    // VARIOUS FUNCTIONS
    //

    [[nodiscard]] inline bool contains(const std::string& s, const std::string& sub)
    {
        return s.find(sub) != std::string::npos;
    }

    inline bool equals(std::string_view a, std::string_view b, CompareOptions compareOptions = CompareOptions::None)
    {
        if (compareOptions == CompareOptions::IgnoreCase)
            return std::equal(a.begin(), a.end(),
                              b.begin(), b.end(),
                              [](char a, char b)
                              {
                                  return ::tolower(a) == ::tolower(b);
                              });

        return a == b;
    }

    template <class Iterator>
    std::string join(const Iterator begin, const Iterator end, std::string_view delimiter)
    {
        std::stringstream ss;
        for (Iterator it = begin; it != end; it++)
        {
            ss << *it;

            if (std::next(it) != end)
                ss.write(delimiter.data(), delimiter.size());
        }
        return ss.str();
    }
}