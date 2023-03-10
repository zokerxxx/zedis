#ifndef ZEDIS_SOTRE_ALLOCATE_ALLOCATENOSE_H_
#define ZEDIS_SOTRE_ALLOCATE_ALLOCATENOSE_H_

// 如何支持变长构造参数?
template <template <class K, class V> class Node, class K, class V>
struct NewNodeAllocate {
    static Node<K, V>* create(const K &key, const V &value) {
        return new(std::nothrow) Node<K, V>(key, value);
    }
};


#endif