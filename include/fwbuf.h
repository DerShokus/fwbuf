/*
 * fwbuf.h
 * Copyright (C) 2016 dershokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef FWBUF_H
#define FWBUF_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

struct fwbuf_node {
        struct fwbuf_node *next;
        uint8_t *position;
        uint8_t chunk[1]; // actually it's greater then one
};

struct fwbuf {
        struct fwbuf_node *head;
        struct fwbuf_node *end;
        const size_t chunk_size;
        size_t node_count;
};

struct fwbuf_iterator {
        struct fwbuf *container;
        struct fwbuf_node *node;
        uint8_t *position;
};

struct fwbuf *
fwbuf_create(size_t chunk_size);

void
fwbuf_delete(struct fwbuf **buffer);

bool
fwbuf_push_back(struct fwbuf *buf, void *data, size_t data_size);

void *
fwbuf_pack_up(struct fwbuf *buf);

size_t
fwbuf_packed_size(struct fwbuf *buf);

bool
fwbuf_emplace(struct fwbuf_iterator iterator, void *data, size_t size);

struct fwbuf_iterator
fwbuf_iterator_begin(struct fwbuf *buf);

struct fwbuf_iterator
fwbuf_iterator_next(struct fwbuf_iterator iterator, size_t shift);

#endif /* !FWBUF_H */
