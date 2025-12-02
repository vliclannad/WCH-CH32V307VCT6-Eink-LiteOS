/*
 * 串口接收图像数据协议说明
 * ===========================
 * 
 * 帧格式：
 * +--------+--------+----------+------+----------+----------+--------------+
 * | 帧头1  | 帧头2  | 命令类型 | 命令 | 长度高位 | 长度低位 | 有效数据     |
 * +--------+--------+----------+------+----------+----------+--------------+
 * |  0xA5  |  0x5A  |   0xD1   | 0x01 |   High   |   Low    | 总包数+包号+ |
 * |        |        |          |      |          |          | 图片数据     |
 * +--------+--------+----------+------+----------+----------+--------------+
 * 
 * 有效数据格式（图像传输）：
 * - 分包总数：1字节 (例如：10表示总共10包)
 * - 包序号：1字节 (从1开始，例如：1,2,3...10)
 * - 图片数据：n字节 (最大1022字节，因为有效数据最大1024字节)
 * 
 * 使用示例：
 * ----------
 * 1. 初始化WiFi模块后，确保WIFI_RECVSTOP = WIFI_RECV_DATA
 * 2. 发送端按帧格式发送图像数据
 * 3. 接收端自动解析并显示到墨水屏
 * 4. 接收到最后一包后自动刷新屏幕
 * 
 * 发送端伪代码：
 * -------------
 * total_packets = (image_size + 1021) / 1022;  // 计算总包数
 * for(i = 1; i <= total_packets; i++) {
 *     frame[0] = 0xA5;
 *     frame[1] = 0x5A;
 *     frame[2] = 0xD1;
 *     frame[3] = 0x01;
 *     data_len = min(1022, remaining_data) + 2;
 *     frame[4] = (data_len >> 8) & 0xFF;
 *     frame[5] = data_len & 0xFF;
 *     frame[6] = total_packets;
 *     frame[7] = i;
 *     memcpy(&frame[8], &image_data[offset], data_len-2);
 *     send_data(frame, 8 + data_len - 2);
 * }
 */

#include "ESP8266.h"
#include "string.h"
#include "stdio.h"
#include "user_usart.h"

void ESP8266_Delay(uint32_t ms)
{
    volatile uint32_t i;
    // 初略延时，肯定不准
    while(ms--)
    {
        for(i = 0; i < 96000; i++);
    }
}






//wifi复位指令
#define WIFI_RST "AT+RST\r\n"             //模块复位

//wifi连接指令
#define WIFI_MODE_SET "AT+CWMODE="      //设置WIFI模式，1:Station模式  2:SoftAP模式  3:SoftAP+Station模式
#define WIFI_LINK_AP  "AT+CWJAP="         //连接AP
#define WIFI_QUIT_AP  "AT+WJAPQ\r"      //断开AP
#define WIFI_DHCP     "AT+CWDHCP=1,1\r\n"     //开启DHCP
#define WIFI_AUTO_LINK "AT+CWAUTOCONN=1\r\n" // 开启WIFI自动连接
#define WIFI_AUTO_LINK_OFF "AT+CWAUTOCONN=0\r\n" // 关闭WIFI自动连接

//wifi传输模式设置
#define WIFI_CIP_MODE "AT+CIPMODE=" //模式设置，0普通模式，1透传模式

//wifi通信指令
#define WIFI_STATUS "AT+CIPSTATUS\r\n"    //查询连接状态
#define WIFI_START "AT+CIPSTART="         //建立连接
#define WIFI_CLOSE "AT+CIPCLOSE\r\n"      //关闭连接
#define WIFI_SEND "AT+CIPSEND\r\n"        //发送指令
#define WIFI_PING "AT+PING="              //ping指定ip
#define WIFI_DOMAIN_RESOLVE "AT+CIPDOMAIN="  //域名解析

//通信参数设置和查询指令
#define WIFI_STA_MAC_Q "AT+CIPSTAMAC?\r\n"//查询station模式下的mac地址
#define WIFI_STA_MAC_S "AT+CIPSTAMAC="    //设置station模式下的mac地址
#define WIFI_STA_IP    "AT+CIPSTA="       //设置station模式下的IP地址
#define WIFI_SET_AP    "AT+CWSAP="        //设置AP模式的参数

// 全局状态变量（在ESP8266.h中声明为extern）
uint8_t WIFI_RECVSTOP;           // 接收模式标志
uint8_t WIFI_CONNECT_STATE;      // TCP连接状态
uint8_t WIFI_AP_STATE;           // Wi-Fi连接状态
uint8_t WIFI_SEND_STATE;         // 透传模式下发送模式状态标志
uint8_t WIFI_BOOTFLAG;           // 启动完成标志
uint8_t WIFI_CMD_FLAG;           // 命令状态标志

static uint8_t MODE_TYPE;


uint8_t WIFI_SetMode(uint8_t modetype);
uint8_t WIFI_SendCMD(uint8_t* cmdstring);
uint8_t WIFI_SetIP(uint8_t* ip);
uint8_t WIFI_CheckIP(uint8_t* ip);
uint8_t WIFI_CheckPort(uint8_t* port);
uint8_t WIFI_CMD_ReturnCheck();
//======================================================================
//函数名称：WIFI_Init
//功能概要：初始化wifi模块
//参数说明：mode_type:决定模块作为客户端还是AP接入点;
//         mode_name:设置模块AP模式下的SSID;
//         pssword:要设置的密码字符串，必须是64位以内ASCII字符;
//         chn:wifi信道号，1~13，只有1、6、11三个信道互不干扰;
//         ip:设置Wi-Fi模块STA模式下的IP地址,设置为0.0.0.0表示开启DHCP
//函数返回：无
//======================================================================
void WIFI_Init(uint8_t mode_type)
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};
    NVIC_InitTypeDef  NVIC_InitStructure = {0};

    //初始化全局变量
    WIFI_RECVSTOP = 0;                     //接收模式标志
    WIFI_CONNECT_STATE=0;                  //TCP连接状态
    WIFI_AP_STATE=0;                       //Wi-Fi连接状态
    WIFI_SEND_STATE=0;                     //透传模式下发送模式状态标志
    WIFI_BOOTFLAG = 0;                     //启动完成标志
    WIFI_CMD_FLAG = 0;                     //命令状态标志


    uint8_t state = 0;

    RCC_APB1PeriphClockCmd(WIFI_USART_CLK , ENABLE);
    RCC_APB2PeriphClockCmd(WIFI_USART_GPIO_CLK , ENABLE);
    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); //开启AFIO时钟

    //GPIO_PinRemapConfig(GPIO_FullRemap_USART6, ENABLE); //配置GPIO复用功能
    //GPIO初始化
    GPIO_InitStructure.GPIO_Pin = WIFI_USART_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(WIFI_USART_TX_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = WIFI_USART_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(WIFI_USART_RX_GPIO_PORT, &GPIO_InitStructure);
    //串口初始化
    USART_InitStructure.USART_BaudRate = WIFI_USART_BAUDRATE;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(WIFI_USARTx, &USART_InitStructure);
    USART_ITConfig(WIFI_USARTx, USART_IT_RXNE, ENABLE);
    //配置接收中断
    NVIC_InitStructure.NVIC_IRQChannel = WIFI_USART_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    USART_ITConfig(WIFI_USARTx,USART_IT_RXNE,ENABLE);
    USART_Cmd(WIFI_USARTx,ENABLE);

    //如果可以硬件重启WIFI模块，此处不必要，可改成硬件重启操作
    WIFI_RECVSTOP = 3; //接收不做处理
    ESP8266_Delay(500);  // 1秒静默期（发送+++之前）
    USER_UART_SendString(WIFI_USARTx, (uint8_t*)"+++");  // 发送+++,退出透传模式
    ESP8266_Delay(500);  // 1秒静默期（发送+++之后）
    WIFI_RECVSTOP = WIFI_RECV_DATA;  // 切换接收模式为接收数据
    
    printf("关闭回显\r\n");
    state = WIFI_SendCMD((uint8_t*)"ATE0\r\n");//关闭回显
    printf("状态：%d\r\n",state);
    if(state == WIFI_OK)
    {
        printf("回显关闭成功\r\n");
    }

    //设置通信模式
    switch(mode_type)
    {
        case WIFI_STATION:
        printf("开始设置WIFI模式为STA模式");
        if(WIFI_SetMode(WIFI_STATION) == WIFI_OK)              //设置STA模式
        printf("设置WIFI模式为STA模式成功\r\n");
        MODE_TYPE = 0;                          //标记当前模式为STA模式
        //if(WIFI_SendCMD((uint8_t*)WIFI_AUTO_LINK))   //开启wif自动连接
        WIFI_SendCMD((uint8_t*)WIFI_AUTO_LINK_OFF);    //关闭wifi自动连接
        //printf("WIFI模块自动连接成功\r\n");
        break;
        //AP模式
        //混合模式
        default:
        break;
    }
}

//======================================================================
//函数名称：wifi_mode_set
//功能概要：设置WIFI应用模式
//参数说明：modetype:应用模式,=1 Station模式
//                      =2 AP模式
//                      =3 混合模式
//函数返回：模式设置是否成功=0成功，=1错误，=2无返回数据
//======================================================================
uint8_t WIFI_SetMode(uint8_t modetype)
{
    uint8_t cmd[14];
    if(modetype<1||modetype>3)
        return WIFI_ERROR;
    sprintf((char*)cmd,"%s%d\r\n",WIFI_MODE_SET,modetype);
    return (WIFI_SendCMD(cmd));
}

uint8_t WIFI_SetTxMode(uint8_t modetype)
{
    uint8_t cmd[14];
    if(modetype<0||modetype>1)
    {
        return WIFI_ERROR;
    }
    sprintf((char*)cmd,"%s%d\r\n",WIFI_CIP_MODE,modetype);
    if(modetype ==1)
    {
        WIFI_SendCMD(cmd);
        memset(cmd,0,sizeof(cmd));
        sprintf((char*)cmd,"%s\r\n","AT+CIPSEND");
        return(WIFI_SendCMD(cmd));
    }
    else
    {
        return(WIFI_SendCMD(cmd));
    }



}

//======================================================================
//函数名称：wifi_send_cmd
//功能概要：发送wifi模块命令，在中断开启的情况下能够返回WiFi模块应答内容
//参数说明：cmdstring:命令字符串
//函数返回：发送后wifi模块应答状态，=0成功，=1错误，=2无应答
//======================================================================
uint8_t WIFI_SendCMD(uint8_t* cmdstring)
{
    const uint8_t MAX_RETRY = 3;           // 最大重试次数
    const uint16_t TIMEOUT_MS = 20000;      // 超时时间(毫秒)
    const uint16_t CHECK_INTERVAL_MS = 10; // 检查间隔(毫秒)
    
    uint8_t retryCount = 0;
    uint16_t waitTime;
    
    // 1.初始化全局变量
    WIFI_CMD_FLAG = 0;
    printf("切换为命令接收模式\r\n");
    WIFI_RECVSTOP = WIFI_RECV_CMD;  // 切换接收模式为接收命令返回
    
    // 2.发送命令并等待响应（带重试机制）
    while(retryCount < MAX_RETRY)
    {
        // 2.1 发送命令
        printf("发送AT指令(第%d次): %s", retryCount + 1, cmdstring);
        USER_UART_SendString(WIFI_USARTx, cmdstring);
        
        // 2.2 等待响应
        waitTime = 0;
        while(waitTime < TIMEOUT_MS)
        {
            // 成功
            if(WIFI_CMD_FLAG == 1)
            {
                WIFI_RECVSTOP = WIFI_RECV_DATA;
                printf("命令执行成功\r\n");
                return WIFI_OK;
            }
            
            // 失败
            if(WIFI_CMD_FLAG == 2)
            {
                printf("命令执行失败，准备重试\r\n");
                break;  // 跳出内层循环，进行重试
            }
            
            // 让出CPU给其他任务，避免占用CPU
            ESP8266_Delay(CHECK_INTERVAL_MS);
            waitTime += CHECK_INTERVAL_MS;
        }
        
        // 2.3 超时处理
        if(waitTime >= TIMEOUT_MS && WIFI_CMD_FLAG == 0)
        {
            printf("命令超时，无响应\r\n");
            WIFI_RECVSTOP = WIFI_RECV_DATA;
            return WIFI_NORESPONSE;
        }
        
        // 准备重试
        retryCount++;
        if(retryCount < MAX_RETRY)
        {
            // 短暂延时后重试，使用RTOS延时函数
            ESP8266_Delay(100);  // 100ms重试间隔
        }
    }
    
    // 3.所有重试都失败
    printf("命令执行失败，已重试%d次\r\n", MAX_RETRY);
    WIFI_RECVSTOP = WIFI_RECV_DATA;
    return WIFI_ERROR;
}

//======================================================================
//函数名称：WIFI_SetIP
//功能概要：设置sta模式下WiFi模块的IP地址
//参数说明：ip:要设置的IP地址
//函数返回：=0设置成功，=1设置失败，=2WiFi模块无应答
//======================================================================
uint8_t WIFI_SetIP(uint8_t* ip)
{
    uint8_t cmd[30];
    if(!WIFI_CheckIP(ip))
    {
        return WIFI_ERROR;
    }
    sprintf((char*)cmd,"%s\"%s\"\r\n",WIFI_STA_IP,ip);
    return WIFI_SendCMD(cmd);
}
uint8_t WIFI_LinkSSID(uint8_t* ssid,uint8_t* password)
{
        //定义局部变量
    uint8_t cmd[40],flag;

    sprintf((char*)cmd,"%s\"%s\"%s\"%s\"\r\n","AT+CWJAP=",(char*)ssid,",",(char*)password);
    flag=WIFI_SendCMD(cmd);
    if(flag == WIFI_OK)
    {
        WIFI_AP_STATE = 1;
    }
    return (flag);
}
//与服务器建立TCP连接
uint8_t WIFI_TCPConnect(uint8_t* server_ip,uint8_t* server_port)
{
    //定义局部变量
    uint8_t flag,cmd[100];
    //1.检测wifi模块当前状态是否处于发送模式
    if(WIFI_SEND_STATE==1)
        return 2;
    //2.检测输入的ip地址是否合法
    if(!WIFI_CheckIP(server_ip))
        return WIFI_ERROR;
    //3.检测输入的port值是否合法
    if(!WIFI_CheckPort(server_port))
        return WIFI_ERROR;
    //4.拼接指令
    sprintf((char*)cmd,"%s\"%s\",\"%s\",%s\r\n",WIFI_START,"TCP",(char*)server_ip,(char*)server_port);
    //5.发送指令
    flag=WIFI_SendCMD(cmd);
    if(flag == WIFI_OK)
    {
        printf("TCP连接建立成功");
        WIFI_CONNECT_STATE = 1;
    }
    return flag;
}
//断开TCP或UDP连接
uint8_t WIFI_Disconnnect()
{
    return WIFI_SendCMD((uint8_t*)WIFI_CLOSE);
}

//===========================内部函数====================================
//======================================================================
//函数名称：WIFI_CheckIP
//功能概要：检查ip地址是否合法
//参数说明：ip:需要检查的ip地址字符串,格式"xxx.xxx.xxx.xxx",xxx取值范围0-255
//函数返回：=1合法，=0不合法
//======================================================================
uint8_t WIFI_CheckIP(uint8_t* ip)
{
    //定义局部变量
    uint8_t i,j,maxlength;           //i用作标记子块,j用作记录子块长度
    uint16_t length,num[4];
    //初始化局部变量
    for(i=0;i<4;i++)
        num[i]=0;
    maxlength=15;
    //1.检查长度并将整个ip字符串切分成四块
    for(length=0,i=0,j=0;*ip!='\0'&&length<=maxlength;length++)
    {
        if(*ip=='.')
        {
            if(j==0||j>3)
                return 0;
            i++;                     //子块标识加一
            j=0;                     //子块长度清零开始计算下一个子块
        }
        else if(*ip>='0'&&*ip<='9')
        {
            num[i]=num[i]*10+(*ip-'0');
            j++;                     //当前子块长度+1
        }
        else return 0;               //检测到非数字返回0
        ip++;                        //转到下个字符
    }
    if(length>maxlength)             //超长,返回0
        return 0;
    //2.检测子块是否小于等于255
    for(i=0;i<4;i++)
    {
        if(!(num[i]<256))            //检测到超过255的子块,返回0
            return 0;
    }
    return 1;                        //所有条件均满足,返回1
}
uint8_t WIFI_CheckPort(uint8_t* port)
{
    //定义局部变量
    uint8_t length,maxlength;
    uint32_t num;
    maxlength=5;    //端口字符串内容最长5字
    for(length=0,num=0;*port!='\0'&&length<maxlength;length++)
    {
        //是数字则计算新的num值，否则返回0
        if(*port>='0'&&*port<='9')
            num=num*10+(*port-'0');
        else return 0;
        port++;
    }
    //超长返回0
    if(length>maxlength)
        return 0;
    //端口值超出范围
    if(num>65535)
        return 0;
    return 1;
}
//======================================================================
//函数名称：WIFI_CMD_ReturnCheck
//功能概要：检查WiFi模块命令返回状态
//参数说明：无
uint8_t WIFI_CMD_ReturnCheck(void)
{
    if(strstr((char*)WIFI_CMD, "OK"))
    {
        WIFI_CMD_FLAG = 1;
    }
    else if(strstr((char*)WIFI_CMD, "ERROR") || strstr((char*)WIFI_CMD, "FAIL"))
    {
        WIFI_CMD_FLAG = 2;
    }
    
    return WIFI_CMD_FLAG;
}









