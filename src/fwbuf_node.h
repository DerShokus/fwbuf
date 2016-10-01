/*
 * fwbuf_node.c
 * Copyright (C) 2016 dershokus <lily.coder@gmail.com>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef FWBUF_SRC_FWBUF_NODE_H_
#define FWBUF_SRC_FWBUF_NODE_H_

#include <fwbuf.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

struct writable {
        uint8_t *data;
        size_t size;
};

struct fwbuf_node *fwbuf_node_next(struct fwbuf_node *node)
{
        return node ? node->next : NULL;
}

void fwbuf_node_write(struct fwbuf_node *node, uint8_t *position,
                      struct writable data);

//----------------------------------------------------------------------------//

void fwbuf_node_emplace(struct fwbuf_node *node, uint8_t *position,
                      struct writable data)
{
        assert(node != NULL);
        assert(position != NULL);
        assert(node->chunk <= position);

        memcpy(position, data.data, data.size);

        if ((position + data.size) > node->position)
                node->position = position + data.size;
}

void fwbuf_node_push(struct fwbuf_node *node, struct writable data)
{
  assert(node != NULL);
  assert(node->position != NULL);

  fwbuf_node_emplace(node, node->position, data);
}

struct fwbuf_node *fwbuf_node_create(size_t chunk_size)
{
        assert(chunk_size > 0);

        const size_t node_size =
            sizeof(struct fwbuf_node) + (chunk_size - 1);
        struct fwbuf_node *node = malloc(node_size);
        if (node == NULL)
                return NULL;

        node->position = node->chunk;
        node->next = NULL;

        return node;
}

#endif /* ifndef FWBUF_SRC_FWBUF_NODE_H_ */
