/**
 * The MIT License (MIT)
 *
 * This portable library variant of QRCode was written by Stefan Meislinger.
 * This original QRCode library is written and maintained by Richard Moore.
 * Major parts were derived from Project Nayuki's library.
 *
 * Copyright (c) 2017 Richard Moore     (https://github.com/ricmoo/QRCode)
 * Copyright (c) 2017 Project Nayuki    (https://www.nayuki.io/page/qr-code-generator-library)
 * Copyright (c) 2025 Stefan Meislinger (https://codeberg.org/gatelinker/QRCodeGenPort)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 *  Special thanks to Nayuki (https://www.nayuki.io/) from which this library was
 *  heavily inspired and compared against.
 *
 *  See: https://github.com/nayuki/QR-Code-generator/tree/master/cpp
 */

#include "qrcode.h"

#include <stdlib.h>
#include <string.h>

#define qrcode_bool_eval(e) (((e) == 0) ? qrcode_false : qrcode_true)
#define QRCODE_MAX_BUFFER_CAPACITY 1024

#ifndef INT32_MAX
#define INT32_MAX 0x7fffffff
#endif

// QR Code Format Encoding
#define QRCODE_MODE_NUMERIC        0
#define QRCODE_MODE_ALPHANUMERIC   1
#define QRCODE_MODE_BYTE           2


/*** Error Correction Lookup tables ***/
/*                                       MEDIUM,  LOW, HIGH, QUART */
#define NUM_ERROR_CORRECTION_CODEWORDS_1  {   10,    7,   17,   13 }
#define NUM_ERROR_CORRECTION_CODEWORDS_2  {   16,   10,   28,   22 }
#define NUM_ERROR_CORRECTION_CODEWORDS_3  {   26,   15,   44,   36 }
#define NUM_ERROR_CORRECTION_CODEWORDS_4  {   36,   20,   64,   52 }
#define NUM_ERROR_CORRECTION_CODEWORDS_5  {   48,   26,   88,   72 }
#define NUM_ERROR_CORRECTION_CODEWORDS_6  {   64,   36,  112,   96 }
#define NUM_ERROR_CORRECTION_CODEWORDS_7  {   72,   40,  130,  108 }
#define NUM_ERROR_CORRECTION_CODEWORDS_8  {   88,   48,  156,  132 }
#define NUM_ERROR_CORRECTION_CODEWORDS_9  {  110,   60,  192,  160 }
#define NUM_ERROR_CORRECTION_CODEWORDS_10 {  130,   72,  224,  192 }
#define NUM_ERROR_CORRECTION_CODEWORDS_11 {  150,   80,  264,  224 }
#define NUM_ERROR_CORRECTION_CODEWORDS_12 {  176,   96,  308,  260 }
#define NUM_ERROR_CORRECTION_CODEWORDS_13 {  198,  104,  352,  288 }
#define NUM_ERROR_CORRECTION_CODEWORDS_14 {  216,  120,  384,  320 }
#define NUM_ERROR_CORRECTION_CODEWORDS_15 {  240,  132,  432,  360 }
#define NUM_ERROR_CORRECTION_CODEWORDS_16 {  280,  144,  480,  408 }
#define NUM_ERROR_CORRECTION_CODEWORDS_17 {  308,  168,  532,  448 }
#define NUM_ERROR_CORRECTION_CODEWORDS_18 {  338,  180,  588,  504 }
#define NUM_ERROR_CORRECTION_CODEWORDS_19 {  364,  196,  650,  546 }
#define NUM_ERROR_CORRECTION_CODEWORDS_20 {  416,  224,  700,  600 }
#define NUM_ERROR_CORRECTION_CODEWORDS_21 {  442,  224,  750,  644 }
#define NUM_ERROR_CORRECTION_CODEWORDS_22 {  476,  252,  816,  690 }
#define NUM_ERROR_CORRECTION_CODEWORDS_23 {  504,  270,  900,  750 }
#define NUM_ERROR_CORRECTION_CODEWORDS_24 {  560,  300,  960,  810 }
#define NUM_ERROR_CORRECTION_CODEWORDS_25 {  588,  312, 1050,  870 }
#define NUM_ERROR_CORRECTION_CODEWORDS_26 {  644,  336, 1110,  952 }
#define NUM_ERROR_CORRECTION_CODEWORDS_27 {  700,  360, 1200, 1020 }
#define NUM_ERROR_CORRECTION_CODEWORDS_28 {  728,  390, 1260, 1050 }
#define NUM_ERROR_CORRECTION_CODEWORDS_29 {  784,  420, 1350, 1140 }
#define NUM_ERROR_CORRECTION_CODEWORDS_30 {  812,  450, 1440, 1200 }
#define NUM_ERROR_CORRECTION_CODEWORDS_31 {  868,  480, 1530, 1290 }
#define NUM_ERROR_CORRECTION_CODEWORDS_32 {  924,  510, 1620, 1350 }
#define NUM_ERROR_CORRECTION_CODEWORDS_33 {  980,  540, 1710, 1440 }
#define NUM_ERROR_CORRECTION_CODEWORDS_34 { 1036,  570, 1800, 1530 }
#define NUM_ERROR_CORRECTION_CODEWORDS_35 { 1064,  570, 1890, 1590 }
#define NUM_ERROR_CORRECTION_CODEWORDS_36 { 1120,  600, 1980, 1680 }
#define NUM_ERROR_CORRECTION_CODEWORDS_37 { 1204,  630, 2100, 1770 }
#define NUM_ERROR_CORRECTION_CODEWORDS_38 { 1260,  660, 2220, 1860 }
#define NUM_ERROR_CORRECTION_CODEWORDS_39 { 1316,  720, 2310, 1950 }
#define NUM_ERROR_CORRECTION_CODEWORDS_40 { 1372,  750, 2430, 2040 }


/*                                     MED,LOW,HIGH,QUART */
#define NUM_ERROR_CORRECTION_BLOCKS_1  {  1,  1,  1,  1 }
#define NUM_ERROR_CORRECTION_BLOCKS_2  {  1,  1,  1,  1 }
#define NUM_ERROR_CORRECTION_BLOCKS_3  {  1,  1,  2,  2 }
#define NUM_ERROR_CORRECTION_BLOCKS_4  {  2,  1,  4,  2 }
#define NUM_ERROR_CORRECTION_BLOCKS_5  {  2,  1,  4,  4 }
#define NUM_ERROR_CORRECTION_BLOCKS_6  {  4,  2,  4,  4 }
#define NUM_ERROR_CORRECTION_BLOCKS_7  {  4,  2,  5,  6 }
#define NUM_ERROR_CORRECTION_BLOCKS_8  {  4,  2,  6,  6 }
#define NUM_ERROR_CORRECTION_BLOCKS_9  {  5,  2,  8,  8 }
#define NUM_ERROR_CORRECTION_BLOCKS_10 {  5,  4,  8,  8 }
#define NUM_ERROR_CORRECTION_BLOCKS_11 {  5,  4, 11,  8 }
#define NUM_ERROR_CORRECTION_BLOCKS_12 {  8,  4, 11, 10 }
#define NUM_ERROR_CORRECTION_BLOCKS_13 {  9,  4, 16, 12 }
#define NUM_ERROR_CORRECTION_BLOCKS_14 {  9,  4, 16, 16 }
#define NUM_ERROR_CORRECTION_BLOCKS_15 { 10,  6, 18, 12 }
#define NUM_ERROR_CORRECTION_BLOCKS_16 { 10,  6, 16, 17 }
#define NUM_ERROR_CORRECTION_BLOCKS_17 { 11,  6, 19, 16 }
#define NUM_ERROR_CORRECTION_BLOCKS_18 { 13,  6, 21, 18 }
#define NUM_ERROR_CORRECTION_BLOCKS_19 { 14,  7, 25, 21 }
#define NUM_ERROR_CORRECTION_BLOCKS_20 { 16,  8, 25, 20 }
#define NUM_ERROR_CORRECTION_BLOCKS_21 { 17,  8, 25, 23 }
#define NUM_ERROR_CORRECTION_BLOCKS_22 { 17,  9, 34, 23 }
#define NUM_ERROR_CORRECTION_BLOCKS_23 { 18,  9, 30, 25 }
#define NUM_ERROR_CORRECTION_BLOCKS_24 { 20, 10, 32, 27 }
#define NUM_ERROR_CORRECTION_BLOCKS_25 { 21, 12, 35, 29 }
#define NUM_ERROR_CORRECTION_BLOCKS_26 { 23, 12, 37, 34 }
#define NUM_ERROR_CORRECTION_BLOCKS_27 { 25, 12, 40, 34 }
#define NUM_ERROR_CORRECTION_BLOCKS_28 { 26, 13, 42, 35 }
#define NUM_ERROR_CORRECTION_BLOCKS_29 { 28, 14, 45, 38 }
#define NUM_ERROR_CORRECTION_BLOCKS_30 { 29, 15, 48, 40 }
#define NUM_ERROR_CORRECTION_BLOCKS_31 { 31, 16, 51, 43 }
#define NUM_ERROR_CORRECTION_BLOCKS_32 { 33, 17, 54, 45 }
#define NUM_ERROR_CORRECTION_BLOCKS_33 { 35, 18, 57, 48 }
#define NUM_ERROR_CORRECTION_BLOCKS_34 { 37, 19, 60, 51 }
#define NUM_ERROR_CORRECTION_BLOCKS_35 { 38, 19, 63, 53 }
#define NUM_ERROR_CORRECTION_BLOCKS_36 { 40, 20, 66, 56 }
#define NUM_ERROR_CORRECTION_BLOCKS_37 { 43, 21, 70, 59 }
#define NUM_ERROR_CORRECTION_BLOCKS_38 { 45, 22, 74, 62 }
#define NUM_ERROR_CORRECTION_BLOCKS_39 { 47, 24, 77, 65 }
#define NUM_ERROR_CORRECTION_BLOCKS_40 { 49, 25, 81, 68 }


#define NUM_RAW_DATA_MODULES_1  208
#define NUM_RAW_DATA_MODULES_2  359
#define NUM_RAW_DATA_MODULES_3  567
#define NUM_RAW_DATA_MODULES_4  807
#define NUM_RAW_DATA_MODULES_5  1079
#define NUM_RAW_DATA_MODULES_6  1383
#define NUM_RAW_DATA_MODULES_7  1568
#define NUM_RAW_DATA_MODULES_8  1936
#define NUM_RAW_DATA_MODULES_9  2336
#define NUM_RAW_DATA_MODULES_10 2768
#define NUM_RAW_DATA_MODULES_11 3232
#define NUM_RAW_DATA_MODULES_12 3728
#define NUM_RAW_DATA_MODULES_13 4256
#define NUM_RAW_DATA_MODULES_14 4651
#define NUM_RAW_DATA_MODULES_15 5243
#define NUM_RAW_DATA_MODULES_16 5867
#define NUM_RAW_DATA_MODULES_17 6523
#define NUM_RAW_DATA_MODULES_18 7211
#define NUM_RAW_DATA_MODULES_19 7931
#define NUM_RAW_DATA_MODULES_20 8683
#define NUM_RAW_DATA_MODULES_21 9252
#define NUM_RAW_DATA_MODULES_22 10068
#define NUM_RAW_DATA_MODULES_23 10916
#define NUM_RAW_DATA_MODULES_24 11796
#define NUM_RAW_DATA_MODULES_25 12708
#define NUM_RAW_DATA_MODULES_26 13652
#define NUM_RAW_DATA_MODULES_27 14628
#define NUM_RAW_DATA_MODULES_28 15371
#define NUM_RAW_DATA_MODULES_29 16411
#define NUM_RAW_DATA_MODULES_30 17483
#define NUM_RAW_DATA_MODULES_31 18587
#define NUM_RAW_DATA_MODULES_32 19723
#define NUM_RAW_DATA_MODULES_33 20891
#define NUM_RAW_DATA_MODULES_34 22091
#define NUM_RAW_DATA_MODULES_35 23008
#define NUM_RAW_DATA_MODULES_36 24272
#define NUM_RAW_DATA_MODULES_37 25568
#define NUM_RAW_DATA_MODULES_38 26896
#define NUM_RAW_DATA_MODULES_39 28256
#define NUM_RAW_DATA_MODULES_40 29648



#if QRCODE_LOCK_VERSION == 0

static const qrcode_uint16_t NUM_ERROR_CORRECTION_CODEWORDS[40][4] ={
    NUM_ERROR_CORRECTION_CODEWORDS_1,  NUM_ERROR_CORRECTION_CODEWORDS_2,  NUM_ERROR_CORRECTION_CODEWORDS_3,  NUM_ERROR_CORRECTION_CODEWORDS_4,
    NUM_ERROR_CORRECTION_CODEWORDS_5,  NUM_ERROR_CORRECTION_CODEWORDS_6,  NUM_ERROR_CORRECTION_CODEWORDS_7,  NUM_ERROR_CORRECTION_CODEWORDS_8,
    NUM_ERROR_CORRECTION_CODEWORDS_9,  NUM_ERROR_CORRECTION_CODEWORDS_10, NUM_ERROR_CORRECTION_CODEWORDS_11, NUM_ERROR_CORRECTION_CODEWORDS_12,
    NUM_ERROR_CORRECTION_CODEWORDS_13, NUM_ERROR_CORRECTION_CODEWORDS_14, NUM_ERROR_CORRECTION_CODEWORDS_15, NUM_ERROR_CORRECTION_CODEWORDS_16,
    NUM_ERROR_CORRECTION_CODEWORDS_17, NUM_ERROR_CORRECTION_CODEWORDS_18, NUM_ERROR_CORRECTION_CODEWORDS_19, NUM_ERROR_CORRECTION_CODEWORDS_20,
    NUM_ERROR_CORRECTION_CODEWORDS_21, NUM_ERROR_CORRECTION_CODEWORDS_22, NUM_ERROR_CORRECTION_CODEWORDS_23, NUM_ERROR_CORRECTION_CODEWORDS_24,
    NUM_ERROR_CORRECTION_CODEWORDS_25, NUM_ERROR_CORRECTION_CODEWORDS_26, NUM_ERROR_CORRECTION_CODEWORDS_27, NUM_ERROR_CORRECTION_CODEWORDS_28,
    NUM_ERROR_CORRECTION_CODEWORDS_29, NUM_ERROR_CORRECTION_CODEWORDS_30, NUM_ERROR_CORRECTION_CODEWORDS_31, NUM_ERROR_CORRECTION_CODEWORDS_32,
    NUM_ERROR_CORRECTION_CODEWORDS_33, NUM_ERROR_CORRECTION_CODEWORDS_34, NUM_ERROR_CORRECTION_CODEWORDS_35, NUM_ERROR_CORRECTION_CODEWORDS_36,
    NUM_ERROR_CORRECTION_CODEWORDS_37, NUM_ERROR_CORRECTION_CODEWORDS_38, NUM_ERROR_CORRECTION_CODEWORDS_39, NUM_ERROR_CORRECTION_CODEWORDS_40
};


static const qrcode_uint8_t NUM_ERROR_CORRECTION_BLOCKS[40][4] = {
    NUM_ERROR_CORRECTION_BLOCKS_1,  NUM_ERROR_CORRECTION_BLOCKS_2,  NUM_ERROR_CORRECTION_BLOCKS_3,  NUM_ERROR_CORRECTION_BLOCKS_4,
    NUM_ERROR_CORRECTION_BLOCKS_5,  NUM_ERROR_CORRECTION_BLOCKS_6,  NUM_ERROR_CORRECTION_BLOCKS_7,  NUM_ERROR_CORRECTION_BLOCKS_8,
    NUM_ERROR_CORRECTION_BLOCKS_9,  NUM_ERROR_CORRECTION_BLOCKS_10, NUM_ERROR_CORRECTION_BLOCKS_11, NUM_ERROR_CORRECTION_BLOCKS_12,
    NUM_ERROR_CORRECTION_BLOCKS_13, NUM_ERROR_CORRECTION_BLOCKS_14, NUM_ERROR_CORRECTION_BLOCKS_15, NUM_ERROR_CORRECTION_BLOCKS_16,
    NUM_ERROR_CORRECTION_BLOCKS_17, NUM_ERROR_CORRECTION_BLOCKS_18, NUM_ERROR_CORRECTION_BLOCKS_19, NUM_ERROR_CORRECTION_BLOCKS_20,
    NUM_ERROR_CORRECTION_BLOCKS_21, NUM_ERROR_CORRECTION_BLOCKS_22, NUM_ERROR_CORRECTION_BLOCKS_23, NUM_ERROR_CORRECTION_BLOCKS_24,
    NUM_ERROR_CORRECTION_BLOCKS_25, NUM_ERROR_CORRECTION_BLOCKS_26, NUM_ERROR_CORRECTION_BLOCKS_27, NUM_ERROR_CORRECTION_BLOCKS_28,
    NUM_ERROR_CORRECTION_BLOCKS_29, NUM_ERROR_CORRECTION_BLOCKS_30, NUM_ERROR_CORRECTION_BLOCKS_31, NUM_ERROR_CORRECTION_BLOCKS_32,
    NUM_ERROR_CORRECTION_BLOCKS_33, NUM_ERROR_CORRECTION_BLOCKS_34, NUM_ERROR_CORRECTION_BLOCKS_35, NUM_ERROR_CORRECTION_BLOCKS_36,
    NUM_ERROR_CORRECTION_BLOCKS_37, NUM_ERROR_CORRECTION_BLOCKS_38, NUM_ERROR_CORRECTION_BLOCKS_39, NUM_ERROR_CORRECTION_BLOCKS_40
};


static const qrcode_uint16_t NUM_RAW_DATA_MODULES[40] = {
    NUM_RAW_DATA_MODULES_1,  NUM_RAW_DATA_MODULES_2,  NUM_RAW_DATA_MODULES_3,  NUM_RAW_DATA_MODULES_4,
    NUM_RAW_DATA_MODULES_5,  NUM_RAW_DATA_MODULES_6,  NUM_RAW_DATA_MODULES_7,  NUM_RAW_DATA_MODULES_8,
    NUM_RAW_DATA_MODULES_9,  NUM_RAW_DATA_MODULES_10, NUM_RAW_DATA_MODULES_11, NUM_RAW_DATA_MODULES_12,
    NUM_RAW_DATA_MODULES_13, NUM_RAW_DATA_MODULES_14, NUM_RAW_DATA_MODULES_15, NUM_RAW_DATA_MODULES_16,
    NUM_RAW_DATA_MODULES_17, NUM_RAW_DATA_MODULES_18, NUM_RAW_DATA_MODULES_19, NUM_RAW_DATA_MODULES_20,
    NUM_RAW_DATA_MODULES_21, NUM_RAW_DATA_MODULES_22, NUM_RAW_DATA_MODULES_23, NUM_RAW_DATA_MODULES_24,
    NUM_RAW_DATA_MODULES_25, NUM_RAW_DATA_MODULES_26, NUM_RAW_DATA_MODULES_27, NUM_RAW_DATA_MODULES_28,
    NUM_RAW_DATA_MODULES_29, NUM_RAW_DATA_MODULES_30, NUM_RAW_DATA_MODULES_31, NUM_RAW_DATA_MODULES_32,
    NUM_RAW_DATA_MODULES_33, NUM_RAW_DATA_MODULES_34, NUM_RAW_DATA_MODULES_35, NUM_RAW_DATA_MODULES_36,
    NUM_RAW_DATA_MODULES_37, NUM_RAW_DATA_MODULES_38, NUM_RAW_DATA_MODULES_39, NUM_RAW_DATA_MODULES_40
};

// @TODO: Put other LOCK_VERSIONS here
#else

#define QR_CONCAT_TOKENS(name, v)  name ## v
#define QR_MAKE_CODEWORDS(v) QR_CONCAT_TOKENS(NUM_ERROR_CORRECTION_CODEWORDS_, v)
#define QR_MAKE_BLOCKS(v) QR_CONCAT_TOKENS(NUM_ERROR_CORRECTION_BLOCKS_, v)
#define QR_MAKE_MODULES(v) QR_CONCAT_TOKENS(NUM_RAW_DATA_MODULES_, v)

static const qrcode_int16_t NUM_ERROR_CORRECTION_CODEWORDS[4] = QR_MAKE_CODEWORDS(QRCODE_LOCK_VERSION);
static const qrcode_int8_t NUM_ERROR_CORRECTION_BLOCKS[4] = QR_MAKE_BLOCKS(QRCODE_LOCK_VERSION);
static const qrcode_uint16_t NUM_RAW_DATA_MODULES = QR_MAKE_MODULES(QRCODE_LOCK_VERSION);

#endif

#if !defined(max)
#   define max(a, b) ((a > b) ? a : b)
#endif


/*** Mode testing and conversion ***/

static qrcode_int8_t getAlphanumeric(char c) {
    
    if (c >= '0' && c <= '9') { return (c - '0'); }
    if (c >= 'A' && c <= 'Z') { return (c - 'A' + 10); }
    
    switch (c) {
        case ' ': return 36;
        case '$': return 37;
        case '%': return 38;
        case '*': return 39;
        case '+': return 40;
        case '-': return 41;
        case '.': return 42;
        case '/': return 43;
        case ':': return 44;
    }
    
    return -1;
}

static qrcode_bool_t isAlphanumeric(const char *text, qrcode_uint16_t length) {
    while (length != 0) {
        if (getAlphanumeric(text[--length]) == -1) { return qrcode_false; }
    }
    return qrcode_true;
}


static qrcode_bool_t isNumeric(const char *text, qrcode_uint16_t length) {
    while (length != 0) {
        const char c = text[--length];
        if (c < '0' || c > '9') { return qrcode_false; }
    }
    return qrcode_true;
}


/*** Counting ***/

// We store the following tightly packed (less 8) in modeInfo
//               <=9  <=26  <= 40
// NUMERIC      ( 10,   12,    14);
// ALPHANUMERIC (  9,   11,    13);
// BYTE         (  8,   16,    16);
static char getModeBits(qrcode_uint8_t version, qrcode_uint8_t mode) {
    // Note: We use 15 instead of 16; since 15 doesn't exist and we cannot store 16 (8 + 8) in 3 bits
    // hex(int("".join(reversed([('00' + bin(x - 8)[2:])[-3:] for x in [10, 9, 8, 12, 11, 15, 14, 13, 15]])), 2))
    qrcode_uint32_t modeInfo = 0x7bbb80a;
    char result;
    
#if QRCODE_LOCK_VERSION == 0 || QRCODE_LOCK_VERSION > 9
    if (version > 9) { modeInfo >>= 9; }
#endif
    
#if QRCODE_LOCK_VERSION == 0 || QRCODE_LOCK_VERSION > 26
    if (version > 26) { modeInfo >>= 9; }
#endif
    
    result = 8 + ((modeInfo >> (3 * mode)) & 0x07);
    if (result == 15) { result = 16; }
    
    return result;
}


/*** BitBucket ***/

typedef struct BitBucket {
    qrcode_uint32_t bitOffsetOrWidth;
    qrcode_uint16_t capacityBytes;
    qrcode_uint8_t *data;
} BitBucket;

static qrcode_uint16_t bb_getGridSizeBytes(qrcode_uint8_t size) {
    return (((size * size) + 7) / 8);
}

static qrcode_uint16_t bb_getBufferSizeBytes(qrcode_uint16_t bits) {
    return ((bits + 7) / 8);
}

static void bb_initBuffer(BitBucket *bitBuffer, qrcode_uint8_t *data, qrcode_uint16_t capacityBytes) {
    bitBuffer->bitOffsetOrWidth = 0;
    bitBuffer->capacityBytes = capacityBytes;
    bitBuffer->data = data;
    
    memset(data, 0, bitBuffer->capacityBytes);
}

static void bb_initGrid(BitBucket *bitGrid, qrcode_uint8_t *data, qrcode_uint8_t size) {
    bitGrid->bitOffsetOrWidth = size;
    bitGrid->capacityBytes = bb_getGridSizeBytes(size);
    bitGrid->data = data;

    memset(data, 0, bitGrid->capacityBytes);
}

static void bb_appendBits(BitBucket *bitBuffer, qrcode_uint32_t val, qrcode_uint8_t length) {
    qrcode_uint32_t offset = bitBuffer->bitOffsetOrWidth;
    qrcode_int8_t i;
    for (i = length - 1; i >= 0; i--, offset++) {
        bitBuffer->data[offset >> 3] |= ((val >> i) & 1) << (7 - (offset & 7));
    }
    bitBuffer->bitOffsetOrWidth = offset;
}

static void bb_setBit(BitBucket *bitGrid, qrcode_uint8_t x, qrcode_uint8_t y, qrcode_bool_t on) {
    const qrcode_uint32_t offset = y * bitGrid->bitOffsetOrWidth + x;
    const qrcode_uint8_t mask = 1 << (7 - (offset & 0x07));
    if (on) {
        bitGrid->data[offset >> 3] |= mask;
    } else {
        bitGrid->data[offset >> 3] &= ~mask;
    }
}

static void bb_invertBit(BitBucket *bitGrid, qrcode_uint8_t x, qrcode_uint8_t y, qrcode_bool_t invert) {
    const qrcode_uint32_t offset = y * bitGrid->bitOffsetOrWidth + x;
    const qrcode_uint8_t mask = 1 << (7 - (offset & 0x07));
    const qrcode_bool_t on = qrcode_bool_eval(bitGrid->data[offset >> 3] & (1 << (7 - (offset & 0x07))));
    if (on ^ invert) {
        bitGrid->data[offset >> 3] |= mask;
    } else {
        bitGrid->data[offset >> 3] &= ~mask;
    }
}

static qrcode_bool_t bb_getBit(BitBucket *bitGrid, qrcode_uint8_t x, qrcode_uint8_t y) {
    const qrcode_uint32_t offset = y * bitGrid->bitOffsetOrWidth + x;
    return qrcode_bool_eval(bitGrid->data[offset >> 3] & (1 << (7 - (offset & 0x07))));
}


/*** Drawing Patterns ***/

// XORs the data modules in this QR Code with the given mask pattern. Due to XOR's mathematical
// properties, calling applyMask(m) twice with the same value is equivalent to no change at all.
// This means it is possible to apply a mask, undo it, and try another mask. Note that a final
// well-formed QR Code symbol needs exactly one mask applied (not zero, not two, etc.).
static void applyMask(BitBucket *modules, BitBucket *isFunction, qrcode_uint8_t mask) {
    qrcode_uint8_t size = (qrcode_uint8_t)modules->bitOffsetOrWidth;
    qrcode_uint8_t x, y;
    int invert;
    
    for (y = 0; y < size; y++) {
        for (x = 0; x < size; x++) {
            if (bb_getBit(isFunction, x, y)) { continue; }
            
            invert = 0;
            switch (mask) {
                case 0:  invert = (x + y) % 2 == 0;                    break;
                case 1:  invert = y % 2 == 0;                          break;
                case 2:  invert = x % 3 == 0;                          break;
                case 3:  invert = (x + y) % 3 == 0;                    break;
                case 4:  invert = (x / 3 + y / 2) % 2 == 0;            break;
                case 5:  invert = x * y % 2 + x * y % 3 == 0;          break;
                case 6:  invert = (x * y % 2 + x * y % 3) % 2 == 0;    break;
                case 7:  invert = ((x + y) % 2 + x * y % 3) % 2 == 0;  break;
            }
            bb_invertBit(modules, x, y, qrcode_bool_eval(invert));
        }
    }
}

static void setFunctionModule(BitBucket *modules, BitBucket *isFunction, qrcode_uint8_t x, qrcode_uint8_t y, qrcode_bool_t on) {
    bb_setBit(modules, x, y, on);
    bb_setBit(isFunction, x, y, qrcode_true);
}

// Draws a 9*9 finder pattern including the border separator, with the center module at (x, y).
static void drawFinderPattern(BitBucket *modules, BitBucket *isFunction, qrcode_uint8_t x, qrcode_uint8_t y) {
    const qrcode_uint8_t size = (qrcode_uint8_t)modules->bitOffsetOrWidth;
    qrcode_int8_t i, j;

    for (i = -4; i <= 4; i++) {
        for (j = -4; j <= 4; j++) {
            const int dist = max(abs(i), abs(j));  // Chebyshev/infinity norm
            const qrcode_int16_t xx = (qrcode_int16_t)x + j;
            const qrcode_int16_t yy = (qrcode_int16_t)y + i;
            if (0 <= xx && xx < size && 0 <= yy && yy < size) {
                setFunctionModule(modules, isFunction, (qrcode_uint8_t)xx, (qrcode_uint8_t)yy, 
                    qrcode_bool_eval( (dist != 2) && (dist != 4) ));
            }
        }
    }
}

// Draws a 5*5 alignment pattern, with the center module at (x, y).
static void drawAlignmentPattern(BitBucket *modules, BitBucket *isFunction, qrcode_uint8_t x, qrcode_uint8_t y) {
    qrcode_int8_t i, j;
    for (i = -2; i <= 2; i++) {
        for (j = -2; j <= 2; j++) {
            setFunctionModule(modules, isFunction, x + j, y + i, qrcode_bool_eval( max(abs(i), abs(j)) != 1 ));
        }
    }
}

// Draws two copies of the format bits (with its own error correction code)
// based on the given mask and this object's error correction level field.
static void drawFormatBits(BitBucket *modules, BitBucket *isFunction, qrcode_uint8_t ecc, qrcode_uint8_t mask) {
    
    const qrcode_uint8_t size = (qrcode_uint8_t)modules->bitOffsetOrWidth;

    // Calculate error correction code and pack bits
    qrcode_uint32_t data = ecc << 3 | mask;  // errCorrLvl is uint2, mask is uint3
    qrcode_uint32_t rem = data;
    qrcode_uint8_t u;
    qrcode_int8_t i;

    for (u = 0; u < 10; u++) {
        rem = (rem << 1) ^ ((rem >> 9) * 0x537);
    }
    
    data = data << 10 | rem;
    data ^= 0x5412;  // uint15
    
    // Draw first copy
    for (u = 0; u <= 5; u++) {
        setFunctionModule(modules, isFunction, 8, u, qrcode_bool_eval(((data >> u) & 1) != 0));
    }
    
    setFunctionModule(modules, isFunction, 8, 7, ((data >> 6) & 1) != 0);
    setFunctionModule(modules, isFunction, 8, 8, ((data >> 7) & 1) != 0);
    setFunctionModule(modules, isFunction, 7, 8, ((data >> 8) & 1) != 0);
    
    for (i = 9; i < 15; i++) {
        setFunctionModule(modules, isFunction, 14 - i, 8, qrcode_bool_eval((data >> i) & 1));
    }
    
    // Draw second copy
    for (i = 0; i <= 7; i++) {
        setFunctionModule(modules, isFunction, size - 1 - i, 8, qrcode_bool_eval((data >> i) & 1));
    }
    
    for (i = 8; i < 15; i++) {
        setFunctionModule(modules, isFunction, 8, size - 15 + i, qrcode_bool_eval((data >> i) & 1));
    }
    
    setFunctionModule(modules, isFunction, 8, size - 8, qrcode_true);
}


// Draws two copies of the version bits (with its own error correction code),
// based on this object's version field (which only has an effect for 7 <= version <= 40).
static void drawVersion(BitBucket *modules, BitBucket *isFunction, qrcode_uint8_t version) {
    
#if QRCODE_LOCK_VERSION != 0 && QRCODE_LOCK_VERSION < 7
    return;
    
#else
    const qrcode_uint8_t size = (qrcode_uint8_t)modules->bitOffsetOrWidth;
    qrcode_uint8_t i;
    qrcode_uint32_t rem;
    qrcode_uint32_t data;
    
    if (version < 7) { return; }
    
    // Calculate error correction code and pack bits
    rem = version;  // version is uint6, in the range [7, 40]
    for (i = 0; i < 12; i++) {
        rem = (rem << 1) ^ ((rem >> 11) * 0x1F25);
    }
    
    data = version << 12 | rem;  // uint18
    
    // Draw two copies
    for (i = 0; i < 18; i++) {
        const qrcode_bool_t bit = qrcode_bool_eval((data >> i) & 1);
        const qrcode_uint8_t a = size - 11 + i % 3, b = i / 3;
        setFunctionModule(modules, isFunction, a, b, bit);
        setFunctionModule(modules, isFunction, b, a, bit);
    }
    
#endif
}

static void drawFunctionPatterns(BitBucket *modules, BitBucket *isFunction, qrcode_uint8_t version, qrcode_uint8_t ecc) {
    
    const qrcode_uint8_t size = (qrcode_uint8_t)modules->bitOffsetOrWidth;
    qrcode_uint8_t i;

    // Draw the horizontal and vertical timing patterns
    for (i = 0; i < size; i++) {
        setFunctionModule(modules, isFunction, 6, i, qrcode_bool_eval(i % 2 == 0));
        setFunctionModule(modules, isFunction, i, 6, qrcode_bool_eval(i % 2 == 0));
    }
    
    // Draw 3 finder patterns (all corners except bottom right; overwrites some timing modules)
    drawFinderPattern(modules, isFunction, 3, 3);
    drawFinderPattern(modules, isFunction, size - 4, 3);
    drawFinderPattern(modules, isFunction, 3, size - 4);
    
#if QRCODE_LOCK_VERSION == 0 || QRCODE_LOCK_VERSION > 1

    if (version > 1) {

        // Draw the numerous alignment patterns
        #define qrcode_max_alignCount   (40 / 7 + 2)
        const qrcode_uint8_t alignCount = version / 7 + 2;
        const qrcode_uint8_t size = version * 4 + 17;
        qrcode_uint8_t alignPositionIndex = alignCount - 1;
        qrcode_uint8_t alignPosition[qrcode_max_alignCount];	/* real size: alignCount */
        qrcode_uint8_t step;
        qrcode_uint8_t i, j, pos;

        if (version != 32) {
            step = (version * 4 + alignCount * 2 + 1) / (2 * alignCount - 2) * 2;  // ceil((size - 13) / (2*numAlign - 2)) * 2
        } else { // C-C-C-Combo breaker!
            step = 26;
        }
        
        alignPosition[0] = 6;
        
        for (i = 0, pos = size - 7; i < alignCount - 1; i++, pos -= step) {
            alignPosition[alignPositionIndex--] = pos;
        }
        
        for (i = 0; i < alignCount; i++) {
            for (j = 0; j < alignCount; j++) {
                if ((i == 0 && j == 0) || (i == 0 && j == alignCount - 1) || (i == alignCount - 1 && j == 0)) {
                    continue;  // Skip the three finder corners
                } else {
                    drawAlignmentPattern(modules, isFunction, alignPosition[i], alignPosition[j]);
                }
            }
        }
    }
    
#endif
    
    // Draw configuration data
    drawFormatBits(modules, isFunction, ecc, 0);  // Dummy mask value; overwritten later in the constructor
    drawVersion(modules, isFunction, version);
}


// Draws the given sequence of 8-bit codewords (data and error correction) onto the entire
// data area of this QR Code symbol. Function modules need to be marked off before this is called.
static void drawCodewords(BitBucket *modules, BitBucket *isFunction, BitBucket *codewords) {
    
    const qrcode_uint32_t bitLength = codewords->bitOffsetOrWidth;
    const qrcode_uint8_t size = (qrcode_uint8_t)modules->bitOffsetOrWidth;
    qrcode_uint8_t *data = codewords->data;
        
    // Bit index into the data
    qrcode_uint32_t i = 0;
    qrcode_int16_t right;
    int j;
    qrcode_uint8_t vert;
    
    // Do the funny zigzag scan
    for (right = size - 1; right >= 1; right -= 2) {  // Index of right column in each column pair
        if (right == 6) { right = 5; }
        
        for (vert = 0; vert < size; vert++) {  // Vertical counter
            for (j = 0; j < 2; j++) {
                const qrcode_uint8_t x = right - j;  // Actual x coordinate
                const qrcode_bool_t upwards = qrcode_bool_eval( ((right & 2) == 0) ^ (x < 6) );
                const qrcode_uint8_t y = upwards ? size - 1 - vert : vert;  // Actual y coordinate
                if (!bb_getBit(isFunction, x, y) && i < bitLength) {
                    bb_setBit(modules, x, y, ((data[i >> 3] >> (7 - (i & 7))) & 1) != 0);
                    i++;
                }
                // If there are any remainder bits (0 to 7), they are already
                // set to 0/false/white when the grid of modules was initialized
            }
        }
    }
}



/*** Penalty Calculation ***/

#define PENALTY_N1      3
#define PENALTY_N2      3
#define PENALTY_N3     40
#define PENALTY_N4     10

// Calculates and returns the penalty score based on state of this QR Code's current modules.
// This is used by the automatic mask choice algorithm to find the mask pattern that yields the lowest score.
// @TODO: This can be optimized by working with the bytes instead of bits.
static qrcode_uint32_t getPenaltyScore(BitBucket *modules) {
    const qrcode_uint8_t size = (qrcode_uint8_t)modules->bitOffsetOrWidth;
    const qrcode_uint16_t total = size * size;
    qrcode_uint32_t result = 0;
    qrcode_uint8_t x, y, runX, runY;
    qrcode_bool_t cx, cy, colorX, colorY;
    qrcode_uint16_t black;
    qrcode_uint16_t bitsRow, bitsCol;
    qrcode_uint16_t k;
    
    // Adjacent modules in row having same color
    for (y = 0; y < size; y++) {
        colorX = bb_getBit(modules, 0, y);
        for (x = 1, runX = 1; x < size; x++) {
            cx = bb_getBit(modules, x, y);
            if (cx != colorX) {
                colorX = cx;
                runX = 1;
                
            } else {
                runX++;
                if (runX == 5) {
                    result += PENALTY_N1;
                } else if (runX > 5) {
                    result++;
                }
            }
        }
    }
    
    // Adjacent modules in column having same color
    for (x = 0; x < size; x++) {
        colorY = bb_getBit(modules, x, 0);
        for (y = 1, runY = 1; y < size; y++) {
            cy = bb_getBit(modules, x, y);
            if (cy != colorY) {
                colorY = cy;
                runY = 1;
            } else {
                runY++;
                if (runY == 5) {
                    result += PENALTY_N1;
                } else if (runY > 5) {
                    result++;
                }
            }
        }
    }
    
    black = 0;
    for (y = 0; y < size; y++) {
        bitsRow = 0; bitsCol = 0;
        for (x = 0; x < size; x++) {
            const qrcode_bool_t color = bb_getBit(modules, x, y);

            // 2*2 blocks of modules having same color
            if (x > 0 && y > 0) {
                const qrcode_bool_t colorUL = bb_getBit(modules, x - 1, y - 1);
                const qrcode_bool_t colorUR = bb_getBit(modules, x, y - 1);
                const qrcode_bool_t colorL = bb_getBit(modules, x - 1, y);
                if (color == colorUL && color == colorUR && color == colorL) {
                    result += PENALTY_N2;
                }
            }

            // Finder-like pattern in rows and columns
            bitsRow = ((bitsRow << 1) & 0x7FF) | color;
            bitsCol = ((bitsCol << 1) & 0x7FF) | bb_getBit(modules, y, x);

            // Needs 11 bits accumulated
            if (x >= 10) {
                if (bitsRow == 0x05D || bitsRow == 0x5D0) {
                    result += PENALTY_N3;
                }
                if (bitsCol == 0x05D || bitsCol == 0x5D0) {
                    result += PENALTY_N3;
                }
            }

            // Balance of black and white modules
            if (color) { black++; }
        }
    }

    // Find smallest k such that (45-5k)% <= dark/total <= (55+5k)%
    for (k = 0; black * 20 < (9 - k) * total || black * 20 > (11 + k) * total; k++) {
        result += PENALTY_N4;
    }
    
    return result;
}


/*** Reed-Solomon Generator ***/

static qrcode_uint8_t rs_multiply(qrcode_uint8_t x, qrcode_uint8_t y) {
    // Russian peasant multiplication
    // See: https://en.wikipedia.org/wiki/Ancient_Egyptian_multiplication
    qrcode_uint16_t z = 0;
    qrcode_int8_t i;
    for (i = 7; i >= 0; i--) {
        z = (z << 1) ^ ((z >> 7) * 0x11D);
        z ^= ((y >> i) & 1) * x;
    }
    return (qrcode_uint8_t)z;
}

static void rs_init(qrcode_uint8_t degree, qrcode_uint8_t *coeff) {
    qrcode_uint16_t root;
    qrcode_uint8_t i, j;
    memset(coeff, 0, degree);
    coeff[degree - 1] = 1;
    
    // Compute the product polynomial (x - r^0) * (x - r^1) * (x - r^2) * ... * (x - r^{degree-1}),
    // drop the highest term, and store the rest of the coefficients in order of descending powers.
    // Note that r = 0x02, which is a generator element of this field GF(2^8/0x11D).
    root = 1;
    for (i = 0; i < degree; i++) {
        // Multiply the current product by (x - r^i)
        for (j = 0; j < degree; j++) {
            coeff[j] = rs_multiply(coeff[j], (qrcode_uint8_t)root);
            if (j + 1 < degree) {
                coeff[j] ^= coeff[j + 1];
            }
        }
        root = (root << 1) ^ ((root >> 7) * 0x11D);  // Multiply by 0x02 mod GF(2^8/0x11D)
    }
}

static void rs_getRemainder(qrcode_uint8_t degree, qrcode_uint8_t *coeff, qrcode_uint8_t *data, qrcode_uint8_t length, qrcode_uint8_t *result, qrcode_uint8_t stride) {
    // Compute the remainder by performing polynomial division
    qrcode_uint8_t i, j;
    
    for (i = 0; i < length; i++) {
        const qrcode_uint8_t factor = data[i] ^ result[0];
        for (j = 1; j < degree; j++) {
            result[(j - 1) * stride] = result[j * stride];
        }
        result[(degree - 1) * stride] = 0;
        
        for (j = 0; j < degree; j++) {
            result[j * stride] ^= rs_multiply(coeff[j], factor);
        }
    }
}



/*** QrCode ***/

static qrcode_int8_t encodeDataCodewords(BitBucket *dataCodewords, const qrcode_uint8_t *text, qrcode_uint16_t length, qrcode_uint8_t version) {
    qrcode_int8_t mode = QRCODE_MODE_BYTE;
    qrcode_uint16_t accumData = 0;
    qrcode_uint8_t accumCount = 0;
    qrcode_uint16_t i = 0;
    
    if (isNumeric((char const*)text, length)) {
        mode = QRCODE_MODE_NUMERIC;
        bb_appendBits(dataCodewords, 1 << QRCODE_MODE_NUMERIC, 4);
        bb_appendBits(dataCodewords, length, getModeBits(version, QRCODE_MODE_NUMERIC));

        for (i = 0; i < length; i++) {
            accumData = accumData * 10 + ((char)(text[i]) - '0');
            accumCount++;
            if (accumCount == 3) {
                bb_appendBits(dataCodewords, accumData, 10);
                accumData = 0;
                accumCount = 0;
            }
        }
        
        // 1 or 2 digits remaining
        if (accumCount > 0) {
            bb_appendBits(dataCodewords, accumData, accumCount * 3 + 1);
        }
        
    } else if (isAlphanumeric((char*)text, length)) {
        mode = QRCODE_MODE_ALPHANUMERIC;
        bb_appendBits(dataCodewords, 1 << QRCODE_MODE_ALPHANUMERIC, 4);
        bb_appendBits(dataCodewords, length, getModeBits(version, QRCODE_MODE_ALPHANUMERIC));

        for (i = 0; i < length; i++) {
            accumData = accumData * 45 + getAlphanumeric((char)(text[i]));
            accumCount++;
            if (accumCount == 2) {
                bb_appendBits(dataCodewords, accumData, 11);
                accumData = 0;
                accumCount = 0;
            }
        }
        
        // 1 character remaining
        if (accumCount > 0) {
            bb_appendBits(dataCodewords, accumData, 6);
        }
        
    } else {
        bb_appendBits(dataCodewords, 1 << QRCODE_MODE_BYTE, 4);
        bb_appendBits(dataCodewords, length, getModeBits(version, QRCODE_MODE_BYTE));
        for (i = 0; i < length; i++) {
            bb_appendBits(dataCodewords, (char)(text[i]), 8);
        }
    }
    
    //bb_setBits(dataCodewords, length, 4, getModeBits(version, mode));
    
    return mode;
}

static void performErrorCorrection(qrcode_uint8_t version, qrcode_uint8_t ecc, BitBucket *data) {
    
    // See: http://www.thonky.com/qr-code-tutorial/structure-final-message
    
#if QRCODE_LOCK_VERSION == 0
    qrcode_uint8_t numBlocks = NUM_ERROR_CORRECTION_BLOCKS[version - 1][ecc];
    qrcode_uint16_t totalEcc = NUM_ERROR_CORRECTION_CODEWORDS[version - 1][ecc];
    qrcode_uint16_t moduleCount = NUM_RAW_DATA_MODULES[version - 1];
#else
    qrcode_uint8_t numBlocks = NUM_ERROR_CORRECTION_BLOCKS[ecc];
    qrcode_uint16_t totalEcc = NUM_ERROR_CORRECTION_CODEWORDS[ecc];
    qrcode_uint16_t moduleCount = NUM_RAW_DATA_MODULES;
#endif
    
    qrcode_uint8_t blockEccLen = totalEcc / numBlocks;
    qrcode_uint8_t numShortBlocks = numBlocks - moduleCount / 8 % numBlocks;
    qrcode_uint8_t shortBlockLen = moduleCount / 8 / numBlocks;
    
    qrcode_uint8_t shortDataBlockLen = shortBlockLen - blockEccLen;
    
#ifdef HAVE_VLA_SUPPORT
    qrcode_uint8_t result[data->capacityBytes];
#else
    static qrcode_uint8_t result[QRCODE_MAX_BUFFER_CAPACITY];
#endif
#ifdef HAVE_VLA_SUPPORT
    qrcode_uint8_t coeff[blockEccLen];
#else
    static qrcode_uint8_t coeff[256];
#endif
    qrcode_uint16_t offset = 0;
    qrcode_uint8_t *dataBytes = data->data;

    qrcode_uint8_t i, blockNum;
    qrcode_uint16_t index;
    qrcode_uint8_t stride;
    qrcode_uint8_t blockSize;

    memset(result, 0, sizeof(result));
    
    rs_init(blockEccLen, coeff);    
    
    // Interleave all short blocks
    for (i = 0; i < shortDataBlockLen; i++) {
        index = i;
        stride = shortDataBlockLen;
        for (blockNum = 0; blockNum < numBlocks; blockNum++) {
            result[offset++] = dataBytes[index];
            
#if QRCODE_LOCK_VERSION == 0 || QRCODE_LOCK_VERSION >= 5
            if (blockNum == numShortBlocks) { stride++; }
#endif
            index += stride;
        }
    }
    
    // Version less than 5 only have short blocks
#if QRCODE_LOCK_VERSION == 0 || QRCODE_LOCK_VERSION >= 5
    {
        // Interleave long blocks
        index = shortDataBlockLen * (numShortBlocks + 1);
        stride = shortDataBlockLen;
        for (blockNum = 0; blockNum < numBlocks - numShortBlocks; blockNum++) {
            result[offset++] = dataBytes[index];
            
            if (blockNum == 0) { stride++; }
            index += stride;
        }
    }
#endif
    
    // Add all ecc blocks, interleaved
    blockSize = shortDataBlockLen;
    for (blockNum = 0; blockNum < numBlocks; blockNum++) {
        
#if QRCODE_LOCK_VERSION == 0 || QRCODE_LOCK_VERSION >= 5
        if (blockNum == numShortBlocks) { blockSize++; }
#endif
        rs_getRemainder(blockEccLen, coeff, dataBytes, blockSize, &result[offset + blockNum], numBlocks);
        dataBytes += blockSize;
    }
    
    memcpy(data->data, result, data->capacityBytes);
    data->bitOffsetOrWidth = moduleCount;
}

// We store the Format bits tightly packed into a single byte (each of the 4 modes is 2 bits)
// The format bits can be determined by ECC_FORMAT_BITS >> (2 * ecc)
static const qrcode_uint8_t ECC_FORMAT_BITS = (0x02 << 6) | (0x03 << 4) | (0x00 << 2) | (0x01 << 0);


/*** Public QRCode functions ***/

qrcode_uint16_t qrcode_getBufferSize(qrcode_uint8_t version) {
    return bb_getGridSizeBytes(4 * version + 17);
}

// @TODO: Return error if data is too big.
qrcode_bool_t qrcode_initBytes(QRCode *qrcode, qrcode_uint8_t *modules, qrcode_uint8_t version_param, 
                               qrcode_uint8_t ecc, qrcode_uint8_t const* data, qrcode_uint16_t length) {

    const qrcode_uint8_t eccFormatBits = (ECC_FORMAT_BITS >> (2 * ecc)) & 0x03;
#if QRCODE_LOCK_VERSION == 0
    const qrcode_uint8_t version = version_param;
    const qrcode_uint16_t moduleCount = NUM_RAW_DATA_MODULES[version - 1];
    const qrcode_uint16_t dataCapacity = moduleCount / 8 - NUM_ERROR_CORRECTION_CODEWORDS[version - 1][eccFormatBits];
#else
    const qrcode_uint8_t version = QRCODE_LOCK_VERSION;
    const qrcode_uint16_t moduleCount = NUM_RAW_DATA_MODULES;
    const qrcode_uint16_t dataCapacity = moduleCount / 8 - NUM_ERROR_CORRECTION_CODEWORDS[eccFormatBits];
#endif
    const qrcode_uint8_t size = version * 4 + 17;
#ifdef HAVE_VLA_SUPPORT
    qrcode_uint8_t codewordBytes[bb_getBufferSizeBytes(moduleCount)];
#else
    static qrcode_uint8_t codewordBytes[QRCODE_MAX_BUFFER_CAPACITY];
#endif
    qrcode_int8_t mode;
    qrcode_uint8_t padByte;
    struct BitBucket codewords;
    qrcode_uint32_t padding;
    BitBucket modulesGrid;
    BitBucket isFunctionGrid;
#ifdef HAVE_VLA_SUPPORT
    qrcode_uint8_t isFunctionGridBytes[bb_getGridSizeBytes(size)];
#else
    static qrcode_uint8_t isFunctionGridBytes[QRCODE_MAX_BUFFER_CAPACITY];
#endif
    qrcode_uint8_t i;
    qrcode_uint8_t mask = 0;
    qrcode_int32_t minPenalty = INT32_MAX;
    int penalty;

    qrcode->version = version;
    qrcode->size = size;
    qrcode->ecc = ecc;
    qrcode->modules = modules;
    
    bb_initBuffer(&codewords, codewordBytes, (qrcode_uint16_t)sizeof(codewordBytes));
    
    // Place the data code words into the buffer
    mode = encodeDataCodewords(&codewords, data, length, version);
    
    if (mode < 0) { return qrcode_false; }
    qrcode->mode = mode;
    
    // Add terminator and pad up to a byte if applicable
    padding = (dataCapacity * 8) - codewords.bitOffsetOrWidth;
    if (padding > 4) { padding = 4; }
    bb_appendBits(&codewords, 0, (qrcode_uint8_t)padding);
    bb_appendBits(&codewords, 0, (8 - codewords.bitOffsetOrWidth % 8) % 8);

    // Pad with alternate bytes until data capacity is reached
    for (padByte = 0xEC; codewords.bitOffsetOrWidth < (dataCapacity * 8U); padByte ^= 0xEC ^ 0x11) {
        bb_appendBits(&codewords, padByte, 8);
    }

    bb_initGrid(&modulesGrid, modules, size);
    
    bb_initGrid(&isFunctionGrid, isFunctionGridBytes, size);
    
    // Draw function patterns, draw all codewords, do masking
    drawFunctionPatterns(&modulesGrid, &isFunctionGrid, version, eccFormatBits);
    performErrorCorrection(version, eccFormatBits, &codewords);
    drawCodewords(&modulesGrid, &isFunctionGrid, &codewords);
    
    // Find the best (lowest penalty) mask
    for (i = 0; i < 8; i++) {
        drawFormatBits(&modulesGrid, &isFunctionGrid, eccFormatBits, i);
        applyMask(&modulesGrid, &isFunctionGrid, i);
        penalty = getPenaltyScore(&modulesGrid);
        if (penalty < minPenalty) {
            mask = i;
            minPenalty = penalty;
        }
        applyMask(&modulesGrid, &isFunctionGrid, i);  // Undoes the mask due to XOR
    }
    
    qrcode->mask = mask;
    
    // Overwrite old format bits
    drawFormatBits(&modulesGrid, &isFunctionGrid, eccFormatBits, mask);
    
    // Apply the final choice of mask
    applyMask(&modulesGrid, &isFunctionGrid, mask);

    return qrcode_true;
}

qrcode_bool_t qrcode_initText(QRCode *qrcode, qrcode_uint8_t *modules, qrcode_uint8_t version, qrcode_uint8_t ecc, const char *data) {
	const qrcode_uint16_t datalen = (qrcode_uint16_t)strlen(data);
    return qrcode_initBytes(qrcode, modules, version, ecc, (qrcode_uint8_t*)data, datalen);
}

qrcode_bool_t qrcode_getModule(QRCode *qrcode, qrcode_uint8_t x, qrcode_uint8_t y) {
    if (x < 0 || x >= qrcode->size || y < 0 || y >= qrcode->size) {
        return qrcode_false;
    }
    else {
        const qrcode_uint32_t offset = y * qrcode->size + x;
        return qrcode_bool_eval(qrcode->modules[offset >> 3] & (1 << (7 - (offset & 0x07))));
    }
}

