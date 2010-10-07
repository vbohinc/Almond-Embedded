#include <avr/pgmspace.h>
#include "display_data.h"

#define FONT_CHAR_MIN 2
#define FONT_CHAR_MAX 95
#define FONT_CHAR_ASCII_OFFSET 32

// FONT SIZE: 0 (6x8)
PROGMEM uint8_t font_0[96] = {6,8,
    {  0,  0,  0,  0,  0,  0}, //   (ASCII 32)
    {  0,  2,  8, 32,  2,  0}, // ! (ASCII 33)
    {  1, 69,  0,  0,  0,  0}, // " (ASCII 34)
    { 81, 79,148,249, 69,  0}, // # (ASCII 35)
    { 33,202, 28, 41,194,  0}, // $ (ASCII 36)
    {  0, 12,180, 33,105,128}, // % (ASCII 37)
    {  0,133,  8, 81, 66,128}, // & (ASCII 38)
    {  1,130, 16,  0,  0,  0}, // ' (ASCII 39)
    {  0,132, 16, 64,128,  0}, // ( (ASCII 40)
    {  1,  2,  8, 33,  0,  0}, // ) (ASCII 41)
    {  2, 70, 60, 98, 64,  0}, // * (ASCII 42)
    {  0,130, 62, 32,128,  0}, // + (ASCII 43)
    {  0,  0,  0, 48,132,  0}, // , (ASCII 44)
    {  0,  0,  0,240,  0,  0}, // - (ASCII 45)
    {  0,  0,  0,  1,134,  0}, // . (ASCII 46)
    {  0,  0,132, 33,  8,  0}, // / (ASCII 47)
    {  0,133, 20, 81, 66,  0}, // 0 (ASCII 48)
    {  0,134,  8, 32,135,  0}, // 1 (ASCII 49)
    {  1,137,  4, 33, 15,  0}, // 2 (ASCII 50)
    {  1,137,  8, 18, 70,  0}, // 3 (ASCII 51)
    {  0,134, 40,240,130,  0}, // 4 (ASCII 52)
    {  3,200, 56, 18, 70,  0}, // 5 (ASCII 53)
    {  1,136, 40,210, 70,  0}, // 6 (ASCII 54)
    {  3,193,  8, 33,  4,  0}, // 7 (ASCII 55)
    {  1,137, 24,146, 70,  0}, // 8 (ASCII 56)
    {  1,137, 44, 80, 70,  0}, // 9 (ASCII 57)
    {  0,  6, 24,  1,134,  0}, // : (ASCII 58)
    {  0,  3, 12,  0,194, 16}, // ; (ASCII 59)
    {  0,  1,  8, 64,129,  0}, // < (ASCII 60)
    {  0,  0, 60,  3,192,  0}, // = (ASCII 61)
    {  0,  4,  8, 16,132,  0}, // > (ASCII 62)
    {  0,133,  4, 32,  2,  0}, // ? (ASCII 63)
    { 49, 41,170,170, 68, 12}, // @ (ASCII 64)
    {  1,137, 36,242, 73,  0}, // A (ASCII 65)
    {  3,137, 56,146, 78,  0}, // B (ASCII 66)
    {  1,137, 32,130, 70,  0}, // C (ASCII 67)
    {  3,137, 36,146, 78,  0}, // D (ASCII 68)
    {  3,200, 56,130, 15,  0}, // E (ASCII 69)
    {  3,200, 56,130,  8,  0}, // F (ASCII 70)
    {  1,137, 32,178, 71,  0}, // G (ASCII 71)
    {  2, 73, 60,146, 73,  0}, // H (ASCII 72)
    {  1,194,  8, 32,135,  0}, // I (ASCII 73)
    {  0,193,  4, 18, 70,  0}, // J (ASCII 74)
    {  2, 74, 48,162,137,  0}, // K (ASCII 75)
    {  2,  8, 32,130, 15,  0}, // L (ASCII 76)
    {  2, 79, 60,146, 73,  0}, // M (ASCII 77)
    {  2, 77, 60,178,201,  0}, // N (ASCII 78)
    {  1,137, 36,146, 70,  0}, // O (ASCII 79)
    {  3,137, 36,226,  8,  0}, // P (ASCII 80)
    {  1,137, 36,210,198,  4}, // Q (ASCII 81)
    {  3,137, 36,226,137,  0}, // R (ASCII 82)
    {  1,200, 32, 96, 78,  0}, // S (ASCII 83)
    {  3,226,  8, 32,130,  0}, // T (ASCII 84)
    {  2, 73, 36,146, 70,  0}, // U (ASCII 85)
    {  2, 73, 36,145,134,  0}, // V (ASCII 86)
    {  2, 73, 36,243,201,  0}, // W (ASCII 87)
    {  2, 73, 24, 98, 73,  0}, // X (ASCII 88)
    {  2, 40,148, 32,130,  0}, // Y (ASCII 89)
    {  3,193,  8, 66, 15,  0}, // Z (ASCII 90)
    {  1,196, 16, 65,  7,  0}, // [ (ASCII 91)
    {  0,  8, 16, 32, 64,128}, // \ (ASCII 92)
    {  1,193,  4, 16, 71,  0}, // ] (ASCII 93)
    {  0,133,  0,  0,  0,  0}, // ^ (ASCII 94)
    {  0,  0,  0,  0,  0, 62}, // _ (ASCII 95)
    {  1,132,  8,  0,  0,  0}, // ` (ASCII 96)
    {  0,  0, 28,146,197,  0}, // a (ASCII 97)
    {  2,  8, 56,146, 78,  0}, // b (ASCII 98)
    {  0,  0, 28,130,  7,  0}, // c (ASCII 99)
    {  0, 65, 28,146,197,  0}, // d (ASCII 100)
    {  0,  0, 24,179,  7,  0}, // e (ASCII 101)
    {  0,133, 16,225,  4,  0}, // f (ASCII 102)
    {  0,  0, 28,145,193, 24}, // g (ASCII 103)
    {  2,  8, 56,146, 73,  0}, // h (ASCII 104)
    {  0,128, 24, 32,135,  0}, // i (ASCII 105)
    {  0, 64,  4, 16, 69,  8}, // j (ASCII 106)
    {  2,  8, 36,226, 73,  0}, // k (ASCII 107)
    {  1,130,  8, 32,135,  0}, // l (ASCII 108)
    {  0,  0, 52,170,168,128}, // m (ASCII 109)
    {  0,  0, 40,210, 73,  0}, // n (ASCII 110)
    {  0,  0, 24,146, 70,  0}, // o (ASCII 111)
    {  0,  0, 56,147,136, 32}, // p (ASCII 112)
    {  0,  0, 28,145,193,  4}, // q (ASCII 113)
    {  0,  0, 40,210,  8,  0}, // r (ASCII 114)
    {  0,  0, 28,192,206,  0}, // s (ASCII 115)
    {  1,  4, 56, 65, 66,  0}, // t (ASCII 116)
    {  0,  0, 36,146,197,  0}, // u (ASCII 117)
    {  0,  0, 20, 81, 66,  0}, // v (ASCII 118)
    {  0,  0, 34,170,167,  0}, // w (ASCII 119)
    {  0,  0, 36, 97,137,  0}, // x (ASCII 120)
    {  0,  0, 36,145,193, 24}, // y (ASCII 121)
    {  0,  0, 60, 33, 15,  0}, // z (ASCII 122)
    { 49,  2, 48, 33,  3,  0}, // { (ASCII 123)
    {  0,130,  8, 32,130,  0}, // | (ASCII 124)
    { 96, 66,  6, 32, 70,  0}, // } (ASCII 125)
    {  1, 74,  0,  0,  0,  0}  // ~ (ASCII 126)
};

// FONT SIZE: 1 (7x13)
PROGMEM uint8_t font_1[96] = {7,13,
    {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, //   (ASCII 32)
    {  0,  0,  1,  2,  4,  8, 16, 32,  0,128,  0,  0}, // ! (ASCII 33)
    {  0,  0,  2,133, 10,  0,  0,  0,  0,  0,  0,  0}, // " (ASCII 34)
    {  0,  0,  0,  5, 10, 62, 40,248,161, 64,  0,  0}, // # (ASCII 35)
    {  0,  0,  1,  7,148, 40, 56, 40, 83,193,  0,  0}, // $ (ASCII 36)
    {  0,  1, 37, 69,  2,  8, 32, 81, 82, 64,  0,  0}, // % (ASCII 37)
    {  0,  0,  2, 10, 20, 16, 80,153, 33,160,  0,  0}, // & (ASCII 38)
    {  0,  0,  1,130,  8,  0,  0,  0,  0,  0,  0,  0}, // ' (ASCII 39)
    {  0,  0,  0,130,  8, 16, 32, 64, 64, 64,  0,  0}, // ( (ASCII 40)
    {  0,  0,  2,  2,  2,  4,  8, 16, 65,  0,  0,  0}, // ) (ASCII 41)
    {  0,  0,  1, 10,142,  8, 56,168, 64,  0,  0,  0}, // * (ASCII 42)
    {  0,  0,  0,  2,  4, 62, 16, 32,  0,  0,  0,  0}, // + (ASCII 43)
    {  0,  0,  0,  0,  0,  0,  0,  0, 96,130,  0,  0}, // , (ASCII 44)
    {  0,  0,  0,  0,  0,  0,124,  0,  0,  0,  0,  0}, // - (ASCII 45)
    {  0,  0,  0,  0,  0,  0,  0,  0, 96,192,  0,  0}, // . (ASCII 46)
    {  0,  0,  0, 64,130,  4, 16, 64,130,  4,  0,  0}, // / (ASCII 47)
    {  0,  0,  1,  5, 17, 34, 68,136,160,128,  0,  0}, // 0 (ASCII 48)
    {  0,  0,  1,  6, 20,  8, 16, 32, 67,224,  0,  0}, // 1 (ASCII 49)
    {  0,  0,  3,136,129,  4, 16, 65,  3,224,  0,  0}, // 2 (ASCII 50)
    {  0,  0,  3,136,129, 12,  4,  9, 17,192,  0,  0}, // 3 (ASCII 51)
    {  0,  0,  0,131, 10, 36, 72,248, 32, 64,  0,  0}, // 4 (ASCII 52)
    {  0,  0,  7,200, 30, 34,  4,  9, 17,192,  0,  0}, // 5 (ASCII 53)
    {  0,  0,  1,196, 16, 60, 68,137, 17,192,  0,  0}, // 6 (ASCII 54)
    {  0,  0,  7,192,130,  4, 16, 32,129,  0,  0,  0}, // 7 (ASCII 55)
    {  0,  0,  3,136,145, 28, 68,137, 17,192,  0,  0}, // 8 (ASCII 56)
    {  0,  0,  3,136,145, 34, 60,  8, 35,128,  0,  0}, // 9 (ASCII 57)
    {  0,  0,  0,  0,  6, 12,  0,  0, 96,192,  0,  0}, // : (ASCII 58)
    {  0,  0,  0,  0,  6, 12,  0,  0, 96,130,  0,  0}, // ; (ASCII 59)
    {  0,  0, 32,130,  8, 32, 32, 32, 32, 32,  0,  0}, // < (ASCII 60)
    {  0,  0,  0,  0, 31,  0,  0,248,  0,  0,  0,  0}, // = (ASCII 61)
    {  0,  2,  2,  2,  2,  2,  8, 32,130,  0,  0,  0}, // > (ASCII 62)
    {  0,  0,  3,136,129,  4, 16, 32,  0,128,  0,  0}, // ? (ASCII 63)
    {  0,  0,  3,136,147, 42, 84,177,  1,224,  0,  0}, // @ (ASCII 64)
    {  0,  0,  1,  5, 17, 34,124,137, 18, 32,  0,  0}, // A (ASCII 65)
    {  0,  0,  7,136,145, 60, 68,137, 19,192,  0,  0}, // B (ASCII 66)
    {  0,  0,  3,136,144, 32, 64,129, 17,192,  0,  0}, // C (ASCII 67)
    {  0,  0,  7,136,145, 34, 68,137, 19,192,  0,  0}, // D (ASCII 68)
    {  0,  0,  7,200, 16, 60, 64,129,  3,224,  0,  0}, // E (ASCII 69)
    {  0,  0,  7,200, 16, 60, 64,129,  2,  0,  0,  0}, // F (ASCII 70)
    {  0,  0,  3,136,144, 32, 76,137, 17,192,  0,  0}, // G (ASCII 71)
    {  0,  0,  4, 72,145, 62, 68,137, 18, 32,  0,  0}, // H (ASCII 72)
    {  0,  0,  3,130,  4,  8, 16, 32, 65,192,  0,  0}, // I (ASCII 73)
    {  0,  0,  0,129,  2,  4,  8, 17, 33,128,  0,  0}, // J (ASCII 74)
    {  0,  0,  4, 72,146, 40,112,145, 18, 32,  0,  0}, // K (ASCII 75)
    {  0,  0,  4,  8, 16, 32, 64,129,  3,224,  0,  0}, // L (ASCII 76)
    {  0,  0,  4, 77,149, 42, 68,137, 18, 32,  0,  0}, // M (ASCII 77)
    {  0,  0,  4, 76,153, 42, 84,153, 50, 32,  0,  0}, // N (ASCII 78)
    {  0,  0,  3,136,145, 34, 68,137, 17,192,  0,  0}, // O (ASCII 79)
    {  0,  0,  7,136,145, 34,120,129,  2,  0,  0,  0}, // P (ASCII 80)
    {  0,  0,  3,136,145, 34, 68,137, 81,192, 64,  0}, // Q (ASCII 81)
    {  0,  0,  7,136,145, 34,120,161, 34, 32,  0,  0}, // R (ASCII 82)
    {  0,  0,  3,136,144, 24,  8,  9, 17,192,  0,  0}, // S (ASCII 83)
    {  0,  0,  7,194,  4,  8, 16, 32, 64,128,  0,  0}, // T (ASCII 84)
    {  0,  0,  4, 72,145, 34, 68,137, 17,192,  0,  0}, // U (ASCII 85)
    {  0,  0,  4, 72,145, 20, 40, 80, 64,128,  0,  0}, // V (ASCII 86)
    {  0,  0,  4, 72,149, 42, 84,169,177, 64,  0,  0}, // W (ASCII 87)
    {  0,  0,  4, 72,138,  8, 16, 81, 18, 32,  0,  0}, // X (ASCII 88)
    {  0,  0,  4, 72,138, 20, 16, 32, 64,128,  0,  0}, // Y (ASCII 89)
    {  0,  0,  7,192,130,  8, 16, 65,  3,224,  0,  0}, // Z (ASCII 90)
    {  0,  0,  3,132,  8, 16, 32, 64,129,192,  0,  0}, // [ (ASCII 91)
    {  0,  0,  4,  8,  8, 16, 16, 16, 32, 32, 64,  0}, // \ (ASCII 92)
    {  0,  0,  3,129,  2,  4,  8, 16, 33,192,  0,  0}, // ] (ASCII 93)
    {  0,  0,130,136,128,  0,  0,  0,  0,  0,  0,  0}, // ^ (ASCII 94)
    {  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,192,  0}, // _ (ASCII 95)
    {  0,  0,  3,  2,  2,  0,  0,  0,  0,  0,  0,  0}, // ` (ASCII 96)
    {  0,  0,  0,  0, 14,  2, 60,137, 49,160,  0,  0}, // a (ASCII 97)
    {  0,  0,  4,  8, 22, 50, 68,137,146,192,  0,  0}, // b (ASCII 98)
    {  0,  0,  0,  0, 14, 34, 64,129, 17,192,  0,  0}, // c (ASCII 99)
    {  0,  0,  0, 64,141, 38, 68,137, 49,160,  0,  0}, // d (ASCII 100)
    {  0,  0,  0,  0, 14, 34,124,129,  1,192,  0,  0}, // e (ASCII 101)
    {  0,  0,  1,132,136, 16,120, 64,129,  0,  0,  0}, // f (ASCII 102)
    {  0,  0,  0,  0,142, 34, 68,113,  1,196, 71,  0}, // g (ASCII 103)
    {  0,  0,  4,  8, 22, 50, 68,137, 18, 32,  0,  0}, // h (ASCII 104)
    {  0,  0,  1,  0, 12,  8, 16, 32, 65,192,  0,  0}, // i (ASCII 105)
    {  0,  0,  0,128,  6,  4,  8, 16, 34, 68,134,  0}, // j (ASCII 106)
    {  0,  0,  4,  8, 18, 40, 96,161, 34, 32,  0,  0}, // k (ASCII 107)
    {  0,  0,  3,  2,  4,  8, 16, 32, 65,192,  0,  0}, // l (ASCII 108)
    {  0,  0,  0,  0, 26, 42, 84,169, 82, 32,  0,  0}, // m (ASCII 109)
    {  0,  0,  0,  0, 22, 50, 68,137, 18, 32,  0,  0}, // n (ASCII 110)
    {  0,  0,  0,  0, 14, 34, 68,137, 17,192,  0,  0}, // o (ASCII 111)
    {  0,  0,  0,  0, 22, 50, 68,137,146,196,  8,  0}, // p (ASCII 112)
    {  0,  0,  0,  0, 13, 38, 68,137, 49,160, 64,128}, // q (ASCII 113)
    {  0,  0,  0,  0, 22, 50, 64,129,  2,  0,  0,  0}, // r (ASCII 114)
    {  0,  0,  0,  0, 14, 34, 48, 17, 17,192,  0,  0}, // s (ASCII 115)
    {  0,  0,  2,  4, 30, 16, 32, 64,144,192,  0,  0}, // t (ASCII 116)
    {  0,  0,  0,  0, 17, 34, 68,137, 49,160,  0,  0}, // u (ASCII 117)
    {  0,  0,  0,  0, 17, 34, 68, 80,160,128,  0,  0}, // v (ASCII 118)
    {  0,  0,  0,  0, 17, 34, 84,169, 81, 64,  0,  0}, // w (ASCII 119)
    {  0,  0,  0,  0, 17, 20, 16, 32,162, 32,  0,  0}, // x (ASCII 120)
    {  0,  0,  0,  0, 17, 34, 68,152,208, 32,142,  0}, // y (ASCII 121)
    {  0,  0,  0,  0, 31,  4, 16, 65,  3,224,  0,  0}, // z (ASCII 122)
    {  0,  0,  0,194,  4,  8, 96, 32, 64,128,192,  0}, // { (ASCII 123)
    {  0,  0,  1,  2,  4,  8, 16, 32, 64,129,  0,  0}, // | (ASCII 124)
    {  0,  0,  6,  2,  4,  8, 12, 32, 64,134,  0,  0}, // } (ASCII 125)
    {  0,  1, 37, 73,  0,  0,  0,  0,  0,  0,  0,  0}  // ~ (ASCII 126)
};

// FONT SIZE: 2 (9x15)
PROGMEM uint8_t font_2[96] = {9,15,
    {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, //   (ASCII 32)
    {  0,  0,  0,  3,  1,128,192, 96, 48, 24, 12,  6,  0,  1,128,  0,  0}, // ! (ASCII 33)
    {  0,  0,  0,  6,195, 97,176,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, // " (ASCII 34)
    {  0,  0,  0,  0,  6,195, 99,248,216,108,127, 27, 13,128,  0,  0,  0}, // # (ASCII 35)
    {  0,  0,  0,  1,  3,227, 89,160,240, 62,  7,130,205, 99,224, 64,  0}, // $ (ASCII 36)
    {  0,  0,  0,  8,110,103, 49, 48, 48, 24, 25, 25,204,236, 32,  0,  0}, // % (ASCII 37)
    {  0,  0,  0,  7,  6,195, 97,224, 96,121,103,177,153,199,176,  0,  0}, // & (ASCII 38)
    {  0,  0,  0,  3,129,193,128,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, // ' (ASCII 39)
    {  0,  0,  1,129,129,128,192,192, 96, 48, 24,  6,  3,  0,192, 48,  0}, // ( (ASCII 40)
    {  0,  0, 24,  6,  1,128,192, 48, 24, 12,  6,  6,  3,  3,  3,  0,  0}, // ) (ASCII 41)
    {  0,  0,  0,  0,  0,  0,  1,176,112,254, 28, 27,  0,  0,  0,  0,  0}, // * (ASCII 42)
    {  0,  0,  0,  0,  0,  0,192, 96, 48,255, 12,  6,  3,  0,  0,  0,  0}, // + (ASCII 43)
    {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,131,131,  0,  0}, // , (ASCII 44)
    {  0,  0,  0,  0,  0,  0,  0,  0,  0,255,  0,  0,  0,  0,  0,  0,  0}, // - (ASCII 45)
    {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3,  3,192,192,  0}, // . (ASCII 46)
    {  0,  0,  0,  0, 96, 96, 48, 48, 48, 24, 24, 24, 12, 12,  0,  0,  0}, // / (ASCII 47)
    {  0,  0,  0,  3,  3,195, 51, 13,134,195, 97,153,135,129,128,  0,  0}, // 0 (ASCII 48)
    {  0,  0,  0,  3,  3,131,192, 96, 48, 24, 12,  6,  3,  7,224,  0,  0}, // 1 (ASCII 49)
    {  0,  0,  0,  7,134,102, 24, 12, 12, 12, 12, 12, 12, 15,240,  0,  0}, // 2 (ASCII 50)
    {  0,  0,  0, 15,140, 96, 24, 24, 56,  6,  1,128,216,199,192,  0,  0}, // 3 (ASCII 51)
    {  0,  0,  0,  0,192,224,240,216,204,198,127,129,128,192, 96,  0,  0}, // 4 (ASCII 52)
    {  0,  0,  0, 31,204,  6,  3,113,204,  3,  1,176,204,195,192,  0,  0}, // 5 (ASCII 53)
    {  0,  0,  0,  7,134,102, 19,  1,184,230, 97,176,204,195,192,  0,  0}, // 6 (ASCII 54)
    {  0,  0,  0, 31,224, 48, 24, 24, 24, 24, 24, 24, 24, 12,  0,  0,  0}, // 7 (ASCII 55)
    {  0,  0,  0,  7,134,102, 25,152,120,102, 97,176,204,195,192,  0,  0}, // 8 (ASCII 56)
    {  0,  0,  0,  7,134,102, 27, 12,206, 59,  1,144,204,195,192,  0,  0}, // 9 (ASCII 57)
    {  0,  0,  0,  0,  0,  0,  0, 96,120, 24,  0,  0,  3,  3,192,192,  0}, // : (ASCII 58)
    {  0,  0,  0,  0,  0,  0,  0, 96,120, 24,  0,  0,  7,131,131,  0,  0}, // ; (ASCII 59)
    {  0,  0,  0,  0,192,192,192,192,192, 96, 24,  6,  1,128, 96,  0,  0}, // < (ASCII 60)
    {  0,  0,  0,  0,  0,  0,  0,  1,254,  0,  0, 63,192,  0,  0,  0,  0}, // = (ASCII 61)
    {  0,  0,  0, 24,  6,  1,128, 96, 24, 12, 12, 12, 12, 12,  0,  0,  0}, // > (ASCII 62)
    {  0,  0,  0,  7,198, 51, 24, 12, 12, 12, 12,  0,  3,  1,128,  0,  0}, // ? (ASCII 63)
    {  0,  0,  0,  7,134,102, 27, 61,182,219,103, 48, 12, 99,224,  0,  0}, // @ (ASCII 64)
    {  0,  0,  0,  3,  3,195, 51, 13,134,195,127,176,216,108, 48,  0,  0}, // A (ASCII 65)
    {  0,  0,  0, 31,140,102, 27, 25,248,198, 97,176,216,207,192,  0,  0}, // B (ASCII 66)
    {  0,  0,  0,  7,198, 54, 11,  1,128,192, 96, 48, 76, 99,224,  0,  0}, // C (ASCII 67)
    {  0,  0,  0, 31,140,102, 27, 13,134,195, 97,176,216,207,192,  0,  0}, // D (ASCII 68)
    {  0,  0,  0, 31,204,  6,  3,  1,248,192, 96, 48, 24, 15,224,  0,  0}, // E (ASCII 69)
    {  0,  0,  0, 31,236,  6,  3,  1,248,192, 96, 48, 24, 12,  0,  0,  0}, // F (ASCII 70)
    {  0,  0,  0,  7,198, 54,  3,  1,128,199, 97,176,204, 99,224,  0,  0}, // G (ASCII 71)
    {  0,  0,  0, 24,108, 54, 27, 13,254,195, 97,176,216,108, 48,  0,  0}, // H (ASCII 72)
    {  0,  0,  0, 15,193,128,192, 96, 48, 24, 12,  6,  3,  7,224,  0,  0}, // I (ASCII 73)
    {  0,  0,  0,  3,192, 96, 48, 24, 12,  6,  3, 17,141,131,128,  0,  0}, // J (ASCII 74)
    {  0,  0,  0, 24,108,102, 99, 97,224,240,108, 51, 24,204, 48,  0,  0}, // K (ASCII 75)
    {  0,  0,  0, 24, 12,  6,  3,  1,128,192, 96, 48, 24, 15,224,  0,  0}, // L (ASCII 76)
    {  0,  0,  0, 24,110,119,251,109,182,219, 97,176,216,108, 48,  0,  0}, // M (ASCII 77)
    {  0,  0,  0, 24,110, 55,155,205,182,219,103,177,216,236, 48,  0,  0}, // N (ASCII 78)
    {  0,  0,  0,  7,134,102, 27, 13,134,195, 97,176,204,195,192,  0,  0}, // O (ASCII 79)
    {  0,  0,  0, 31,204, 54, 27, 13,252,192, 96, 48, 24, 12,  0,  0,  0}, // P (ASCII 80)
    {  0,  0,  0,  7,134,102, 27, 13,134,195,109,179,204,195,208,  0,  0}, // Q (ASCII 81)
    {  0,  0,  0, 31,204, 54, 27, 13,252,248,102, 49,152,108, 48,  0,  0}, // R (ASCII 82)
    {  0,  0,  0, 15,204, 54,  3,  0,252,  3,  1,128,216,103,224,  0,  0}, // S (ASCII 83)
    {  0,  0,  0, 31,225,128,192, 96, 48, 24, 12,  6,  3,  1,128,  0,  0}, // T (ASCII 84)
    {  0,  0,  0, 24,108, 54, 27, 13,134,195, 97,176,204,195,192,  0,  0}, // U (ASCII 85)
    {  0,  0,  0, 24,108, 54, 25,152,204,102, 30, 15,  3,  1,128,  0,  0}, // V (ASCII 86)
    {  0,  0,  0, 24,108, 54, 27, 13,182,219,109,191,220,236, 48,  0,  0}, // W (ASCII 87)
    {  0,  0,  0, 24,108, 51, 48,240, 48, 24, 30, 25,152,108, 48,  0,  0}, // X (ASCII 88)
    {  0,  0,  0, 24,108, 51, 48,240, 48, 24, 12,  6,  3,  1,128,  0,  0}, // Y (ASCII 89)
    {  0,  0,  0, 31,192, 96, 48, 48, 48, 48, 48, 48, 24, 15,224,  0,  0}, // Z (ASCII 90)
    {  0,  0,  0,  7,195,  1,128,192, 96, 48, 24, 12,  6,  3,224,  0,  0}, // [ (ASCII 91)
    {  0,  0,  0, 24,  6,  3,  0,192, 48, 24,  6,  1,128,192, 48,  0,  0}, // \ (ASCII 92)
    {  0,  0,  0, 15,128,192, 96, 48, 24, 12,  6,  3,  1,135,192,  0,  0}, // ] (ASCII 93)
    {  0,  0,  6,  7,134, 98, 16,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, // ^ (ASCII 94)
    {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,248,  0}, // _ (ASCII 95)
    {  0,  0,  0,  7,  3,128, 96,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, // ` (ASCII 96)
    {  0,  0,  0,  0,  0,  0,  0,248,198,  3, 63,176,216,231,176,  0,  0}, // a (ASCII 97)
    {  0,  0,  0, 24, 12,  6,  3,113,204,195, 97,176,220,205,192,  0,  0}, // b (ASCII 98)
    {  0,  0,  0,  0,  0,  0,  0,248,198,192, 96, 48, 12, 99,224,  0,  0}, // c (ASCII 99)
    {  0,  0,  0,  0, 96, 48, 24,236,206,195, 97,176,204,227,176,  0,  0}, // d (ASCII 100)
    {  0,  0,  0,  0,  0,  0,  0,240,204,195,127,176, 12, 99,224,  0,  0}, // e (ASCII 101)
    {  0,  0,  0,  3,195, 49,152,192, 96,252, 24, 12,  6,  3,  0,  0,  0}, // f (ASCII 102)
    {  0,  0,  0,  0,  0,  0,  1,245,142,198, 99, 31, 24,  7,230, 25,248}, // g (ASCII 103)
    {  0,  0,  0, 24, 12,  6,  3,113,204,195, 97,176,216,108, 48,  0,  0}, // h (ASCII 104)
    {  0,  0,  0,  3,  1,128,  0,224, 48, 24, 12,  6,  3,  7,224,  0,  0}, // i (ASCII 105)
    {  0,  0,  0,  0,192, 96,  0, 56, 12,  6,  3,  1,128,204,102, 49,240}, // j (ASCII 106)
    {  0,  0,  0, 12,  6,  3,  1,152,216,120, 60, 27, 12,198, 48,  0,  0}, // k (ASCII 107)
    {  0,  0,  0,  7,  1,128,192, 96, 48, 24, 12,  6,  3,  3,192,  0,  0}, // l (ASCII 108)
    {  0,  0,  0,  0,  0,  0,  2,217,182,219,109,182,219,109,176,  0,  0}, // m (ASCII 109)
    {  0,  0,  0,  0,  0,  0,  3,113,204,195, 97,176,216,108, 48,  0,  0}, // n (ASCII 110)
    {  0,  0,  0,  0,  0,  0,  0,240,204,195, 97,176,204,195,192,  0,  0}, // o (ASCII 111)
    {  0,  0,  0,  0,  0,  0,  3,113,204,195, 97,176,220,205,198,  3,  0}, // p (ASCII 112)
    {  0,  0,  0,  0,  0,  0,  0,236,206,195, 97,176,204,227,176, 24, 12}, // q (ASCII 113)
    {  0,  0,  0,  0,  0,  0,  3,120,230, 96, 48, 24, 12,  6,  0,  0,  0}, // r (ASCII 114)
    {  0,  0,  0,  0,  0,  0,  1,249,134,192, 63,  0,216,103,224,  0,  0}, // s (ASCII 115)
    {  0,  0,  0,  0,  3,  1,131,240, 96, 48, 24, 12,  6, 97,224,  0,  0}, // t (ASCII 116)
    {  0,  0,  0,  0,  0,  0,  3, 13,134,195, 97,176,204,227,176,  0,  0}, // u (ASCII 117)
    {  0,  0,  0,  0,  0,  0,  3, 13,134,102, 51, 15,  7,129,128,  0,  0}, // v (ASCII 118)
    {  0,  0,  0,  0,  0,  0,  3, 13,134,219,109,182,223,230, 96,  0,  0}, // w (ASCII 119)
    {  0,  0,  0,  0,  0,  0,  3, 12,204, 60, 12, 15, 12,204, 48,  0,  0}, // x (ASCII 120)
    {  0,  0,  0,  0,  0,  0,  3, 13,134,195, 97,176,204,227,180, 25,248}, // y (ASCII 121)
    {  0,  0,  0,  0,  0,  0,  1,248, 12, 12, 12, 12, 12,  7,224,  0,  0}, // z (ASCII 122)
    {  0,  0,  0,  1,193,128,192, 96, 96, 48, 12,  6,  3,  0,224,  0,  0}, // { (ASCII 123)
    {  0,  0,  0,  3,  1,128,192, 96, 48, 24, 12,  6,  3,  1,128,  0,  0}, // | (ASCII 124)
    {  0,  0,  0, 14,  1,128,192, 96, 24, 12, 12,  6,  3,  7,  0,  0,  0}, // } (ASCII 125)
    {  0,  0, 28,219,108,224,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}  // ~ (ASCII 126)
};
