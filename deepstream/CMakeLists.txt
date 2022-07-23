cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

project(plugins LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 14)
find_package(CUDA REQUIRED)
enable_language(CUDA)
find_package(OpenCV REQUIRED)
find_package(PkgConfig)
pkg_check_modules(GST REQUIRED gstreamer-1.0>=1.4
                               gstreamer-sdp-1.0>=1.4
                               gstreamer-video-1.0>=1.4
                               gstreamer-app-1.0>=1.4)

if(DEFINED TensorRT_DIR)
  include_directories("${TensorRT_DIR}/include")
  link_directories("${TensorRT_DIR}/lib")
endif(DEFINED TensorRT_DIR)
if(DEFINED DeepStream_DIR)
  include_directories("nvdsincludes")
  link_directories("${DeepStream_DIR}/lib")
endif(DEFINED DeepStream_DIR)
include_directories(${CUDA_INCLUDE_DIRS})
include_directories(${GST_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/common/includes)
include_directories(${PROJECT_SOURCE_DIR}/common/nvdsincludes)

get_filename_component(PARENT_DIR ../ ABSOLUTE)
message(STATUS "====================${PLATFORM_TEGRA}")
# message(CMAKE_BINARY_DIR "====================${CMAKE_BINARY_DIR}")
# message(CMAKE_SOURCE_DIR "====================${CMAKE_SOURCE_DIR}")
# message(CMAKE_CURRENT_BINARY_DIR "====================${CMAKE_CURRENT_BINARY_DIR}")
# message(CMAKE_CURRENT_SOURCE_DIR "====================${CMAKE_CURRENT_SOURCE_DIR}")
message(PARENT_DIR "====================${PARENT_DIR}")
# message(TensorRT_DIR "====================${TensorRT_DIR}")
message(PROJECT_SOURCE_DIR "====================${PROJECT_SOURCE_DIR}")

include_directories(${OpenCV_INCLUDE_DIRS})

# add_subdirectory(${PARENT_DIR}/protos ${PARENT_DIR}/protos)

# find_package(Protobuf REQUIRED)
# include_directories(${CMAKE_BINARY_DIR}/protos)
# include_directories(${PARENT_DIR}/protos)

include_directories(${PARENT_DIR}/includes)
include_directories(${PARENT_DIR}/nvdsincludes)

add_library(spoof_utils SHARED
  ../common/src/detect_meta.cpp
  ../common/src/utils.cpp
  ../common/includes/detect_meta.h
  ../common/includes/utils.h
)

target_link_libraries(spoof_utils PUBLIC ${OpenCV_LIBS} cuda ${CUDA_LIBRARIES} ${LIBCRYPTOPP})

add_library(gstantispoof SHARED
  gst-antispoof/gstantispoof.h
  gst-antispoof/gstantispoof.cpp
)

# target_link_libraries(gstpgieparse PUBLIC ${PROJECT_SOURCE_DIR}/pgieparser)
# target_include_directories(gstpgieparse PUBLIC ${PROJECT_SOURCE_DIR}/pgieparser)

set_target_properties(gstantispoof PROPERTIES
  LINKER_LANGUAGE CXX
)

# target_include_directories(gstpgieparse PRIVATE "${PARENT_DIR}/plugins/pgie-parser")

# add_executable(main main.cpp )
if(DEFINED PLATFORM_TEGRA)
  add_definitions(-DPLATFORM_TEGRA=1)
  set(PLUGINS "/usr/lib/aarch64-linux-gnu/gstreamer-1.0")
else()
  set(PLUGINS "/usr/lib/x86_64-linux-gnu/gstreamer-1.0")
endif(DEFINED PLATFORM_TEGRA)
target_link_libraries(gstantispoof PRIVATE spoof_utils nvinfer nvinfer_plugin nvonnxparser nvdsparsebbox_retinaface ${OpenCV_LIBS} 
                      ${GST_LIBRARIES}
                      cuda ${CUDA_LIBRARIES} 
                      nvdsgst_meta
                      nvds_meta 
                      nvbufsurface
                      gstreamer-1.0 
                      gstvideo-1.0
                      gstbase-1.0
                      glib-2.0
                      gobject-2.0)
  set_target_properties(gstantispoof PROPERTIES INSTALL_RPATH_USE_LINK_PATH ON)


# message(NVDS_VERSION "====================${NVDS_VERSION}")
# message(RUNTIME "====================${RUNTIME}")
# message(LIBRARY "====================${PROJECT_SOURCE_DIR}")
# message(DESTINATION "====================${DESTINATION}")

install(
  TARGETS gstantispoof
  LIBRARY DESTINATION ${PLUGINS}/lib
)

install(
  TARGETS spoof_utils 
  LIBRARY DESTINATION lib
)
