
cmake_minimum_required(VERSION 2.8)

PROJECT(FiberParametrization)

#Find VTK Package
find_package(VTK REQUIRED)

#Find ITK Package
find_package(ITK REQUIRED)

#Find Slicer Execution Model Package
find_package(SlicerExecutionModel REQUIRED)

include(${VTK_USE_FILE})
include(${ITK_USE_FILE})
include(${SlicerExecutionModel_USE_FILE})

file(
GLOB
source_files
*.[ch]pp
*.[ch]xx
*.[ch]
)

SET(FiberParametrization ${source_files})

#add_executable(FiberParametrization
#${source_files})


#target_link_libraries(FiberParametrization ${ITK_LIBRARIES} ${VTK_LIBRARIES})

SEMMacroBuildCLI(
  NAME fiberparametrization
  ADDITIONAL_SRCS ${source_files}
  EXECUTABLE_ONLY
  TARGET_LIBRARIES ${ITK_LIBRARIES} ${VTK_LIBRARIES}
  )
