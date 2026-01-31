# Copyright (C) 2022-2024 Trevor Last
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.31)

file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

project("Sickle Editor"
    VERSION 0.12.2
    DESCRIPTION "Open-Source GoldSrc Editor"
    LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    message("CMAKE_BUILD_TYPE is unset, defaulting to Release")
    set(CMAKE_BUILD_TYPE Release)
endif()

# MSYS2 only offers flex as a base package, but the compiler needs access to
# the flex headers and libraries. Therefore we need to tell CMake to search for
# flex in the MSYS2 /usr directory.
if(WIN32)
    set(MSYS_PREFIX "C:/msys64" CACHE FILEPATH "path to the MSYS installation")
    set(FLEX_ROOT "${MSYS_PREFIX}/usr" CACHE FILEPATH "flex search root")
endif()


# --- Dependencies --- ###########################

include(FetchContent)
FetchContent_Declare(
    glutils
    GIT_REPOSITORY https://github.com/Treecase/glutils
    GIT_TAG        4901def3f8bd14d1cf5c352b0b471a291adbebea)
FetchContent_MakeAvailable(glutils)

find_package(BISON 3.2 REQUIRED)
find_package(FLEX 2.6 REQUIRED)
find_package(GLEW 2.1 REQUIRED)
find_package(glm 0.9 REQUIRED)
find_package(Lua 5.4 REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(glibmm REQUIRED IMPORTED_TARGET glibmm-2.4)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-3.0)
pkg_check_modules(sigc++ REQUIRED IMPORTED_TARGET sigc++-2.0)
pkg_check_modules(OpenGL REQUIRED IMPORTED_TARGET opengl>=4.5)


# --- Variables --- ##############################
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Application ID
set(APPLICATION_ID com.github.Treecase.SickleEditor)
# GResource prefix
set(GRESOURCE_PREFIX /com/github/Treecase/SickleEditor/)
# GSchema path
set(SCHEMA_DIR "${CMAKE_CURRENT_BINARY_DIR}")


# --- Target --- #################################
add_executable(sickle)
target_link_libraries(sickle PRIVATE ${OpenGL_LIBRARIES} ${CMAKE_DL_LIBS})

include(GNUInstallDirs)

add_subdirectory(data)
include_directories(sickle PUBLIC src)
add_subdirectory(src)


install(TARGETS sickle)
