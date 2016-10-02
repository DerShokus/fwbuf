/*
 * run_tests.c
 * Copyright (C) 2016 dershokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <cmocka.h>
#include <limits.h>

#include <fwbuf.h>

static void create_zero_size(void **state)
{
    assert_null(fwbuf_create(0));
}

static void create(void **state)
{
    struct fwbuf *buffer = fwbuf_create(sizeof(uint64_t));
    assert_non_null(buffer);
    fwbuf_delete(&buffer);
    assert_null(buffer);
}

static void push_back_simple(void **state)
{
    struct fwbuf *buffer = fwbuf_create(sizeof(uint64_t));
    for (uint64_t i = 0; i < 1024; ++i) {
        assert_true(fwbuf_push_back(buffer, &i, sizeof(i)));
    }
    fwbuf_delete(&buffer);
    assert_null(buffer);
}

static void pack_up(void **state)
{
    struct fwbuf *buffer = fwbuf_create(sizeof(uint64_t));
    for (uint64_t i = 0; i < 1024; ++i) {
        assert_true(fwbuf_push_back(buffer, &i, sizeof(i)));
    }
    const size_t size = fwbuf_packed_size(buffer) / sizeof(uint64_t);
    assert_true(size == 1024);
    uint64_t *packed = fwbuf_pack_up(buffer);
    assert_non_null(packed);
    for (uint64_t i = 0; i < 1024; ++i, ++packed) {
        assert_true(i == *packed);
    }
    fwbuf_delete(&buffer);
    assert_null(buffer);
}

int main(int argc, char *argv[])
{
    struct fwbuf *buffer = NULL;

    const struct CMUnitTest tests[] = {
        cmocka_unit_test(create_zero_size),
        cmocka_unit_test(create),
        cmocka_unit_test(push_back_simple),
        cmocka_unit_test(pack_up),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}


