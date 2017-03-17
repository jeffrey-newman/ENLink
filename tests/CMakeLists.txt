

add_executable(testEval test.cpp ../EN2Link.h)
TARGET_LINK_LIBRARIES(testEval EN2Link ${Boost_LIBRARIES})