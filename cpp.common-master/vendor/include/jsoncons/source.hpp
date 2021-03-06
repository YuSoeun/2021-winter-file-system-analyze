// Copyright 2018 Daniel Parker
// Distributed under the Boost license, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

// See https://github.com/danielaparker/jsoncons for latest version

#ifndef JSONCONS_SOURCE_HPP
#define JSONCONS_SOURCE_HPP

#include <stdexcept>
#include <string>
#include <vector>
#include <istream>
#include <memory> // std::addressof
#include <cstring> // std::memcpy
#include <algorithm>
#include <exception>
#include <jsoncons/config/jsoncons_config.hpp>
#include <jsoncons/jsoncons_utilities.hpp>

namespace jsoncons { 

// text sources

template <class CharT>
class text_stream_source 
{
public:
    typedef CharT value_type;
    typedef std::char_traits<CharT> traits_type;
private:
    std::basic_istream<CharT>* is_;
    std::basic_streambuf<CharT>* sbuf_;
    size_t position_;

    // Noncopyable 
    text_stream_source(const text_stream_source&) = delete;
    text_stream_source& operator=(const text_stream_source&) = delete;
public:
    text_stream_source(text_stream_source&&) = default;

    text_stream_source(std::basic_istream<CharT>& is)
        : is_(std::addressof(is)), sbuf_(is.rdbuf()), position_(0)
    {
    }

    ~text_stream_source()
    {
    }

    text_stream_source& operator=(text_stream_source&&) = default;

    bool eof() const
    {
        return is_->eof();  
    }

    bool is_error() const
    {
        return is_->bad();  
    }

    size_t position() const
    {
        return position_;
    }

    size_t get(value_type& c)
    {
        try
        {
            int val = sbuf_->sbumpc();
            if (!(val == traits_type::eof()))
            {
                c = (value_type)val;
                ++position_;
                return 1;
            }
            else
            {
                is_->clear(is_->rdstate() | std::ios::eofbit);
                return 0;
            }
        }
        catch (const std::exception&)
        {
            is_->clear(is_->rdstate() | std::ios::badbit | std::ios::eofbit);
            return 0;
        }
    }

    int get()
    {
        try
        {
            int c = sbuf_->sbumpc();
            if (c == traits_type::eof())
            {
                is_->clear(is_->rdstate() | std::ios::eofbit);
            }
            else
            {
                ++position_;
            }
            return c;
        }
        catch (const std::exception&)
        {
            is_->clear(is_->rdstate() | std::ios::badbit | std::ios::eofbit);
            return traits_type::eof();
        }
    }

    void ignore(size_t count)
    {
        try
        {
            for (size_t i = 0; i < count; ++i)
            {
                int c = sbuf_->sbumpc();
                if (c == traits_type::eof())
                {
                    is_->clear(is_->rdstate() | std::ios::eofbit);
                    return;
                }
                else
                {
                    ++position_;
                }
            }
        }
        catch (const std::exception&)
        {
            is_->clear(is_->rdstate() | std::ios::badbit | std::ios::eofbit);
        }
    }

    int peek() 
    {
        try
        {
            int c = sbuf_->sgetc();
            if (c == traits_type::eof())
            {
                is_->clear(is_->rdstate() | std::ios::eofbit);
            }
            return c;
        }
        catch (const std::exception&)
        {
            is_->clear(is_->rdstate() | std::ios::badbit);
            return traits_type::eof();
        }
    }

    size_t read(value_type* p, size_t length)
    {
        try
        {
            std::streamsize count = sbuf_->sgetn(p, length); // never negative
            if (static_cast<size_t>(count) < length)
            {
                is_->clear(is_->rdstate() | std::ios::eofbit);
            }
            position_ += length;
            return static_cast<size_t>(count);
        }
        catch (const std::exception&)
        {
            is_->clear(is_->rdstate() | std::ios::badbit | std::ios::eofbit);
            return 0;
        }
    }

    template <class OutputIt>
    typename std::enable_if<!std::is_same<OutputIt,value_type*>::value,size_t>::type
    read(OutputIt p, size_t length)
    {
        size_t count = 0;
        try
        {
            for (count = 0; count < length; ++count)
            {
                int c = sbuf_->sbumpc();
                if (c == traits_type::eof())
                {
                    is_->clear(is_->rdstate() | std::ios::eofbit);
                    return count;
                }
                else
                {
                    ++position_;
                }
                *p++ = (value_type)c;
            }
            return count;
        }
        catch (const std::exception&)
        {
            is_->clear(is_->rdstate() | std::ios::badbit | std::ios::eofbit);
            return count;
        }
    }
};

// string_source

template <class CharT>
class string_source 
{
public:
    typedef CharT value_type;
    typedef std::char_traits<CharT> traits_type;
    typedef jsoncons::basic_string_view<value_type> string_view_type;
private:
    const value_type* data_;
    const value_type* input_ptr_;
    const value_type* input_end_;
    bool eof_;

    // Noncopyable 
    string_source(const string_source&) = delete;
    string_source& operator=(const string_source&) = delete;
public:
    string_source(string_source&&) = default;

    string_source(const string_view_type& s)
        : data_(s.data()), input_ptr_(s.data()), input_end_(s.data()+s.size()), eof_(s.size() == 0)
    {
    }

    string_source(const std::basic_string<value_type>& s)
        : data_(s.data()), input_ptr_(s.data()), input_end_(s.data()+s.size()), eof_(s.size() == 0)
    {
    }

    string_source(const value_type* data, size_t size)
        : data_(data), input_ptr_(data), input_end_(data+size), eof_(size == 0)  
    {
    }

    string_source& operator=(string_source&&) = default;

    bool eof() const
    {
        return eof_;  
    }

    bool is_error() const
    {
        return false;  
    }

    size_t position() const
    {
        return (input_ptr_ - data_)/sizeof(value_type) + 1;
    }

    size_t get(value_type& c)
    {
        if (input_ptr_ < input_end_)
        {
            c = *input_ptr_++;
            return 1;
        }
        else
       {
            eof_ = true;
            input_ptr_ = input_end_;
            return 0;
        }
    }

    int get()
    {
        if (input_ptr_ < input_end_)
        {
            return *input_ptr_++;
        }
        else
       {
            eof_ = true;
            input_ptr_ = input_end_;
            return traits_type::eof();
        }
    }

    void ignore(size_t count)
    {
        size_t len;
        if ((size_t)(input_end_ - input_ptr_) < count)
        {
            len = input_end_ - input_ptr_;
            eof_ = true;
        }
        else
        {
            len = count;
        }
        input_ptr_ += len;
    }

    int peek() 
    {
        return input_ptr_ < input_end_ ? *input_ptr_ : traits_type::eof();
    }

    size_t read(value_type* p, size_t length)
    {
        size_t len;
        if ((size_t)(input_end_ - input_ptr_) < length)
        {
            len = input_end_ - input_ptr_;
            eof_ = true;
        }
        else
        {
            len = length;
        }
        std::memcpy(p, input_ptr_, len*sizeof(value_type));
        input_ptr_  += len;
        return len;
    }

    template <class OutputIt>
    typename std::enable_if<!std::is_same<OutputIt,value_type*>::value,size_t>::type
    read(OutputIt d_first, size_t count)
    {
        size_t len;
        if ((size_t)(input_end_ - input_ptr_) < count)
        {
            len = input_end_ - input_ptr_;
            eof_ = true;
        }
        else
        {
            len = count;
        }
        for (size_t i = 0; i < len; ++i)
        {
            *d_first++ = *input_ptr_++;
        }
        return len;
    }
};

// binary sources

struct binary_traits
{
    static constexpr int eof() 
    {
        return std::char_traits<char>::eof();
    }
};

class binary_stream_source 
{
public:
    typedef uint8_t value_type;
    typedef binary_traits traits_type;
private:
    std::istream* is_;
    std::streambuf* sbuf_;
    size_t position_;

    // Noncopyable 
    binary_stream_source(const binary_stream_source&) = delete;
    binary_stream_source& operator=(const binary_stream_source&) = delete;
public:
    binary_stream_source(binary_stream_source&&) = default;

    binary_stream_source(std::istream& is)
        : is_(std::addressof(is)), sbuf_(is.rdbuf()), position_(0)
    {
    }

    ~binary_stream_source()
    {
    }

    binary_stream_source& operator=(binary_stream_source&&) = default;

    bool eof() const
    {
        return is_->eof();  
    }

    bool is_error() const
    {
        return is_->bad();  
    }

    size_t position() const
    {
        return position_;
    }

    size_t get(value_type& c)
    {
        try
        {
            int val = sbuf_->sbumpc();
            if (!(val == traits_type::eof()))
            {
                c = (value_type)val;
                ++position_;
                return 1;
            }
            else
            {
                is_->clear(is_->rdstate() | std::ios::eofbit);
                return 0;
            }
        }
        catch (const std::exception&)
        {
            is_->clear(is_->rdstate() | std::ios::badbit | std::ios::eofbit);
            return 0;
        }
    }

    int get()
    {
        try
        {
            int c = sbuf_->sbumpc();
            if (c == traits_type::eof())
            {
                is_->clear(is_->rdstate() | std::ios::eofbit);
            }
            else
            {
                ++position_;
            }
            return c;
        }
        catch (const std::exception&)
        {
            is_->clear(is_->rdstate() | std::ios::badbit | std::ios::eofbit);
            return traits_type::eof();
        }
    }

    void ignore(size_t count)
    {
        try
        {
            for (size_t i = 0; i < count; ++i)
            {
                int c = sbuf_->sbumpc();
                if (c == traits_type::eof())
                {
                    is_->clear(is_->rdstate() | std::ios::eofbit);
                    return;
                }
                else
                {
                    ++position_;
                }
            }
        }
        catch (const std::exception&)
        {
            is_->clear(is_->rdstate() | std::ios::badbit | std::ios::eofbit);
        }
    }

    int peek() 
    {
        try
        {
            int c = sbuf_->sgetc();
            if (c == traits_type::eof())
            {
                is_->clear(is_->rdstate() | std::ios::eofbit);
            }
            return c;
        }
        catch (const std::exception&)
        {
            is_->clear(is_->rdstate() | std::ios::badbit);
            return traits_type::eof();
        }
    }

    template <class OutputIt>
    size_t read(OutputIt p, size_t length)
    {
        size_t count = 0;
        try
        {
            for (count = 0; count < length; ++count)
            {
                int c = sbuf_->sbumpc();
                if (c == traits_type::eof())
                {
                    is_->clear(is_->rdstate() | std::ios::eofbit);
                    return count;
                }
                else
                {
                    ++position_;
                }
                *p++ = (value_type)c;
            }
            return count;
        }
        catch (const std::exception&)
        {
            is_->clear(is_->rdstate() | std::ios::badbit | std::ios::eofbit);
            return count;
        }
    }
};

class buffer_source 
{
public:
    typedef uint8_t value_type;
    typedef binary_traits traits_type;
private:
    const value_type* data_;
    const value_type* input_ptr_;
    const value_type* input_end_;
    bool eof_;

    // Noncopyable 
    buffer_source(const buffer_source&) = delete;
    buffer_source& operator=(const buffer_source&) = delete;
public:
    buffer_source(buffer_source&&) = default;

    buffer_source(const std::vector<value_type>& s)
        : data_(s.data()), 
          input_ptr_(s.data()), 
          input_end_(s.data()+s.size()), 
          eof_(s.size() == 0)
    {
    }

    buffer_source(const value_type* data, size_t size)
        : data_(data), 
          input_ptr_(data), 
          input_end_(data+size), 
          eof_(size == 0)  
    {
    }

    buffer_source& operator=(buffer_source&&) = default;

    bool eof() const
    {
        return eof_;  
    }

    bool is_error() const
    {
        return false;  
    }

    size_t position() const
    {
        return input_ptr_ - data_ + 1;
    }

    size_t get(value_type& c)
    {
        if (input_ptr_ < input_end_)
        {
            c = *input_ptr_++;
            return 1;
        }
        else
       {
            eof_ = true;
            input_ptr_ = input_end_;
            return 0;
        }
    }

    int get()
    {
        if (input_ptr_ < input_end_)
        {
            return *input_ptr_++;
        }
        else
       {
            eof_ = true;
            input_ptr_ = input_end_;
            return traits_type::eof();
        }
    }

    void ignore(size_t count)
    {
        size_t len;
        if ((size_t)(input_end_ - input_ptr_) < count)
        {
            len = input_end_ - input_ptr_;
            eof_ = true;
        }
        else
        {
            len = count;
        }
        input_ptr_ += len;
    }

    int peek() 
    {
        return input_ptr_ < input_end_ ? *input_ptr_ : traits_type::eof();
    }

    size_t read(value_type* p, size_t length)
    {
        size_t len;
        if ((size_t)(input_end_ - input_ptr_) < length)
        {
            len = input_end_ - input_ptr_;
            eof_ = true;
        }
        else
        {
            len = length;
        }
        std::memcpy(p, input_ptr_, len);
        input_ptr_  += len;
        return len;
    }

    template <class OutputIt>
    typename std::enable_if<!std::is_same<OutputIt,value_type*>::value,size_t>::type
    read(OutputIt d_first, size_t count)
    {
        size_t len;
        if ((size_t)(input_end_ - input_ptr_) < count)
        {
            len = input_end_ - input_ptr_;
            eof_ = true;
        }
        else
        {
            len = count;
        }
        for (size_t i = 0; i < len; ++i)
        {
            *d_first++ = *input_ptr_++;
        }
        return len;
    }
};

}

#endif
