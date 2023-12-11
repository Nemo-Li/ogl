file(GLOB_RECURSE extra_mesh extra_mesh/*.cpp extra_mesh/Common/*.cxx)

set(FBX_SDK_DIR "/Users/nemoli/Autodesk/FBX_SDK/2020.0.1")
include_directories("${FBX_SDK_DIR}/include")

#set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
find_library(FBXSDK_LIBRARY fbxsdk HINTS "${FBX_SDK_DIR}/lib/clang/debug")
message(${FBXSDK_LIBRARY})
message(${extra_mesh})
file(COPY ${FBXSDK_LIBRARY} DESTINATION "./")
find_library(FILESYSTEM_LIBRARY boost_filesystem HINTS "/usr/local/lib")
message(${FILESYSTEM_LIBRARY})
include_directories(extra_mesh)

add_executable(extra_mesh extraMesh.cpp ../engine/helpers/debug.cpp ${extra_mesh})
target_link_libraries(extra_mesh ${FBXSDK_LIBRARY} ${FILESYSTEM_LIBRARY})

add_executable(extra_animation extra_skeleton_animation.cpp ../engine/helpers/debug.cpp ${extra_mesh})
target_link_libraries(extra_animation ${FBXSDK_LIBRARY} ${FILESYSTEM_LIBRARY})
