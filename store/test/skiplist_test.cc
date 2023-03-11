#include <gtest/gtest.h>
#include <string>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <vector>
#include <algorithm>

#include <skipList.h>
using namespace std;

TEST(InsertTest, InsertTestIncrement) {
    GTEST_SKIP();
    SkipList<int, string, SkipNode> *list = new SkipList<int, string, SkipNode>();
    for (int i = 0;i < 10;i++) {
        SkipNode<int, string> *node = new SkipNode<int, string>(i, "z" + to_string(i));
        list->insertOne(*node);
    }
    list->watch();
}

TEST(InsertTest, InsertTestDecrement) {
    GTEST_SKIP();
    srand((unsigned)time(NULL));
    SkipList<int, string, SkipNode> *list = new SkipList<int, string, SkipNode>();
    for (int i = 10;i >= 0;i--) {
        SkipNode<int, string> *node = new SkipNode<int, string>(i, "z" + to_string(i));
        list->insertOne(*node);
    }
    list->watch();
}

TEST(InsertTest1, InsertTestRandom) {
    // GTEST_SKIP();
    std::vector<int> index;
    index.reserve(30);
    for (int i = 0;i < 30;i++) {
        index.push_back(i);
    }
    random_shuffle(index.begin(), index.end());
    SkipList<int, string, SkipNode> *list = new SkipList<int, string, SkipNode>();
    for (int i : index) {
        SkipNode<int, string> *node = new SkipNode<int, string>(i, to_string(i));
        list->insertOne(*node);
    }
    list->watch();
}

TEST(WatchTest, WatchTestNotSupportNOTString) {
    // GTEST_SKIP();
    std::vector<int> index;
    for (int i = 0;i < 30;i++) {
        index.push_back(rand() % 30);
    }
    SkipList<int, int, SkipNode> *list = new SkipList<int, int, SkipNode>();
    for (int i : index) {
        SkipNode<int, int> *node = new SkipNode<int, int>(i, i);
        list->insertOne(*node);
    }
    list->watch();
}

TEST(AllocaNodeTest, NewNode) {
    GTEST_SKIP();
    auto *node = NewNodeAllocate<SkipNode, int, string>::create(1, "zx1");
    std::cout << node->key << std::endl;
}
