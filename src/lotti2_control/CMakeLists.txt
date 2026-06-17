cmake_minimum_required(VERSION 3.16)
project(lotti2_control LANGUAGES CXX)

# Include and lib directories
include_directories(include)
link_directories(lib)

# Unitree SDK library selection based on architecture
set(UNITREE_SDK_LIB "${CMAKE_CURRENT_SOURCE_DIR}/lib/libUnitreeMotorSDK_Linux64.so")

# Output path for libraries
set(LIBRARY_OUTPUT_PATH "../lib")

# Compiler options for GCC/Clang
if(CMAKE_CXX_COMPILER_ID MATCHES "(GNU|Clang)")
    add_compile_options(-Wall -Wextra)
endif()

find_package(ros2_control_cmake REQUIRED)
set_compiler_options()
export_windows_symbols()

# Ensure Windows export all symbols (for DLLs)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Hardware interface dependencies
set(HW_IF_INCLUDE_DEPENDS
    pluginlib
    rcpputils
    hardware_interface
    rclcpp
    rclcpp_lifecycle
)

# Specify the required version of ros2_control
find_package(controller_manager 4.0.0)
# Handle the case where the required version is not found
if(NOT controller_manager_FOUND)
    message(FATAL_ERROR "ros2_control version 4.0.0 or higher is required. ")
endif()

# Find dependencies
find_package(backward_ros REQUIRED)
find_package(ament_cmake REQUIRED)


foreach(Dependency IN ITEMS ${HW_IF_INCLUDE_DEPENDS})
    find_package(${Dependency} REQUIRED)
endforeach()

# Your main shared library
add_library(
    lotti2_control
    SHARED
    hardware/lotti2_flipper_interface.cpp
    hardware/lotti2_drive_interface.cpp
    hardware/lotti2_arm_interface.cpp
)

# Compile features and includes
target_compile_features(lotti2_control PUBLIC cxx_std_20)
target_include_directories(lotti2_control PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/hardware/include>
    $<INSTALL_INTERFACE:include/lotti2_control>
)

ament_target_dependencies(
    lotti2_control PUBLIC
    ${HW_IF_INCLUDE_DEPENDS}
)

# Link Unitree SDK library
target_link_libraries(lotti2_control PUBLIC serial ${UNITREE_SDK_LIB})

# Export hardware plugins
pluginlib_export_plugin_description_file(hardware_interface lotti2_control.xml)

# Install rules
install(
    DIRECTORY hardware/include/
    DESTINATION include/lotti2_control
)

install(
    DIRECTORY description/launch description/urdf description/srdf description/meshes
    DESTINATION share/lotti2_control/description
)

install(
    DIRECTORY description/ros2_control 
    DESTINATION share/lotti2_control
)

install(
    DIRECTORY bringup/launch bringup/config
    DESTINATION share/lotti2_control
)

install(TARGETS lotti2_control
    EXPORT export_lotti2_control
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# Testing
if(BUILD_TESTING)
    find_package(ament_cmake_pytest REQUIRED)
endif()

# Export targets and dependencies
ament_export_targets(export_lotti2_control HAS_LIBRARY_TARGET)
ament_export_dependencies(${HW_IF_INCLUDE_DEPENDS})

ament_package()
