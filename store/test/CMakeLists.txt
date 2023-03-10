enable_testing()

## 定义cmake查找lib的路径
link_directories("/home/zhengxin/coding/zedis/lib")
## 引入库文件
link_libraries(gtest)
## 定义cmake 的include path （即便C_PLUS_INCLUDE中已经有定义，也要加）
include_directories("/home/zhengxin/github/googletest/googletest/include")
include_directories("/home/zhengxin/coding/zedis/store/include")

## 生成可执行文件
add_executable(skiplisttest main.cc skiplist_test.cc)
## 为可执行文件链接库文件
target_link_libraries(skiplisttest PUBLIC store_lib gtest pthread)

