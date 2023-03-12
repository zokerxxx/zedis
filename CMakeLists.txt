cmake_minimum_required(VERSION 3.10)

project(zedis)

## 通过set可以设置一些cmake相关的环境信息
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -g2)


## 添加子CMakeLists.txt路径
add_subdirectory(store/src)
add_subdirectory(store/test)