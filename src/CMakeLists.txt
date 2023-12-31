find_program(
  GENERATE_METASPRITES
  generate-metasprites
  PATHS "${CMAKE_SOURCE_DIR}/tools"
)

if (NOT GENERATE_METASPRITES)
  message(FATAL_ERROR "The generate-metasprites tool is required!")
endif()

add_custom_command(
  OUTPUT metasprites.s ${CMAKE_CURRENT_BINARY_DIR}/metasprites.hpp
  COMMAND ${GENERATE_METASPRITES} generate ${CMAKE_CURRENT_BINARY_DIR}/metasprites.s ${CMAKE_CURRENT_BINARY_DIR}/metasprites.hpp ${CMAKE_SOURCE_DIR}/assets/metasprites.nss --bank .prg_rom_1 --label-prefix metasprite
  DEPENDS ${GENERATE_METASPRITES} ${CMAKE_SOURCE_DIR}/assets/metasprites.nss
)

add_library(SourceObj
  OBJECT

  config.s

  main.cpp
  maestro.cpp
  title-screen.cpp
  music-editor.cpp

  banked-asset-helpers.s
  banked-asset-helpers.cpp
  camera.cpp
  ggsound.cpp
  log.cpp
  zx02.s

  assets.s
  soundtrack.hpp

  ${CMAKE_CURRENT_BINARY_DIR}/metasprites.s
)

set_property(
  SOURCE
  assets.s
  PROPERTY
  OBJECT_DEPENDS

  BG.chr
  SPR.chr
  
  BG.pal
  SPR.pal
  
  title.nam.zx02
  music-editor.nam.zx02
)

add_raw_asset(SOURCE "BG.chr")
add_raw_asset(SOURCE "SPR.chr")

add_raw_asset(SOURCE "BG.pal")
add_raw_asset(SOURCE "SPR.pal")
add_zx02_asset(SOURCE "title.nam")
add_zx02_asset(SOURCE "music-editor.nam" ALT "music-editor-alt.nam")

set_property(
  SOURCE
  soundtrack.s
  PROPERTY
  OBJECT_DEPENDS
  ${CMAKE_CURRENT_BINARY_DIR}/soundtrack.asm
  ${CMAKE_CURRENT_BINARY_DIR}/soundtrack_dpcm.asm
  ${CMAKE_CURRENT_SOURCE_DIR}/ggsound.inc
)

set_target_properties(SourceObj PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(
  SourceObj
  PRIVATE
  ${CMAKE_BINARY_DIR}/assets
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
)
