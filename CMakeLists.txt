CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
ENABLE_LANGUAGE(CXX)
SET(CMAKE_VERBOSE_MAKEFILE OFF)

PROJECT("Animated B-spline")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG")
endif (CMAKE_BUILD_TYPE STREQUAL "Release")

############
# Includes #
############

# Qt4 required
FIND_PACKAGE(Qt4 COMPONENTS QtCore QtGui QtXml REQUIRED)

# Required for Qt
SET(CMAKE_AUTOMOC ON)

# Useful variable
SET(BSPLINE_SRC_PATH  "${CMAKE_CURRENT_SOURCE_DIR}")

# Include paths
INCLUDE_DIRECTORIES(${BSPLINE_SRC_PATH})

# Include src/headers/forms
INCLUDE(${BSPLINE_SRC_PATH}/SourcesLib.cmake)

# Include Qt files
INCLUDE(${QT_USE_FILE})

###########
# Library #
###########

# Create UI header
QT4_WRAP_UI(BSPLINE_FORMS_HEADERS ${BSPLINE_FORMS})

# Make UI header available through include
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

# Create library file
ADD_LIBRARY(bspline SHARED ${BSPLINE_SRC} ${BSPLINE_HEADERS} ${BSPLINE_FORMS_HEADERS})

##############
# Executable #
##############

# Create executable
ADD_EXECUTABLE(BezierCurve "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
TARGET_LINK_LIBRARIES(BezierCurve bspline ${QT_LIBRARIES})

# Set output paths
SET(LIBRARY_OUTPUT_PATH lib/${CMAKE_BUILD_TYPE})
SET(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})


