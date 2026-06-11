// Based on:

// Copyright 2023 ros2_control Development Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//         http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Changes made to the example by the Res.Q Bots Austria Team
// of Vienna University of Applied Sciences

#include "lotti2_flipper_controller/lotti2_flipper_controller.hpp"

#include <stddef.h>
#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "rclcpp/qos.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp/time.hpp"
#include "rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp"
#include "rclcpp_lifecycle/state.hpp"

#include "std_msgs/msg/float32.h"
// #include "lotti2_msgs/msg/flipper.hpp"


using config_type = controller_interface::interface_configuration_type;

namespace lotti2_flipper_controller {
FlipperController::FlipperController()
    : controller_interface::ControllerInterface() {}

controller_interface::CallbackReturn FlipperController::on_init() {
    // should have error handling
    joint_names_ = auto_declare<std::vector<std::string>>("joints", joint_names_);
    command_interface_types_ =
      auto_declare<std::vector<std::string>>("command_interfaces", command_interface_types_);
    state_interface_types_ =
      auto_declare<std::vector<std::string>>("state_interfaces", state_interface_types_);

    return CallbackReturn::SUCCESS;
}

controller_interface::InterfaceConfiguration FlipperController::command_interface_configuration() const {
    controller_interface::InterfaceConfiguration conf = {config_type::INDIVIDUAL, {}};

    conf.names.reserve(joint_names_.size() * command_interface_types_.size());
    for (const auto &joint_name : joint_names_) {
        for (const auto &interface_type : command_interface_types_) {
            conf.names.push_back(joint_name + "/" + interface_type);
        }
    }

    return conf;
}

controller_interface::InterfaceConfiguration FlipperController::state_interface_configuration() const {
    controller_interface::InterfaceConfiguration conf = {config_type::INDIVIDUAL, {}};

    conf.names.reserve(joint_names_.size() * state_interface_types_.size());
    for (const auto &joint_name : joint_names_) {
        for (const auto &interface_type : state_interface_types_) {
            conf.names.push_back(joint_name + "/" + interface_type);
        }
    }

    return conf;
}

controller_interface::CallbackReturn FlipperController::on_configure(const rclcpp_lifecycle::State &) {
    // define callbacks for subscribers
    auto fr_callback = [this](std_msgs::msg::Float32 fr_flipper_msg_) -> void {
        flipper_cmd_[0] = fr_flipper_msg_.data;
    };

    auto fl_callback = [this](std_msgs::msg::Float32 fl_flipper_msg_) -> void {
        flipper_cmd_[1] = fl_flipper_msg_.data;
    };

    auto rr_callback = [this](std_msgs::msg::Float32 rr_flipper_msg_) -> void {
        flipper_cmd_[2] = rr_flipper_msg_.data;
    };

    auto rl_callback = [this](std_msgs::msg::Float32 rl_flipper_msg_) -> void {
        flipper_cmd_[3] = rl_flipper_msg_.data;
    };

    // configure subscribers to listen to command topics from teleop node
    fr_flipper_command_subscriber_ =
      get_node()->create_subscription<std_msgs::msg::Float32>(
        "/cmd/fr_flipper", 1, fr_callback);

    fl_flipper_command_subscriber_ =
      get_node()->create_subscription<std_msgs::msg::Float32>(
        "/cmd/fl_flipper", 1, fl_callback);

    rr_flipper_command_subscriber_ =
      get_node()->create_subscription<std_msgs::msg::Float32>(
        "/cmd/rr_flipper", 1, rr_callback);

    rl_flipper_command_subscriber_ =
      get_node()->create_subscription<std_msgs::msg::Float32>(
        "/cmd/rl_flipper", 1, rl_callback);


    for (std::size_t i = 0; i < joint_names_.size(); i++) {
        flipper_cmd_[i] = 0.0;
    }

    return CallbackReturn::SUCCESS;
}

controller_interface::CallbackReturn FlipperController::on_activate(const rclcpp_lifecycle::State &) {
    // clear out vectors in case of restart
    joint_velocity_command_interface_.clear();
    joint_position_state_interface_.clear();
    joint_velocity_state_interface_.clear();
    joint_effort_state_interface_.clear();
    joint_temp_state_interface_.clear();

    // assign command interfaces
    for (auto &interface : command_interfaces_) {
        command_interface_map_[interface.get_interface_name()]->push_back(interface);
    }

    // assign state interfaces
    for (auto &interface : state_interfaces_) {
        state_interface_map_[interface.get_interface_name()]->push_back(interface);
    }

    return CallbackReturn::SUCCESS;
}

controller_interface::return_type FlipperController::update(
  const rclcpp::Time & /*time*/, const rclcpp::Duration & /*period*/) {
    for (std::size_t i = 0; i < joint_names_.size(); i++) {
        (void)joint_velocity_command_interface_[i].get().set_value(flipper_cmd_[i] * max_speed_);
    }

    return controller_interface::return_type::OK;
}

controller_interface::CallbackReturn FlipperController::on_deactivate(const rclcpp_lifecycle::State &) {
    return CallbackReturn::SUCCESS;
}
}  // namespace lotti2_flipper_controller

#include "pluginlib/class_list_macros.hpp"
PLUGINLIB_EXPORT_CLASS(lotti2_flipper_controller::FlipperController, controller_interface::ControllerInterface)
