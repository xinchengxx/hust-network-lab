cmake_minimum_required(VERSION 3.10)
project(StopWait)



SET(CMAKE_C_COMPTLER GCC)
set(CMAKE_CXX FLAGS -fexec-charset=GBK)
add_definitions(-std=c++11)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
#aux_source_directory(${PROJECT_SOURCE_DIR}/src SRC_LIST)
add_library(rdt STATIC
        ${PROJECT_SOURCE_DIR}/src/GBNRdtSender.cpp
        ${PROJECT_SOURCE_DIR}/src/GBNRdtReceiver.cpp
        ${PROJECT_SOURCE_DIR}/src/SRRdtSender.cpp
        ${PROJECT_SOURCE_DIR}/src/SRRdtReceiver.cpp
        ${PROJECT_SOURCE_DIR}/src/TCPReceiver.cpp
        ${PROJECT_SOURCE_DIR}/src/TCPSender.cpp
        )
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
FIND_LIBRARY(NETSIM_LIB libnetsim.a ${PROJECT_SOURCE_DIR}/lib)
add_executable(gbn
        ${PROJECT_SOURCE_DIR}/src/GBN.cpp
        )
add_executable(sr
        ${PROJECT_SOURCE_DIR}/src/SR.cpp
        )
add_executable(tcp
        ${PROJECT_SOURCE_DIR}/src/TCP.cpp
        )
target_link_libraries(gbn PUBLIC
        rdt
        ${NETSIM_LIB}
        )
target_link_libraries(sr PUBLIC
        rdt
        ${NETSIM_LIB}
        )
target_link_libraries(tcp PUBLIC
        rdt
        ${NETSIM_LIB}
        )

#ADD_EXECUTABLE(stop_wait ${SRC_LIST})

#TARGET_LINK_LIBRARIES(stop_wait  ${NETSIM_LIB})