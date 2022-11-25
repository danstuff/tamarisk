#include "include/vector.h"
#include "include/log.h"

int mdynamic_size = 0;

mvector* mvector_create(u_int32_t capacity, mspan span)
{
    mvector* vec = (mvector*) malloc(sizeof(mvector));
    vec->capacity = capacity;
    vec->length = 0;
    vec->flags = span;

    vec->data = malloc(vec->capacity);

    mdynamic_size += sizeof(mvector) + vec->capacity;

    return vec;
}

void mvector_destroy(mvector* vec)
{
    mdynamic_size -= sizeof(mvector) + vec->capacity;

    free(vec->data);
    free(vec);
}

void mvector_resize(mvector* vec, u_int32_t new_capacity)
{
    u_int32_t old_capacity = vec->capacity;
    vec->capacity = new_capacity;
    vec->length = vec->length > new_capacity ? new_capacity : vec->length;

    vec->data = realloc(vec->data, vec->capacity);

    mdynamic_size += vec->capacity - old_capacity;
}

void mvector_set_length(mvector* vec, u_int32_t new_length)
{
   vec->length = new_length;
   if (vec->length > vec->capacity)
   {
        mvector_resize(vec, vec->length);
   }
}

mvector* mvector_from_void(void* initial, u_int32_t length, mspan span)
{
    mvector* vec = mvector_create(length, span);
    memcpy(vec->data, initial, length);
    vec->length = length;
    return vec;
}

mvector* mvector_from_cstr(const char* initial)
{
    return mvector_from_void((void*)initial, strlen(initial), sizeof(char));
}

const char* mvector_to_cstr(mvector* vec)
{
    if (vec->data[vec->length-1] != '\0')
    {
        mvector_set_length(vec, vec->length+1);
        vec->data[vec->length-1] = '\0';
        vec->length -= 1;
    }

    return vec->data;
}

void mvector_set(mvector* vec, u_int32_t index, mvector* val)
{
    memcpy(&vec->data[index*val->length], &val->data, val->length);
}

mvector* mvector_get(mvector* vec, u_int32_t index)
{
    return mvector_from_void(&vec->data[index*mspan(vec)], mspan(vec), mspan(vec));
}

mvector* mvector_copy(mvector* vec_from)
{
    return mvector_from_void((void*)vec_from->data, vec_from->length, mspan(vec_from));
}

void mvector_push(mvector* vec, void* data_in, u_int32_t length)
{
    u_int32_t initial_length = vec->length;
    mvector_set_length(vec, vec->length+length);
    memcpy(&vec->data[initial_length], data_in, length);
}

void mvector_pop(mvector* vec, void* data_out, u_int32_t length)
{
    mvector_set_length(vec, vec->length - length);
    memcpy(&vec->data[vec->length], data_out, length);
}

void mvector_push_cstr(mvector* vec, const char* cstr)
{
    mvector_push(vec, (void*)cstr, strlen(cstr));
}

void mvector_check_freed()
{
    if (mdynamic_size != 0)
    {
        merrf("Memory leak of %d bytes", mdynamic_size);
    }
}

void mvector_await_lock(mvector* vec, char thread_id)
{
    while (mmutex(vec));

    mmutex_set(vec, thread_id);
}

void mvector_unlock(mvector* vec, char thread_id)
{
    if (mmutex(vec) == thread_id)
    {
        mmutex_set(vec, 0);
    }
    else
    {
        merrf("Tried to unlock from thread %u when thread %u had lock.",
            thread_id, mmutex(vec));
    }
}

mvector* mvector_query()
{
    mvector* buffer = mvector_create(MSTRING_BUFFER_SIZE, sizeof(char));
    buffer->length = getline(&buffer->data, (size_t*)&buffer->capacity, stdin);
    return buffer;
}

void mvector_print_to(mvector* vec, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    u_int32_t len_d = vsprintf(&vec->data[vec->length], format, args);
    massert(vec->capacity > (vec->length + len_d));
    mvector_set_length(vec, vec->length + len_d);
}

#define _stringify(type, vec, str, l_brace, format, r_brace, m)\
{\
    massert(mspan(vec) == sizeof(type));\
    str->length = 0;\
    mvector_push_cstr(str, l_brace);\
    u_int32_t j = 0;\
    mvector_each(type, vec, element)\
    {\
        mvector_print_to(str, format, *element);\
        if (j % m == m-1 && j < mcount(vec)-1)\
        {\
            mvector_print_to(str, "\n");\
        }\
        j++;\
    }\
    mvector_push_cstr(str, r_brace);\
}

mvector* mvector_stringifyf(mvector* vec, u_int32_t m)
{
    mvector* str = mvector_create(MSTRING_BUFFER_SIZE, sizeof(char));
    _stringify(float, vec, str, "[ ", "%3.3f, ", "]", m);
    return str;
}

mvector* mvector_stringifyi(mvector* vec, u_int32_t m)
{
    mvector* str = mvector_create(MSTRING_BUFFER_SIZE, sizeof(char));
    _stringify(int, vec, str, "[ ", "%6f, ", "]", m);
    return str;
}

#undef _stringify

static u_int32_t _getj(u_int32_t i, u_int32_t m, u_int32_t n, double bstep)
{
    u_int32_t bi = (u_int32_t)floor(i / bstep + 0.5);
    return i + (i - bi*bstep) * (n - 1);
}

void mvector_transpose(mvector* matrix, u_int32_t m, u_int32_t n)
{
    if (mmin(m, n) == 1)
    {
        return;
    }

    double bstep = ((double)m*n - 1) / (n - 1);

    mvector* cache_a = mvector_create(mspan(matrix), mspan(matrix));
    mvector* cache_b = mvector_create(mspan(matrix), mspan(matrix));

    u_int32_t i = 1;
    u_int32_t ex = m*n-2;

    while (ex > 0)
    {
        u_int32_t j = _getj(i, m, n, bstep);
        ex--;

        if (i != j && ex > 0)
        {
            memcpy(cache_a->data, &matrix->data[i*mspan(matrix)], mspan(matrix));

            while (i != j && ex > 0)
            {
                memcpy(cache_b->data, &matrix->data[j*mspan(matrix)], mspan(matrix));
                memcpy(&matrix->data[j*mspan(matrix)], cache_a->data, mspan(matrix));
                memcpy(cache_a->data, cache_b->data, mspan(matrix));
                ex--;
                
                mvector_logff("%s", matrix, m*n);

                j = _getj(j, m, n, bstep);
            }

            memcpy(&matrix->data[i*mspan(matrix)], cache_a->data, mspan(matrix));
        }

        i++;
    }

    mvector_destroy(cache_a);
    mvector_destroy(cache_b);
}

mvector* mvector_bezierf(mvector* points, u_int32_t axes, float t)
{
    massert(mspan(points) == sizeof(float));

    u_int32_t cache_size = 0;
    u_int32_t step = axes*mspan(points);

    for (u_int32_t i = points->length; i >= step*2; i -= step)
    {
        cache_size += i;
    }

    mvector* cache = mvector_create(cache_size, mspan(points));
    mvector_push(cache, points->data, points->length);

    u_int32_t row_start = 0;
    u_int32_t row_length = points->length;

    while (row_length >= step*2)
    {
        for(u_int32_t i = row_start; i < row_start+row_length; i += step)
        {
            mvector* vec = mvector_get(cache, i);
            mvector* last_vec = NULL;

            if (last_vec)
            {   
                mvector_both(float, last_vec, vec, a, b)
                {
                    *a = mlerp(*a, *b, t);
                }

                mvector_push(cache, last_vec->data, last_vec->length);

                mvector_destroy(last_vec);
                last_vec = NULL;

                mvector_destroy(vec);
            }
            else
            {
                last_vec = vec;
            }
        }

        row_start += row_length;
        row_length -= step;
    }

    mvector* result = mvector_get(cache, cache->length-step);
    mvector_destroy(cache);

    return result;
}

#define _dot(type, mat_a, mat_b, m, n, p)\
massert(mspan(mat_a) == sizeof(type));\
massert(mspan(mat_b) == sizeof(type));\
mvector* mat_c = mvector_create(m*p*mspan(mat_a), mspan(mat_a));\
u_int8_t s = sizeof(type);\
u_int32_t k = 0;\
u_int32_t l = 0;\
mvector_each(type, mat_c, el_c)\
{\
    type c = 0;\
    for (u_int32_t j = 0; j < n*s; j += s)\
    {\
        c += (*(type*)&mat_a->data[j+l]) * (*(type*)&mat_b->data[j+k]);\
    }\
    memcpy(el_c, &c, s);\
    k += n*s;\
    if (k >= m)\
    {\
        k = 0;\
        l += n*s;\
    }\
}\
return mat_c;\

mvector* mvector_dotf(mvector* mat_a, mvector* mat_b,
    u_int32_t m, u_int32_t n, u_int32_t p)
{
    _dot(float, mat_a, mat_b, m, n, p);

}
mvector* mvector_doti(mvector* mat_a, mvector* mat_b,
    u_int32_t m, u_int32_t n, u_int32_t p)
{
    _dot(int, mat_a, mat_b, m, n, p);
}

#undef _dot
