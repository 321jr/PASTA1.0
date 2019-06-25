/*
 * MIT License
 *
 * Copyright (c) 2019 LandF Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// -*-c++-*-
// $RCSfile: ecu.c,v $
// $Revision: 1.00 $
// $Date: 2016/12/15 14:14:48 $
// 
// Copyright (c) 2016 LandF Corporation.
//
// History:
//

// 
// �V�X�e����`
//
#include	<stdio.h>
#include	<string.h>
#include	<stdlib.h>
#include	<sysio.h>
#include	"altypes.h"
#include	"iodefine.h"
#include	"timer.h"
#include	"sci.h"
#include	"usb.h"
#include	"r_can_api.h"
#include	"flash_data.h"
#include	"r_Flash_API_RX600.h"

//
// ���[�U��`
// 
#include	"memo.h"			/*	ECU �J������			*/
#include	"ecu.h"				/*	ECU ���ʒ�`			*/
#include	"ecu_io.h"			/*	ECU ���o�̓|�[�g��`	*/
#include	"can3_spi2.h"
#include	"uSD_rspi1.h"
#include	"cantp.h"			/*	CAN-TP ��`				*/

//	�����l�ݒ�w�b�_�g�ݍ���
#include	"ecu_def_config.h"

//---------------------------------------------------------------------------------------
//
// �ϐ��錾 =====
//
//	<<	E2DATA�t���b�V���ۑ��ϐ�	>>
//	���[�e�B���O�}�b�v
ECU_ROUT_MAP	rout_map;	//	�}�b�v�ϐ�
//	��`�ێ��o�b�t�@
CYCLE_EVENTS	conf_ecu;	//	�����E�C�x���g�E�����[�g�Ǘ���`�ϐ�
//	ECU���o�̓`�F�b�N���X�g
EXTERNUL_IO		ext_list[ECU_EXT_MAX];
int				ext_list_count;		//	�O�����o�͏����̓o�^��

//	<<	RAM��݂̂̕ϐ�	>>
//	�^�C���A�b�v�҂��o�b�t�@
CYCLE_EVENTS	wait_tup;	//	�����E�C�x���g�҂��ϐ�
//	���b�Z�[�W�{�b�N�X���̑��M�҂��o�b�t�@
SEND_WAIT_BUF	send_msg[CAN_CH_MAX];
//	CAN�f�[�^�o�b�t�@�ϐ�
CAN_FRAME_BUF	can_buf;
CAN_FRAME_BUF	can_random_mask;
//	���b�Z�[�W�{�b�N�X�͈�
MBOX_SELECT_ID	mbox_sel;

/*
//	CAN�f�[�^�ω��t���O
unsigned char	can_chainge[CAN_ID_MAX];
int				can_chainge_cnt;
unsigned char	can_used_mark[CAN_ID_MAX];
*/

//	�f�[�^���ϊ��e�[�u��
const int	DLC_VALUE_TABLE[] = {0,1,2,3,4,5,6,7,8,8,8,8,8,8,8,8};

//	���v�����[�h�t���O
int	repro_mode = 0;			//	0=�ʏ탂�[�h / 1=���v�����[�h

//	1ms�^�C�}�[�J�E���^
int	timer_count;
//	��ԑ��M�^�C�}�[
int	status_timer;

//	���O�@�\
void	logging(char *fmt, ...);
void SendPC(char *msg);

//	CAN���W���[���ꗗ(CH0�`3)
extern	const can_st_ptr CAN_CHANNELS[];

//	�O�����o�͋@����
EXT_IO_STATUS	exiosts;
unsigned char	exio_chg[EX_IO_MAX];
int				exio_chg_mark;
//	CAN-ID -> EX-I/O-ID �ϊ��e�[�u��
unsigned char	can_to_exio[CAN_ID_MAX];	//	CAN-ID�̎����f�[�^���O��I/O�Ǘ��ԍ��ƂȂ�B00�`3F=�K�p�AFF=�Ή�����


//	ECU�V�[�P���X�ϐ�
int		job = 0;				//	ECU�����t���[
int		led = 0;				//	LED�_��
int		stat_update_id = 0;		//	LCD�֒ʒm����ID�ʒu
#ifdef		__LFY_RX63N__
int		stat_comm = 1;			//	LCD�֒ʒm����ʐM�|�[�g�ԍ�0�`6
#else
int		stat_comm = 0;			//	LCD�֒ʒm����ʐM�|�[�g�ԍ�0�`6
#endif
int		ds_xross_pt_index = -1;	//	DS����I/O���X�g�ԍ��ێ�
int		ds_x_lost_counter = 0;	//	�h���C�r���O�V�~�����[�^�����I�����o�p�J�E���^
//int		ds_conect_active = 0;	//	�h���C�r���O�V�~�����[�^�ڑ��t���O

RX_MB_BUF	rxmb_buf[3];		//	��M�T�u�o�b�t�@

//	LED���j�^�����OID�ݒ�
int				led_monit_id = 0;				//	���j�^�[ID
unsigned char	led_monit_ch = 0;				//	���j�^�[CH�r�b�g�Z�b�g
int				led_monit_first = 0x7FFFFFFF;	//	�ŒZ����
int				led_monit_slow = 0;				//	�Œ�����
int				led_monit_time = 0;				//	���ώ���
int				led_monit_count = 0;			//	���ω���
int				led_monit_sample = 0;			//	�T���v����

//	16�i�������`
const char	HEX_CHAR[] = "0123456789ABCDEF";

//---------------------------------------------------------------------------------------
//  
//  �@�\   : ���ǂ��Ǘ�����ID�����`�F�b�N����
//  
//  ����   : int id		����ID�ԍ�
//  
//  ����   : ��`�o�b�t�@����w��ID�ԍ�����������
//  
//  �߂�l : int		0�ȏ�F��`�o�b�t�@�ԍ� / -1�F�ΏۊOID
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
int search_target_id(int id)
{
	int				i, sid;
	CAN_ID_FORM		*idf;
	
	for(i = 0; i < MESSAGE_MAX && i < conf_ecu.CNT; i++)
	{
		sid = conf_ecu.LIST[i].ID.BIT.SID & CAN_ID_MASK;
		if(sid == id) return i;	//	ID��v
	}
	return -1;
}
//---------------------------------------------------------------------------------------
//  
//  �@�\   : �^�C���A�b�v�҂����X�g���猟��
//  
//  ����   : int id		����ID�ԍ�
//  
//  ����   : ��`�o�b�t�@����w��ID�ԍ�����������
//  
//  �߂�l : int		0�ȏ�F��`�o�b�t�@�ԍ� / -1�F�ΏۊOID
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
int search_wait_id(int id)
{
	int				i, j, n, sid;
	CAN_ID_FORM		*idf;
	
	n = wait_tup.TOP;
	if(n < 0) return -1;	//	�҂�����
	for(i = 0; i < MESSAGE_MAX && n < MESSAGE_MAX; i++)
	{
		sid = wait_tup.LIST[n].ID.BIT.SID & CAN_ID_MASK;
		if(sid == id) return n;	//	ID��v
		n = wait_tup.LIST[n].ID.BIT.NXT;	//	���̑҂����b�Z�[�W�ԍ�
	}
	return -1;
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : ���[���{�b�N�X�̏�Ԏ擾
//  
//  ����   : int ch	CAN�|�[�g�ԍ�
//           int mb ���[���{�b�N�X�ԍ�
//  
//  ����   : ���M�҂��o�b�t�@�̃t���[����CAN���W�X�^�֓]�������M�J�n����
//  
//  �߂�l : ���	0=���g�p(����M�\) / 1=�g�p��
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
int can_check_mb(int ch, int mb)
{
	unsigned char	f;
	switch(ch)
	{
	default:
		return 1;
	case 0:	//	CAN0
		f = CAN0.MCTL[mb].BYTE;
		break;
	case 1:	//	CAN1
		f = CAN1.MCTL[mb].BYTE;
		break;
	case 2:	//	CAN2
		f = CAN2.MCTL[mb].BYTE;
		break;
	case 3:	//	CAN3(MCSP2515-CAN�R���g���[��)
		f = CAN3_GetTxMCTL(mb);
		break;
	}
	return (((f & 0xC0) == 0) ? 0 : 1);
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : CANn�|�[�g�֑��M
//  
//  ����   : SEND_WAIT_FLAME *act	���M�҂����X�g�\���̂ւ̃|�C���^
//			 int mb					���M���[���{�b�N�X�ԍ�
//  
//  ����   : �^�C���A�E�g�ɂ��폜����鑗�M�҂��o�b�t�@�̃t���[�����󂢂Ă���
//			 CAN���W�X�^�֓]�������M�J�n����
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
int can_do_txmb_ch0(SEND_WAIT_FLAME *act, int mb)
{
	int		i;
	
	while(CAN0.MCTL[mb].BYTE != 0) CAN0.MCTL[mb].BYTE = 0x00;
	CAN0.MB[mb].ID.LONG = 0;
	CAN0.MB[mb].ID.BIT.SID = act->ID.BIT.SID;
	CAN0.MB[mb].ID.BIT.RTR = act->ID.BIT.RTR;
	CAN0.MB[mb].DLC = act->ID.BIT.DLC;
	for(i = 0; i < 8; i++) CAN0.MB[mb].DATA[i] = act->FD.BYTE[i];
//	while(CAN0.MB[mb].ID.BIT.SID != act->ID.BIT.SID) CAN0.MB[mb].ID.BIT.SID = act->ID.BIT.SID;
	CAN0.MCTL[mb].BYTE = 0x80;
	return R_CAN_OK;
}
int can_do_txmb_ch1(SEND_WAIT_FLAME *act, int mb)
{
	int		i;
	
	while(CAN1.MCTL[mb].BYTE != 0) CAN1.MCTL[mb].BYTE = 0x00;
	CAN1.MB[mb].ID.LONG = 0;
	CAN1.MB[mb].ID.BIT.SID = act->ID.BIT.SID;
	CAN1.MB[mb].ID.BIT.RTR = act->ID.BIT.RTR;
	CAN1.MB[mb].DLC = act->ID.BIT.DLC;
	for(i = 0; i < 8; i++) CAN1.MB[mb].DATA[i] = act->FD.BYTE[i];
//	while(CAN1.MB[mb].ID.BIT.SID != act->ID.BIT.SID) CAN1.MB[mb].ID.BIT.SID = act->ID.BIT.SID;
	CAN1.MCTL[mb].BYTE = 0x80;
	return R_CAN_OK;
}
int can_do_txmb_ch2(SEND_WAIT_FLAME *act, int mb)
{
	int		i;
	
	while(CAN2.MCTL[mb].BYTE != 0) CAN2.MCTL[mb].BYTE = 0x00;
	CAN2.MB[mb].ID.LONG = 0;
	CAN2.MB[mb].ID.BIT.SID = act->ID.BIT.SID;
	CAN2.MB[mb].ID.BIT.RTR = act->ID.BIT.RTR;
	CAN2.MB[mb].DLC = act->ID.BIT.DLC;
	for(i = 0; i < 8; i++) CAN2.MB[mb].DATA[i] = act->FD.BYTE[i];
//	while(CAN2.MB[mb].ID.BIT.SID != act->ID.BIT.SID) CAN2.MB[mb].ID.BIT.SID = act->ID.BIT.SID;
	CAN2.MCTL[mb].BYTE = 0x80;
	return R_CAN_OK;
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : �폜����郁�b�Z�[�W�̋󂫃��[���{�b�N�X���M����
//  
//  ����   : int ch		CAN�|�[�g�ԍ�
//           int mb 	���b�Z�[�W�{�b�N�X�ԍ��i���[���{�b�N�X�ԍ��j
//           int mi		���b�Z�[�W�ԍ�
//  
//  ����   : �^�C���A�E�g�ɂ��폜����鑗�M�҂��o�b�t�@�̃t���[�����󂢂Ă���
//			 CAN���W�X�^�֓]�������M�J�n����
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void can_powtx_delmb(int ch, int mb, int mi)
{
	int				i, id, bkmb;
	SEND_WAIT_FLAME	*act;
	
	if(ch >= 3) return;	//	CPU����CH�̂ݗL��
	
	//	��MB����
	for(bkmb = 3; bkmb < 16; bkmb++)
	{
		if(can_check_mb(ch, bkmb) == 0) break;
	}
	
	if(bkmb < 16)
	{	//	���[���{�b�N�X�g�p�\
		if(mi >= 0 && mi < MESSAGE_MAX)
		{	//	�҂��L��
			act = &send_msg[ch].BOX[mb].MSG[mi];
			id = act->ID.BIT.SID;
			if(act->ID.BIT.ENB != 0)
			{	//	���o����
				switch(ch)
				{
				case 0:	//CAN0
					can_do_txmb_ch0(act, bkmb);
					break;
				case 1:	//CAN1
					can_do_txmb_ch1(act, bkmb);
					break;
				case 2:	//CAN2
					can_do_txmb_ch2(act, bkmb);
					break;
				}
			//	logging("PowTx CH[%d] MB[%d][%d] ID=%03X\r", ch, mb, mi, id);
			}
		}
	}
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : ���b�Z�[�W�{�b�N�X�����b�Z�[�W�폜����
//  
//  ����   : int ch		���MCAN�`�����l���ԍ�(0�`3)
//         : int mb		���b�Z�[�W�{�b�N�X�ԍ�
//         : int mi		���b�Z�[�W�ԍ�
//  
//  ����   : �w��CAN�`�����l���̑��M�҂����[���{�b�N�X���̎w��t���[�����폜����
//  
//  �߂�l : �Ȃ�
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void delete_mbox_frame(int ch, int mb, int mi)
{
	int				i, id;
	int				nxt;
	CAN_ID_FORM		*idf;
	
	idf = &send_msg[ch].BOX[mb].MSG[mi].ID;
	id = idf->BIT.SID;
	nxt = idf->BIT.NXT;	//	�p���`�F�[���ێ�
	idf->LONG = 0;		//	���b�Z�[�W������
	//	�`�F�[������
	if(send_msg[ch].BOX[mb].TOP == mi && mi != nxt)
	{	//	�擪�̃��b�Z�[�W
		send_msg[ch].BOX[mb].TOP = nxt;
	}
	else
	{	//	���Ԃ̃��b�Z�[�W
		for(i = 0; i < MESSAGE_MAX; i++)
		{
			idf = &send_msg[ch].BOX[mb].MSG[i].ID;
			if(idf->BIT.ENB == 0) continue;
			if(idf->BIT.NXT == mi)
			{	//	�ڑ����`�F�[������
				idf->BIT.NXT = nxt;	//	�`�F�[������O��
			//	logging("Del CH[%d] MB[%d] P%03d ID=%03X\r", ch, mb, mi, id);
				break;
			}
		}
	}
	//	�J�E���^-1
	if(send_msg[ch].BOX[mb].CNT > 0) send_msg[ch].BOX[mb].CNT--;
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : ���M�҂����b�Z�[�W�̃��[���{�b�N�X���M����
//  
//  ����   : int ch	CAN�|�[�g�ԍ�
//           int mb ���b�Z�[�W�{�b�N�X�ԍ��i���[���{�b�N�X�ԍ��j
//  
//  ����   : ���M�҂��o�b�t�@�̃t���[����CAN���W�X�^�֓]�������M�J�n����
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void can_tx_mb(int ch, int mb)
{
	int				i;
	int				mp;
	SEND_WAIT_FLAME	*act;
	uint32_t		lwk;
//	can_frame_t		ftp;
	
	if(can_check_mb(ch, mb) == 0)
	{	//	���[���{�b�N�X�g�p�\
		mp = send_msg[ch].BOX[mb].TOP;	//	���M�҂��`�F�[��
		if(mp >= 0 && mp < MESSAGE_MAX)
		{	//	�҂��L��
			act = &send_msg[ch].BOX[mb].MSG[mp];
			if(act->ID.BIT.ENB != 0)
			{	//	���o����
				switch(ch)
				{
				case 0:	//CAN0
					lwk = can_do_txmb_ch0(act, mb);
					break;
				case 1:	//CAN1
					lwk = can_do_txmb_ch1(act, mb);
					break;
				case 2:	//CAN2
					lwk = can_do_txmb_ch2(act, mb);
					break;
				case 3:	//CAN3(MCP2515)
					lwk = CAN3_TxSet(mb, act);
					break;
				}
				if(lwk == R_CAN_OK)
				{	//	�Z�b�g�A�b�vOK
#ifdef	SORT_TXWAITLIST_ENABLE
					send_msg[ch].BOX[mb].TOP = act->ID.BIT.NXT;	//	���񑗐M�t���[��
#else
					send_msg[ch].BOX[mb].TOP++;
					send_msg[ch].BOX[mb].TOP &= MESSAGE_MSK;	//	�ǂݏo���|�C���^�X�V
#endif
					send_msg[ch].BOX[mb].CNT--;
					act->ID.LONG = 0;		//	�폜
				//	logging("CAN_Tx OK = %03X\r", act->ID.BIT.SID);
				}
				else
				{	//	�G���[����
					logging("CAN_TxSet Err = %08lX\r",lwk);
				}
			}
			else
			{	//	�`�F�[���G���[
				send_msg[ch].BOX[mb].WP = 0;
				send_msg[ch].BOX[mb].TOP = -1;
				send_msg[ch].BOX[mb].CNT = 0;
			}
		}
	}
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : ���b�Z�[�W�{�b�N�XCAN�t���[�����M����
//  
//  ����   : ����
//  
//  ����   : ���M�҂��o�b�t�@�̃t���[����CAN���W�X�^�֓]�������M�J�n����
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void send_mbox_frame(void)
{
	int				ch;
	int				mb;
	
#ifdef	__LFY_RX63N__
	ch = CAN_TEST_LFY_CH;
#else
	for(ch = 0; ch < CAN_CH_MAX; ch++)
#endif
	{	//	CAN�|�[�g�ԍ�
		for(mb = 0; mb < MESSAGE_BOXS; mb++)
		{	//	MBOX�ԍ�
			can_tx_mb(ch, mb);	//	���M�҂������Ƒ��M���s
		}
	}
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : ���b�Z�[�W�{�b�N�XCAN�t���[�����M�o�b�t�@�ςݏグ����
//  
//  ����   : int ch		���MCAN�`�����l���ԍ�(0�`3)
//         : int dlc	���M�f�[�^��
//         : int rtr	���M�t���[���I��(0=�f�[�^/1=�����[�g)
//         : int id		���MID
//  
//  ����   : �w��CAN�`�����l���̑��M�҂��o�b�t�@�֎w��f�[�^��ςݏグ��
//  
//  �߂�l : �Ȃ�
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void add_mbox_frame(int ch, int dlc, int rtr, int id)
{
	int				i;
	int				mb, mi, mp;
	SEND_WAIT_FLAME	*msg, *old, *act;
	//	MBOX�I��
	mb = (id < mbox_sel.CH[ch].MB1) ? 0 : (id < mbox_sel.CH[ch].MB2) ? 1 : 2;
	
	mi = send_msg[ch].BOX[mb].WP;						//	�������݃|�C���^�擾
	send_msg[ch].BOX[mb].WP = (mi + 1) & MESSAGE_MSK;	//	�������݃|�C���^�X�V
	act = &send_msg[ch].BOX[mb].MSG[mi];				//	�o�b�t�@�擾
	//	�g�p�����`�F�b�N
	if(act->ID.BIT.ENB != 0)
	{	//	256�O�̖����M���b�Z�[�W�̓^�C���A�E�g�A�폜����
		can_powtx_delmb(ch, mb, mi);	//	���b�Z�[�W�������M�����݂�
		delete_mbox_frame(ch, mb, mi);	//	���b�Z�[�W�폜
	}
	//	���b�Z�[�W�o�^
	act->ID.LONG = 0;
	act->ID.BIT.SID = id;
	act->ID.BIT.RTR = rtr;	//	�t���[���ݒ�
	act->ID.BIT.ENB = 1;	//	���M�L��
	act->ID.BIT.NXT = MESSAGE_END;
	act->ID.BIT.DLC = dlc;
	act->FD.LONG[0] = can_buf.ID[id].LONG[0];
	act->FD.LONG[1] = can_buf.ID[id].LONG[1];
#ifdef	SORT_TXWAITLIST_ENABLE
	//	���M�҂��`�F�[��
	mp = send_msg[ch].BOX[mb].TOP;
	if(mp < 0 || mp >= MESSAGE_MAX)
	{	//	�҂������Ȃ̂Ő擪�ɂ���
		send_msg[ch].BOX[mb].TOP = mi;	//	1�ڂ̃��b�Z�[�W
	//	logging("Send new = %d:%d:%d:%08lX\r", ch, mb, mi, act->ID.LONG);
	}
	else
	{	//	�҂��L��Ȃ̂Ń`�F�[���ڑ�����
		msg = &send_msg[ch].BOX[mb].MSG[mp];
		if(msg->ID.BIT.SID > id)
		{	//	�擪�̃��b�Z�[�W���D�悳����
			send_msg[ch].BOX[mb].TOP = mi;
			act->ID.BIT.NXT = mp;
		//	logging("Send top = %d:%d:%d:%08lX\r", ch, mb, mi, act->ID.LONG);
		}
		else
		{	//	���ɐڑ�
			for(i = 0; i < MESSAGE_MAX; i++)
			{
				old = msg;	//	1�O��ێ�
				mp = msg->ID.BIT.NXT;	//	���̃��b�Z�[�W
				if(mp >= MESSAGE_MAX && i != mi)
				{	//	�p�������Ȃ̂ŏI�[�ɒǋL
					msg->ID.BIT.NXT = mi;
				//	logging("Send add = %d:%d:%d:%08lX\r", ch, mb, mi, act->ID.LONG);
					break;
				}
				//	�p�����b�Z�[�W
				msg = &send_msg[ch].BOX[mb].MSG[mp];
				if(msg->ID.BIT.SID > id)
				{	//	�D��x���Ⴂ���b�Z�[�W�Ȃ̂ł�����O�Ɋ��荞��
					act->ID.BIT.NXT = mp;
					old->ID.BIT.NXT = mi;
				//	logging("Send ins = %d:%d:%d:%08lX\r", ch, mb, mi, act->ID.LONG);
					break;
				}
			}
		}
	}
#endif
	send_msg[ch].BOX[mb].CNT++;
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : CAN�f�[�^��M����
//  
//  ����   : int ch
//           ��MCAN�`�����l���ԍ�(0�`3)
//         : CAN_MBOX *mbox
//           ��M���b�Z�[�W�ւ̃|�C���^
//  
//  ����   : ���b�Z�[�W�{�b�N�X�ɓ͂����t���[������������B
//           �f�[�^�t���[���̓o�b�t�@�փR�s�[��ɓ]�����������{�B
//           �����[�g�t���[���͑Ώۃf�[�^��ԐM����B
//  
//  �߂�l : �f�[�^�X�V�@0=���� / 1=�L��
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
extern	int  retport;
int can_recv_frame(int ch, CAN_MBOX *mbox)
{
	int				ret = 0;	//	�߂�l
	int				i;
	int				id;			//	ID�擾
	int				dlc;		//	�f�[�^�o�C�g��
	CAN_DATA_BYTE	data;		//	�f�[�^�o�b�t�@
	unsigned char	rxmsk;		//	�`�����l����M�}�X�N
	unsigned char	txmsk;		//	�`�����l�����M�}�X�N
	unsigned char	cgw;		//	�]���t���O
	unsigned char	c1, c2, c3;	//	�J�E���^
	
	id = mbox->ID.BIT.SID;

	dlc = DLC_VALUE_TABLE[mbox->DLC & 15];
//	logging("RX%d - ID=%03X\r", ch, id);
	rxmsk = 0x10 << ch;			//	��M�`�����l���r�b�g
	txmsk = 0x01 << ch;			//	���M�`�����l���r�b�g
	cgw = rout_map.ID[id].BYTE;	//	�]��MAP�r�b�g
	
	if(mbox->ID.BIT.RTR == 0)
	{	//	�f�[�^�t���[��
		if((cgw & rxmsk) == 0 && (cgw & txmsk) != 0)
		{	//	���M��p�͎�M����
			return 0;
		}
		for(i = 0; i < dlc; i++) data.BYTE[i] = mbox->DATA[i];
		for(; i < 8; i++) data.BYTE[i] = 0;	//can_buf.ID[id].BYTE[i];
		//---------------------------------
		//	�h���C�r���O�V�~�����[�^��������
		//---------------------------------
		if(id == DS_X_POWERTRAIN_ID)
		{	//	�h���C�u�V�~�����[�^����ID��M
			if(SELECT_ECU_UNIT == ECU_UNIT_POWERTRAIN)
			{	//	�p���g���݂̂������Ώ�
				if(ds_xross_pt_index >= 0)
				{	//	���o
					if(ext_list[ds_xross_pt_index].PORT.BIT.MODE < 4)
					{	//	ECU�O�����́�CAN�o�̓��[�h�̏ꍇ�ɓK�p
						ext_list[ds_xross_pt_index].PORT.BIT.MODE |= 4;	//	CAN���́�ECU�O���o�͂֕ύX
					}
					ds_x_lost_counter = 0;	//	�p���J�E���^������
					//	������`���邽�߁A��M�f�[�^�̐擪�o�C�g��MSB�𔽓]����
					data.BYTE[0] |= 0x80;
				}
			}
		}
		if(data.LONG[0] == can_buf.ID[id].LONG[0] && data.LONG[1] == can_buf.ID[id].LONG[1] && id < 0x700)
		{	//	�f�[�^�ω�����
			return 0;
		}
		can_buf.ID[id].LONG[0] = data.LONG[0];
		can_buf.ID[id].LONG[1] = data.LONG[1];
		ret = 1;
		//---------------------------------
		//	�g�����X�|�[�g�w����
		//---------------------------------
		if(id >= 0x7DF && id <= 0x7EF)	// || (id >= 0x7BE && id <= 0x7CF))
		{	//CAN-TP��pID
			if(can_tp_job(ch, id, data.BYTE) > 0 && id != 0x7DF) return 0;	//	�P��TP(UDS,OBD2)����
		}
		//---------------------------------
		//	���|�[�g�]������
		//---------------------------------
		if((cgw & rxmsk) != 0)
		{	//	�]�������Ώ�
			txmsk = cgw & ~txmsk;
			if((txmsk & 0x01) != 0)
			{	//	CAN0�]���L��
				add_mbox_frame(0, dlc, CAN_DATA_FRAME, id);
			}
			if((txmsk & 0x02) != 0)
			{	//	CAN1�]���L��
				add_mbox_frame(1, dlc, CAN_DATA_FRAME, id);
			}
			if((txmsk & 0x04) != 0)
			{	//	CAN2�]���L��
				add_mbox_frame(2, dlc, CAN_DATA_FRAME, id);
			}
			if((txmsk & 0x08) != 0)
			{	//	CAN3�]���L��
				add_mbox_frame(3, dlc, CAN_DATA_FRAME, id);
			}
		}
	}
	else
	{	//	�����[�g�t���[��
		if((cgw & rxmsk) != 0)
		{	//	�����Ώ�
			if(search_target_id(id) >= 0)
			{	//	�Ώ�ID�Ȃ̂Ńf�[�^�t���[���ԐM
				add_mbox_frame(ch, dlc, CAN_DATA_FRAME, id);
			}
			//	�]���Ώۊm�F
			txmsk = cgw & ~txmsk;
			if((txmsk & 0x01) != 0)
			{	//	CAN0�]���L��
				add_mbox_frame(0, dlc, CAN_REMOTE_FRAME, id);
			}
			if((txmsk & 0x02) != 0)
			{	//	CAN1�]���L��
				add_mbox_frame(1, dlc, CAN_REMOTE_FRAME, id);
			}
			if((txmsk & 0x04) != 0)
			{	//	CAN2�]���L��
				add_mbox_frame(2, dlc, CAN_REMOTE_FRAME, id);
			}
			if((txmsk & 0x08) != 0)
			{	//	CAN3�]���L��
				add_mbox_frame(3, dlc, CAN_REMOTE_FRAME, id);
			}
		}
	}
	return ret;
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : CAN�f�[�^���M�҂��ςݏグ����
//  
//  ����   : ECU_CYC_EVE *ev
//           �����E�C�x���g���
//  
//  ����   : �����E�C�x���g���ɏ]�����M�o�b�t�@�֐ςݏグ��B
//           �f�[�^�t���[�����M���̓����[�g�t���[�����M���������{����B
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void can_send_proc(ECU_CYC_EVE *ev)
{
	int				id;			//	ID�擾
	int				dlc;		//	�f�[�^�o�C�g��
	unsigned char	msk;		//	�`�����l���}�X�N
	CAN_DATA_BYTE	*act;		//	�A�N�e�B�u�f�[�^
	CAN_DATA_BYTE	*rms;		//	�����_���f�[�^�}�X�N
	CAN_DATA_BYTE	val;		//	�����_���f�[�^
	
	id = ev->ID.BIT.SID;
	dlc = DLC_VALUE_TABLE[ev->ID.BIT.DLC];
	msk = rout_map.ID[id].BYTE & ((repro_mode == 0) ? 0x0F : 0xF0);
	
	if(ev->ID.BIT.RTR == 0)
	{	//	�f�[�^�t���[��
		if(id == DS_X_POWERTRAIN_ID)
		{
			if(ds_xross_pt_index >= 0)
			{
				if(ext_list[ds_xross_pt_index].PORT.BIT.MODE > 3)
				{	//	DS���o��
					if(ds_x_lost_counter < 1000)
					{	//	����M10�b�҂�
						ds_x_lost_counter++;
						if(ds_x_lost_counter >= 1000)
						{
							ext_list[ds_xross_pt_index].PORT.BIT.MODE &= 3;
							//	�f�[�^������
							exiosts.DATA[ext_list[ds_xross_pt_index].PORT.BIT.NOM].LONG = 0;
							can_buf.ID[id].LONG[0] = 0;
							can_buf.ID[id].LONG[1] = 0;
						}
						else return;	//	DS���͑��M�L�����Z��
					}
				}
			}
		}
		if(id < 0x700)
		{	//	700�`7FF��DS����ID�̓����_�������悹�Ȃ�
			//	�����_���f�[�^�����ǉ�����
			act = &can_buf.ID[id];
			rms = &can_random_mask.ID[id];
			val.WORD[0] = rand();
			val.WORD[3] = val.WORD[0];
			val.WORD[1] = rand();
			val.WORD[2] = val.WORD[1];
			act->LONG[0] = ((act->LONG[0] + val.LONG[0]) & (~rms->LONG[0])) | (act->LONG[0] & rms->LONG[0]);
			act->LONG[1] = ((act->LONG[1] - val.LONG[1]) & (~rms->LONG[1])) | (act->LONG[1] & rms->LONG[1]);
		}
		if((msk & 0x01) != 0)
		{	//	CAN0�]���L��
			add_mbox_frame(0, dlc, CAN_DATA_FRAME, id);
		}
		if((msk & 0x02) != 0)
		{	//	CAN1�]���L��
			add_mbox_frame(1, dlc, CAN_DATA_FRAME, id);
		}
		if((msk & 0x04) != 0)
		{	//	CAN2�]���L��
			add_mbox_frame(2, dlc, CAN_DATA_FRAME, id);
		}
		if((msk & 0x08) != 0)
		{	//	CAN3�]���L��
			add_mbox_frame(3, dlc, CAN_DATA_FRAME, id);
		}
	}
	else
	{	//	�����[�g�t���[��
		if((msk & 0x01) != 0)
		{	//	CAN0�]���L��
			add_mbox_frame(0, dlc, CAN_REMOTE_FRAME, id);
		}
		if((msk & 0x02) != 0)
		{	//	CAN1�]���L��
			add_mbox_frame(1, dlc, CAN_REMOTE_FRAME, id);
		}
		if((msk & 0x04) != 0)
		{	//	CAN2�]���L��
			add_mbox_frame(2, dlc, CAN_REMOTE_FRAME, id);
		}
		if((msk & 0x08) != 0)
		{	//	CAN3�]���L��
			add_mbox_frame(3, dlc, CAN_REMOTE_FRAME, id);
		}
	}
	//	���M����
	send_mbox_frame();
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : CAN�f�[�^�^�C���A�b�v���M����(1msec�����Ăяo��)
//  
//  ����   : int tcnt	1ms�^�C�}�[�̃J�E���g�l
//  
//  ����   : 1ms���ɌĂяo�����M�^�C�~���O�҂������E�C�x���g���M����
//           �c����0�ƂȂ������b�Z�[�W�𑗐M�o�b�t�@�ɐςݏグ��
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void can_timer_send(int tcnt)
{
	int				i, n, c;	//	�|�C���^
	int				t;
	int				id;			//	ID�擾
	ECU_CYC_EVE		*act;		//	�����E�C�x���g���
	ECU_CYC_EVE		*old;		//	�����E�C�x���g���
	
	i = wait_tup.TOP;
	if(i < 0)
	{
	//	logging("Wait free\r");
		return;	//	�҂�����
	}
	old = 0;
	c = 0;
	while(i < MESSAGE_MAX && i >= 0)
	{	//	�p��
		c++;
		act = &wait_tup.LIST[i];	//	�^�C���A�b�v�҂��s����
		n = act->ID.BIT.NXT;	//	�p���|�C���^
		if(act->ID.BIT.ENB == 0)
		{	//	��̖����҂����폜
			if(i == wait_tup.TOP)
			{
				if(n < MESSAGE_MAX)
				{	//	�p����擪��
					wait_tup.TOP = n;
				}
				else
				{	//	�҂�����
					wait_tup.TOP = -1;
				}
			}
			else
			{	//	���Ԃ��폜
				if(old != 0)
				{
					old->ID.BIT.NXT = n;
				}
			}
			act->ID.LONG = 0;	//	��~
			wait_tup.CNT--;
		}
		else
		{	//	�����L��
			if(act->TIMER.WORD.TIME == 0)
			{	//	�����M�i�������b�Z�[�W��TIME=0�͑��݂��Ȃ��j
				can_send_proc(act);	//	���M�o�b�t�@�ςݏグ
				if(i == wait_tup.TOP)
				{	//	�擪������
					if(n < MESSAGE_MAX)
					{	//	�p����擪��
						wait_tup.TOP = n;
					}
					else
					{	//	�҂�����
						wait_tup.TOP = -1;
					}
				}
				else
				{	//	���Ԃ��폜
					if(old != 0)
					{
						old->ID.BIT.NXT = n;
					}
				}
				act->ID.LONG = 0;	//	��~
				wait_tup.CNT--;
			}
			else
		//	if(act->TIMER.WORD.CNT > 0)
			{	//	�c���Ԃ���
				t = (int)act->TIMER.WORD.CNT;
				t -= tcnt;
				if(t <= 0)
				{	//	���M�^�C�~���O���B
					can_send_proc(act);	//	���M�o�b�t�@�ςݏグ
					if(act->ID.BIT.REP != 0)
					{	//	�������b�Z�[�W�̃^�C�}�[���Z�b�g
						t += (int)act->TIMER.WORD.TIME;
					}
					else
					{	//	�C�x���g�폜
						if(old != 0)
						{
							old->ID.BIT.NXT = n;
						}
						act->ID.LONG = 0;	//	��~
						wait_tup.CNT--;
					}
				}
				act->TIMER.WORD.CNT = t;
			}
		//	else
		//	{	//	�G���[�C�x���g�폜
		//		act->ID.LONG = 0;	//	��~
		//		old->ID.BIT.NXT = n;
		//		wait_tup.CNT--;
		//	}
		}
		old = act;
		if(i == n || c >= MESSAGE_MAX)
		{	//	�s��ُ�
			wait_tup.TOP = -1;
			logging("Chain Error\r");
			break;
		}
		i = n;
	}
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : �����C�x���g�f�[�^�ǉ�
//  
//  ����   : int rtr			�����[�g�t���[���w��	0/1
//           int id				CAN���b�Z�[�W�ԍ�		0�`2047
//           int dlc			�f�[�^�o�C�g��			0�`8
//           int enb			�������t���O			0/1
//           int rep			�����t���[���w��		0/1
//           int time			�������Ԗ��͒x������(ms)0�`65535
//           int cnt			�x����������(ms)		0�`65535
//  
//  ����   : �����E�C�x���g�̓o�^
//  
//  �߂�l : �o�b�t�@�̒ǉ��ʒu
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
int	add_cyceve_list(int rtr, int id, int dlc, int enb, int rep, int time, int cnt)
{
	int				i;
	int				mb, mi, mp;
	ECU_CYC_EVE		*msg, *old, *act;
	
	//	�󂢂Ă���o�b�t�@������
	for(i = 0; i < MESSAGE_MAX; i++)
	{
		if(conf_ecu.LIST[i].ID.BIT.ENB == 0) break;
	}
	if(i < MESSAGE_MAX)
	{
		mi = i;
	}
	else
	{
		mi = conf_ecu.WP;						//	�������݃|�C���^�擾
		conf_ecu.WP = (mi + 1) & MESSAGE_MSK;	//	�������݃|�C���^�X�V
	}
	act = &conf_ecu.LIST[mi];				//	�o�b�t�@�擾
	//	���b�Z�[�W�o�^
	act->ID.LONG = 0;
	act->ID.BIT.RTR = rtr;	//	�t���[���ݒ�
	act->ID.BIT.SID = id;	//	ID�ݒ�
	act->ID.BIT.DLC = dlc;	//	�f�[�^�o�C�g��
	act->ID.BIT.ENB = enb;	//	�L��
	act->ID.BIT.REP = rep;	//	�������b�Z�[�W�ݒ�
	act->ID.BIT.NXT = MESSAGE_END;
	act->TIMER.WORD.TIME = time;
	act->TIMER.WORD.CNT = cnt;
	//	���M�҂��`�F�[��
	mp = conf_ecu.TOP;
	if(mp < 0)
	{	//	�҂������Ȃ̂Ő擪�ɂ���
		conf_ecu.TOP = mi;	//	1�ڂ̃��b�Z�[�W
		logging("conf new %d\r", mi);
	}
	else
	{	//	�҂��L��Ȃ̂Ń`�F�[���ڑ�����
		msg = &conf_ecu.LIST[mp];
		if(msg->ID.BIT.SID > id)
		{	//	�擪�̃��b�Z�[�W���D�悳����
			conf_ecu.TOP = mi;
			act->ID.BIT.NXT = mp;
			logging("conf top %d\r", mi);
		}
		else
		{	//	���ɐڑ�
			for(i = 0; i < MESSAGE_MAX; i++)
			{
				old = msg;			//	1�O��ێ�
				mp = msg->ID.BIT.NXT;	//	���̃��b�Z�[�W
				if(mp >= MESSAGE_MAX)
				{	//	�p�������Ȃ̂ŏI�[�ɒǋL
					msg->ID.BIT.NXT = mi;
					logging("conf add %d\r", mi);
					break;
				}
				//	�p�����b�Z�[�W
				msg = &conf_ecu.LIST[mp];
				if(msg->ID.BIT.SID > id)
				{	//	�D��x���Ⴂ���b�Z�[�W�Ȃ̂ł�����O�Ɋ��荞��
					act->ID.BIT.NXT = mp;
					old->ID.BIT.NXT = mi;
					logging("conf ins %d\r", mi);
					break;
				}
			}
		}
	}
	conf_ecu.CNT++;
	return mi;
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : �����C�x���g�f�[�^�̓r���ǉ�
//  
//  ����   : int mi		�o�b�t�@�ԍ�		0�`255
//  
//  ����   : �����E�C�x���g�̓o�^�ςݏ���A������
//  
//  �߂�l : �o�b�t�@�̒ǉ��ʒu
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void insert_cyceve_list(int mi)
{
	int				i, id;
	int				mb, mp;
	ECU_CYC_EVE		*msg, *old, *act;
	
	act = &conf_ecu.LIST[mi];	//	�o�b�t�@�擾
	if(act->ID.BIT.ENB == 0) return;	//	�^�]���Ȃ����͘A�����Ȃ�
	id = act->ID.BIT.SID;
	act->ID.BIT.NXT = MESSAGE_END;
	//	���M�҂��`�F�[��
	mp = conf_ecu.TOP;
	if(mp < 0)
	{	//	�҂������Ȃ̂Ő擪�ɂ���
		conf_ecu.TOP = mi;	//	1�ڂ̃��b�Z�[�W
		logging("conf new %d\r", mi);
	}
	else
	{	//	�҂��L��Ȃ̂Ń`�F�[���ڑ�����
		msg = &conf_ecu.LIST[mp];
		if(msg->ID.BIT.SID > id)
		{	//	�擪�̃��b�Z�[�W���D�悳����
			conf_ecu.TOP = mi;
			act->ID.BIT.NXT = mp;
			logging("conf top %d\r", mi);
		}
		else
		{	//	���ɐڑ�
			for(i = 0; i < MESSAGE_MAX; i++)
			{
				old = msg;			//	1�O��ێ�
				mp = msg->ID.BIT.NXT;	//	���̃��b�Z�[�W
				if(mp >= MESSAGE_MAX)
				{	//	�p�������Ȃ̂ŏI�[�ɒǋL
					msg->ID.BIT.NXT = mi;
					logging("conf add %d\r", mi);
					break;
				}
				//	�p�����b�Z�[�W
				msg = &conf_ecu.LIST[mp];
				if(msg->ID.BIT.SID > id)
				{	//	�D��x���Ⴂ���b�Z�[�W�Ȃ̂ł�����O�Ɋ��荞��
					act->ID.BIT.NXT = mp;
					old->ID.BIT.NXT = mi;
					logging("conf ins %d\r", mi);
					break;
				}
			}
		}
	}
	conf_ecu.CNT++;
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : �����C�x���g�f�[�^�폜
//  
//  ����   : int id				CAN���b�Z�[�W�ԍ�		0�`2047
//  
//  ����   : �����E�C�x���g�̍폜
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void delete_cyceve_list(int id)
{
	int				mi, mp;
	ECU_CYC_EVE		*msg, *old;
	
	mp = conf_ecu.TOP;
	while(mp >= 0)
	{
		mi = mp;
		msg = &conf_ecu.LIST[mp];
		mp = msg->ID.BIT.NXT;	//	���̃��b�Z�[�W
		if(msg->ID.BIT.SID == id)
		{	//	��v
			if(conf_ecu.TOP == mi)
			{	//	�擪���폜
				conf_ecu.TOP = mp;
			}
			else
			{	//	�r�����폜
				old->ID.BIT.NXT = mp;
			}
			conf_ecu.CNT--;
			return;
		}
		old = msg;
	}
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : �f�[�^�X�V�C�x���g��������
//  
//  ����   : int id		�ω���������ID�ԍ�
//         : int tp		�ǉ��҂�����(ms)
//  
//  ����   : CAN�ȊO�̊O���@�킩��̃f�[�^�X�V�v���ɂ���ČĂяo�����
//           �C�x���g�����Ώۂ�ID�̓^�C���A�b�v�҂��s��ɒǉ�����
//  
//  �߂�l : �����͓o�^���s / 0�ȏ�͑�������(ms)
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
int can_id_event(int id, int tp)
{
	int				i, n, p;	//	�|�C���^
	int				at;			//	�x������(ms)
	ECU_CYC_EVE		*new;		//	�����E�C�x���g���
	ECU_CYC_EVE		*act;		//	�����E�C�x���g���
	ECU_CYC_EVE		*old;		//	�����E�C�x���g���
	
	//	�Ώ�ID���𒲍�
	i = search_target_id(id);
	if(i < 0) return -1;	//	�ΏۊO��ID
	act = &conf_ecu.LIST[i];
	if(act->ID.BIT.REP != 0)
	{	//	�������b�Z�[�W�̑��d�o�^�͋֎~
		n = search_wait_id(id);
		if(n >= 0) return -2;	//	�o�^�ς�
	}
	p = wait_tup.WP++;
	wait_tup.WP &= MESSAGE_MSK;
	new = &wait_tup.LIST[p];
	if(new->ID.BIT.ENB != 0)
	{
		//	���g�p�҂�����
		for(p = 0; p < MESSAGE_MAX; p++)
		{
			if(wait_tup.LIST[p].ID.BIT.ENB == 0) break;	//	���g�p�ԍ�����
		}
		if(p >= MESSAGE_MAX) return -3;	//	�󂫖���
	}
	//	�o�^
	new = &wait_tup.LIST[p];
	new->ID.LONG = act->ID.LONG;						//	���b�Z�[�W�����R�s�[
	new->TIMER.LONG = act->TIMER.LONG;					//	�ݒ�l�R�s�[
	at = (int)act->TIMER.WORD.CNT;						//	�x������(ms)
	new->TIMER.WORD.CNT = new->TIMER.WORD.TIME + tp;	//	�҂�����(ms)
	new->ID.BIT.NXT = MESSAGE_END;						//	�p������
	new->ID.BIT.ENB = 1;								//	�����L����
	//	�҂��擪�m�F
	i = wait_tup.TOP;
	if(i < 0)
	{	//	�҂�����(�擪)
		wait_tup.TOP = p;								//	�擪�ɂ���
		wait_tup.CNT = 1;								//	���݂̑҂���1��
		logging("Wait new %08lX:%d\r", new->ID.LONG, p);
		return at;										//	�p���o�^�̒x������(ms)
	}
	//	�}���挟��
	old = 0;
	while(i < MESSAGE_MAX)
	{
		act = &wait_tup.LIST[i];	//	�^�C���A�b�v�҂��s����
		n = act->ID.BIT.NXT;	//	�p���|�C���^
		if(act->ID.BIT.ENB != 0)
		{	//	�����L��
			if(act->ID.BIT.SID > id)
			{	//	�D��x���Ⴂ���b�Z�[�W����
				if(i == wait_tup.TOP)
				{	//	�擪�����ւ�
					wait_tup.TOP = p;
					new->ID.BIT.NXT = i;
					wait_tup.CNT++;	//	�҂�������
					logging("Wait top %08lX:%d��%d\r", new->ID.LONG, p, i);
					return at;
				}
				else
				{	//	�r���ɒǉ�
					old->ID.BIT.NXT = p;
					new->ID.BIT.NXT = i;
					wait_tup.CNT++;	//	�҂�������
					logging("Wait ins %08lX:%d��%d\r", new->ID.LONG, p, i);
					return at;
				}
			}
			else
			if(n >= MESSAGE_MAX)
			{	//	�Ō���ɒǉ�
				act->ID.BIT.NXT = p;
				wait_tup.CNT++;	//	�҂�������
				logging("Wait add %08lX:��%d\r", new->ID.LONG, p);
				return at;
			}
		}
		old = act;	//	�ЂƂO�̏��
		if(i == n)
		{	//	�`�F�[���ُ�
			logging("Wait chain error %08lX:%d��%d\r", new->ID.LONG, i, n);
			break;
		}
		i = n;		//	�p���|�C���^
	}
	return -4;	//	�`�F�[�����X�g�j��
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : �����C�x���g�^�C���A�b�v�҂��f�[�^�폜
//  
//  ����   : int id				CAN���b�Z�[�W�ԍ�		0�`2047
//  
//  ����   : �����E�C�x���g�^�C���A�b�v�҂��f�[�^�̍폜
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void delete_waiting_list(int id)
{
	int				mi, mp;
	ECU_CYC_EVE		*msg, *old;
	
	mp = wait_tup.TOP;
	while(mp >= 0)
	{
		mi = mp;
		msg = &wait_tup.LIST[mp];
		mp = msg->ID.BIT.NXT;	//	���̃��b�Z�[�W
		if(msg->ID.BIT.SID == id)
		{	//	��v
			if(wait_tup.TOP == mi)
			{	//	�擪���폜
				wait_tup.TOP = mp;
			}
			else
			{	//	�r�����폜
				old->ID.BIT.NXT = mp;
			}
			wait_tup.CNT--;
			return;
		}
		old = msg;
	}
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : �ʐM�o�R�O�����͒l�擾
//  
//  ����   : int nom	�o�b�t�@�ԍ�0�`63
//		     int md		���[�h		0:BIT / 1:BYTE / 2:WORD / 3:LONG
//  
//  ����   : ECU�O��I/O�ڑ��R�l�N�^�̓��͏�Ԃ��擾����
//  
//  �߂�l : int	���͒l�i�f�W�^����0/1�A�A�i���O��0�`FFF�j
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
int port_input_ex(int nom, int md)
{
	switch(md)
	{
	default:
		break;
	case 0:	//	�r�b�g���
		return ((int)exiosts.DATA[nom].BIT.B0);
	case 1:	//	�o�C�g���
		return ((int)exiosts.DATA[nom].BYTE[0]);
	case 2:	//	���[�h���
		return ((int)exiosts.DATA[nom].WORD[0]);
	case 3:	//	�����O���[�h���
		return (exiosts.DATA[nom].INTE);
	}
	return 0;
}
//---------------------------------------------------------------------------------------
//  
//  �@�\   : �O���o�͒l�X�V
//  
//  ����   : int nom	�o�b�t�@�ԍ�0�`63
//         : int val	�o�͒l
//		     int md		���[�h		0:BIT / 1:BYTE / 2:WORD / 3:LONG
//  
//  ����   : ECU�O��I/O�ڑ��R�l�N�^�̏o�͏�Ԃ��X�V����
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void port_output_ex(int nom, int val, int md)
{
	switch(md)
	{
	case 4:	//	�r�b�g���
		val = (val == 0) ? 0 : 1;
		if(exiosts.DATA[nom].BIT.B0 != val)
		{
			if(exio_chg[nom] < 100) exio_chg[nom]++;		//	�f�[�^�X�V�ʒm
			exio_chg_mark++;	//	�X�V�}�[�N
		}
		exiosts.DATA[nom].BIT.B0 = (val == 0) ? 0 : 1;
		break;
	case 5:	//	�o�C�g���
		if(exiosts.DATA[nom].BYTE[0] != val)
		{
			if(exio_chg[nom] < 100) exio_chg[nom]++;		//	�f�[�^�X�V�ʒm
			exio_chg_mark++;	//	�X�V�}�[�N
		}
		exiosts.DATA[nom].BYTE[0] = (unsigned char)val;
		break;
	case 6:	//	���[�h���
		if(exiosts.DATA[nom].WORD[0] != val)
		{
			if(exio_chg[nom] < 100) exio_chg[nom]++;		//	�f�[�^�X�V�ʒm
			exio_chg_mark++;	//	�X�V�}�[�N
		}
		exiosts.DATA[nom].WORD[0] = (unsigned short)val;
		break;
	case 7:	//	�����O���[�h���
		if(exiosts.DATA[nom].INTE != val)
		{
			if(exio_chg[nom] < 100) exio_chg[nom]++;		//	�f�[�^�X�V�ʒm
			exio_chg_mark++;	//	�X�V�}�[�N
		}
		exiosts.DATA[nom].INTE = val;
		break;
	}
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : �ʐM�o�R�O�����o�͍X�V����
//  
//  ����   : ����
//  
//  ����   : ECU�O��I/O�ڑ��R�l�N�^�̓��͏�Ԃ��擾���K�p��f�[�^�o�b�t�@���X�V����
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void extern_io_update_ex(void)
{
	int						i, d, m, tp;
	int						id;		//	ID�ԍ�
	int						md;		//	I/O�������[�h
	int						bp;		//	�o�C�g�ʒu
	int						nm;		//	I/O�|�[�g�ԍ�
	int						msk;	//	�r�b�g���[�h�}�X�N
	EXTERNUL_IO				*act;	//	�A�N�e�B�u��`�|�C���^
	CAN_DATA_BYTE			*buf;	//	�t���[���f�[�^�o�b�t�@
	CAN_DATA_BYTE			val;	//	�R�s�[�t���[���f�[�^
	
	tp = 0;	//	�������͕ω��̎����ݒ�
	
	for(i = 0; i < ext_list_count; i++)
	{
		act = &ext_list[i];
		id = act->SID;
		if(id >= 0 && id < CAN_ID_MAX)
		{	//	�ݒ�L��
			nm = act->PORT.BIT.NOM;			//	�o�b�t�@�ԍ�
			md = act->PORT.BIT.MODE;		//	���[�h			���́F0:bit / 1:byte / 2:word / 3:long�@�o�́F4:bit / 5:byte / 6:word / 7:long
			bp = act->PORT.BIT.BPOS;		//	�o�C�g�ʒu
			msk = act->PORT.BIT.MSK;		//	�}�X�N�p�^�[��
			buf = &can_buf.ID[id];			//	�f�[�^�o�b�t�@�ւ̃|�C���^
			val.LONG[0] = buf->LONG[0];		//	�f�[�^�擾1
			val.LONG[1] = buf->LONG[1];		//	�f�[�^�擾2
			//	��Ԏ擾
			d = port_input_ex(nm, (md & 3));	//	�ʐM�o�R���͏�Ԏ擾
			//	���[�h����
			switch(md)
			{
				default:	//	����
					break;
				case 0:		//	�r�b�g����
					val.BYTE[bp] &= ~msk;
					val.BYTE[bp] |= (d == 0) ? 0 : msk;
					break;
				case 4:		//	�r�b�g�o��
					d = ((val.BYTE[bp] & msk) == 0) ? 0 : 1;
					port_output_ex(nm, d, md);
					break;
				case 1:		//	�o�C�g����
					val.BYTE[bp] = (unsigned char)d;
					break;
				case 5:		//	�o�C�g�o��
					d = (int)((unsigned long)val.BYTE[bp]);
					port_output_ex(nm, d, md);
					break;
				case 2:		//	���[�h����
					val.BYTE[bp] = (unsigned char)(d >> 8);
					val.BYTE[bp + 1] = (unsigned char)d;
					break;
				case 6:		//	���[�h�o��
					d = (int)((((unsigned long)val.BYTE[bp]) << 8) | ((unsigned long)val.BYTE[bp + 1]));
					port_output_ex(nm, d, md);
					break;
				case 3:		//	�����O���[�h����
					val.BYTE[bp] = (unsigned char)(d >> 24);
					val.BYTE[bp + 1] = (unsigned char)(d >> 16);
					val.BYTE[bp + 2] = (unsigned char)(d >> 8);
					val.BYTE[bp + 3] = (unsigned char)d;
					break;
				case 7:		//	�����O���[�h�o��
					d = (int)((((unsigned long)val.BYTE[bp]) << 24) | (((unsigned long)val.BYTE[bp + 1]) << 16) | (((unsigned long)val.BYTE[bp + 2]) << 8) | ((unsigned long)val.BYTE[bp + 3]));
					port_output_ex(nm, d, md);
					break;
			}
			if(val.LONG[0] != buf->LONG[0] || val.LONG[1] != buf->LONG[1])
			{	//	�ω��L��
				buf->LONG[0] = val.LONG[0];				//	�f�[�^�X�V1
				buf->LONG[1] = val.LONG[1];				//	�f�[�^�X�V2
				if(exio_chg[nm] < 100) exio_chg[nm]++;	//	CAN�f�[�^�X�V�J�E���g
				exio_chg_mark++;						//	�X�V�}�[�N
				tp = can_id_event(id, tp);				//	CAN�f�[�^�X�V�ʒm
			}
		}
	}
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : �O�����͒l�擾
//  
//  ����   : int nom	�|�[�g�ԍ��i0�`33�j
//  
//  ����   : ECU�O��I/O�ڑ��R�l�N�^�̓��͏�Ԃ��擾����
//  
//  �߂�l : int	���͒l�i�f�W�^����0/1�A�A�i���O��0�`FFF�j
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
int port_input(int nom)
{
	switch(nom)
	{
		case 0:		return (X_DB_0 == 0) ? 0 : -1;
		case 1:		return (X_DB_1 == 0) ? 0 : -1;
		case 2:		return (X_DB_2 == 0) ? 0 : -1;
		case 3:		return (X_DB_3 == 0) ? 0 : -1;
		case 4:		return (X_DB_4 == 0) ? 0 : -1;
		case 5:		return (X_DB_5 == 0) ? 0 : -1;
		case 6:		return (X_DB_6 == 0) ? 0 : -1;
		case 7:		return (X_DB_7 == 0) ? 0 : -1;
		case 8:		return (X_DB_8 == 0) ? 0 : -1;
		case 9:		return (X_DB_9 == 0) ? 0 : -1;
		case 10:	return (X_DB_10 == 0) ? 0 : -1;
		case 11:	return (X_DB_11 == 0) ? 0 : -1;
		case 12:	return (X_DB_12 == 0) ? 0 : -1;
		case 13:	return (X_DB_13 == 0) ? 0 : -1;
		case 14:	return (X_DB_14 == 0) ? 0 : -1;
		case 15:	return (X_DB_15 == 0) ? 0 : -1;
		case 16:	return (X_DB_16 == 0) ? 0 : -1;
		case 17:	return (X_DB_17 == 0) ? 0 : -1;
		case 18:	return (X_DB_18 == 0) ? 0 : -1;
		case 19:	return (X_DB_19 == 0) ? 0 : -1;
		case 20:	return (X_DB_20 == 0) ? 0 : -1;
		case 21:	return (X_DB_21 == 0) ? 0 : -1;
		case 22:	return (X_DB_22 == 0) ? 0 : -1;
		case 23:	return (X_DB_23 == 0) ? 0 : -1;
		case 24:	return (X_AN_0 & 0x0FFF);
		case 25:	return (X_AN_1 & 0x0FFF);
		case 26:	return (X_AN_2 & 0x0FFF);
		case 27:	return (X_AN_3 & 0x0FFF);
		case 28:	return (X_AN_4 & 0x0FFF);
		case 29:	return (X_AN_5 & 0x0FFF);
		case 30:	return (X_AN_6 & 0x0FFF);
		case 31:	return (X_AN_7 & 0x0FFF);
	}
	return 0;
}
//---------------------------------------------------------------------------------------
//  
//  �@�\   : �O���o�͒l�X�V
//  
//  ����   : int nom	�|�[�g�ԍ�0�`33
//         : int val	�o�͒l
//  
//  ����   : ECU�O��I/O�ڑ��R�l�N�^�̏o�͏�Ԃ��X�V����
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void port_output(int nom, int val)
{
	switch(nom)
	{
		case 0:		Y_DB_0 = (val == 0) ? 0 : 1;	break;
		case 1:		Y_DB_1 = (val == 0) ? 0 : 1;	break;
		case 2:		Y_DB_2 = (val == 0) ? 0 : 1;	break;
		case 3:		Y_DB_3 = (val == 0) ? 0 : 1;	break;
		case 4:		Y_DB_4 = (val == 0) ? 0 : 1;	break;
		case 5:		Y_DB_5 = (val == 0) ? 0 : 1;	break;
		case 6:		Y_DB_6 = (val == 0) ? 0 : 1;	break;
		case 7:		Y_DB_7 = (val == 0) ? 0 : 1;	break;
		case 8:		Y_DB_8 = (val == 0) ? 0 : 1;	break;
		case 9:		Y_DB_9 = (val == 0) ? 0 : 1;	break;
		case 10:	Y_DB_10 = (val == 0) ? 0 : 1;	break;
		case 11:	Y_DB_11 = (val == 0) ? 0 : 1;	break;
		case 12:	Y_DB_12 = (val == 0) ? 0 : 1;	break;
		case 13:	Y_DB_13 = (val == 0) ? 0 : 1;	break;
		case 14:	Y_DB_14 = (val == 0) ? 0 : 1;	break;
		case 15:	Y_DB_15 = (val == 0) ? 0 : 1;	break;
		case 16:	Y_DB_16 = (val == 0) ? 0 : 1;	break;
		case 17:	Y_DB_17 = (val == 0) ? 0 : 1;	break;
		case 18:	Y_DB_18 = (val == 0) ? 0 : 1;	break;
		case 19:	Y_DB_19 = (val == 0) ? 0 : 1;	break;
		case 20:	Y_DB_20 = (val == 0) ? 0 : 1;	break;
		case 21:	Y_DB_21 = (val == 0) ? 0 : 1;	break;
		case 22:	Y_DB_22 = (val == 0) ? 0 : 1;	break;
		case 23:	Y_DB_23 = (val == 0) ? 0 : 1;	break;
		case 24:	Y_AN_0 = (val >> 2) & 0x03FF;	break;
		case 25:	Y_AN_1 = (val >> 2) & 0x03FF;	break;
	}
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : �O�����o�͍X�V����
//  
//  ����   : ����
//  
//  ����   : ECU�O��I/O�ڑ��R�l�N�^�̓��͏�Ԃ��擾���K�p��f�[�^�o�b�t�@���X�V����
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void extern_io_update(void)
{
	int						i, d, m, tp;
	int						id;		//	ID�ԍ�
	int						md;		//	I/O�������[�h
	int						ng;		//	�f�[�^���]�t���O
	int						sz;		//	�A�N�Z�X�T�C�Y
	int						bp;		//	�o�C�g�ʒu
	int						dlc;	//	�f�[�^��
	int						nm;		//	I/O�|�[�g�ԍ�
	int						msk;	//	�r�b�g���[�h�}�X�N
	unsigned char			*pat;	//	�p�^�[���f�[�^
	EXTERNUL_IO				*act;	//	�A�N�e�B�u��`�|�C���^
	CAN_DATA_BYTE			*buf;	//	�t���[���f�[�^�o�b�t�@
	CAN_DATA_BYTE			val;	//	�R�s�[�t���[���f�[�^
	
	tp = 0;	//	�������͕ω��̎����ݒ�

	for(i = 0; i < ext_list_count; i++)
	{
		act = &ext_list[i];
		id = act->SID;
		if(id >= 0 && id < CAN_ID_MAX)
		{	//	�ݒ�L��
			nm = act->PORT.BIT.NOM;			//	�|�[�g�ԍ�
			md = act->PORT.BIT.MODE;		//	���[�h			0:���� / 1:DI / 2:DO / 3:AI / 4:AO / 5:2bit�l / 6:3bit�l / 7:4bit�l
			ng = act->PORT.BIT.NEG;			//	���]�t���O
			sz = act->PORT.BIT.SIZE;		//	�A�N�Z�X�T�C�Y	0:BIT / 1�`7:nBYTE
			bp = act->PORT.BIT.BPOS;		//	�o�C�g�ʒu
			dlc = act->PORT.BIT.DLC;		//	��r�f�[�^��
			msk = act->PORT.BIT.MSK;		//	�}�X�N�p�^�[��
			pat = act->PAT;					//	�p�^�[���f�[�^
			buf = &can_buf.ID[act->SID];		//	�f�[�^�o�b�t�@�ւ̃|�C���^
			val.LONG[0] = buf->LONG[0];		//	�f�[�^�擾1
			val.LONG[1] = buf->LONG[1];		//	�f�[�^�擾2
			//	��Ԏ擾
			m = 1;
			d = port_input(nm);				//	���͏�Ԏ擾
			if(md > 2 && md < 5)
			{	//	ADC���[�h����
				m = 0x0FFF;	//	12bit
			}
			else
			{	//	�r�b�g����
				if(md > 4)
				{	//	�����r�b�g
					d = ((d & 1) << 1) | (port_input(nm + 1) & 1);
					m = (m << 1) | 1;
					if(md > 5)
					{	//	�����r�b�g
						d = ((d & 1) << 1) | (port_input(nm + 2) & 1);
						m = (m << 1) | 1;
						if(md > 6)
						{	//	�����r�b�g
							d = ((d & 1) << 1) | (port_input(nm + 3) & 1);
							m = (m << 1) | 1;
						}
					}
				}
			}
			//	���]�v��
			if(ng)
			{	//	�f�[�^���]
				d = ~d;
				d &= m;
			}
			//	���[�h����
			switch(md)
			{
				default:	//	����
					break;
				case 5:		//	2bit�l
				case 6:		//	3bit�l
				case 7:		//	4bit�l
				case 1:		//	�r�b�g����
					if(sz == 0)
					{	//	�r�b�g����
						val.BYTE[bp] &= ~msk;
						val.BYTE[bp] |= (d == 0) ? 0 : msk;
					}
					else
					{	//	�p�^�[���]��
						memcpy(&val.BYTE[bp], &pat[sz * d], sz);
					}
					tp = can_id_event(id, tp);	//	CAN�o�X�֒ʒm
					break;
				case 2:		//	�r�b�g�o��
					if(sz == 0)
					{	//	�r�b�g����
						d = ((val.BYTE[bp] & msk) == 0) ? 0 : 1;
						if(ng) d = ~d & 1;
						port_output(nm, d);
					}
					else
					{	//	�p�^�[����r
						for(d = 0; d < 2; d++)
						{
							if(memcmp(&val.BYTE[bp], &pat[(sz * d)], sz) == 0)
							{	//	��v
								if(ng) d = ~d & 1;
								port_output(nm, d);
								break;
							}
						}
					}
					break;
				case 3:		//	ADC���[�h����
					val.BYTE[bp] = (unsigned char)(d >> 8);
					val.BYTE[bp + 1] = (unsigned char)(d & 0xFF);
					tp = can_id_event(id, tp);	//	CAN�o�X�֒ʒm
					break;
				case 4:		//	DAC���[�h�o��
					d = (((unsigned long)val.BYTE[bp]) << 8) | ((unsigned long)val.BYTE[bp + 1]);
					if(act->PORT.BIT.MODE & 0x08)
					{	//	���]
						d = (0 - d) & 0x0FFF;	//	12bit��
					}
					port_output(nm, d);
					break;
			}
			if(val.LONG[0] != buf->LONG[0] || val.LONG[1] != buf->LONG[1])
			{	//	�ω��L��
				buf->LONG[0] = val.LONG[0];		//	�f�[�^�X�V1
				buf->LONG[1] = val.LONG[1];		//	�f�[�^�X�V2
			//	if(can_chainge[act->SID] < 100) can_chainge[act->SID]++;		//	CAN�f�[�^�X�V�ʒm
			//	can_chainge_cnt = 1;	//	�X�V�}�[�N
			}
		}
	}
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : �O�����o�͓o�^����
//  
//  ����   : int id		�t���[��ID�ԍ�		0�`2047(000�`7FF)
//           int mode	I/O�������[�h		0:���� / 1:DI / 2:DO / 3:AI / 4:AO / 5:2bit�l / 6:3bit�l / 7:4bit�l
//           int neg	�f�[�^���]�w��		0:�ʏ� / 1:���]
//           int size	�A�N�Z�X�T�C�Y		0:BIT / 1�`7:nBYTE
//           int bpos	�o�C�g�ʒu			0�`7
//           int dlc	�f�[�^�o�C�g��		0�`8
//           int nom	�K�p�|�[�g�ԍ�		0�`63
//           int msk	�}�X�N�p�^�[��		00�`FF
//           unsigned char *pat	�p�^�[���f�[�^	24byte
//  
//  ����   : ECU�O��I/O�ڑ��R�l�N�^�̓��͏�Ԃ��擾���K�p��f�[�^�o�b�t�@���X�V����
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
int add_extern_io(int id, int mode, int neg, int size, int bpos, int dlc, int nom, int msk, unsigned char *pat)
{
	int				i;
	EXTERNUL_IO		*act;
	CAN_DATA_BYTE	*cmk;
	
	i = ext_list_count;
	if(i >= 0 && i < ECU_EXT_MAX)
	{
		ext_list_count++;				//	�����X�V
		can_to_exio[id] = i;			//	�t�����}�b�v�ݒ�
		act = &ext_list[i];				//	�o�^��|�C���^
		act->SID = id;					//	�t���[��ID�ԍ�
__break__
		act->PORT.BIT.MODE = mode;		//	I/O�������[�h
		act->PORT.BIT.NEG = neg;		//	�f�[�^���]�w��
		act->PORT.BIT.SIZE = size;		//	�A�N�Z�X�T�C�Y
		act->PORT.BIT.BPOS = bpos;		//	�o�C�g�ʒu
		act->PORT.BIT.DLC = dlc;		//	�f�[�^�o�C�g��
		act->PORT.BIT.MSK = msk;		//	�}�X�N�p�^�[��
		act->PORT.BIT.NOM = nom;		//	�K�p�|�[�g�ԍ�
		if(pat != 0)
		{
			memcpy(act->PAT, pat, 24);		//	�p�^�[���f�[�^
		}
		cmk = &can_random_mask.ID[id];
		//	�}�X�N����
		switch(mode)
		{
		default:	//	�}�X�N����
			cmk->LONG[0] = -1;
			cmk->LONG[1] = -1;
			break;
		case 0:		//	�r�b�g����
			cmk->BYTE[bpos] = msk;
			break;
		case 1:		//	�o�C�g����
			cmk->BYTE[bpos] = 0xFF;
			break;
		case 2:		//	���[�h����
			cmk->BYTE[bpos] = 0xFF;
			cmk->BYTE[bpos + 1] = 0xFF;
			break;
		case 3:		//	�����O���[�h����
			cmk->BYTE[bpos] = 0xFF;
			cmk->BYTE[bpos + 1] = 0xFF;
			cmk->BYTE[bpos + 2] = 0xFF;
			cmk->BYTE[bpos + 3] = 0xFF;
			break;
		}
		//	DS����ID�̃��X�g�ԍ�
		if(id == DS_X_POWERTRAIN_ID)
		{	//	DS��p���[��
			ds_xross_pt_index = i;	//	�ێ�
			rout_map.ID[id].BYTE = 0x11;
		}
		else
		{	//	���[�e�B���O�}�b�v�ݒ�
			rout_map.ID[id].BYTE = (mode < 4) ? 0x01 : 0x10;	//	���o�͕����ő���M�t���O���`(0..3:���͒l��CAN���M / 4..7:�o�͒l��CAN��M)
		}
		return i;
	}
	return -1;
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : �f�[�^�o�b�t�@���ڐݒ�
//  
//  ����   : int id		�t���[��ID�ԍ�		0�`2047(000�`7FF)
//           int dlc	�f�[�^��
//           unsigned char	*dat	�f�[�^�o�b�t�@�ւ̃|�C���^
//  
//  ����   : CAN�f�[�^�o�b�t�@�֒l��ݒ肷��
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void set_frame_data(int id, int dlc, unsigned char *dat)
{
	int				i;
	CAN_DATA_BYTE	*act;
	
	act = &can_buf.ID[id];	//	�f�[�^�o�b�t�@�I��
//	can_chainge[id]++;		//	��ԕω��t���O
	memcpy(act, dat, dlc);	//	�R�s�[
//	can_used_mark[id]++;
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : ECU����������
//  
//  ����   : ����
//  
//  ����   : ECU�f�[�^�̈�̏������Ǝ������b�Z�[�W�̓o�^
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void ecu_init(void)
{
	int						i, j, addr;
	POINTER_MULTI_ACCESS	s, d;
	EXTERNUL_IO				*act;
	CAN_DATA_BYTE			*cmk;
	
	Init_FlashData();

	//	�ϐ�������
	memset(&wait_tup, 0, sizeof(wait_tup));					//	�����E�C�x���g�҂�������
	memset(&send_msg, 0, sizeof(send_msg));					//	���b�Z�[�W�{�b�N�X���̑��M�҂��o�b�t�@������
	memset(&can_buf, 0, sizeof(can_buf));					//	CAN�f�[�^�o�b�t�@������
//	memset(&can_chainge, 0, sizeof(can_chainge));			//	CAN�f�[�^�ω��t���O������
	memset(&mbox_sel, 0, sizeof(mbox_sel));					//	���b�Z�[�W�{�b�N�X�͈͏�����
	memset(&exiosts, 0, sizeof(exiosts));					//	�O�����o�͏�ԏ�����
	memset(&exio_chg, 0, sizeof(exio_chg));					//	�O�����o�͏�ԏ�����
	memset(&can_random_mask, 0, sizeof(can_random_mask));	//	�����_���R�[�h�}�X�N������
	memset(&rxmb_buf, 0, sizeof(rxmb_buf));					//	��M�o�b�t�@
	memset(&conf_ecu, 0, sizeof(conf_ecu));					//	�C�x���g���X�g
	memset(ext_list, 0, sizeof(ext_list));					//	ECU���o�̓`�F�b�N���X�g������
	memset(can_to_exio, -1, sizeof(can_to_exio));			//	ECU���o�͕ϊ��e�[�u��������

	exio_chg_mark = 0;
	
//	memset(&can_used_mark, 0, sizeof(can_used_mark));
	
	ext_list_count = 0;										//	�`�F�b�N���X�g�����Z�b�g
	wait_tup.TOP = -1;
//	can_chainge_cnt = 0;
	for(i = 0; i < CAN_CH_MAX; i++)
	{
		mbox_sel.CH[i].MB1 = MBOX_POINT_1;	//	MBOX0 : ID=000�`MBOX_POINT_1
		mbox_sel.CH[i].MB2 = MBOX_POINT_2;	//	MBOX1 : ID=MBOX_POINT_1�`MBOX_POINT_2 , ����ȏ�� MBOX2
		for(j = 0; j < MESSAGE_BOXS; j++)
		{
			send_msg[i].BOX[j].TOP = -1;
		}
	}

	//	�}�b�v�擾
	addr = g_flash_BlockAddresses[BLOCK_DB0];
	//	�u�����N�`�F�b�N
	if((i = R_FlashDataAreaBlankCheck(addr, BLANK_CHECK_ENTIRE_BLOCK)) == FLASH_NOT_BLANK)
	{	//	�f�[�^�t���b�V���L��(8KB: 0x00100000 - 0x00101FFF)
		//	���[�e�B���O�}�b�v�ǂݏo��
		s.LONG = ADDRESS_OF_ROOTMAP;
		d.MAP = &rout_map;
		memcpy(d.UB, s.UB, sizeof(rout_map));			//	�}�b�v������
		//	�C�x���g���X�g�ǂݏo��
		s.LONG = ADDRESS_OF_CYCEVE;
		d.CYE = &conf_ecu.LIST[0];
		memcpy(d.UB, s.UB, sizeof(ECU_CYC_EVE) * MESSAGE_MAX);	//	�����E�C�x���g�E�����[�g�Ǘ���`������
		j = CAN_ID_MAX;
		for(i = 0; i < MESSAGE_MAX; i++)
		{
			if(conf_ecu.LIST[i].ID.LONG == 0)	//ID.BIT.ENB != 0)
			{	//	�I�[
				conf_ecu.WP = i;
				break;
			}
			if(conf_ecu.LIST[i].ID.BIT.SID < j)
			{
				j = conf_ecu.LIST[i].ID.BIT.SID;
				conf_ecu.TOP = i;
			}
			conf_ecu.CNT++;
		}
		//	I/O�ݒ�ǂݏo��
		s.LONG = ADDRESS_OF_IOLIST;
		d.EXL = ext_list;
		memcpy(d.UB, s.UB, sizeof(ext_list));			//	ECU���o�̓`�F�b�N���X�g������
		//	���X�g�L���o�^���`�F�b�N
		for(i = 0; i < ECU_EXT_MAX; i++, ext_list_count++)
		{
			if(ext_list[i].PORT.LONG == 0)	//ext_list[i].SID < 0 || ext_list[i].PORT.BIT.MODE == 0)
			{	//	�I�[
				break;
			}
			ext_list_count++;				//	�����X�V
			act = &ext_list[i];				//	�o�^��|�C���^
			can_to_exio[act->SID] = i;		//	�t�����}�b�v�ݒ�
			cmk = &can_random_mask.ID[act->SID];
			//	�}�X�N����
			switch(act->PORT.BIT.MODE)
			{
			default:	//	�}�X�N����
				cmk->LONG[0] = -1;
				cmk->LONG[1] = -1;
				break;
			case 0:		//	�r�b�g����
				cmk->BYTE[act->PORT.BIT.BPOS] = act->PORT.BIT.MSK;
				break;
			case 1:		//	�o�C�g����
				cmk->BYTE[act->PORT.BIT.BPOS] = 0xFF;
				break;
			case 2:		//	���[�h����
				cmk->BYTE[act->PORT.BIT.BPOS] = 0xFF;
				cmk->BYTE[act->PORT.BIT.BPOS + 1] = 0xFF;
				break;
			case 3:		//	�����O���[�h����
				cmk->BYTE[act->PORT.BIT.BPOS] = 0xFF;
				cmk->BYTE[act->PORT.BIT.BPOS + 1] = 0xFF;
				cmk->BYTE[act->PORT.BIT.BPOS + 2] = 0xFF;
				cmk->BYTE[act->PORT.BIT.BPOS + 3] = 0xFF;
				break;
			}
			//	DS����ID�̃��X�g�ԍ�
			if(act->SID == DS_X_POWERTRAIN_ID)
			{	//	DS��p���[��
				ds_xross_pt_index = i;	//	�ێ�
				rout_map.ID[act->SID].BYTE = 0x11;
			}
		//	else
		//	{	//	���[�e�B���O�}�b�v�ݒ�
		//		//	���o�͕����ő���M�t���O���`(0..3:���͒l��CAN���M / 4..7:�o�͒l��CAN��M)
		//		rout_map.ID[act->SID].BYTE = (act->PORT.BIT.MODE < 4) ? 0x01 : 0x10;
		//	}
		}
	}
	else
	if(i == FLASH_BLANK)
	{	//	�ۑ����Ȃ�
		//	�f�t�H���g�ݒ�Ăяo��
		defset_rootmap();		//	�}�b�v�����l
		defset_confecu();		//	�����E�C�x���g�����l
	//	defset_extlist();		//	�O�����o�͒�`�����l
		defset_extlist_ex();	//	�ʐM�o�R�O�����o�͒�`�����l
	}
	//	�t���[���f�[�^�����l
	defset_framedat();
	//	����C�x���g�o�^
	for(i = 0; i < conf_ecu.CNT; i++)
	{
		if(conf_ecu.LIST[i].ID.BIT.ENB != 0)
		{	//	�����E�C�x���g
			j = conf_ecu.LIST[i].ID.BIT.SID;
			can_id_event(j, 0);
		}
	}
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : CAN�|�[�g�ݒ�
//  
//  ����   : ����
//  
//  ����   : CAN�Ŏg�p����|�[�g�����̏�����
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void can_pin_init(void)
{
	SYSTEM.PRCR.WORD = 0xA503;	//	�|�[�g�ݒ苖��
	MPC.PWPR.BIT.B0WI = 0;		//	
	MPC.PWPR.BIT.PFSWE = 1;		//	

#ifdef	__LFY_RX63N__
	// CAN1�̒[�q�ݒ�
	PORT5.PMR.BIT.B5 = 0;		//	���Ӌ@�\
	PORT5.PMR.BIT.B4 = 0;		//	���Ӌ@�\
	PORT5.PODR.BIT.B5 = 1;		//	P55 -- CRX1
	PORT5.PODR.BIT.B4 = 1;		//	P54 -- CTX1
	PORT5.PDR.BIT.B5 = 0;		//	P55 -- CRX1
	PORT5.PDR.BIT.B4 = 1;		//	P54 -- CTX1
	MPC.P55PFS.BYTE = 0x10;		//	CRX1
	MPC.P54PFS.BYTE = 0x10;		//	CTX1
	PORT5.PMR.BIT.B5 = 1;		//	���Ӌ@�\
	PORT5.PMR.BIT.B4 = 1;		//	���Ӌ@�\
#else
	//	CAN�h���C�oS�[�q����|�[�g
	PORT6.PODR.BYTE = 0xF0;		//	�|�[�g������
	PORT6.PDR.BIT.B6 = 1;
	PORT6.PDR.BIT.B7 = 0;
	PORT6.PDR.BIT.B0 = 1;		//	P60 -- Port-out CAN0S
	PORT6.PDR.BIT.B1 = 1;		//	P61 -- Port-out CAN1S
	PORT6.PDR.BIT.B2 = 1;		//	P62 -- Port-out CAN2S
	PORT6.PDR.BIT.B3 = 1;		//	P63 -- Port-out CAN3S
	PORT6.PDR.BIT.B5 = 1;		//	P65 -- Port-out LED
	
	//	ID�`�F�b�NLED
	PORTE.PMR.BIT.B0 = 0;		//	PORT
	PORTE.PODR.BIT.B0 = 1;		//	PE0
	PORTE.PDR.BIT.B0 = 1;		//	LED output


	// CAN0�̒[�q�ݒ�
	PORT3.PMR.BIT.B3 = 0;		//	���Ӌ@�\
	PORT3.PMR.BIT.B2 = 0;		//	���Ӌ@�\
	PORT3.PODR.BIT.B3 = 1;		//	P32 -- CRX0
	PORT3.PODR.BIT.B2 = 1;		//	P32 -- CTX0
	PORT3.PDR.BIT.B3 = 0;		//	P33 -- CRX0
	PORT3.PDR.BIT.B2 = 1;		//	P32 -- CTX0
	MPC.P33PFS.BYTE = 0x10;		//	CRX0
	MPC.P32PFS.BYTE = 0x10;		//	CTX0
	PORT3.PMR.BIT.B3 = 1;		//	���Ӌ@�\
	PORT3.PMR.BIT.B2 = 1;		//	���Ӌ@�\

	// CAN1�̒[�q�ݒ�
	PORT5.PMR.BIT.B5 = 0;		//	���Ӌ@�\
	PORT5.PMR.BIT.B4 = 0;		//	���Ӌ@�\
	PORT5.PODR.BIT.B5 = 1;		//	P55 -- CRX1
	PORT5.PODR.BIT.B4 = 1;		//	P54 -- CTX1
	PORT5.PDR.BIT.B5 = 0;		//	P55 -- CRX1
	PORT5.PDR.BIT.B4 = 1;		//	P54 -- CTX1
	MPC.P55PFS.BYTE = 0x10;		//	CRX1
	MPC.P54PFS.BYTE = 0x10;		//	CTX1
	PORT5.PMR.BIT.B5 = 1;		//	���Ӌ@�\
	PORT5.PMR.BIT.B4 = 1;		//	���Ӌ@�\

	// CAN2�̒[�q�ݒ�
	PORT6.PMR.BIT.B7 = 0;		//	���Ӌ@�\
	PORT6.PMR.BIT.B6 = 0;		//	���Ӌ@�\
	PORT6.PODR.BIT.B7 = 1;		//	P67 -- CRX2
	PORT6.PODR.BIT.B6 = 1;		//	P66 -- CTX2
	PORT6.PDR.BIT.B7 = 0;		//	P67 -- CRX2
	PORT6.PDR.BIT.B6 = 1;		//	P66 -- CTX2
	MPC.P67PFS.BYTE = 0x10;		//	CRX2
	MPC.P66PFS.BYTE = 0x10;		//	CTX2
	PORT6.PMR.BIT.B7 = 1;		//	���Ӌ@�\
	PORT6.PMR.BIT.B6 = 1;		//	���Ӌ@�\
#endif
/*
	// CAN3�̒[�q�ݒ�(RSPI2�o�R)
	PORTD.PDR.BIT.B2 = 0;		//	PD2 <- MISOC
	PORTD.PDR.BIT.B1 = 1;		//	PD1 -> MOSIC
	PORTD.PDR.BIT.B3 = 1;		//	PD3 -> RSPCKC
	PORTD.PDR.BIT.B4 = 1;		//	PD4 -> SSLC0
	PORTD.PDR.BIT.B0 = 0;		//	PD0 <- CINT
	PORTD.PDR.BIT.B6 = 0;		//	PD6 <- CRX0BF
	PORTD.PDR.BIT.B7 = 0;		//	PD7 <- CRX1BF
	PORT9.PDR.BIT.B0 = 1;		//	P90 -> CTX0RTS
	PORT9.PDR.BIT.B1 = 1;		//	P91 -> CTX1RTS
	PORT9.PDR.BIT.B2 = 1;		//	P92 -> CTX2RTS
	PORT9.PDR.BIT.B3 = 1;		//	P93 -> CRSET
	PORT0.PDR.BIT.B7 = 0;		//	P07 <- CSOF
	PORT9.PODR.BYTE = 0x00;		//	�|�[�g������

	PORTD.PMR.BIT.B2 = 1;		//	���Ӌ@�\	MISOC
	PORTD.PMR.BIT.B1 = 1;		//	���Ӌ@�\	MOSIC
	PORTD.PMR.BIT.B3 = 1;		//	���Ӌ@�\	RSPCKC
	PORTD.PMR.BIT.B4 = 1;		//	���Ӌ@�\	SSLC0
	PORTD.PMR.BIT.B0 = 1;		//	���Ӌ@�\	CINT
	PORTD.PMR.BIT.B6 = 1;		//	���Ӌ@�\	CRX0BF
	PORTD.PMR.BIT.B7 = 1;		//	���Ӌ@�\	CRX1BF

	MPC.PD2PFS.BYTE = 0x0D;		//	MISOC		SO
	MPC.PD1PFS.BYTE = 0x0D;		//	MOSIC		SI
	MPC.PD3PFS.BYTE = 0x0D;		//	RSPCKC		SCK
	MPC.PD4PFS.BYTE = 0x0D;		//	SSLC0		/CS
	MPC.PD0PFS.BYTE = 0x40;		//	IRQ0		/INT
	MPC.PD6PFS.BYTE = 0x40;		//	IRQ6		/RX0BF
	MPC.PD7PFS.BYTE = 0x40;		//	IRQ7		/RX1BF
*/
	MPC.PWPR.BIT.PFSWE = 0;		//	
	MPC.PWPR.BIT.B0WI = 1;		//	
	SYSTEM.PRCR.WORD = 0xA500;	//	�|�[�g�ݒ�֎~
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : CAN���W���[�����W�X�^�ݒ�
//  
//  ����   : int ch		�������`�����l��
//           int bps	�ʐM���x(bps)	���ԗp:500kbps
//  
//  ����   : CAN���W���[���^�]�����̏�����
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void can_init(int ch, int bps)
{
	int			i, j;
	int			x, tbit, tseg1, tseg2, sjw, brp, fc;
	uint32_t	lwk;
    volatile struct st_can __evenaccess * can_block_p = CAN_CHANNELS[ch];

	if(ch < 3)
	{	//	CPU����CAN0,1,2�̂ݗL��
		//	CAN�|�[�g������
		logging("CAN%d Init\r",ch);
		lwk = R_CAN_Create(ch);			// CAN���W���[��������
		if ( lwk != R_CAN_OK )
		{
			logging("CAN_Create = %08lX\r",lwk);
		}
		else
		{
			logging("CAN_Create OK\r");
		}
		//	���Z�b�g���{
		lwk = R_CAN_Control(ch, RESET_CANMODE); //RESET���[�h�ݒ�
		if ( lwk != R_CAN_OK )
		{
			logging("CAN_Control = %08lX\r",lwk);
		}
		else
		{
			logging("R_CAN_Control RESET_CANMODE\r");
		}
		//	�ݒ胂�[�h
		lwk = R_CAN_Control(ch, HALT_CANMODE); //HALT���[�h�ݒ�
		if ( lwk != R_CAN_OK )
		{
			logging("CAN_Control = %08lX\r",lwk);
		}
		else
		{
			logging("R_CAN_Control HALT_CANMODE\r");
		}
		
		//	�ʐM���x�ݒ�
		//	BRP = 1�`1024(0�`1023)
		//	SS = 1
		//	TSEG1 = 4�`16(3�`15) : TSEG2 = 2�`8(1�`7)
		//	SJW = 1�`4(0�`3)
		//	��TSEG1 > TSEG2 �� SJW
		//	�ŏ�	4 : 2 : 1	TCANBIT = SS + TSEG1 + TSEG2 = 1 + 4 + 2 = 5 �` 1 + 16 + 8 = 25
		//	
		//	PCLK = 48,000,000 Hz
		//	BPS  =    500,000 bps
		//
		//	BRP = PCLK / BPS / TBIT		��TBIT = 16TQ (5�`25TQ) �ɂȂ�悤�ɋ��߂�
		//		= 48000000 / 500000 = 96 / 16 = 6
		//		= 6
		//
		//	tBit= PCLK / BPS / BRP = 96 / 6 = 16Tq
		//
		//	TSEG2 = (tBit - SS) / 4 = (16 - 1) / 4 = 3.75
		//		  = 3
		//	TSEG1 = tBit - SS - TSEG2 = 16 - 1 - 3 = 12
		//		  = 12
		//	SJW	  = TSEG2 / 2 = 3 / 2 = 1.5
		//		  = 1
		//
		for(x = 25; x > 4; x--)
		{
			if((48000000 % x) != 0) continue;
			fc = 48000000 / x;
			if((fc % bps) > (bps * 17 / 1000)) continue;	//	�덷1.7%����
			brp = 48000000 / bps / x;		//	1025 > brp > 0
			if(brp < 1) continue;			//	�͈͊O
			if(brp > 1024) continue;		//	�͈͊O
			tbit = 48000000 / bps / brp;	//	26 > tbit > 4
			if(tbit != x) continue;			//	�t�Z�s��v
			if(tbit < 5) continue;			//	�͈͊O
			if(tbit > 25) continue;			//	�͈͊O
			tseg2 = tbit / 3;				//	9 > tseg2 > 1
			if(tseg2 < 2) continue;			//	�͈͊O
			if(tseg2 > 8) continue;			//	�͈͊O
			tseg1 = tbit - 1 - tseg2;		//	17 > tseg1 > 3
			if(tseg1 <= tseg2) continue;	//	�͈͊O
			if(tseg1 > 16) continue;		//	�͈͊O
			sjw = (tseg2 + 1) / 2;			//	5 > sjw > 0
			if(sjw < 1) continue;			//	�͈͊O
			if(sjw > 4) continue;			//	�͈͊O
			break;
		}
		if(x == 4)
		{	//	���s�����̂�500kbps�ɏ���������
			logging("can_init: invalid parameter (%ld)\r", bps);
			brp = 6;
			tbit = 16;
			tseg1 = 10;
			tseg2 = 5;
			sjw = 4;
		}
		can_block_p->BCR.BIT.CCLKS = 0;		//	PCLK(48MHz)
		can_block_p->BCR.BIT.BRP = brp - 1;
		can_block_p->BCR.BIT.TSEG1 = tseg1 - 1;
		can_block_p->BCR.BIT.TSEG2 = tseg2 - 1;
		can_block_p->BCR.BIT.SJW = sjw - 1;
		logging("BPS=%d SS=1 BRP=%d TSEG1=%d TSEG2=%d SJW=%d TBIT=%d\r", bps, brp, tseg1, tseg2, sjw, tbit);
		
		//	���[���{�b�N�X�f�[�^������
		for(i = 0 ;i < 32 ;i++)
		{
			can_block_p->MKR[i].LONG = 0;
			can_block_p->MB[i].ID.LONG = 0;
		}
		//	�}�X�N�E�t�B���^����
		can_block_p->MKIVLR.LONG = 0;	//0x0000FFFF;
		
		//	���[���{�b�N�X���䏉����
		for(i = 0 ;i < 32 ;i++)
		{
			while(can_block_p->MCTL[i].BYTE != 0) can_block_p->MCTL[i].BYTE = 0;
		}
		//	�^�]�J�n
		can_block_p->MIER.LONG = 0xFFFFFFFF;		//	���荞�݋���
		lwk = R_CAN_Control(ch, OPERATE_CANMODE);	//	OPERATE���[�h�ݒ�
		if ( lwk != R_CAN_OK ) {
			logging("R_CAN_Control = %08lX\r", lwk);
		}
		else
		{
			logging("R_CAN_Control OPERATE_CANMODE\r");
		}
		//	��M����
		for(i = 16 ;i < 32 ;i++)
		{
			can_block_p->MCTL[i].BYTE = 0x40;	//	MB16�`32�͎�M��p
		}
	}
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : ECU�^�C�}�[�J�E���^�X�V����
//  
//  ����   : ����
//  
//  ����   : 1ms�^�C�}�[�ɂ��Ăяo�����
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void ecu_timeup(void)
{
	timer_count++;
	status_timer++;
	after_call(0, -1, ecu_timeup);	//	�����^�C�}�[�Ăяo��
}

void ecu_rxmb_proc(void)
{
	int	ch;
	for(ch = 0; ch < 3; ch++)
	{
		while(rxmb_buf[ch].WP != rxmb_buf[ch].RP)
		{
			can_recv_frame(ch, (void *)&rxmb_buf[ch].MB[rxmb_buf[ch].RP++]);	//	��M�f�[�^�擾
			rxmb_buf[ch].RP &= (RX_MB_BUF_MAX-1);
		}
	}
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : ECU�^�]�V�[�P���X
//  
//  ����   : ����
//  
//  ����   : main()�֐�����Ăяo���BCAN�t���[���̑��o�Ǘ��A�荏�����A�C�x���g�������s���B
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void ecu_job(void)
{
	int				i, ch, t;

	if(job > 1)
	{
		ecu_rxmb_proc();	//	��M�𖈉�`�F�b�N
		send_mbox_frame();	//	���M�𖈉�`�F�b�N
	}

	switch(job)
	{
		case 0:	//	����������
			job++;
			logging("Init ECU\r");
			ecu_init();			//	ECU�o�b�t�@������
			can_pin_init();		//	CPU�s���ݒ�
			timer_count = 0;
			status_timer = 0;
			ecu_timeup();
		case 1:
			if(timer_count > 0)
			{	//	�^�C�}�[�X�V�L��
				led += timer_count;
				timer_count = 0;	//	�^�C���J�E���g������
				if(led >= 500)
				{
					led -= 500;
					PORT6.PODR.BYTE ^= 0x20;	//	LED���]
					logging("Start ECU\r");
					job++;
#ifdef	__LFY_RX63N__
					i = CAN_TEST_LFY_CH;
#else
					for(i = 0; i < CAN_CH_MAX; i++)
#endif
					{
						can_init(i, 500000);	//	CAN�|�[�g������
					}
				}
			}
			break;
		//------------------------------------------
		//	�ȉ��͌J��Ԃ�����
		//------------------------------------------
		case 2:	//	I/O�A�b�v�f�[�g
			job++;
		//	extern_io_update();		//	ECU�{�̂�I/O
			extern_io_update_ex();	//	RS-232C�ʐM�o�R��I/O
			break;
		case 3:	//	1ms�T�C�N������
			job = 5;
			if(timer_count > 0)
			{	//	�^�C�}�[�X�V�L��
			//	logging("Timer ECU\r");
				t = timer_count;
				timer_count = 0;	//	�^�C���J�E���g������
				led += t;
				if(led >= 500)
				{
					led -= 500;
					PORT6.PODR.BYTE ^= 0x20;	//	LED���]
				}
				can_timer_send(t);	//	�^�C���A�b�v����
			}
			break;
		case 4:	//	CAN���M����
		//	job = 6;
			job++;
		//	send_mbox_frame();
			break;
		case 5:	//	CAN��M����
			job++;
#ifndef	CAN_RX_INT_ENB
			CAN0.MSMR.BYTE = 0;	//	��MMB��SENTDATA����
			while(CAN0.MSSR.BIT.SEST == 0)
			{	//	���ʗL��
				i = (int)CAN0.MSSR.BIT.MBNST;
				if(!CAN0.MCTL[i].BIT.RX.INVALDATA)
				{
					if(CAN0.MCTL[i].BIT.RX.NEWDATA)
					{
						can_recv_frame(0, (void *)&CAN0.MB[i]);	//	��M�f�[�^�擾
					}
					CAN0.MCTL[i].BYTE = 0x40;	//	��M�ĊJ
				}
			}
			CAN1.MSMR.BYTE = 0;	//	��MMB��SENTDATA����
			while(CAN1.MSSR.BIT.SEST == 0)
			{	//	���ʗL��
				i = (int)CAN1.MSSR.BIT.MBNST;
				if(!CAN1.MCTL[i].BIT.RX.INVALDATA)
				{
					if(CAN1.MCTL[i].BIT.RX.NEWDATA)
					{
						can_recv_frame(1, (void *)&CAN1.MB[i]);	//	��M�f�[�^�擾
					}
					CAN1.MCTL[i].BYTE = 0x40;	//	��M�ĊJ
				}
			}
			CAN2.MSMR.BYTE = 0;	//	��MMB��SENTDATA����
			while(CAN2.MSSR.BIT.SEST == 0)
			{	//	���ʗL��
				i = (int)CAN2.MSSR.BIT.MBNST;
				if(!CAN2.MCTL[i].BIT.RX.INVALDATA)
				{
					if(CAN2.MCTL[i].BIT.RX.NEWDATA)
					{
						can_recv_frame(2, (void *)&CAN2.MB[i]);	//	��M�f�[�^�擾
					}
					CAN2.MCTL[i].BYTE = 0x40;	//	��M�ĊJ
				}
			}
			break;
#endif
		case 6:	//	RS-232C���M����
			job = 2;
#if	1
			if(stat_update_id < EX_IO_MAX)
			{	//	�p���ʒm���M
				if(sci_txbytes(stat_comm) == 0)
				{	//	���M�o�b�t�@��
					int		j, k;
					int		r;
					char	buf[128];
					r = 0;
					//	1�d����80�������x�Ƃ��đ��o����
					for(; r < 80 && stat_update_id < EX_IO_MAX; stat_update_id++)
					{	//	1�s�����M����
						if(exio_chg[stat_update_id] != 0)
						{	//	�f�[�^�X�VID
							exio_chg[stat_update_id] = 0;
							if(r == 0)
							{	//	�s���́uEXU�v�Ŏn�܂�
								buf[r++] = 'E';
								buf[r++] = 'X';
								buf[r++] = 'U';
							}
							//	I/O-ID�R�[�h2��
							k = stat_update_id;
							buf[r++] = ' ';
							buf[r++] = HEX_CHAR[((k >> 4) & 15)];
							buf[r++] = HEX_CHAR[(k & 15)];
							//	�X�V�f�[�^4�o�C�g
							k = exiosts.DATA[stat_update_id].INTE;
							j = 8;
							if(((k >> 28) & 15) == 0)
							{
								j--;
								if(((k >> 24) & 15) == 0)
								{
									j--;
									if(((k >> 20) & 15) == 0)
									{
										j--;
										if(((k >> 16) & 15) == 0)
										{
											j--;
											if(((k >> 12) & 15) == 0)
											{
												j--;
												if(((k >> 8) & 15) == 0)
												{
													j--;
													if(((k >> 4) & 15) == 0)
													{
														j--;
													}
												}
											}
										}
									}
								}
							}
							switch(j)
							{
							case 8:
								buf[r++] = HEX_CHAR[((k >> 28) & 15)];
							case 7:
								buf[r++] = HEX_CHAR[((k >> 24) & 15)];
							case 6:
								buf[r++] = HEX_CHAR[((k >> 20) & 15)];
							case 5:
								buf[r++] = HEX_CHAR[((k >> 16) & 15)];
							case 4:
								buf[r++] = HEX_CHAR[((k >> 12) & 15)];
							case 3:
								buf[r++] = HEX_CHAR[((k >> 8) & 15)];
							case 2:
								buf[r++] = HEX_CHAR[((k >> 4) & 15)];
							case 1:
								buf[r++] = HEX_CHAR[(k & 15)];
							case 0:
								break;
							}
						}
					}
					if(r > 0)
					{
						buf[r++] = '\r';
						buf[r] = 0;
						sci_puts(stat_comm, buf);	//	���M���s
					}
				}
			}
			else
			if(status_timer >= 500)
			{	//	500ms�����ŋ����S���M
				status_timer = 0;	//	�^�C�}�[�N���A
			//	if(sci_txbytes(stat_comm) == 0 && exio_chg_mark > 0)
			//	{	//	���M�o�b�t�@�󂫁A��ԕω��L��
					stat_update_id = 0;		//	���M�J�nID�Z�b�g
					exio_chg_mark = 0;	//	�ω��t���O�N���A
			//	}
				for(i = 0; i < ext_list_count; i++) exio_chg[(ext_list[i].PORT.BIT.NOM)] = 1;
			}
			else
			if(sci_txbytes(stat_comm) == 0 && exio_chg_mark > 0)
			{	//	���M�o�b�t�@�󂫁A��ԕω��L��Ōʑ��M
				status_timer = 0;	//	�^�C�}�[�N���A
				stat_update_id = 0;		//	���M�J�nID�Z�b�g
				exio_chg_mark = 0;	//	�ω��t���O�N���A
			}
#else
			if(stat_update_id < CAN_ID_MAX)
			{	//	�p���ʒm���M
				if(sci_txbytes(stat_comm) == 0)
				{	//	���M�o�b�t�@��
					int		j, k;
					int		r;
					char	buf[128];
					r = 0;
					//	1�d����80�������x�Ƃ��đ��o����
					for(; r < 80 && stat_update_id < EX_IO_MAX; stat_update_id++)
					{	//	1�s�����M����
						if(can_chainge[stat_update_id] != 0)
						{	//	�f�[�^�X�VID
							can_chainge[stat_update_id] = 0;
							if(r == 0)
							{	//	�s���́uECU�v�Ŏn�܂�
								buf[r++] = 'E';
								buf[r++] = 'C';
								buf[r++] = 'U';
							}
							//	ID�R�[�h3��
							k = stat_update_id;
							buf[r++] = ' ';
							buf[r++] = HEX_CHAR[((k >> 8) & 15)];
							buf[r++] = HEX_CHAR[((k >> 4) & 15)];
							buf[r++] = HEX_CHAR[(k & 15)];
							//	�t���[���f�[�^8�o�C�g
							for(j = 0; j < 8; j++)
							{	//	�f�[�^
								k = (int)can_buf.ID[stat_update_id].BYTE[j];
								buf[r++] = HEX_CHAR[((k >> 4) & 15)];
								buf[r++] = HEX_CHAR[(k & 15)];
							}
						}
					}
					if(r > 0)
					{
						buf[r++] = '\r';
						buf[r] = 0;
						sci_puts(stat_comm, buf);	//	���M���s
					}
				}
			}
			else
			if(status_timer >= 20)
			{	//	50Hz(20ms)����
				status_timer = 0;	//	�^�C�}�[�N���A
				if(sci_txbytes(stat_comm) == 0 && can_chainge_cnt > 0)
				{	//	���M�o�b�t�@�󂫁A��ԕω��L��
					stat_update_id = 0;		//	���M�J�nID�Z�b�g
					can_chainge_cnt = 0;	//	�ω��t���O�N���A
				}
			}
#endif
			break;
		default:
			job = 2;
			break;
	}
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : ECU�^�]�p�����[�^�ʒm
//  
//  ����   : ����
//  
//  ����   : ���݂̃p�����[�^�l��COM1�ɑ��M����
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void SendPC(char *msg);

void ecu_status(char *cmd)
{
	int		i, j;
	int		ch, mb, id;
	char	tx[39], c;

	if(cmd == 0 || *cmd == 0) return;
	while(*cmd == ' ') cmd++;
	switch(*cmd++)
	{
	case 'L':	//	conf_ecu�ϐ��\��
		logging("conf_ecu WP=%d TOP=%d CNT=%d\r", conf_ecu.WP, conf_ecu.TOP, conf_ecu.CNT);
		for(i = 0; i < MESSAGE_MAX; i++)
		{
			if(conf_ecu.LIST[i].ID.BIT.ENB != 0)
			{
				logging("No.%d RTR=%d SID=%03X DLC=%d ENB=%d REP=%d NXT=%d TIM=%d CNT=%d DATA=%02X %02X %02X %02X %02X %02X %02X %02X\r",
					i, 
					(int)conf_ecu.LIST[i].ID.BIT.RTR,
					(int)conf_ecu.LIST[i].ID.BIT.SID,
					(int)conf_ecu.LIST[i].ID.BIT.DLC,
					(int)conf_ecu.LIST[i].ID.BIT.ENB,
					(int)conf_ecu.LIST[i].ID.BIT.REP,
					(int)conf_ecu.LIST[i].ID.BIT.NXT,
					(int)conf_ecu.LIST[i].TIMER.WORD.TIME,
					(int)conf_ecu.LIST[i].TIMER.WORD.CNT,
					(int)can_buf.ID[i].BYTE[0], (int)can_buf.ID[i].BYTE[1], (int)can_buf.ID[i].BYTE[2], (int)can_buf.ID[i].BYTE[3],
					(int)can_buf.ID[i].BYTE[4], (int)can_buf.ID[i].BYTE[5], (int)can_buf.ID[i].BYTE[6], (int)can_buf.ID[i].BYTE[7]
					);
			}
		}
		break;
	case 'W':	//	wait_tup�ϐ��\��
		logging("wait_tup WP=%d TOP=%d CNT=%d\r", wait_tup.WP, wait_tup.TOP, wait_tup.CNT);
		for(i = 0; i < MESSAGE_MAX; i++)
		{
			if(wait_tup.LIST[i].ID.BIT.ENB != 0)
			{
				logging("No.%d RTR=%d SID=%03X DLC=%d ENB=%d REP=%d NXT=%d TIM=%d CNT=%d DATA=%02X %02X %02X %02X %02X %02X %02X %02X\r",
					i, 
					(int)wait_tup.LIST[i].ID.BIT.RTR,
					(int)wait_tup.LIST[i].ID.BIT.SID,
					(int)wait_tup.LIST[i].ID.BIT.DLC,
					(int)wait_tup.LIST[i].ID.BIT.ENB,
					(int)wait_tup.LIST[i].ID.BIT.REP,
					(int)wait_tup.LIST[i].ID.BIT.NXT,
					(int)conf_ecu.LIST[i].TIMER.WORD.TIME,
					(int)conf_ecu.LIST[i].TIMER.WORD.CNT,
					(int)can_buf.ID[i].BYTE[0], (int)can_buf.ID[i].BYTE[1], (int)can_buf.ID[i].BYTE[2], (int)can_buf.ID[i].BYTE[3],
					(int)can_buf.ID[i].BYTE[4], (int)can_buf.ID[i].BYTE[5], (int)can_buf.ID[i].BYTE[6], (int)can_buf.ID[i].BYTE[7]
					);
			}
		}
		break;
		
	case 'S':	//	send_msg�ϐ��\��
		for(ch = 0; ch < CAN_CH_MAX; ch++)
		{
			for(mb = 0; mb < MESSAGE_BOXS; mb++)
			{
				logging("send_msg[%d].BOX[%d] WP=%d TOP=%d CNT=%d\r", ch, mb, send_msg[ch].BOX[mb].WP, send_msg[ch].BOX[mb].TOP, send_msg[ch].BOX[mb].CNT);
				for(i = 0; i < MESSAGE_MAX; i++)
				{
					if(send_msg[ch].BOX[mb].MSG[i].ID.BIT.ENB != 0)
					{
						logging("No.%d RTR=%d SID=%03X DLC=%d ENB=%d NXT=%d DATA=%02X %02X %02X %02X %02X %02X %02X %02X\r",
							i,
							(int)send_msg[ch].BOX[mb].MSG[i].ID.BIT.RTR,
							(int)send_msg[ch].BOX[mb].MSG[i].ID.BIT.SID,
							(int)send_msg[ch].BOX[mb].MSG[i].ID.BIT.DLC,
							(int)send_msg[ch].BOX[mb].MSG[i].ID.BIT.ENB,
							(int)send_msg[ch].BOX[mb].MSG[i].ID.BIT.NXT,
							(int)send_msg[ch].BOX[mb].MSG[i].FD.BYTE[0], (int)send_msg[ch].BOX[mb].MSG[i].FD.BYTE[1], 
							(int)send_msg[ch].BOX[mb].MSG[i].FD.BYTE[2], (int)send_msg[ch].BOX[mb].MSG[i].FD.BYTE[3], 
							(int)send_msg[ch].BOX[mb].MSG[i].FD.BYTE[4], (int)send_msg[ch].BOX[mb].MSG[i].FD.BYTE[5], 
							(int)send_msg[ch].BOX[mb].MSG[i].FD.BYTE[6], (int)send_msg[ch].BOX[mb].MSG[i].FD.BYTE[7]
							);
					}
				}
			}
		}
		break;
	}
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : ECU�t���[���f�[�^�擾
//  
//  ����   : char *cmd	�R�}���h������
//  
//  ����   : ���݂̎w��ID�t���[���f�[�^��ԐM����
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void ecu_get_command(char *cmd)
{
	int		i, j;
	int		id;
	char	tx[39], c;

	if(cmd == 0 || *cmd == 0) return;
	while(*cmd == ' ') cmd++;
	if(*cmd < '0' || *cmd > '7') return;
	SendPC("ECU ");
	while(*cmd >= '0' && *cmd <= '7')
	{
		id = 0;
		for(i = 0; i < 4; i++)
		{
			c = *cmd;
			if(c >= 'a' && c <= 'f') c -= 0x27;
			if(c >= 'A' && c <= 'F') c -= 7;
			if(c < 0x30 || c > 0x3F) break;
			id = (id << 4) | (int)(c & 0x0F);
			cmd++;
		}
		if(i == 3 && id < CAN_ID_MAX)
		{	//	ID����
			j = 0;
			tx[j++] = HEX_CHAR[(id >> 8) & 0x0F];
			tx[j++] = HEX_CHAR[(id >> 4) & 0x0F];
			tx[j++] = HEX_CHAR[id & 0x0F];
			for(i = 0; i < 8; i++)
			{
				c = can_buf.ID[id].BYTE[i];
				tx[j++] = HEX_CHAR[(c >> 4) & 0x0F];
				tx[j++] = HEX_CHAR[c & 0x0F];
			}
			if(*cmd >= ' ') tx[j++] = ' ';
			tx[j++] = 0;
			SendPC(tx);
		}
		while(*cmd == ' ') cmd++;
	}
	SendPC("\r");
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : ECU�t���[���f�[�^��������
//  
//  ����   : char *cmg	�R�}���h������
//  
//  ����   : �w��ID�̃t���[���f�[�^�������ւ���
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void ecu_set_command(char *cmd)
{
	int				i, j, f;
	int				id, tp;
	int				xid[32];
	int				xwp = 0;
	char			tx[39], c;
	unsigned char	d;

	if(cmd == 0 || *cmd == 0) return;
	while(*cmd == ' ') cmd++;
	if(*cmd < '0' || *cmd > '7') return;
	//	������������
	tp = 0;	//	�x�����ԏ�����
	while(*cmd >= '0' && *cmd <= '7')
	{
		//	�Ώ�ID�擾
		id = 0;
		//	16�i�����񂩂�o�C�i���f�[�^16�i3��(0x000�`FFF)�擾
		for(i = 0; i < 3; i++)
		{
			c = *cmd;
			if(c >= 'a' && c <= 'f') c -= 0x27;
			if(c >= 'A' && c <= 'F') c -= 7;
			if(c < 0x30 || c > 0x3F) break;
			id = (id << 4) | (int)(c & 0x0F);
			cmd++;
		}
		//	ID�`�F�b�N
		if(i == 3 && id < CAN_ID_MAX)
		{	//	ID����A�f�[�^����
			f = 0;
			//	16�i�����񂩂�o�C�i���f�[�^�擾
			for(i = 0; i < 8; i++)
			{
				c = *cmd;
				if(c >= 'a' && c <= 'f') c -= 0x27;
				if(c >= 'A' && c <= 'F') c -= 7;
				if(c < 0x30 || c > 0x3F) break;
				d = (unsigned char)(c & 0x0F);
				cmd++;
				c = *cmd;
				if(c >= 'a' && c <= 'f') c -= 0x27;
				if(c >= 'A' && c <= 'F') c -= 7;
				if(c < 0x30 || c > 0x3F) break;
				d = (d << 4) | (unsigned char)(c & 0x0F);
				cmd++;
				can_buf.ID[id].BYTE[i] = d;
				f++;
			}
			if(f > 0)
			{	//	�f�[�^���������L��
				i = can_tp_job(-1, id, &can_buf.ID[id].BYTE);
				if(i > 0)
				{	//	�����L��
					xid[xwp++] = i;
				}
				else
				{	//	���̂܂ܕԂ�
					tp += can_id_event(id, tp);
					xid[xwp++] = id;
				}
				if(xwp >= 32) break;	//	������
			}
		}
		while(*cmd == ' ') cmd++;
	}
/*
	//	�����d��
	if(xwp > 0)
	{	//	�������������A�ʒm
		SendPC("ECU ");
		for(f = 0; f < xwp; f++)
		{
			id = xid[f];
			j = 0;
			tx[j++] = HEX_CHAR[(id >> 8) & 0x0F];
			tx[j++] = HEX_CHAR[(id >> 4) & 0x0F];
			tx[j++] = HEX_CHAR[id & 0x0F];
			for(i = 0; i < 8; i++)
			{
				c = can_buf.ID[id].BYTE[i];
				tx[j++] = HEX_CHAR[(c >> 4) & 0x0F];
				tx[j++] = HEX_CHAR[c & 0x0F];
			}
			if((f + 1) < xwp) tx[j++] = ' ';
			tx[j++] = 0;
			SendPC(tx);
		}
		SendPC("\r");
	}*/
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : ECU�t���[���f�[�^���M
//  
//  ����   : char *cmg	�R�}���h������
//  
//  ����   : �w��ID�̃t���[���f�[�^�������ւ���
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void ecu_put_command(char *cmd)
{
	ECU_CYC_EVE		mbox;
	int				i, j, f;
	int				id, tp;
	char			c;
	unsigned char	d;

	if(cmd == 0 || *cmd == 0) return;
	while(*cmd == ' ') cmd++;
	if(*cmd < '0' || *cmd > '7') return;
	//	������������
	while(*cmd >= '0' && *cmd <= '7')
	{
		//	�Ώ�ID�擾
		id = 0;
		for(i = 0; i < 3; i++)
		{
			c = *cmd;
			if(c >= 'a' && c <= 'f') c -= 0x27;
			if(c >= 'A' && c <= 'F') c -= 7;
			if(c < 0x30 || c > 0x3F) break;
			id = (id << 4) | (int)(c & 0x0F);
			cmd++;
		}
		if(i == 3 && id < CAN_ID_MAX)
		{	//	ID����A�f�[�^����
			f = 0;
			for(i = 0; i < 8; i++)
			{
				c = *cmd;
				if(c >= 'a' && c <= 'f') c -= 0x27;
				if(c >= 'A' && c <= 'F') c -= 7;
				if(c < 0x30 || c > 0x3F) break;
				d = (unsigned char)(c & 0x0F);
				cmd++;
				c = *cmd;
				if(c >= 'a' && c <= 'f') c -= 0x27;
				if(c >= 'A' && c <= 'F') c -= 7;
				if(c < 0x30 || c > 0x3F) break;
				d = (d << 4) | (unsigned char)(c & 0x0F);
				cmd++;
				can_buf.ID[id].BYTE[i] = d;
				f++;
			}
			if(f > 0)
			{	//	�f�[�^���������L��
				i = can_tp_job(-1, id, &can_buf.ID[id].BYTE);
				if(i == 0)
				{	//	��������
					mbox.ID.LONG = 0;
					mbox.TIMER.LONG = 0;
					mbox.ID.BIT.SID = id;
					mbox.ID.BIT.ENB = 1;
					mbox.ID.BIT.DLC = f;
					can_send_proc(&mbox);
				}
			}
			else
			{	//	�����[�g�t���[�����s
				mbox.ID.LONG = 0;
				mbox.TIMER.LONG = 0;
				mbox.ID.BIT.SID = id;
				mbox.ID.BIT.ENB = 1;
				mbox.ID.BIT.RTR = 1;
				mbox.ID.BIT.DLC = 8;
				can_send_proc(&mbox);
			}
		}
		while(*cmd == ' ') cmd++;
	}
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : �ʐM�o�R�O�����͏��̍X�V
//  
//  ����   : char *cmg	�R�}���h������
//  
//  ����   : �w����̓o�b�t�@�̃f�[�^�������ւ���
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void ecu_input_update(char *cmd)
{
	int				i, f;
	int				id;
	char			c;
	unsigned long	d;

	if(cmd == 0 || *cmd == 0) return;
	//	������������
	while(*cmd != 0)
	{
		while(*cmd == ' ') cmd++;
		//	�Ώ�ID�擾
		id = 0;
		for(i = 0; i < 2; i++)
		{
			c = *cmd;
			if(c >= 'a' && c <= 'f') c -= 0x27;
			if(c >= 'A' && c <= 'F') c -= 7;
			if(c < 0x30 || c > 0x3F) break;
			id = (id << 4) | (int)(c & 0x0F);
			cmd++;
		}
		if(i == 2 && id < EX_IO_MAX)
		{	//	ID����A�f�[�^����
			f = 0;
			d = 0;
			for(i = 0; i < 8; i++)
			{
				c = *cmd;
				if(c >= 'a' && c <= 'f') c -= 0x27;
				if(c >= 'A' && c <= 'F') c -= 7;
				if(c < 0x30 || c > 0x3F) break;
				d = (d << 4) | (unsigned char)(c & 0x0F);
				cmd++;
				f++;
			}
			if(f > 0)
			{	//	�f�[�^���������L��
				exiosts.DATA[id].LONG = d;
			}
		}
		else break;
	}
}

