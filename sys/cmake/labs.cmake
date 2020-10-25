#directory names
set(LIB_DIR ${CMAKE_SOURCE_DIR}/../lib)
set(SYS_DIR ${CMAKE_SOURCE_DIR}/../sys)
set(CMSIS_DIR ${SYS_DIR}/CMSIS)

#lib directories included
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${LIB_DIR}
)

set(LIBSRCS
    ${LIB_DIR}/adc.c
    ${LIB_DIR}/pinAccess.c
    ${LIB_DIR}/spi.c
	${LIB_DIR}/serial.cpp
	${LIB_DIR}/encoder.cpp
	${LIB_DIR}/fixmath.cpp
	${LIB_DIR}/svpwm.cpp
)

set(APP_SRCS ${SRCS} ${LIBSRCS})

#STM32F303 compiler/linker specific stuff
# => APP_SRCS and SYS_DIR should defined.
include("../sys/cmake/stm32f303.cmake")
