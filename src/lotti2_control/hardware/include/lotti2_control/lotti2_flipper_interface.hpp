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

#ifndef LOTTI2_CONTROL__LOTTI2_FLIPPER_INTERFACE_HPP_
#define LOTTI2_CONTROL__LOTTI2_FLIPPER_INTERFACE_HPP_
// general purpose
#include <chrono>
#include <cstdint>
#include <memory>
#include "string"
#include "vector"
// ros2_control
#include "hardware_interface/handle.hpp"
#include "hardware_interface/hardware_info.hpp"
#include "hardware_interface/system_interface.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"
#include "hardware_interface/types/hardware_interface_type_values.hpp"
#include "rclcpp/clock.hpp"
#include "rclcpp/duration.hpp"
#include "rclcpp/logger.hpp"
#include "rclcpp/macros.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp/time.hpp"
#include "rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp"
#include "rclcpp_lifecycle/state.hpp"
// hardware
#include <unistd.h>
#include "serialPort/SerialPort.h"
#include "unitreeMotor/unitreeMotor.h"

namespace lotti2_flipper_interface {
class FlipperInterface : public hardware_interface::SystemInterface {

  public:
    RCLCPP_SHARED_PTR_DEFINITIONS(FlipperInterface)

    hardware_interface::CallbackReturn on_init(
      const hardware_interface::HardwareComponentInterfaceParams& params) override;

    hardware_interface::CallbackReturn on_configure(
      const rclcpp_lifecycle::State& previous_state) override;
    hardware_interface::CallbackReturn on_cleanup(
      const rclcpp_lifecycle::State& previous_state) override;

    hardware_interface::CallbackReturn on_activate(
      const rclcpp_lifecycle::State& previous_state) override;
    hardware_interface::CallbackReturn on_deactivate(
      const rclcpp_lifecycle::State& previous_state) override;

    hardware_interface::return_type read(
      const rclcpp::Time& time, const rclcpp::Duration& period) override;
    hardware_interface::return_type write(
      const rclcpp::Time& /*time*/, const rclcpp::Duration& /*period*/) override;

  private:
    std::string device_;           // usb connector
    double gear_ratio_;            // belt drive ratio + unitree ratio
    double unitree_ratio_ = 6.33;  // unitree integrated gear ratio
    int use_hardware_;             // use hardware or emulate
    int motor_error_type_[4];      // errors sent by the motors
    int dir_[4];                   // directional correction for motors according to positioning inside the robot
    bool first;

    // --- Add Unitree SDK members here ---
    std::unique_ptr<SerialPort> serial_;
    MotorCmd motor_cmd_[4];    // Array of motor commands
    MotorData motor_data_[4];  // Array of motor feedback data
    // to avoid movement before operator is ready
    MotorCmd first_cmd_[4];
};
}  // namespace lotti2_flipper_interface
#endif  // LOTTI2_CONTROL_LOTTI2_FLIPPER_INTERFACE_HPP_
