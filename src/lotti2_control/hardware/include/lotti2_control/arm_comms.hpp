#ifndef LOTTI2_CONTROL__ARM_COMMS_HPP
#define LOTTI2_CONTROL__ARM_COMMS_HPP

#pragma once

#include <time.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include "libserial/SerialPort.h"
#include "rclcpp/rclcpp.hpp"
#include "sstream"

#include "lotti2_arm_interface.hpp"

class ArmComms {

  public:
    ArmComms() = default;

    void connect(const std::string& serial_device) {
        timeout_ms_ = 1000;
        serial_conn_.Open(serial_device);
        serial_conn_.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
        serial_conn_.FlushIOBuffers();
    }


    void disconnect() {
        serial_conn_.Close();
    }


    bool connected() const {
        return serial_conn_.IsOpen();
    }

    void clear() {
        serial_conn_.FlushIOBuffers();
    }


    void setReq(uint8_t cmd, int mode) {
        uint8_t motor_id;
        for (size_t i = 1; i < 7; i++) {
            motor_id = static_cast<uint8_t>(i);
            sendRequest(cmd, motor_id, static_cast<uint8_t>(mode));  // issue the request to the motors
            usleep(10000);
        }
    }


    int64_t readPos(uint8_t motor_id) {
        bool success = false;
        int64_t motor_pos;
        while (!success) {
            serial_conn_.FlushIOBuffers();
            sendRequest(0x31, motor_id + 1, 0);  // issue a query position information command for id-motor
            uint8_t b;
            while (b != 0xFB) {
                serial_conn_.ReadByte(b);
            }
            posRxBuffer[0] = b;
            for (int i = 1; i < 10; i++) {
                serial_conn_.ReadByte(b);
                posRxBuffer[i] = b;
            }
            if (posRxBuffer[1] == (motor_id + 1) && posRxBuffer[2] == 0x31 && posRxBuffer[9] == getCheckSum(posRxBuffer, 9)) {
                motor_pos = static_cast<int64_t>(
                  static_cast<uint64_t>(posRxBuffer[3]) << 56 |
                  static_cast<uint64_t>(posRxBuffer[4]) << 48 |
                  static_cast<uint64_t>(posRxBuffer[5]) << 40 |
                  static_cast<uint64_t>(posRxBuffer[6]) << 32 |
                  static_cast<uint64_t>(posRxBuffer[7]) << 24 |
                  static_cast<uint64_t>(posRxBuffer[8]) << 16);
                motor_pos = motor_pos / 0xffff;

                success = true;
            }
        }
        return (motor_pos);
    }


    void setArmValues(uint8_t motor_id, uint8_t direction, uint16_t speed, uint8_t accel) {
        cmdTxMsg.clear();  // clear old TxMsg buffer
        cmdTxMsg.resize(7);
        cmdTxBuffer[1] = motor_id;                                               // motor address
        cmdTxBuffer[3] = static_cast<uint8_t>((speed >> 8) & 0xFF) + direction;  // higher 8 bit speed
        cmdTxBuffer[4] = static_cast<uint8_t>((speed >> 0) & 0xFF);              // lower 8 bit speed
        cmdTxBuffer[5] = accel;                                                  // acceleration
        cmdTxBuffer[6] = getCheckSum(cmdTxBuffer, 6);                            // Calculate checksum
        for (int l = 0; l < 7; l++) {
            cmdTxMsg[l] = cmdTxBuffer[l];  // write TxBuffer to TxMsg to be sent (conversion necessary due to LibSerial Write function)
        }
        serial_conn_.Write(cmdTxMsg);  // the serial port sends the speed command
    }


  private:

    // for serial connection
    LibSerial::SerialPort serial_conn_;
    int timeout_ms_;  // timeout before connection error is called
    // for motor communication
    std::vector<uint8_t> cmdTxMsg;
    std::vector<uint8_t> reqMsg;
    uint8_t cmdTxBuffer[7] = {0xFA, 0, 0xF6, 0, 0, 0, 0};
    uint8_t reqTxBuffer[5] = {0xFA, 0, 0, 0, 0};
    uint8_t posRxBuffer[10];
    uint8_t reqRxBuffer[5];


    void sendRequest(uint8_t cmd, uint8_t id, uint8_t mode) {
        reqMsg.clear();
        reqTxBuffer[1] = id;
        reqTxBuffer[2] = cmd;
        switch (cmd) {
            case 0xF3:                  // enable motor
                reqTxBuffer[3] = mode;  // 1 = enable, 0 = disable
                reqTxBuffer[4] = getCheckSum(reqTxBuffer, 4);
                break;
            case 0x82:                  // set motor command mode
                reqTxBuffer[3] = mode;  // choose absolute axis position control mode
                reqTxBuffer[4] = getCheckSum(reqTxBuffer, 4);
                break;
            case 0x92:  // set zero position
                reqTxBuffer[3] = getCheckSum(reqTxBuffer, 3);
                break;
            case 0x4A:                  // set sync execute mode
                reqTxBuffer[3] = mode;  // chose sync mode on/off
                reqTxBuffer[4] = getCheckSum(reqTxBuffer, 4);
                break;
            case 0x31:  // request real time location
                reqTxBuffer[3] = getCheckSum(reqTxBuffer, 3);
                break;
            case 0x32:  // request real time speed
                reqTxBuffer[3] = getCheckSum(reqTxBuffer, 3);
                break;
        }
        if (cmd == 0x92 || cmd == 0x31 || cmd == 0x32) {
            reqMsg.resize(4);
            for (std::size_t i = 0; i < 4; i++) {
                reqMsg[i] = reqTxBuffer[i];  // write TxBuffer to TxMsg to be sent (conversion necessary due to LibSerial Write function)
            }
        }
        else {
            reqMsg.resize(5);
            for (std::size_t i = 0; i < 5; i++) {
                reqMsg[i] = reqTxBuffer[i];  // write TxBuffer to TxMsg to be sent (conversion necessary due to LibSerial Write function)
            }
        }
        // serial_conn_.FlushIOBuffers();  // just in case
        serial_conn_.Write(reqMsg);  // the serial port issues a motor enable command
    }

    void sendArm() {
    }


    uint8_t getCheckSum(uint8_t* buffer, uint8_t size) {
        uint8_t i;
        uint16_t sum = 0;
        for (i = 0; i < size; i++) {
            sum += buffer[i];  // Calculate accumulated value
        }
        return (static_cast<uint8_t>(sum & 0xFF));  // return checksum
    }
};
#endif  // LOTTI2_CONTROL__ARM_COMMS_HPP