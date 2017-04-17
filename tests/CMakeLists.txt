

add_executable(testEval test.cpp ../ENLink.h)
TARGET_LINK_LIBRARIES(testEval EN2Link ${Boost_LIBRARIES})