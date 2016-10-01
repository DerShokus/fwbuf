/*
 * deque.c
 * Copyright (C) 2016 dershokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <stdio.h>
#include <assert.h>

#include <fwbuf.h>

// void deque_buffer_create_test(void)
//{
//    struct deque_buffer *deque = deque_buffer_create(10);
//    assert(deque != NULL);
//
//    struct deque_buffer_node *node = deque_buffer_node_create(deque);
//    assert(node != NULL);
//    assert(node->next == NULL);
//    assert(deque->head == deque->end);
//    assert(deque->head == node);
//
//    struct deque_buffer_node *second = deque_buffer_node_create(deque);
//    assert(second != NULL);
//    assert(second->next == NULL);
//    assert(deque->head == node);
//    assert(deque->end == second);
//    assert(node->next == second);
//
//    struct deque_buffer_node *third = deque_buffer_node_create(deque);
//    assert(third != NULL);
//    assert(third->next == NULL);
//    assert(deque->head == node);
//    assert(deque->end == third);
//    assert(node->next == second);
//    assert(second->next == third);
//}

// void deque_buffer_push_back_test(void)
//{
//    struct deque_buffer *deque = deque_buffer_create(10);
//    assert(deque != NULL);
//
//    int64_t data = 1LL;
//    deque_buffer_push_back(deque, &data, sizeof(data));
//    assert(deque->head == deque->end);
//    struct deque_buffer_node *first = deque->head;
//    assert(first->position == (first->chunk + sizeof(data)));
//    assert(*((int64_t*)first->chunk) == data);
//
//    data = 2LL;
//    deque_buffer_push_back(deque, &data, sizeof(data));
//    assert(deque->head == first);
//    assert(deque->head != deque->end);
//    struct deque_buffer_node *second = deque->end;
//
//    assert(first->position == (first->chunk + 10));
//    assert(second->position == (second->chunk + 6));
//
//    assert(*((int64_t*)first->chunk) == 1LL);
//
//    int64_t get;
//    uint8_t *p = (uint8_t*)&get;
//
//    p[0] = *(first->chunk+8);
//    p[1] = *(first->chunk+9);
//    p[2] = *(second->chunk);
//    p[3] = *(second->chunk+1);
//    p[4] = *(second->chunk+2);
//    p[5] = *(second->chunk+3);
//    p[6] = *(second->chunk+4);
//    p[7] = *(second->chunk+5);
//
//    assert(get == data);
//}

int main(void)
{
        //    deque_buffer_create_test();
        //    deque_buffer_push_back_test();

        struct fwbuf *buf = fwbuf_create(10);
        int64_t the_first = 0;
        struct fwbuf_iterator iterator =
            fwbuf_push_back(buf, &the_first, sizeof(the_first));
        for (int64_t i = 1; i < 1000; i++) {
            fwbuf_push_back(buf, &i, sizeof(i));
        }

        for (int64_t *p = fwbuf_pack_up(buf), i = 0;
             i < (fwbuf_packed_size(buf) / sizeof(i)); ++i) {
                assert(*p == i);
                ++p;
        }

        for (int64_t i = 999; i >= 0; --i)
        {
            fwbuf_emplace(iterator, &i, sizeof(i));
            iterator = fwbuf_iterator_next(iterator, sizeof(i));
        }

        for (int64_t *p = fwbuf_pack_up(buf), i = 999;
             i < (fwbuf_packed_size(buf) / sizeof(i)); --i) {
                assert(*p == i);
                ++p;
        }

        return 0;
}
