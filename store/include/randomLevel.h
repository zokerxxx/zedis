#ifndef ZEDIS_STORE_RANDOMLEVEL_H_
#define ZEDIS_STORE_RANDOMLEVEL_H_


template <
    class RandomAlgo
>
struct RandomLevel {
 public:
    unsigned short operator() () {
        return RandomAlgo.generate();
    }
};




#endif