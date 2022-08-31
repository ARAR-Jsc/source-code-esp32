project(xboard)

cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD_REQUIRED 14)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)


# Board selection
add_compile_definitions(BOARD_RASBERRY_PI=0)
add_compile_definitions(BOARD_ORANGE_PI=1)

add_compile_definitions(BOARD_BUILD=BOARD_ORANGE_PI)

set(CMAKE_CXX_FLAGS "-no-pie")

find_library(SQLITE sqlite3 HINTS /usr/local/lib)

option(USE_DLT "Using DLT for debugging" OFF)

find_package(PkgConfig REQUIRED)

if(USE_DLT)
    pkg_check_modules(DLT REQUIRED automotive-dlt)
    add_compile_definitions(LOG_USE_DLT=1)
else()
    add_compile_definitions(LOG_USE_DLT=0)
endif()

set(HDRS_LIB
    utils/Logger.h
    utils/Looper.h
	utils/Thread.h
    utils/SharedQueue.h
    utils/xtras/nlohmann/json.hpp
    utils/xtras/SqliteORM/sqlite_orm.h
	devices/uart/Uart.h
    devices/media/Speaker.h
    devices/thermal/Thermal.h
    devices/thermal/ThermalDS18B20.h
    database/Storage.h
	devices/i2c/I2C.h
	devices/rtc/IRtc.h
	devices/rtc/Ds3231.h
)

set(SRCS_LIB
    utils/Logger.cpp
    utils/Looper.cpp
	utils/Thread.cpp
	devices/uart/Uart.cpp
    devices/media/Speaker.cpp
    devices/thermal/Thermal.cpp
    devices/thermal/ThermalDS18B20.cpp
    database/Storage.cpp
	devices/i2c/I2C.cpp
	devices/rtc/IRtc.cpp
	devices/rtc/Ds3231.cpp
)

set(SRCS_WATERBOT
    app/CommonDefs.h
    app/CommonTypes.h
    app/waterbot.cpp
	app/sub_sys_comm/UartCmd.h
	app/sub_sys_comm/UartCmd.cpp
	app/sub_sys_comm/SubSysComm.h
	app/sub_sys_comm/SubSysComm.cpp
    app/sub_sys_rcm/SubSysRecommendation.h
    app/sub_sys_rcm/SubSysRecommendation.cpp
	app/time_sync/TimeSync.h
	app/time_sync/TimeSync.cpp
)

set(SRCS_TEST_UTILS
    test/testUtils.cpp
)

set(SRCS_TEST_SUB_SYS_COMM
	test/test_sub_sys_comm.cpp
    app/sub_sys_comm/UartCmd.h
	app/sub_sys_comm/UartCmd.cpp
	app/sub_sys_comm/SubSysComm.h
	app/sub_sys_comm/SubSysComm.cpp
)

set(SRCS_TEST_RTC
	test/TestRtc.cpp
	app/time_sync/TimeSync.h
	app/time_sync/TimeSync.cpp
)

set(SRCS_TEST_THERMAL
    test/testThermal.cpp
)

set(SRCS_TEST_DATABASE
    test/testDatabase.cpp
)

set(SRCS_TEST_MEDIA
    test/testMedia.cpp
)

set(SRCS_TEST_SUB_SYS_RCM
    test/test_sub_rcm.cpp
    app/sub_sys_rcm/SubSysRecommendation.h
    app/sub_sys_rcm/SubSysRecommendation.cpp
)


set(CUSTOMISED_LIBS
    pthread
    #/Compilers/SQLite_for_ARM/lib/libsqlite3.a
    #sqlite3
    asound
    stdc++fs
    ${SQLITE}
    ${DLT_LIBRARIES}
)

include_directories(
    ./
    ./app
    ./utils/xtras
    ${DLT_INCLUDE_DIRS}
)

add_executable( waterbot ${HDRS_LIB} ${SRCS_LIB} ${SRCS_WATERBOT})
target_link_libraries( waterbot
    ${CUSTOMISED_LIBS}
)

add_executable( testUtils ${HDRS_LIB} ${SRCS_LIB} ${SRCS_TEST_UTILS})
target_link_libraries( testUtils
    ${CUSTOMISED_LIBS}
)

add_executable( test_sub_sys_comm ${HDRS_LIB} ${SRCS_LIB} ${SRCS_TEST_SUB_SYS_COMM})
target_link_libraries( test_sub_sys_comm
    ${CUSTOMISED_LIBS}
)

add_executable( test_rtc ${HDRS_LIB} ${SRCS_LIB} ${SRCS_TEST_RTC})
target_link_libraries( test_rtc
    ${CUSTOMISED_LIBS}
)

add_executable( testThermal ${HDRS_LIB} ${SRCS_LIB} ${SRCS_TEST_THERMAL})
target_link_libraries( testThermal
    ${CUSTOMISED_LIBS}
)

add_executable( testDatabase ${HDRS_LIB} ${SRCS_LIB} ${SRCS_TEST_DATABASE})
target_link_libraries( testDatabase
    ${CUSTOMISED_LIBS}
)

add_executable( testMedia ${HDRS_LIB} ${SRCS_LIB} ${SRCS_TEST_MEDIA})
target_link_libraries( testMedia
    ${CUSTOMISED_LIBS}
)

add_executable( test_sub_rcm ${HDRS_LIB} ${SRCS_LIB} ${SRCS_TEST_SUB_SYS_RCM})
target_link_libraries( test_sub_rcm
    ${CUSTOMISED_LIBS}
)
