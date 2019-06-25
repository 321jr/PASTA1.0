/*******************************************************************************
Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.    

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*******************************************************************************/
/*******************************************************************************
* File Name    : config_r_can_api.h
* Version      : 2.00
* Description  : Exports the Renesas CAN API
*******************************************************************************/
/*******************************************************************************
* History : DD.MM.YYYY Version  Description
*         : 22.3.2010  1.00     For RX62N with new CAN API.
*         : 15.4.2010           R_CAN_Control 'Enter Sleep' added.
*                               R_CAN_TxStopMsg added.
*         : 15.06.2011          Added typedefs and defines to support multiple 
*                               CAN channel selection in RX630
*         : 08.12.2011  2.00    Added support for extended ID frame mode.
*         : 06.03.2012          Settings for YRDKRX63N
*******************************************************************************/

#ifndef R_CAN_API_CFG_H
#define R_CAN_API_CFG_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
//#include "platform.h" 

/******************************************************************************
Typedef definitions
******************************************************************************/
/******************************************************************************
Macro definitions
******************************************************************************/
/* ���荞�݂���у|�[�����O
CAN ���[���{�b�N�X�Ŏ�M����ё��M���ꂽ���b�Z�[�W���|�[�����O����ꍇ�́A�}�N��
USE_CAN_POLL ��g�ݍ��݂܂��BCAN ���荞�݂��g�p���邽�߂ɑg�ݍ��܂Ȃ��ł��������B */
#define USE_CAN_POLL        0   //1 or 0

/* CAN ���荞�ݐݒ�
CAN ���荞�݃��x����ݒ肵�܂��B */
#define CAN0_INT_LVL		2
#define CAN1_INT_LVL        2
#define CAN2_INT_LVL        2

/* �����̃R�[�h��ƍ������Ȃ��悤�ɂ��邽�ߍŐV�̃R�[�h�����[�X�ɂ͎�������Ă��܂���B�����̃��[
���{�b�N�X��ݒ肵�Ă��āA����ʂ�CAN �g���t�B�b�N������ꍇ�ɂ́A���̋@�\�̎g�p���������Ă���
�����BMCU �̃n�[�h�E�F�A�}�j���A�����Q�Ƃ��Ă��������B */
#define USE_CAN_API_SEARCH  1

/* Frame ID mode: Standard ID, Extended ID, or Mixed ID mode */
/* Must choose 1 and only 1 from: */
#define FRAME_ID_MODE   STD_ID_MODE
//#define FRAME_ID_MODE   EXT_ID_MODE
//#define FRAME_ID_MODE   MIXED_ID_MODE

/*** Board specific ports ******************************************************
Map the transceiver control pins here. Tranceivers vary in the control pins present
and these may need I/O ports assigned to them. This example has "Enable" and
"Standby" control pins on its tranceiver.
If the board has more than one CAN port, then expand these defines as necessary */

/* Configure CAN0 STBn pin. 
Output. High = not standby. */
//#define CAN0_TRX_STB_PORT     ?
//#define CAN0_TRX_STB_PIN      ?
//#define CAN0_TRX_STB_LVL      1 //High = Not standby.

/* Configure CAN0 EN pin. Output. High to enable CAN transceiver. */
//#define CAN0_TRX_ENABLE_PORT  ?
//#define CAN0_TRX_ENABLE_PIN   ?
//#define CAN0_TRX_ENABLE_LVL   1 //High = Enable.

/* Configure CAN1 STBn pin. 
Output. High = not standby. */
//#define CAN1_TRX_STB_PORT       G
//#define CAN1_TRX_STB_PIN        0
//#define CAN1_TRX_STB_LVL        1 //High = Not standby.

/* Configure CAN1 EN pin. Output. High to enable CAN transceiver. */
//#define CAN1_TRX_ENABLE_PORT    G
//#define CAN1_TRX_ENABLE_PIN     1
//#define CAN1_TRX_ENABLE_LVL     1 //High = Enable.

/* Configure CAN2 STBn pin. 
Output. High = not standby. */
//#define CAN2_TRX_STB_PORT     ?
//#define CAN2_TRX_STB_PIN      ?
//#define CAN2_TRX_STB_LVL      1 //High = Not standby.

/* Configure CAN2 EN pin. Output. High to enable CAN transceiver. */
//#define CAN2_TRX_ENABLE_PORT  ?
//#define CAN2_TRX_ENABLE_PIN   ?
//#define CAN2_TRX_ENABLE_LVL   1 //High = Enable.


/*** RX and TX ports ******************************************************
/* The RX630 and RX63N may have up to 3 CAN channels depending on package.*/
/* Check RX Group User Manual for port availability for your device.      */

    /* Configure the CAN0 RX and TX pins to be used. */
#define CAN0_RX_PORT    P33
    //#define CAN0_RX_PORT    PD2
#define CAN0_TX_PORT    P32
    //#define CAN0_TX_PORT    PD1

    /* Configure the CAN1 RX and TX pins to be used. */
    //#define CAN1_RX_PORT    P15
#define CAN1_RX_PORT   P55
    //#define CAN1_TX_PORT    P14
#define CAN1_TX_PORT   P54

    /* Configure the CAN2 RX and TX pins if used. */
#define CAN2_RX_PORT    P67
#define CAN2_TX_PORT    P66


    /*** Baudrate settings ********************************************************
        Calculation of baudrate:
        ********************************
        *    PCLK = 48 MHz = fcan.     *
        *    fcanclk = fcan/prescale   *
        ********************************
    
        Example 1) 
        Desired baudrate 500 kbps.
        Selecting prescale to 4.
        fcanclk = 48000000/4
        fcanclk = 12000000 Hz
        Bitrate = fcanclk/Tqtot
        or,
        Tqtot = fcanclk/bitrate
        Tqtot = 12000000/500000
        Tqtot = 120/5 = 24.
        Tqtot = TSEG1 + TSEG2 + SS
        Using TSEG1 = 15 Tq
          TSEG2 = 8 Tq
          SS = 1 Tq always
          Re-synchronization Control (SJW) should be 1-4 Tq (must be <=TSEG2). 
    */
    #define CAN_BRP     16
    #define CAN_TSEG1   15
    #define CAN_TSEG2   8
    #define CAN_SJW		2
    
    /* 
    Example 2) Selecting prescale to 8.
    Desired baudrate 500 kbps.
    fcanclk = 48000000/8
    fcanclk = 6000000 Hz
    Tqtot = fcanclk/bitrate
    Tqtot = 6000000/500000
    Tqtot = 60/5 = 12.
    Tqtot = TSEG1 + TSEG2 + SS
    Using     TSEG1 = 8 Tq
            TSEG2 = 3 Tq
            SS = 1 Tq always
            SJW should be 1-4 Tq (<=TSEG2). *
    #define CAN_BRP     8
    #define CAN_TSEG1   8
    #define CAN_TSEG2   3
    #define CAN_SJW     1    */


/* ���W�X�^���|�[�����O���邽�߂̍ő厞��
���Ғl�𓾂邽�߂�CAN ���W�X�^�r�b�g���|�[�����O����ۂ̍ő僋�[�v�ł��B�|�[�����O���[�h���g�p
���Ă���ꍇ�ŁA������̎��Ԃ����ҋ@���ă��[���{�b�N�X���t���[������M�������Ƃ��m�F�������ꍇ
�ɁA���̒l�𑝂₵�Ă��������B����͔��ɏ����Ȓl�ɐݒ肷�邱�Ƃ��ł��܂����A�[���ɂ͐ݒ肵�Ȃ�
�ł��������B�[���ɐݒ肷��ƁA���[���{�b�N�X���܂������`�F�b�N����Ȃ��Ȃ�\��������܂��B */
#define MAX_CANREG_POLLCYCLES   5

#endif    /* R_CAN_API_CFG_H */
/* eof */