//
// Created by alex on 2/8/19.
//
#include <iostream>
#include "cache.h"
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

unsigned char *FileCache::Read(uint64_t offset) {
    FileCacheNode *node;
    int blk_idx = int(offset / block_sz);
    int blk_offset = int(offset % block_sz);
    auto res = this->map.find(blk_idx);
    if (res == this->map.end()) {
        // Load the data block from file if it does not exist in cache.
        auto sz = this->block_sz * sizeof(unsigned char);
        auto data = (unsigned char *) malloc(sz);
        lseek64(this->fd, blk_idx * this->block_sz, SEEK_SET);
        read(this->fd, data, sz);
        node = (FileCacheNode *) malloc(sizeof(FileCacheNode));
        node->data = data;
        node->blk_idx = blk_idx;
        // Add the new node to cache map.
        std::pair<int, FileCacheNode*> kv_pair (blk_idx, node);
        this->map.insert(kv_pair);
    } else {
        node = res->second;
    }
    // Append the node to the end of the cache list.
    if (this->head == nullptr) {
        this->head = this->tail = node;
    } else if (node != this->tail) {
        if (this->head == node) {
            this->head = node->next;
        } else if (node->prev != nullptr) {
            node->prev = node->next;
        }
        auto curr_tail = this->tail;
        curr_tail->next = node;
        node->prev = curr_tail;
        node->next = nullptr;
        this->tail = node;
    }
    // Increment the cache size.
    this->num_nodes++;
    if (this->num_nodes > this->num_nodes_limit) {
        // Remove the oldest node.
        auto head = this->head;
        this->head = head->next;
        this->map.erase(this->head->blk_idx);
        // Free the space.
        free(head->data);
        free(head);
        this->num_nodes--;
    }
    // Read 8 bytes from the node.
    auto buf = (unsigned char *) malloc(sizeof(unsigned char) * 8);
    bcopy(node->data + blk_offset, buf, 8);
    return buf;
}

FileCache::FileCache(char *filename, int max_num_nodes, int block_sz, bool direct) {
    int flag = O_RDONLY;
    if (direct) {
        flag |= O_DIRECT;
    }
    auto _fd = open(filename, flag);
    this->fd = _fd;
    this->num_nodes_limit = max_num_nodes;
    this->block_sz = block_sz;
    this->head = this->tail = nullptr;
}
