
cmake_minimum_required(VERSION 3.16)
project(lotti2_flipper_controller)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(backward_ros REQUIRED)
find_package(ros2_control_cmake REQUIRED)
set_compiler_options()
export_windows_symbols()

set(THIS_PACKAGE_INCLUDE_DEPENDS
    control_toolbox
    controller_interface
    generate_parameter_library
    hardware_interface
    pluginlib
    rcpputils
    rclcpp
    rclcpp_action
    rclcpp_lifecycle
    realtime_tools
    rsl
    #lotti2_msgs
    std_msgs
)

foreach(Dependency IN ITEMS ${THIS_PACKAGE_INCLUDE_DEPENDS})
    find_package(${Dependency} REQUIRED)
endforeach()

add_library(lotti2_flipper_controller SHARED
    src/lotti2_flipper_controller.cpp
)

target_compile_features(lotti2_flipper_controller PUBLIC cxx_std_17)
if(WIN32)
    target_compile_definitions(lotti2_flipper_controller PRIVATE _USE_MATH_DEFINES)
endif()

target_include_directories(lotti2_flipper_controller PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/lotti2_flipper_controller>
)

target_link_libraries(lotti2_flipper_controller PUBLIC
                      control_toolbox::control_toolbox
                      controller_interface::controller_interface
                      hardware_interface::hardware_interface
                      pluginlib::pluginlib
                      rclcpp::rclcpp
                      rclcpp_action::rclcpp_action
                      rclcpp_lifecycle::rclcpp_lifecycle
                      realtime_tools::realtime_tools
                      rsl::rsl
                     )

pluginlib_export_plugin_description_file(controller_interface lotti2_flipper_controller.xml)

if(BUILD_TESTING)
    find_package(ament_cmake_gmock REQUIRED)
    find_package(controller_manager REQUIRED)
    find_package(ros2_control_test_assets REQUIRED)
endif()

install(
    DIRECTORY include/
    DESTINATION include/lotti2_flipper_controller
)
install(TARGETS
        lotti2_flipper_controller
    EXPORT export_lotti2_flipper_controller
    RUNTIME DESTINATION bin
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

ament_export_targets(export_lotti2_flipper_controller HAS_LIBRARY_TARGET)
ament_export_dependencies(${THIS_PACKAGE_INCLUDE_DEPENDS})
ament_package()