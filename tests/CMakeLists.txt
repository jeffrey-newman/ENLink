

add_executable(testEval test.cpp ../ENLink.h)
TARGET_LINK_LIBRARIES(testEval ENLink ${Boost_LIBRARIES})