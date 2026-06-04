// Based on:

// Copyright 2021 Department of Engineering Cybernetics, NTNU
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Changes made to the example by the Res.Q Bots Austria Team
// of Vienna University of Applied Sciences

#include "lotti2_control/lotti2_arm_interface.hpp"
// general purpose
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <limits>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
// ros2_control
#include "hardware_interface/lexical_casts.hpp"
#include "hardware_interface/types/hardware_interface_type_values.hpp"
#include "rclcpp/rclcpp.hpp"
// serial connection
#include "lotti2_control/arm_comms.hpp"


namespace lotti2_arm_interface {

hardware_interface::CallbackReturn ArmInterface::on_init(
  const hardware_interface::HardwareComponentInterfaceParams& params) {
    if (hardware_interface::SystemInterface::on_init(params) !=
        hardware_interface::CallbackReturn::SUCCESS) {
        return hardware_interface::CallbackReturn::ERROR;
    }

    // get the parameters from the ros2_control file
    device_             = info_.hardware_parameters["device"];
    gear_ratio_         = stof(info_.hardware_parameters["gear_ratio"]);
    motor_resolution_   = stoi(info_.hardware_parameters["resolution"]);
    motor_acceleration_ = static_cast<uint8_t>(stoi(info_.hardware_parameters["acceleration"]));
    use_hardware_       = stoi(info_.hardware_parameters["use_hardware"]);
    if (!(use_hardware_ == 0 || use_hardware_ == 1)) {
        RCLCPP_ERROR(get_logger(), "ArmInterface: Invalid value for \"use_hardware\" in ros2_control file");
    }
    use_sync_ = static_cast<uint8_t>(stoi(info_.hardware_parameters["use_synchronization"]));
    if (!(use_sync_ == 0 || use_sync_ == 1)) {
        RCLCPP_ERROR(get_logger(), "ArmInterface: Invalid value for \"use_synchronization\" in ros2_control file");
    }

    return CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn ArmInterface::on_configure(
  const rclcpp_lifecycle::State& /*previous_state*/) {
    // set current positional buffer and command buffer to 0
    for (std::size_t i = 0; i < info_.joints.size(); i++) {
        arm_pos_[i]     = 0;
        arm_spd_[i]     = 0;
        arm_pos_cmd_[i] = 0;
        arm_spd_cmd_[i] = 0;
    }

    // prepare serial connection only if use_hardware is set to "true"
    if (use_hardware_ == 1) {
        if (arm_comms_.connected()) {
            arm_comms_.disconnect();
        }
        arm_comms_.connect(device_);
        // set motor command mode
        arm_comms_.setReq(0x82, info_.joints.size(), 0x05);
    }

    // always reset values when configuring hardware
    for (const auto& [name, descr] : joint_state_interfaces_) {
        set_state(name, 0.0);
    }

    for (const auto& [name, descr] : joint_command_interfaces_) {
        set_command(name, 0.0);
    }

    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn ArmInterface::on_cleanup(
  const rclcpp_lifecycle::State& /*previous_state*/) {
    // if serial connection was established, close it
    if (arm_comms_.connected()) {
        arm_comms_.disconnect();
    }

    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn ArmInterface::on_activate(
  const rclcpp_lifecycle::State& /*previous_state*/) {
    if (use_hardware_ == 1) {
        if (!arm_comms_.connected()) {
            RCLCPP_ERROR(rclcpp::get_logger("ArmInterface"), "Motors not connected");
            return hardware_interface::CallbackReturn::ERROR;
        }
        // enable motors
        arm_comms_.setReq(0xF3, info_.joints.size(), 1);
        // set synchronous movement flag
        arm_comms_.setReq(0x4A, info_.joints.size(), use_sync_);
        // set zero positions
        arm_comms_.setReq(0x92, info_.joints.size(), 0);
    }
    // command and state should be equal when starting
    for (const auto& [name, descr] : joint_command_interfaces_) {
        set_command(name, get_state(name));
    }

    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn ArmInterface::on_deactivate(
  const rclcpp_lifecycle::State& /*previous_state*/) {
    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::return_type ArmInterface::read(
  const rclcpp::Time& /*time*/, const rclcpp::Duration& /*period*/) {
    // if use_hardware is set to 1 -> read real data
    if (use_hardware_ == 1) {
        if (!arm_comms_.connected()) {
            RCLCPP_ERROR(rclcpp::get_logger("ArmInterface"), "Motors not connected");
            return hardware_interface::return_type::ERROR;
        }

        for (std::size_t i = 0; i < info_.joints.size(); i++) {
            // set motor id to choose which motor to poll from
            uint8_t motor_id = static_cast<uint8_t>(i) + 1;
            // get motor position in motor steps and convert to radiant
            arm_pos_[i] = (static_cast<double>(arm_comms_.readPos(motor_id)) * 2 * M_PI) / (motor_resolution_ * gear_ratio_);
            // set state interface to current value
            set_state(info_.joints[i].name + "/position", arm_pos_[i]);
            // get motor speed in rpm and convert to rad/s
            arm_spd_[i] = (static_cast<double>(arm_comms_.readSpd(motor_id)) * 2 * M_PI) / (60 * motor_resolution_ * gear_ratio_);
            // set state interface to current value
            set_state(info_.joints[i].name + "/velocity", arm_spd_[i]);
        }
    }
    // if use_hardware is set to 0 -> pretend all commands are executed instantly
    else {
        for (std::size_t i = 0; i < info_.joints.size(); i++) {
            set_state(info_.joints[i].name + "/position", get_command(info_.joints[i].name + "/position"));
            set_state(info_.joints[i].name + "/velocity", get_command(info_.joints[i].name + "/velocity"));
        }
    }

    return hardware_interface::return_type::OK;
}

hardware_interface::return_type ArmInterface::write(
  const rclcpp::Time& /*time*/, const rclcpp::Duration& /*period*/) {
    // only perform write if use_hardware is set to 1
    if (use_hardware_ == 1) {
        for (std::size_t i = 0; i < info_.joints.size(); i++) {
            // select motor
            uint8_t motor_id = static_cast<uint8_t>(i) + 1;
            // convert arm position from radiant to motor steps
            arm_pos_cmd_[i] = static_cast<uint32_t>((get_command(info_.joints[i].name + "/position") * gear_ratio_ * motor_resolution_) / (2 * M_PI));
            // convert arm speed from rad/s to rpm
            arm_spd_cmd_[i] = static_cast<uint16_t>((get_command(info_.joints[i].name + "/velocity") * 60 * gear_ratio_) / (2 * M_PI));
            // send commands to arm comms
            arm_comms_.setArmValues(motor_id, arm_spd_cmd_[i], motor_acceleration_, arm_pos_cmd_[i]);
        }

        // the motors can be programmed to start movement on a command (this enables better synchronization)
        if (use_sync_ == 1) {
            arm_comms_.startSync();
        }
    }

    return hardware_interface::return_type::OK;
}
}  // namespace lotti2_arm_interface

#include "pluginlib/class_list_macros.hpp"
PLUGINLIB_EXPORT_CLASS(
  lotti2_arm_interface::ArmInterface, hardware_interface::SystemInterface)