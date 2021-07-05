cmake_minimum_required(VERSION 3.5)
project(kaldi_gst)

include(cmake/kaldi.cmake)

include_directories(${openfst_SOURCE_DIR}/src/include)

find_program(BASH bash HINTS /bin)
find_program(MARSHAL glib-genmarshal)

find_package(PkgConfig)
pkg_search_module(JANSSON REQUIRED jansson)
pkg_search_module(GST REQUIRED gstreamer-1.0)
pkg_search_module(GST_AUDIO REQUIRED gstreamer-audio-1.0)
pkg_search_module(GLIB REQUIRED glib-2.0)

message(STATUS "GST_LIBRARIES: ${GST_LIBRARIES}")
message(STATUS "GST_INCLUDE_DIRS: ${GST_INCLUDE_DIRS}")
message(STATUS "JANSSON_LIBRARIES: ${JANSSON_LIBRARIES}")
message(STATUS "JANSSON_INCLUDE_DIRS: ${JANSSON_INCLUDE_DIRS}")
message(STATUS "GLIB_LIBRARIES: ${GLIB_LIBRARIES}")
message(STATUS "GLIB_INCLUDE_DIRS: ${GLIB_INCLUDE_DIRS}")
message(STATUS "GST_AUDIO_LIBRARIES: ${GST_AUDIO_LIBRARIES}")
message(STATUS "GST_AUDIO_INCLUDE_DIRS: ${GST_AUDIO_INCLUDE_DIRS}")

include_directories(${JANSSON_INCLUDE_DIRS})
include_directories(${GST_INCLUDE_DIRS})
include_directories(${GST_AUDIO_INCLUDE_DIRS})
include_directories(${GLIB_INCLUDE_DIRS})


set(MARSHAL_LIST "${PROJECT_SOURCE_DIR}/kaldimarshal.list")
execute_process(COMMAND ${MARSHAL} --header --prefix=kaldi_marshal ${MARSHAL_LIST}
                WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                OUTPUT_VARIABLE MARSHAL_HEADER_OUT
                RESULT_VARIABLE MARSHAL_HEADER_RES)

if(NOT ${MARSHAL_HEADER_RES})
    message(STATUS "write marshal header file.")
    file(WRITE ${PROJECT_SOURCE_DIR}/kaldimarshal.h ${MARSHAL_HEADER_OUT})
else()
    message(FATAL_ERROR "can't generate marshal header file." )
endif()

execute_process(COMMAND ${MARSHAL} --body --prefix=kaldi_marshal ${MARSHAL_LIST}
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        OUTPUT_VARIABLE MARSHAL_SOURCE_OUT
        RESULT_VARIABLE MARSHAL_SOURCE_RES)

if(NOT ${MARSHAL_SOURCE_RES})
    message(STATUS "write marshal source file.")
    file(WRITE ${PROJECT_SOURCE_DIR}/kaldimarshal.cc ${MARSHAL_SOURCE_OUT})
else()
    message(FATAL_ERROR "can't generate marshal source file." )
endif()

set(GST_KALDI_DEPS kaldi-online2 kaldi-lat kaldi-decoder kaldi-feat kaldi-transform
        kaldi-gmm kaldi-hmm kaldi-tree kaldi-matrix  kaldi-util kaldi-base kaldi-lm
        kaldi-nnet2 kaldi-nnet3 kaldi-cudamatrix kaldi-ivector kaldi-fstext kaldi-chain)

set(EXTRA_DEPS -lpthread -lrt)


add_library(gstkaldinnet2onlinedecoder SHARED
        kaldimarshal.cc
        gst_audio_source.cc
        gstkaldinnet2onlinedecoder.cc
        simple_option.cc)

add_library(gstkaldinnet2onlinedecoder-shared SHARED
        kaldimarshal.cc
        gst_audio_source.cc
        simple_option.cc
        gstkaldinnet2onlinedecoder-shared.cc)

add_executable(gst-plugin-test
        test/gst-plugin-test.cpp)

target_compile_definitions(gstkaldinnet2onlinedecoder PRIVATE -DPIC)

target_link_libraries(gstkaldinnet2onlinedecoder
        ${GST_KALDI_DEPS}
        ${JANSSON_LIBRARIES}
        ${GST_LIBRARIES}
        ${GST_AUDIO_LIBRARIES}
        ${GLIB_LIBRARIES}
        ${EXTRA_DEPS}
        jansson)

#target_link_libraries(gstkaldinnet2onlinedecoder
#        kaldi-lat kaldi-online2 kaldi-decoder kaldi-feat kaldi-transform
#        kaldi-gmm kaldi-hmm kaldi-tree kaldi-matrix  kaldi-util kaldi-base kaldi-lm
#        kaldi-nnet2 kaldi-nnet3 kaldi-cudamatrix kaldi-ivector kaldi-fstext kaldi-chain)
#

target_link_libraries(gst-plugin-test
        gstkaldinnet2onlinedecoder
        ${JANSSON_LIBRARIES}
        ${GST_LIBRARIES}
        ${GST_AUDIO_LIBRARIES}
        ${GLIB_LIBRARIES}
        ${EXTRA_DEPS}
        jansson)
