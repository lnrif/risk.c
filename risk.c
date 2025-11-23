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
// Primitive types

typedef signed char        rk_i8;
typedef short              rk_i16;
typedef int                rk_i32;
typedef long long          rk_i64;

typedef unsigned char      rk_u8;
typedef unsigned short     rk_u16;
typedef unsigned int       rk_u32;
typedef unsigned long long rk_u64;

typedef long long          rk_isz;
typedef unsigned long long rk_usize;

typedef float              rk_f32;
typedef double             rk_f64;

#define RK_I8_MIN  (-128i8)
#define RK_I16_MIN (-32768i16)
#define RK_I32_MIN (-2147483648i32)
#define RK_I64_MIN (-9223372036854775808i64)

#define RK_I8_MAX  127i8
#define RK_I16_MAX 32767i16
#define RK_I32_MAX 2147483647i32
#define RK_I64_MAX 9223372036854775807i64

#define RK_U8_MAX  0xffui8
#define RK_U16_MAX 0xffffui16
#define RK_U32_MAX 0xffffffffui32
#define RK_U64_MAX 0xffffffffffffffffui64

#define RK_ISZ_MIN RK_I64_MIN
#define RK_ISZ_MAX RK_I64_MAX
#define RK_USZ_MAX RK_U64_MAX

////////////////////////////////////////
// Colors

#define RK_CLEAN               "\e[0m"
#define RK_BOLD                "\e[1m"
#define RK_ITALIC              "\e[3m"
#define RK_UNDERLINE           "\e[4m"

#define RK_BLACK               "\e[0;90m"
#define RK_RED                 "\e[0;31m"
#define RK_GREEN               "\e[0;32m"
#define RK_YELLOW              "\e[0;33m"
#define RK_ORANGE              "\e[0;34m"
#define RK_MAGENTA             "\e[0;35m"
#define RK_CYAN                "\e[0;36m"
#define RK_WHITE               "\e[0;37m"

#define RK_BLACK_BOLD          "\e[0;1;90m"
#define RK_RED_BOLD            "\e[0;1;31m"
#define RK_GREEN_BOLD          "\e[0;1;32m"
#define RK_YELLOW_BOLD         "\e[0;1;33m"
#define RK_ORANGE_BOLD         "\e[0;1;34m"
#define RK_MAGENTA_BOLD        "\e[0;1;35m"
#define RK_CYAN_BOLD           "\e[0;1;36m"
#define RK_WHITE_BOLD          "\e[0;1;37m"

#define RK_BLACK_ITALIC        "\e[0;3;90m"
#define RK_RED_ITALIC          "\e[0;3;31m"
#define RK_GREEN_ITALIC        "\e[0;3;32m"
#define RK_YELLOW_ITALIC       "\e[0;3;33m"
#define RK_ORANGE_ITALIC       "\e[0;3;34m"
#define RK_MAGENTA_ITALIC      "\e[0;3;35m"
#define RK_CYAN_ITALIC         "\e[0;3;36m"
#define RK_WHITE_ITALIC        "\e[0;3;37m"

#define RK_BLACK_BOLD_ITALIC   "\e[0;1;3;90m"
#define RK_RED_BOLD_ITALIC     "\e[0;1;3;31m"
#define RK_GREEN_BOLD_ITALIC   "\e[0;1;3;32m"
#define RK_YELLOW_BOLD_ITALIC  "\e[0;1;3;33m"
#define RK_ORANGE_BOLD_ITALIC  "\e[0;1;3;34m"
#define RK_MAGENTA_BOLD_ITALIC "\e[0;1;3;35m"
#define RK_CYAN_BOLD_ITALIC    "\e[0;1;3;36m"
#define RK_WHITE_BOLD_ITALIC   "\e[0;1;3;37m"

////////////////////////////////////////
// Caller

typedef struct {
    char const *file;
    rk_u32 line;
} RK_Caller;

static
void rk_caller_println(RK_Caller caller) {
    printf(RK_CYAN_BOLD " --> %s:%i\n" RK_CLEAN, caller.file, caller.line);
}

#define RK_CALLER_HERE ((RK_Caller){.file = __FILE__, .line = __LINE__})

////////////////////////////////////////
// Assertions

#define RK_ASSERT(expr, fmt, args...)                               \
    do {                                                            \
        if (expr) break;                                            \
        if (sizeof(fmt) <= 1) RK_FAILED("assert", #expr);           \
        else RK_FAILED_ASSERT(#expr, sizeof(#expr) - 1, fmt, args); \
    } while (0)                                                     \

#define RK_TODO(fmt, args...)                                                         \
    do {                                                                              \
        if (sizeof(fmt) <= 1) RK_FAILED("todo", "%s not implemented yet!", __func__); \
        else RK_FAILED("todo", fmt, args);                                            \
    } while (0)                                                                       \

#define RK_UNIMPLEMENTED(fmt, args...)                      \
    do {                                                    \
        if (sizeof(fmt) <= 1) RK_PANIC("not implemented!"); \
        else RK_FAILED("unimplemented", fmt, args);         \
    } while (0)                                             \

#define RK_PANIC(fmt, args...)                                             \
    do {                                                                   \
        if (sizeof(fmt) <= 1) RK_FAILED("panic", "something went wrong!"); \
        else RK_FAILED("panic", fmt, args);                                \
    } while (0)                                                            \

#define RK_UNREACHABLE(fmt, args...)                                \
    do {                                                            \
        if (sizeof(fmt) <= 1) RK_PANIC("reached the unreachable!"); \
        else RK_FAILED("unreachable", fmt, args);                   \
    } while (0)                                                     \

#define RK_FAILED(type, fmt, args...)          \
    rk_failed(RK_CALLER_HERE, type, fmt, args) \

#define RK_FAILED_ASSERT(expr, expr_len, fmt, args...)          \
    rk_failed_assert(RK_CALLER_HERE, expr, expr_len, fmt, args) \

static inline
rk_u32 rk_decimal_len(rk_usize x) {
    rk_u32 n = 1;
    while (x >= 10) { x /= 10; n += 1; }
    return n;
}

static noreturn
void rk_failed(
    RK_Caller caller,
    char const *type,
    char const *fmt, ...
) {
    printf(RK_RED_BOLD "\n%s" RK_WHITE_BOLD ": ", type);

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");
    rk_caller_println(caller);
    printf(RK_CLEAN);
    exit(1);
}


static noreturn
void rk_failed_assert(
    RK_Caller caller,
    char const *expr,
    rk_usize len,
    char const *fmt, ...
) {
    rk_u32 num_len = rk_decimal_len(caller.line);

    printf(RK_RED_BOLD "\nassert" RK_WHITE_BOLD ": ");

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");

    printf("%*s", num_len - 1, "");
    rk_caller_println(caller);

    printf(RK_CYAN_BOLD "%*s |\n", num_len, "");
    printf("%u | " RK_MAGENTA_BOLD "RK_ASSERT(%s, ...)\n", caller.line, expr);
    printf(RK_CYAN_BOLD "%*s |           " RK_RED_BOLD, num_len, "");
    for (rk_usize i = 0; i < len; i += 1) putchar('^');
    printf(" must be true\n");
    printf(RK_CLEAN);
    exit(1);
}

////////////////////////////////////////
// Allocations

#include <stdalign.h>

#define RK_KB(n) ((n) * 1024ui64)
#define RK_MB(n) ((n) * 1024ui64 * 1024ui64)
#define RK_GB(n) ((n) * 1024ui64 * 1024ui64 * 1024ui64)

#define RK_PAGE_SIZE    RK_KB(4)
#define RK_PAGE_ALIGN   RK_KB(4)
#define RK_SIMD_ALIGN   32
#define RK_MALLOC_ALIGN 16

#if defined RK_ALLOC && RK_REALLOC && RK_DEALLOC
    // ...
#elif defined RK_ALLOC || RK_REALLOC || RK_DEALLOC
    #error "RK_ALLOC, RK_REALLOC and RK_DEALLOC must be defined together"
#else
    #ifdef _WIN32
        #define RK_ALLOC(len, align)        _aligned_malloc(len, align)
        #define RK_REALLOC(ptr, len, align) _aligned_realloc(ptr, len, align)
        #define RK_DEALLOC(ptr)             _aligned_free(ptr)
    #else
        #error "UNSUPPORTED: RK_ALLOC, RK_REALLOC and RK_DEALLOC only for windows by default"
    #endif
#endif

#define RK_ALLOC_ARRAY(len, T)        RK_ALLOC(len * sizeof(T), alignof(T))
#define RK_REALLOC_ARRAY(ptr, len, T) RK_REALLOC(ptr, len * sizeof(T), alignof(T))

////////////////////////////////////////
// List

#define RK_LIST_ALLOC(ptr, len, cap, init)                                  \
    do {                                                                    \
        (ptr) = NULL;                                                       \
        (len) = 0;                                                          \
        (cap) = (init);                                                     \
        if ((cap) == 0) break;                                              \
        (ptr) = RK_ALLOC_ARRAY((init), typeof(*ptr));                       \
        RK_ASSERT((ptr) != NULL, "failed vector init");                     \
    } while (0)                                                             \

#define RK_LIST_DEALLOC(ptr)                                                \
    do {                                                                    \
        if ((ptr) != NULL) RK_DEALLOC(ptr);                                 \
    } while (0)                                                             \

#define RK_LIST_RESERVE(ptr, len, cap, add)                                 \
    do {                                                                    \
        if ((cap) >= (len) + (add)) break;                                  \
        if ((cap) == 0) (cap) = 3;                                          \
        while ((cap) < (len) + (add)) (cap) *= 2;                           \
        (ptr) = RK_REALLOC_ARRAY((ptr), (cap), typeof(*ptr));               \
        RK_ASSERT((ptr) != NULL, "failed vector resize");                   \
    } while (0)                                                             \

#define RK_LIST_EXTEND(d1, s1, dlen, dcap, slen, ...)                       \
    do {                                                                    \
        RK_LIST_RESERVE(d1, dlen, dcap, slen);                              \
        for (rk_usize i = 0; i < (slen); i += 1) {                            \
            (d1)[(dlen) + i] = (s1)[i];                                     \
        }                                                                   \
        (dlen) += slen;                                                     \
    } while (0)                                                             \

#define RK_LIST_PUSH(ptr, len, cap, val)                                    \
    do {                                                                    \
        RK_LIST_RESERVE(ptr, len, cap, 1);                                  \
        RK_ASSERT((len) < (cap), "");                                       \
        (ptr)[(len)] = (val);                                               \
        (len) += 1;                                                         \
    } while (0)                                                             \

#define RK_LIST(                                                            \
    NAME, SLICE, INDEXED,                                                   \
    PREFIX, TYPE,                                                           \
    INDEX, INDEX_MAX,                                                       \
    ...                                                                     \
)                                                                           \
    typedef struct {                                                        \
        TYPE     *ptr;                                                      \
        rk_usize len;                                                         \
        rk_usize cap;                                                         \
    } NAME;                                                                 \
                                                                            \
    typedef struct {                                                        \
        INDEX start;                                                        \
        INDEX len;                                                          \
    } INDEXED;                                                              \
                                                                            \
    typedef struct {                                                        \
        TYPE const *ptr;                                                    \
        rk_usize   len;                                                       \
    } SLICE;                                                                \
                                                                            \
    static                                                                  \
    NAME PREFIX##_alloc(rk_usize cap) {                                       \
        NAME buf = {0};                                                     \
        RK_LIST_ALLOC(buf.ptr, buf.len, buf.cap, cap);                      \
        return buf;                                                         \
    }                                                                       \
                                                                            \
    static inline                                                           \
    void PREFIX##_dealloc(NAME * buf) {                                     \
        RK_LIST_DEALLOC(buf->ptr);                                          \
    }                                                                       \
                                                                            \
    static                                                                  \
    void PREFIX##_reserve(NAME *buf, rk_usize add) {                          \
        RK_LIST_RESERVE(buf->ptr, buf->len, buf->cap, add);                 \
    }                                                                       \
                                                                            \
    static                                                                  \
    SLICE PREFIX##_extend(NAME *buf, SLICE slice) {                         \
        rk_usize start = buf->len;                                            \
        RK_LIST_EXTEND(buf->ptr, slice.ptr, buf->len, buf->cap, slice.len); \
        return (SLICE){.ptr = &buf->ptr[start], .len = slice.len};          \
    }                                                                       \
                                                                            \
    static                                                                  \
    INDEXED PREFIX##_extend_indexed(NAME *buf, SLICE slice) {               \
        INDEX start = buf->len;                                             \
        PREFIX##_extend(buf, slice);                                        \
        return (INDEXED){.start = start, .len = slice.len};                 \
    }                                                                       \
                                                                            \
    static                                                                  \
    SLICE PREFIX##_slice(NAME const *buf, rk_usize start, rk_usize len) {       \
        RK_ASSERT(start + len <= buf->len, "");                             \
        return (SLICE){.ptr = &buf->ptr[start], .len = len};                \
    }                                                                       \
                                                                            \
    static                                                                  \
    void PREFIX##_push(NAME *buf, TYPE v) {                                 \
        RK_LIST_PUSH(buf->ptr, buf->len, buf->cap, v);                      \
    }                                                                       \
                                                                            \
    static                                                                  \
    INDEX PREFIX##_push_id(NAME *buf, TYPE v) {                             \
        RK_ASSERT(buf->len < INDEX_MAX, #NAME " overflow");                 \
        INDEX id = (INDEX)buf->len;                                         \
        RK_LIST_PUSH(buf->ptr, buf->len, buf->cap, v);                      \
        return id;                                                          \
    }                                                                       \
                                                                            \
    static                                                                  \
    TYPE PREFIX##_pop(NAME *buf) {                                          \
        RK_ASSERT(buf->len != 0, #NAME " is empty");                        \
        buf->len -= 1;                                                      \
        return buf->ptr[buf->len];                                          \
    }                                                                       \
                                                                            \
    static                                                                  \
    void PREFIX##_pop_n(NAME *buf, rk_usize n) {                              \
        RK_ASSERT(buf->len >= n, #NAME " not have %llu items", n);          \
        buf->len -= n;                                                      \
    }                                                                       \
                                                                            \
    static inline                                                           \
    TYPE PREFIX##_at(NAME const *buf, rk_usize index) {                       \
        RK_ASSERT(index < buf->len, "index out of bounds");                 \
        return buf->ptr[index];                                             \
    }                                                                       \
                                                                            \
    static inline                                                           \
    TYPE * PREFIX##_at_mut(NAME *buf, rk_usize index) {                       \
        RK_ASSERT(index < buf->len, "index `%llu` out of bounds", index);   \
        return &buf->ptr[index];                                            \
    }                                                                       \
                                                                            \
    static inline                                                           \
    INDEX PREFIX##_id(NAME const *buf) {                                    \
        RK_ASSERT(buf->len < INDEX_MAX, #NAME " overflow");                 \
        return (INDEX)buf->len;                                             \
    }                                                                       \

////////////////////////////////////////
// String

RK_LIST(
    RkStrBuf, RkStrRef, RkStrIdx,
    rk_sb, char, rk_usize, RK_USZ_MAX,
)

#define RK_SB_EMPTY (RkStrBuf){.ptr = NULL, .len = 0, .cap = 0}

static inline
bool rk_ch_is_space(rk_u8 c) {
    return c == ' ' || c == '\t'|| c == '\r' || c == '\n';
}

static inline
void rk_sb_strip_right(RkStrBuf *buf) {
    for (;;) {
        if (buf->len == 0) break;
        rk_u8 c = buf->ptr[buf->len - 1];
        if (!rk_ch_is_space(c)) break;
        buf->len -= 1;
    }
}

static
void rk_sb_write_cstr(RkStrBuf *buf, char const * const cstr) {
    RkStrRef slice = {.ptr = cstr, .len = strlen(cstr)};
    rk_sb_extend(buf, slice);
}

static
void rk_sb_vprintf(RkStrBuf *buf, char const *fmt, va_list args) {
    int len = vsnprintf(NULL, 0, fmt, args);
    rk_sb_reserve(buf, len + 1);
    vsnprintf(&buf->ptr[buf->len], len + 1, fmt, args);
    buf->len += len;
}

static
void rk_sb_printf(RkStrBuf *buf, char const *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    rk_sb_vprintf(buf, fmt, args);
    va_end(args);
}

static
void rk_sb_vprintf_repeat(RkStrBuf *buf, rk_u32 n, char const *fmt, va_list args) {
    if (n == 0) return;

    int fmt_len = vsnprintf(NULL, 0, fmt, args);
    RK_ASSERT(fmt_len >= 0, "fmt error");
    rk_sb_reserve(buf, fmt_len * n + 1);

    char *start = &buf->ptr[buf->len];
    char *end = &buf->ptr[buf->len + fmt_len];
    rk_usize len = end - start;

    vsnprintf(start, fmt_len + 1, fmt, args);
    for (rk_usize i = 1; i < n; i += 1) {
        memcpy(&start[len * i], start, len);
    }

    buf->len += fmt_len * n;
}

static
void rk_sb_printf_repeat(RkStrBuf *buf, rk_u32 n, char const *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    rk_sb_vprintf_repeat(buf, n, fmt, args);
    va_end(args);
}

////////////////////////////////////////
// File Path

typedef struct {
    char  *ptr;
    rk_usize len;
    rk_usize cap;
} RkPathBuf;

#define RK_PB_EMPTY (RkPathBuf){.ptr = NULL, .len = 0, .cap = 0}

static inline
RkPathBuf rk_pb_from_cstr(char const *ptr) {
    RkPathBuf path = RK_PB_EMPTY;
    RK_LIST_EXTEND(path.ptr, ptr, path.len, path.cap, strlen(ptr) + 1);
    return path;
}

static
void rk_pb_join(RkPathBuf *path, RkStrRef add) {
    if (add.len == 0) return;

    for (rk_usize i = 0; i <= add.len; i += 1) {
        RK_ASSERT(add.ptr[i] != '\0', "unexpected NULL in slice");
    }

    rk_usize reserve_len = add.len + 1;
    if (path->len > 0) {
        // strip NULL
        path->len -= 1;
        // add slash
        reserve_len += 1;
    }
    RK_LIST_RESERVE(path->ptr, path->len, path->cap, reserve_len);

    if (path->len > 0) RK_LIST_PUSH(path->ptr, path->len, path->cap, '\\');
    RK_LIST_EXTEND(path->ptr, add.ptr, path->len, path->cap, add.len);
    RK_LIST_PUSH(path->ptr, path->len, path->cap, '\0');
}

static inline
char const *rk_pb_tail(char const * const path, rk_usize n) {
    rk_usize len = strlen(path);
    RK_ASSERT(len > 0, "expected non-empty path");
    RK_ASSERT(n > 0, "expected at least 1 component");

    char const * const start = path;
    char const * peek = &path[len + 1];

    for (;;) {
        peek -= 1;
        if (peek <= start) return start;
        if (*peek == '\\' || *peek == '/') n -= 1;
        if (n == 0) return peek + 1;
    }
}

static inline
void rk_pb_dealloc(RkPathBuf * path) {
    RK_LIST_DEALLOC(path->ptr);
}

////////////////////////////////////////
// File

#include <direct.h>
#include <string.h>

typedef enum {
    RK_FILE_LOAD_OK,
    RK_FILE_LOAD_PERMISSION_DENIED,
    RK_FILE_LOAD_EXPECTED_FILE,
    RK_FILE_LOAD_NOT_FOUND,
    RK_FILE_LOAD_UNKNOWN_ERROR,
} RkFileLoadKind;

typedef struct {
    rk_u8 *ptr;
    rk_usize len;
} RkBytes;

static inline
RkStrBuf rk_sb_from_bytes(RkBytes bytes) {
    return (RkStrBuf){
        .ptr = (void*)bytes.ptr,
        .len = bytes.len,
        .cap = bytes.len,
    };
}

typedef struct {
    RkFileLoadKind kind;
    RkBytes bytes;
} RkFileLoadResult;

static
char *rk_file_result_as_cstr(RkFileLoadKind kind) {
    switch (kind) {
        case RK_FILE_LOAD_OK:                return "ok";
        case RK_FILE_LOAD_PERMISSION_DENIED: return "permission denied";
        case RK_FILE_LOAD_EXPECTED_FILE:     return "expected file, but found directory";
        case RK_FILE_LOAD_NOT_FOUND:         return "file not found";
        case RK_FILE_LOAD_UNKNOWN_ERROR:     return "failed read file (unknown reason)";
    }
}

static
RkFileLoadKind file_result_from_errno(errno_t err) {
    switch (err) {
        case 0:      return RK_FILE_LOAD_OK;
        case EPERM:  return RK_FILE_LOAD_PERMISSION_DENIED;
        case ENOENT: return RK_FILE_LOAD_NOT_FOUND;
        case EACCES: return RK_FILE_LOAD_EXPECTED_FILE;
        default:     return RK_FILE_LOAD_UNKNOWN_ERROR;
    }
}

static inline
FILE *rk_open_file_or_fail(char const *path) {
    FILE *file;
    errno_t err = fopen_s(&file, path, "wb");
    RK_ASSERT(err == 0 && file != NULL, "failed open file");
    return file;
}

static inline
void rk_file_save(
    char const * const path,
    rk_u8 const * const buf,
    rk_usize const len
) {
    FILE *file;

    errno_t err = fopen_s(&file, (char *)path, "wb");
    RK_ASSERT(err == 0 && file != NULL, "failed open file");
    
    size_t written = fwrite(buf, 1, len, file);
    RK_ASSERT(written == len, "failed write in file");
    
    RK_ASSERT(fclose(file) == 0, "failed close file");
}

static
RkFileLoadResult rk_file_try_load(char const *path) {
    FILE *file = NULL;
    errno_t err = fopen_s(&file, path, "rb");

    RkFileLoadKind kind = file_result_from_errno(err);
    if (kind != RK_FILE_LOAD_OK) return (RkFileLoadResult){.kind = kind, .bytes = {0}};

    fseek(file, 0, SEEK_END);
    rk_usize file_len = ftell(file);
    RK_ASSERT(file_len != RK_U32_MAX, "failed getting file len");
    fseek(file, 0, SEEK_SET);

    rk_u8 *buf = RK_ALLOC_ARRAY(file_len, rk_u8);
    RK_ASSERT(buf != NULL, "failed allocating buf");

    rk_usize read = fread(buf, 1, file_len, file);
    RK_ASSERT(read == file_len, "incomplete reading file");

    RK_ASSERT(fclose(file) == 0, "failed close file");

    RkBytes bytes = {.ptr = buf, .len = file_len};
    return (RkFileLoadResult){.kind = RK_FILE_LOAD_OK, .bytes = bytes};
}

static inline
RkBytes rk_file_load_or_exit(char const *path) {
    RkFileLoadResult load = rk_file_try_load(path);
    RK_ASSERT(load.kind == RK_FILE_LOAD_OK, "failed load `%s`", path);
    return load.bytes;
}

////////////////////////////////////////
// Location in file

typedef rk_u32 RkPos;
#define RK_POS_MAX RK_U32_MAX

typedef struct {
    RkPos start;
    RkPos len;
} RkSpan;

typedef struct {
    RkPos line;
    RkPos column;
} RkLoc;

#define RK_LOC_ZERO ((RkLoc){.line = 0, .column = 0})

typedef struct {
    RkSpan span;
    RkLoc loc;
} RkLine;

static inline
rk_usize rk_utf8_len(rk_u8 b) {
    if      ((b & 0xFF) < 0x80)  return 1;
    else if ((b & 0xE0) == 0xC0) return 2;
    else if ((b & 0xF0) == 0xE0) return 3;
    else if ((b & 0xF8) == 0xF0) return 4;
    // invalid codepoint: just skip
    else                         return 1;
}

static
RkLine rk_line(
    char const *ptr,
    rk_usize len,
    rk_usize pos,
    rk_u32 tab
) {
    rk_usize start = 0;
    rk_usize line = 1;
    rk_usize column = 1;
    for (rk_usize i = 0; i < pos; i += rk_utf8_len(ptr[i])) {
        rk_u8 b = ptr[i];

        if (b == '\n') {
            start = i + 1;
            line += 1;
            column = 0;
        }

        if (b == '\t') column += tab;
        else           column += 1;
    }

    rk_usize end = start;
    while (end < len && ptr[end] != '\n') end += 1;

    RkSpan span = {.start = start, .len = end - start};
    RkLoc loc = {.line = line, .column = column};
    return (RkLine){.span = span, .loc = loc};
}

static
rk_u32 rk_utf8_chars_count(char const *ptr, rk_usize len) {
    rk_usize i = 0;
    rk_u32 n = 0;
    for (;;) {
        if (i >= len) return n;
        n += 1;
        i += rk_utf8_len(ptr[i]);
    }
}

////////////////////////////////////////
// Diagnostic

typedef struct {
    RkStrBuf buf;
} RkDiag;

#define RK_DIAG_EMPTY ((RkDiag){.buf = RK_SB_EMPTY})

static inline
RkDiag rk_diag_alloc(rk_usize cap) {
    return (RkDiag){.buf = rk_sb_alloc(cap)};
}

static inline void
rk_diag_vprint(RkDiag *diag, char const *fmt, va_list args) {
    rk_sb_vprintf(&diag->buf, fmt, args);
}

static inline void
rk_diag_write_cstr(RkDiag *diag, char const * const cstr) {
    rk_sb_write_cstr(&diag->buf, cstr);
}

static inline void
rk_diag_print(RkDiag *diag, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    rk_diag_vprint(diag, fmt, args);
    va_end(args);
}

static inline void
rk_diag_repeat_print(RkDiag *diag, rk_u32 n, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    rk_sb_vprintf_repeat(&diag->buf, n, fmt, args);
    va_end(args);
}

static inline
void rk_diag_flush(RkDiag * const diag, FILE * const stream) {
    if (diag->buf.len == 0) return;
    size_t written = fwrite(diag->buf.ptr, 1, diag->buf.len, stream);
    RK_ASSERT(written == diag->buf.len, "failed writing to file");
    diag->buf.len = 0;
}

static inline
void rk_diag_dealloc(RkDiag * const diag) {
    rk_sb_dealloc(&diag->buf);
}

static inline
void rk_diag_flush_and_dealloc(RkDiag * const diag, FILE * const stream) {
    rk_diag_flush(diag, stream);
    rk_diag_dealloc(diag);
}

////////////////////////////////////////
// Unit

typedef RkStrBuf RkSrcBuf;

typedef struct {
    RkPathBuf path;
    RkSrcBuf src;
} RkUnit;

static inline
void rk_unit_dealloc(RkUnit * unit) {
    RK_ASSERT(unit->src.ptr != NULL && unit->path.ptr != NULL, "");
    RK_DEALLOC(unit->src.ptr);
    rk_pb_dealloc(&unit->path);
}

typedef enum {
    RK_UNIT_LOAD_OK,
    RK_UNIT_LOAD_TOO_BIG,
    RK_UNIT_LOAD_PERMISSION_DENIED,
    RK_UNIT_LOAD_EXPECTED_FILE,
    RK_UNIT_LOAD_NOT_FOUND,
    RK_UNIT_LOAD_UNKNOWN_ERROR,
} RkUnitLoadKind;

typedef struct {
    RkUnit unit;
    RkUnitLoadKind kind;
} RkUnitLoadResult;

static
RkUnitLoadKind rk_unit_load_kind_from_errno(errno_t err) {
    switch (err) {
        case 0:      return RK_UNIT_LOAD_OK;
        case EPERM:  return RK_UNIT_LOAD_PERMISSION_DENIED;
        case ENOENT: return RK_UNIT_LOAD_NOT_FOUND;
        case EACCES: return RK_UNIT_LOAD_EXPECTED_FILE;
        default:     return RK_UNIT_LOAD_UNKNOWN_ERROR;
    }
}

static
RkUnitLoadResult rk_unit_try_load(char const *path) {
    FILE *file = NULL;
    errno_t err = fopen_s(&file, path, "rb");

    RkUnitLoadKind kind = rk_unit_load_kind_from_errno(err);
    if (kind != RK_UNIT_LOAD_OK) {
        return (RkUnitLoadResult){.unit = {0}, .kind = kind};
    }

    fseek(file, 0, SEEK_END);
    long file_len = ftell(file);
    RK_ASSERT(file_len != -1, "failed getting file len");
    rk_usize len = file_len;
    fseek(file, 0, SEEK_SET);

    if (len > RK_POS_MAX) {
        RK_ASSERT(fclose(file) == 0, "failed close file");
        return (RkUnitLoadResult){.unit = {0}, .kind = RK_UNIT_LOAD_TOO_BIG};
    }

    rk_u8 *buf = RK_ALLOC_ARRAY(len, rk_u8);
    RK_ASSERT(buf != NULL, "failed allocating buf");

    rk_usize read = fread(buf, 1, len, file);
    RK_ASSERT(read == len, "incomplete reading file");

    RK_ASSERT(fclose(file) == 0, "failed close file");

    RkBytes bytes = {.ptr = buf, .len = len};
    return (RkUnitLoadResult){
        .unit = (RkUnit){
            .src = rk_sb_from_bytes(bytes),
            .path = rk_pb_from_cstr(path),
        },
        .kind = RK_UNIT_LOAD_OK,
    };
}

////////////////////////////////////////
// Lexer & Tokens

typedef enum: rk_u8 {
    /// End of file
    RK_TOKEN_EOF,

    /// (`0-9`)(`@0-9a-zA-Z_`)*
    RK_TOKEN_INTEGER,
    /// RK_TOKEN_INTEGER `.` RK_TOKEN_INTEGER
    RK_TOKEN_FLOAT,
    /// (`@a-zA-Z_`)(`@0-9a-zA-Z_`)*
    RK_TOKEN_IDENT,
    /// `"` (^`\n"`)* `"`
    RK_TOKEN_STRING,
    /// `"` (^`\n`)* `\n`
    RK_TOKEN_STRING_UNTERMINATED,

    /// `+`
    RK_TOKEN_PLUS,
    /// `-`
    RK_TOKEN_MINUS,
    /// `*`
    RK_TOKEN_STAR,
    /// `/`
    RK_TOKEN_SLASH,

    /// `+=`
    RK_TOKEN_PLUS_EQ,
    /// `-=`
    RK_TOKEN_MINUS_EQ,
    /// `*=`
    RK_TOKEN_STAR_EQ,
    /// `/=`
    RK_TOKEN_SLASH_EQ,

    /// `=`
    RK_TOKEN_EQ,
    /// `:=`
    RK_TOKEN_COLON_EQ,

    /// `==`
    RK_TOKEN_EQ_EQ,
    /// `!=`
    RK_TOKEN_NOT_EQ,

    /// `<`
    RK_TOKEN_LT,
    /// `>`
    RK_TOKEN_GT,
    /// `<=`
    RK_TOKEN_LE,
    /// `>=`
    RK_TOKEN_GE,

    /// `&&`
    RK_TOKEN_AND_LOGIC,
    /// `&`
    RK_TOKEN_AND_BIT,

    /// `||`
    RK_TOKEN_OR_LOGIC,
    /// `|`
    RK_TOKEN_OR_BIT,

    /// `@`
    RK_TOKEN_AT,
    /// `!`
    RK_TOKEN_BANG,
    /// `**`
    RK_TOKEN_STAR_STAR,
    /// `//`
    RK_TOKEN_SLASH_SLASH,
    /// `.*`
    RK_TOKEN_DOT_STAR,

    /// `=>`
    RK_TOKEN_EQ_ARROW,
    /// `<-`
    RK_TOKEN_LT_ARROW,
    /// `->`
    RK_TOKEN_RT_ARROW,

    /// `(`
    RK_TOKEN_OPEN_PAREN,
    /// `)`
    RK_TOKEN_CLOSE_PAREN,

    /// `{`
    RK_TOKEN_OPEN_BRACE,
    /// `}`
    RK_TOKEN_CLOSE_BRACE,

    /// `[`
    RK_TOKEN_OPEN_BRACKET,
    /// `]`
    RK_TOKEN_CLOSE_BRACKET,

    /// `[|`
    RK_TOKEN_OPEN_ATTRIBUTES,
    /// `|]`
    RK_TOKEN_CLOSE_ATTRIBUTES,

    /// `.`
    RK_TOKEN_DOT,
    /// `..`
    RK_TOKEN_DOT_DOT,
    /// `...`
    RK_TOKEN_DOT_DOT_DOT,
    /// `..<`
    RK_TOKEN_DOT_DOT_LT,
    /// `..=`
    RK_TOKEN_DOT_DOT_EQ,

    /// `,`
    RK_TOKEN_COMMA,
    /// `\'`
    RK_TOKEN_APOSTROPHE,
    /// `:`
    RK_TOKEN_COLON,
    /// `::`
    RK_TOKEN_COLON_COLON,
    /// `;`
    RK_TOKEN_SEMICOLON,
    
    /// `_`
    RK_TOKEN_UNDERSCORE,
    /// `---`
    RK_TOKEN_UNDEFINED,

    /// `pub`
    RK_TOKEN_PUB,
    /// `mut`
    RK_TOKEN_MUT,
    
    /// `let`
    RK_TOKEN_LET,
    /// `const`
    RK_TOKEN_CONST,
    /// `comptime`
    RK_TOKEN_COMPTIME,

    /// `null`
    RK_TOKEN_NULL,
    /// `true`
    RK_TOKEN_TRUE,
    /// `false`
    RK_TOKEN_FALSE,

    /// `fn`
    RK_TOKEN_FN,
    /// `enum`
    RK_TOKEN_ENUM,
    /// `struct`
    RK_TOKEN_STRUCT,

    /// `if`
    RK_TOKEN_IF,
    /// `then`
    RK_TOKEN_THEN,
    /// `elif`
    RK_TOKEN_ELIF,
    /// `else`
    RK_TOKEN_ELSE,
    
    /// `match`
    RK_TOKEN_MATCH,
    /// `loop`
    RK_TOKEN_LOOP,
    /// `break`
    RK_TOKEN_BREAK,
    /// `return`
    RK_TOKEN_RETURN,
} RkTokenKind;

static
rk_u32 rk_token_kind_tag_name_len(RkTokenKind kind) {
    switch(kind) {
        case RK_TOKEN_EOF:                 return sizeof("eof") - 1;
        
        case RK_TOKEN_FLOAT:               return sizeof("float") - 1;
        case RK_TOKEN_INTEGER:             return sizeof("integer") - 1;
        case RK_TOKEN_IDENT:               return sizeof("ident") - 1;
        case RK_TOKEN_STRING:              return sizeof("string") - 1;
        case RK_TOKEN_STRING_UNTERMINATED: return sizeof("string_unt") - 1;

        case RK_TOKEN_PLUS:                return sizeof("plus") - 1;
        case RK_TOKEN_MINUS:               return sizeof("minus") - 1;
        case RK_TOKEN_STAR:                return sizeof("star") - 1;
        case RK_TOKEN_SLASH:               return sizeof("slash") - 1;
        
        case RK_TOKEN_PLUS_EQ:             return sizeof("plus_eq") - 1;
        case RK_TOKEN_MINUS_EQ:            return sizeof("minus_eq") - 1;
        case RK_TOKEN_STAR_EQ:             return sizeof("star_eq") - 1;
        case RK_TOKEN_SLASH_EQ:            return sizeof("slash_eq") - 1;
        
        case RK_TOKEN_EQ:                  return sizeof("eq") - 1;
        case RK_TOKEN_COLON_EQ:            return sizeof("colon_eq") - 1;

        case RK_TOKEN_EQ_EQ:               return sizeof("eq_eq") - 1;
        case RK_TOKEN_NOT_EQ:              return sizeof("not_eq") - 1;

        case RK_TOKEN_LT:                  return sizeof("lt") - 1;
        case RK_TOKEN_GT:                  return sizeof("gt") - 1;
        case RK_TOKEN_LE:                  return sizeof("le") - 1;
        case RK_TOKEN_GE:                  return sizeof("ge") - 1;
        
        case RK_TOKEN_AND_LOGIC:           return sizeof("and_and") - 1;
        case RK_TOKEN_AND_BIT:             return sizeof("and") - 1;

        case RK_TOKEN_OR_LOGIC:            return sizeof("or_or") - 1;
        case RK_TOKEN_OR_BIT:              return sizeof("or") - 1;
        
        case RK_TOKEN_AT:                  return sizeof("at") - 1;
        case RK_TOKEN_BANG:                return sizeof("bang") - 1;
        case RK_TOKEN_STAR_STAR:           return sizeof("star_star") - 1;
        case RK_TOKEN_SLASH_SLASH:         return sizeof("slash_slash") - 1;
        case RK_TOKEN_DOT_STAR:            return sizeof("dot_star") - 1;

        case RK_TOKEN_EQ_ARROW:            return sizeof("eq_arrow") - 1;
        case RK_TOKEN_LT_ARROW:            return sizeof("lt_arrow") - 1;
        case RK_TOKEN_RT_ARROW:            return sizeof("rt_arrow") - 1;

        case RK_TOKEN_OPEN_PAREN:          return sizeof("open_paren") - 1;
        case RK_TOKEN_CLOSE_PAREN:         return sizeof("close_paren") - 1;

        case RK_TOKEN_OPEN_BRACE:          return sizeof("open_brace") - 1;
        case RK_TOKEN_CLOSE_BRACE:         return sizeof("close_brace") - 1;

        case RK_TOKEN_OPEN_BRACKET:        return sizeof("open_bracket") - 1;
        case RK_TOKEN_CLOSE_BRACKET:       return sizeof("close_bracket") - 1;

        case RK_TOKEN_OPEN_ATTRIBUTES:     return sizeof("open_attr") - 1;
        case RK_TOKEN_CLOSE_ATTRIBUTES:    return sizeof("close_attr") - 1;
        
        case RK_TOKEN_DOT:                 return sizeof("dot") - 1;
        case RK_TOKEN_DOT_DOT:             return sizeof("dot_dot") - 1;
        case RK_TOKEN_DOT_DOT_DOT:         return sizeof("dot_dot_dot") - 1;
        case RK_TOKEN_DOT_DOT_LT:          return sizeof("dot_dot_lt") - 1;
        case RK_TOKEN_DOT_DOT_EQ:          return sizeof("dot_dot_eq") - 1;

        case RK_TOKEN_COMMA:               return sizeof("comma") - 1;
        case RK_TOKEN_APOSTROPHE:          return sizeof("apostrophe") - 1;
        case RK_TOKEN_COLON:               return sizeof("colon") - 1;
        case RK_TOKEN_COLON_COLON:         return sizeof("colon_colon") - 1;
        case RK_TOKEN_SEMICOLON:           return sizeof("semicolon") - 1;

        case RK_TOKEN_UNDERSCORE:          return sizeof("underscore") - 1;
        case RK_TOKEN_UNDEFINED:           return sizeof("undefined") - 1;

        case RK_TOKEN_PUB:                 return sizeof("pub") - 1;
        case RK_TOKEN_MUT:                 return sizeof("mut") - 1;

        case RK_TOKEN_LET:                 return sizeof("let") - 1;
        case RK_TOKEN_CONST:               return sizeof("const") - 1;
        case RK_TOKEN_COMPTIME:            return sizeof("comptime") - 1;

        case RK_TOKEN_NULL:                return sizeof("null") - 1;
        case RK_TOKEN_TRUE:                return sizeof("true") - 1;
        case RK_TOKEN_FALSE:               return sizeof("false") - 1;

        case RK_TOKEN_FN:                  return sizeof("fn") - 1;
        case RK_TOKEN_ENUM:                return sizeof("enum") - 1;
        case RK_TOKEN_STRUCT:              return sizeof("struct") - 1;

        case RK_TOKEN_IF:                  return sizeof("if") - 1;
        case RK_TOKEN_THEN:                return sizeof("then") - 1;
        case RK_TOKEN_ELIF:                return sizeof("elif") - 1;
        case RK_TOKEN_ELSE:                return sizeof("else") - 1;

        case RK_TOKEN_MATCH:               return sizeof("match") - 1;
        case RK_TOKEN_LOOP:                return sizeof("loop") - 1;
        case RK_TOKEN_BREAK:               return sizeof("break") - 1;
        case RK_TOKEN_RETURN:              return sizeof("return") - 1;
    }
}


static
char const * rk_token_kind_tag_name(RkTokenKind kind) {
    switch(kind) {
        case RK_TOKEN_EOF:                 return "eof";
        
        case RK_TOKEN_FLOAT:               return "float";
        case RK_TOKEN_INTEGER:             return "integer";
        case RK_TOKEN_IDENT:               return "ident";
        case RK_TOKEN_STRING:              return "string";
        case RK_TOKEN_STRING_UNTERMINATED: return "string.unt";
        
        case RK_TOKEN_PLUS:                return "plus";
        case RK_TOKEN_MINUS:               return "minus";
        case RK_TOKEN_STAR:                return "star";
        case RK_TOKEN_SLASH:               return "slash";

        case RK_TOKEN_PLUS_EQ:             return "plus_eq";
        case RK_TOKEN_MINUS_EQ:            return "minus_eq";
        case RK_TOKEN_STAR_EQ:             return "star_eq";
        case RK_TOKEN_SLASH_EQ:            return "slash_eq";

        case RK_TOKEN_EQ:                  return "eq";
        case RK_TOKEN_COLON_EQ:            return "colon_eq";

        case RK_TOKEN_EQ_EQ:               return "eq_eq";
        case RK_TOKEN_NOT_EQ:              return "not_eq";

        case RK_TOKEN_LT:                  return "lt";
        case RK_TOKEN_GT:                  return "gt";
        case RK_TOKEN_LE:                  return "le";
        case RK_TOKEN_GE:                  return "ge";
        
        case RK_TOKEN_AND_LOGIC:           return "and_and";
        case RK_TOKEN_AND_BIT:             return "and";

        case RK_TOKEN_OR_LOGIC:            return "or_or";
        case RK_TOKEN_OR_BIT:              return "or";
        
        case RK_TOKEN_AT:                  return "at";
        case RK_TOKEN_BANG:                return "bang";
        case RK_TOKEN_STAR_STAR:           return "star_star";
        case RK_TOKEN_SLASH_SLASH:         return "slash_slash";
        case RK_TOKEN_DOT_STAR:            return "dot_star";

        case RK_TOKEN_EQ_ARROW:            return "eq_arrow";
        case RK_TOKEN_LT_ARROW:            return "lt_arrow";
        case RK_TOKEN_RT_ARROW:            return "rt_arrow";

        case RK_TOKEN_OPEN_PAREN:          return "open_paren";
        case RK_TOKEN_CLOSE_PAREN:         return "close_paren";

        case RK_TOKEN_OPEN_BRACE:          return "open_brace";
        case RK_TOKEN_CLOSE_BRACE:         return "close_brace";

        case RK_TOKEN_OPEN_BRACKET:        return "open_bracket";
        case RK_TOKEN_CLOSE_BRACKET:       return "close_bracket";

        case RK_TOKEN_OPEN_ATTRIBUTES:     return "open_attr";
        case RK_TOKEN_CLOSE_ATTRIBUTES:    return "close_attr";
        
        case RK_TOKEN_DOT:                 return "dot";
        case RK_TOKEN_DOT_DOT:             return "dot_dot";
        case RK_TOKEN_DOT_DOT_DOT:         return "dot_dot_dot";
        case RK_TOKEN_DOT_DOT_LT:          return "dot_dot_lt";
        case RK_TOKEN_DOT_DOT_EQ:          return "dot_dot_eq";

        case RK_TOKEN_COMMA:               return "comma";
        case RK_TOKEN_APOSTROPHE:          return "apostrophe";
        case RK_TOKEN_COLON:               return "colon";
        case RK_TOKEN_COLON_COLON:         return "colon_colon";
        case RK_TOKEN_SEMICOLON:           return "semicolon";

        case RK_TOKEN_UNDERSCORE:          return "underscore";
        case RK_TOKEN_UNDEFINED:           return "undefined";

        case RK_TOKEN_PUB:                 return "pub";
        case RK_TOKEN_MUT:                 return "mut";

        case RK_TOKEN_LET:                 return "let";
        case RK_TOKEN_CONST:               return "const";
        case RK_TOKEN_COMPTIME:            return "comptime";

        case RK_TOKEN_NULL:                return "null";
        case RK_TOKEN_TRUE:                return "true";
        case RK_TOKEN_FALSE:               return "false";

        case RK_TOKEN_FN:                  return "fn";
        case RK_TOKEN_ENUM:                return "enum";
        case RK_TOKEN_STRUCT:              return "struct";

        case RK_TOKEN_IF:                  return "if";
        case RK_TOKEN_THEN:                return "then keyword";
        case RK_TOKEN_ELIF:                return "elif";
        case RK_TOKEN_ELSE:                return "else";

        case RK_TOKEN_MATCH:               return "match";
        case RK_TOKEN_LOOP:                return "loop";
        case RK_TOKEN_BREAK:               return "break";
        case RK_TOKEN_RETURN:              return "return";
    }
}

typedef struct {
    RkSpan span;
    RkTokenKind kind;
} RkToken;

typedef struct {
    char const * const ptr;
    rk_usize idx;
    rk_usize end;
} RkLexer;

static inline
RkToken rk_token_new(RkTokenKind kind, RkPos start, RkPos len) {
    return (RkToken){.kind = kind, .span = {.start = start, .len = len}};
}

static inline
RkLexer rk_lexer_new(char const * const ptr, rk_usize len) {
    return (RkLexer){.ptr = ptr, .idx = 0, .end = len};
}

////////////////////////////////////////
// Helping conditions

static inline
bool rk_is_digit(rk_u8 b) {
    return '0' <= b && b <= '9';
}

static inline
bool rk_is_alpha(rk_u8 b) {
    return ('A' <= b && b <= 'Z') || ('a' <= b && b <= 'z');
}

static inline
bool rk_is_space(rk_u8 b) {
    return b == ' ' || b == '\t' || b == '\r' || b == '\n';
}

static inline
bool rk_is_ascii(rk_u8 b) {
    return b < 0x80;
}

static inline
bool rk_is_integer_continue(rk_u8 b) {
    return rk_is_digit(b) || rk_is_alpha(b) || b == '_' || b == '@';
}

static inline
bool rk_is_ident_continue(rk_u8 b) {
    return rk_is_integer_continue(b);
}

static inline
bool rk_is_ident(rk_u8 b1, rk_u8 b2) {
    return rk_is_alpha(b1) || b1 == '_' || !rk_is_ascii(b1) || (b1 == '@' && (rk_is_ident_continue(b2) || !rk_is_ascii(b2)));
}

////////////////////////////////////////
// Lexer implementation

static inline
rk_u8 rk_lexer_peek(RkLexer const * const lexer, rk_usize n) {
    rk_usize index = lexer->idx + n;
    if (index < lexer->end) {
        return lexer->ptr[index];
    } else {
        return 0;
    }
}

static inline
bool rk_lexer_at_eof(RkLexer * const lexer) {
    return lexer->end <= lexer->idx;
}

static inline
void rk_lexer_skip(RkLexer * const lexer, rk_usize n) {
    RK_ASSERT(n > 0, "`n` is natural number");

    rk_usize index = lexer->idx + n;
    if (index <= lexer->end) {
        lexer->idx = index;
    } else {
        lexer->idx = lexer->end;
    }
}

#define rk_lexer_eat_while(lexer, condition) ({                 \
    rk_usize loop_count = 0;                                    \
    for (;;) {                                                  \
        if (rk_lexer_at_eof(lexer)) break;                      \
        rk_u8 b = rk_lexer_peek(lexer, 0);                      \
        if (!(condition)) break;                                \
        (lexer)->idx += 1;                                      \
        loop_count += 1;                                        \
        RK_ASSERT(loop_count <= 1024, "loop limit is reached"); \
    };                                                          \
    loop_count;                                                 \
})

static inline
rk_usize rk_lexer_eat(RkLexer * const lexer, char const * const pat) {
    RK_ASSERT(pat[0] != '\0', "expected that len of `pat` not equals zero");
    for (rk_usize p = 0; true; p += 1) {
        rk_usize i = lexer->idx + p;

        if (pat[p] == 0) {
            lexer->idx = i;
            return p;
        }

        if (lexer->end <= i || lexer->ptr[i] != pat[p]) return 0;
    }
}

static inline
bool rk_lexer_kw(RkLexer * const lexer, char const * const kw) {
    return rk_lexer_eat(lexer, kw) != 0 && !rk_is_ident_continue(rk_lexer_peek(lexer, 0));
}

static inline
RkTokenKind rk_lexer_ident(RkLexer * const lexer) {
    rk_u8 b1 = rk_lexer_peek(lexer, 0);
    rk_u8 b2 = rk_lexer_peek(lexer, 1);
    RK_ASSERT(rk_is_ident(b1, b2), "");
    
    for (;;) {
        if (rk_lexer_at_eof(lexer)) break;
        rk_u8 b = rk_lexer_peek(lexer, 0);
        if (rk_is_ascii(b) && !rk_is_ident_continue(b)) break;
        lexer->idx += rk_utf8_len(b);
    }

    return RK_TOKEN_IDENT;
}

static inline
RkTokenKind rk_lexer_number(RkLexer * const lexer) {
    for (;;) {
        if (rk_lexer_at_eof(lexer)) break;
        rk_u8 b = rk_lexer_peek(lexer, 0);
        if (rk_is_ascii(b) && !rk_is_integer_continue(b)) break;
        lexer->idx += rk_utf8_len(b);
    }
    
    rk_u8 b1 = rk_lexer_peek(lexer, 0);
    rk_u8 b2 = rk_lexer_peek(lexer, 1);
    if (b1 != '.' || !rk_is_integer_continue(b2)) return RK_TOKEN_INTEGER;
    lexer->idx += 2;

    for (;;) {
        if (rk_lexer_at_eof(lexer)) break;
        rk_u8 b = rk_lexer_peek(lexer, 0);
        if (rk_is_ascii(b) && !rk_is_integer_continue(b)) break;
        lexer->idx += rk_utf8_len(b);
    }

    return RK_TOKEN_FLOAT;
}

static inline
RkTokenKind rk_lexer_string(RkLexer * const lexer) {
    rk_lexer_eat_while(lexer, b != '\n' && b != '"');
    if (rk_lexer_eat(lexer, "\"") != 0) return RK_TOKEN_STRING;
    else                                return RK_TOKEN_STRING_UNTERMINATED;
}

static inline
void rk_lexer_skip_unused(RkLexer * const lexer) {
    for (;;) {
        rk_u8 b1 = rk_lexer_peek(lexer, 0);
        rk_u8 b2 = rk_lexer_peek(lexer, 1);

        if (b1 == '/' && b2 == '/') {
            lexer->idx += 2;
            rk_lexer_eat_while(lexer, b != '\n');

            rk_lexer_eat(lexer, "\n");
            continue;
        }

        if (rk_is_space(b1)) {
            rk_lexer_eat_while(lexer, rk_is_space(b));
            continue;
        }

        break;
    }
}

static inline
RkToken rk_lexer_next_token(RkLexer * const lexer) {
    rk_usize start = lexer->idx;

    rk_lexer_skip_unused(lexer);
    if (rk_lexer_at_eof(lexer)) return rk_token_new(RK_TOKEN_EOF, start, 0);
    
    start = lexer->idx;
    RkTokenKind kind = RK_TOKEN_EOF;
    rk_u8 b1 = rk_lexer_peek(lexer, 0);
    rk_u8 b2 = rk_lexer_peek(lexer, 1);
    
    if (false) {}
    else if (rk_lexer_kw(lexer, "comptime")) kind = RK_TOKEN_COMPTIME;
    else if (rk_lexer_kw(lexer, "struct"))   kind = RK_TOKEN_STRUCT;
    else if (rk_lexer_kw(lexer, "return"))   kind = RK_TOKEN_RETURN;
    else if (rk_lexer_kw(lexer, "const"))    kind = RK_TOKEN_CONST;
    else if (rk_lexer_kw(lexer, "match"))    kind = RK_TOKEN_MATCH;
    else if (rk_lexer_kw(lexer, "break"))    kind = RK_TOKEN_BREAK;
    else if (rk_lexer_kw(lexer, "false"))    kind = RK_TOKEN_FALSE;
    else if (rk_lexer_kw(lexer, "enum"))     kind = RK_TOKEN_ENUM;
    else if (rk_lexer_kw(lexer, "true"))     kind = RK_TOKEN_TRUE;
    else if (rk_lexer_kw(lexer, "null"))     kind = RK_TOKEN_NULL;
    else if (rk_lexer_kw(lexer, "loop"))     kind = RK_TOKEN_LOOP;
    else if (rk_lexer_kw(lexer, "then"))     kind = RK_TOKEN_THEN;
    else if (rk_lexer_kw(lexer, "elif"))     kind = RK_TOKEN_ELIF;
    else if (rk_lexer_kw(lexer, "else"))     kind = RK_TOKEN_ELSE;
    else if (rk_lexer_kw(lexer, "let"))      kind = RK_TOKEN_LET;
    else if (rk_lexer_kw(lexer, "mut"))      kind = RK_TOKEN_MUT;
    else if (rk_lexer_kw(lexer, "pub"))      kind = RK_TOKEN_PUB;
    else if (rk_lexer_kw(lexer, "fn"))       kind = RK_TOKEN_FN;
    else if (rk_lexer_kw(lexer, "if"))       kind = RK_TOKEN_IF;
    else if (rk_is_ident(b1, b2))            kind = rk_lexer_ident(lexer);
    else if (rk_is_digit(b1))                kind = rk_lexer_number(lexer);
    else if (rk_lexer_eat(lexer, "\""))      kind = rk_lexer_string(lexer);
    else if (rk_lexer_eat(lexer, "---"))     kind = RK_TOKEN_UNDEFINED;
    else if (rk_lexer_eat(lexer, "..."))     kind = RK_TOKEN_DOT_DOT_DOT;
    else if (rk_lexer_eat(lexer, "..<"))     kind = RK_TOKEN_DOT_DOT_LT;
    else if (rk_lexer_eat(lexer, "..="))     kind = RK_TOKEN_DOT_DOT_EQ;
    else if (rk_lexer_eat(lexer, "&&"))      kind = RK_TOKEN_AND_LOGIC;
    else if (rk_lexer_eat(lexer, "||"))      kind = RK_TOKEN_OR_LOGIC;
    else if (rk_lexer_eat(lexer, "+="))      kind = RK_TOKEN_PLUS_EQ;
    else if (rk_lexer_eat(lexer, "*="))      kind = RK_TOKEN_STAR_EQ;
    else if (rk_lexer_eat(lexer, "-="))      kind = RK_TOKEN_MINUS_EQ;
    else if (rk_lexer_eat(lexer, "/="))      kind = RK_TOKEN_SLASH_EQ;
    else if (rk_lexer_eat(lexer, "=="))      kind = RK_TOKEN_EQ_EQ;
    else if (rk_lexer_eat(lexer, "!="))      kind = RK_TOKEN_NOT_EQ;
    else if (rk_lexer_eat(lexer, "<="))      kind = RK_TOKEN_LE;
    else if (rk_lexer_eat(lexer, ">="))      kind = RK_TOKEN_GE;
    else if (rk_lexer_eat(lexer, "=>"))      kind = RK_TOKEN_EQ_ARROW;
    else if (rk_lexer_eat(lexer, "<-"))      kind = RK_TOKEN_LT_ARROW;
    else if (rk_lexer_eat(lexer, "->"))      kind = RK_TOKEN_RT_ARROW;
    else if (rk_lexer_eat(lexer, "**"))      kind = RK_TOKEN_STAR_STAR;
    else if (rk_lexer_eat(lexer, "//"))      kind = RK_TOKEN_SLASH_SLASH;
    else if (rk_lexer_eat(lexer, ".*"))      kind = RK_TOKEN_DOT_STAR;
    else if (rk_lexer_eat(lexer, ".."))      kind = RK_TOKEN_DOT_DOT;
    else if (rk_lexer_eat(lexer, ":="))      kind = RK_TOKEN_COLON_EQ;
    else if (rk_lexer_eat(lexer, "::"))      kind = RK_TOKEN_COLON_COLON;
    else if (rk_lexer_eat(lexer, "|]"))      kind = RK_TOKEN_CLOSE_ATTRIBUTES;
    else if (rk_lexer_eat(lexer, "[|"))      kind = RK_TOKEN_OPEN_ATTRIBUTES;
    else if (rk_lexer_eat(lexer, "@"))       kind = RK_TOKEN_AT;
    else if (rk_lexer_eat(lexer, "&"))       kind = RK_TOKEN_AND_BIT;
    else if (rk_lexer_eat(lexer, "|"))       kind = RK_TOKEN_OR_BIT;
    else if (rk_lexer_eat(lexer, "+"))       kind = RK_TOKEN_PLUS;
    else if (rk_lexer_eat(lexer, "-"))       kind = RK_TOKEN_MINUS;
    else if (rk_lexer_eat(lexer, "*"))       kind = RK_TOKEN_STAR;
    else if (rk_lexer_eat(lexer, "/"))       kind = RK_TOKEN_SLASH;
    else if (rk_lexer_eat(lexer, "!"))       kind = RK_TOKEN_BANG;
    else if (rk_lexer_eat(lexer, "<"))       kind = RK_TOKEN_LT;
    else if (rk_lexer_eat(lexer, ">"))       kind = RK_TOKEN_GT;
    else if (rk_lexer_eat(lexer, "="))       kind = RK_TOKEN_EQ;
    else if (rk_lexer_eat(lexer, "("))       kind = RK_TOKEN_OPEN_PAREN;
    else if (rk_lexer_eat(lexer, ")"))       kind = RK_TOKEN_CLOSE_PAREN;
    else if (rk_lexer_eat(lexer, "{"))       kind = RK_TOKEN_OPEN_BRACE;
    else if (rk_lexer_eat(lexer, "}"))       kind = RK_TOKEN_CLOSE_BRACE;
    else if (rk_lexer_eat(lexer, "["))       kind = RK_TOKEN_OPEN_BRACKET;
    else if (rk_lexer_eat(lexer, "]"))       kind = RK_TOKEN_CLOSE_BRACKET;
    else if (rk_lexer_eat(lexer, "."))       kind = RK_TOKEN_DOT;
    else if (rk_lexer_eat(lexer, ","))       kind = RK_TOKEN_COMMA;
    else if (rk_lexer_eat(lexer, "'"))       kind = RK_TOKEN_APOSTROPHE;
    else if (rk_lexer_eat(lexer, ":"))       kind = RK_TOKEN_COLON;
    else if (rk_lexer_eat(lexer, ";"))       kind = RK_TOKEN_SEMICOLON;
    else {
        RK_UNREACHABLE("");
        // lexer->idx += rk_utf8_len(b1);
        // rk_usize save = lexer->idx;
        // RkToken next = rk_lexer_next_token(lexer);
        // if (save != next.span.start || next.kind != RK_TOKEN_ILLEGAL) {
        //     lexer->idx = save;
        // }
        // kind = RK_TOKEN_ILLEGAL;
    }

    RkPos len = lexer->idx - start;
    return rk_token_new(kind, start, len);
}

////////////////////////////////////////
// Tokens

RK_LIST(
    RkTokenBuf, RkTokensSlice, RkTokensIndexed,
    rk_tokens, RkToken,
    rk_u32, RK_U32_MAX,
)

static inline
RkTokenBuf rk_tokens_from_source(char const * const ptr, rk_usize len) {
    RK_ASSERT(len <= RK_MB(1), "too big source");
    RkTokenBuf buf = rk_tokens_alloc(len / 4);
    RkLexer lexer = rk_lexer_new(ptr, len);
    for (;;) {
        RkToken token = rk_lexer_next_token(&lexer);
        rk_tokens_push(&buf, token);
        if (token.kind == RK_TOKEN_EOF) break;
    }

    // if (buf.len > 1) {
    //     RkSpan span = buf.ptr[buf.len - 2].span;
    //     RkPos start = span.start + span.len;
    //     buf.ptr[buf.len - 1].span = (RkSpan){.start = start,.len = 0};
    // }
    return buf;
}

typedef struct {
    RkUnit unit;
    RkTokenBuf tokens;
} RkLex;

static inline
RkLex rk_lex(RkUnit const unit, RkDiag * const diag) {
    (void)diag; // TODO: add errors on illegal tokens
    RK_ASSERT(unit.src.len <= RK_MB(1), "too big source");
    RkTokenBuf tokens = rk_tokens_alloc(unit.src.len / 4);
    RkLexer lexer = rk_lexer_new(unit.src.ptr, unit.src.len);
    for (;;) {
        RkToken token = rk_lexer_next_token(&lexer);
        rk_tokens_push(&tokens, token);
        if (token.kind == RK_TOKEN_EOF) break;
    }
    return (RkLex){.unit = unit, .tokens = tokens};
}

static
void rk_lex_display(RkLex const * const lex, RkDiag * const diag) {
    RkUnit const unit = lex->unit;
    RkTokenBuf const tokens = lex->tokens;
    
    rk_f64 const ratio = (rk_f64)tokens.len / unit.src.len;
    rk_diag_print(diag, RK_GREEN_BOLD "ratio" RK_WHITE_BOLD ": %llu/%llu = %.2f\n" RK_CLEAN, tokens.len, unit.src.len, ratio);
    
    rk_u32 const token_width = rk_decimal_len(tokens.len - 1);
    rk_u32 const byte_width = rk_decimal_len(unit.src.len);
    RK_ASSERT(token_width >= 1, "");
    RK_ASSERT(byte_width >= 1, "");

    rk_u32 tag_width = 0;
    rk_u32 lexeme_width = 0;
    for (rk_usize i = 0; i < tokens.len; i += 1) {
        RkToken const token = tokens.ptr[i];
        rk_u32 tag_len = rk_token_kind_tag_name_len(token.kind);
        rk_u32 lexeme_len = rk_utf8_chars_count(&lex->unit.src.ptr[token.span.start], token.span.len);
        if (tag_width < tag_len)          tag_width = tag_len;
        if (lexeme_width < lexeme_len) lexeme_width = lexeme_len;
    }
    
    rk_u32 span_width = byte_width * 2 + 1;
    if (span_width < 4)   span_width   = 4;
    if (lexeme_width < 6) lexeme_width = 6;

    rk_diag_print(diag, RK_BLACK_BOLD);
    rk_diag_print(diag, "┌");
    rk_diag_repeat_print(diag, 1 + token_width + 1, "─");
    rk_diag_print(diag, "┬");
    rk_diag_repeat_print(diag, 1 + tag_width + 1, "─");
    rk_diag_print(diag, "┬");
    rk_diag_repeat_print(diag, 1 + span_width + 1, "─");
    rk_diag_print(diag, "┬");
    rk_diag_repeat_print(diag, 1 + lexeme_width + 1, "─");
    rk_diag_print(diag, "┐");
    rk_diag_print(diag, "\n" RK_CLEAN);

    rk_diag_print(
        diag,
        RK_BLACK_BOLD "│ "
        RK_ORANGE_BOLD  "%*s" "n"      "%*s" RK_BLACK_BOLD " │ "
        RK_YELLOW_BOLD  "%*s" "tag"    "%*s" RK_BLACK_BOLD " │ "
        RK_CYAN_BOLD    "%*s" "span"   "%*s" RK_BLACK_BOLD " │ "
        RK_MAGENTA_BOLD "%*s" "lexeme" "%*s" RK_BLACK_BOLD " │" "\n",
        (token_width - 1) / 2, "", (token_width - 1 + 1) / 2, "",
        (tag_width - 3) / 2, "", (tag_width - 3 + 1) / 2, "",
        (span_width - 4) / 2, "", (span_width - 4 + 1) / 2, "",
        (lexeme_width - 6) / 2, "", (lexeme_width - 6 + 1) / 2, ""
    );

    for (rk_usize i = 0; i < tokens.len; i += 1) {
        RkToken const token = tokens.ptr[i];
        char const * const name = rk_token_kind_tag_name(token.kind);
        
        rk_u32 span_space = byte_width * 2 + 1;
        if (span_space > 4) span_space = 0;
        else                span_space = 4 - span_space;

        if (i % 16 == 0) {
            rk_diag_print(diag, RK_BLACK_BOLD);
            rk_diag_print(diag, "├");
            rk_diag_repeat_print(diag, 1 + token_width + 1, "─");
            rk_diag_print(diag, "┼");
            rk_diag_repeat_print(diag, 1 + tag_width + 1, "─");
            rk_diag_print(diag, "┼");
            rk_diag_repeat_print(diag, 1 + (byte_width * 2 + 1 + span_space) + 1, "─");
            rk_diag_print(diag, "┼");
            rk_diag_repeat_print(diag, 1 + lexeme_width + 1, "─");
            rk_diag_print(diag, "┤");
            rk_diag_print(diag, "\n" RK_CLEAN);
        }

        rk_u32 start = token.span.start;
        rk_u32 len = token.span.len;
        rk_u32 lexeme_len = rk_utf8_chars_count(&lex->unit.src.ptr[token.span.start], len);
        char const * const lexeme = &unit.src.ptr[start];

        rk_diag_print(
            diag,
            RK_BLACK_BOLD "│ "
            RK_ORANGE_BOLD "%0*llu"                 RK_BLACK_BOLD " │ "
            RK_YELLOW_BOLD "%-*s"                   RK_BLACK_BOLD " │ "
            RK_CYAN_BOLD   "%*hu" ":" "%-*hu" "%*s" RK_BLACK_BOLD " │ "
            RK_MAGENTA_BOLD "%.*s" "%*s"            RK_BLACK_BOLD " │" "\n" RK_CLEAN,
            token_width, i,
            tag_width, name,
            byte_width, start, byte_width, start + len, span_space, "",
            len, lexeme,
            lexeme_width - lexeme_len, ""
        );

        if (i == tokens.len - 1) {
            rk_diag_print(diag, RK_BLACK_BOLD);
            rk_diag_print(diag, "└");
            rk_diag_repeat_print(diag, 1 + token_width + 1, "─");
            rk_diag_print(diag, "┴");
            rk_diag_repeat_print(diag, 1 + tag_width + 1, "─");
            rk_diag_print(diag, "┴");
            rk_diag_repeat_print(diag, 1 + (byte_width * 2 + 1 + span_space) + 1, "─");
            rk_diag_print(diag, "┴");
            rk_diag_repeat_print(diag, 1 + lexeme_width + 1, "─");
            rk_diag_print(diag, "┘");
            rk_diag_print(diag, "\n" RK_CLEAN);
            break;
        }
    }
}

////////////////////////////////////////
// Diagnostic extention

#define RK_DIAG_COMMAND_BUILD \
    RK_CYAN_BOLD "build" RK_WHITE_BOLD ", " RK_CYAN_BOLD "b" RK_MAGENTA_BOLD " <FILE>"

#define RK_DIAG_COMMAND_LEX \
    RK_CYAN_BOLD "lex" RK_WHITE_BOLD ", " RK_CYAN_BOLD "l" RK_MAGENTA_BOLD " <FILE>"

static inline
void rk_diag_usage(RkDiag * const diag) {
    rk_diag_write_cstr(diag,
        RK_GREEN_BOLD "USAGE" "\n"
        "    " RK_RED_BOLD "risk " RK_CYAN_BOLD "[COMMAND]\n"
        RK_GREEN_BOLD "COMMANDS" "\n"
        "    " RK_DIAG_COMMAND_BUILD "\n"
        "    " RK_DIAG_COMMAND_LEX "\n"
        RK_CLEAN
    );
}

static
void rk_diag_can_not_load_unit(
    RkDiag * const diag,
    char const * const path,
    RkUnitLoadKind const error
) {
    char const *msg = NULL;
    switch (error) {
        case RK_UNIT_LOAD_OK:                   RK_UNREACHABLE("");
        case RK_UNIT_LOAD_TOO_BIG:              msg = "file too big";                       break;
        case RK_UNIT_LOAD_PERMISSION_DENIED:    msg = "permission denied";                  break;
        case RK_UNIT_LOAD_EXPECTED_FILE:        msg = "expected file, but found folder";    break;
        case RK_UNIT_LOAD_NOT_FOUND:            msg = "file not found";                     break;
        case RK_UNIT_LOAD_UNKNOWN_ERROR:        msg = "can not load file (unknown error)";  break;
    }

    rk_diag_print(diag,
        RK_RED_BOLD "error" RK_WHITE_BOLD ": %s\n"
        RK_CYAN_BOLD " --> %s" "\n" RK_CLEAN,
        msg, path
    );
}

////////////////////////////////////////
// Compiler

typedef struct {
    char const * const exe;
    RkDiag diag;
} RkCompiler;

typedef enum {
    RK_ACTION_BUILD,
    RK_ACTION_LEX,
} RkActionKind;

typedef struct {
    char const * input;
} RkActionBuild;

typedef struct {
    char const * input;
} RkActionLex;

typedef struct {
    RkActionKind kind;
    union {
        RkActionBuild build;
        RkActionLex lex;
    };
} RkAction;

static inline noreturn
void rk_diag_flush_and_exit(RkDiag * const diag, rk_i32 const code) {
    // TODO: hardcore output
    rk_diag_flush_and_dealloc(diag, stderr);
    exit(code);
}

static inline
RkUnit rk_compiler_unit_load_or_exit(RkDiag * const diag, char const * const path) {
    RkUnitLoadResult result = rk_unit_try_load(path);
    if (result.kind == RK_UNIT_LOAD_OK) return result.unit;
    rk_diag_can_not_load_unit(diag, path, result.kind);
    rk_diag_flush_and_exit(diag, -1);
}

static
void rk_compile_unit(RkDiag * const diag, RkUnit unit) {
    (void)diag;
    RK_TODO("");
    rk_unit_dealloc(&unit);
}

static
void rk_compile(RkDiag * const diag, char const * const path) {
    RkUnit unit = rk_compiler_unit_load_or_exit(diag, path);
    rk_diag_print(diag, RK_CYAN_BOLD "load" RK_WHITE_BOLD ": %s", unit.path.ptr);
    rk_compile_unit(diag, unit);
}

static noreturn
void rk_diag_build_command_help_and_exit(RkDiag * const diag) {
    rk_diag_write_cstr(
        diag,
        RK_GREEN_BOLD "USAGE" "\n"
        "    " RK_DIAG_COMMAND_BUILD "\n"
        RK_GREEN_BOLD "DESCRIPTION" "\n"
        "    " RK_BLACK_BOLD_ITALIC "build " RK_MAGENTA_BOLD "<FILE>"
               RK_BLACK_BOLD_ITALIC " in executable (`main.rk` -> `main.exe`)" "\n"
        RK_GREEN_BOLD "ARGS" "\n"
        "    " RK_MAGENTA_BOLD "<FILE>" RK_BLACK_BOLD_ITALIC " ~ source file (aka 'main.rk')" "\n"
        RK_GREEN_BOLD "FLAGS" "\n"
        "    " RK_ORANGE_BOLD "--help" RK_WHITE_BOLD ", " RK_ORANGE_BOLD "-h" RK_BLACK_BOLD_ITALIC " ~ print this message" "\n"
        RK_CLEAN
    );
    rk_diag_flush_and_exit(diag, 0);
}

static noreturn
void rk_diag_failed_analyze_build_command(RkDiag * const diag) {
    rk_diag_write_cstr(
        diag,
        RK_GREEN_BOLD_ITALIC "[?]" RK_BLACK_BOLD_ITALIC " use `"
        RK_RED_BOLD "risk" RK_CYAN_BOLD " build" RK_ORANGE_BOLD " --help"
        RK_BLACK_BOLD_ITALIC "` for more information" "\n" RK_CLEAN
    );
    rk_diag_flush_and_exit(diag, -1);
}

static
RkActionBuild rk_analyze_build_command(
    RkDiag * const diag,
    rk_i32 const argc,
    char const * const * const argv
) {
    if (argc == 0) rk_diag_build_command_help_and_exit(diag);

    if (argc != 1) {
        rk_diag_print(diag, RK_RED_BOLD "error" RK_WHITE_BOLD ": too many args\n" RK_CLEAN);
        rk_diag_failed_analyze_build_command(diag);
    }

    if (strcmp(argv[0], "-h") == 0 || strcmp(argv[0], "--help") == 0) {
        rk_diag_build_command_help_and_exit(diag);
    } else if (argv[0][0] == '-') {
        rk_diag_print(diag, RK_RED_BOLD "error" RK_WHITE_BOLD ": unknown flag `" RK_ORANGE_BOLD "%s" RK_WHITE_BOLD "`" "\n", argv[0]);
        rk_diag_failed_analyze_build_command(diag);
    }

    return (RkActionBuild){.input = argv[0]};
}

static noreturn
void rk_diag_lex_command_help_and_exit(RkDiag * const diag) {
    rk_diag_write_cstr(diag,
        RK_GREEN_BOLD "USAGE" "\n"
        "    " RK_DIAG_COMMAND_LEX "\n"
        RK_GREEN_BOLD "DESCRIPTION" "\n"
        "    " RK_BLACK_BOLD_ITALIC "lex " RK_MAGENTA_BOLD "<FILE>"
               RK_BLACK_BOLD_ITALIC " into tokens (`main.rk` -> `main.rk.lex`)" "\n"
        RK_GREEN_BOLD "ARGS" "\n"
        "    " RK_MAGENTA_BOLD "<FILE>" RK_BLACK_BOLD_ITALIC " ~ source file (aka 'main.rk')" "\n"
        RK_GREEN_BOLD "FLAGS" "\n"
        "    " RK_ORANGE_BOLD "--help" RK_WHITE_BOLD ", " RK_ORANGE_BOLD "-h" RK_BLACK_BOLD_ITALIC " ~ print this message" "\n"
        RK_CLEAN
    );
    rk_diag_flush_and_exit(diag, 0);
}

static noreturn
void rk_diag_failed_analyze_lex_command(RkDiag * const diag) {
    rk_diag_write_cstr(diag,
        RK_GREEN_BOLD_ITALIC "[?]" RK_BLACK_BOLD_ITALIC " use `"
        RK_RED_BOLD "risk" RK_CYAN_BOLD " lex" RK_ORANGE_BOLD " --help"
        RK_BLACK_BOLD_ITALIC "` for more information" "\n" RK_CLEAN
    );
    rk_diag_flush_and_exit(diag, -1);
}

static
RkActionLex rk_analyze_lex_command(
    RkDiag * const diag,
    rk_i32 const argc,
    char const * const * const argv
) {
    if (argc == 0) rk_diag_lex_command_help_and_exit(diag);

    if (argc != 1) {
        rk_diag_print(diag, RK_RED_BOLD "error" RK_WHITE_BOLD ": too many args\n" RK_CLEAN);
        rk_diag_failed_analyze_lex_command(diag);
    }

    if (strcmp(argv[0], "-h") == 0 || strcmp(argv[0], "--help") == 0) {
        rk_diag_lex_command_help_and_exit(diag);
    } else if (argv[0][0] == '-') {
        rk_diag_print(diag, RK_RED_BOLD "error" RK_WHITE_BOLD ": unknown flag `" RK_ORANGE_BOLD "%s" RK_WHITE_BOLD "`" "\n", argv[0]);
        rk_diag_failed_analyze_lex_command(diag);
    }

    return (RkActionLex){.input = argv[0]};
}

static
void rk_diag_error_renamed_compiler(RkDiag * const diag, char const * const compiler) {
    rk_diag_print(
        diag,
        RK_RED_BOLD "error" RK_WHITE_BOLD ": compiler renamed to "
        "`" RK_RED_BOLD "%s" RK_WHITE_BOLD "`" ", but original name `" RK_RED_BOLD "risk.exe" RK_WHITE_BOLD "`" "\n"
        RK_YELLOW_BOLD_ITALIC "[!]" RK_BLACK_BOLD_ITALIC " it could be "
        RK_BLACK_BOLD_ITALIC RK_UNDERLINE "risky" RK_BLACK_BOLD_ITALIC ", you know?" "\n"
        RK_GREEN_BOLD_ITALIC "[?]" RK_BLACK_BOLD_ITALIC " rename compiler to "
        "`" RK_RED_BOLD "risk.exe" RK_BLACK_BOLD_ITALIC "` (plz)" "\n"
        RK_CLEAN, compiler
    );
} 

static
void rk_diag_error_global_flag(RkDiag * const diag, char const * const flag) {
    rk_diag_print(
        diag,
        RK_RED_BOLD "error" RK_WHITE_BOLD ": unknown global flag `"
        RK_ORANGE_BOLD "%s" RK_WHITE_BOLD "`" "\n"
        RK_YELLOW_BOLD_ITALIC "[!]" RK_BLACK_BOLD_ITALIC " global " RK_ORANGE_BOLD "flags" " "
        RK_BLACK_BOLD_ITALIC RK_UNDERLINE "don't exist" "\n"
        RK_CLEAN, flag
    );
}

static noreturn
void rk_diag_for_more_info_and_exit(RkDiag * const diag) {
    rk_diag_write_cstr(
        diag,
        RK_GREEN_BOLD_ITALIC "[?]" RK_BLACK_BOLD_ITALIC " use `"
        RK_RED_BOLD "risk"
        RK_BLACK_BOLD_ITALIC "` for more information" "\n" RK_CLEAN
    );
    rk_diag_flush_and_exit(diag, -1);
}

static
RkAction rk_analyze_args(
    RkDiag * const diag,
    rk_i32 const argc,
    char const * const * const argv
) {
    char const * const compiler = rk_pb_tail(argv[0], 1);
    if (strcmp(compiler, "risk") != 0 && strcmp(compiler, "risk.exe") != 0) {
        rk_diag_error_renamed_compiler(diag, compiler);
        rk_diag_flush_and_exit(diag, -1);
    }

    if (argc < 2) {
        rk_diag_usage(diag);
        rk_diag_flush_and_exit(diag, 0);
    }

    char const * const first = argv[1];
    if (first[0] == '-') {
        rk_diag_error_global_flag(diag, first);
        rk_diag_for_more_info_and_exit(diag);
    }

    if (strcmp(first, "b") == 0 || strcmp(first, "build") == 0) {
        RkActionBuild build = rk_analyze_build_command(diag, argc - 2, &argv[2]);
        return (RkAction){.kind = RK_ACTION_BUILD, .build = build};
    }

    if (strcmp(first, "l") == 0 || strcmp(first, "lex") == 0) {
        RkActionLex lex = rk_analyze_lex_command(diag, argc - 2, &argv[2]);
        return (RkAction){.kind = RK_ACTION_LEX, .lex = lex};
    }

    rk_diag_print(diag, RK_RED_BOLD "error" RK_WHITE_BOLD ": unknown command `" RK_CYAN_BOLD "%s" RK_WHITE_BOLD "`" "\n", first);
    rk_diag_for_more_info_and_exit(diag);
}

////////////////////////////////////////
// Console

static inline
void rk_enable_ansi() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

static inline
void rk_console_init() {
    if(!setlocale(LC_ALL, "en_US.UTF-8")) {
        setlocale(LC_ALL, "");
    }
    
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
        rk_enable_ansi();
    #endif
}

////////////////////////////////////////
// Main

// BUILD: clang -Wall -Wextra -Wno-unused-function risk.c -o risk.exe

rk_i32 main(
    rk_i32 const argc,
    char const * const * const argv
) {
    rk_console_init();
    RkDiag diag = RK_DIAG_EMPTY;
    RkAction action = rk_analyze_args(&diag, argc, argv);
    switch (action.kind) {
        case RK_ACTION_BUILD: RK_TODO("build action");
        case RK_ACTION_LEX: {
            RkUnit unit = rk_compiler_unit_load_or_exit(&diag, action.lex.input);
            RkLex lex = rk_lex(unit, &diag);
            rk_lex_display(&lex, &diag);
            // TODO: dealloc `lex`
        } break;
    }

    rk_diag_flush_and_dealloc(&diag, stdout);
    return 0;
}

#endif // RISK_H
