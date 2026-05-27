// Based on:

// Copyright 2021 ros2_control Development Team
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

#include "lotti2_control/lotti2_drive_interface.hpp"
// general purpose
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <limits>
#include <memory>
#include <sstream>
#include <vector>
// ros2_control
#include "hardware_interface/lexical_casts.hpp"
#include "hardware_interface/types/hardware_interface_type_values.hpp"
#include "rclcpp/rclcpp.hpp"
// hardware
#include <unistd.h>
#include "lotti2_control/cubeMars_motor.h"
#include "lotti2_control/drive_comms.hpp"

namespace lotti2_drive_interface {
hardware_interface::CallbackReturn DriveInterface::on_init(
  const hardware_interface::HardwareComponentInterfaceParams& params) {
    if (hardware_interface::SystemInterface::on_init(params) !=
        hardware_interface::CallbackReturn::SUCCESS) {
        return hardware_interface::CallbackReturn::ERROR;
    }

    // get the parameters from the ros2_control file
    device_       = info_.hardware_parameters["device"].c_str();
    e_conv_       = stof(info_.hardware_parameters["e_conversion"]);
    gear_ratio_   = stof(info_.hardware_parameters["gear_ratio"]);
    kt_           = stof(info_.hardware_parameters["Kt"]);
    use_hardware_ = std::__cxx11::stoi(info_.hardware_parameters["use_hardware"]);
    if (!(use_hardware_ == 0 || use_hardware_ == 1)) {
        RCLCPP_ERROR(get_logger(), "DriveInterface: Invalid value for \"use_hardware\" in ros2_control file");
    }

    for (const hardware_interface::ComponentInfo& joint : info_.joints) {
        // DiffBotSystem has exactly two states and one command interface on each joint
        if (joint.command_interfaces.size() != 1) {
            RCLCPP_FATAL(
              get_logger(), "Joint '%s' has %zu command interfaces found. 1 expected.",
              joint.name.c_str(), joint.command_interfaces.size());
            return hardware_interface::CallbackReturn::ERROR;
        }

        if (joint.command_interfaces[0].name != hardware_interface::HW_IF_VELOCITY) {
            RCLCPP_FATAL(
              get_logger(), "Joint '%s' have %s command interfaces found. '%s' expected.",
              joint.name.c_str(), joint.command_interfaces[0].name.c_str(),
              hardware_interface::HW_IF_VELOCITY);
            return hardware_interface::CallbackReturn::ERROR;
        }

        if (joint.state_interfaces[0].name != hardware_interface::HW_IF_POSITION) {
            RCLCPP_FATAL(
              get_logger(), "Joint '%s' have '%s' as first state interface. '%s' expected.",
              joint.name.c_str(), joint.state_interfaces[0].name.c_str(),
              hardware_interface::HW_IF_POSITION);
            return hardware_interface::CallbackReturn::ERROR;
        }

        if (joint.state_interfaces[1].name != hardware_interface::HW_IF_VELOCITY) {
            RCLCPP_FATAL(
              get_logger(), "Joint '%s' have '%s' as second state interface. '%s' expected.",
              joint.name.c_str(), joint.state_interfaces[1].name.c_str(),
              hardware_interface::HW_IF_VELOCITY);
            return hardware_interface::CallbackReturn::ERROR;
        }
    }

    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn DriveInterface::on_configure(
  const rclcpp_lifecycle::State& /*previous_state*/) {
    // set default motor states
    for (std::size_t i = 0; i < info_.joints.size(); i++) {
        uint8_t motor_id;
        if (i == 0) {
            motor_id = 7;
        }
        else if (i == 1) {
            motor_id = 63;
        }
        motorStates_[i].motor_id = motor_id;
        // set all states to 0
        motorStates_[i].velocity   = 0.0;
        motorStates_[i].current    = 0.0;
        motorStates_[i].motor_temp = 0.0;
        motorStates_[i].error_code = 0;
        // set all commands to 0
        motorCommands_[i].motor_id = motor_id;
        motorCommands_[i].speed    = 0.0;
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

hardware_interface::CallbackReturn DriveInterface::on_cleanup(
  const rclcpp_lifecycle::State& /*previous_state*/) {
    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn DriveInterface::on_activate(
  const rclcpp_lifecycle::State& /*previous_state*/) {
    // only prepare can connection, if use_hardware is set to 1
    if (use_hardware_ == 1) {
        drive_comms_.open(device_);
    }

    // command and state should be equal when starting
    for (const auto& [name, descr] : joint_command_interfaces_) {
        set_command(name, get_state(name));
    }

    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn DriveInterface::on_deactivate(
  const rclcpp_lifecycle::State& /*previous_state*/) {
    // only necessary, if use_hardware is set to 1
    if (use_hardware_ == 1) {
        drive_comms_.close();
    }

    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::return_type DriveInterface::read(
  const rclcpp::Time& /*time*/, const rclcpp::Duration& period) {
    // if use_hardware is set to 1 -> read real data
    if (use_hardware_ == 1) {
        // read can buffer and extract state interface data from motorStates_
        for (size_t i = 0; i < info_.joints.size(); i++) {
            motorStates_[i] = drive_comms_.readCANFrame(motorStates_[i].motor_id);
            // switch for error cases
            switch (motorStates_[i].error_code) {
                case 1:
                    RCLCPP_ERROR(get_logger(), "DriveInterface: Motror with ID %d overheating!", motorStates_[i].motor_id);
                    break;
                case 2:
                    RCLCPP_ERROR(get_logger(), "DriveInterface: Motror with ID %d over current!", motorStates_[i].motor_id);
                    break;
                case 3:
                    RCLCPP_ERROR(get_logger(), "DriveInterface: Motror with ID %d over voltage!", motorStates_[i].motor_id);
                    break;
                case 4:
                    RCLCPP_ERROR(get_logger(), "DriveInterface: Motror with ID %d under voltage!", motorStates_[i].motor_id);
                    break;
                case 5:
                    RCLCPP_ERROR(get_logger(), "DriveInterface: Motror with ID %d has encoder error!", motorStates_[i].motor_id);
                    break;
                case 6:
                    RCLCPP_ERROR(get_logger(), "DriveInterface: Motror with ID %d phase current unbalanced. Hardware might be damaged!", motorStates_[i].motor_id);
                    break;
                // if no errors occurred, parse motorStates data to state interfaces
                default:
                    // get actual speed from eRPM: / (e_conv * gear_ratio) to get motor rpm, then axis rpm, * (2Pi/60) to convert RPM into rad/s
                    set_state(info_.joints[i].name + "/velocity", (motorStates_[i].velocity / (e_conv_ * gear_ratio_)) * (2 * M_PI / 60));
                    // calc torque from current using Kt value
                    set_state(info_.joints[i].name + "/effort", motorStates_[i].current * kt_);
                    set_state(info_.joints[i].name + "/temp", motorStates_[i].motor_temp);
                    // at last set the position, pretending the motor moved at the current speed for the last interval
                    set_state(info_.joints[i].name + "/position", get_state(info_.joints[i].name + "/position") + period.seconds() * get_state(info_.joints[i].name + "/velocity"));
                    break;
            }
        }
    }

    // if use_hardware is set to 0 -> pretend all commands are executed instantly
    else {
        for (const auto& [name, descr] : joint_state_interfaces_) {
            if (descr.get_interface_name() == hardware_interface::HW_IF_POSITION) {
                auto velo = get_command(descr.get_prefix_name() + "/" + hardware_interface::HW_IF_VELOCITY);
                set_state(name, get_state(name) + period.seconds() * velo);
            }
            else if (descr.get_interface_name() == hardware_interface::HW_IF_VELOCITY) {
                set_state(name, get_command(name));
            }
        }
    }

    return hardware_interface::return_type::OK;
}

hardware_interface::return_type DriveInterface::write(
  const rclcpp::Time& /*time*/, const rclcpp::Duration& /*period*/) {
    // if use_hardware is set to 1 -> use real hardware
    if (use_hardware_ == 1) {
        // caluclate motor eRPM to send: * (60/2Pi) to convert from rad/s to rpm, gear ratio for the gear box, e_conv_ is the factor between eRPM and motor rpm
        motorCommands_[0].speed = static_cast<int32_t>(get_command(info_.joints[0].name + "/velocity") * (60 / (2 * M_PI)) * gear_ratio_ * e_conv_ * (-1));
        motorCommands_[1].speed = static_cast<int32_t>(get_command(info_.joints[1].name + "/velocity") * (60 / (2 * M_PI)) * gear_ratio_ * e_conv_);
        // send command to motor via can interface
        drive_comms_.sendSpd(motorCommands_[0].motor_id, motorCommands_[0].speed);
        drive_comms_.sendSpd(motorCommands_[1].motor_id, motorCommands_[1].speed);
    }

    return hardware_interface::return_type::OK;
}
}  // namespace lotti2_drive_interface

#include "pluginlib/class_list_macros.hpp"

PLUGINLIB_EXPORT_CLASS(
  lotti2_drive_interface::DriveInterface, hardware_interface::SystemInterface)
