cmake_minimum_required(VERSION 3.5.1)
project(epoll_try)

set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_BUILD_TYPE Release)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -march=native -msse4.2 -g")
add_definitions(-DRUN_LOCALL_DEBUG)

set(SOURCE_FILES main.cpp Buffer.cpp Buffer.h Routing.cpp Routing.h storage.cpp storage.h model/User.h model/Location.h model/Visit.h fileReade.cpp fileReade.h model/User.cpp Util.cpp Util.h model/Location.cpp Const.cpp Const.h handlers/UserGetHandler.cpp handlers/UserGetHandler.h SimpleLog.h handlers/LocationGetHandler.cpp handlers/LocationGetHandler.h handlers/VisitGetHandler.cpp handlers/VisitGetHandler.h handlers/LocationPostHandler.cpp handlers/LocationPostHandler.h handlers/VisitPostHandler.cpp handlers/VisitPostHandler.h handlers/UserPostHandler.cpp handlers/UserPostHandler.h model/Visit.cpp)
#SET(CMAKE_EXE_LINKER_FLAGS "-static")
add_executable(epoll_try ${SOURCE_FILES})