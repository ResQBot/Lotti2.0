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

#ifndef LOTTI2_CONTROL__LOTTI2_ARM_INTERFACE_HPP_
#define LOTTI2_CONTROL__LOTTI2_ARM_INTERFACE_HPP_
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
#include "lotti2_control/arm_comms.hpp"


namespace lotti2_arm_interface {
class ArmInterface : public hardware_interface::SystemInterface {

  public:
    RCLCPP_SHARED_PTR_DEFINITIONS(ArmInterface)

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
    // ros2 parameters
    std::string device_;
    double gear_ratio_;
    int motor_resolution_;
    uint8_t motor_acceleration_;
    int use_hardware_;
    uint8_t use_sync_;
    // data conversion
    double arm_pos_[6];
    double arm_spd_[6];
    uint32_t arm_pos_cmd_[6];
    uint16_t arm_spd_cmd_[6];

    // motor members
    ArmComms arm_comms_;
};
}  // namespace lotti2_arm_interface
#endif  // LOTTI2_CONTROL_LOTTI2_ARM_INTERFACE_HPP_
