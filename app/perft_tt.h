/*
MIT License

Copyright (c) 2018 Manik Charan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef LOLTAXX_PERFT_TT_H
#define LOLTAXX_PERFT_TT_H

#include <cinttypes>
#include <memory>

namespace loltaxx {

enum class PerftTTConstants
{
    CLUSTER_SIZE = 4
};

struct PerftTTEntry {
    PerftTTEntry();
    std::uint64_t get_key() const;
    void set(int depth, std::uint64_t nodes, std::uint64_t key);
    int get_depth() const;
    std::uint64_t get_nodes() const;
    void clear();

   private:
    std::uint64_t data;
    std::uint64_t key;
};

inline PerftTTEntry::PerftTTEntry() {
}
inline void PerftTTEntry::set(int depth, std::uint64_t nodes, std::uint64_t key) {
    data = (nodes & std::uint64_t(0xFFFFFFFF)) | (std::uint64_t(depth) << 56u);
    this->key = key ^ data;
}
inline std::uint64_t PerftTTEntry::get_key() const {
    return key ^ data;
}
inline int PerftTTEntry::get_depth() const {
    return data >> 56u;
}
inline std::uint64_t PerftTTEntry::get_nodes() const {
    return data & std::uint64_t(0xFFFFFFFF);
}
inline void PerftTTEntry::clear() {
    key = data = 0;
}

struct PerftTTCluster {
    PerftTTEntry& get_entry(std::uint64_t key);
    void clear();

   private:
    PerftTTEntry entries[static_cast<int>(PerftTTConstants::CLUSTER_SIZE)];
};

inline PerftTTEntry& PerftTTCluster::get_entry(std::uint64_t key) {
    // If any entry key matches, return it
    for (PerftTTEntry& entry : entries) {
        if (entry.get_key() == key)
            return entry;
    }
    // Otherwise, return entry with minimum depth in cluster
    int min_depth_index = 0;
    for (int i = 1; i < static_cast<int>(PerftTTConstants::CLUSTER_SIZE); ++i) {
        if (entries[i].get_depth() < entries[min_depth_index].get_depth())
            min_depth_index = i;
    }
    return entries[min_depth_index];
}

inline void PerftTTCluster::clear() {
    for (PerftTTEntry& entry : entries)
        entry.clear();
}

struct PerftTranspositionTable {
    PerftTranspositionTable();
    ~PerftTranspositionTable();
    PerftTranspositionTable(int MB);
    void resize(int MB);
    PerftTTEntry probe(std::uint64_t key) const;
    void write(int depth, std::uint64_t nodes, std::uint64_t key);
    void clear();
    int hash(std::uint64_t key) const;
    int get_size() const;

   private:
    PerftTTCluster* table;
    int size;
};

inline PerftTranspositionTable::PerftTranspositionTable() {
    size = (1 << 20) / sizeof(PerftTTCluster);
    table = new PerftTTCluster[size];
    clear();
}

inline PerftTranspositionTable::~PerftTranspositionTable() {
    delete[] table;
}

inline PerftTranspositionTable::PerftTranspositionTable(int MB) {
    resize(MB);
}

inline void PerftTranspositionTable::resize(int MB) {
    if (MB <= 0)
        MB = 1;

    size = ((1 << 20) / sizeof(PerftTTCluster)) * MB;
    delete[] table;
    table = new PerftTTCluster[size];
    clear();
}

inline void PerftTranspositionTable::clear() {
    for (int i = 0; i < size; ++i)
        table[i].clear();
}

inline int PerftTranspositionTable::hash(std::uint64_t key) const {
    return key % size;
}

inline PerftTTEntry PerftTranspositionTable::probe(std::uint64_t key) const {
    int index = hash(key);
    return table[index].get_entry(key);
}

inline void PerftTranspositionTable::write(int depth, std::uint64_t nodes, std::uint64_t key) {
    int index = hash(key);
    table[index].get_entry(key).set(depth, nodes, key);
}

inline int PerftTranspositionTable::get_size() const {
    return size;
}

inline PerftTranspositionTable perft_tt{16};

}  // namespace loltaxx

#endif  // LOLTAXX_PERFT_TT_H
