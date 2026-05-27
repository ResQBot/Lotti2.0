// original code by Duong Quang Duy  https://github.com/hcmutduygit/USB_CAN_A_Waveshare/tree/main
// modified by Res.Q Bots
#ifndef LOTTI2_CONTROL__DRIVE_COMMS_HPP
#define LOTTI2_CONTROL__DRIVE_COMMS_HPP

#pragma once

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <thread>
#include <vector>

#include "lotti2_control/cubeMars_motor.h"
#include "lotti2_control/lotti2_drive_interface.hpp"

class DriveComms {
  public:
    using Callback = std::function<void(uint32_t, const std::vector<uint8_t> &)>;

    DriveComms() = default;

    ~DriveComms() {
        close();
    }


    void open(std::string device) {
        fd_ = ::open(device.c_str(), O_RDWR | O_NOCTTY);
        if (fd_ == -1) {
            throw std::runtime_error("Failed to open serial port: " + device + " (" + std::strerror(errno) + ")");
        }

        struct termios options;
        tcgetattr(fd_, &options);
        cfsetispeed(&options, B2000000);
        cfsetospeed(&options, B2000000);
        options.c_cflag |= (CLOCAL | CREAD);
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;
        options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
        options.c_iflag &= ~(IXON | IXOFF | IXANY);
        options.c_oflag &= ~OPOST;
        options.c_cc[VTIME] = static_cast<unsigned char>(timeout_ * 10);  // Timeout in tenths of a second
        options.c_cc[VMIN]  = 0;                                          // Minimum bytes to read
        tcsetattr(fd_, TCSANOW, &options);

        // Clear input buffer
        tcflush(fd_, TCIFLUSH);
        std::cout << "Serial port opened: " << device << " @ " << baudrate_ << " baud\n";
    }


    void close() {
        rx_running_ = false;
        if (rx_thread_ && rx_thread_->joinable()) {
            rx_thread_->join();
        }
        if (fd_ != -1) {
            ::close(fd_);
            fd_ = -1;
            std::cout << "Serial port closed\n";
        }
    }


    void clearBuffer() {
        tcflush(fd_, TCIFLUSH);
    }


    void sendSpd(uint id, int32_t speed) {
        if (fd_ == -1) {
            throw std::runtime_error("Serial port is not open. Call open() first.");
        }
        std::vector<uint8_t> frame;
        frame.push_back(0xAA);       // Start byte
        frame.push_back(0xE4);       // flags for extended frame and
        frame.push_back(id & 0xFF);  // motor ID
        frame.push_back(0x03);       // send speed flag
        frame.push_back(0x00);       // fill ID with 0
        frame.push_back(0x00);       // fill ID with 0
        frame.push_back(static_cast<__u8>(speed >> 24) & 0xFF);
        frame.push_back(static_cast<__u8>(speed >> 16) & 0xFF);
        frame.push_back(static_cast<__u8>(speed >> 8) & 0xFF);
        frame.push_back(static_cast<__u8>(speed) & 0xFF);
        frame.push_back(0x55);  // Tail byte
        if (write(fd_, frame.data(), frame.size()) != static_cast<ssize_t>(frame.size())) {
            throw std::runtime_error("Failed to write full frame");
        }
    }


    motorState readCANFrame(uint8_t motor_id) {
        motorState state;
        while (state.motor_id != motor_id) {
            std::pair<uint32_t, std::vector<uint8_t>> rxMessage = receive();
            if ((((rxMessage.first) >> 8) & 0xFF) == 0x29) {
                if (((rxMessage.first) & 0xFF) == motor_id) {
                    state.motor_id = motor_id;
                    // int16_t pos_int = (rxMessage.data[0] << 8 | rxMessage.data[1]);
                    int16_t spd_int = (rxMessage.second[2] << 8 | rxMessage.second[3]);
                    int16_t cur_int = (rxMessage.second[4] << 8 | rxMessage.second[5]);
                    //*motor_pos      = static_cast<double>(pos_int * 0.1f);   // motor position
                    state.velocity   = static_cast<double>(spd_int) * 10;    // motor velocity in eRPM
                    state.current    = static_cast<double>(cur_int) * 0.01;  // motor current in A
                    state.motor_temp = rxMessage.second[6];                  // motor temperature in °C
                    state.error_code = rxMessage.second[7];                  // motor error code
                }
            }
        }
        return state;
    }


  private:
    std::string port_;
    uint32_t baudrate_;
    float timeout_;
    int fd_;
    std::atomic<bool> rx_running_;
    std::unique_ptr<std::thread> rx_thread_;


    std::pair<uint32_t, std::vector<uint8_t>> receive() {
        if (fd_ == -1) {
            throw std::runtime_error("Serial port is not open. Call open() first.");
        }

        uint8_t b;
        // Wait for start byte (0xAA)
        while (true) {
            if (read_exact(&b, 1) && b == 0xAA) {
                break;
            }
        }
        // Read CMD byte
        if (!read_exact(&b, 1) || b != 0xE8) {
            // throw std::runtime_error("Invalid CMD byte");
            std::cout << "invalid CMD byte" << static_cast<char>(b);
        }
        // Read CAN ID (4 bytes)
        uint8_t id4, id3, id2, id1;
        if (!read_exact(&id4, 1) || !read_exact(&id3, 1) || !read_exact(&id2, 1) || !read_exact(&id1, 1)) {
            throw std::runtime_error("Failed to read CAN ID");
        }
        // Read 8 data bytes
        std::vector<uint8_t> data(8);
        if (!read_exact(data.data(), 8)) {
            throw std::runtime_error("Failed to read data bytes");
        }
        // Read tail byte
        if (!read_exact(&b, 1) || b != 0x55) {
            throw std::runtime_error("Invalid tail byte");
        }
        uint32_t can_id = id4 | (id3 << 8) | (id2 << 16) | (id1 << 24);

        return {can_id, data};
    }


    bool read_exact(uint8_t *buffer, size_t len) {
        size_t bytes_read = 0;
        auto start_time   = std::chrono::steady_clock::now();

        while (bytes_read < len) {
            ssize_t n = read(fd_, buffer + bytes_read, len - bytes_read);
            if (n > 0) {
                bytes_read += n;
            }
            else if (n == 0 || errno == EAGAIN || errno == EWOULDBLOCK) {
                // Timeout or no data available
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                  std::chrono::steady_clock::now() - start_time)
                                 .count();
                if (elapsed >= static_cast<long int>(timeout_ * 1000)) {
                    return false;  // Timeout
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            else {
                std::cerr << "Read error: " << std::strerror(errno) << "\n";
                return false;
            }
        }
        return true;
    }
};
#endif  // LOTTI2-CONTROL__DRIVE_COMMS_HPP