#ifndef RISK_H
#define RISK_H

#ifndef _WIN64
    #error "supported only win64"
#endif

#include <locale.h>
#include <windows.h>

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>

////////////////////////////////////////
// Primitive types and constants

typedef signed char        RISK_i8;
typedef short              RISK_i16;
typedef int                RISK_i32;
typedef long long          RISK_i64;

typedef unsigned char      RISK_u8;
typedef unsigned short     RISK_u16;
typedef unsigned int       RISK_u32;
typedef unsigned long long RISK_u64;

typedef long long          RISK_isz;
typedef unsigned long long RISK_usz;

typedef float              RISK_f32;
typedef double             RISK_f64;

#define RISK_I8_MIN        -128i8
#define RISK_I16_MIN       -32768i16
#define RISK_I32_MIN       -2147483648i32
#define RISK_I64_MIN       -9223372036854775808i64

#define RISK_I8_MAX        127i8
#define RISK_I16_MAX       32767i16
#define RISK_I32_MAX       2147483647i32
#define RISK_I64_MAX       9223372036854775807i64

#define RISK_U8_MAX        0xffui8
#define RISK_U16_MAX       0xffffui16
#define RISK_U32_MAX       0xffffffffui32
#define RISK_U64_MAX       0xffffffffffffffffui64

#define RISK_ISZ_MIN       RISK_I64_MIN
#define RISK_ISZ_MAX       RISK_I64_MAX
#define RISK_USZ_MAX       RISK_U64_MAX

////////////////////////////////////////
// Colors

#define RISK_CLEAN              "\e[0m"
#define RISK_BOLD               "\e[1m"
#define RISK_ITALIC             "\e[3m"
#define RISK_UNDERLINE          "\e[4m"

#define RISK_RED_BOLD           "\e[1;31m"
#define RISK_GREEN_BOLD         "\e[1;32m"
#define RISK_YELLOW_BOLD        "\e[1;33m"
#define RISK_ORANGE_BOLD        "\e[1;34m"
#define RISK_MAGENTA_BOLD       "\e[1;35m"
#define RISK_CYAN_BOLD          "\e[1;36m"
#define RISK_WHITE_BOLD         "\e[1;37m"

#define RISK_RED_BOLD_ITALIC     "\e[1;3;31m"
#define RISK_GREEN_BOLD_ITALIC   "\e[1;3;32m"
#define RISK_YELLOW_BOLD_ITALIC  "\e[1;3;33m"
#define RISK_ORANGE_BOLD_ITALIC  "\e[1;3;34m"
#define RISK_MAGENTA_BOLD_ITALIC "\e[1;3;35m"
#define RISK_CYAN_BOLD_ITALIC    "\e[1;3;36m"
#define RISK_WHITE_BOLD_ITALIC   "\e[1;3;37m"


////////////////////////////////////////
// Caller

typedef struct {
    char const *file;
    RISK_u32 line;
} RISK_Caller;

static void RISK_caller_println(RISK_Caller caller) {
    printf(RISK_CYAN_BOLD " --> %s:%i\n" RISK_CLEAN, caller.file, caller.line);
}

#define RISK_CALLER_HERE ((RISK_Caller){.file = __FILE__, .line = __LINE__})

////////////////////////////////////////
// Assertions

#define RISK_ASSERT(expr, fmt, args...)                               \
    do {                                                              \
        if (expr) break;                                              \
        if (sizeof(fmt) <= 1) RISK_FAILED("assert", #expr);           \
        else RISK_FAILED_ASSERT(#expr, sizeof(#expr) - 1, fmt, args); \
    } while (0)

#define RISK_TODO(fmt, args...)                                                         \
    do {                                                                                \
        if (sizeof(fmt) <= 1) RISK_FAILED("todo", "%s not implemented yet!", __func__); \
        else RISK_FAILED("todo", fmt, args);                                            \
    } while (0)

#define RISK_UNIMPLEMENTED(fmt, args...)                      \
    do {                                                      \
        if (sizeof(fmt) <= 1) RISK_PANIC("not implemented!"); \
        else RISK_FAILED("unimplemented", fmt, args);         \
    } while (0)

#define RISK_PANIC(fmt, args...)                                             \
    do {                                                                     \
        if (sizeof(fmt) <= 1) RISK_FAILED("panic", "something went wrong!"); \
        else RISK_FAILED("panic", fmt, args);                                \
    } while (0)

#define RISK_UNREACHABLE(fmt, args...)                                \
    do {                                                              \
        if (sizeof(fmt) <= 1) RISK_PANIC("reached the unreachable!"); \
        else RISK_FAILED("unreachable", fmt, args);                   \
    } while (0)

#define RISK_FAILED(type, fmt, args...)                  RISK_failed(RISK_CALLER_HERE, type, fmt, args)
#define RISK_FAILED_ASSERT(expr, expr_len, fmt, args...) RISK_failed_assert(RISK_CALLER_HERE, expr, expr_len, fmt, args)

static inline
RISK_u32 RISK_decimal_len(RISK_usz x) {
    RISK_u32 n = 1;
    while (x >= 10) { x /= 10; n += 1; }
    return n;
}

static noreturn
void RISK_failed(
    RISK_Caller caller,
    char const *type,
    char const *fmt, ...
) {
    printf(RISK_RED_BOLD "%s" RISK_WHITE_BOLD ": ", type);

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");
    RISK_caller_println(caller);
    exit(1);
}

static noreturn
void RISK_failed_assert(
    RISK_Caller caller,
    char const *expr,
    RISK_usz len,
    char const *fmt, ...
) {
    RISK_u32 num_len = RISK_decimal_len(caller.line);

    printf(RISK_RED_BOLD "assert" RISK_WHITE_BOLD ": ");

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");

    printf("%*s", num_len - 1, "");
    RISK_caller_println(caller);

    printf(RISK_CYAN_BOLD "%*s |\n", num_len, "");
    printf("%u | " RISK_MAGENTA_BOLD "RISK_ASSERT(%s, ...)\n", caller.line, expr);
    printf(RISK_CYAN_BOLD "%*s |             " RISK_RED_BOLD, num_len, "");
    for (RISK_usz i = 0; i < len; i += 1) putchar('^');
    printf(" must be true\n\n" RISK_CLEAN);
    exit(1);
}

////////////////////////////////////////
// Allocations

#include <malloc.h>
#include <stdalign.h>

#define RISK_KB(n) ((n) * 1024ui64)
#define RISK_MB(n) ((n) * 1024ui64 * 1024ui64)
#define RISK_GB(n) ((n) * 1024ui64 * 1024ui64 * 1024ui64)

#define RISK_PAGE_SIZE    RISK_KB(4)
#define RISK_PAGE_ALIGN   RISK_KB(4)
#define RISK_SIMD_ALIGN   32
#define RISK_MALLOC_ALIGN 16

#if defined RISK_ALLOC && RISK_REALLOC && RISK_DEALLOC
    // ...
#elif defined RISK_ALLOC || RISK_REALLOC || RISK_DEALLOC
    #error "RISK_ALLOC, RISK_REALLOC and RISK_DEALLOC must be defined together"
#else
    #ifdef _WIN32
        #define RISK_ALLOC(len, align)        _aligned_malloc(len, align)
        #define RISK_REALLOC(ptr, len, align) _aligned_realloc(ptr, len, align)
        #define RISK_DEALLOC(ptr)             _aligned_free(ptr)
    #else
        #error "UNSUPPORTED: RISK_ALLOC, RISK_REALLOC and RISK_DEALLOC only for windows by default"
    #endif
#endif

#define RISK_ALLOC_ARRAY(len, T)        RISK_ALLOC(len * sizeof(T), alignof(T))
#define RISK_REALLOC_ARRAY(ptr, len, T) RISK_REALLOC(ptr, len * sizeof(T), alignof(T))

////////////////////////////////////////
// List

#define LIST_ALLOC(p1, len, cap, init)                   \
    do {                                                 \
        (p1) = NULL;                                     \
        (len) = 0;                                       \
        (cap) = (init);                                  \
        if ((cap) == 0) break;                           \
        (p1) = RISK_ALLOC_ARRAY((init), typeof(*p1));    \
        RISK_ASSERT((p1) != NULL, "failed vector init"); \
    } while (0)

#define LIST_DEALLOC(p1)                    \
    do {                                    \
        if ((p1) != NULL) RISK_DEALLOC(p1); \
    } while (0)

#define LIST_RESERVE(p1, len, cap, add)                      \
    do {                                                     \
        if ((cap) >= (len) + (add)) break;                   \
        if ((cap) == 0) (cap) = 3;                           \
        while ((cap) < (len) + (add)) (cap) *= 2;            \
        (p1) = RISK_REALLOC_ARRAY((p1), (cap), typeof(*p1)); \
        RISK_ASSERT((p1) != NULL, "failed vector resize");   \
    } while (0)

#define LIST_EXTEND(d1, s1, dlen, dcap, slen, ...) \
    do {                                           \
        LIST_RESERVE(d1, dlen, dcap, slen);        \
        for (RISK_usz i = 0; i < (slen); i += 1) { \
            (d1)[(dlen) + i] = (s1)[i];            \
        }                                          \
        (dlen) += slen;                            \
    } while (0)

#define LIST_PUSH(p1, len, cap, val)    \
    do {                                \
        LIST_RESERVE(p1, len, cap, 1);  \
        RISK_ASSERT((len) < (cap), ""); \
        (p1)[(len)] = (val);            \
        (len) += 1;                     \
    } while (0)

#define LIST_IMPL(N, P, T, I, I_MAX, ...)                                   \
    typedef struct {                                                        \
        T    *ptr;                                                          \
        RISK_usz len;                                                       \
        RISK_usz cap;                                                       \
    } N;                                                                    \
                                                                            \
    typedef struct {                                                        \
        I start;                                                            \
        I len;                                                              \
    } N##Indexed;                                                           \
                                                                            \
    typedef struct {                                                        \
        T const *ptr;                                                       \
        RISK_usz        len;                                                \
    } N##Slice;                                                             \
    static N P##_alloc(RISK_usz cap) {                                      \
        N buf = {0};                                                        \
        LIST_ALLOC(buf.ptr, buf.len, buf.cap, cap);                         \
        return buf;                                                         \
    }                                                                       \
                                                                            \
    static inline void P##_dealloc(N buf) { LIST_DEALLOC(buf.ptr); }        \
                                                                            \
    static void P##_reserve(N *buf, RISK_usz add) {                         \
        LIST_RESERVE(buf->ptr, buf->len, buf->cap, add);                    \
    }                                                                       \
                                                                            \
    static N##Slice P##_extend(N *buf, N##Slice slice) {                    \
        RISK_usz start = buf->len;                                          \
        LIST_EXTEND(buf->ptr, slice.ptr, buf->len, buf->cap, slice.len, );  \
        return (N##Slice){.ptr = &buf->ptr[start], .len = slice.len};       \
    }                                                                       \
                                                                            \
    static N##Indexed P##_extend_id(N *buf, N##Slice slice) {               \
        I start = buf->len;                                                 \
        P##_extend(buf, slice);                                             \
        return (N##Indexed){.start = start, .len = slice.len};              \
    }                                                                       \
                                                                            \
    static N##Slice P##_slice(N const *buf, RISK_usz start, RISK_usz len) { \
        RISK_ASSERT(start + len <= buf->len, "");                           \
        return (N##Slice){.ptr = &buf->ptr[start], .len = len};             \
    }                                                                       \
                                                                            \
    static void P##_push(N *buf, T v) {                                     \
        LIST_PUSH(buf->ptr, buf->len, buf->cap, v);                         \
    }                                                                       \
                                                                            \
    static I P##_push_id(N *buf, T v) {                                     \
        RISK_ASSERT(buf->len < I_MAX, #N " overflow");                      \
        I id = (I)buf->len;                                                 \
        LIST_PUSH(buf->ptr, buf->len, buf->cap, v);                         \
        return id;                                                          \
    }                                                                       \
                                                                            \
    static T P##_pop(N *buf) {                                              \
        RISK_ASSERT(buf->len != 0, #N " is empty");                         \
        buf->len -= 1;                                                      \
        return buf->ptr[buf->len];                                          \
    }                                                                       \
                                                                            \
    static void P##_pop_n(N *buf, RISK_usz n) {                             \
        RISK_ASSERT(buf->len >= n, #N " not have %llu items", n);           \
        buf->len -= n;                                                      \
    }                                                                       \
                                                                            \
    static inline T P##_at(N const *buf, RISK_usz index) {                  \
        RISK_ASSERT(index < buf->len, "index out of bounds");               \
        return buf->ptr[index];                                             \
    }                                                                       \
                                                                            \
    static inline T *P##_at_mut(N *buf, RISK_usz index) {                   \
        RISK_ASSERT(index < buf->len, "index `%llu` out of bounds", index); \
        return &buf->ptr[index];                                            \
    }                                                                       \
                                                                            \
    static inline I P##_id(N const *buf) {                                  \
        RISK_ASSERT(buf->len < I_MAX, #N " overflow");                      \
        return (I)buf->len;                                                 \
    }


LIST_IMPL(RISK_String, RISK_String, char, RISK_usz, RISK_USZ_MAX)

static void RISK_String_vprintf(RISK_String *string, char const *fmt, va_list args) {
    int len = vsnprintf(NULL, 0, fmt, args);
    RISK_String_reserve(string, len + 1);
    vsnprintf(&string->ptr[string->len], len + 1, fmt, args);
    string->len += len;
}

static void RISK_String_printf(RISK_String *string, char const *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    RISK_String_vprintf(string, fmt, args);
    va_end(args);
}

static void
RISK_String_vprintf_repeat(RISK_String *string, RISK_u32 n, char const *fmt, va_list args) {
    if (n == 0) return;

    int fmt_len = vsnprintf(NULL, 0, fmt, args);
    RISK_ASSERT(fmt_len >= 0, "");
    RISK_String_reserve(string, fmt_len * n + 1);

    char *start = &string->ptr[string->len];
    char *end = &string->ptr[string->len + fmt_len];
    RISK_usz    len = end - start;

    vsnprintf(start, fmt_len + 1, fmt, args);
    for (RISK_usz i = 1; i < n; i += 1) {
        memcpy(&start[len * i], start, len);
    }

    string->len += fmt_len * n;
}

static void RISK_String_printf_repeat(RISK_String *string, RISK_u32 n, char const *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    RISK_String_vprintf_repeat(string, n, fmt, args);
    va_end(args);
}

static inline void RISK_String_flush(RISK_String *string, FILE *stream) {
    if (string->len == 0) return;
    fprintf(stream, "%.*s", (RISK_u32)string->len, string->ptr);
    string->len = 0;
}

#define RISK_string_slice(p, l) ((RISK_StringSlice){.ptr = p, .len = l})

// BUILD: clang -Wall -Wextra -Wno-unused-function risk.c -o risk.exe

RISK_i32 main(void) {
    RISK_String string = RISK_String_alloc(RISK_PAGE_SIZE);
    RISK_String_printf(&string, RISK_RED_BOLD "RISK" RISK_WHITE_BOLD " is " RISK_GREEN_BOLD_ITALIC "self-known" RISK_CLEAN "\n");
    
    RISK_String_flush(&string, stdout);
    RISK_String_dealloc(string);
}

#endif // RISK_H

