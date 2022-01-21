set(TARGET_NAME core)

add_library(${TARGET_NAME}
  base64.cpp
  bplist.cpp
  bplist_repr.cpp
  byte_buffer2.cpp
  file_util.cpp
  stream.cpp
  gzip.cpp
  plist_parser.cpp
  string_util.cpp
  time_stamp.cpp
  zlib.cpp
)

target_include_directories (${TARGET_NAME}
  INTERFACE ./
)

install (TARGETS ${TARGET_NAME}
  ARCHIVE DESTINATION lib/
)
