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
typedef unsigned long long rk_usz;

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
rk_u32 rk_decimal_len(rk_usz x) {
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
    printf(RK_RED_BOLD "%s" RK_WHITE_BOLD ": ", type);

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
    rk_usz len,
    char const *fmt, ...
) {
    rk_u32 num_len = rk_decimal_len(caller.line);

    printf(RK_RED_BOLD "assert" RK_WHITE_BOLD ": ");

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
    for (rk_usz i = 0; i < len; i += 1) putchar('^');
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
        for (rk_usz i = 0; i < (slen); i += 1) {                            \
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
        rk_usz len;                                                         \
        rk_usz cap;                                                         \
    } NAME;                                                                 \
                                                                            \
    typedef struct {                                                        \
        INDEX start;                                                        \
        INDEX len;                                                          \
    } INDEXED;                                                              \
                                                                            \
    typedef struct {                                                        \
        TYPE const *ptr;                                                    \
        rk_usz   len;                                                       \
    } SLICE;                                                                \
                                                                            \
    static                                                                  \
    NAME PREFIX##_alloc(rk_usz cap) {                                       \
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
    void PREFIX##_reserve(NAME *buf, rk_usz add) {                          \
        RK_LIST_RESERVE(buf->ptr, buf->len, buf->cap, add);                 \
    }                                                                       \
                                                                            \
    static                                                                  \
    SLICE PREFIX##_extend(NAME *buf, SLICE slice) {                         \
        rk_usz start = buf->len;                                            \
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
    SLICE PREFIX##_slice(NAME const *buf, rk_usz start, rk_usz len) {       \
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
    void PREFIX##_pop_n(NAME *buf, rk_usz n) {                              \
        RK_ASSERT(buf->len >= n, #NAME " not have %llu items", n);          \
        buf->len -= n;                                                      \
    }                                                                       \
                                                                            \
    static inline                                                           \
    TYPE PREFIX##_at(NAME const *buf, rk_usz index) {                       \
        RK_ASSERT(index < buf->len, "index out of bounds");                 \
        return buf->ptr[index];                                             \
    }                                                                       \
                                                                            \
    static inline                                                           \
    TYPE * PREFIX##_at_mut(NAME *buf, rk_usz index) {                       \
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
    rk_sb, char, rk_usz, RK_USZ_MAX,
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
    rk_usz len = end - start;

    vsnprintf(start, fmt_len + 1, fmt, args);
    for (rk_usz i = 1; i < n; i += 1) {
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
    rk_usz len;
    rk_usz cap;
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

    for (rk_usz i = 0; i <= add.len; i += 1) {
        RK_ASSERT(add.ptr[i] != '\0', "unexpected NULL in slice");
    }

    rk_usz reserve_len = add.len + 1;
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
char const *rk_pb_tail(char const * const path, rk_usz n) {
    rk_usz len = strlen(path);
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
    rk_usz len;
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
    rk_usz const len
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
    rk_usz file_len = ftell(file);
    RK_ASSERT(file_len != RK_U32_MAX, "failed getting file len");
    fseek(file, 0, SEEK_SET);

    rk_u8 *buf = RK_ALLOC_ARRAY(file_len, rk_u8);
    RK_ASSERT(buf != NULL, "failed allocating buf");

    rk_usz read = fread(buf, 1, file_len, file);
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
rk_usz rk_utf8_len(rk_u8 b) {
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
    rk_usz len,
    rk_usz pos,
    rk_u32 tab
) {
    rk_usz start = 0;
    rk_usz line = 1;
    rk_usz column = 1;
    for (rk_usz i = 0; i < pos; i += rk_utf8_len(ptr[i])) {
        rk_u8 b = ptr[i];

        if (b == '\n') {
            start = i + 1;
            line += 1;
            column = 0;
        }

        if (b == '\t') column += tab;
        else           column += 1;
    }

    rk_usz end = start;
    while (end < len && ptr[end] != '\n') end += 1;

    RkSpan span = {.start = start, .len = end - start};
    RkLoc loc = {.line = line, .column = column};
    return (RkLine){.span = span, .loc = loc};
}

////////////////////////////////////////
// Diagnostic

typedef struct {
    RkStrBuf buf;
} RkDiag;

#define RK_DIAG_EMPTY ((RkDiag){.buf = RK_SB_EMPTY})

static inline
RkDiag rk_diag_alloc(rk_usz cap) {
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

static inline
void rk_diag_flush(RkDiag *diag, FILE *stream) {
    if (diag->buf.len == 0) return;
    size_t written = fwrite(diag->buf.ptr, 1, diag->buf.len, stream);
    RK_ASSERT(written == diag->buf.len, "failed writing to file");
    diag->buf.len = 0;
}

static inline
void rk_diag_dealloc(RkDiag * diag) {
    rk_sb_dealloc(&diag->buf);
}

////////////////////////////////////////
// Unit

typedef struct {
    RkPathBuf path;
    RkStrBuf src;
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
    rk_usz len = file_len;
    fseek(file, 0, SEEK_SET);

    if (len > RK_POS_MAX) {
        RK_ASSERT(fclose(file) == 0, "failed close file");
        return (RkUnitLoadResult){.unit = {0}, .kind = RK_UNIT_LOAD_TOO_BIG};
    }

    rk_u8 *buf = RK_ALLOC_ARRAY(len, rk_u8);
    RK_ASSERT(buf != NULL, "failed allocating buf");

    rk_usz read = fread(buf, 1, len, file);
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
// Diagnostic extention

typedef struct {
    char const * const * names;
    char const * const * args;
    rk_usz names_len;
    rk_usz args_len;
} RkCommand;

#define RK_COMMAND(NAMES, ARGS) (RkCommand){        \
    .names = NAMES,                                 \
    .names_len = sizeof(NAMES) / sizeof(NAMES[0]),  \
    .args = ARGS,                                   \
    .args_len = sizeof(ARGS) / sizeof(ARGS[0]),     \
}                                                   \

static inline
void rk_diag_command(RkDiag * const diag, RkCommand command) {
    for (rk_usz i = 0; i < command.names_len - 1; i += 1) {
        rk_diag_print(diag, RK_CYAN_BOLD "%s" RK_WHITE_BOLD ", ", command.names[i]);
    }
    rk_diag_print(diag, RK_CYAN_BOLD "%s", command.names[command.names_len - 1]);
    
    rk_diag_print(diag, RK_MAGENTA_BOLD);
    for (rk_usz i = 0; i < command.args_len; i += 1) {
        rk_diag_print(diag, " %s", command.args[i]);
    }
    rk_diag_print(diag, "\n");
}

static char const * const RK_DIAG_BUILD_NAMES[] = {"build", "b"};
static char const * const RK_DIAG_BUILD_ARGS[] = {"<INPUT>"};

static char const * const RK_DIAG_LEX_NAMES[] = {"lex", "l"};
static char const * const RK_DIAG_LEX_ARGS[] = {"<INPUT>"};

static RkCommand RK_COMMAND_BUILD = RK_COMMAND(RK_DIAG_BUILD_NAMES, RK_DIAG_BUILD_ARGS);
static RkCommand RK_COMMAND_LEX = RK_COMMAND(RK_DIAG_LEX_NAMES, RK_DIAG_LEX_ARGS);

static inline
void rk_diag_usage(RkDiag * const diag, char const * const compiler) {
    rk_diag_print(diag, RK_GREEN_BOLD "USAGE" RK_WHITE_BOLD ": " RK_RED_BOLD "%s " RK_CYAN_BOLD "[COMMAND]\n\n", rk_pb_tail(compiler, 1));
    rk_diag_print(diag, RK_GREEN_BOLD "COMMANDS" RK_WHITE_BOLD ":\n", compiler);
    rk_diag_write_cstr(diag, "    "); rk_diag_command(diag, RK_COMMAND_BUILD);
    rk_diag_write_cstr(diag, "    "); rk_diag_command(diag, RK_COMMAND_LEX);

    rk_diag_write_cstr(diag, RK_CLEAN);
}

static inline
void rk_diag_unknown_command(RkDiag * const diag, char const * const command) {
    rk_diag_print(diag, RK_RED_BOLD "error" RK_WHITE_BOLD ": unknown command `" RK_CYAN_BOLD "%s" RK_WHITE_BOLD "`\n" RK_CLEAN, command);
}

static inline
void rk_diag_unknown_global_flag(RkDiag * const diag, char const * const flag) {
    rk_diag_print(diag, RK_RED_BOLD "error" RK_WHITE_BOLD ": unknown global flag `" RK_ORANGE_BOLD "%s" RK_WHITE_BOLD "`\n", flag);
    rk_diag_print(diag, RK_YELLOW_BOLD_ITALIC "[*]" RK_BLACK_BOLD_ITALIC " global flags " RK_UNDERLINE "don't exist\n");
    rk_diag_print(
        diag,
        RK_GREEN_BOLD_ITALIC "[?]" " "
        RK_BLACK_BOLD_ITALIC RK_UNDERLINE "instead"
        RK_BLACK_BOLD_ITALIC " use flags "
        RK_BLACK_BOLD_ITALIC RK_UNDERLINE "with commands"
        "\n"
    );
    rk_diag_print(diag, RK_BLACK_BOLD_ITALIC "   " " like `" RK_CYAN_BOLD "build" RK_ORANGE_BOLD " --help" RK_BLACK_BOLD_ITALIC "`\n");
    rk_diag_print(diag, RK_CLEAN);
}

static inline
void rk_diag_error(
    RkDiag * const diag,
    char const * const path,
    RkLoc const loc,
    rk_u32 offset,
    char const * const fmt,
    ...
) {
    rk_diag_print(diag, RK_RED_BOLD "error" RK_WHITE_BOLD ": ");
    va_list args;
    va_start(args, fmt);
    rk_diag_vprint(diag, fmt, args);
    va_end(args);
    rk_diag_print(diag, "\n");

    rk_diag_print(diag, RK_CYAN_BOLD " %*s--> %s", offset, "", path);
    if (loc.line != 0 && loc.column != 0) {
        rk_diag_print(diag, ":%hu:%hu", loc.line, loc.column);
    }

    rk_diag_print(diag, "\n" RK_CLEAN);
}

static
void rk_diag_can_not_load_unit(
    RkDiag * const diag,
    char const * const path,
    RkUnitLoadKind const error
) {
    char const *fmt = NULL;
    switch (error) {
        case RK_UNIT_LOAD_OK:                   RK_UNREACHABLE("");
        case RK_UNIT_LOAD_TOO_BIG:              fmt = "file too big";                       break;
        case RK_UNIT_LOAD_PERMISSION_DENIED:    fmt = "permission denied";                  break;
        case RK_UNIT_LOAD_EXPECTED_FILE:        fmt = "expected file, but found folder";    break;
        case RK_UNIT_LOAD_NOT_FOUND:            fmt = "file not found";                     break;
        case RK_UNIT_LOAD_UNKNOWN_ERROR:        fmt = "can not load file (unknown error)";  break;
    }
    rk_diag_error(diag, path, RK_LOC_ZERO, 0, fmt);
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

static inline
RkAction rk_action_build(char const * input) {
    return (RkAction){
        .kind = RK_ACTION_BUILD,
        .build = (RkActionBuild){.input = input},
    };
}

static inline
RkAction rk_action_lex(char const * input) {
    return (RkAction){
        .kind = RK_ACTION_LEX,
        .lex = (RkActionLex){.input = input},
    };
}

static inline noreturn
void rk_diag_flush_and_exit(RkDiag * const diag, rk_i32 const code) {
    // TODO: hardcore output
    rk_diag_flush(diag, stderr);
    rk_diag_dealloc(diag);
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
    RK_TODO("");
    rk_unit_dealloc(&unit);
}

static
void rk_compile(RkDiag * const diag, char const * const path) {
    RkUnit unit = rk_compiler_unit_load_or_exit(diag, path);
    rk_diag_print(diag, RK_CYAN_BOLD "load" RK_WHITE_BOLD ": %s", unit.path.ptr);
    rk_compile_unit(diag, unit);
}

static
RkActionBuild rk_analyze_build_command(
    RkDiag * const diag,
    rk_i32 const argc,
    char const * const * const argv
) {
    if (argc == 0) {
        rk_diag_print(diag, RK_RED_BOLD "error" RK_WHITE_BOLD ": expected " RK_MAGENTA_BOLD "<INPUT>" "\n");
        rk_diag_print(
            diag,
            RK_GREEN_BOLD_ITALIC "[?]" RK_BLACK_BOLD_ITALIC " use `"
            RK_CYAN_BOLD "build" RK_ORANGE_BOLD " --help"
            RK_BLACK_BOLD_ITALIC "` for more information\n"
            RK_CLEAN
        );
        rk_diag_flush_and_exit(diag, -1);
    }

    if (argc != 1) {
        rk_diag_print(diag, RK_RED_BOLD "error" RK_WHITE_BOLD ": too many args\n" RK_CLEAN);
        rk_diag_flush_and_exit(diag, -1);
    }

    if (strcmp(argv[0], "--help") == 0 || strcmp(argv[0], "-h") == 0) {
        rk_diag_print(diag, RK_GREEN_BOLD "USAGE\n");
        rk_diag_print(diag, "    ");
        rk_diag_command(diag, RK_COMMAND_BUILD);
        rk_diag_print(diag, RK_GREEN_BOLD "DESCRIPTION\n");
        rk_diag_print(diag,
            "    " RK_BLACK_BOLD_ITALIC "build " RK_MAGENTA_BOLD "<INPUT>"
            RK_BLACK_BOLD_ITALIC " in executable (`main.rk` -> `main.exe`)\n"
        );
        rk_diag_print(diag, RK_GREEN_BOLD "ARGS\n");
        rk_diag_print(diag, "    " RK_MAGENTA_BOLD "<INPUT>" RK_BLACK_BOLD_ITALIC " ~ source file (aka `main.rk`)\n");
        rk_diag_print(diag, RK_GREEN_BOLD "FLAGS\n");
        rk_diag_print(diag, "    " RK_ORANGE_BOLD "--help" RK_WHITE_BOLD ", " RK_ORANGE_BOLD "-h" RK_BLACK_BOLD_ITALIC " ~ print this message\n");
        rk_diag_write_cstr(diag, RK_CLEAN);
        rk_diag_flush_and_exit(diag, 0);
    }

    return (RkActionBuild){.input = argv[0]};
}

static
RkActionLex rk_analyze_lex_command(
    RkDiag * const diag,
    rk_i32 const argc,
    char const * const * const argv
) {
    if (argc == 0) {
        rk_diag_print(diag, RK_RED_BOLD "error" RK_WHITE_BOLD ": expected " RK_MAGENTA_BOLD "<INPUT>" "\n");
        rk_diag_print(
            diag,
            RK_GREEN_BOLD_ITALIC "[?]" RK_BLACK_BOLD_ITALIC " use `"
            RK_CYAN_BOLD "lex" RK_ORANGE_BOLD " --help"
            RK_BLACK_BOLD_ITALIC "` for more information\n"
            RK_CLEAN
        );
        rk_diag_flush_and_exit(diag, -1);
    }

    if (argc != 1) {
        rk_diag_print(diag, RK_RED_BOLD "error" RK_WHITE_BOLD ": too many args\n" RK_CLEAN);
        rk_diag_flush_and_exit(diag, -1);
    }

    if (strcmp(argv[0], "--help") == 0 || strcmp(argv[0], "-h") == 0) {
        rk_diag_print(diag, RK_GREEN_BOLD "USAGE\n");
        rk_diag_print(diag, "    ");
        rk_diag_command(diag, RK_COMMAND_LEX);
        rk_diag_print(diag, RK_GREEN_BOLD "DESCRIPTION\n");
        rk_diag_print(diag,
            "    " RK_BLACK_BOLD_ITALIC "lex " RK_MAGENTA_BOLD "<INPUT>"
            RK_BLACK_BOLD_ITALIC " into tokens (`main.rk` -> `main.rk.lex`)\n"
        );
        rk_diag_print(diag, RK_GREEN_BOLD "ARGS\n");
        rk_diag_print(diag, "    " RK_MAGENTA_BOLD "<INPUT>" RK_BLACK_BOLD_ITALIC " ~ source file (aka 'main.rk')\n");
        rk_diag_print(diag, RK_GREEN_BOLD "FLAGS\n");
        rk_diag_print(diag, "    " RK_ORANGE_BOLD "--help" RK_WHITE_BOLD ", " RK_ORANGE_BOLD "-h" RK_BLACK_BOLD_ITALIC " ~ print this message\n");
        rk_diag_write_cstr(diag, RK_CLEAN);
        rk_diag_flush_and_exit(diag, 0);
    }


    return (RkActionLex){.input = argv[0]};
}

static
RkAction rk_analyze_args(
    RkDiag * diag,
    rk_i32 const argc,
    char const * const * const argv
) {
    if (argc < 2) {
        rk_diag_usage(diag, argv[0]);
        rk_diag_flush_and_exit(diag, 0);
    }

    char const * const first = argv[1];

    if (first[0] == '-') {
        rk_diag_unknown_global_flag(diag, first);
        rk_diag_flush_and_exit(diag, -1);
    }

    for (rk_usz i = 0; i < RK_COMMAND_BUILD.names_len; i += 1) {
        if (strcmp(first, RK_COMMAND_BUILD.names[i]) != 0) continue;
        RkActionBuild build = rk_analyze_build_command(diag, argc - 2, &argv[2]);
        return (RkAction){.kind = RK_ACTION_BUILD, .build = build};
    }

    for (rk_usz i = 0; i < RK_COMMAND_LEX.names_len; i += 1) {
        if (strcmp(first, RK_COMMAND_LEX.names[i]) != 0) continue;
        RkActionLex lex = rk_analyze_lex_command(diag, argc - 2, &argv[2]);
        return (RkAction){.kind = RK_ACTION_LEX, .lex = lex};
    }

    rk_diag_unknown_command(diag, first);
    rk_diag_flush_and_exit(diag, -1);
}

////////////////////////////////////////
// Main

// BUILD: clang -Wall -Wextra -Wno-unused-function risk.c -o risk.exe

static inline
void rk_console_init() {
    if(!setlocale(LC_ALL, "en_US.UTF-8")) {
        setlocale(LC_ALL, "");
    }

    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
        SetConsoleCP(CP_UTF8);
    #endif
}

rk_i32 main(
    rk_i32 const argc,
    char const * const * const argv
) {
    rk_console_init();
    RkDiag diag = RK_DIAG_EMPTY;
    RkAction action = rk_analyze_args(&diag, argc, argv);
    RK_TODO("");
    return 0;
}

#endif // RISK_H
