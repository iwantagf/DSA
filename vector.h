/*
    idea from: https://github.com/eteran/c-vector
*/
#ifndef VECTOR_H_
#define VECTOR_H_
#endif

//free
#ifndef vector_lib_free
#include <stdlib.h>
#define vector_lib_free free
#endif

//malloc
#ifndef vector_lib_malloc
#include <stdlib.h>
#define vector_lib_malloc malloc
#endif

//realloc
#ifndef vector_lib_realloc
#include <stdlib.h>
#define vector_lib_realloc realloc
#endif

//calloc
#ifndef vector_lib_calloc
#include <stdlib.h>
#define vector_lib_calloc calloc
#endif

//memcpy
#ifndef vector_lib_memcpy
#include <string.h>
#define vector_lib_memcpy memcpy
#endif

//memmove
#ifndef vector_lib_memmove
#include <string.h>
#define vector_lib_memmove memmove
#endif

#ifndef vector_lib_assert
#include <assert.h>
#define vector_lib_assert assert
#endif

//cap phat bo nho dong = vector

typedef void (*vector_element_destructor)(void *elem_ptr); // dung de giai phong bo nho cho con tro


// cac du lieu trong vector
typedef struct vector_metadata {
    size_t size;
    size_t capacity;
    vector_element_destructor elem_destructor;
} vector_metadata;

#define swap(x, y) \
    do {\
        typeof(x) _tmp = (x); \
        (x) = (y); \
        (y) = _tmp; \
    } while (0)


/*
khai bao vector cho 1 kieu du lieu
neu la int co the dung vector(int) chang han <33333
*/
#define vector_type(type) type*
#define vector(type) vector_type(type)
#define vector_iterator(type) vector(type)


/*
    Chuyen doi giua con tro tro toi cac vector_metadata va con tro tro toi vector
*/
#define vector_to_base(vec) (&((vector_metadata*)(vec))[-1])
#define base_to_vector(base) ((void*) &((vector_metadata*)(base))[1])


#define vector_capacity(vec) \
    ((vec) ? vector_to_base(vec)->capacity : (size_t) 0)

#define vector_size(vec) \
    ((vec) ? vector_to_base(vec)->size : (size_t) 0)

#define vector_empty(vec) \
    (vector_to_base(vec)->size == 0)

#define vector_elem_destructor(vec) \
    ((vec) ? vector_to_base(vec)->elem_destructor : NULL)

//con tro ve vi tri dau tien
#define vector_begin(vec) \
    (vec)

//con tro ve vi tri dang sau vi tri cuoi cung
#define vector_end(vec) \
    ((vec) ? &(vec[vector_size(vec)]) : NULL)

//set size
#define vector_set_size(vec, n) \
    do {\
        if (vec) \
            vector_to_base(vec)->size = (size_t) n;\
    } while (0)

//set capacity
#define vector_set_capacity(vec, n) \
    do {\
        if (vec) \
            vector_to_base(vec)->capacity = n;\
    } while (0)

//viet ham de free cac phan tu
#define vector_set_elem_destructor(vec, n) \
    do {\
        if (vec) \
            vector_to_base(vec)->elem_destructor = n; \
    } while (0)

//moi lan capacity day, ta nhan doi so luong phan tu
#define next_capacity(size) \
    ((size) ? (size << 1) : 1)


//tang size cua vector len n
#define vector_grow(vec, n) \
    do {\
        const size_t sz = n * sizeof(*vec) + sizeof(vector_metadata); \
        if (vec) {\
            void *p1 = vector_to_base(vec); \
            void *p2 = vector_lib_realloc(p1, sz); \
            vector_lib_assert(p2); \
            (vec) = base_to_vector(p2); \
        } \
        else { \
            void *p = vector_lib_malloc(sz); \
            vector_lib_assert(p); \
            (vec) = base_to_vector(p); \
            vector_set_size((vec), 0); \
            vector_set_elem_destructor((vec), NULL); \
        }\
        vector_set_capacity(vec, n); \
    } while (0)


//chuyen capacity = size
#define shrink_to_fit(vec) \
    do {\
        if (vec) {\
            const size_t sz = vector_size(vec); \
            vector_grow(vec, sz); \
        } \
    } while (0)

//copy tu vector from sang vector to
#define vector_copy(from, to) \
    do {\
        if ((from)) {\
            vector_grow(to, vector_size(from)); \
            vector_set_size(to, vector_size(to)); \
            vector_lib_memcpy(to, from, vector_size(from) * sizeof(*(from))); \
        } \
    } while (0)

//lam cho capacity cua vector >= n
#define vector_reserve(vec, n) \
    do {\
        const size_t sz = vector_size(vec); \
        if (sz < n) \
            vector_grow(vec, n); \
    } while (0)

//vector::push_back()
#define vector_push_back(vec, val) \
    do {\
        size_t cap_ = vector_capacity(vec); \
        if (cap_ <= vector_size(vec)) { \
            vector_grow((vec), next_capacity(cap_)); \
        } \
        (vec)[vector_size(vec)] = val; \
       vector_set_size((vec), vector_size(vec) + 1); \
    } while (0)

#define vector_pop_back(vec) \
    do {\
        vector_element_destructor elem_destructor = vector_elem_destructor(vec); \
        if (elem_destructor) {\
            elem_destructor(&(vec)[vector_size(vec) - 1]); \
        } \
        vector_set_size(vec, vector_size(vec) - 1); \
    } while (0)

//vector::clear()
#define vector_clear(vec) \
    do { \
        if (vec) {\
            vector_element_destructor elem_destructor_ = vector_elem_destructor(vec); \
            if (elem_destructor_) {\
                size_t i__; \
                for (i__ = 0; i__ < vector_size(vec); ++i__) {\
                    elem_destructor_(&(vec)[i__]); \
                } \
            } \
            vector_set_size(vec, 0); \
        } \
    } while (0)

//init vector
#define vector_init(vec, cap, elem_des) \
    do {\
        if (!(vec)) { \
            vector_reserve((vec), cap); \
            vector_set_elem_destructor((vec), (elem_des)); \
        } \
    } while (0)

//vector::resize
#define vector_resize(vec, sz, val) \
    do {\
        if (vec) {\
            size_t sz_ = (size_t)(sz); \
            size_t vec_size = vector_to_base(vec)->count;\
            if (sz_ > vec_size) { \
                vector_reserve((vec), sz_); \
                vector_set_size((vec), sz_); \
                do { \
                    (vec)[vec_size++] = val; \
                } while (vec_size < sz_); \
            } \
            else { \
                while (sz_ < vec_size--) {\
                    vector_pop_back(vec); \
                }\
            } \
        } \
    } while (0)

//giai phong vung nho cua 1 vector
#define vector_free(vec) \
    do {\
        if (vec) { \
            void *p1 = vector_to_base(vec); \
            vector_element_destructor A = vector_elem_destructor(vec); \
            if (A) {\
                size_t pos; \
                for (pos = 0; pos < vector_size(vec); ++pos) \
                    A(&(vec)[pos]); \
            } \
            vector_lib_free(p1); \
        } \
    } while (0)

#define vector_at(vec, n) \
    ((vec) ? (((int) n < 0 || (size_t) n >= vector_size(vec)) ? NULL : &(vec)[n]) : NULL)

#define vector_front(vec) \
    ((vec) ? ((vector_size(vec) > 0) ? vector_at(vec, 0) : NULL) : NULL)

#define vector_back(vec) \
    ((vec) ? ((vector_size(vec) > 0) ? vector_at(vec, vector_size(vec) - 1) : NULL) : NULL)

// for (int i : vec)
#define vector_for_each_in(it, vec) \
    for (it = vector_begin(vec); it < vector_end(vec); it++)

#define vector_for_each(vec, func) \
    do { \
        if ((vec) && (func) != NULL) { \
            size_t pos; \
            for (pos = 0; pos < vector_size(vec); pos++) \
                func(pos); \
        } \
    } while (0)
