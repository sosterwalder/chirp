// ==========
// Quickcheck
// ==========
//
// Fork of https://github.com/mcandre/qc by Andrew Pennebaker removing gc.h
// and changing almost everything.
//
// Forked at commit fc8e7f76af339fdd56546ad46cb9d97cd42ec5cb
//
//
// .. code-block:: cpp
//
//
// Project includes
// ================
//
// .. code-block:: cpp
//
#include "quickcheck.h"
#include "util.h"

// System includes
// ===============
//
// .. code-block:: cpp
//
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// Declarations
// ============
//
// .. c:function::
static
void
_ch_qc_free_mem(void);
//
//    Free memory allocations made during quickcheck test.

// .. c:var:: ch_qc_mem_track_t* _ch_qc_mem_track
//
//    List of memory allocations during current quickcheck test.
//
// .. code-block:: cpp
//
static ch_qc_mem_track_t* _ch_qc_mem_track = NULL;

// Definitions
// ===========
//
// Sglib Prototypes
// ----------------
//
// .. code-block:: cpp
//
SGLIB_DEFINE_LIST_FUNCTIONS( // NOCOV
    ch_qc_mem_track_t,
    CH_QC_MEM_TRACK_COMPARATOR,
    next
)

// Functions
// ---------
//
// .. c:function::
int
_ch_qc_for_all(
        ch_qc_prop property,
        size_t arglen,
        ch_qc_gen gs[],
        ch_qc_print ps[],
        size_t max_size
)
//    :noindex:
//
//    see: :c:func:`_ch_qc_for_all`
//
// .. code-block:: cpp
//
{
    size_t i, j;
    ch_buf* values;

    values = malloc(arglen * max_size);

    for (i = 0; i < 100; i++) {
        for (j = 0; j < arglen; j++) {
            gs[j](values + j * max_size);
        }

        bool holds = property(values);

        if (!holds) {
            printf("*** Failed!\n");

            for (j = 0; j < arglen; j++) {
                printf("Arg %02d: ", (int) j);
                ps[j](values + j * max_size);
                printf("\n");
            }
            _ch_qc_free_mem();
            free(values);
            return 0;
        }
        _ch_qc_free_mem();
    }

    printf("+++ OK, passed 100 tests.\n");

    free(values);
    return 1;
}

// .. c:function::
static
void
_ch_qc_free_mem(void)
//    :noindex:
//
//    see: :c:func:`_ch_qc_free_mem`
//
// .. code-block:: cpp
//
{
    ch_qc_mem_track_t* t;
    struct sglib_ch_qc_mem_track_t_iterator it;
    for(
            t = sglib_ch_qc_mem_track_t_it_init(
                &it,
                _ch_qc_mem_track
            );
            t != NULL;
            t = sglib_ch_qc_mem_track_t_it_next(&it)
    ) {
        free(t->data);
        free(t);
    }
    _ch_qc_mem_track = NULL;
}

// .. c:function::
void
_ch_qc_gen_array(
        ch_buf* data,
        ch_qc_gen g,
        size_t size
)
//    :noindex:
//
//    see: :c:func:`_ch_qc_gen_array`
//
// .. code-block:: cpp
//
{
    int len = rand() % 100;

    ch_buf* arr = malloc((size_t) len * size);
    ch_qc_mem_track_t* item = malloc(sizeof(ch_qc_mem_track_t));
    item->data = arr;
    item->count = len;
    item->size = size;
    sglib_ch_qc_mem_track_t_add(&_ch_qc_mem_track, item);

    size_t i;

    for (i = 0; i < (size_t) len; i++) {
        g(arr + i * size);
    }

    ch_qc_return(ch_qc_mem_track_t*, item);
}

// .. c:function::
void
ch_qc_gen_bool(ch_buf* data)
//    :noindex:
//
//    see: :c:func:`ch_qc_gen_bool`
//
// .. code-block:: cpp
//
{
    int b = rand() % 2 == 0;

    ch_qc_return(int, b);
}

// .. c:function::
void
ch_qc_gen_byte(ch_buf* data)
//    :noindex:
//
//    see: :c:func:`ch_qc_gen_byte`
//
// .. code-block:: cpp
//
{
    uint8_t c = rand();  // No need to mod overflow of unsigned char is
                         // is well defined.

    ch_qc_return(uint8_t, c);
}

// .. c:function::
void
ch_qc_gen_bytes(ch_buf* data)
//    :noindex:
//
//    see: :c:func:`ch_qc_gen_bytes`
//
// .. code-block:: cpp
//
{
    ch_qc_mem_track_t *item;

    ch_qc_gen_array((ch_buf*) &item, ch_qc_gen_byte, uint8_t);

    ch_qc_return(ch_qc_mem_track_t*, item);
}

// .. c:function::
void
ch_qc_gen_char(ch_buf* data)
//    :noindex:
//
//    see: :c:func:`ch_qc_gen_char`
//
// .. code-block:: cpp
//
{
    char c = (char)((rand() % 127) + 1);

    ch_qc_return(char, c);
}

// .. c:function::
void
ch_qc_gen_int(ch_buf* data)
//    :noindex:
//
//    see: :c:func:`ch_qc_gen_int`
//
// .. code-block:: cpp
//
{
    int i = rand();

    ch_qc_return(int, i);
}

// .. c:function::
void
ch_qc_gen_string(ch_buf* data)
//    :noindex:
//
//    see: :c:func:`ch_qc_gen_string`
//
// .. code-block:: cpp
//
{
    ch_qc_mem_track_t *item;

    ch_qc_gen_array((ch_buf*) &item, ch_qc_gen_char, char);
    if(item->count == 0) {
        ch_buf* arr = malloc(1);
        item = malloc(sizeof(ch_qc_mem_track_t));
        item->data = arr;
        item->count = 1;
        item->size = sizeof(char);
        sglib_ch_qc_mem_track_t_add(&_ch_qc_mem_track, item);
        *arr = 0;
    } else
        item->data[(item->count * item->size) - 1] = 0;

    ch_qc_return(ch_qc_mem_track_t*, item);
}

// .. c:function::
void
ch_qc_init(void)
//    :noindex:
//
//    see: :c:func:`ch_qc_init`
//
// .. code-block:: cpp
//
{
    srand((unsigned int) time(NULL));
}

// .. c:function::
void
ch_qc_print_byte(ch_buf* data)
//    :noindex:
//
//    see: :c:func:`ch_qc_print_byte`
//
// .. code-block:: cpp
//
{
    char out[3];
    uint8_t b = ch_qc_args(uint8_t, 0, uint8_t);
    ch_bytes_to_hex(&b, 1, out, 3);

    printf("%s", out);
}

// .. c:function::
void
ch_qc_print_bytes(ch_buf* data)
//    :noindex:
//
//    see: :c:func:`ch_qc_print_byte`
//
// .. code-block:: cpp
//
{
    char* out;
    ch_qc_mem_track_t* item = ch_qc_args(
        ch_qc_mem_track_t*,
        0,
        ch_qc_mem_track_t*
    );
    size_t out_size = item->count * 2 + 1;
    out = malloc(out_size);
    ch_bytes_to_hex(
        (uint8_t*) item->data,
        item->count,
        out,
        out_size
    );

    printf("%s", out);
    free(out);
}

// .. c:function::
void
ch_qc_print_bool(ch_buf* data)
//    :noindex:
//
//    see: :c:func:`ch_qc_print_bool`
//
// .. code-block:: cpp
//
{
    int b = ch_qc_args(int, 0, int);

    printf("%s", b ? "true" : "false");
}

// .. c:function::
void
ch_qc_print_char(ch_buf* data)
//    :noindex:
//
//    see: :c:func:`ch_qc_print_char`
//
// .. code-block:: cpp
//
{
    char c = ch_qc_args(char, 0, char);

    printf("'%c'", c);
}

// .. c:function::
void
ch_qc_print_int(ch_buf* data)
//    :noindex:
//
//    see: :c:func:`ch_qc_print_int`
//
// .. code-block:: cpp
//
{
    int i = ch_qc_args(int, 0, int);

    printf("%d", i);
}

// .. c:function::
void
ch_qc_print_string(ch_buf* data)
//    :noindex:
//
//    see: :c:func:`ch_qc_print_string`
//
// .. code-block:: cpp
//
{
     ch_qc_mem_track_t* item = ch_qc_args(
        ch_qc_mem_track_t*,
        0,
        ch_qc_mem_track_t*
    );

    printf("%s", item->data);
}
