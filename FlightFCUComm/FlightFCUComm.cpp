// FlightFCUComm.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include<hidapi/hidapi.h>
#include"main.h"
#include"log.h"
#include"sim_client.hpp"

#ifdef _WIN32
#pragma comment (lib,"setupapi.lib")
#endif

#define M_PID 22352 
#define M_VID 2023

using std::cout,std::endl;

//std::shared_ptr<spdlog::logger> simconnect_logger;
//std::shared_ptr<spdlog::logger> hidapi_logger;
//std::shared_ptr<spdlog::logger> main_logger;

//hid_device* setup_hid();
//void clean_hid();
hid_device* hid_dev;

int main()
{
    //初始化日志
    FCUPanel::Log::Init();
    printf("%zu", sizeof(fcu_panel_hid_t));
    assert(sizeof(fcu_panel_hid_t) == 12);
    // 初始化HIDAPI库
    if (hid_init() != 0) {
        FCU_CRITICAL("无法初始化HIDAPI库");
        return 1;
    }
    FCU_DEBUG("初始化HIDAPI库成功");

    // 打开USB HID设备
    hid_dev = hid_open(M_VID, M_PID, nullptr); // 替换为实际的Vendor ID和Product ID
    if (hid_dev == nullptr) {
        FCU_CRITICAL("无法打开USB HID设备");
        hid_exit();
        return 1;
    }
    FCU_INFO("连接设备成功 VID:{} PID:{}",M_VID,M_PID);

    init_SimConnect();
    
    unsigned char transmit_buf[13] = { 0 };//发送缓冲 13bytes
    uint32_t receive_buf = { 0 };//接收缓冲 4bytes
    bool v_quit = false;
    while(v_quit!=true)
    {

        int bytesRead = hid_read(hid_dev, (uint8_t*)&receive_buf, sizeof(receive_buf));
        if (bytesRead < 0) {
            // 读取数据失败
            // 处理错误
            FCU_ERROR("读取数据失败");
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


    // 在这里可以使用HIDAPI函数与USB HID设备进行通信
    // 例如使用hid_read()函数读取设备发送的数据
    // 例如使用hid_write()函数向设备发送数据

    deinit_SimConnect();

    // 关闭USB HID设备
    FCU_DEBUG("关闭USB HID设备");
    hid_close(hid_dev);

    // 释放HIDAPI库资源
    hid_exit();

    return 0;
}

//void* Memcpy(void* dest, void* src, unsigned int count)
//{
//    assert((dest != NULL) && (src != NULL));
//    int i = 0, k = 0;
//    if (dest == src)
//        return src;
//    unsigned short* d = (unsigned short*)dest;
//    char* s = (char*)src;
//
//    while (count-- > 0)
//    {
//
//        d[i] = (s[k] << 8) | s[k + 1];
//        i += 1;
//        k += 2;
//    }
//    return dest;
//}
// Serialization function
void serialize(const fcu_panel_hid_t* panel, unsigned char* buffer) {
    // Copy each member of panel into buffer with proper byte order
    buffer[0] = (panel->ap_master & 0x01) | ((panel->ap1_active & 0x01) << 1) | ((panel->ap2_active & 0x01) << 2) | ((panel->ap_throttle_active & 0x01) << 3) | ((panel->loc_mode_active & 0x01) << 4) | ((panel->exped_mode_active & 0x01) << 5) | ((panel->appr_mode_active & 0x01) << 6) | ((panel->spd_mach_mode & 0x01) << 7);
    buffer[1] = (panel->trk_fpa_mode & 0x01) | ((panel->spd_dashes & 0x01) << 1) | ((panel->spd_dot & 0x01) << 2) | ((panel->hdg_dashes & 0x01) << 3) | ((panel->hdg_dot & 0x01) << 4) | ((panel->alt_dot & 0x01) << 5) | ((panel->alt_increment & 0x01) << 6) | ((panel->vs_dashes & 0x01) << 7);
    buffer[2] = panel->hdg_selected & 0xFF; // low byte
    buffer[3] = (panel->hdg_selected >> 8) & 0xFF; // high byte
    memcpy(buffer + 4, &(panel->spd_selected), sizeof(float)); // copy float as is
    buffer[8] = panel->fpa_selected; // copy int8 as is
    buffer[9] = panel->vs_selected; // copy int8 as is
    buffer[10] = panel->alt_selected & 0xFF; // low byte
    buffer[11] = (panel->alt_selected >> 8) & 0xFF; // high byte
}

// Deserialization function
void deserialize(unsigned char* buffer, fcu_panel_hid_t* panel) {
    // Copy each byte from buffer into panel with proper byte order
    panel->ap_master = buffer[0] & 0x01;
    panel->ap1_active = buffer[0] & 0x02;
    panel->ap2_active = buffer[0] & 0x04;
    panel->ap_throttle_active = buffer[0] & 0x08;
    panel->loc_mode_active = buffer[0] & 0x10;
    panel->exped_mode_active = buffer[0] & 0x20;
    panel->appr_mode_active = buffer[0] & 0x40;
    panel->spd_mach_mode = buffer[0] & 0x80;

    panel->trk_fpa_mode = buffer[1] & 0x01;
    panel->spd_dashes = (buffer[1] >> 1) & 0x01;
    panel->spd_dot = (buffer[1] >> 2) & 0x01;
    panel->hdg_dashes = (buffer[1] >> 3) & 0x01;
    panel->hdg_dot = (buffer[1] >> 4) & 0x01;
    panel->alt_dot = (buffer[1] >> 5) & 0x01;
    panel->alt_increment = (buffer[1] >> 6) & 0x01;
    panel->vs_dashes = (buffer[1] >> 7) & 0x01;

    panel->hdg_selected = buffer[2] | (buffer[3] << 8); // low byte + high byte
    memcpy(&(panel->spd_selected), buffer + 4, sizeof(float)); // copy float as is
    panel->fpa_selected = buffer[8]; // copy int8 as is
    panel->vs_selected = buffer[9]; // copy int8 as is
    panel->alt_selected = buffer[10] | (buffer[11] << 8); // low byte + high byte
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

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
