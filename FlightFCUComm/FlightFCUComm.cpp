// FlightFCUComm.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include<hidapi/hidapi.h>
#include<csignal>
#include"main.h"
#include"log.h"
#include"sim_client.hpp"

#ifdef _WIN32
#pragma comment (lib,"setupapi.lib")
#endif

#define M_PID 22352 
#define M_VID 2023

using std::cout,std::endl;

hid_device* hid_dev = nullptr;

void safe_exit();

// SIGINT信号处理函数
void signal_handler(int signal)
{
    if (signal == SIGINT)
    {
        FCU_DEBUG("Received SIGINT signal. Exiting...");
        safe_exit();

        std::exit(0);
    }
}

void safe_exit() {
    deinit_SimConnect();

    if (hid_dev) {
        // 关闭USB HID设备
        FCU_DEBUG("关闭USB HID设备");
        hid_close(hid_dev);
    }

    // 释放HIDAPI库资源
    hid_exit();
}

// 等待设备连接
void waitForDeviceConnection()
{
    FCU_INFO("Waiting for device connection...");
    deinit_SimConnect();
    UINT32 i = 0;
    // 不断尝试打开设备，直到成功连接
    while (hid_dev == nullptr)
    {
        hid_dev = hid_open(M_VID, M_PID, nullptr); // 替换为实际的Vendor ID和Product ID
        if (i++ == 10000) {
            i = 0;
            FCU_INFO(".");
        }
    }
    if (init_SimConnect() != 0) {
        safe_exit();
    }
    FCU_INFO("Device connected.");

}

int main()
{
    //初始化日志
    FCUPanel::Log::Init();
    //printf("%zu", sizeof(fcu_panel_hid_t));
    assert(sizeof(fcu_panel_hid_t) == 12);

    // 注册SIGINT信号处理函数
    std::signal(SIGINT, signal_handler);

    // 初始化HIDAPI库
    if (hid_init() != 0) {
        FCU_CRITICAL("无法初始化HIDAPI库");
        return 1;
    }
    FCU_DEBUG("初始化HIDAPI库成功");

    //// 打开USB HID设备
    //hid_dev = hid_open(M_VID, M_PID, nullptr); // 替换为实际的Vendor ID和Product ID
    //if (hid_dev == nullptr) {
    //    FCU_CRITICAL("无法打开USB HID设备");
    //    hid_exit();
    //    return 1;
    //}
    //FCU_INFO("连接设备成功 VID:{} PID:{}",M_VID,M_PID);
    waitForDeviceConnection();

    unsigned char transmit_buf[13] = { 0 };//发送缓冲 13bytes
    uint32_t receive_buf = { 0 };//接收缓冲 4bytes
    while (true)
    {
        waitForDeviceConnection();
        while (true)
        {

            int bytesRead = hid_read(hid_dev, (uint8_t*)&receive_buf, sizeof(receive_buf));
            if (bytesRead < 0) {
                // 读取数据失败
                // 处理错误
                FCU_ERROR("读取数据失败");
                hid_close(hid_dev);
                hid_dev = nullptr;
                break;
            }
            else {
                // 处理读取到的数据
                //FCU_DEBUG("RECEIVE:{:X}", receive_buf);
                if (receive_buf != 0)
                {
                    FCU_TRACE("接收到数据:{:x}", receive_buf);
                    uint32_t mask = 1;
                    for (size_t i = 0; i < 25; i++)
                    {
                        if (receive_buf & mask)
                        {
                            aircraft_event_send((EVENT_ID)(i + 1));
                        }
                        mask <<= 1;
                    }
                    receive_buf = 0;
                }
            }
            simconnect_call();
            //unsigned char data[] = { 0x01, 0x02, 0x03 };
            //int bytesWritten = hid_write(hid_device, transmit_buf, sizeof(transmit_buf));
            //if (bytesWritten < 0) {
            //    // 发送数据失败
            //    // 处理错误
            //}
            //else {
            //    // 数据成功发送
            //}
        }

    }

    // Should not be there
    safe_exit();
    return 0;
}

void hid_data_send(fcu_panel_hid_t* panel) {
    //uint8_t* data = (uint8_t*)panel;
    //printf("sizeof(fcu_panel_hid_t)=%zu\n",sizeof(fcu_panel_hid_t));
    //printf("READY DATA:");
    //for (uint8_t i = 0; i < 12; i++) {
    //    printf("%02X", data[i]);
    //}
    //printf("\n");
    // 
    //!!!注意，第一个字节是report id 对于单output的置为0x00
    uint8_t value[13];
    value[0] = 0;
    memcpy(value + 1, (UINT8*)panel, 12);
    //serialize(panel, value+1);
    int bytesWritten = hid_write(hid_dev, value, 13);
    
    if (bytesWritten < 0) {
        // 发送数据失败
        // 处理错误
        FCU_WARN("数据发送失败!");
    }
    else {
        // 数据成功发送
        //printf("SEND DATA:");
        //for (uint8_t i = 0; i < 12; i++) {
        //    printf("%02X", value[i]);
        //}
        //printf("\n");
        FCU_TRACE("数据共{}字节发送成功!",bytesWritten);
    }
}