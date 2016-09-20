
cmake_minimum_required(VERSION 2.8)

PROJECT(FiberTractSampling)

#Find VTK Package
find_package(VTK REQUIRED)

#Find ITK Package
find_package(ITK REQUIRED)

#Find Qt Package

find_package(Qt4 4.4.3 REQUIRED QtGui)

include(${VTK_USE_FILE})
include(${ITK_USE_FILE})


file(
GLOB
source_files
*.[ch]pp
*.[ch]xx
*.[ch]
)

SET(FiberTractSampling ${source_files})

add_executable(FiberTractSampling
${source_files})

target_link_libraries(FiberTractSampling ${ITK_LIBRARIES} ${VTK_LIBRARIES})
