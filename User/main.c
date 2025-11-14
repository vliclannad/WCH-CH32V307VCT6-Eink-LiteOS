/*
 * Copyright (c) 2013-2019 Huawei Technologies Co., Ltd. All rights reserved.
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include "user.h"
#include "debug.h"
#include "los_tick.h"
#include "los_task.h"
#include "los_config.h"
#include "los_interrupt.h"
#include "los_debug.h"
#include "los_compiler.h"

/* Global define */


/* Global Variable */
__attribute__((aligned (8))) UINT8 g_memStart[LOSCFG_SYS_HEAP_SIZE];
UINT32 g_VlaueSp=0;

// WiFi事件标志组
EVENT_CB_S gWifiEventGroup;


/*********************************************************************
 * @fn      taskSample
 *
 * @brief   taskSample program.
 *
 * @return  none
 */
UINT32 taskSample(VOID)
{
    UINT32  uwRet;
    UINT32 thread_EinkID, thread_WiFiID;
    TSK_INIT_PARAM_S stTask={0};
    stTask.pfnTaskEntry = (TSK_ENTRY_FUNC)thread_Eink_Start;
    stTask.uwStackSize  = 0x800;
    stTask.pcName       = "thread_Eink";
    stTask.usTaskPrio   = 6;
    printf("Creating thread_Eink task.\n");
    uwRet = LOS_TaskCreate(&thread_EinkID, &stTask);
    if (uwRet != LOS_OK) {
        printf("create thread_Eink failed\n");
    }


    stTask.pfnTaskEntry = (TSK_ENTRY_FUNC)thread_WIFI_Start;
    stTask.pcName       = "thread_WIFI";
    stTask.usTaskPrio   = 5;
    printf("Creating thread_WIFI task.\n");
    uwRet = LOS_TaskCreate(&thread_WiFiID, &stTask);
    if (uwRet != LOS_OK) {
        printf("create thread_WIFI failed\n");
    }

}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
LITE_OS_SEC_TEXT_INIT int main(void)
{
    unsigned int ret;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
	USART_Printf_Init(115200);
		
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    LOS_EventInit(&gWifiEventGroup);

    ret = LOS_KernelInit();
    taskSample();
    if (ret == LOS_OK)
    {
        LOS_Start();
    }

    while (1) {
        __asm volatile("nop");
    }
  }
