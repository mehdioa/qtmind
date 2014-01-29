# ------------------------------------------------------------------------------
#  QTMIND CMAKE FILE
# ------------------------------------------------------------------------------
cmake_minimum_required(VERSION 2.8.8)

project(qtmind)

# Recover canonical (x.y.z) version number from `src/appinfo.h`.
file(STRINGS ${PROJECT_SOURCE_DIR}/src/appinfo.h qtmind_VER_MAJOR REGEX "VER_MAJOR")
string(REGEX MATCH "([0-9]+)" qtmind_VER_MAJOR ${qtmind_VER_MAJOR})
file(STRINGS ${PROJECT_SOURCE_DIR}/src/appinfo.h qtmind_VER_MINOR REGEX "VER_MINOR")
string(REGEX MATCH "([0-9]+)" qtmind_VER_MINOR ${qtmind_VER_MINOR})
file(STRINGS ${PROJECT_SOURCE_DIR}/src/appinfo.h qtmind_VER_PATCH REGEX "VER_BUGFIX")
string(REGEX MATCH "([0-9]+)" qtmind_VER_PATCH ${qtmind_VER_PATCH})
SET(QTMIND_VERSION ${qtmind_VER_MAJOR}.${qtmind_VER_MINOR}.${qtmind_VER_PATCH})

# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra")
#
 set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  -std=c++0x -Wall -Wextra")

# if(${CMAKE_BUILD_TYPE} MATCHES "Release")
#   add_definitions(-DQT_NO_DEBUG)
#   add_definitions(-DQT_GUI_LIB)
#   add_definitions(-DQT_CORE_LIB)
#   add_definitions(-DQT_SHARED)
# endif(${CMAKE_BUILD_TYPE} MATCHES "Release")
#unset(CMAKE_REQUIRED_FLAGS)


# ------------------------------------------------------------------------------
#  EXTERNAL PACKAGE
# ------------------------------------------------------------------------------
find_package( Qt4 4.5.0 COMPONENTS QtCore QtGui REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
#FIND_PACKAGE(Qt5Widgets)
# ------------------------------------------------------------------------------
#  includeS
# ------------------------------------------------------------------------------
include(${QT_USE_FILE})
include(CheckCXXCompilerFlag)
include(FindPkgConfig)
include(cmake/Deb.cmake)
include(cmake/Rpm.cmake)
include(cmake/Summary.cmake)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# if (CMAKE_CXX_COMPILER MATCHES ".*clang")
#   set(CMAKE_COMPILER_IS_CLANGXX 1)
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-uninitialized")
# endif ()

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fmessage-length=0")
if (CMAKE_COMPILER_IS_GNUCXX)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fmessage-length=0")
    if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,--as-needed")
    endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
endif (CMAKE_COMPILER_IS_GNUCXX)


include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
#	${QT_INCLUDES}
)

file( GLOB TRANS_FILES ${CMAKE_SOURCE_DIR}/translation/qtmind_*.ts)
set(TS_FILES ${TRANS_FILES})

set(UPDATE_TRANSLATIONS OFF) # no ts update
#set(UPDATE_TRANSLATIONS ON) # ts update

# ------------------------------------------------------------------------------
#  SOURCES
# ------------------------------------------------------------------------------
add_subdirectory(src)

# ------------------------------------------------------------------------------
#  INSTALLATION
# ------------------------------------------------------------------------------
# -- install binary


# -- install destop file
install(FILES qtmind.desktop  DESTINATION /usr/share/applications/)

# -- install icons files
install(FILES icons/hicolor/16x16/qtmind.png DESTINATION share/icons/hicolor/16x16/apps/ RENAME application-x-qtmind.png)
install(FILES icons/hicolor/32x32/qtmind.png DESTINATION share/icons/hicolor/32x32/apps/ RENAME application-x-qtmind.png)
install(FILES icons/hicolor/48x48/qtmind.png DESTINATION share/icons/hicolor/48x48/apps/ RENAME application-x-qtmind.png)
install(FILES icons/hicolor/64x64/qtmind.png DESTINATION share/icons/hicolor/64x64/apps/ RENAME application-x-qtmind.png)
install(FILES icons/hicolor/128x128/qtmind.png DESTINATION share/icons/hicolor/128x128/apps/ RENAME application-x-qtmind.png)
install(FILES icons/hicolor/256x256/qtmind.png DESTINATION share/icons/hicolor/256x256/apps/ RENAME application-x-qtmind.png)
install(FILES icons/hicolor/512x512/qtmind.png DESTINATION share/icons/hicolor/512x512/apps/ RENAME application-x-qtmind.png)


# ------------------------------------------------------------------------------
#  UN-INSTALL SUPPORT
# ------------------------------------------------------------------------------
add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

# Show a summary of what we have enabled
summary_show()
