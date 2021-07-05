cmake_minimum_required(VERSION 3.14)
include(FetchContent)

FetchContent_Declare(
        kaldi
        GIT_REPOSITORY  https://github.com/kaldi-asr/kaldi
        GIT_TAG         master
)

FetchContent_GetProperties(kaldi)
if(NOT kaldi_POPULATED)
    FetchContent_Populate(kaldi)
    set(KALDI_INCLUDE ${kaldi_SOURCE_DIR}/src)
    include_directories(${KALDI_INCLUDE})
    add_subdirectory(${kaldi_SOURCE_DIR} ${kaldi_BINARY_DIR})
endif()