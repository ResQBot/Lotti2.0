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

#include "lotti2_control/lotti2_flipper_interface.hpp"
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
// hardware
#include <unistd.h>
#include "serialPort/SerialPort.h"
#include "unitreeMotor/unitreeMotor.h"

namespace lotti2_flipper_interface {

hardware_interface::CallbackReturn FlipperInterface::on_init(
  const hardware_interface::HardwareComponentInterfaceParams& params) {
    if (hardware_interface::SystemInterface::on_init(params) !=
        hardware_interface::CallbackReturn::SUCCESS) {
        return hardware_interface::CallbackReturn::ERROR;
    }

    // get the parameters from the ros2_control file
    device_ = info_.hardware_parameters["device"];
    // max_torque_   = stof(info_.hardware_parameters["max_torque"]);
    // gear_ratio_   = stof(info_.hardware_parameters["gear_ratio"]) * unitree_ratio_;
    use_hardware_ = stoi(info_.hardware_parameters["use_hardware"]);
    if (!(use_hardware_ == 0 || use_hardware_ == 1)) {
        RCLCPP_ERROR(get_logger(), "FlipperInterface: Invalid value for \"use_hardware\" in ros2_control file");
    }

    return CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn FlipperInterface::on_configure(
  const rclcpp_lifecycle::State& /*previous_state*/) {
    // configure motor parameters for Unitree SDK message type
    for (std::size_t i = 0; i < info_.joints.size(); i++) {
        motor_data_[i].motorType = MotorType::GO_M8010_6;
        motor_data_[i].motor_id  = static_cast<unsigned char>(i);
        motor_data_[i].temp      = 0;
        motor_data_[i].merror    = 0;
        motor_data_[i].tau       = 0.0;
        motor_data_[i].dq        = 0.0;
        motor_data_[i].q         = 0.0;
        motor_error_type_[i]     = 0;
        motor_cmd_[i].motorType  = MotorType::GO_M8010_6;
        motor_cmd_[i].mode       = static_cast<unsigned short>(queryMotorMode(MotorType::GO_M8010_6, MotorMode::FOC));
        motor_cmd_[i].kp         = 0.2f;  // positional stiffness
        motor_cmd_[i].kd         = 0.0;   // velocity stiffness
        motor_cmd_[i].q          = 0.0;   // position []
        motor_cmd_[i].dq         = 0.0;   // speed    [Rads/s]
        motor_cmd_[i].tau        = 0.0;   // torque  [Nm]

        // setting motor IDs.
        switch (i) {
            case 0:  // front_right_flipper
                motor_cmd_[i].id        = 3;
                motor_data_[i].motor_id = 3;
                break;
            case 1:  // front_left_flipper
                motor_cmd_[i].id        = 2;
                motor_data_[i].motor_id = 2;
                break;
            case 2:  // rear_right_flipper
                motor_cmd_[i].id        = 1;
                motor_data_[i].motor_id = 1;
                break;
            case 3:  // rear_left_flipper
                motor_cmd_[i].id        = 4;
                motor_data_[i].motor_id = 4;
                break;
        }
    }

    // prepare serial connection only if use_hardware is set to "true"
    if (use_hardware_ == 1) {
        serial_ = std::make_unique<SerialPort>(device_);
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

hardware_interface::CallbackReturn FlipperInterface::on_cleanup(
  const rclcpp_lifecycle::State& /*previous_state*/) {
    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn FlipperInterface::on_activate(
  const rclcpp_lifecycle::State& /*previous_state*/) {
    // command and state should be equal when starting
    for (const auto& [name, descr] : joint_command_interfaces_) {
        set_command(name, get_state(name));
    }

    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn FlipperInterface::on_deactivate(
  const rclcpp_lifecycle::State& /*previous_state*/) {
    return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::return_type FlipperInterface::read(
  const rclcpp::Time& /*time*/, const rclcpp::Duration& /*period*/) {
    // if use_hardware is set to 1 -> read real data
    if (use_hardware_ == 1) {
        for (std::size_t i = 0; i < info_.joints.size(); i++) {
            if (motor_data_[i].correct) {
                if (motor_data_[i].temp >= 60) {
                    RCLCPP_ERROR(rclcpp::get_logger("FlipperInterface"), "MOTOR %ld OVER 60 DEGREES", i);
                }
                if (motor_data_[i].merror == 1) {
                    RCLCPP_ERROR(rclcpp::get_logger("FlipperInterface"), "MOTOR %ld OVERHEATING", i);
                }
                else if (motor_data_[i].merror == 2) {
                    RCLCPP_ERROR(rclcpp::get_logger("FlipperInterface"), "MOTOR %ld OVERCURRENT", i);
                }
                else if (motor_data_[i].merror == 3) {
                    RCLCPP_ERROR(rclcpp::get_logger("FlipperInterface"), "MOTOR %ld OVERVOLTAGE", i);
                }
                else if (motor_data_[i].merror == 4) {
                    RCLCPP_ERROR(rclcpp::get_logger("FlipperInterface"), "MOTOR %ld ENCODER ERROR", i);
                }
                motor_error_type_[i] = motor_data_[i].merror;

                set_state(info_.joints[i].name + "/position", static_cast<double>(motor_data_[i].q) / 6.33);
                set_state(info_.joints[i].name + "/velocity", static_cast<double>(motor_data_[i].dq / gear_ratio_));
                set_state(info_.joints[i].name + "/effort", static_cast<double>(motor_data_[i].tau));
                set_state(info_.joints[i].name + "/temp", static_cast<double>(motor_data_[i].temp));
            }
            else {
                switch (i) {
                    case 0:
                        RCLCPP_ERROR(rclcpp::get_logger("FlipperInterface"), "FRONT_RIGHT_MOTOR DATA ERROR");
                        break;
                    case 1:
                        RCLCPP_ERROR(rclcpp::get_logger("FlipperInterface"), "FRONT_LEFT_MOTOR DATA ERROR");
                        break;
                    case 2:
                        RCLCPP_ERROR(rclcpp::get_logger("FlipperInterface"), "REAR_RIGHT_MOTOR DATA ERROR");
                        break;
                    case 3:
                        RCLCPP_ERROR(rclcpp::get_logger("FlipperInterface"), "REAR_LEFT_MOTOR DATA ERROR");
                        break;
                }
            }
        }
    }
    // if use_hardware is set to 0 -> pretend all commands are executed instantly
    else {
        for (std::size_t i = 0; i < info_.joints.size(); i++) {
            set_state(info_.joints[i].name + "/position", get_command(info_.joints[i].name + "/position"));
            set_state(info_.joints[i].name + "/effort", 0.0);
            set_state(info_.joints[i].name + "/velocity", 0.0);
            set_state(info_.joints[i].name + "/temp", 22.0);
        }
    }

    return hardware_interface::return_type::OK;
}

hardware_interface::return_type FlipperInterface::write(
  const rclcpp::Time& /*time*/, const rclcpp::Duration& /*period*/) {
    // only perform write if use_hardware is set to 1
    if (use_hardware_ == 1) {
        for (std::size_t i = 0; i < info_.joints.size(); i++) {
            // check if motors are close to overheating
            if (motor_error_type_[i] == 0) {
                // set command values
                motor_cmd_[i].q = static_cast<float>(get_command(info_.joints[i].name + "/position") * 6.33);
            }
            // if motor has returned error -> stop
            else {
                motor_cmd_[i].q = static_cast<float>(get_state(info_.joints[i].name + "/position") * 6.33);
            }
        }

        // Send commands and receive data via serial port
        // careful which motors are connected to which USB adaptor!
        if (serial_) {
            for (std::size_t i = 0; i < info_.joints.size(); i++) {
                serial_->sendRecv(&motor_cmd_[i], &motor_data_[i]);
            }
        }
    }

    return hardware_interface::return_type::OK;
}
}  // namespace lotti2_flipper_interface

#include "pluginlib/class_list_macros.hpp"
PLUGINLIB_EXPORT_CLASS(
  lotti2_flipper_interface::FlipperInterface, hardware_interface::SystemInterface)