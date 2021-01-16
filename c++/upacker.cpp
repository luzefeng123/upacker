/**
  ******************************************************************************
  * @file    drv_packer.c
  * @author  zpw
  * @version V1.0
  * @date    
  * @brief   链路层通讯协议
  ******************************************************************************
  * @attention
  *          链路层通讯协议，数据封包解包
  ******************************************************************************
  */
#include "upacker.h"

using namespace std;
/**
 * @brief  使用动态内存时需要初始化
 * @note   size pack缓存的长度，大于最大的数据包长度就行,使用PACK_SIZE
            无rtos最好用静态内存,不然要改heap
 * @param  *cmd_packer: 
 * @param  *handler: 
 * @retval None
 */

const unsigned char CRCHi[256] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
} ;
/* CRC 低字节值表 */
const unsigned char CRCLo[256] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
0x43, 0x83, 0x41, 0x81, 0x80, 0x40
} ;
unsigned short CRC16(uint8_t *puchMsg,uint8_t usDataLen) // 低位在前
{
    unsigned char uchCRCHi = 0xFF; //CRC高字节初始化
    unsigned char uchCRCLo = 0xFF; //CRC低字节初始化
    unsigned char uIndex;  //CRC循环索引

    while (usDataLen--) //消息缓冲区
        {
        uIndex = uchCRCHi ^ *puchMsg++; // 计算CRC
            uchCRCHi = uchCRCLo ^ CRCHi[uIndex];
            uchCRCLo = CRCLo[uIndex] ;
        }
        return(uchCRCHi | uchCRCLo  << 8);
}
int Upacker::upacker_init(PACKER_CB handler, PACKER_CB s)
{
#if USE_DYNAMIC_MEM
    packer->data = (uint8_t *)UP_MALLOC(MAX_PACK_SIZE);
    if (!packer->data)
    {
        return -1;
    }
#endif

    cb = handler;
    send = s;
    return 0;
}

/**
 * @brief  封包数据并发送
 * @note
 * @param  *packer:
 * @param  *buff:
 * @param  size:
 * @retval None
 */
void Upacker::upacker_pack(uint8_t *buff, uint16_t size)
{
     frame_encode(buff, size);
}

/**
 * @brief  解包输入数据
 * @note
 * @param  cmd_packer:
 * @param  *buff:
 * @param  size:
 * @retval None
 */
void Upacker::upacker_unpack(uint8_t *buff, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++)
    {
        if (frame_decode(buff[i]))
        {
            //解析成功,回调处理
            this->cb(this->data, this->flen);
        }
    }
}

uint8_t Upacker::frame_decode(uint8_t d)
{
    if (this->state == 0 && d == STX_L)
    {
        this->state = 1;
        this->calc = 0x55;
    }
    else if (this->state == 1)
       {
           this->flen = d;
           this->calc ^= d; //********
           this->state = 2;
       }
       else if (this->state == 2)
       {
           //长度信息
           this->flen |= (uint16_t)d << 8;
           this->calc ^= d; // & 0x3F; ******************

           //数据包超长得情况下直接丢包
           if ((this->flen & 0x3FFF) > MAX_PACK_SIZE)
           {
               this->state = 0;
           }
           this->state = 3;
           this->cnt = 0;
       }
       else if (this->state == 3) // 头校验
       {
           //header校验
           uint8_t hc = d; //  ((d & 0x03) << 4) | (packer->flen & 0xC000) >> 12;********************


           if (hc != (this->calc ))//& 0X3C))
           {

               this->state = 0;
               return 0;
           }
           this->state = 4;
           this->flen &= 0x3FFF;
       }
           else if (this->state == 4) // 数据低8位校验
           {
              this->check = d<<8;


              this->state = 5;
           }
           else if (this->state == 5) // 数据高8位校验
           {

                this->check |= d;

              this->state = 6;
           }
       else if (this->state == 6)
       {
           this->data[this->cnt++] = d;
       //    packer->calc ^= d;

           if (this->cnt == this->flen)
           {
               this->state = 0;
              uint16_t crc_res;
                       crc_res = CRC16(this->data,this->flen);
               //接收完，检查check
               if (crc_res == (this->check ))
               {
                   return 1;
               }
               else
               {
                   return 0;
               }
           }
       }
    else
    {
        this->state = 0;
    }
    return 0;
}

uint8_t Upacker::frame_encode(uint8_t *data, uint16_t size)
{
    uint8_t tmp[6] = {0};
    uint8_t crc = 0;
    uint16_t data_crc;
    if(size > 16384){
        return 0;
    }

    tmp[0] = 0x55;
tmp[1] = size & 0xff;
tmp[2] = (size >> 8) & 0x3f; //低14位用来保存size;header校验4位
crc = tmp[0] ^ tmp[1] ^ tmp[2];
tmp[3] = crc; //tmp[3]保存header检验

    data_crc =  CRC16(data,size);

    tmp[4] = data_crc >> 8; // crc  校验,低8位
    tmp[5] = data_crc & 0xff; //crc 校验,高8位

    this->send(tmp, 6);
    this->send(data, size);

    return 1;
}
