/**
 * 目前为止出现过的错误：
 * 1.template template参数不能与tamplate参数重名，同理嵌套的template class的参数也不能同名
 * 2.使用了Node<K, V>模板类型去调用一个方法，见错误expected primary-expression before ‘.’ token
 * 3.类成员函数与类成员变量同名
 * 4.realloc函数设置的size不能小于原size（越界问题）
 * 5.出现了多次先更新在赋值的错误（原顺序应该是先赋值再更新），如下面的代码
 *          current = current->levelNext[currentLevel];
 *          prev = node_index_map[current];
 *  上面的代码原意是将prev更新为当前节点的index，但是由于错误的顺序，导致prev赋值为了current的next节点的index值
 *
 * 6.new出来的skipnode通过free的方式进行释放
*/
#ifndef ZEDIS_STORE_SKIPLIST_H_
#define ZEDIS_STORE_SKIPLIST_H_

#define RED  "\033[31m"

#include <iostream>
#include <unordered_map>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <deque>

#include <string.h>

#include <randomalgo/binarySelect.h>
#include <allocate/newNodeAlloc.h>

/**
 * 用作watch函数特例化。
 * SkipList中有一个watch函数，由于打印时需要知道每个value的长度，
 * 由于不同类型的V获取长度的方式也不同
 * 所以为了简单，watch只限定模板参数 V 为string的情况，其他类型的V只会
 * 打印不支持的信息
 */ 
struct NoStringVersionTag {};
struct StringVersionTag {};

template <class T>
struct VersionDispatcher {
    using Tag = NoStringVersionTag;
};

template <>
struct VersionDispatcher<std::string> {
    using Tag = StringVersionTag;
};


/**
 * SkipNode
*/
template<class K, class V>
class SkipNode {
 public:
    SkipNode(const K &key, const V &value) : key(key), value(value), topLevel(BinarySelect::generate()) {
        this->init();
    }

    SkipNode(const K &key, const V &value, int level) : key(key), value(value), topLevel(level) {
        this->init();
    }

    ~SkipNode() {
        free(this->levelNext);
    }

 public:
    V& getValue() {return this->value;}
    void setValue(V& value) {this->value = value;}

 public:
    void expansion(int n) {
        SkipNode<K, V> **newLevelNext = (SkipNode<K, V>**)realloc(this->levelNext, sizeof(SkipNode<K, V>*) * (n + 1));
        if (newLevelNext == nullptr) {
            newLevelNext = (SkipNode<K, V>**)malloc(sizeof(SkipNode<K, V>*) * (n + 1));
            memcpy(newLevelNext, this->levelNext, sizeof(SkipNode<K, V>*) * (this->topLevel + 1));
            free(this->levelNext);
        }
        this->levelNext = newLevelNext;
        memset(this->levelNext + this->topLevel + 1, 0, sizeof(SkipNode<K, V>*) * (n - this->topLevel));
        this->topLevel = n;
    }

    // TO-DO, 可以进一步优化，提前开辟出空间，然后使用placement new，
    // 可以使得levelNext中的元素紧邻对象
    void init() {
        SkipNode<K, V> **levelNext = (SkipNode<K, V>**)malloc(sizeof(SkipNode<K, V>*) * (this->topLevel + 1));
        this->levelNext = levelNext;
        memset(this->levelNext, 0, sizeof(SkipNode<K, V>*) * (this->topLevel + 1));
    }

 public:
    K key;
    int topLevel;
    SkipNode<K, V> **levelNext = nullptr;

 private:
    V value;
};


/**
 * 每一个SkipList中的SkipNode都保持相同的key、value类型，所以此处使用K、V的typename，
 * 并使用K、V作用于一个template template参数的Node中，以保持Node中的K、V类型和SkipList中
 * 的K、V类型是一致的.
 * 除此之外还可以通过只定义一个class Node模板参数，然后通过以下语句还保证某个SkipList的K、V类型：
 * typedef SkipList<Node<int, string>> ISSkipList 
 * 此处定义了一个key为int、value为string类型的SkipList，但是这种方法还是无法保证Node的类型
 * 与SkipList的类型一致.
 * 注：如果想在域内控制某个模板参数的模板参数，可以使用template template parameter的方式
 * 
 * 当前版本的skiplist有一个致命的缺陷：插入的数据，如果大量的递减key，则会退化成单链表,
 * 后续需要通过设置哨兵head解决这个问题。
*/
template<
    class K, class V,
    template <class Key, class Value> class Node = SkipNode,
    class Alloc = NewNodeAllocate<Node, K, V>,
    class RandomLevel = BinarySelect
>
class SkipList {
 public:
    SkipList() {}

    void init(Node<K, V> *node) {
        this->head = node;
        this->maxLevel = node->topLevel;
    }

    void destory();

    // 插入一个现成的node
    void insertOne(Node<K, V> &node);

    // 指定key和value，并自动new一个node，随后插入到list中
    void insertOne(const K& key, const V& value);

    void deleteOne(const K &key);

    void find(const K &key);

    // 跳表序列化
    void flush();

    // 适合查看极小量数据,debug用
    void watch() {
        this->watchInner(typename VersionDispatcher<V>::Tag {});
    }

 public:
    Node<K, V> *head = nullptr;

 private:
    unsigned int maxLevel = 0;
    unsigned int total = 0;

    /**
     * 为即将插入的元素寻找“坑位”,如果没有坑位（返回false）代表新的节点需要成为head
    */
    bool findHoles(const Node<K, V>& node, std::vector<Node<K, V>*>& index) {
        int level = this->head->topLevel;
        Node<K, V> *prev, *current;
        prev = nullptr;
        current = this->head;
        while (level != -1) {
            while (current != nullptr && node.key > current->key) {
                prev = current;
                current = current->levelNext[level];
            }
            if (prev == nullptr) return false;
            if (current == nullptr || node.key < current->key) {
                current = prev;
            }
            index[level] = current;
            level--;
            if (node.key == current->key) {
                while (level != -1) {
                    index[level] = current;
                    level--;
                }
                // 同key情况返回true
                return true;
            }
        }
        return true;
    }

 public:
    bool findNode(const K& key, std::vector<std::pair<Node<K, V>*, int> >& index) {
        int level = this->head->topLevel;
        Node<K, V> *prev, *current;
        prev = nullptr;
        current= this->head;
        while (level != -1) {
            while (current != nullptr && key > current->key) {
                prev = current;
                current = current->levelNext[level];
            }
            if (prev == nullptr) return false;
            if (current != nullptr && current->key == key) {
                index.push_back({prev, level});
                current = prev;
            }
            if (current == nullptr || current->key > key) {
                current = prev;
            }
            level--;
        }
        return index.size() != 0;
    }

 private:
    // 重载了两个watchInner函数，用于特例化模板参数V为string的情况
    void watchInner(NoStringVersionTag) {
        std::cout << RED << "Only support SkipList template arg V is [string]!" << std::endl;
    }

    void watchInner(StringVersionTag) {
        if (this->head == nullptr || this->total > 30) {
            std::cout << RED << "Empty SkipList or SkipList total nodes > 30" << std::endl;
            return;
        }
        std::unordered_map<Node<K, V>*, int> node_index_map;
        std::unordered_map<int, int> node_len_map;
        Node<K, V> *current = this->head;
        int currentLevel = this->head->topLevel;
        // 首先将最底层的节点指针连同其index保存起来
        int count = 0;
        node_index_map[current] = 0;
        node_len_map[0] = current->getValue().size();
        current = current->levelNext[0];
        count++;
        while (current != nullptr) {
            node_index_map[current] = count;
            node_len_map[count] = current->getValue().size() + node_len_map[count -1];
            current = current->levelNext[0];
            count++;
        }

        while (currentLevel != 0) {
            current = this->head;
            int prev = 0;
            if (current != nullptr) {
                std::cout << current->getValue();
                current = current->levelNext[currentLevel];
            }

            while(current != nullptr) {
                int interval = 3;
                if (node_index_map[current] - 1 != prev) {
                    interval = 3 * (node_index_map[current] - prev) + 
                            node_len_map[node_index_map[current] - 1] - 
                            node_len_map[prev];
                }
                std::cout << std::setw(interval) << " ";
                std::cout << current->getValue();
                prev = node_index_map[current];
                current = current->levelNext[currentLevel];
            }
            std::cout << std::endl;
            currentLevel--;
        }

        // 打印最后一层
        current = this->head;
        std::cout << current->getValue();
        current = current->levelNext[0];
        while (current != nullptr) {
            std::cout << std::setw(3) << " ";
            std::cout << current->getValue();
            current = current->levelNext[0];
        }
        std::cout << std::endl;
    }

    // 使得node成为新的head，但是并不删除原始head
    void headTransfer(Node<K, V> &node) {
        if (node.topLevel < this->head->topLevel) {
             node.expansion(this->head->topLevel); 
        }

        int currentNodeLevel = 0;
        int currentHeadLevel = 0;
        while (currentHeadLevel <= this->head->topLevel) {
            node.levelNext[currentNodeLevel] = this->head;
            currentNodeLevel++;
            currentHeadLevel++;
        }
        while (currentNodeLevel <= node.topLevel) {
            node.levelNext[currentNodeLevel] = nullptr;
            currentNodeLevel++;
        }
        this->maxLevel = node.topLevel > this->maxLevel ? node.topLevel : this->maxLevel;
        this->head = &node;
        this->total = this->total + 1;
    }
};

template<
    class K, class V,
    template <class Key, class Value> class Node,
    class Alloc,
    class RandomLevel
>
void SkipList<K, V, Node, Alloc, RandomLevel>::insertOne(Node<K, V> &node) {
    if (this->head == nullptr) {
        this->init(&node);
        this->total = this->total + 1;
        return;
    }

    // 存储待插入node在当前list每一层的hole
    std::vector<Node<K, V>*> index;
    index.reserve(this->maxLevel + 1);

    // 新的节点成为head
    if (!this->findHoles(node, index)) {
        return this->headTransfer(node);  
    }

    if(index[0]->key == node.key) {
        index[0]->setValue(node.getValue());
        return;
    }

    int currentLevel = this->maxLevel > node.topLevel ? node.topLevel : this->maxLevel;
    while (currentLevel != -1) {
        node.levelNext[currentLevel] = index[currentLevel]->levelNext[currentLevel];
        index[currentLevel]->levelNext[currentLevel] = &node;
        currentLevel--;
    }
    if (node.topLevel > this->maxLevel) {
        currentLevel = node.topLevel;
        this->head->expansion(node.topLevel);
        while (currentLevel > this->maxLevel) {
            this->head->levelNext[currentLevel] = &node;
            currentLevel--;
        }
        this->maxLevel = node.topLevel;
    }
    this->total = this->total + 1;
}

template<
    class K, class V,
    template <class Key, class Value> class Node,
    class Alloc,
    class RandomLevel
>
void SkipList<K, V, Node, Alloc, RandomLevel>::deleteOne(const K &key) {
    if (this->head == nullptr) return;
    std::vector<std::pair<Node<K, V>*, int>> index;
    // 删除节点
    if (this->findNode(key, index)) {
        Node<K, V> *deletingNode = index[0].first->levelNext[index[0].second];
        for (auto node_level : index) {
            node_level.first->levelNext[node_level.second] = deletingNode->levelNext[node_level.second];
        }
        delete deletingNode;
        this->total--;
    // 删除头节点，并且list中只有头节点
    } else if (this->head->key == key && this->head->levelNext[0] == nullptr) {
        delete this->head;
        this->head = nullptr;
        this->total = 0;
    // 删除头节点  
    } else if (this->head->levelNext[0] != nullptr) {
        Node<K, V> *newHead = this->head->levelNext[0];
        int oldLevel = newHead->topLevel;
        int headLevel = this->head->topLevel;
        if (newHead->topLevel < this->head->topLevel) {
            newHead->expansion(this->head->topLevel);
        }
        while (headLevel > oldLevel) {
            newHead->levelNext[headLevel] = this->head->levelNext[headLevel];
            headLevel--;
        }
        delete this->head;
        this->head = newHead;
        this->total--;
    }
    // 该节点不存在
    else return;
}

// template<
//     class K, class V,
//     template <class Key, class Value> class Node,
//     class Alloc,
//     class RandomLevel
// >
// V 


/**
 * 下面是使用Type2Type实现watch函数特例化的一个模板
 * 但是这种方法，并不适用在类的成员函数，当SkipList
 * 的V是string类型时，会出现重复定义的问题
 */
template <class T>
struct Type2Type {
    typedef T originType;
};

template<
    class K,
    template <class Key, class Value> class Node,
    class RandomLevel
>
void watch_tt(SkipList<K, std::string, Node, RandomLevel>* list, Type2Type<std::string>) {
    std::cout << "watch a list " << std::endl;
}

template<
    class K, class V,
    template <class Key, class Value> class Node,
    class RandomLevel
>
void watch_tt(SkipList<K, V, Node, RandomLevel>* list, Type2Type<V>) {
    std::cout << "Only support SkipList template arg V is string!" << std::endl;
}

#endif