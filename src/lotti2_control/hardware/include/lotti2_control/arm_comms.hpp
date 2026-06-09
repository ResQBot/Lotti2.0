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
    }


    void disconnect() {
        serial_conn_.Close();
    }


    bool connected() const {
        return serial_conn_.IsOpen();
    }


    void setReq(uint8_t cmd, int mode) {
        /*std::string command_;
        switch (cmd) {
            case 0x01:
                command_ = "enable auto-update";
                break;
            case 0xF3:
                command_ = "enable motor";
                break;
            case 0x82:
                command_ = "set motor command mode";
                break;
            case 0x92:
                command_ = "set zero position";
                break;
            case 0x4A:
                command_ = "set synchronous execution mode";
                break;
        }*/
        // bool ackStatus;
        uint8_t motor_id;
        for (size_t i = 1; i < 7; i++) {
            //  ackStatus = false;
            motor_id = static_cast<uint8_t>(i);
            // while (!ackStatus) {
            sendRequest(cmd, motor_id, static_cast<uint8_t>(mode));  // issue the request to the motors
            usleep(3000);
            //  ackStatus = waitingForACK(req_len, reqRxBuffer);         // Wait for the motors to answer
            //  if (ackStatus == true) {                                 // Received answer
            //      if (reqRxBuffer[2] == cmd && reqRxBuffer[3] == 1) {  // motor confirms mode set
            //          break;
            //      }
            //      else {  // motor error setting mode (connection is good, since data was received. Check program)
            //          std::cout << "ArmInterface failed to request " << command_ << " from motor " << motor_id << ". Retrying ..." << std::endl;
            //      }
            //  }
            //  // if failed to receive mode confirmation
            //  // 1. Check the connection of the serial cable; 2. Check whether the motor is powered on; 3. Check the slave address and baud rate
            //  else {
            //      std::cout << "ArmInterface failed to receive feedback from motor" << motor_id << ". Retrying ..." << std::endl;
            //  }
            // }
        }
        serial_conn_.FlushIOBuffers();
    }


    int64_t readPos(uint8_t motor_id) {
        bool success = false;
        int64_t motor_pos;
        while (!success) {
            std::cout << "sending position request" << std::endl;
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

            std::cout << "received " << std::hex;
            for (int l = 0; l < 10; l++) {
                std::cout << static_cast<int>(posRxBuffer[l]) << " ";
            }
            std::cout << std::endl;

            if (posRxBuffer[1] == (motor_id + 1) && posRxBuffer[2] == 0x31 && posRxBuffer[9] == getCheckSum(posRxBuffer, 9)) {
                motor_pos = static_cast<int64_t>(
                  static_cast<uint64_t>(posRxBuffer[3]) << 40 |
                  static_cast<uint64_t>(posRxBuffer[4]) << 32 |
                  static_cast<uint64_t>(posRxBuffer[5]) << 24 |
                  static_cast<uint64_t>(posRxBuffer[6]) << 16 |
                  static_cast<uint64_t>(posRxBuffer[7]) << 8 |
                  static_cast<uint64_t>(posRxBuffer[8]) << 0);

                success = true;
            }
        }
        std::cout << motor_pos << std::endl;
        return (motor_pos);
    }

    /*
            int16_t readSpd(uint8_t motor_id) {
                bool ackStatus = false;
                int16_t motor_spd;
                while (!ackStatus) {
                    sendRequest(0x32, motor_id, 0);                   // issue a query velocity information command for id-motor
                    ackStatus = waitingForACK(spd_len, spdRxBuffer);  // Wait for the motor to answer
                    if (ackStatus == true) {                          // Received velocity information
                        motor_spd = static_cast<int16_t>(
                          static_cast<uint16_t>(spdRxBuffer[4]) << 8 |
                          static_cast<uint16_t>(spdRxBuffer[5]) << 0);
                    }
                    // if failed to receive velocity information
                    // 1. Check the connection of the serial cable; 2. Check whether the motor is powered on; 3. Check the slave address and baud rate
                    else {
                        RCLCPP_ERROR(rclcpp::get_logger("ArmInterface"), "Failed to get velocity data. Retrying ...");
                    }
                }
                return (motor_spd);
            }
        */

    void setArmValues(uint8_t motor_id, uint16_t speed, uint8_t accel, uint32_t position) {
        cmdTxMsg.clear();                                                 // clear old TxMsg buffer
        cmdTxBuffer[1]  = motor_id;                                       // motor address
        cmdTxBuffer[3]  = static_cast<uint8_t>((speed >> 8) & 0xFF);      // higher 8 bit speed
        cmdTxBuffer[4]  = static_cast<uint8_t>((speed >> 0) & 0xFF);      // lower 8 bit speed
        cmdTxBuffer[5]  = accel;                                          // acceleration
        cmdTxBuffer[6]  = static_cast<uint8_t>((position >> 24) & 0xFF);  // position command bit31 - bit24
        cmdTxBuffer[7]  = static_cast<uint8_t>((position >> 16) & 0xFF);  // position command bit23 - bit16
        cmdTxBuffer[8]  = static_cast<uint8_t>((position >> 8) & 0xFF);   // position command bit15 - bit8
        cmdTxBuffer[9]  = static_cast<uint8_t>((position >> 0) & 0xFF);   // position command bit7  - bit0
        cmdTxBuffer[10] = getCheckSum(cmdTxBuffer, 10);                   // Calculate checksum
        for (std::size_t i = 0; i < 11; i++) {
            cmdTxMsg[i] = cmdTxBuffer[i];  // write TxBuffer to TxMsg to be sent (conversion necessary due to LibSerial Write function)
        }
        serial_conn_.FlushIOBuffers();  // just in case
        serial_conn_.Write(cmdTxMsg);   // the serial port issues a command to read the real-time position
        // return messages are switched off, since position is polled regularly anyways
        /*
                std::cout << "sent " << std::hex;
                for (int num = 0; num < 11; num++) {
                    std::cout << static_cast<int>(cmdTxMsg[num]) << " ";
                }
                std::cout << std::endl;
        */
    }


    void startSync() {
        serial_conn_.FlushIOBuffers();  // just in case
        serial_conn_.Write(syncTxMsg);  // the serial port issues a command to all motors to start the synchronous movement
        /*
        // if not all motors react to the sync action call, repeat as often as needed
        for (size_t i = 0; i <= 3; i++) {
            serial_conn_.Write(TxMsg);   // the serial port issues a command to read the real-time position
            usleep(1);
        }
        */
    }


  private:

    // for serial connection
    LibSerial::SerialPort serial_conn_;
    int timeout_ms_;  // timeout before connection error is called
    // for motor communication
    std::vector<uint8_t> cmdTxMsg  = {0xFA, 0, 0xF5, 0, 0, 0, 0, 0, 0, 0, 0};
    std::vector<uint8_t> syncTxMsg = {0xFA, 0x00, 0x4B, 0x45};
    std::vector<uint8_t> reqMsg;
    uint8_t cmdTxBuffer[11] = {0xFA, 0, 0xF5, 0, 0, 0, 0, 0, 0, 0, 0};
    uint8_t reqTxBuffer[7]  = {0xFA, 0, 0, 0, 0, 0, 0};
    uint8_t posRxBuffer[10];
    // uint8_t spdRxBuffer[6];
    uint8_t reqRxBuffer[5];
    int update_rate_ = 50;

    void sendRequest(uint8_t cmd, uint8_t id, uint8_t mode) {
        reqMsg.clear();
        reqTxBuffer[1] = id;
        reqTxBuffer[2] = cmd;
        switch (cmd) {
            case 0x01:
                reqTxBuffer[3] = mode;
                reqTxBuffer[4] = static_cast<uint8_t>((update_rate_ >> 8) & 0xFF);
                reqTxBuffer[5] = static_cast<uint8_t>(update_rate_ & 0xFF);
                reqTxBuffer[6] = getCheckSum(reqTxBuffer, 6);
                break;
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
        else if (cmd == 0x01) {
            reqMsg.resize(7);
            for (std::size_t i = 0; i < 7; i++) {
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


    uint8_t getCheckSum(uint8_t* buffer, uint8_t size) {
        uint8_t i;
        uint16_t sum = 0;
        for (i = 0; i < size; i++) {
            sum += buffer[i];  // Calculate accumulated value
        }
        return (static_cast<uint8_t>(sum & 0xFF));  // return checksum
    }

    /*
        bool waitingForACK(uint8_t len, uint8_t (&rxBuffer)[]) {
            bool retVal;  // return value to flag success/error
            std::vector<uint8_t> rxMsg;
            serial_conn_.Read(rxMsg, len, 1000);  // read received data
            for (std::size_t i = 0; i < len; i++) {
                rxBuffer[i] = rxMsg[i];
            }
            if (rxBuffer[0] == 0xFB) {  // check received header
                if (rxBuffer[len - 1] == getCheckSum(rxBuffer, len - 1)) {
                    retVal = true;  // checksum correct
                }
                else {
                    retVal = false;  // Verification error, return false
                }
            }
            else {
                retVal = false;  // wrong header
            }
            return (retVal);
        }
            */
};
#endif  // LOTTI2_CONTROL__ARM_COMMS_HPP