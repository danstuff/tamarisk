#ifndef VECTOR_H
#define VECTOR_H

#include "global.h"

#define MMATH_E 2.71828
#define MMATH_PI 3.14159

#define MSTRING_BUFFER_SIZE 1024
#define MSTRING_BUFFER_STEP 32

#define MFLAG_ANY 0xFF

#define MFLAG_SPAN 0x0F
#define MFLAG_MUTEX 0xF0

#define mmin(a, b) (a < b ? a : b)

#define mmax(a, b) (a > b ? a : b)

#define mlerp(a, b, t) ((1 - t) * b) + (t * a)

#define msig(x) (1 / (1 + pow(MMATH_E, -x)))

#define mtrap(m, n, d) (m + n) * d / 2

#define mspan(vec) (vec->flags & MFLAG_SPAN)

#define mcount(vec) (vec->length / mspan(vec))

#define mback(vec, i) (mcount(vec) - i)

#define mmutex(vec) ((vec->flags & MFLAG_MUTEX) >> 4)

#define mmutex_set(vec, mut) vec->flags = (vec->flags & MFLAG_SPAN) | (mut << 4)

#define mvector_from(type, ...)\
mvector_from_void((type[]){__VA_ARGS__}, sizeof((type[]){__VA_ARGS__}), sizeof(type))

#define mvector_at(type, vec, index) (type*)&vec->data[index*mspan(vec)]

#define mvector_cast(type_to, type_from, vec)\
for (u_int32_t index = 0; index < vec->length; index += mspan(vec))\
{\
    *(type_to*)&vec->data[index] = (type_to)*(type_from*)&vec->data[index];\
}

#define mvector_each(type, vec, element)\
for (type* element = (type*)vec->data;\
    element < (type*)(vec->data + vec->length);\
    element++)

#define mvector_both(type, vec_a, vec_b, a, b)\
massert(mspan(vec_a) == mspan(vec_b));\
massert(mspan(vec_a) == sizeof(type));\
massert(vec_a->length % vec_b->length == 0);\
type* a;\
type* b;\
for (a = (type*)&vec_a->data[0],\
    b = (type*)&vec_b->data[0];\
    a <= (type*)(&vec_a->data[vec_a->length]) - 1;\
    a++, b = (type*)(b + 1 % mcount(vec_b)))

#define mvector_set_identity(type, mat, m)\
{\
    memset(mat->data, 0, mat->length);\
    for (u_int32_t i = 0; i < mat->length; i += m+1)\
    {\
        *(type*)&mat->data[i*mspan(mat)] = (type)1;\
    }\
}

#define mvector_logff(format, vec, m)\
{\
    mvector* str = mvector_stringifyf(vec, m);\
    mlogf(format, mvector_to_cstr(str));\
    mvector_destroy(str);\
}

typedef enum _mspan
{
    MSPAN_8 = 1,
    MSPAN_16 = 2,
    MSPAN_32 = 4,
    MSPAN_64 = 8,
} mspan;

typedef struct _mvector
{
    char* data;
    u_int32_t length;
    u_int32_t capacity;
    char flags;
} mvector;

mvector* mvector_create(u_int32_t count, mspan span);
void mvector_destroy(mvector* vec);

void mvector_resize(mvector* vec, u_int32_t new_capacity);
void mvector_set_length(mvector* vec, u_int32_t new_length);

mvector* mvector_from_void(void* initial, u_int32_t length, mspan span);

mvector* mvector_from_cstr(const char* initial);
const char* mvector_to_cstr(mvector* vec);
void mvector_append_cstr(mvector* vec, const char* cstr);

void mvector_copy(mvector* vec_to, mvector* vec_from,
    u_int32_t i, u_int32_t j, u_int32_t count);

mvector* mvector_sub(mvector* vec_from, u_int32_t i, u_int32_t count);

void mvector_check_freed();

void mvector_await_lock(mvector* vec, char thread_id);
void mvector_unlock(mvector* vec, char thread_id);

mvector* mvector_query();
void mvector_print_to(mvector* vec, const char* format, ...);

mvector* mvector_stringifyf(mvector* vec, u_int32_t m);
mvector* mvector_stringifyi(mvector* vec, u_int32_t m);

void mvector_transpose(mvector* matrix, u_int32_t m, u_int32_t n);

mvector* mvector_bezierf(mvector* points, u_int32_t axes, float t);

mvector* mvector_dotf(mvector* mat_a, mvector* mat_b,
    u_int32_t m, u_int32_t n, u_int32_t p);
mvector* mvector_doti(mvector* mat_a, mvector* mat_b,
    u_int32_t m, u_int32_t n, u_int32_t p);

#endif
