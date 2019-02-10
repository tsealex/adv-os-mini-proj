////
//// Created by alex on 2/8/19.
////
//
//#include <cstdint>
//#include <unordered_map>
//
//#ifndef MINI_PROJECT_CACHE_H
//#define MINI_PROJECT_CACHE_H
//
//#endif //MINI_PROJECT_CACHE_H
//
//// Each FileCacheNode stores a data block of size specified in the FileCache
//// instance.
//struct FileCacheNode {
//    int blk_idx;
//    unsigned char *data;
//    FileCacheNode *prev, *next;
//};
//
//class FileCache {
//private:
//    // Maximum number of nodes in the cache.
//    int num_nodes_limit;
//
//    // Current number of nodes in the cache.
//    int num_nodes;
//
//    // Number of bytes in each file block.
//    int block_sz;
//
//    // Descriptor of the file to load the data from.
//    int fd;
//
//    // Map that maps offsets to pointers to the corresponding cache nodes.
//    std::unordered_map<int, FileCacheNode*> map;
//
//    // Newly accessed nodes are appended to tail; in case num_nodes_limit has
//    // been exceeded, nodes are removed from head.
//    FileCacheNode *head, *tail;
//
//public:
//    // Read 8 bytes from the file offset.
//    unsigned char *Read(uint64_t offset);
//    // Constructor.
//    FileCache(char* filename, int max_num_nodes, int block_sz, bool direct);
//};