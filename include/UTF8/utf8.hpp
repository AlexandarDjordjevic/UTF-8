#pragma once

#include <vector>
#include <memory>
#include <stdint.h>

namespace UTF8
{
    typedef uint32_t UnicodeCodePoint;

    std::vector< UnicodeCodePoint > AsciiToUnicode(const std::string& ascii);
    class UTF8 {

    public:
        UTF8();
        ~UTF8();
        UTF8(const UTF8&) = delete;
        UTF8(UTF8&&) = delete;
        UTF8& operator=(const UTF8&) = delete;
        UTF8& operator=(UTF8&) = delete;

        static std::vector< uint8_t > Encode(const std::vector< UnicodeCodePoint >& codePoints);
        std::vector< UnicodeCodePoint > Decode(const std::vector< uint8_t >& utf8Input); 
    private:
        struct Impl;
        std::unique_ptr< Impl > pimpl;
    };
} // namespace UTF8