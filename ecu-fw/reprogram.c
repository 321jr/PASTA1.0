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

//________________________________________________________________________________________
//
//	REPROGRAM	�v���g�R������
//
//----------------------------------------------------------------------------------------
//	�J������
//
//	2017/02/13	�R�[�f�B���O�J�n�i�k�j
//
//----------------------------------------------------------------------------------------
//	T.Tachibana
//	��L&F
//________________________________________________________________________________________
//

#include <sysio.h>
#include <string.h>
#include <stdio.h>
#include "iodefine.h"
#include "altypes.h"
#include "timer.h"
#include "flash_data.h"
#include "r_Flash_API_RX600.h"
#include "flash_rom.h"
#include "ecu.h"			/*	ECU ���ʒ�`			*/
#include "can3_spi2.h"		/*	CAN3 ��`				*/

/*
	Reprogram �����̊T�v

	CAN�t���[����`
	
	ECU�ʎw��
	CAN-ID	0x7C0�`7C7
	
					ECU�N�G����M(0x7DF)�����ׂĂ�ECU�������ΏۂƂȂ�
				+---------------+---------------+---------------+---------------+---------------+---------------+---------------+---------------+
		Byte	|		0		|		1		|		2		|		3		|		4		|		5		|		6		|		7		|
				+---------------+---------------+---------------+---------------+---------------+---------------+---------------+---------------+
		�Ӗ�	|	�f�[�^��	|	�R�}���h	|	�t�я��	|				|				|				|				|				|
				+---------------+---------------+---------------+---------------+---------------+---------------+---------------+---------------+
		�͈�	|	  0�`7		|	00 �` FF	|	00 �` FF	|				|				|				|				|				|
				+---------------+---------------+---------------+---------------+---------------+---------------+---------------+---------------+
	
	CAN-ID	0x7C0	ECU0(�p���g���n)�ւ̗v��
			0x7C1	ECU1(�{�f�B�[�n)�ւ̗v��
			0x7C2	ECU2(�V���V�[�n)�ւ̗v��
			0x7C7	ECU3(CGW-DTC)�ւ̗v��

	ECU����ID
	CAN-ID	0x7C8	ECU0(�p���g���n)����̉���
			0x7C9	ECU1(�{�f�B�[�n)����̉���
			0x7CA	ECU2(�V���V�[�n)����̉���
			0x7CF	ECU3(CGW-DTC)����̉���
*/

extern  void	restart_pos(void);

//----------------------------------------------------------------------------------------
//	OBD2�p�P�b�g���p�̒�`
//----------------------------------------------------------------------------------------
typedef	union	__repro_query_frame__	{
	unsigned char	BYTE[8];
	struct	{
		unsigned char	LEN;		//	�f�[�^��
		unsigned char	CMD;		//	�R�}���h
		unsigned char	ADH;		//	�A�h���X���8�r�b�g
		unsigned char	ADL;		//	�A�h���X����8�r�b�g
		unsigned char	DAT[4];		//	�t�я��
	}	PACK;
}	REPRO_QUERY_FRAME;

//----------------------------------------------------------------------------------------
//	�R�}���h�R�[�h��`
//----------------------------------------------------------------------------------------
#define		ALL_CONFIG_SAVE			0x07		/*	�^�p�f�[�^���f�[�^�t���b�V���ֈꊇ�ۑ�			*/

#define		GET_ROUTING_MAP			0x08		/*	���[�e�B���O�}�b�v�擾							*/
#define		SET_ROUTING_MAP			0x0A		/*	���[�e�B���O�}�b�v�ύX							*/
#define		ERA_ROUTING_MAP			0x0E		/*	���[�e�B���O�}�b�vROM����						*/
#define		SAV_ROUTING_MAP			0x0F		/*	���[�e�B���O�}�b�vROM�ۑ�						*/

#define		DEL_CYCEVE_LIST			0x11		/*	�ғ����̎������M�E�C�x���g���X�g����w��ID���폜	*/
#define		NEW_CYCEVE_LIST			0x12		/*	�ғ����̎������M�E�C�x���g���X�g�֐V�KID��ǉ�	*/
#define		GET_CYCEVE_LIST			0x13		/*	�������M�E�C�x���g���X�g�擾					*/
#define		SET_CYCEVE_LIST			0x14		/*	�������M�E�C�x���g���X�g�ύX					*/
#define		GET_CYCEVE_LIST1		0x18		/*	�������M�E�C�x���g���X�gID�擾					*/
#define		GET_CYCEVE_LIST2		0x19		/*	�������M�E�C�x���g���X�gTimer�擾				*/
#define		SET_CYCEVE_LIST1		0x1A		/*	�������M�E�C�x���g���X�gID�ύX					*/
#define		SET_CYCEVE_LIST2		0x1B		/*	�������M�E�C�x���g���X�gTimer�ύX				*/
#define		ERA_CYCEVE_LIST			0x1E		/*	�������M�E�C�x���g���X�gROM����					*/
#define		SAV_CYCEVE_LIST			0x1F		/*	�������M�E�C�x���g���X�gROM�ۑ�					*/

#define		GET_EXT_IO_LIST			0x28		/*	�O�����o�̓��X�g�擾							*/
#define		SET_EXT_IO_LIST			0x2A		/*	�O�����o�̓��X�g�ύX							*/
#define		ERA_EXT_IO_LIST			0x2E		/*	�O�����o�̓��X�gROM����							*/
#define		SAV_EXT_IO_LIST			0x2F		/*	�O�����o�̓��X�gROM�ۑ�							*/

#define		READ_FIRMWARE			0x38		/*	�t�@�[���E�F�A�擾								*/
#define		UPDATE_FIRMWARE			0x3C		/*	�t�@�[���E�F�AROM�ۑ�							*/
#define		REMOVE_FIRMWARE			0x3D		/*	�t�@�[���E�F�AROM����							*/
#define		COPY_FIRMWARE			0x3E		/*	�t�@�[���E�F�AROM�������ݗp�o�b�t�@�ւ̓]��		*/
#define		WRITE_FIRMWARE			0x3F		/*	�t�@�[���E�F�AROM�������݁i�������Ɏ��s����j	*/

//----------------------------------------------------------------------------------------
//	�ϐ���`
//----------------------------------------------------------------------------------------
REPRO_QUERY_FRAME	repro_req;		//	�v���f�[�^
REPRO_QUERY_FRAME	repro_ret;		//	�����f�[�^
unsigned char		fw_image[128];	//	�������ݐ�p�������o�b�t�@
unsigned long		fw_address;		//	�������ݐ�p�������A�h���X

/*
//________________________________________________________________________________________
//
//  boot_copy
//----------------------------------------------------------------------------------------
//  �@�\����
//	  APP��ROM�ɃR�s�[����
//  ����
//	  ����
//  �߂�
//	  ����
//________________________________________________________________________________________
//
int fw_copy(unsigned long sapp, unsigned long eapp)
{
	int				err = 0;
	unsigned long   rom = 0xFFF40000;
	unsigned long   app = sapp; //0x00000000;
	unsigned long   ape = eapp; //0x00020000;
	unsigned long   stp = 0x00008000;

//	reset_fcu();	//  FCU���Z�b�g
//	flash_init();	//  FCU�C�j�V�����C�Y
	for(; app < ape && err == 0; app += stp, rom += stp)
	{
		_di();
		//  �u���b�N����(�u���b�N�T�C�Y)
		if(flash_erase_rom(rom) != 0)
		{
			err++;
		}
		//  ��������(128byte�P��)
		if(flash_write_rom(rom, app, stp) != 0)
		{
			err++;
		}
		_ei();
	}
	return ((err == 0) ? 1 : 0);
}
*/
//----------------------------------------------------------------------------------------
//	BootCopy ����
//----------------------------------------------------------------------------------------
int bootcopy(void)
{
	unsigned long   rom = 0xFFF40000;
	unsigned long   app = 0x00000000;
	unsigned long   ape = 0x00020000;
	unsigned long   stp = 128;

	*((unsigned long *)0x60) = (unsigned long)restart_pos;

//	reset_fcu();	//  FCU���Z�b�g
//	flash_init();	//  FCU�C�j�V�����C�Y

	for(; app < ape; app += stp, rom += stp)
	{
		_di();
		if(R_FlashWrite(rom, app, (unsigned short)stp) != FLASH_SUCCESS)
		{
			_ei();
			return 0;
		}
		_ei();
	}
//	if(R_FlashWrite(0xFFF00000, 0, 0x00020000) != FLASH_SUCCESS) return 0;
//	return fw_copy(0x00000000, 0x00020000);
	return 1;
}

//________________________________________________________________________________________
//
//  erace_rom
//----------------------------------------------------------------------------------------
//  �@�\����
//	  ROM�̎w��G���A����������(0xFFF00000�`0xFFF7FFFF)
//  ����
//	  sadr		�J�n�Ԓn
//	  eadr		�I���Ԓn
//  �߂�
//	  char*	   ���b�Z�[�W
//________________________________________________________________________________________
//
int bootclear(void)
{
	int blk;

//	reset_fcu();	//  FCU���Z�b�g
//	flash_init();	//  FCU�C�j�V�����C�Y

	for(blk = BLOCK_53; blk > BLOCK_37; blk--)
	{
		_di();
		if(R_FlashErase(blk) != FLASH_SUCCESS)
		{
			_ei();
			return 0;
		}
		_ei();
	}
	return 1;
}

//----------------------------------------------------------------------------------------
//	ECU�^�p�f�[�^�̈ꊇ�ۑ�
//----------------------------------------------------------------------------------------
int ecu_data_write(void)
{
	int	bk, i;
	int wp = 0;
	int fe = 0;
	
	//	�̈�������s
	for(i = 0, bk = BLOCK_DB0; bk <= BLOCK_DB2; bk++, i++)
	{
		if(R_FlashDataAreaBlankCheck(g_flash_BlockAddresses[bk], BLANK_CHECK_ENTIRE_BLOCK) == FLASH_NOT_BLANK)
		{	//	�������ݗL��
			while(R_FlashGetStatus() != FLASH_SUCCESS);
			if(R_FlashErase(bk) == FLASH_SUCCESS)
			{
				while(R_FlashGetStatus() != FLASH_SUCCESS);
				if(R_FlashDataAreaBlankCheck(g_flash_BlockAddresses[bk], BLANK_CHECK_ENTIRE_BLOCK) == FLASH_BLANK)
				{	//	��������
					fe |= (1 << i);
				}
			}
		}
		else
		{
			fe |= (1 << i);
		}
	}

	//	MAP��������
	if((fe & 1) != 0)
	{	//	�����m�F�ς�
		if(R_FlashWrite(ADDRESS_OF_ROOTMAP, (int)&rout_map, sizeof(ECU_ROUT_MAP)) == FLASH_SUCCESS)
		{	//	�������݊���
			wp |= 1;
		}
	}
	//	CONF��������
	if((fe & 2) != 0)
	{	//	�����m�F�ς�
		if(R_FlashWrite(ADDRESS_OF_CYCEVE, (int)&conf_ecu.LIST[0], (sizeof(ECU_CYC_EVE) * MESSAGE_MAX)) == FLASH_SUCCESS)
		{	//	�������݊���
			wp |= 2;
		}
	}
	//	I/O��������
	if((fe & 4) != 0)
	{	//	�����m�F�ς�
		if(R_FlashWrite(ADDRESS_OF_IOLIST, (int)&ext_list[0], sizeof(ext_list)) == FLASH_SUCCESS)
		{	//	�������݊���
			wp |= 4;
		}
	}
	while(R_FlashGetStatus() != FLASH_SUCCESS);
	return wp;
}

//----------------------------------------------------------------------------------------
//	ECU�^�p�f�[�^�̈ꊇ����
//----------------------------------------------------------------------------------------
int ecu_data_erase(void)
{
	int	bk, i;
	int fe = 0;
	
	for(i = 0, bk = BLOCK_DB0; bk <= BLOCK_DB15; bk++, i++)
	{
		if(R_FlashDataAreaBlankCheck(g_flash_BlockAddresses[bk], BLANK_CHECK_ENTIRE_BLOCK) == FLASH_NOT_BLANK)
		{	//	�������ݗL��
			while(R_FlashGetStatus() != FLASH_SUCCESS);
			if(R_FlashErase(bk) == FLASH_SUCCESS)
			{
				while(R_FlashGetStatus() != FLASH_SUCCESS);
				if(R_FlashDataAreaBlankCheck(g_flash_BlockAddresses[bk], BLANK_CHECK_ENTIRE_BLOCK) == FLASH_BLANK)
				{	//	��������
					fe |= (1 << i);
				}
			}
		}
	}
	return fe;
}

//----------------------------------------------------------------------------------------
//	ECU�^�p�f�[�^�̏������ݏ�Ԋm�F
//----------------------------------------------------------------------------------------
int ecu_data_check(void)
{
	int	bk, i;
	int wf = 0;
	
	for(i = 0, bk = BLOCK_DB0; bk <= BLOCK_DB15; bk++, i++)
	{
		if(R_FlashDataAreaBlankCheck(g_flash_BlockAddresses[bk], BLANK_CHECK_ENTIRE_BLOCK) == FLASH_NOT_BLANK)
		{	//	�������ݗL��
			wf |= (1 << i);
		}
	}
	return wf;
}

/*
//----------------------------------------------------------------------------------------
//	Reprogram ����
//----------------------------------------------------------------------------------------
int repro_job(int ch, int id, void *frame)
{
	int	ret;
	unsigned long	adr;
	int	sw = SELECT_ECU_UNIT + 0x7C0;

	if(id == 0x7BF)
	{	//	���v���v��
		unsigned char	buf[8];
		repro_mode = 0;
		memcpy(buf, (REPRO_QUERY_FRAME *)frame, sizeof(REPRO_QUERY_FRAME));
		//	�t�H�[�}�b�g�m�F	43 41 4E 3X 00 00 00 00
		if(buf[0] == 0x43 && buf[1] == 0x41 && buf[2] == 0x4E && buf[3] == (SELECT_ECU_UNIT | 0x30))
		{	//	���v���R�[�h�m�F
			repro_mode = 1;
			//	�����ԐM
			sw += 8;
			memcpy(&can_buf.ID[sw], buf, 8);
			if(ch >= 0) add_mbox_frame(ch, 8, CAN_DATA_FRAME, sw);	//	���M�҂��o�b�t�@�ςݏグ
			return sw;	//	�]������
		}
	}
	if(id != sw || repro_mode == 0) return 0;	//	ID�ΏۊO

	memcpy(&repro_req, (REPRO_QUERY_FRAME *)frame, sizeof(REPRO_QUERY_FRAME));
	adr = ((int)repro_req.PACK.ADH << 8) | ((int)repro_req.PACK.ADL);
	repro_ret.PACK.LEN = 0;
	repro_ret.PACK.CMD = repro_req.PACK.CMD + 0x40;	//	�����R�[�h
	repro_ret.PACK.ADH = repro_req.PACK.ADH;
	repro_ret.PACK.ADL = repro_req.PACK.ADL;
	switch(repro_req.PACK.CMD)
	{
	case ALL_CONFIG_SAVE:	//	�f�[�^�t���b�V���ֈꊇ�ۑ�
		repro_ret.PACK.LEN += 1;
		if(ecu_data_write() == 7) break;
		repro_ret.PACK.CMD |= 0x80;	//	�G���[
		break;
	//------------------------------------------------
	//	���[�e�B���O�}�b�v		adr=000h�`7FFh
	//------------------------------------------------
	case GET_ROUTING_MAP:	//	���[�e�B���O�}�b�v�擾
		repro_ret.PACK.LEN += 7;
		memcpy(&repro_ret.PACK.DAT[0], &rout_map.ID[adr].BYTE, 4);
		break;
	case SET_ROUTING_MAP:	//	���[�e�B���O�}�b�v�ύX
		repro_ret.PACK.LEN += 3;
		memcpy(&rout_map.ID[adr].BYTE, &repro_ret.PACK.DAT[0], (repro_req.PACK.LEN - 3));
		break;
	case SAV_ROUTING_MAP:	//	���[�e�B���O�}�b�vROM�ۑ�
		repro_ret.PACK.LEN += 1;
		//	�����ς݊m�F
		if(R_FlashDataAreaBlankCheck(g_flash_BlockAddresses[BLOCK_DB0], BLANK_CHECK_ENTIRE_BLOCK) == FLASH_BLANK)
		{	//	����
			while(R_FlashGetStatus() != FLASH_SUCCESS);
			if(R_FlashWrite(ADDRESS_OF_ROOTMAP, (int)&rout_map, sizeof(ECU_ROUT_MAP)) == FLASH_SUCCESS)
			{	//	�������݊���
				break;
			}
		}
		while(R_FlashGetStatus() != FLASH_SUCCESS);
		repro_ret.PACK.CMD |= 0x80;	//	�G���[
		break;
	case ERA_ROUTING_MAP:	//	���[�e�B���O�}�b�vROM����
		repro_ret.PACK.LEN += 1;
		//	�����ς݊m�F
		if(R_FlashDataAreaBlankCheck(g_flash_BlockAddresses[BLOCK_DB0], BLANK_CHECK_ENTIRE_BLOCK) == FLASH_BLANK)
		{	//	����
			break;
		}
		while(R_FlashGetStatus() != FLASH_SUCCESS);
		if(R_FlashErase(BLOCK_DB0) == FLASH_SUCCESS)
		{
			while(R_FlashGetStatus() != FLASH_SUCCESS);
			if(R_FlashDataAreaBlankCheck(g_flash_BlockAddresses[BLOCK_DB0], BLANK_CHECK_ENTIRE_BLOCK) == FLASH_BLANK)
			{	//	����
				break;
			}
		}
		while(R_FlashGetStatus() != FLASH_SUCCESS);
		repro_ret.PACK.CMD |= 0x80;	//	�G���[
		break;
	//------------------------------------------------
	//	�������M�E�C�x���g���X�g	adr=0�`255
	//------------------------------------------------
	case DEL_CYCEVE_LIST:	//	�ғ����̎������M�E�C�x���g���X�g����w��ID���폜
		repro_ret.PACK.LEN += 3;
		delete_cyceve_list(adr);	//	�Ǘ�����ID���폜
		delete_waiting_list(adr);	//	���ԑ҂�ID���폜
		break;
	case NEW_CYCEVE_LIST:	//	�ғ����̎������M�E�C�x���g���X�g�֐V�KID��ǉ�
		repro_ret.PACK.LEN += 3;
		insert_cyceve_list(adr);	//	���X�g�A��
		can_id_event(adr, 0);		//	�C�x���g�o�^
		break;
	case GET_CYCEVE_LIST:	//	�������M�E�C�x���gID���X�g�擾
		repro_ret.PACK.LEN += 7;
		memcpy(&repro_ret.PACK.DAT[0], &((unsigned char *)&conf_ecu.LIST[0])[adr], 4);
		break;
	case SET_CYCEVE_LIST:	//	�������M�E�C�x���gID���X�g�ύX
		repro_ret.PACK.LEN += 3;
		memcpy(&((unsigned char *)&conf_ecu.LIST[0])[adr], &repro_ret.PACK.DAT[0], (repro_req.PACK.LEN - 3));
		break;
	case GET_CYCEVE_LIST1:	//	�������M�E�C�x���gID���X�g�擾
		repro_ret.PACK.LEN += 7;
		memcpy(&repro_ret.PACK.DAT[0], &conf_ecu.LIST[adr].ID, 4);
		break;
	case GET_CYCEVE_LIST2:	//	�������M�E�C�x���gTimer���X�g�擾
		repro_ret.PACK.LEN += 7;
		memcpy(&repro_ret.PACK.DAT[0], &conf_ecu.LIST[adr].TIMER, 4);
		break;
	case SET_CYCEVE_LIST1:	//	�������M�E�C�x���gID���X�g�ύX
		repro_ret.PACK.LEN += 3;
		memcpy(&conf_ecu.LIST[adr].ID, &repro_ret.PACK.DAT[0], (repro_req.PACK.LEN - 3));
		break;
	case SET_CYCEVE_LIST2:	//	�������M�E�C�x���g���X�g�ύX
		repro_ret.PACK.LEN += 3;
		memcpy(&conf_ecu.LIST[adr].TIMER, &repro_ret.PACK.DAT[0], (repro_req.PACK.LEN - 3));
		break;
	case ERA_CYCEVE_LIST:	//	�������M�E�C�x���g���X�gROM����
		repro_ret.PACK.LEN += 1;
		//	�����ς݊m�F
		if(R_FlashDataAreaBlankCheck(g_flash_BlockAddresses[BLOCK_DB1], BLANK_CHECK_ENTIRE_BLOCK) == FLASH_BLANK)
		{	//	����
			break;
		}
		while(R_FlashGetStatus() != FLASH_SUCCESS);
		if(R_FlashErase(BLOCK_DB1) == FLASH_SUCCESS)
		{
			while(R_FlashGetStatus() != FLASH_SUCCESS);
			if(R_FlashDataAreaBlankCheck(g_flash_BlockAddresses[BLOCK_DB1], BLANK_CHECK_ENTIRE_BLOCK) == FLASH_BLANK)
			{	//	����
				break;
			}
		}
		while(R_FlashGetStatus() != FLASH_SUCCESS);
		repro_ret.PACK.CMD |= 0x80;	//	�G���[
		break;
	case SAV_CYCEVE_LIST:	//	�������M�E�C�x���g���X�gROM�ۑ�
		repro_ret.PACK.LEN += 1;
		//	�����ς݊m�F
		if(R_FlashDataAreaBlankCheck(g_flash_BlockAddresses[BLOCK_DB1], BLANK_CHECK_ENTIRE_BLOCK) == FLASH_BLANK)
		{	//	����
			while(R_FlashGetStatus() != FLASH_SUCCESS);
			if(R_FlashWrite(ADDRESS_OF_CYCEVE, (int)&conf_ecu.LIST[0], (sizeof(ECU_CYC_EVE) * MESSAGE_MAX)) == FLASH_SUCCESS)
			{	//	�������݊���
				break;
			}
		}
		while(R_FlashGetStatus() != FLASH_SUCCESS);
		repro_ret.PACK.CMD |= 0x80;	//	�G���[
		break;
	//------------------------------------------------
	//	�O�����o�̓��X�g
	//------------------------------------------------
	case GET_EXT_IO_LIST:	//	�O�����o�̓��X�g�擾
		repro_ret.PACK.LEN += 7;
		memcpy(&repro_ret.PACK.DAT[0], &((unsigned char *)&ext_list[0])[adr], 4);
		break;
	case SET_EXT_IO_LIST:	//	�O�����o�̓��X�g�ύX
		repro_ret.PACK.LEN += 3;
		memcpy(&((unsigned char *)&ext_list[0])[adr], &repro_ret.PACK.DAT[0], (repro_req.PACK.LEN - 3));
		break;
	case ERA_EXT_IO_LIST:	//	�O�����o�̓��X�gROM����
		repro_ret.PACK.LEN += 1;
		//	�����ς݊m�F
		if(R_FlashDataAreaBlankCheck(g_flash_BlockAddresses[BLOCK_DB2], BLANK_CHECK_ENTIRE_BLOCK) == FLASH_BLANK)
		{	//	����
			break;
		}
		while(R_FlashGetStatus() != FLASH_SUCCESS);
		if(R_FlashErase(BLOCK_DB2) == FLASH_SUCCESS)
		{
			while(R_FlashGetStatus() != FLASH_SUCCESS);
			if(R_FlashDataAreaBlankCheck(g_flash_BlockAddresses[BLOCK_DB2], BLANK_CHECK_ENTIRE_BLOCK) == FLASH_BLANK)
			{	//	����
				break;
			}
		}
		while(R_FlashGetStatus() != FLASH_SUCCESS);
		repro_ret.PACK.CMD |= 0x80;	//	�G���[
		break;
	case SAV_EXT_IO_LIST:	//	�O�����o�̓��X�gROM�ۑ�
		repro_ret.PACK.LEN += 1;
		//	�����ς݊m�F
		if(R_FlashDataAreaBlankCheck(g_flash_BlockAddresses[BLOCK_DB2], BLANK_CHECK_ENTIRE_BLOCK) == FLASH_BLANK)
		{	//	����
			while(R_FlashGetStatus() != FLASH_SUCCESS);
			if(R_FlashWrite(ADDRESS_OF_IOLIST, (int)&ext_list[0], sizeof(ext_list)) == FLASH_SUCCESS)
			{	//	�������݊���
				break;
			}
		}
		while(R_FlashGetStatus() != FLASH_SUCCESS);
		repro_ret.PACK.CMD |= 0x80;	//	�G���[
		break;
	//------------------------------------------------
	//	�t�@�[���E�F�A
	//------------------------------------------------
	case READ_FIRMWARE:	//	�t�@�[���E�F�A�擾
		//	�����m�F
		adr = 0xFFF00000 + (adr << 2);
		repro_ret.PACK.LEN += 7;
		memcpy(&repro_ret.PACK.DAT[0], ((unsigned char *)adr), 4);
		break;
	case UPDATE_FIRMWARE:	//	�t�@�[���E�F�AROM�ۑ�
		repro_ret.PACK.LEN += 1;
		if(bootcopy() == 1) break;	//	����
		repro_ret.PACK.CMD |= 0x80;	//	�G���[
		break;
	case REMOVE_FIRMWARE:	//	�t�@�[���E�F�AROM����
		repro_ret.PACK.LEN += 1;
		if(bootclear() == 1)
		{	//	��������
			break;
		}
		while(R_FlashGetStatus() != FLASH_SUCCESS);
		repro_ret.PACK.CMD |= 0x80;	//	�G���[
		break;
	case COPY_FIRMWARE:	//	�t�@�[���E�F�AROM�������ݗp�o�b�t�@�ւ̓]��
		repro_ret.PACK.LEN += 3;
		memcpy(&fw_image[adr], &repro_ret.PACK.DAT[0], 4);
		break;
	case WRITE_FIRMWARE:	//	�t�@�[���E�F�AROM�������݁i�������Ɏ��s����j
		repro_ret.PACK.LEN += 3;
		adr = 0xFFF00000 + (adr << 2);
		if(R_FlashWrite(adr, (unsigned long)&fw_image[0], 128) == FLASH_SUCCESS)
		{	//	�ۑ�����
			break;
		}
		while(R_FlashGetStatus() != FLASH_SUCCESS);
		repro_ret.PACK.CMD |= 0x80;	//	�G���[
		break;
	}
	//	�ԐM����
	if(repro_ret.PACK.LEN > 0)
	{	//	�����ԐM
		sw += 8;
		memcpy(&can_buf.ID[sw], repro_ret.BYTE, 8);
		if(ch >= 0) add_mbox_frame(ch, 8, CAN_DATA_FRAME, sw);	//	���M�҂��o�b�t�@�ςݏグ
		return sw;	//	�]������
	}
	return 0;
}
*/


