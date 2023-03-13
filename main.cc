#include <iostream>
#include <string.h>

#include <skipList.h>

using namespace std;

int main() {
    // cout << a->buffer << endl;
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
    delete list;
    return 0;
}
