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

#include <gtest/gtest.h>

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

TEST(InsertTest, InsertTestRandom) {
    GTEST_SKIP();
    std::vector<int> index;
    index.reserve(10);
    for (int i = 0;i < 10;i++) {
        index.push_back(i);
    }
    random_shuffle(index.begin(), index.end());
    for (int num : index) {
        cout << num << " ";
    }
    cout << endl;
    SkipList<int, string, SkipNode> *list = new SkipList<int, string, SkipNode>();
    for (int i : index) {
        SkipNode<int, string> *node = new SkipNode<int, string>(i, to_string(i));
        list->insertOne(*node);
    }
    list->watch();
}

TEST(WatchTest, WatchTestNotSupportNOTString) {
    GTEST_SKIP();
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

TEST(DeleteTest, FindNodeTest) {
    GTEST_SKIP();
    std::vector<int> nums;
    for (int i = 0;i <= 100000;i++) {
        nums.push_back(i);
    }
    random_shuffle(nums.begin(), nums.end());

    SkipList<int, string, SkipNode> *list = new SkipList<int, string, SkipNode>();
    for (int i : nums) {
        SkipNode<int, string> *node = new SkipNode<int, string>(i, to_string(i));
        list->insertOne(*node);
    }
    // list->watch();
    cout << endl;
    std::vector<std::pair<SkipNode<int ,string>*, int>> index;
    
    std::vector<int>  deleteNodes;
    for (int i = 100000;i >= 0;i--) {
        list->deleteOne(nums[i]);
    }
    list->watch();
}