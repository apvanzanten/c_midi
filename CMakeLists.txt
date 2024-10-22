cmake_minimum_required(VERSION 3.24) # this is the version I use, it may well be compatible with lower ones
project(c_midi)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(TST_DIR ${PROJECT_SOURCE_DIR}/tst)
set(INC_DIR ${PROJECT_SOURCE_DIR}/inc/cmidi)
set(DOC_DIR ${PROJECT_SOURCE_DIR}/doc)

set(CMAKE_C_STANDARD 11)

# --- fetch c facilities ---
include(FetchContent)
FetchContent_Declare(
  c_facilities
  GIT_REPOSITORY https://github.com/apvanzanten/c_facilities/
  GIT_TAG        origin/v2_develop
  )
FetchContent_MakeAvailable(c_facilities)
set(CFAC_INC_DIR ${c_facilities_SOURCE_DIR}/inc)
include_directories(${CFAC_INC_DIR})

set(WARNINGS 
    -Wall; 
    -Wextra; 
    -pedantic; 
    -Werror;)

set(SANITIZERS 
    -fsanitize=address; 
    -fsanitize=leak; 
    -fsanitize=undefined;
    -fno-omit-frame-pointer;
    )

set(DEBUG_FLAGS -Og; -g; -DDEBUG)
set(RELEASE_FLAGS -O3)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_compile_options(${WARNINGS})

if (DEBUG)
    add_compile_options(${DEBUG_FLAGS})
    add_compile_options(${SANITIZERS})
    add_link_options(${SANITIZERS})
else()
    add_compile_options(${RELEASE_FLAGS})
endif()

find_program(HAS_CPPCHECK NAMES cppcheck)
if (HAS_CPPCHECK)
    set(CMAKE_C_CPPCHECK 
                cppcheck;
                    --enable=all; 
                    --suppress=unusedFunction; # functions go unused in libraries a lot
                    --suppress=uninitvar; # generates some false positives, and anyway compilers do this pretty OK already
                    --suppress=missingIncludeSystem; # cppcheck can't find system headers and I don't care to help it
                    --suppress=unmatchedSuppression;
                    --suppress=redundantAssignment; # generates some false positives in some cases where functions with side-effects are used
                    --suppress=internalAstError; # cppcheck doesn't deal with generics very well, I don't particulary care to hear about it.
    )
endif()

# --- libraries ---

include_directories(${INC_DIR})

add_library(midi_note ${SRC_DIR}/note.c)

add_library(midi_message ${SRC_DIR}/message.c)
target_link_libraries(midi_message midi_note)

add_library(midi_decoder ${SRC_DIR}/decoder.c)
target_link_libraries(midi_decoder midi_note midi_message log)

add_library(midi_encoder ${SRC_DIR}/encoder.c)
target_link_libraries(midi_encoder midi_note midi_message log)

# --- tests ---

if (DEBUG) # For some reason cmake won't rebuild on test changes if this if statement is here :(
    enable_testing()

    add_library(midi_test_common ${TST_DIR}/test_common.c)
    target_link_libraries(midi_test_common midi_note midi_message)

    function(AddTest TEST_NAME TEST_SOURCE #[[test dependencies...]])
        add_executable(${TEST_NAME} ${TST_DIR}/${TEST_SOURCE})

        target_include_directories(${TEST_NAME} PUBLIC ${PROJECT_SOURCE_DIR} ${SRC_DIR})
        target_link_libraries(${TEST_NAME} test_utils midi_test_common ${ARGN})

        add_test(${TEST_NAME} ${TEST_NAME})
    endfunction()

    AddTest(message_test message.test.c midi_message midi_note)
    AddTest(note_test note.test.c midi_note)
    AddTest(decoder_test decoder.test.c midi_decoder midi_message midi_note darray)
    AddTest(encoder_test encoder.test.c midi_encoder midi_message midi_note darray)
    AddTest(encode_decode_roundtrip_test encode_decode_roundtrip.test.c midi_encoder midi_decoder midi_message midi_note darray)

endif()