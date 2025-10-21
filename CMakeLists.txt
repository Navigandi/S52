cmake_minimum_required(VERSION 3.19 FATAL_ERROR)

# Read project metadata
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/project.json PROJECT_METADATA)
string(JSON PROJECT_NAME GET ${PROJECT_METADATA} "name")
string(JSON PROJECT_VERSION GET ${PROJECT_METADATA} "version")

# Set project metatada
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION})
message("Project: " ${PROJECT_NAME})
message("Version: " ${PROJECT_VERSION})

#Read dependencies metadata
#string(JSON DEPENDENCIES_LENGTH LENGTH ${PROJECT_METADATA} "dependencies")
#message("Dependencies:")

set(VAR_PREFIX "NAVIGANDI")
set(VAR_SUFFIX "VERSION")
MATH(EXPR DEPENDENCIES_LIMIT "${DEPENDENCIES_LENGTH}-1")

#foreach (DEPENDENCY_IDX RANGE ${DEPENDENCIES_LIMIT})
#  string(JSON DEPENDENCY_NAME MEMBER ${PROJECT_METADATA} "dependencies" ${DEPENDENCY_IDX})
#  string(JSON DEPENDENCY_VERSION GET ${PROJECT_METADATA} "dependencies" ${DEPENDENCY_NAME})
#  if (DEPENDENCY_VERSION STREQUAL "")
#    set(DEPENDENCY_VERSION "[any]")
#  else()
#    string(TOUPPER ${DEPENDENCY_NAME} DEPENDENCY_VAR)
#    set(${VAR_PREFIX}_${DEPENDENCY_VAR}_${VAR_SUFFIX} ${DEPENDENCY_VERSION})
#  endif()
#  message("\t${DEPENDENCY_NAME}: ${DEPENDENCY_VERSION}")
#endforeach()

set (CPACK_PACKAGE_VENDOR NAVIGANDI)
set (CPACK_PACKAGE_VERSION_MAJOR 1)
set (CPACK_PACKAGE_VERSION_MINOR 0)
set (CPACK_PACKAGE_VERSION_PATCH 0)

# Set compiler variable
# if(MSVC_TOOLSET_VERSION EQUAL 141)
#   set(VAR_COMPILER msvc15)
# elseif(MSVC_TOOLSET_VERSION EQUAL 120)
#   set(VAR_COMPILER msvc12)
# elseif(MSVC_TOOLSET_VERSION EQUAL 110)
#   set(VAR_COMPILER msvc11)
# elseif(MSVC_TOOLSET_VERSION EQUAL 100)
#   set(VAR_COMPILER msvc10)
# elseif(MSVC_TOOLSET_VERSION EQUAL 90)
#   set(VAR_COMPILER msvc9)
# elseif(UNIX)
#   string(REGEX MATCH "([0-9]+\\.[0-9]+)" VAR_COMPILER ${CMAKE_CXX_COMPILER_VERSION})
#   set(VAR_COMPILER ${CMAKE_CXX_COMPILER_ID}${VAR_COMPILER})
# endif()

# Set version string variable
#string(REPLACE "." "_" VAR_VERSION ${PROJECT_VERSION})
## Set postfixes
#set(CMAKE_DEBUG_POSTFIX -${VAR_COMPILER}-d-${VAR_VERSION})
#set(CMAKE_RELEASE_POSTFIX -${VAR_COMPILER}-${VAR_VERSION})
# if (WIN32)
#   set(LIB_PREFIX ${CMAKE_PROJECT_NAME}_)
#   set(LIB_SUFFIX .lib)
# else()
#   set(LIB_PREFIX ${CMAKE_PROJECT_NAME}_)
#   set(LIB_SUFFIX ${CMAKE_RELEASE_POSTFIX}.a)
# endif()

# Flags para garantir a compilação em windows
if(WIN32)
  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(PLATFORM_TAG win_x64)
    set(VAR_ADDRESS_MODE 64)
  else()
    set(PLATFORM_TAG win_32)
    set(VAR_ADDRESS_MODE 32)
  	set(WIN32_FLAGS "/EHsc /D WIN32")
  	set(WIN32_LIBS ws2_32.lib odbc32.lib odbccp32.lib)
  endif()
#  set(CMAKE_EXE_LINKER_FLAGS
#   /NODEFAULTLIB:LIBC
#	/NODEFAULTLIB:LIBCMT
#	/NODEFAULTLIB:LIBCD
#	/NODEFAULTLIB:LIBCMTD)


  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W3 /EHsc /bigobj /MP")
  add_definitions(-D_SILENCE_ALL_CXX17_DEPRECATION_WARNINGS)
  add_definitions(-D_CRTDBG_MAP_ALLOC)
else()
  # Nightmare compiler options
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-unused-local-typedefs ")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC  -std=c++11")
  #set(PLATFORM_TAG linux_x86_64)
endif()

#message("Selected Platform: " ${PLATFORM_TAG})
# if (DEFINED DEPLOY)
# set(INSTALL_BASE "${PROJECT_NAME}_${VAR_VERSION}_${VAR_ADDRESS_MODE}_${VAR_COMPILER}")
# else ()
# set(INSTALL_BASE "${CMAKE_INSTALL_PREFIX}")
# endif()

#set(VAR_COMPONENTS "")
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(SearchInstalledSoftware)

if(SWIG_FOUND)
  # set(temp_CMAKE_RELEASE_POSTFIX ${CMAKE_RELEASE_POSTFIX})
  # set(temp_CMAKE_DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

  # set(CMAKE_RELEASE_POSTFIX "")
  # set(CMAKE_DEBUG_POSTFIX "")

  if (SWIG_VERSION VERSION_LESS "3.0.7")
    message (FATAL_ERROR "Swig version must be equal to or newer than 3.0.7")
  endif ()
  include(${SWIG_USE_FILE})
 
  # set(CMAKE_RELEASE_POSTFIX ${temp_CMAKE_RELEASE_POSTFIX})
  # set(CMAKE_DEBUG_POSTFIX ${temp_CMAKE_DEBUG_POSTFIX})
endif()

# Configurations
if (NOT DEFINED DISABLE_TESTS)
  enable_testing()
  include(CTest)
  include(CPack)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_definitions(-D_SCL_SECURE_NO_WARNINGS)



# svn revision header file generation
# add_custom_target(revision_file ALL DEPENDS ${PROJECT_BINARY_DIR}/nothing.hpp)
# add_custom_command(
#   OUTPUT ${PROJECT_BINARY_DIR}/nothing.hpp
#   COMMAND ${CMAKE_COMMAND} -DPROJ_DIR=${PROJECT_SOURCE_DIR}
#                            -DOUTPATH=${PROJECT_BINARY_DIR}
#                            -DPROJECT_NAME=${PROJECT_NAME}
#                            -P ${PROJECT_SOURCE_DIR}/cmake/write_git_revision_file.cmake
#   )

# set_source_files_properties(${PROJECT_BINARY_DIR}/nothing.hpp
#   PROPERTIES GENERATED TRUE
#   HEADER_FILE_ONLY TRUE
#   )

# Search paths
include_directories(${CMAKE_SOURCE_DIR})

include_directories(${GLIB2_INCLUDE_DIRS})
include_directories(${GDAL_INCLUDE_DIRS})
#include_directories("/opt/homebrew/opt/lcms2/include")

#include_directories(${HDF5_INCLUDE_DIRS})
#include_directories(${PugiXML_INCLUDE_DIR})
#include_directories(${OPENSSL_INCLUDE_DIR})
#include_directories(${LUA_INCLUDE_DIR})
#include_directories(SYSTEM ${Boost_INCLUDE_DIR})
#include_directories(${PROJECT_BINARY_DIR})

include_directories(/opt/homebrew/opt/lcms2/include/)

message("C++ compiler: " ${CMAKE_CXX_COMPILER})

# message ("HDF5 inc dir: " ${HDF5_INCLUDE_DIRS})

# message ("GEOLIB inc dir: " ${GeographicLib_INCLUDE_DIR})
# message ("GEOLIB lib: " ${GeographicLib_LIBRARY})

# message ("PugiXML inc dir: " ${PugiXML_INCLUDE_DIR})
# message ("PugiXML lib: " ${PugiXML_LIBRARY})

# include_directories(${GeographicLib_INCLUDE_DIR})
# include_directories(${SQLite3_INCLUDE_DIR})
# include_directories(${GDAL_INCLUDE_DIR})
#include_directories(${RapidJSON_INCLUDE_DIR})

# add_subdirectory(GISLibrary)
# add_subdirectory(GeoMetryLibrary)
# add_subdirectory(LuaScriptingReference)
# add_subdirectory(PortrayalCatalogue)
# add_subdirectory(FeatureCatalog)
# add_subdirectory(LatLonUtility)
# add_subdirectory(S100Engine)
# add_subdirectory(OpenS100)

# set(CMAKE_ASM_COMPILER /opt/homebrew/anaconda3/bin/as) 
# set(CMAKE_ASM_SOURCE_FILE_EXTENSIONS s) 
# set(CMAKE_ASM_OBJECT_FORMAT elf64)
# enable_language(C ASM)


#add_definitions(-DS52_USE_PROJ)
#include_directories(${PROJ_INCLUDE_DIRS})
#message("PROJ : ${PROJ_INCLUDE_DIRS}")


add_definitions(-DS52_USE_LCMS2 -DS52_USE_C_AGGR_C_ASSO)

set_property(SOURCE S52raz.s  PROPERTY LANGUAGE C)

# set_property(SOURCE S52raz.s  PROPERTY OUTPUT_NAME S52raz-3.2.rle.o)
# set_source_files_properties(S52raz.s PROPERTIES COMPILE_FLAGS "-f elf64")


add_library(s52
  S52.c
  S52CS.c
  S52MP.c
  S52PL.c
  S52utils.c
  S57data.c
  S57ogr.c
  parson.c
  S52raz.s
)


add_executable(s52_test
   main.cpp
)

# set(CMAKE_SWIG_FLAGS "")
# set(SWIG_CXX_EXTENSION "cpp")
# set(CMAKE_SWIG_OUTDIR ${CMAKE_CURRENT_BINARY_DIR}/csharp)
# set_source_files_properties(${CMAKE_SOURCE_DIR}/src/S101ToGpkgConverter.i PROPERTIES CPLUSPLUS ON)
# set_source_files_properties(${CMAKE_SOURCE_DIR}/src/S101ToGpkgConverter.i PROPERTIES SWIG_FLAGS "-module;S101ToGpkgConverter;")

# swig_add_library(S101ToGpkgConverter TYPE SHARED  LANGUAGE csharp  SOURCES ${CMAKE_SOURCE_DIR}/src/S101ToGpkgConverter.i)

set(S52_LIBS
  s52
  # ${HDF5_LIBRARIES}
  # ${OPENSSL_LIBRARIES}
  # ${PugiXML_LIBRARY}
  # ${LUA_LIBRARIES}
  # ${GeographicLib_LIBRARY}
  # ${LIBXML2_LIBRARIES}
  # ${LIBXSLT_LIBRARIES}
  # ${SQLite3_LIBRARIES}
  ${GDAL_LIBRARIES}
  #${GLIB2_LIBRARIES}
  ${PROJ_LIBRARIES}
  #${GLIB2_LIBRARIES}
  /opt/homebrew/opt/glib/lib/libglib-2.0.dylib
  /opt/homebrew/opt/lcms2/lib/liblcms2.dylib 
  # ZLIB::ZLIB
  # libzip::zip
  # s101
)

message("glib2 lib: ${GLIB2_LIBRARIES}")

# swig_link_libraries(S101ToGpkgConverter
#   ${S101_LIBS}
#   ${CMAKE_THREAD_LIBS_INIT}
# )

target_link_libraries(s52_test
  ${S52_LIBS}
)





# if(DOXYGEN_FOUND)
#   FIND_PACKAGE(Git)
#   if(GIT_FOUND)
#     execute_process(COMMAND ${GIT_EXECUTABLE} -C ${PROJECT_SOURCE_DIR} rev-parse --short HEAD
#       OUTPUT_VARIABLE GIT_REV
#       OUTPUT_STRIP_TRAILING_WHITESPACE)
#     execute_process(COMMAND ${GIT_EXECUTABLE} -C ${PROJECT_SOURCE_DIR} rev-parse --abbrev-ref HEAD
#       OUTPUT_VARIABLE GIT_BRANCH
#       OUTPUT_STRIP_TRAILING_WHITESPACE)
#     set(GIT_DOXYGEN_REV "\"${PROJECT_VERSION} [${GIT_BRANCH}: ${GIT_REV}]\"")
#   endif()
#   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/user_doc.in ${CMAKE_CURRENT_BINARY_DIR}/user_doc @ONLY)
#   add_custom_target(doc
#     ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/user_doc
#     WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#     COMMENT "Generating API documentation with Doxygen" VERBATIM
#   )
#   file( GLOB IMG_FILES "${PROJECT_SOURCE_DIR}/doc/*.png")
#   foreach(filename ${IMG_FILES})
# 	  file( COPY ${filename} DESTINATION ${CMAKE_BINARY_DIR}/doc/html )
#   endforeach()
#   install( DIRECTORY ${PROJECT_BINARY_DIR}/doc/html
#      		   DESTINATION ${INSTALL_BASE}/doc/${PROJECT_NAME}_${VAR_VERSION}
#    		)
# endif()

# Set library version header
#configure_file("${PROJECT_NAME}_version.hpp.in" "${CMAKE_BINARY_DIR}/${PROJECT_NAME}_version.hpp")
#message("binary dir set to: ${CMAKE_BINARY_DIR}")
#install(FILES "${CMAKE_BINARY_DIR}/${PROJECT_NAME}_version.hpp" DESTINATION ${INSTALL_BASE}/include)

################################################################################
# Configuration files for find_package matching                                #
################################################################################

# Configuration
# set(version_config "ConfigVersion.cmake")
# set(project_config "Config.cmake")
# set(config_install_dir ".")
# Include module with fuction 'write_basic_package_version_file'
#include(CMakePackageConfigHelpers)
# Configure '<PROJECT-NAME>Config.cmake'
# Use variables:
#   * targets_export_name
#   * PROJECT_NAME
# configure_package_config_file(
#     "Config.cmake.in"
#     "${project_config}"
#     INSTALL_DESTINATION "${config_install_dir}"
# )
# Configure '<PROJECT-NAME>ConfigVersion.cmake'
# Note: PROJECT_VERSION is used as a VERSION
# write_basic_package_version_file(
#     "${version_config}" COMPATIBILITY AnyNewerVersion
# )

# install(FILES ${CMAKE_BINARY_DIR}/${project_config} DESTINATION ${INSTALL_BASE}/cmake)
# install(FILES ${CMAKE_BINARY_DIR}/${version_config} DESTINATION ${INSTALL_BASE}/cmake)

# install(FILES ${PROJECT_NAME}_defs.hpp DESTINATION ${INSTALL_BASE}/include)

# install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_revision.hpp DESTINATION ${INSTALL_BASE}/include)
# install(FILES ${PROJECT_NAME}_defs.hpp DESTINATION ${INSTALL_BASE}/include)
