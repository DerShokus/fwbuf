/*
 * deque.h
 * Copyright (C) 2016 dershokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef DEQUE_H
#define DEQUE_H

#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>
#include <stddef.h>



bool check_position(struct deque_buffer *buffer,
                    struct deque_buffer_node *node,
                    uint8_t *position)
{
        if (buffer == NULL || node == NULL || position == NULL)
                return false;

        const size_t chunk_size = buffer->chunk_size;
        const uint8_t *chunk    = node->chunk;

        if (chunk > position || position > (chunk + chunk_size))
                return false;

        return true;
}


void deque_write(struct deque_buffer *buffer, struct deque_buffer_node *node,
                 uint8_t *position, void *data, size_t size)
{
        assert(buffer != NULL);
        assert(node != NULL);
        assert(data != NULL);
        assert(size != 0);
        assert(position != NULL);
        assert(position >= node->chunk);

        uint8_t *data_b = data;
        while (size > 0) {
            size_t portion = deque_node_space(buffer, node, position);
            deque_node_write(node, position, data_b, portion);
            size -= portion;
            node = deque_buffer_node_next(node);
            data_b += portion;
            if (node == NULL)
            {
              deque_buffer_push_back(buffer, data_b, size);
              return;
            }
            position = node->chunk;
        }
}

bool deque_update(struct deque_iterator *iterator, void *data, size_t size)
{
        if (iterator == NULL || data == NULL || iterator->node == NULL ||
            iterator->container == NULL || iterator->position == NULL)
                return false;
        if (size == 0)
                return true;
        if (!check_position(iterator->container, iterator->node,
                            iterator->position))
                return false;
        deque_write(iterator->container, iterator->node, iterator->position, data, size);

        return true;
}

#endif /* !DEQUE_H */
