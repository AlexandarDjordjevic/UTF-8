#include <stddef.h>
#include <UTF8/utf8.hpp>

namespace{

    const std::vector< UTF8::UnicodeCodePoint > UTF8_REPLACEMENT_CHARACTER = {0xef, 0xbf, 0xbd};
    const size_t UTF8_ENCODED_CHAR_MAX_LEN = 4;
    
    /*
        Consts
    */
    const UTF8::UnicodeCodePoint LAST_LEGAL_UNICODE_CODEPOINT = 0x10FFFF;
    const UTF8::UnicodeCodePoint FIRST_SUROGATE = 0xD800, LAST_SUROGATE = 0xDFFF;
    const UTF8::UnicodeCodePoint UNICODE_REPLACEMEMENT_CHARACTER =0xFFFD;

    template<typename T> 
    size_t log2n(T number){
        size_t answer = 0;
        while(number > 0){
            answer++;
            number >>= 1;
        }
        return answer;
    }
}
namespace UTF8{

    std::vector< UnicodeCodePoint > AsciiToUnicode(const std::string& ascii){
        return std::vector< UnicodeCodePoint > (ascii.begin(), ascii.end());
    }

    struct UTF8::Impl {
        UnicodeCodePoint unicodeCodePointValue;
        size_t utf8RepresnetationLen;
        size_t encodingPosition;
    };

    UTF8::UTF8()
        : pimpl(new Impl)
    {
    }

    UTF8::~UTF8() = default;

    std::vector< uint8_t> UTF8::Encode(const std::vector< UnicodeCodePoint >& codePoints){
        std::vector< uint8_t > encode;
        encode.reserve(4);
        for ( auto codePoint : codePoints){
            const auto bitsForCodePoint = log2n(codePoint);
            if(bitsForCodePoint <= 7){
                encode.emplace_back(UnicodeCodePoint(codePoint & 0x7f));
            } else if (bitsForCodePoint <= 11){ 
                //chars between 0x80 - 0x7ff
                //1101 1111 | 1011 1111 
                encode.emplace_back(UnicodeCodePoint(((codePoint >> 6) & 0x1f) | 0xC0));
                encode.emplace_back(UnicodeCodePoint((codePoint & 0x3f) | 0x80));
            } else if (bitsForCodePoint <= 16){
                //chars between 0x800 - 0xffff
                //1110 1111 | 1011 1111 | 1011 1111
                if(
                    (codePoint >= FIRST_SUROGATE) && 
                    (codePoint <= LAST_SUROGATE)
                )
                {
                    encode.insert(encode.end(), UTF8_REPLACEMENT_CHARACTER.begin(), UTF8_REPLACEMENT_CHARACTER.end());
                } else {
                    encode.emplace_back(UnicodeCodePoint(((codePoint >> 12) & 0x0f) | 0xE0));
                    encode.emplace_back(UnicodeCodePoint(((codePoint >> 6) & 0x3f) | 0x80));
                    encode.emplace_back(UnicodeCodePoint((codePoint & 0x3f) | 0x80));
                }
            } else if (bitsForCodePoint <= 21 && codePoint <= LAST_LEGAL_UNICODE_CODEPOINT){
                //1111 0111 | 1011 1111 | 1011 1111 | 1011 1111
                encode.emplace_back(UnicodeCodePoint(((codePoint >> 18) & 0x7) | 0xF0));
                encode.emplace_back(UnicodeCodePoint(((codePoint >> 12) & 0x3f) | 0x80));
                encode.emplace_back(UnicodeCodePoint(((codePoint >> 6) & 0x3f) | 0x80));
                encode.emplace_back(UnicodeCodePoint((codePoint & 0x3f) | 0x80));
            } else {
                //The standard also recommends replacing each error with the replacement character "�" (U+FFFD)
                encode.insert(encode.end(), UTF8_REPLACEMENT_CHARACTER.begin(), UTF8_REPLACEMENT_CHARACTER.end());
            }
        }
        return encode;
    }

    std::vector< UnicodeCodePoint > UTF8::Decode(const std::vector< uint8_t >& encoding){
        std::vector< UnicodeCodePoint > output;
        pimpl->unicodeCodePointValue = 0;
        pimpl->encodingPosition = 0;
        for ( auto chunk = encoding.begin(); chunk != encoding.end(); ++chunk){ 
            if(!(*chunk & 0x80) && (pimpl->encodingPosition == 0)){
                output.push_back(UnicodeCodePoint(*chunk));
            }else{
                if(pimpl->encodingPosition == 0){
                    if((*chunk >> 5) == 0x6){ //b1100 0000
                        pimpl->utf8RepresnetationLen = 11;
                        pimpl->unicodeCodePointValue = *chunk & 0x1F;
                    }else if((*chunk >> 4) == 0xE){//b1110 0000
                        pimpl->utf8RepresnetationLen = 16;
                        pimpl->unicodeCodePointValue = *chunk & 0x0F;
                    }else if((*chunk >> 3) == 0x1E){//b1111 0000
                        pimpl->utf8RepresnetationLen = 21;
                        pimpl->unicodeCodePointValue = *chunk & 0x07;
                    }else{
                       output.push_back(UNICODE_REPLACEMEMENT_CHARACTER);
                       pimpl->utf8RepresnetationLen = 0; 
                    }
                    pimpl->encodingPosition = pimpl->utf8RepresnetationLen % 6;
                    if((encoding.end() - chunk) < (pimpl->utf8RepresnetationLen / 6 + 1)){
                        output.push_back(UNICODE_REPLACEMEMENT_CHARACTER);
                        pimpl->unicodeCodePointValue = 0;
                        pimpl->encodingPosition = 0;
                    }
                }
                else
                {
                    if((*chunk & 0xc0) == 0x80){
                        pimpl->unicodeCodePointValue <<= 6;
                        pimpl->unicodeCodePointValue += (*chunk & 0x3f);
                        pimpl->encodingPosition += 6;
                        if(pimpl->encodingPosition == pimpl->utf8RepresnetationLen )
                        {
                            output.push_back(pimpl->unicodeCodePointValue);
                            pimpl->unicodeCodePointValue = 0;
                            pimpl->encodingPosition = 0;
                        }
                    }else{
                        output.push_back(UNICODE_REPLACEMEMENT_CHARACTER);
                        pimpl->unicodeCodePointValue = 0;
                        pimpl->encodingPosition = 0;
                    }
                    
                }
            }
        }     
        return output;
    }

    std::vector< UnicodeCodePoint > UTF8::Decode(const std::string& encoding){
        return Decode( std::vector< uint8_t > (encoding.begin(), encoding.end()));
    }
}
