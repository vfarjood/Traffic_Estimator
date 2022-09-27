cmake_minimum_required(VERSION 3.0 )
project(estimator VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)

file(GLOB PROJECT_SRCS  src/main.cpp
                        src/utils/Log.cpp
                        src/utils/Timer.cpp
                        src/utils/Lane.cpp
                        src/utils/FileUtils.cpp
                        src/utils/CmdLineParser.cpp
                        src/detector/YoloDetector.cpp
                        src/detector/MobilenetDetector.cpp
                        src/tracker/VehicleTracker.cpp
                        src/estimator/TrafficEstimator.cpp)

file(GLOB PROJECT_SRC_EVAL src/evaluate/Evaluate.cpp)

if(APPLE)
    message(STATUS "Platform: Apple")
    set(SUPPORT_EVAL true)
    add_executable(estimator ${PROJECT_SRCS} ${PROJECT_SRC_EVAL})
    find_package(OpenCV REQUIRED)
    include_directories(${OpenCV_INCLUDE_DIRS})
    target_link_libraries(estimator  ${OpenCV_LIBS})
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES Linux)
    message(STATUS "Platform: Linux")
    set(SUPPORT_EVAL true)
    add_executable(estimator ${PROJECT_SRCS} ${PROJECT_SRC_EVAL})
    find_package(OpenCV REQUIRED)
    include_directories(${OpenCV_INCLUDE_DIRS})
    target_link_libraries(estimator  ${OpenCV_LIBS})
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm")
    message(STATUS "Platform: ARM")
    set(SUPPORT_EVAL false)
    add_executable(estimator ${PROJECT_SRCS})
    # add_subdirectory(lib/opencv)
    set(CMAKE_INSTALL_RPATH "${PROJECT_SOURCE_DIR}/lib/opencv-4/lib/")
    list(APPEND EXTRA_LIB_DIRS ${PROJECT_SOURCE_DIR}/lib/opencv-4/lib)
    list(APPEND EXTRA_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/lib/opencv-4/include/opencv4)
    
    file(STRINGS ${PROJECT_SOURCE_DIR}/lib/opencv-4/lib/modules_name.txt modules)
    list(APPEND EXTRA_LINKS ${modules})
    
    #Add INCLUDE directories of third-party libraries
    target_include_directories(${PROJECT_NAME} 
        PUBLIC ${PROJECT_BINARY_DIR}
        PUBLIC ${EXTRA_INCLUDE_DIRS} )
    
    #Add LIB or SRC directories of third-party libraries
    target_link_directories(${PROJECT_NAME} PRIVATE ${EXTRA_LIB_DIRS})
    
    #Add the name of third-party libraries
    target_link_libraries(${PROJECT_NAME} ${EXTRA_LINKS})
endif()

#Configure file for version
configure_file(Config.h.in ${CMAKE_BINARY_DIR}/config.h)
include_directories(${CMAKE_BINARY_DIR})

