#ifndef ZEDIS_STORE_RANDOMALGOO_BINARYSELECT_H_
#define ZEDIS_STORE_RANDOMALGOO_BINARYSELECT_H_

#include <iostream>
#include <ctime>
#include <cstdlib>
#include <float.h>

#include <config/skiplist.h>

/**
 * 已遇到的错误
 * 1.double(rand() / RAND_MAX) rand() /RAND_MAX 输出整数0，所以最终结果都为0
*/


class BinarySelect {
 public:
    static unsigned short generate() {
        int level = 1;
        while (random() - FLT_EPSILON < 0.5) {
            level++;
        }
        if (level > MAX_LEVEL) level = MAX_LEVEL;
        return level;
    }

    static double random() {
        return double(rand()) / RAND_MAX;
    }

};

#endif