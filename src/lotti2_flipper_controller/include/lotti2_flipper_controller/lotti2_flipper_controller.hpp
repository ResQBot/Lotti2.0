#ifndef LOTTI2_FLIPPER_CONTROLLER_HPP_
#define LOTTI2_FLIPPER_CONTROLLER_HPP_

#include <atomic>
#include <chrono>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "controller_interface/controller_interface.hpp"
#include "hardware_interface/types/hardware_interface_type_values.hpp"
#include "rclcpp/duration.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp/subscription.hpp"
#include "rclcpp/time.hpp"
#include "rclcpp/timer.hpp"
#include "rclcpp_lifecycle/lifecycle_publisher.hpp"
#include "rclcpp_lifecycle/node_interfaces/lifecycle_node_interface.hpp"
#include "realtime_tools/realtime_thread_safe_box.hpp"
#include "std_msgs/msg/float32.hpp"
// #include "lotti2_msgs/msg/flipper.hpp"

namespace lotti2_flipper_controller {

class FlipperController : public controller_interface::ControllerInterface {

  public:
    FlipperController();

    controller_interface::InterfaceConfiguration command_interface_configuration() const override;

    controller_interface::InterfaceConfiguration state_interface_configuration() const override;

    controller_interface::return_type update(
      const rclcpp::Time &time, const rclcpp::Duration &period) override;

    controller_interface::CallbackReturn on_init() override;

    controller_interface::CallbackReturn on_configure(
      const rclcpp_lifecycle::State &previous_state) override;

    controller_interface::CallbackReturn on_activate(
      const rclcpp_lifecycle::State &previous_state) override;

    controller_interface::CallbackReturn on_deactivate(
      const rclcpp_lifecycle::State &previous_state) override;

  protected:
    // for setup
    std::vector<std::string> joint_names_;
    std::vector<std::string> command_interface_types_;
    std::vector<std::string> state_interface_types_;

    // for PID controller
    float Kp_ = 0.8f;  // proportional gain
    float Ki_ = 0.0f;  // integral gain
    float Kd_ = 0.5f;  // derivative gain
    float tau = 2.0f;  // low-pass filter for derivative term
    float prevError[4];
    float prevVel[4];
    float proportional[4];
    float integrator[4];
    float differentiator[4];
    float Time[4];

    // for motor commands
    float maxTorque = 2.5f;                          // max motor torque output before gearbox [Nm]
    float maxSpeed  = (3.1416f / 2.0f) * (45 / 18);  // max motor speed [Rad/s] = max flipper speed * belt ratio
    float flipperCMD[4];
    float torqueCMD[4];

    // for subscriber
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr fr_flipper_command_subscriber_;
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr fl_flipper_command_subscriber_;
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr rr_flipper_command_subscriber_;
    rclcpp::Subscription<std_msgs::msg::Float32>::SharedPtr rl_flipper_command_subscriber_;


    // list all interfaces
    // std::vector<std::reference_wrapper<hardware_interface::LoanedCommandInterface>>
    // joint_velocity_command_interface_;
    std::vector<std::reference_wrapper<hardware_interface::LoanedCommandInterface>>
      joint_effort_command_interface_;
    std::vector<std::reference_wrapper<hardware_interface::LoanedStateInterface>>
      joint_position_state_interface_;
    std::vector<std::reference_wrapper<hardware_interface::LoanedStateInterface>>
      joint_effort_state_interface_;
    std::vector<std::reference_wrapper<hardware_interface::LoanedStateInterface>>
      joint_velocity_state_interface_;
    std::vector<std::reference_wrapper<hardware_interface::LoanedStateInterface>>
      joint_temp_state_interface_;

    // mapping command interfaces
    std::unordered_map<
      std::string, std::vector<std::reference_wrapper<hardware_interface::LoanedCommandInterface>> *>
      command_interface_map_ = {
        {"effort", &joint_effort_command_interface_}};
    // mapping state interfaces
    std::unordered_map<
      std::string, std::vector<std::reference_wrapper<hardware_interface::LoanedStateInterface>> *>
      state_interface_map_ = {
        {"position", &joint_position_state_interface_},
        {"velocity", &joint_velocity_state_interface_},
        {"effort", &joint_effort_state_interface_},
        {"temp", &joint_temp_state_interface_},
      };
};
}  // namespace lotti2_flipper_controller

#endif  // LOTTI2_FLIPPER_CONTROLLER_HPP_