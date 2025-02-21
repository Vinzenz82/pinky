cmake_minimum_required(VERSION 3.10)

# set the project name and version
project(HelloWorld VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ----------------------------------------------------------------------
# lg
#
include(FetchContent)
FetchContent_Declare(
  lg
  GIT_REPOSITORY https://github.com/joan2937/lg.git
  GIT_TAG        b959a17
)
FetchContent_MakeAvailable(lg)

set(LG_SOURCE 
                ${lg_SOURCE_DIR}/lgCtx.c
                ${lg_SOURCE_DIR}/lgDbg.c
                ${lg_SOURCE_DIR}/lgErr.c
                ${lg_SOURCE_DIR}/lgGpio.c
                ${lg_SOURCE_DIR}/lgHdl.c
                ${lg_SOURCE_DIR}/lgI2C.c
                ${lg_SOURCE_DIR}/lgNotify.c
                ${lg_SOURCE_DIR}/lgPthAlerts.c
                ${lg_SOURCE_DIR}/lgPthTx.c
                ${lg_SOURCE_DIR}/lgSerial.c
                ${lg_SOURCE_DIR}/lgSPI.c
                ${lg_SOURCE_DIR}/lgThread.c
                ${lg_SOURCE_DIR}/lgUtil.c
)
#--------------------------------------------------------------------------

#--------------------------------------------------------------------------
# e-paper
#
FetchContent_Declare(
  epaper
  GIT_REPOSITORY https://github.com/Vinzenz82/ws-e-paper.git
  GIT_TAG ecdd8cf

)
FetchContent_MakeAvailable(epaper)

set(EPAPER_SOURCE
                #${epaper_SOURCE_DIR}/RaspberryPi_JetsonNano/c/examples/main.c
                #${epaper_SOURCE_DIR}/RaspberryPi_JetsonNano/c/examples/EPD_4in2_V2_test.c
                ${epaper_SOURCE_DIR}/RaspberryPi_JetsonNano/c/examples/ImageData.c
                ${epaper_SOURCE_DIR}/RaspberryPi_JetsonNano/c/lib/Config/DEV_Config.c
                ${epaper_SOURCE_DIR}/RaspberryPi_JetsonNano/c/lib/GUI/GUI_BMPfile.c
                ${epaper_SOURCE_DIR}/RaspberryPi_JetsonNano/c/lib/GUI/GUI_Paint.c
                ${epaper_SOURCE_DIR}/RaspberryPi_JetsonNano/c/lib/e-Paper/EPD_4in2_V2.c
                ${epaper_SOURCE_DIR}/RaspberryPi_JetsonNano/c/lib/Fonts/font20.c
                ${epaper_SOURCE_DIR}/RaspberryPi_JetsonNano/c/lib/Fonts/font16.c
                ${epaper_SOURCE_DIR}/RaspberryPi_JetsonNano/c/lib/Fonts/font12.c
                ${epaper_SOURCE_DIR}/RaspberryPi_JetsonNano/c/lib/Fonts/font24CN.c
                ${epaper_SOURCE_DIR}/RaspberryPi_JetsonNano/c/lib/Fonts/font12CN.c

)

#--------------------------------------------------------------------------

#--------------------------------------------------------------------------
# curl
# set(BUILD_STATIC_LIBS ON CACHE INTERNAL "Build static libraries")
# set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build shared libraries")
# set(BUILD_STATIC_CURL ON CACHE INTERNAL "Build curl executable with static libcurl")
set(HTTP_ONLY ON CACHE INTERNAL "disables all protocols except HTTP (This overrides all CURL_DISABLE_* options)")
set(CURL_ENABLE_SSL OFF CACHE INTERNAL "Enable SSL support")
#set(USE_LIBIDN2 OFF CACHE INTERNAL "Use libidn2 for IDN support")
set(BUILD_CURL_EXE OFF CACHE INTERNAL "Set to ON to build curl executable.")

FetchContent_Declare(
  curl
  GIT_REPOSITORY https://github.com/curl/curl.git
  GIT_TAG        172e54c
)
FetchContent_MakeAvailable(curl)
find_package(curl)

#--------------------------------------------------------------------------

#--------------------------------------------------------------------------
# json
#
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG        9cca280
)
FetchContent_MakeAvailable(json)

#--------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------------
# tools
# FetchContent_Declare(
#   image_converter
#   URL https://github.com/mtribiere/EPAPER-Image-Converter/archive/refs/heads/main.zip
#   SOURCE_DIR ${PROJECT_SOURCE_DIR}/tools

# )
# FetchContent_MakeAvailable(image_converter)

#-----------------------------------------------------------------------------------------------

# add the executable
add_executable(HelloWorld src/main.cxx ${LG_SOURCE} ${EPAPER_SOURCE} )

target_include_directories(HelloWorld PUBLIC 
                            ${lg_SOURCE_DIR}
                            ${epaper_SOURCE_DIR}
                            ${epaper_SOURCE_DIR}/RaspberryPi_JetsonNano/c/lib/Config
                            ${epaper_SOURCE_DIR}/RaspberryPi_JetsonNano/c/lib/e-Paper
                            ${epaper_SOURCE_DIR}/RaspberryPi_JetsonNano/c/lib/GUI
                            ${epaper_SOURCE_DIR}/RaspberryPi_JetsonNano/c/lib/Fonts
                            ${epaper_SOURCE_DIR}/RaspberryPi_JetsonNano/c/examples/
                            ${json_SOURCE_DIR}/single_include/nlohmann
                            ${curl_SOURCE_DIR}/include/curl
                            ${PROJECT_SOURCE_DIR}/src/epaper
)

target_compile_definitions(HelloWorld PUBLIC
                            RPI
                            USE_LGPIO_LIB
                            epd4in2V2
)                           

target_compile_options(HelloWorld PUBLIC
                      #-v
                    #   -mthumb 
                    #   -mfpu=neon-vfpv4 
                    #   -mfloat-abi=hard 
                    #   -mcpu=cortex-a7 
                    #   -fstack-protector-strong  
                    #   -O2 
                    #   -D_FORTIFY_SOURCE=2 
                    #   -Wformat=1 
                    #   -Wformat-security 
                    #   -Werror=format-security 
                    #   --sysroot=${TOOLCHAIN_SYSROOT}
                 )

target_link_options(HelloWorld PUBLIC 
                    #  -Wl,-O1 
                    #  -Wl,--hash-style=gnu 
                    #  -Wl,--as-needed 
                    #  -Wl,-z,relro,-z,now
                    # --sysroot=${TOOLCHAIN_SYSROOT}
                    #--static
                     )

target_link_directories(HelloWorld PRIVATE
                        ${PROJECT_SOURCE_DIR}/src/external
                        )

target_link_libraries(HelloWorld PRIVATE m CURL::libcurl z)

# add_custom_command(TARGET HelloWorld POST_BUILD
# #                    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
# #                    COMMAND python3 -m venv ${CMAKE_CURRENT_BINARY_DIR}/.venv
# #                    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/.venv/bin/pip install patchelf
# #                    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/.venv/bin/patchelf --set-interpreter /lib/ld-linux-armhf.so.3 ${CMAKE_CURRENT_BINARY_DIR}/HelloWorld
#                     COMMAND scp ${CMAKE_CURRENT_BINARY_DIR}/HelloWorld scp://vinzenz@192.168.1.209
#                     #COMMAND scp ${PROJECT_SOURCE_DIR}/src/pictures/cat_mouse.bmp scp://vinzenz@192.168.1.209
#                   )

