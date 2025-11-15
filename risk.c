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

#define RK_I8_MIN        -128i8
#define RK_I16_MIN       -32768i16
#define RK_I32_MIN       -2147483648i32
#define RK_I64_MIN       -9223372036854775808i64

#define RK_I8_MAX        127i8
#define RK_I16_MAX       32767i16
#define RK_I32_MAX       2147483647i32
#define RK_I64_MAX       9223372036854775807i64

#define RK_U8_MAX        0xffui8
#define RK_U16_MAX       0xffffui16
#define RK_U32_MAX       0xffffffffui32
#define RK_U64_MAX       0xffffffffffffffffui64

#define RK_ISZ_MIN       RK_I64_MIN
#define RK_ISZ_MAX       RK_I64_MAX
#define RK_USZ_MAX       RK_U64_MAX

////////////////////////////////////////
// Colors

#define RK_CLEAN              "\e[0m"
#define RK_BOLD               "\e[1m"
#define RK_ITALIC             "\e[3m"
#define RK_UNDERLINE          "\e[4m"

#define RK_RED_BOLD           "\e[1;31m"
#define RK_GREEN_BOLD         "\e[1;32m"
#define RK_YELLOW_BOLD        "\e[1;33m"
#define RK_ORANGE_BOLD        "\e[1;34m"
#define RK_MAGENTA_BOLD       "\e[1;35m"
#define RK_CYAN_BOLD          "\e[1;36m"
#define RK_WHITE_BOLD         "\e[1;37m"

#define RK_RED_BOLD_ITALIC     "\e[1;3;31m"
#define RK_GREEN_BOLD_ITALIC   "\e[1;3;32m"
#define RK_YELLOW_BOLD_ITALIC  "\e[1;3;33m"
#define RK_ORANGE_BOLD_ITALIC  "\e[1;3;34m"
#define RK_MAGENTA_BOLD_ITALIC "\e[1;3;35m"
#define RK_CYAN_BOLD_ITALIC    "\e[1;3;36m"
#define RK_WHITE_BOLD_ITALIC   "\e[1;3;37m"

////////////////////////////////////////
// Caller

typedef struct {
    char const *file;
    rk_u32 line;
} RK_Caller;

static void
RK_caller_println(RK_Caller caller) {
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
    } while (0)

#define RK_TODO(fmt, args...)                                                         \
    do {                                                                              \
        if (sizeof(fmt) <= 1) RK_FAILED("todo", "%s not implemented yet!", __func__); \
        else RK_FAILED("todo", fmt, args);                                            \
    } while (0)

#define RK_UNIMPLEMENTED(fmt, args...)                      \
    do {                                                    \
        if (sizeof(fmt) <= 1) RK_PANIC("not implemented!"); \
        else RK_FAILED("unimplemented", fmt, args);         \
    } while (0)

#define RK_PANIC(fmt, args...)                                             \
    do {                                                                   \
        if (sizeof(fmt) <= 1) RK_FAILED("panic", "something went wrong!"); \
        else RK_FAILED("panic", fmt, args);                                \
    } while (0)

#define RK_UNREACHABLE(fmt, args...)                                \
    do {                                                            \
        if (sizeof(fmt) <= 1) RK_PANIC("reached the unreachable!"); \
        else RK_FAILED("unreachable", fmt, args);                   \
    } while (0)

#define RK_FAILED(type, fmt, args...)                  rk_failed(RK_CALLER_HERE, type, fmt, args)
#define RK_FAILED_ASSERT(expr, expr_len, fmt, args...) rk_failed_assert(RK_CALLER_HERE, expr, expr_len, fmt, args)

static inline rk_u32
RK_decimal_len(rk_usz x) {
    rk_u32 n = 1;
    while (x >= 10) { x /= 10; n += 1; }
    return n;
}

static noreturn void
rk_failed(
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
    RK_caller_println(caller);
    exit(1);
}

static noreturn void
rk_failed_assert(
    RK_Caller caller,
    char const *expr,
    rk_usz len,
    char const *fmt, ...
) {
    rk_u32 num_len = RK_decimal_len(caller.line);

    printf(RK_RED_BOLD "assert" RK_WHITE_BOLD ": ");

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    printf("\n");

    printf("%*s", num_len - 1, "");
    RK_caller_println(caller);

    printf(RK_CYAN_BOLD "%*s |\n", num_len, "");
    printf("%u | " RK_MAGENTA_BOLD "RK_ASSERT(%s, ...)\n", caller.line, expr);
    printf(RK_CYAN_BOLD "%*s |             " RK_RED_BOLD, num_len, "");
    for (rk_usz i = 0; i < len; i += 1) putchar('^');
    printf(" must be true\n\n" RK_CLEAN);
    exit(1);
}

////////////////////////////////////////
// Allocations

#include <malloc.h>
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
    static NAME                                                             \
    PREFIX##_alloc(rk_usz cap) {                                            \
        NAME buf = {0};                                                     \
        RK_LIST_ALLOC(buf.ptr, buf.len, buf.cap, cap);                      \
        return buf;                                                         \
    }                                                                       \
                                                                            \
    static inline void                                                      \
    PREFIX##_dealloc(NAME buf) {                                            \
        RK_LIST_DEALLOC(buf.ptr);                                           \
    }                                                                       \
                                                                            \
    static void                                                             \
    PREFIX##_reserve(NAME *buf, rk_usz add) {                               \
        RK_LIST_RESERVE(buf->ptr, buf->len, buf->cap, add);                 \
    }                                                                       \
                                                                            \
    static SLICE                                                            \
    PREFIX##_extend(NAME *buf, SLICE slice) {                               \
        rk_usz start = buf->len;                                            \
        RK_LIST_EXTEND(buf->ptr, slice.ptr, buf->len, buf->cap, slice.len); \
        return (SLICE){.ptr = &buf->ptr[start], .len = slice.len};          \
    }                                                                       \
                                                                            \
    static INDEXED                                                          \
    PREFIX##_extend_indexed(NAME *buf, SLICE slice) {                       \
        INDEX start = buf->len;                                             \
        PREFIX##_extend(buf, slice);                                        \
        return (INDEXED){.start = start, .len = slice.len};                 \
    }                                                                       \
                                                                            \
    static SLICE                                                            \
    PREFIX##_slice(NAME const *buf, rk_usz start, rk_usz len) {             \
        RK_ASSERT(start + len <= buf->len, "");                             \
        return (SLICE){.ptr = &buf->ptr[start], .len = len};                \
    }                                                                       \
                                                                            \
    static void                                                             \
    PREFIX##_push(NAME *buf, TYPE v) {                                      \
        RK_LIST_PUSH(buf->ptr, buf->len, buf->cap, v);                      \
    }                                                                       \
                                                                            \
    static INDEX                                                            \
    PREFIX##_push_id(NAME *buf, TYPE v) {                                   \
        RK_ASSERT(buf->len < INDEX_MAX, #NAME " overflow");                 \
        INDEX id = (INDEX)buf->len;                                         \
        RK_LIST_PUSH(buf->ptr, buf->len, buf->cap, v);                      \
        return id;                                                          \
    }                                                                       \
                                                                            \
    static TYPE                                                             \
    PREFIX##_pop(NAME *buf) {                                               \
        RK_ASSERT(buf->len != 0, #NAME " is empty");                        \
        buf->len -= 1;                                                      \
        return buf->ptr[buf->len];                                          \
    }                                                                       \
                                                                            \
    static void                                                             \
    PREFIX##_pop_n(NAME *buf, rk_usz n) {                                   \
        RK_ASSERT(buf->len >= n, #NAME " not have %llu items", n);          \
        buf->len -= n;                                                      \
    }                                                                       \
                                                                            \
    static inline TYPE                                                      \
    PREFIX##_at(NAME const *buf, rk_usz index) {                            \
        RK_ASSERT(index < buf->len, "index out of bounds");                 \
        return buf->ptr[index];                                             \
    }                                                                       \
                                                                            \
    static inline TYPE *                                                    \
    PREFIX##_at_mut(NAME *buf, rk_usz index) {                              \
        RK_ASSERT(index < buf->len, "index `%llu` out of bounds", index);   \
        return &buf->ptr[index];                                            \
    }                                                                       \
                                                                            \
    static inline INDEX                                                     \
    PREFIX##_id(NAME const *buf) {                                          \
        RK_ASSERT(buf->len < INDEX_MAX, #NAME " overflow");                 \
        return (INDEX)buf->len;                                             \
    }                                                                       \

////////////////////////////////////////
// String

RK_LIST(
    RkStrBuf, RkStrRef, RkStrIdx,
    rk_sb, char, rk_usz, RK_USZ_MAX,
)

static inline bool
rk_ch_is_space(rk_u8 c) {
    return c == ' ' || c == '\t'|| c == '\r' || c == '\n';
}

static inline void
rk_sb_strip_right(RkStrBuf *buf) {
    for (;;) {
        if (buf->len == 0) break;
        rk_u8 c = buf->ptr[buf->len - 1];
        if (!rk_ch_is_space(c)) break;
        buf->len -= 1;
    }
}

static void
rk_sb_vprintf(RkStrBuf *buf, char const *fmt, va_list args) {
    int len = vsnprintf(NULL, 0, fmt, args);
    rk_sb_reserve(buf, len + 1);
    vsnprintf(&buf->ptr[buf->len], len + 1, fmt, args);
    buf->len += len;
}

static void
rk_sb_printf(RkStrBuf *buf, char const *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    rk_sb_vprintf(buf, fmt, args);
    va_end(args);
}

static void
rk_sb_vprintf_repeat(RkStrBuf *buf, rk_u32 n, char const *fmt, va_list args) {
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

static void
rk_sb_printf_repeat(RkStrBuf *buf, rk_u32 n, char const *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    rk_sb_vprintf_repeat(buf, n, fmt, args);
    va_end(args);
}

static inline void
rk_sb_flush(RkStrBuf *buf, FILE *stream) {
    if (buf->len == 0) return;
    fprintf(stream, "%.*s", (rk_u32)buf->len, buf->ptr);
    buf->len = 0;
}

////////////////////////////////////////
// File & Path

#include <direct.h>
#include <string.h>

typedef struct {
    char  *ptr;
    rk_usz len;
    rk_usz cap;
} RkPathBuf;

#define RK_PB_EMPTY (RkPathBuf){.ptr = NULL, .len = 0, .cap = 0}

typedef enum {
    RK_FILE_OK,
    RK_FILE_PERMISSION_DENIED,
    RK_FILE_EXPECTED_FILE,
    RK_FILE_NOT_FOUND,
    RK_FILE_UNKNOWN_ERROR,
} RkFileResult;

typedef struct {
    rk_u8 *ptr;
    rk_usz len;
} RkBytes;

static inline RkStrBuf
rk_sb_from_bytes(RkBytes bytes) {
    return (RkStrBuf){
        .ptr = (void*)bytes.ptr,
        .len = bytes.len,
        .cap = bytes.len,
    };
}

typedef struct {
    RkFileResult result;
    RkBytes bytes;
} RkFile;

static char *
rk_file_result_as_cstr(RkFileResult kind) {
    switch (kind) {
        case RK_FILE_OK:             return "ok";
        case RK_FILE_PERMISSION_DENIED: return "permission denied";
        case RK_FILE_EXPECTED_FILE:     return "expected file, but found directory";
        case RK_FILE_NOT_FOUND:         return "file not found";
        case RK_FILE_UNKNOWN_ERROR:     return "failed read file (unknown reason)";
    }
}

static RkFileResult
file_result_from_errno(errno_t err) {
    switch(err) {
        case 0:      return RK_FILE_OK;
        case EPERM:  return RK_FILE_PERMISSION_DENIED;
        case ENOENT: return RK_FILE_NOT_FOUND;
        case EACCES: return RK_FILE_EXPECTED_FILE;
        default:     return RK_FILE_UNKNOWN_ERROR;
    }
}

static inline FILE *
open_file_or_failed(char const *path) {
    FILE *file;
    errno_t err = fopen_s(&file, path, "wb");
    RK_ASSERT(err == 0 && file != NULL, "failed open file");
    return file;
}

static inline void
rk_file_save(
    char const * const path,
    rk_u8 const * const buf,
    rk_usz const len
) {
    FILE *file;

    errno_t err = fopen_s(&file, (char *)path, "wb");
    RK_ASSERT(err == 0 && file != NULL, "failed open file");
    
    size_t bytes_written = fwrite(buf, 1, len, file);
    RK_ASSERT(bytes_written == len, "failed write in file");
    
    fclose(file);
}

static RkFile
rk_file_load(char const *path) {
    FILE *file = NULL;
    errno_t err = fopen_s(&file, path, "rb");

    RkFileResult kind = file_result_from_errno(err);
    if (kind != RK_FILE_OK) return (RkFile){.result = kind, .bytes = {0}};

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
    return (RkFile){.result = RK_FILE_OK, .bytes = bytes};
}

static void
rk_print_error(
    char const *path,
    rk_u32 const line,
    rk_u32 const column,
    char const *fmt, ...
) {
    printf(RK_RED_BOLD "error" RK_WHITE_BOLD ": ");

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
    
    printf("\n" RK_CYAN_BOLD " --> %s", path);
    if (line != 0 && column != 0) printf(":%hu:%hu", line, column);
    printf("\n" RK_CLEAN);
}

static inline RkBytes
rk_file_load_or_exit(char const *path) {
    RkFile load = rk_file_load(path);
    if (load.result == RK_FILE_OK) return load.bytes;
    rk_print_error(path, 0, 0, "%s", rk_file_result_as_cstr(load.result));
    printf("\n");
    exit(1);
}

static inline RkPathBuf
rk_pb_from_cstr(char const *ptr) {
    RkPathBuf path = RK_PB_EMPTY;
    RK_LIST_EXTEND(path.ptr, ptr, path.len, path.cap, strlen(ptr) + 1);
    return path;
}

static void
rk_pb_join(RkPathBuf *path, RkStrRef add) {
    if (add.len == 0) return;

    for (rk_usz i = 0; i <= add.len; i += 1) {
        RK_ASSERT(add.ptr[i] != '\0', "unexpected NULL in slice");
    }

    rk_usz reserve_len = add.len + 1;
    if (path->len > 0) {
        path->len -= 1;   // strip NULL
        reserve_len += 1; // add slash
    }
    RK_LIST_RESERVE(path->ptr, path->len, path->cap, reserve_len);

    if (path->len > 0) RK_LIST_PUSH(path->ptr, path->len, path->cap, '\\');
    RK_LIST_EXTEND(path->ptr, add.ptr, path->len, path->cap, add.len);
    RK_LIST_PUSH(path->ptr, path->len, path->cap, '\0');
}

static inline char const *
rk_pb_tail(RkPathBuf const *path, rk_usz n) {
    RK_ASSERT(path->len > 0, "expected non-empty path");
    RK_ASSERT(n > 0, "expected at least 1 component");

    char const * const start = &path->ptr[0];
    char const *peek = &path->ptr[path->len];

    for (;;) {
        peek -= 1;
        if (peek <= start) return start;
        if (*peek == '\\' || *peek == '/') n -= 1;
        if (n == 0) return peek + 1;
    }
}

static inline void
rk_pb_dealloc(RkPathBuf path) {
    RK_LIST_DEALLOC(path.ptr);
}

// BUILD: clang -Wall -Wextra -Wno-unused-function risk.c -o risk.exe

rk_i32
main(void) {
    RkStrBuf buf = rk_sb_alloc(RK_PAGE_SIZE);
    rk_sb_printf(&buf, RK_RED_BOLD "RISK" RK_WHITE_BOLD " is " RK_GREEN_BOLD_ITALIC "self-known" RK_CLEAN "\n");
    // RkPathBuf path = rk_pb_from_cstr("examples/main.rk");
    RkBytes bytes = rk_file_load_or_exit("examples/main.rk");
    RkStrBuf src = rk_sb_from_bytes(bytes);
    rk_sb_strip_right(&src);
    rk_sb_printf(&buf, RK_MAGENTA_BOLD "```\n%.*s\n```" RK_CLEAN, (rk_u32)src.len, src.ptr);
    rk_sb_flush(&buf, stdout);
    rk_sb_dealloc(buf);
}

#endif // RISK_H
