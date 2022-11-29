# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(app)

# 包含所有使用到的头文件路径
target_include_directories(app PRIVATE src)
target_include_directories(app PRIVATE src/app)
target_include_directories(app PRIVATE src/test)
target_include_directories(app PRIVATE src/common)
target_include_directories(app PRIVATE src/core)
target_include_directories(app PRIVATE src/driver)

# 包含所有使用到的源代码路径
aux_source_directory(./src/app DIR_APP_SRCS)
aux_source_directory(./src/test DIR_TEST_SRCS)
aux_source_directory(./src/common DIR_COMMON_SRCS)
aux_source_directory(./src/core DIR_CORE_SRCS)
aux_source_directory(./src/driver DIR_DRIVER_SRCS)

# 包含所有被编译的源代码文件
target_sources(app PRIVATE src/main.c 
                ${DIR_APP_SRCS} 
                ${DIR_TEST_SRCS} 
                ${DIR_COMMON_SRCS} 
                ${DIR_CORE_SRCS} 
                ${DIR_DRIVER_SRCS} 
                )
