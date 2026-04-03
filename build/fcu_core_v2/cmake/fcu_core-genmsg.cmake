# generated from genmsg/cmake/pkg-genmsg.cmake.em

message(STATUS "fcu_core: 1 messages, 0 services")

set(MSG_I_FLAGS "-Ifcu_core:/home/ubuntu/fcu_ws/src/fcu_core_v2/msg;-Istd_msgs:/opt/ros/noetic/share/std_msgs/cmake/../msg")

# Find all generators
find_package(gencpp REQUIRED)
find_package(geneus REQUIRED)
find_package(genlisp REQUIRED)
find_package(gennodejs REQUIRED)
find_package(genpy REQUIRED)

add_custom_target(fcu_core_generate_messages ALL)

# verify that message/service dependencies have not changed since configure



get_filename_component(_filename "/home/ubuntu/fcu_ws/src/fcu_core_v2/msg/uwb.msg" NAME_WE)
add_custom_target(_fcu_core_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "fcu_core" "/home/ubuntu/fcu_ws/src/fcu_core_v2/msg/uwb.msg" "std_msgs/Header"
)

#
#  langs = gencpp;geneus;genlisp;gennodejs;genpy
#

### Section generating for lang: gencpp
### Generating Messages
_generate_msg_cpp(fcu_core
  "/home/ubuntu/fcu_ws/src/fcu_core_v2/msg/uwb.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/noetic/share/std_msgs/cmake/../msg/Header.msg"
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/fcu_core
)

### Generating Services

### Generating Module File
_generate_module_cpp(fcu_core
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/fcu_core
  "${ALL_GEN_OUTPUT_FILES_cpp}"
)

add_custom_target(fcu_core_generate_messages_cpp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_cpp}
)
add_dependencies(fcu_core_generate_messages fcu_core_generate_messages_cpp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/ubuntu/fcu_ws/src/fcu_core_v2/msg/uwb.msg" NAME_WE)
add_dependencies(fcu_core_generate_messages_cpp _fcu_core_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(fcu_core_gencpp)
add_dependencies(fcu_core_gencpp fcu_core_generate_messages_cpp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS fcu_core_generate_messages_cpp)

### Section generating for lang: geneus
### Generating Messages
_generate_msg_eus(fcu_core
  "/home/ubuntu/fcu_ws/src/fcu_core_v2/msg/uwb.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/noetic/share/std_msgs/cmake/../msg/Header.msg"
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/fcu_core
)

### Generating Services

### Generating Module File
_generate_module_eus(fcu_core
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/fcu_core
  "${ALL_GEN_OUTPUT_FILES_eus}"
)

add_custom_target(fcu_core_generate_messages_eus
  DEPENDS ${ALL_GEN_OUTPUT_FILES_eus}
)
add_dependencies(fcu_core_generate_messages fcu_core_generate_messages_eus)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/ubuntu/fcu_ws/src/fcu_core_v2/msg/uwb.msg" NAME_WE)
add_dependencies(fcu_core_generate_messages_eus _fcu_core_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(fcu_core_geneus)
add_dependencies(fcu_core_geneus fcu_core_generate_messages_eus)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS fcu_core_generate_messages_eus)

### Section generating for lang: genlisp
### Generating Messages
_generate_msg_lisp(fcu_core
  "/home/ubuntu/fcu_ws/src/fcu_core_v2/msg/uwb.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/noetic/share/std_msgs/cmake/../msg/Header.msg"
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/fcu_core
)

### Generating Services

### Generating Module File
_generate_module_lisp(fcu_core
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/fcu_core
  "${ALL_GEN_OUTPUT_FILES_lisp}"
)

add_custom_target(fcu_core_generate_messages_lisp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_lisp}
)
add_dependencies(fcu_core_generate_messages fcu_core_generate_messages_lisp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/ubuntu/fcu_ws/src/fcu_core_v2/msg/uwb.msg" NAME_WE)
add_dependencies(fcu_core_generate_messages_lisp _fcu_core_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(fcu_core_genlisp)
add_dependencies(fcu_core_genlisp fcu_core_generate_messages_lisp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS fcu_core_generate_messages_lisp)

### Section generating for lang: gennodejs
### Generating Messages
_generate_msg_nodejs(fcu_core
  "/home/ubuntu/fcu_ws/src/fcu_core_v2/msg/uwb.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/noetic/share/std_msgs/cmake/../msg/Header.msg"
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/fcu_core
)

### Generating Services

### Generating Module File
_generate_module_nodejs(fcu_core
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/fcu_core
  "${ALL_GEN_OUTPUT_FILES_nodejs}"
)

add_custom_target(fcu_core_generate_messages_nodejs
  DEPENDS ${ALL_GEN_OUTPUT_FILES_nodejs}
)
add_dependencies(fcu_core_generate_messages fcu_core_generate_messages_nodejs)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/ubuntu/fcu_ws/src/fcu_core_v2/msg/uwb.msg" NAME_WE)
add_dependencies(fcu_core_generate_messages_nodejs _fcu_core_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(fcu_core_gennodejs)
add_dependencies(fcu_core_gennodejs fcu_core_generate_messages_nodejs)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS fcu_core_generate_messages_nodejs)

### Section generating for lang: genpy
### Generating Messages
_generate_msg_py(fcu_core
  "/home/ubuntu/fcu_ws/src/fcu_core_v2/msg/uwb.msg"
  "${MSG_I_FLAGS}"
  "/opt/ros/noetic/share/std_msgs/cmake/../msg/Header.msg"
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/fcu_core
)

### Generating Services

### Generating Module File
_generate_module_py(fcu_core
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/fcu_core
  "${ALL_GEN_OUTPUT_FILES_py}"
)

add_custom_target(fcu_core_generate_messages_py
  DEPENDS ${ALL_GEN_OUTPUT_FILES_py}
)
add_dependencies(fcu_core_generate_messages fcu_core_generate_messages_py)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/ubuntu/fcu_ws/src/fcu_core_v2/msg/uwb.msg" NAME_WE)
add_dependencies(fcu_core_generate_messages_py _fcu_core_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(fcu_core_genpy)
add_dependencies(fcu_core_genpy fcu_core_generate_messages_py)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS fcu_core_generate_messages_py)



if(gencpp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/fcu_core)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/fcu_core
    DESTINATION ${gencpp_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_cpp)
  add_dependencies(fcu_core_generate_messages_cpp std_msgs_generate_messages_cpp)
endif()

if(geneus_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/fcu_core)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/fcu_core
    DESTINATION ${geneus_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_eus)
  add_dependencies(fcu_core_generate_messages_eus std_msgs_generate_messages_eus)
endif()

if(genlisp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/fcu_core)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/fcu_core
    DESTINATION ${genlisp_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_lisp)
  add_dependencies(fcu_core_generate_messages_lisp std_msgs_generate_messages_lisp)
endif()

if(gennodejs_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/fcu_core)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/fcu_core
    DESTINATION ${gennodejs_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_nodejs)
  add_dependencies(fcu_core_generate_messages_nodejs std_msgs_generate_messages_nodejs)
endif()

if(genpy_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/fcu_core)
  install(CODE "execute_process(COMMAND \"/home/ubuntu/miniconda3/bin/python3\" -m compileall \"${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/fcu_core\")")
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/fcu_core
    DESTINATION ${genpy_INSTALL_DIR}
  )
endif()
if(TARGET std_msgs_generate_messages_py)
  add_dependencies(fcu_core_generate_messages_py std_msgs_generate_messages_py)
endif()
