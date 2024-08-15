#ifndef HF_MACROS_H
#define HF_MACROS_H

#define HF_UNUSED(VALUE) ((void)(VALUE))

#define HF_MIN(A, B) (((A) < (B)) ? (A) : (B))
#define HF_MAX(A, B) (((A) > (B)) ? (A) : (B))

#define HF_ARRAY_LENGTH(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))

#endif