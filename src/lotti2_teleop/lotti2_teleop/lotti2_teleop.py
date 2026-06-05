import rclpy
from rclpy.node import Node
import time
import sys
from moveit_msgs.srv import ServoCommandType

from sensor_msgs.msg import Joy
from geometry_msgs.msg import TwistStamped
from control_msgs.msg import JointJog
from std_msgs.msg import Float32
from threading import Lock
from std_srvs.srv import Trigger


# Global Defines --------------------------------------------------------------
# mapping Gamepad buttons and sticks to joy topic axes and buttons

# axes
left_stick_x = 0
left_stick_y = 1
right_stick_x = 3
right_stick_y = 4
left_trigger = 2
right_trigger = 5
d_pad_x = 6
d_pad_y = 7

#buttons
a_button = 0
b_button = 1
x_button = 2
y_button = 3
lb_button = 4
rb_button = 5
opt_left_button = 6
opt_right_button = 7
xBox_button = 8
left_stick_button = 9
right_stick_button = 10
space_button = 11

PI=3.1415926535897

# Class -----------------------------------------------------------------------
class TeleOp(Node):
    
    def __init__(self):
        #Entrypoint of the class
        super().__init__('tele_op')

        #define variables

        #axes
        self.__left_stick_x = float(0)
        self.__left_stick_x_lock = Lock()
        self.__left_stick_y = float(0)
        self.__left_stick_y_lock = Lock()
        self.__left_stick_press = float(0)
        self.__left_stick_press_lock = Lock()
        
        self.__left_trigger = float(0)
        self.__left_trigger_lock = Lock()

        self.__right_stick_x = float(0)
        self.__right_stick_x_lock = Lock()
        self.__right_stick_y = float(0)
        self.__right_stick_y_lock = Lock()
        self.__right_stick_press = float(0)
        self.__right_stick_press_lock = Lock()
        
        self.__right_trigger = float(0)
        self.__right_trigger_lock = Lock()

        self.__d_pad_x = float(0)
        self.__d_pad_x_lock = Lock()
        self.__d_pad_y = float(0)
        self.__d_pad_y_lock = Lock()

        #buttons
        self.__button_a = int(0)
        self.__button_a_lock = Lock()
        self.__button_b = int(0)
        self.__button_b_lock = Lock()
        self.__button_x = int(0)
        self.__button_x_lock = Lock()
        self.__button_y = int(0)
        self.__button_y_lock = Lock()

        self.__button_rb = int(0)
        self.__button_rb_lock = Lock()
        self.__button_lb = int(0)
        self.__button_lb_lock = Lock()
       
        self.__button_opt_right = int(0)
        self.__button_opt_right_lock = Lock()
        self.__button_opt_left = int(0)
        self.__button_opt_left_lock = Lock()
        self.__button_space = int(0)
        self.__button_space_lock = Lock()


        # safety stuff
        self.__first_joy_msg_received = bool(False)
        self.__joy_enabled = bool(False)
        self.__joy_enabled_old = bool(False)
        self.__button_opt_right_pressed = bool(False)
        self.__button_opt_right_last_pressed = 0.0

        # switch to arm
        self.__arm_enabled = bool(False)
        self.__arm_enabled_old = bool(True)
        self.__button_opt_left_pressed = bool(False)
        self.__button_opt_left_last_pressed = 0.0

        # flipper controlls
        self.__flipper_speed = float (0.0)
        self.__fr_flipper_cmd = Float32()
        self.__fl_flipper_cmd = Float32()
        self.__rr_flipper_cmd = Float32()
        self.__rl_flipper_cmd = Float32()

        #chain controlls
        self.__chain_msg = TwistStamped()
        self.__chain_msg.header.frame_id = ""
        self.__chain_msg.twist.linear.y = float(0)
        self.__chain_msg.twist.linear.z = float(0)
        self.__chain_msg.twist.angular.y = float(0)
        self.__chain_msg.twist.angular.x = float(0)
        self.__chains_angle = float(0)
        
        self.__direction = bool(False)
        self.__direction_old = bool(True)
        self.__button_space_pressed = bool(False)
        self.__button_space_last_pressed = 0.0

        # arm controlls
        self.__arm_msg = TwistStamped()
        self.__arm_msg.header.frame_id = "arm6_link"
        self.__servo_active = bool(False)

        # gripper controlls
        self.__gripper_msg = JointJog()
        self.__gripper_msg.joint_names = ["arm1_joint","arm2_joint", "arm3_joint", "arm4_joint", "arm5_joint", "arm6_joint", "arm7_joint"]
        self.__gripper_msg.duration = 0.05

        # Init class ->create subscriber, create timer
        self.__readParams()
        self.__createSubscribers()
        self.__createPublishers()
        self.__createTimer()

        self.get_logger().info("Tele_OP initiated")



    def __readParams(self):
        # declare parameters
        self.declare_parameter('Publish_rate', 25)              #[Hz]

        # read parameters
        self.__Publish_rate = rclpy.parameter.Parameter(
            'Publish_rate',
            rclpy.Parameter.Type.DOUBLE,
            25.0
        ) 



     # the servo_node service needs to be called to start
    """def __callServo(self):
        self.__cli = self.create_client(ServoCommandType, '/servo_node/switch_command_type')
        while not self.__cli.wait_for_service(timeout_sec=1.0):
            self.get_logger().info('service not available, waiting again...')
        request = ServoCommandType.Request()
        request.command_type = ServoCommandType.Request.TWIST
        self.__future = self.__cli.call(request)
        rclpy.spin_until_future_complete(self, self.__future)
        return self.__future.result() 
    """


    def __checkCMDOutputEnable(self):

        # ----- Enable button check -----
        # If there was no joy msg received before and the twist output somehow is ENABLED, DISABLE the twist output.
        if (self.__first_joy_msg_received == False and self.__joy_enabled != False):
            self.__joy_enabled = False
        
        elif (self.__first_joy_msg_received == True):
            # If the output is disabled, the output-enable button was not pressed and is now pressed, set the button
            # pressed var to true and save the time point when it was pressed to get the time difference.
            if (self.__joy_enabled == False and self.__button_opt_right_pressed == False and self.__button_opt_right == 1):
                    self.__button_opt_right_pressed = True
                    self.__button_opt_right_last_pressed = time.time()
            
            # check if button is pressed long enough.
            elif (self.__joy_enabled == False and self.__button_opt_right_pressed == True and self.__button_opt_right == 1):
                # messure how long the start button is pressed
                time_difference = time.time() - self.__button_opt_right_last_pressed

                # If the start button is pressed long enough, enable the controlles
                if (time_difference > 1):
                    self.__joy_enabled = True
                        
            # After the output was enabled, wait for the button to be released befor the button_pressed var is set to false.
            elif (self.__joy_enabled == True and self.__button_opt_right_pressed == True and self.__button_opt_right == 0):
                self.__button_opt_right_pressed = False
            
            # if joy is disabled and the start_button was pressed but is released before the output could be 
            # enabled, the button_pressed var is set to false to receive a new button press.
            elif (self.__joy_enabled == False and self.__button_opt_right_pressed == True and self.__button_opt_right == 0):
                self.__button_opt_right_pressed = False

            # if joy is enabled and the enable button is pressed, disable the output.
            elif (self.__joy_enabled == True and self.__button_opt_right_pressed == False and self.__button_opt_right == 1):
                self.__joy_enabled = False


        # Print a message to show the current output status.
        if self.__joy_enabled != self.__joy_enabled_old:
            if (self.__joy_enabled == True and self.__joy_enabled_old == False):
                self.get_logger().info("Tele Op: ENABLED")
            else:
                if (self.__joy_enabled == False and self.__joy_enabled_old == True):
                    self.get_logger().info("Tele Op: DISABLED")

            # Set the old state to the new state
            self.__joy_enabled_old = self.__joy_enabled



    def __checkArmMode(self):

        # ----- Enable button check -----
        # If there was no joy msg received before and the arm mode is ENABLED, DISABLE it.
        if (self.__first_joy_msg_received == False and self.__arm_enabled != False):
            self.__arm_enabled = False
        
        elif (self.__first_joy_msg_received == True):
            # if current mode equals old mode and button is newly pressed, start timer
            if (self.__arm_enabled != self.__arm_enabled_old and self.__button_opt_left == 1 and self.__button_opt_left_pressed == False):
                self.__button_opt_left_pressed = True
                self.__button_opt_left_last_pressed = time.time()

            # if timer has been started before
            elif (self.__arm_enabled != self.__arm_enabled_old and self.__button_opt_left == 1 and self.__button_opt_left_pressed == True):
                # messure how long the start button has been pressed
                time_difference = time.time() - self.__button_opt_left_last_pressed
                # If the start button has been pressed long enough, switch mode
                if (time_difference > 1):
                    self.__arm_enabled = not self.__arm_enabled
                    if (self.__arm_enabled == True):
                        
                        self.get_logger().info("Control Mode: ARM")
                    else:
                        self.get_logger().info("Control Mode: BODY")
            
            # if mode has been switched, wait until start button is released, then change old mode status to enable new switch cycle
            elif (self.__arm_enabled == self.__arm_enabled_old and self.__button_opt_left_pressed == True and self.__button_opt_left == 0):
                self.__button_opt_left_pressed = False
                self.__arm_enabled_old = not self.__arm_enabled_old 
            
            # if button is not pressed long enough, reset
            elif (self.__arm_enabled != self.__arm_enabled_old and self.__button_opt_left_pressed == True and self.__button_opt_left == 0):
                self.__button_opt_left_pressed = False



    def __checkDir(self):
        # if current mode equals old mode and button is newly pressed, start timer
        if (self.__direction != self.__direction_old and self.__button_space == 1 and self.__button_space_pressed == False):
                self.__button_space_pressed = True
                self.__button_space_last_pressed = time.time()

        # if timer has been started before
        elif (self.__direction != self.__direction_old and self.__button_space == 1 and self.__button_space_pressed == True):
            # messure how long the start button has been pressed
            time_difference = time.time() - self.__button_space_last_pressed
            # If the start button has been pressed long enough, switch mode
            if (time_difference > 1):
                self.__direction = not self.__direction
                if (self.__direction == False):
                    self.get_logger().info("Drive forward")
                elif (self.__direction == True):
                    self.get_logger().info("Drive backward")
            
        # if mode has been switched, wait until start button is released, then change old mode status to enable new switch cycle
        elif (self.__direction == self.__direction_old and self.__button_space_pressed == True and self.__button_space == 0):
            self.__button_space_pressed = False
            self.__direction_old = not self.__direction_old
        
        # if button is not pressed long enough, reset
        elif (self.__direction != self.__direction_old and self.__button_space_pressed == True and self.__button_space == 0):
            self.__button_space_pressed = False



    def __calcAndSendFlippers(self):
        
        # check if arm mode is active        
        if (self.__arm_enabled == False):
            # right trigger lowers flippers, left trigger lifts flippers
            self.__flipper_speed = ((self.__right_trigger + 1) / 2) - ((self.__left_trigger + 1) / 2)
            self.__fr_flipper_cmd.data = self.__button_y * self.__flipper_speed
            self.__fl_flipper_cmd.data = self.__button_x * self.__flipper_speed
            self.__rr_flipper_cmd.data = self.__button_b * self.__flipper_speed         
            self.__rl_flipper_cmd.data = self.__button_a * self.__flipper_speed
        else:
            self.__fr_flipper_cmd.data = 0.0
            self.__fl_flipper_cmd.data = 0.0
            self.__rr_flipper_cmd.data = 0.0         
            self.__rl_flipper_cmd.data = 0.0

        # send the commands
        if (self.__joy_enabled == True):
            self.__fr_flipper_publisher.publish(self.__fr_flipper_cmd)
            self.__fl_flipper_publisher.publish(self.__fl_flipper_cmd)
            self.__rr_flipper_publisher.publish(self.__rr_flipper_cmd)
            self.__rl_flipper_publisher.publish(self.__rl_flipper_cmd)



    def __calcAndSendChains(self):

        # check for arm mode and construct messages to be sent
        if (self.__arm_enabled == False):
            # corrections for driving backwards
            if (self.__left_stick_y < 0):
                self.__chains_angle = -self.__left_stick_x
            else:
                self.__chains_angle = self.__left_stick_x
            # check "backward driving" button for convenience
            if (self.__direction == True):
                self.__chain_msg.twist.linear.x = -self.__left_stick_y
                self.__chain_msg.twist.angular.z = -self.__chains_angle
            else:
                self.__chain_msg.twist.linear.x = self.__left_stick_y
                self.__chain_msg.twist.angular.z = self.__chains_angle
        else:       
            self.__chain_msg.twist.linear.x = 0.0
            self.__chain_msg.twist.angular.z = 0.0

        # the message of format TwistStamped needs a time stamp.
        self.__chain_msg.header.stamp = self.get_clock().now().to_msg()
        # send movement commands
        if (self.__joy_enabled == True):
            self.__chain_publisher.publish(self.__chain_msg)
        


    def __calc_and_send_arm(self):

        # check for arm mode 
        if (self.__arm_enabled == True):

            # calc arm tilt        
            tilt = abs((self.__right_trigger -1)/2) + (self.__left_trigger -1)/2

            # construct arm message
            # linear x-y-z is for linear motion relative to the reference link
            # angular x-y-z is for rotation around the reference links axes
            self.__arm_msg.twist.linear.x = self.__left_stick_y
            self.__arm_msg.twist.linear.y = - self.__d_pad_x
            self.__arm_msg.twist.linear.z = self.__d_pad_y
            self.__arm_msg.twist.angular.z = self.__left_stick_x        
            self.__arm_msg.twist.angular.y = - self.__right_stick_y
            self.__arm_msg.twist.angular.x = tilt

            # construct joint message
            joint1 = float(self.__button_rb -self.__button_lb)
            joint2 = float(self.__button_x - self.__button_a)
            joint3 = float(-self.__button_y + self.__button_b)
            gripper = float(self.__right_stick_press - self.__left_stick_press)
    
            self.__gripper_msg.velocities = [joint1, joint2, joint3, - self.__right_stick_x, self.__right_stick_y, tilt, gripper]

        else:
            self.__arm_msg.twist.linear.x = 0.0
            self.__arm_msg.twist.linear.y = 0.0
            self.__arm_msg.twist.linear.z = 0.0
            self.__arm_msg.twist.angular.z = 0.0        
            self.__arm_msg.twist.angular.y = 0.0
            self.__arm_msg.twist.angular.x = 0.0

            self.__gripper_msg.velocities = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]

        # add time stamps and seq number
        self.__gripper_msg.header.stamp = self.get_clock().now().to_msg()
        self.__arm_msg.header.stamp = self.get_clock().now().to_msg()

        # send arm commands
        if (self.__joy_enabled == True):
            self.__arm_publisher.publish(self.__arm_msg)
            self.__gripper_publisher.publish(self.__gripper_msg)
        


    def __timerCallback(self):
        self.__checkCMDOutputEnable()
        self.__checkArmMode()
        self.__checkDir()
        self.__calcAndSendFlippers()
        self.__calcAndSendChains()
        self.__calc_and_send_arm()



    def __joyCallback(self, msg):
        # save controller input to local variables

        #axes
        self.__left_stick_x_lock.acquire()
        self.__left_stick_x = msg.axes[left_stick_x]
        self.__left_stick_x_lock.release()

        self.__left_stick_y_lock.acquire()
        self.__left_stick_y = msg.axes[left_stick_y]
        self.__left_stick_y_lock.release()

        self.__left_stick_press_lock.acquire()
        self.__left_stick_press = msg.buttons[left_stick_button]
        self.__left_stick_press_lock.release()

        self.__left_trigger_lock.acquire()
        self.__left_trigger = msg.axes[left_trigger]
        self.__left_trigger_lock.release()

        self.__right_stick_x_lock.acquire()
        self.__right_stick_x = msg.axes[right_stick_x]
        self.__right_stick_x_lock.release()

        self.__right_stick_y_lock.acquire()
        self.__right_stick_y = msg.axes[right_stick_y]
        self.__right_stick_y_lock.release()

        self.__right_stick_press_lock.acquire()
        self.__right_stick_press = msg.buttons[right_stick_button]
        self.__right_stick_press_lock.release()

        self.__right_trigger_lock.acquire()
        self.__right_trigger = msg.axes[right_trigger]
        self.__right_trigger_lock.release()

        self.__d_pad_y_lock.acquire()
        self.__d_pad_y = msg.axes[d_pad_y]
        self.__d_pad_y_lock.release()

        self.__d_pad_x_lock.acquire()
        self.__d_pad_x = msg.axes[d_pad_x]
        self.__d_pad_x_lock.release()

        #buttons
        self.__button_x_lock.acquire()
        self.__button_x = msg.buttons[x_button]
        self.__button_x_lock.release()

        self.__button_y_lock.acquire()
        self.__button_y = msg.buttons[y_button]
        self.__button_y_lock.release()

        self.__button_a_lock.acquire()
        self.__button_a = msg.buttons[a_button]
        self.__button_a_lock.release()

        self.__button_b_lock.acquire()
        self.__button_b = msg.buttons[b_button]
        self.__button_b_lock.release()

        self.__button_lb_lock.acquire()
        self.__button_lb = msg.buttons[lb_button]
        self.__button_lb_lock.release()

        self.__button_rb_lock.acquire()
        self.__button_rb = msg.buttons[rb_button]
        self.__button_rb_lock.release()

        self.__button_opt_right_lock.acquire()
        self.__button_opt_right = msg.buttons[opt_right_button]
        self.__button_opt_right_lock.release()

        self.__button_opt_left_lock.acquire()
        self.__button_opt_left = msg.buttons[opt_left_button]
        self.__button_opt_left_lock.release()

        self.__button_space_lock.acquire()
        self.__button_space = msg.buttons[space_button]
        self.__button_space_lock.release()


        # Check if there was a joy msgs since the node was started
        if (self.__first_joy_msg_received == False):
            self.__first_joy_msg_received = True
        


    def __createSubscribers(self):

        self._joy_sub = self.create_subscription(
            Joy,
            'joy',
            self.__joyCallback,
            1,
        )



    def __createPublishers(self):

        self.__chain_publisher = self.create_publisher(
            TwistStamped,
            'cmd/chains',
            1
        )

        self.__fr_flipper_publisher = self.create_publisher(
            Float32,
            'cmd/fr_flipper',
            1
        )

        self.__fl_flipper_publisher = self.create_publisher(
            Float32,
            'cmd/fl_flipper',
            1
        )

        self.__rr_flipper_publisher = self.create_publisher(
            Float32,
            'cmd/rr_flipper',
            1
        )

        self.__rl_flipper_publisher = self.create_publisher(
            Float32,
            'cmd/rl_flipper',
            1
        )

        self.__arm_publisher = self.create_publisher(
            TwistStamped,
            "cmd/arm/joy_twiststamped",
            1
        )

        self.__gripper_publisher = self.create_publisher(
            JointJog,
            "cmd/arm/joy_joint",
            1
        )



    def __createTimer(self):
        # Create timer
        self._timer = self.create_timer(
            1.0 / self.__Publish_rate.value,
            self.__timerCallback
        )



def main(args=None):
    rclpy.init(args=args)

    tele_op = TeleOp()

    rclpy.spin(tele_op)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    tele_op.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()