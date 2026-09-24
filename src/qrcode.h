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


#ifndef QRCODE_H_INCLUDED
#define QRCODE_H_INCLUDED

#if defined(QRCODE_BUILD_SHARED_LIBS)
#   if defined(qrcodegen_EXPORTS)
#       if defined(_WIN32)
#           define QRCODE_PUBLIC_API __declspec(dllexport)
#       else
#           define QRCODE_PUBLIC_API __attribute__((visibility ("default")))
#       endif
#   else
#       if defined(_WIN32)
#           define QRCODE_PUBLIC_API __declspec(dllimport)
#       else
#           define QRCODE_PUBLIC_API extern
#       endif
#   endif
#endif

#ifndef QRCODE_PUBLIC_API
#   define QRCODE_PUBLIC_API extern
#endif


#if defined(ARDUINO)
#   include <arduino.h>
#endif


#ifdef HAVE_STDINT_H
#   include <stdint.h>
    typedef int8_t      qrcode_int8_t;
    typedef uint8_t     qrcode_uint8_t;
    typedef int16_t     qrcode_int16_t;
    typedef uint16_t    qrcode_uint16_t;
    typedef int32_t     qrcode_int32_t;
    typedef uint32_t    qrcode_uint32_t;
#else
    typedef signed char     qrcode_int8_t;
    typedef unsigned char   qrcode_uint8_t;
    typedef signed short    qrcode_int16_t;
    typedef unsigned short  qrcode_uint16_t;
    typedef signed long     qrcode_int32_t;
    typedef unsigned long   qrcode_uint32_t;
#endif

#if defined(__cplusplus)
    typedef bool qrcode_bool_t;
    #define qrcode_true    true
    #define qrcode_false   false
#elif defined(HAVE_STDBOOL_H)
#   include <stdbool.h>
    typedef _Bool qrcode_bool_t;
    #define qrcode_true    true
    #define qrcode_false   false
#else
    typedef unsigned char qrcode_bool_t;
    #define qrcode_true    1
    #define qrcode_false   0
#endif

#include <stdint.h>


// Error Correction Code Levels
#define QRCODE_ECC_LOW            0
#define QRCODE_ECC_MEDIUM         1
#define QRCODE_ECC_QUARTILE       2
#define QRCODE_ECC_HIGH           3


// If set to non-zero, this library can ONLY produce QR codes at that version
// This saves a lot of dynamic memory, as the codeword tables are skipped
#ifndef QRCODE_LOCK_VERSION
#define QRCODE_LOCK_VERSION       8
#endif


typedef struct QRCode {
    qrcode_uint8_t version;
    qrcode_uint8_t size;
    qrcode_uint8_t ecc;
    qrcode_uint8_t mode;
    qrcode_uint8_t mask;
    qrcode_uint8_t *modules;
} QRCode;


#ifdef __cplusplus
extern "C"{
#endif  /* __cplusplus */



QRCODE_PUBLIC_API uint16_t qrcode_getBufferSize(qrcode_uint8_t version);

QRCODE_PUBLIC_API qrcode_bool_t qrcode_initText(QRCode *qrcode, qrcode_uint8_t *modules, qrcode_uint8_t version, qrcode_uint8_t ecc, const char *data);
QRCODE_PUBLIC_API qrcode_bool_t qrcode_initBytes(QRCode *qrcode, qrcode_uint8_t *modules, qrcode_uint8_t version, qrcode_uint8_t ecc, qrcode_uint8_t const* data, qrcode_uint16_t length);

QRCODE_PUBLIC_API qrcode_bool_t qrcode_getModule(QRCode *qrcode, qrcode_uint8_t x, qrcode_uint8_t y);



#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif  /* __QRCODE_H_ */
