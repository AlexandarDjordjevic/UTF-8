#include <gtest/gtest.h>
#include <vector>
#include <stdint.h>

#include <UTF8/utf8.hpp>

TEST(UTF8Test, AsciiToUnicode){
    const std::vector< UTF8::UnicodeCodePoint > expected{ 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x57, 0x6f, 0x72, 0x6c, 0x64 };
    const auto result = UTF8::AsciiToUnicode("Hello World");

    ASSERT_EQ(expected, result);
}

   /*
    Test cases:
    Unicode code point  - Character - UTF-8         - Name
    -----------------------------------------------------------------------
    U+0041              -   A       - 41            - Latin capital letter A
    U+007F              -           - 7F            - <control>
    U+0080              -           - C2 80         - <control>
    U+00A1              -   ¡       - C2 A1         - Inverted exclamation mark
    U+0100              -   Ā       - C4 80         - Latin capital letter A with macron
    U+03FF              -   Ͽ       - CF BF         - Greek capital reversed dotted lunate sigma symbol
    U+0400              -   Ѐ       - D0 80         - Cyrillic capital letter Ie with grave
    U+07FF              -   ߿       - DF BF         - Nko Taman Sign
    U+0800              -   ࠀ       - E0 A0 80      - Samaritan Letter Alaf
    U+FFFF              -           - EF BF BF      - 
    U+2003E             -   𠀾       - F0 A0 80 BE   -
    U+233B4             -   𣎴       - F0 A3 8E B4   - Chinese CodePoints_acter meaning 'stump of tree'
    
 */

TEST(UTF8_Encode, UnicodeCodePoints_00to7f){
    UTF8::UTF8 utf8;
    const std::vector< uint8_t > expected{ 0x41, 0x7f };
    const auto result = utf8.Encode(UTF8::AsciiToUnicode("A\x7f"));
    ASSERT_EQ(expected, result);
}

TEST(UTF8_Encode, UnicodeCodePoints_80to7FF){
    UTF8::UTF8 utf8;
    const std::vector< uint8_t > expected{ 0xC2, 0x80, 0xC2, 0xA1, 0xC4, 0x80, 0xCF, 0xBF, 0xD0, 0x80, 0xDF, 0xBF };
    const auto result = utf8.Encode({0x80, 0xA1, 0x100, 0x3ff, 0x400, 0x7ff});
    ASSERT_EQ(expected, result);
}

TEST(UTF8_Encode, UnicodeCodePoints_800toFFFF){
    UTF8::UTF8 utf8;
    const std::vector< uint8_t > expected{ 0xE0, 0xA0, 0x80, 0xEF, 0xBF, 0xBF };
    const auto result = utf8.Encode({0x800, 0xffff});
    ASSERT_EQ(expected, result);
}

TEST(UTF8_Encode, UnicodeCodePoints_10000to10FFFF){
    UTF8::UTF8 utf8;
    const std::vector< uint8_t > expected{ 0xF0, 0xA0, 0x80, 0xBE, 0xF0, 0xA3, 0x8E, 0xB4 };
    const auto result = utf8.Encode({0x2003E, 0x233B4});
    ASSERT_EQ(expected, result);
}

TEST(UTF8_Encode, InvalidUnicodeCodePoints_10FFFFto1FFFFF){
    UTF8::UTF8 utf8;
    const std::vector< uint8_t > expected{ 0xF0, 0xA0, 0x80, 0xBE, 0xEF, 0xBF, 0xBD, 0xF0, 0xA3, 0x8E, 0xB4 };
    const auto result = utf8.Encode({0x2003E, 0x110000, 0x233B4});
    ASSERT_EQ(expected, result);
}

TEST(UTF8_Encode, InvalidUnicodeCodePoints_Over1FFFFF){
    UTF8::UTF8 utf8;
    const std::vector< uint8_t > expected{ 0xF0, 0xA0, 0x80, 0xBE, 0xEF, 0xBF, 0xBD, 0xF0, 0xA3, 0x8E, 0xB4 };
    const auto result = utf8.Encode({0x2003E, 0x200000, 0x233B4});
    ASSERT_EQ(expected, result);
}

/*
* Since RFC 3629 (November 2003), the high and low surrogate halves used by UTF-16 (U+D800 through U+DFFF)and code 
* points not encodable by UTF-16 (those after U+10FFFF) are not legal Unicode values, and their UTF-8 encoding must be
* treated as an invalid byte sequence.
*/
TEST(UTF8_Encode, InvalidUnicodeCodePoints_HightAndLowSurrogateHalves){
    UTF8::UTF8 utf8;
    const std::vector< uint8_t > replacementCharacter = {0xEF, 0xBF, 0xBD};
    ASSERT_EQ((std::vector< uint8_t >{0xED, 0x9F, 0xBF}), utf8.Encode({0xD7FF}));
    ASSERT_EQ(replacementCharacter, utf8.Encode({0xD800}));
    ASSERT_EQ(replacementCharacter, utf8.Encode({0xD805}));
    ASSERT_EQ(replacementCharacter, utf8.Encode({0xDFFF}));
    ASSERT_EQ((std::vector< uint8_t >{0xEE, 0x80, 0x80}), utf8.Encode({0xE000}));
}


TEST(UTF8_Encode, StressTest){
    struct StressTest{
        std::string encode;
        std::vector< UTF8::UnicodeCodePoint > unicode;
    };

    UTF8::UTF8 utf8;

    const std::vector< StressTest > StressTests{
        {"κόσμε", { 0x03ba, 0x1f79, 0x03c3, 0x03bc, 0x03b5}},
        {"πορώναφάωσπασμέναγυαλιάχωρίςναπάθωτίποτα", 
        {
            0x03c0, 0x03bf, 0x03c1, 0x03ce, 0x03bd, 0x03b1, 0x03c6, 0x03ac, 0x03c9, 0x03c3, 0x03c0, 0x03b1, 0x03c3,
            0x03bc, 0x03ad, 0x03bd, 0x03b1, 0x03b3, 0x03c5, 0x03b1, 0x03bb, 0x03b9, 0x03ac, 0x03c7, 0x03c9, 0x03c1,
            0x03af, 0x03c2, 0x03bd, 0x03b1, 0x03c0, 0x03ac, 0x03b8, 0x03c9, 0x03c4, 0x03af, 0x03c0, 0x03bf, 0x03c4,
            0x03b1
        }},
    };
    for (const auto& test : StressTests){
        ASSERT_EQ(test.unicode, utf8.Decode(test.encode));
    }
}

/***
 * 
 *  DECODE
 * 
 */
TEST(UTF8_Decode, UnicodeCodePoints_00to7f){
    UTF8::UTF8 utf8;
    const std::vector< UTF8::UnicodeCodePoint > expected{'A', 0x7f};
    const auto result = utf8.Decode(std::vector<uint8_t>({0x41, 0x7f}));
    ASSERT_EQ(expected, result);
}

TEST(UTF8_Decode, UnicodeCodePoints_80to7FF){
    UTF8::UTF8 utf8;
    const std::vector< UTF8::UnicodeCodePoint > expected{0x80, 0xA1, 0x100, 0x3ff, 0x400, 0x7ff };
    const auto result = utf8.Decode(std::vector<uint8_t>({ 0xC2, 0x80, 0xC2, 0xA1, 0xC4, 0x80, 0xCF, 0xBF, 0xD0, 0x80, 0xDF, 0xBF }));
    ASSERT_EQ(expected, result);
}

TEST(UTF8_Decode, UnicodeCodePoints_800toFFFF){
    UTF8::UTF8 utf8;
    const std::vector< UTF8::UnicodeCodePoint > expected{0x800, 0xffff};
    const auto result = utf8.Decode(std::vector<uint8_t>({ 0xE0, 0xA0, 0x80, 0xEF, 0xBF, 0xBF }));
    ASSERT_EQ(expected, result);
}

TEST(UTF8_Decode, UnicodeCodePoints_10000to10FFFF){
    UTF8::UTF8 utf8;
    const std::vector<  UTF8::UnicodeCodePoint > expected{0x2003E, 0x233B4};
    const auto result = utf8.Decode(std::vector<uint8_t>({ 0xF0, 0xA0, 0x80, 0xBE, 0xF0, 0xA3, 0x8E, 0xB4 }));
    ASSERT_EQ(expected, result);
}

TEST(UTF8_Decode, InvalidUnicodeCodePoints_10FFFFto1FFFFF){
    UTF8::UTF8 utf8;
    const std::vector<  UTF8::UnicodeCodePoint > expected{0x2003E, 0xFFFD, 0x233B4};
    const auto result = utf8.Decode(std::vector<uint8_t>({ 0xF0, 0xA0, 0x80, 0xBE, 0xEF, 0xBF, 0xBD, 0xF0, 0xA3, 0x8E, 0xB4 }));
    ASSERT_EQ(expected, result);
}

TEST(UTF8_Decode, InvalidUTFSequenceStart){
    UTF8::UTF8 utf8;
    const std::vector<  UTF8::UnicodeCodePoint > expected{ 0xFFFD };
    const auto result = utf8.Decode(std::vector<uint8_t>({ 0x80 }));
    ASSERT_EQ(expected, result);
}

TEST(UTF8_Decode, InvalidUTFSequenceEnd){
    UTF8::UTF8 utf8;
    const std::vector< UTF8::UnicodeCodePoint > expected{0xFFFD};
    const auto result = utf8.Decode(std::vector<uint8_t>({ 0xF1, 0x81, 0x81, 0x70}));
    ASSERT_EQ(expected, result);
}

TEST(UTF8_Decode, InvalidUTFSequenceMissingByte){
    UTF8::UTF8 utf8;
    const std::vector<  UTF8::UnicodeCodePoint > expected{ 0xFFFD, 0xFFFD, 0xFFFD };
    const auto result = utf8.Decode(std::vector<uint8_t>({ 0xF1, 0x81, 0x80 }));
    ASSERT_EQ(expected, result);
}



