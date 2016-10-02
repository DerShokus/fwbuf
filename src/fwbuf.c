/*
 * fwbuf.c
 * Copyright (C) 2016 dershokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#include <fwbuf.h>
#include <stdbool.h>

#include "fwbuf_node.h"

void fwbuf_clip(struct fwbuf *buf, struct fwbuf_node *node)
{
        assert(buf != NULL);
        assert(node != NULL);
        // TODO: check it        assert(node->next != NULL);

        if (buf->end == NULL) {
                buf->head = buf->end = node;
        } else {
                assert(buf->end->next == NULL);

                buf->end->next = node;
                buf->end       = node;
        }

        ++buf->node_count;
}

struct fwbuf_node *fwbuf_clip_new(struct fwbuf *buf)
{
        assert(buf != NULL);

        struct fwbuf_node *node = fwbuf_node_create(buf->chunk_size);
        if (node != NULL)
                fwbuf_clip(buf, node);

        return node;
}

struct fwbuf_node *fwbuf_node_last(struct fwbuf *deque)
{
        assert(deque != NULL);

        if (deque->head == NULL) {
                assert(deque->head == deque->end);
                fwbuf_clip_new(deque);
        }
        if (deque->head == NULL) {
                return NULL;
        }
        assert(deque->end != NULL);
        return deque->end;
}

size_t fwbuf_node_space(const struct fwbuf *buf, const struct fwbuf_node *node,
                        const uint8_t *pos)
{
        assert(buf != NULL);
        assert(node != NULL);
        // assert(buffer_contains_the_node(buf, node));

        const size_t size  = buf->chunk_size;
        const uint8_t *end = node->chunk + size;
        assert(end >= pos);
        const ptrdiff_t space =
            (end -
             pos); // a strange place (without brackets the result is negative)
        assert(space >= 0);
        return space;
}

size_t fwbuf_iterator_space(const struct fwbuf_iterator iterator)
{
        return fwbuf_node_space(iterator.container, iterator.node,
                                iterator.position);
}

size_t fwbuf_node_free_space(const struct fwbuf *buf,
                             const struct fwbuf_node *node)
{
        return fwbuf_node_space(buf, node, node->position);
}

size_t fwbuf_node_filled(const struct fwbuf_node *node)
{
        assert(node != NULL);
        assert(node->position != NULL);
        assert(node->position >= node->chunk);

        return node->position - node->chunk;
}

bool fwbuf_node_full(struct fwbuf *buf, struct fwbuf_node *node)
{
        return fwbuf_node_free_space(buf, node) == 0;
}

size_t min_sz(size_t a, size_t b) { return a > b ? b : a; }

bool fwbuf_push_back(struct fwbuf *buf, void *data, size_t data_size)
{
        assert(buf != NULL);
        assert(data != NULL);

        struct fwbuf_node *node        = fwbuf_node_last(buf);

        uint8_t *bytes = data;
        if (data_size == 0)
                return false;

        while (data_size) {
                // create new node if the last one is full
                if (fwbuf_node_full(buf, node)) {
                        node = fwbuf_clip_new(buf);
                }
                // push as much as posible
                const size_t push_size =
                    min_sz(data_size, fwbuf_node_free_space(buf, node));
                struct writable write = {.data = bytes, .size = push_size};
                fwbuf_node_push(node, write);
                // update data to push
                data_size -= push_size;
                bytes += push_size;
        }
        return true;
}

bool fwbuf_iterator_valid(struct fwbuf_iterator iterator)
{
        return iterator.container != NULL && iterator.position != NULL &&
               iterator.node != NULL;
}

bool fwbuf_emplace(struct fwbuf_iterator iterator, void *data, size_t size)
{
        const size_t node_size = fwbuf_node_space(
            iterator.container, iterator.node, iterator.position);
        uint8_t *bytes = data;
        // update data for the current node
        struct writable write = {.data = bytes,
                                 .size = min_sz(node_size, size)};
        fwbuf_node_emplace(iterator.node, iterator.position, write);
        // update the data parameters
        bytes += write.size;
        size -= write.size;
        // update all other nodes
        while (size > 0) {
                struct fwbuf_node *node = fwbuf_node_next(iterator.node);
                if (node) {
                        // emplace data to the node
                        write.data = bytes;
                        write.size =
                            min_sz(iterator.container->chunk_size, size);
                        fwbuf_node_emplace(node, node->chunk, write);
                        // update data parameters
                        assert(size >= write.size);
                        bytes += write.size;
                        size -= write.size;
                } else {
                        return fwbuf_push_back(iterator.container, bytes, size);
                }
        }

        return true;
}

struct fwbuf *fwbuf_create(size_t chunk_size)
{
        if (chunk_size == 0)
                return NULL;
        struct fwbuf init = {.head       = NULL,
                             .end        = NULL,
                             .chunk_size = chunk_size,
                             .node_count = 0};
        struct fwbuf *res = malloc(sizeof(init));
        if (res == NULL)
                return NULL;
        memcpy(res, &init, sizeof(init));

        return res;
}

struct fwbuf_node *fwbuf_node_first(struct fwbuf *buf)
{
        return buf ? buf->head : NULL;
}

size_t fwbuf_packed_size(struct fwbuf *buf)
{
        assert(buf != NULL);
        if (buf->node_count == 0)
                return 0;
        const size_t rough_size            = buf->chunk_size * buf->node_count;
        const struct fwbuf_node *last_node = fwbuf_node_last(buf);
        assert(last_node != NULL);
        const size_t packed_size =
            rough_size - fwbuf_node_free_space(buf, last_node);
        assert(packed_size <= rough_size);
        return packed_size;
}

void *fwbuf_pack_up(struct fwbuf *buf)
{
        assert(buf != NULL);

        const size_t buffer_size = fwbuf_packed_size(buf);
        void *buffer             = malloc(buffer_size);
        if (buffer == NULL)
                return NULL;

        uint8_t *p = buffer;
        for (struct fwbuf_node *node = fwbuf_node_first(buf); node != NULL;
             node                    = fwbuf_node_next(node)) {
                memcpy(p, node->chunk, fwbuf_node_filled(node));
                p += fwbuf_node_filled(node);
        }
        assert((p - (uint8_t *)buffer) == buffer_size);
        return buffer;
}

struct fwbuf_iterator fwbuf_iterator_next(struct fwbuf_iterator iterator,
                                          size_t shift)
{
        if (!fwbuf_iterator_valid(iterator))
                return iterator;
        const size_t node_space = fwbuf_iterator_space(iterator);
        const size_t node_shift = min_sz(node_space, shift);
        if (shift > node_shift) {
                shift -= node_shift;
                size_t skip_nodes = shift / iterator.container->chunk_size + 1;
                while (skip_nodes) {
                        iterator.node = fwbuf_node_next(iterator.node);
                        if (iterator.node == NULL) {
                                iterator.position = NULL;
                                break;
                        }
                        --skip_nodes;
                }
                shift -= shift / iterator.container->chunk_size;
                if (iterator.node != NULL && shift > 0) {
                        iterator.position = iterator.node->chunk + shift;
                }
        } else {
                iterator.position += shift;
        }

        return iterator;
}

struct fwbuf_iterator fwbuf_iterator_begin(struct fwbuf *buf)
{
    struct fwbuf_iterator iterator = { NULL };
    if (buf == NULL) return iterator;
    if (buf->node_count == 0)
    {
        fwbuf_clip_new(buf);
    }

    iterator.container = buf;
    iterator.node      = buf->head;
    iterator.position  = buf->head->chunk;

    return iterator;
}

void
fwbuf_delete(struct fwbuf **buffer) {
    if (buffer && *buffer) {
        struct fwbuf_node *node = fwbuf_node_first(*buffer);
        do {
            struct fwbuf_node *sentenced = node;
            node = fwbuf_node_next(node);
            free(sentenced);
        } while (node);
        free(*buffer);
        *buffer = NULL;
    }
}
