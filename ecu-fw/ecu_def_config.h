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
// $RCSfile: ecu_def_powertrain.h,v $
// $Revision: 1.00 $
// $Date: 2017/01/15 11:05:00 $
// 
// Copyright (c) 2017 LandF Corporation.
//
// History:
//

#ifndef		__ECU_DEFAULT_SETTING__
#define		__ECU_DEFAULT_SETTING__

//---------------------------------------------------------------------------------------
//	�h���C�r���O�V�~�����[�^�E�p���[�g���C���ԋ���ID�ݒ�
//
//	�����ɐݒ肵��ID���p���g��ECU����M����ƁA����ȍ~��ID�̑���M������ύX���܂��B
//	LCD�ɑ΂��ẮA�Y��ID�̃t���[���f�[�^�̌��Ƀ}�[�N��t�����ēn���܂��B
//---------------------------------------------------------------------------------------

#define		DS_X_POWERTRAIN_ID		0x043		/*	�G���W����]���Ƒ��x	*/

//---------------------------------------------------------------------------------------
//	CGW��CAN-ID���[�h
//---------------------------------------------------------------------------------------

#define		CGW_ALL_ID_PASS			0			/*	0=�S�ē]��, 1=�w������̂ݓ]��	*/

//---------------------------------------------------------------------------------------
//	���[�e�B���O�}�b�v�����l
//---------------------------------------------------------------------------------------

#ifdef		__LFY_RX63N__
const	unsigned char	CH_MAP_PAT[] = { 0x00, 0x22, 0x22, 0x22, 0x22 };	//	LFY-RX63N��CAN1�̂ݗL��
#define		CH_MAP_CGW		0x22
#define		CH_MAP_SYS		0x22
#define		CH_MAP_RECV		0x20
#define		CH_MAP_SEND		0x02
#else
const	unsigned char	CH_MAP_PAT[] = { 0x00, 0x11, 0x33, 0x77, 0xFF };	//	CAN2ECU�͎w��`�����l��
#define		CH_MAP_CGW		0x7F
#define		CH_MAP_SYS		0xFF
#define		CH_MAP_RECV		0x10
#define		CH_MAP_SEND		0x01
#endif

//---------------------------------------------------------------------------------------
//	�t���[�������l
//---------------------------------------------------------------------------------------
typedef	struct	__def_frame_data__
{
	int				ID;		//	ID�ԍ�
	int				PAT;	//	�p�^�[���ԍ�
	unsigned char	DAT[8];	//	�f�[�^
}	DEF_FRAME_DATA;
const DEF_FRAME_DATA	DEF_FRAME_BUFFER[] = {
	{0x024, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 0x00}},	//	
	{0x146, 0, {0x00, 0x00, 0x00, 0x00, 0x01, 0x46, 0x00, 0x00}},	//	
//	{0x150, 0, {0x00, 0x00, 0x00, 0x00, 0x01, 0x50, 0x00, 0x00}},	//	
	{0x15A, 0, {0x00, 0x00, 0x00, 0x00, 0x01, 0x5A, 0x00, 0x00}},	//	
//	{0x164, 0, {0x00, 0x00, 0x00, 0x00, 0x01, 0x64, 0x00, 0x00}},	//	
//	{0x39E, 0, {0x00, 0x00, 0x00, 0x00, 0x03, 0x9E, 0x00, 0x00}},	//	
	{0x039, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x39, 0x00, 0x00}},	//	
	{0x16F, 0, {0x00, 0x00, 0x00, 0x00, 0x01, 0x6F, 0x00, 0x00}},	//	
	{0x043, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x43, 0x00, 0x00}},	//	
//	{0x179, 0, {0x00, 0x00, 0x00, 0x00, 0x01, 0x79, 0x00, 0x00}},	//	
	{0x183, 0, {0x00, 0x00, 0x00, 0x00, 0x01, 0x83, 0x00, 0x00}},	//	
//	{0x3A9, 0, {0x00, 0x00, 0x00, 0x00, 0x03, 0xA9, 0x00, 0x00}},	//	
//	{0x3B3, 0, {0x00, 0x00, 0x00, 0x00, 0x03, 0xB3, 0x00, 0x00}},	//	
	{0x3BD, 0, {0x00, 0x00, 0x00, 0x00, 0x03, 0xBD, 0x00, 0x00}},	//	
//	{0x3C7, 0, {0x00, 0x00, 0x00, 0x00, 0x03, 0xC7, 0x00, 0x00}},	//	
	{0x18D, 0, {0x00, 0x00, 0x00, 0x00, 0x01, 0x8D, 0x00, 0x00}},	//	
	{0x062, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x62, 0x00, 0x00}},	//	
	{0x198, 0, {0x00, 0x00, 0x00, 0x00, 0x01, 0x98, 0x00, 0x00}},	//	
//	{0x1A2, 0, {0x00, 0x00, 0x00, 0x00, 0x01, 0xA2, 0x00, 0x00}},	//	
	{0x077, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x77, 0x00, 0x00}},	//	
//	{0x1AD, 0, {0x00, 0x00, 0x00, 0x00, 0x01, 0xAD, 0x00, 0x00}},	//	
	{0x19A, 0, {0x00, 0x00, 0x00, 0x00, 0x01, 0x9A, 0x00, 0x00}},	//	
	{0x3D4, 0, {0x00, 0x00, 0x00, 0x00, 0x03, 0xD4, 0x00, 0x00}},	//	
	{0x3DE, 0, {0x00, 0x00, 0x00, 0x00, 0x03, 0xDE, 0x00, 0x00}},	//	
	{0x1D3, 0, {0x00, 0x00, 0x00, 0x00, 0x01, 0xD3, 0x00, 0x00}},	//	
//	{0x42B, 0, {0x00, 0x00, 0x00, 0x00, 0x04, 0x2B, 0x00, 0x00}},	//	
	{0x482, 0, {0x00, 0x00, 0x00, 0x00, 0x04, 0x82, 0x00, 0x00}},	//	

	{0x01A, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x1A, 0x00, 0x00}},	//	
	{0x02F, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x2F, 0x00, 0x00}},	//	
	{0x058, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x58, 0x00, 0x00}},	//	
	{0x06D, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x6D, 0x00, 0x00}},	//	
	{0x083, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x83, 0x00, 0x00}},	//	
	{0x098, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x98, 0x00, 0x00}},	//	
	{0x1A7, 0, {0x00, 0x00, 0x00, 0x00, 0x01, 0xA7, 0x00, 0x00}},	//	
	{0x1B1, 0, {0x00, 0x00, 0x00, 0x00, 0x01, 0xB1, 0x00, 0x00}},	//	
	{0x1B8, 0, {0x00, 0x00, 0x00, 0x00, 0x01, 0xB8, 0x00, 0x00}},	//	
	{0x1C9, 0, {0x00, 0x00, 0x00, 0x00, 0x01, 0xC9, 0x00, 0x00}},	//	
	{0x25C, 0, {0x00, 0x00, 0x00, 0x00, 0x02, 0x5C, 0x00, 0x00}},	//	
	{0x271, 0, {0x00, 0x00, 0x00, 0x00, 0x02, 0x71, 0x00, 0x00}},	//	
	{0x286, 0, {0x00, 0x00, 0x00, 0x00, 0x02, 0x86, 0x00, 0x00}},	//	
	{0x29C, 0, {0x00, 0x00, 0x00, 0x00, 0x02, 0x9C, 0x00, 0x00}},	//	
	{0x2B1, 0, {0x00, 0x00, 0x00, 0x00, 0x02, 0xB1, 0x00, 0x00}},	//	

	{0x08D, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x8D, 0x00, 0x00}},	//	
//	{0x3E9, 0, {0x00, 0x00, 0x00, 0x00, 0x03, 0xE9, 0x00, 0x00}},	//	
	{0x0A2, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0xA2, 0x00, 0x00}},	//	
//	{0x3F4, 0, {0x00, 0x00, 0x00, 0x00, 0x03, 0xF4, 0x00, 0x00}},	//	
	{0x1BB, 0, {0x00, 0x00, 0x00, 0x00, 0x01, 0xBB, 0x00, 0x00}},	//	
//	{0x3FF, 0, {0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0x00, 0x00}},	//	
	{0x266, 0, {0x00, 0x00, 0x00, 0x00, 0x02, 0x66, 0x00, 0x00}},	//	
//	{0x40A, 0, {0x00, 0x00, 0x00, 0x00, 0x04, 0x0A, 0x00, 0x00}},	//	
	{0x27B, 0, {0x00, 0x00, 0x00, 0x00, 0x02, 0x7B, 0x00, 0x00}},	//	
//	{0x415, 0, {0x00, 0x00, 0x00, 0x00, 0x04, 0x15, 0x00, 0x00}},	//	
	{0x290, 0, {0x00, 0x00, 0x00, 0x00, 0x02, 0x90, 0x00, 0x00}},	//	
	{0x420, 0, {0x00, 0x00, 0x00, 0x00, 0x04, 0x20, 0x00, 0x00}},	//	
	{0x2A6, 0, {0x00, 0x00, 0x00, 0x00, 0x02, 0xA6, 0x00, 0x00}},	//	
//	{0x436, 0, {0x00, 0x00, 0x00, 0x00, 0x04, 0x36, 0x00, 0x00}},	//	
	{0x2BB, 0, {0x00, 0x00, 0x00, 0x00, 0x02, 0xBB, 0x00, 0x00}},	//	
//	{0x441, 0, {0x00, 0x00, 0x00, 0x00, 0x04, 0x41, 0x00, 0x00}},	//	
	{0x0B4, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0xB4, 0x00, 0x00}},	//	
//	{0x44C, 0, {0x00, 0x00, 0x00, 0x00, 0x04, 0x4C, 0x00, 0x00}},	//	
	{0x457, 0, {0x00, 0x00, 0x00, 0x00, 0x04, 0x57, 0x00, 0x00}},	//	
	{0x461, 0, {0x00, 0x00, 0x00, 0x00, 0x04, 0x61, 0x00, 0x00}},	//	
	{0x46C, 0, {0x00, 0x00, 0x00, 0x00, 0x04, 0x6C, 0x00, 0x00}},	//	
	{0x477, 0, {0x00, 0x00, 0x00, 0x00, 0x04, 0x77, 0x00, 0x00}},	//	
	{   -1, 0, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},	//	
};

//---------------------------------------------------------------------------------------
//  
//  �@�\   : �}�b�v������
//  
//  ����   : ����
//  
//  ����   : ���[�e�B���O�}�b�v�̏����l�ݒ�
//  
//  �߂�l : ����
//  
//  ���l   : �����l�͑S�Ẵ��b�Z�[�W����t�E�]������B
//  
//---------------------------------------------------------------------------------------
#define		cgw_rootmap(a,b)	rout_map.ID[(a)].BYTE=(b)
void defset_rootmap(void)
{
	int	i;
	memset(&rout_map, 0, sizeof(rout_map));
	memset(can_to_exio, -1, sizeof(can_to_exio));
	if(SELECT_ECU_UNIT == ECU_UNIT_CGW)
	{	//	ECU_UNIT_POWERTRAIN
#if	(CGW_ALL_ID_PASS==0)
		memset(&rout_map, CH_MAP_SYS, sizeof(rout_map));
#else
		cgw_rootmap(0x024, PT_TO_CS);		//P	�u���[�L�o�͗�
		cgw_rootmap(0x039, PT_TO_CS);		//P	�X���b�g���ʒu
		cgw_rootmap(0x043, PT_TO_CS);		//P	�G���W����]���Ƒ��x
		cgw_rootmap(0x062, PT_TO_CS);		//P	�p���X�e�o�͗�
		cgw_rootmap(0x077, PT_TO_CS);		//P	�V�t�g�|�W�V�����ʒu
		cgw_rootmap(0x146, PT_TO_CS);		//P	�u���[�L�I�C����
		cgw_rootmap(0x150, PT_TO_CS);		//P	�u���[�L���Ռx���E�A�C�X�o�[���x��
		cgw_rootmap(0x15A, PT_TO_CS);		//P	�A���`���b�N�u���[�L�쓮
		cgw_rootmap(0x164, PT_TO_CS);		//P	�u���[�L�p�b�h���x�E�^�C�����x
		cgw_rootmap(0x16F, PT_TO_CS);		//P	�X���b�g������
		cgw_rootmap(0x179, PT_TO_CS);		//P	�R������E�����C�䗦
		cgw_rootmap(0x183, PT_TO_CS);		//P	�G���W����p�����x
		cgw_rootmap(0x18D, PT_TO_CS);		//P	�G���W���̏�
		cgw_rootmap(0x198, PT_TO_CS);		//P	�p���X�e�̏�
		cgw_rootmap(0x19A, PT_TO_CS);		//P	�G���W���X�^�[�^�쓮
		cgw_rootmap(0x1A2, PT_TO_CS);		//P	������h�~����
		cgw_rootmap(0x1AD, PT_TO_CS);		//P	�~�b�V�����̏�
		cgw_rootmap(0x1D3, PT_TO_CS);		//P	�T�C�h�u���[�L������
		cgw_rootmap(0x39E, PT_TO_CS);		//P	�񐶃u���[�L���d��
		cgw_rootmap(0x3A9, PT_TO_CS);		//P	�O�C���x�E�r�C���x
		cgw_rootmap(0x3B3, PT_TO_CS);		//P	�L�Q�r�C�K�X�Z�x�E���q�󕨎��Z�x
		cgw_rootmap(0x3BD, PT_TO_CS);		//P	�G���W���I�C����
		cgw_rootmap(0x3C7, PT_TO_CS);		//P	�_�Εs�ǁE�_�΃^�C�~���O�ُ�
		cgw_rootmap(0x3D4, PT_TO_CS);		//P	�G���W���X�^�[�^�̏�
		cgw_rootmap(0x3DE, PT_TO_CS);		//P	�o�b�e���[�x��
		cgw_rootmap(0x42B, PT_TO_CS);		//P	�T�C�h�u���[�L�x��
		cgw_rootmap(0x482, PT_TO_CS);		//P	�G�R�h���C�u����
		//	ECU_UNIT_CHASSIS
		cgw_rootmap(0x01A, CS_TO_AL);		//C	�u���[�L�����
		cgw_rootmap(0x02F, CS_TO_AL);		//C	�A�N�Z�������
		cgw_rootmap(0x058, CS_TO_AL);		//C	�n���h������ʒu
		cgw_rootmap(0x06D, CS_TO_AL);		//C	�V�t�g�|�W�V�����X�C�b�`
		cgw_rootmap(0x083, CS_TO_AL);		//C	�E�B���J�[���E�E�n�U�[�h�X�C�b�`
		cgw_rootmap(0x098, CS_TO_AL);		//C	�N���N�V�����X�C�b�`
		cgw_rootmap(0x1A7, CS_TO_AL);		//C	�|�W�V�����E�w�b�h���C�g�E�n�C�r�[���X�C�b�`
		cgw_rootmap(0x1B1, CS_TO_AL);		//C	�p�b�V���O�X�C�b�`
		cgw_rootmap(0x1B8, CS_TO_AL);		//C	�G���W���X�^�[�g�{�^��
		cgw_rootmap(0x1C9, CS_TO_AL);		//C	�T�C�h�u���[�L
		cgw_rootmap(0x25C, CS_TO_AL);		//C	�t�����g���C�p�[�E�Ԍ��ELOW�EHIGH�E�E�H�b�V���[�X�C�b�`
		cgw_rootmap(0x271, CS_TO_AL);		//C	���A���C�p�[�E�E�H�b�V���[�X�C�b�`
		cgw_rootmap(0x286, CS_TO_AL);		//C	�h�A���b�N�X�C�b�`�E�A�����b�N�X�C�b�`
		cgw_rootmap(0x29C, CS_TO_AL);		//C	�E�h�A�E�E�B���h�E���~�X�C�b�`
		cgw_rootmap(0x2B1, CS_TO_AL);		//C	���h�A�E�E�B���h�E���~�X�C�b�`
		//	ECU_UNIT_BODY
		cgw_rootmap(0x08D, BD_TO_CS);		//B	�E�B���J�[���E�_�����
		cgw_rootmap(0x0A2, BD_TO_CS);		//B	�N���N�V������
		cgw_rootmap(0x0B4, BD_TO_CS);		//B	�G�A�o�b�O�쓮�X�C�b�`
		cgw_rootmap(0x1BB, BD_TO_CS);		//B	�|�W�V�����E�w�b�h���C�g�E�n�C�r�[���_�����
		cgw_rootmap(0x266, BD_TO_CS);		//B	�t�����g���C�p�[�E�Ԍ��ELOW�EHIGH�E�E�H�b�V���[������
		cgw_rootmap(0x27B, BD_TO_CS);		//B	���A���C�p�[�E�E�H�b�V���[������
		cgw_rootmap(0x290, BD_TO_CS);		//B	�h�A�J�E�{�����
		cgw_rootmap(0x2A6, BD_TO_CS);		//B	�E�h�A�E�E�B���h�E�ʒu�E���~�b�g�X�C�b�`���
		cgw_rootmap(0x2BB, BD_TO_CS);		//B	���h�A�E�E�B���h�E�ʒu�E���~�b�g�X�C�b�`���
		cgw_rootmap(0x3E9, BD_TO_CS);		//B	�E�B���J�[���؂�x��
		cgw_rootmap(0x3F4, BD_TO_CS);		//B	�N���N�V�����̏�
		cgw_rootmap(0x3FF, BD_TO_CS);		//B	�|�W�V�����E�w�b�h���C�g�E�n�C�r�[�����؂�E�o���u����̏�
		cgw_rootmap(0x40A, BD_TO_CS);		//B	�t�����g���C�p�[�E�Ԍ��ELOW�EHIGH�E�E�H�b�V���[���[�^�E�|���v�̏�
		cgw_rootmap(0x415, BD_TO_CS);		//B	���A���C�p�[�E�E�H�b�V���[���[�^�E�|���v�̏�
		cgw_rootmap(0x420, BD_TO_CS);		//B	�h�A���b�N�쓮���u�̏�
		cgw_rootmap(0x436, BD_TO_CS);		//B	�E�h�A�E�E�B���h�E���[�^�̏�
		cgw_rootmap(0x441, BD_TO_CS);		//B	���h�A�E�E�B���h�E���[�^�̏�
		cgw_rootmap(0x44C, BD_TO_CS);		//B	�G�A�o�b�O�̏�
		cgw_rootmap(0x457, BD_TO_CS);		//B	�V�[�g�x���g�Z���T�[
		cgw_rootmap(0x461, BD_TO_CS);		//B	�V�[�g�x���g�x��
		cgw_rootmap(0x46C, BD_TO_CS);		//B	�{���l�b�g�J�X�C�b�`
		cgw_rootmap(0x477, BD_TO_CS);		//B	�g�����N�J�X�C�b�`
	
		//	�g�����X�|�[�gID
		cgw_rootmap(0x7DF, AL_TO_AL);		//�_�C�A�O�|�[�g��M�A�S�đ��M
		cgw_rootmap(0x7E3, EX_TO_AL);		//�_�C�A�O�|�[�g��M�A�S�đ��M
		cgw_rootmap(0x7EB, AL_TO_EX);		//�_�C�A�O�|�[�g���M�A�S�Ď�M
		cgw_rootmap(0x7E0 + ECU_UNIT_POWERTRAIN, EX_TO_PT);
		cgw_rootmap(0x7E0 + ECU_UNIT_CHASSIS,	 EX_TO_CS);
		cgw_rootmap(0x7E0 + ECU_UNIT_BODY,		 EX_TO_BD);
		cgw_rootmap(0x7E4 + ECU_UNIT_POWERTRAIN, EX_TO_PT);
		cgw_rootmap(0x7E4 + ECU_UNIT_CHASSIS,	 EX_TO_CS);
		cgw_rootmap(0x7E4 + ECU_UNIT_BODY,		 EX_TO_BD);
		cgw_rootmap(0x7E8 + ECU_UNIT_POWERTRAIN, PT_TO_EX);
		cgw_rootmap(0x7E8 + ECU_UNIT_CHASSIS,	 CS_TO_EX);
		cgw_rootmap(0x7E8 + ECU_UNIT_BODY,		 BD_TO_EX);
		cgw_rootmap(0x7EC + ECU_UNIT_POWERTRAIN, PT_TO_EX);
		cgw_rootmap(0x7EC + ECU_UNIT_CHASSIS,	 CS_TO_EX);
		cgw_rootmap(0x7EC + ECU_UNIT_BODY,		 BD_TO_EX);
#endif
	}
	else
	{	//	ECU�͌ʂɑ���M�ݒ���s��
		rout_map.ID[0x7DF].BYTE = CH_MAP_RECV;						//	�S�Ă�ECU����M����u���[�h�L���X�gID
		rout_map.ID[(0x7E0 + SELECT_ECU_UNIT)].BYTE = CH_MAP_RECV;	//	��ECU��TP��MID
		rout_map.ID[(0x7E8 + SELECT_ECU_UNIT)].BYTE = CH_MAP_SEND;	//	��ECU��TP���MID
	}
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : �����E�C�x���g�����l
//  
//  ����   : ����
//  
//  ����   : �������b�Z�[�W�E�C�x���g���b�Z�[�W�̏����l�ݒ�
//  
//  �߂�l : ����
//  
//  ���l   : �ő�255�̊Ǘ�ID���`�\
//  
//---------------------------------------------------------------------------------------
int	add_cyceve_list(int rtr, int id, int dlc, int enb, int rep, int time, int cnt);
void defset_confecu(void)
{
	//	�[��������
	memset(&conf_ecu, 0, sizeof(conf_ecu));			//	�����E�C�x���g�E�����[�g�Ǘ���`������
	conf_ecu.TOP = -1;
	/*
		�o�^�֐�
		int	add_cyceve_list(int rtr, int id, int dlc, int enb, int rep, int time, int cnt)

		����
		int rtr			�����[�g�t���[���w��		0:�f�[�^ / 1:�����[�g
		int id			CAN���b�Z�[�W�ԍ�			0x000�`0x7FF(0�`2047)
		int dlc			�f�[�^�o�C�g��				0�`8
		int enb			�������t���O				0:�֎~ / 1:����
		int rep			�����t���[���w��			0:�C�x���g / 1:����
		int time		�������Ԗ��͒x������(ms)	0�`65535
		int cnt			�x����������(ms)			0�`65535
	*/
													//P=�p���g�� / B=�{�f�B�[ / C=�V���V�[
	if(SELECT_ECU_UNIT == ECU_UNIT_POWERTRAIN)
	{
		add_cyceve_list(0, 0x024, 8, 1, 1, 20, 0);		//P	�u���[�L�o�͗�
		add_cyceve_list(0, 0x039, 8, 1, 1, 20, 1);		//P	�X���b�g���ʒu
		add_cyceve_list(0, 0x043, 8, 1, 1, 20, 2);		//P	�G���W����]���Ƒ��x
		add_cyceve_list(0, 0x062, 8, 1, 1, 20, 3);		//P	�p���X�e�o�͗�
		add_cyceve_list(0, 0x077, 8, 1, 1, 20, 4);		//P	�V�t�g�|�W�V�����ʒu
		add_cyceve_list(0, 0x146, 8, 1, 1, 50, 5);		//P	�u���[�L�I�C����
	//X	add_cyceve_list(0, 0x150, 8, 1, 1, 50, 6);		//P	�u���[�L���Ռx���E�A�C�X�o�[���x��
		add_cyceve_list(0, 0x15A, 8, 1, 1, 50, 7);		//P	�A���`���b�N�u���[�L�쓮
	//X	add_cyceve_list(0, 0x164, 8, 1, 1, 50, 8);		//P	�u���[�L�p�b�h���x�E�^�C�����x
		add_cyceve_list(0, 0x16F, 8, 1, 1, 50, 9);		//P	�X���b�g������
	//X	add_cyceve_list(0, 0x179, 8, 1, 1, 50,10);		//P	�R������E�����C�䗦
		add_cyceve_list(0, 0x183, 8, 1, 1, 50,11);		//P	�G���W����p�����x
		add_cyceve_list(0, 0x18D, 8, 1, 1, 50,12);		//P	�G���W���̏�
		add_cyceve_list(0, 0x198, 8, 1, 1, 50,13);		//P	�p���X�e�̏�
		add_cyceve_list(0, 0x19A, 8, 1, 1, 50,14);		//P	�G���W���X�^�[�^�쓮
	//X	add_cyceve_list(0, 0x1A2, 8, 1, 1, 50,15);		//P	������h�~����
	//X	add_cyceve_list(0, 0x1AD, 8, 1, 1, 50,16);		//P	�~�b�V�����̏�
		add_cyceve_list(0, 0x1D3, 8, 1, 1, 50,17);		//P	�T�C�h�u���[�L������
	//X	add_cyceve_list(0, 0x39E, 8, 1, 1,500,18);		//P	�񐶃u���[�L���d��
	//X	add_cyceve_list(0, 0x3A9, 8, 1, 1,500,19);		//P	�O�C���x�E�r�C���x
	//X	add_cyceve_list(0, 0x3B3, 8, 1, 1,500,20);		//P	�L�Q�r�C�K�X�Z�x�E���q�󕨎��Z�x
		add_cyceve_list(0, 0x3BD, 8, 1, 1,500,21);		//P	�G���W���I�C����
	//X	add_cyceve_list(0, 0x3C7, 8, 1, 1,500,22);		//P	�_�Εs�ǁE�_�΃^�C�~���O�ُ�
		add_cyceve_list(0, 0x3D4, 8, 1, 1,500,23);		//P	�G���W���X�^�[�^�̏�
		add_cyceve_list(0, 0x3DE, 8, 1, 1,500,24);		//P	�o�b�e���[�x��
	//X	add_cyceve_list(0, 0x42B, 8, 1, 1,500,25);		//P	�T�C�h�u���[�L�x��
		add_cyceve_list(0, 0x482, 8, 1, 1,500,26);		//P	�G�R�h���C�u����
	}
	else
	if(SELECT_ECU_UNIT == ECU_UNIT_CHASSIS)
	{
		add_cyceve_list(0, 0x01A, 8, 1, 1, 20, 0);		//C	�u���[�L�����
		add_cyceve_list(0, 0x02F, 8, 1, 1, 20, 1);		//C	�A�N�Z�������
		add_cyceve_list(0, 0x058, 8, 1, 1, 20, 2);		//C	�n���h������ʒu
		add_cyceve_list(0, 0x06D, 8, 1, 1, 20, 3);		//C	�V�t�g�|�W�V�����X�C�b�`
		add_cyceve_list(0, 0x083, 8, 1, 1, 20, 4);		//C	�E�B���J�[���E�E�n�U�[�h�X�C�b�`
		add_cyceve_list(0, 0x098, 8, 1, 1, 20, 5);		//C	�N���N�V�����X�C�b�`
		add_cyceve_list(0, 0x1A7, 8, 1, 1, 50, 6);		//C	�|�W�V�����E�w�b�h���C�g�E�n�C�r�[���X�C�b�`
		add_cyceve_list(0, 0x1B1, 8, 1, 1, 50, 7);		//C	�p�b�V���O�X�C�b�`
		add_cyceve_list(0, 0x1B8, 8, 1, 1, 50, 8);		//C	�G���W���X�^�[�g�{�^��
		add_cyceve_list(0, 0x1C9, 8, 1, 1, 50, 9);		//C	�T�C�h�u���[�L
		add_cyceve_list(0, 0x25C, 8, 1, 1,100,10);		//C	�t�����g���C�p�[�E�Ԍ��ELOW�EHIGH�E�E�H�b�V���[�X�C�b�`
		add_cyceve_list(0, 0x271, 8, 1, 1,100,11);		//C	���A���C�p�[�E�E�H�b�V���[�X�C�b�`
		add_cyceve_list(0, 0x286, 8, 1, 1,100,12);		//C	�h�A���b�N�X�C�b�`�E�A�����b�N�X�C�b�`
		add_cyceve_list(0, 0x29C, 8, 1, 1,100,13);		//C	�E�h�A�E�E�B���h�E���~�X�C�b�`
		add_cyceve_list(0, 0x2B1, 8, 1, 1,100,14);		//C	���h�A�E�E�B���h�E���~�X�C�b�`
	}
	else
	if(SELECT_ECU_UNIT == ECU_UNIT_BODY)
	{
		add_cyceve_list(0, 0x08D, 8, 1, 1, 20, 0);		//B	�E�B���J�[���E�_�����
		add_cyceve_list(0, 0x0A2, 8, 1, 1, 20, 1);		//B	�N���N�V������
		add_cyceve_list(0, 0x0B4, 8, 1, 1, 20, 2);		//B	�G�A�o�b�O�쓮�X�C�b�`
		add_cyceve_list(0, 0x1BB, 8, 1, 1, 50, 3);		//B	�|�W�V�����E�w�b�h���C�g�E�n�C�r�[���_�����
		add_cyceve_list(0, 0x266, 8, 1, 1,100, 4);		//B	�t�����g���C�p�[�E�Ԍ��ELOW�EHIGH�E�E�H�b�V���[������
		add_cyceve_list(0, 0x27B, 8, 1, 1,100, 5);		//B	���A���C�p�[�E�E�H�b�V���[������
		add_cyceve_list(0, 0x290, 8, 1, 1,100, 6);		//B	�h�A�J�E�{�����
		add_cyceve_list(0, 0x2A6, 8, 1, 1,100, 7);		//B	�E�h�A�E�E�B���h�E�ʒu�E���~�b�g�X�C�b�`���
		add_cyceve_list(0, 0x2BB, 8, 1, 1,100, 8);		//B	���h�A�E�E�B���h�E�ʒu�E���~�b�g�X�C�b�`���
	//X	add_cyceve_list(0, 0x3E9, 8, 1, 1,500, 9);		//B	�E�B���J�[���؂�x��
	//X	add_cyceve_list(0, 0x3F4, 8, 1, 1,500,10);		//B	�N���N�V�����̏�
	//X	add_cyceve_list(0, 0x3FF, 8, 1, 1,500,11);		//B	�|�W�V�����E�w�b�h���C�g�E�n�C�r�[�����؂�E�o���u����̏�
	//X	add_cyceve_list(0, 0x40A, 8, 1, 1,500,12);		//B	�t�����g���C�p�[�E�Ԍ��ELOW�EHIGH�E�E�H�b�V���[���[�^�E�|���v�̏�
	//X	add_cyceve_list(0, 0x415, 8, 1, 1,500,13);		//B	���A���C�p�[�E�E�H�b�V���[���[�^�E�|���v�̏�
		add_cyceve_list(0, 0x420, 8, 1, 1,500,14);		//B	�h�A���b�N�쓮���u�̏�
	//X	add_cyceve_list(0, 0x436, 8, 1, 1,500,15);		//B	�E�h�A�E�E�B���h�E���[�^�̏�
	//X	add_cyceve_list(0, 0x441, 8, 1, 1,500,16);		//B	���h�A�E�E�B���h�E���[�^�̏�
	//X	add_cyceve_list(0, 0x44C, 8, 1, 1,500,17);		//B	�G�A�o�b�O�̏�
		add_cyceve_list(0, 0x457, 8, 1, 1,500,18);		//B	�V�[�g�x���g�Z���T�[
		add_cyceve_list(0, 0x461, 8, 1, 1,500,19);		//B	�V�[�g�x���g�x��
		add_cyceve_list(0, 0x46C, 8, 1, 1,500,20);		//B	�{���l�b�g�J�X�C�b�`
		add_cyceve_list(0, 0x477, 8, 1, 1,500,21);		//B	�g�����N�J�X�C�b�`
	}
	else
	if(SELECT_ECU_UNIT == ECU_UNIT_CGW)
	{
//		add_cyceve_list(0, 0x7FD, 8, 1, 1, 1000, 0);	//C	���b�ʒm
	}
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : �O�����o�͒�`�����l
//  
//  ����   : ����
//  
//  ����   : �O�����o�͂̏����l�ݒ�
//  
//  �߂�l : ����
//  
//  ���l   : �f�t�H���g�ł́A���o�͂�LF74���s��ECU�����ڎQ�Ƃ���|�[�g�͖���
//  
//---------------------------------------------------------------------------------------
#define		DEF_IOPAT_XX	0	/*	�p�^�[������`	*/
//	�`�F�b�N���X�g�ω��p�^�[����`
const unsigned char	DEF_IOPAT_00[24] = {
//	0     1     2     3     4     5     6     7     8     9     10    11    12    13    14    15    16    17    18    19    20    21    22    23
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	};
const unsigned char	DEF_IOPAT_01[24] = {
//	0     1     2     3     4     5     6     7     8     9     10    11    12    13    14    15    16    17    18    19    20    21    22    23
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	};
const unsigned char	DEF_IOPAT_02[24] = {
//	0     1     2     3     4     5     6     7     8     9     10    11    12    13    14    15    16    17    18    19    20    21    22    23
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	};
const unsigned char	DEF_IOPAT_03[24] = {
//	0     1     2     3     4     5     6     7     8     9     10    11    12    13    14    15    16    17    18    19    20    21    22    23
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	};
const unsigned char	DEF_IOPAT_04[24] = {
//	0     1     2     3     4     5     6     7     8     9     10    11    12    13    14    15    16    17    18    19    20    21    22    23
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	};
const unsigned char	DEF_IOPAT_05[24] = {
//	0     1     2     3     4     5     6     7     8     9     10    11    12    13    14    15    16    17    18    19    20    21    22    23
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	};
const unsigned char	DEF_IOPAT_06[24] = {
//	0     1     2     3     4     5     6     7     8     9     10    11    12    13    14    15    16    17    18    19    20    21    22    23
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	};
const unsigned char	DEF_IOPAT_07[24] = {
//	0     1     2     3     4     5     6     7     8     9     10    11    12    13    14    15    16    17    18    19    20    21    22    23
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	};

int add_extern_io(int id, int mode, int neg, int size, int bpos, int dlc, int nom, int msk, unsigned char *pat);
void defset_extlist(void)
{
	memset(ext_list, 0, sizeof(ext_list));	//	ECU���o�̓`�F�b�N���X�g������
	ext_list_count = 0;						//	�`�F�b�N���X�g�����Z�b�g
	/*
		�o�^�֐�
		int add_extern_io(int id, int mode, int neg, int size, int bpos, int dlc, int nom, int msk, unsigned char *pat)

		����
		int				id		�t���[��ID�ԍ�		0�`2047(000�`7FF)
		int				mode	I/O�������[�h�ݒ�	0:���� / 1:DI / 2:DO / 3:AI / 4:AO / 5:2bit�l / 6:3bit�l / 7:4bit�l
		int				neg		�f�[�^���]�w��		0:�ʏ� / 1:���]
		int				size	�A�N�Z�X�T�C�Y		0:BIT / 1�`7:nBYTE
		int				bpos	�o�C�g�ʒu			0�`7
		int				dlc		�f�[�^�o�C�g��		0�`8
		int				nom		�K�p�|�[�g�ԍ�		0�`33
		int				msk		�}�X�N�p�^�[��		00�`FF
		unsigned char	*pat	�p�^�[���f�[�^		24bytes	(0=�g�p���Ȃ�)
	*/
#if	0
	if(SELECT_ECU_UNIT == ECU_UNIT_POWERTRAIN)
	{
		//�p���g��
		add_extern_io(0x055, 3, 0, 2, 0, 8, 23, 0xFF, DEF_IOPAT_XX);		//P	�A�N�Z���^���x�֘A			word		A/D�A�N�Z���y�_���l
		add_extern_io(0x075, 3, 0, 2, 0, 8, 24, 0xFF, DEF_IOPAT_XX);		//P	�n���h������֘A(�Ɋp)		word		A/D�X�e�A�����O�l
		add_extern_io(0x07F, 3, 0, 2, 0, 8, 25, 0xFF, DEF_IOPAT_XX);		//P	�u���[�L�y�_�����			word		A/D�u���[�L�y�_���l
		add_extern_io(0x0B4, 1, 0, 2, 0, 8,  0, 0xFF, DEF_IOPAT_XX);		//P	�n���h������֘A(�p���X�e)	0.bit0		�X�e�[�^�X
		add_extern_io(0x0F7, 1, 0, 1, 0, 8,  1, 0xFF, DEF_IOPAT_XX);		//P	�G���W���n����				0.bit6,1,0	�X�^�[�^�X�C�b�`
		add_extern_io(0x121, 6, 0, 2, 0, 8,  2, 0xFF, DEF_IOPAT_XX);		//P	�G���W���n����				word.pat[4]	�X�e�[�^�X
		add_extern_io(0x139, 6, 0, 2, 0, 8,  6, 0xFF, DEF_IOPAT_XX);		//P	�G���W���n����				word.pat[4]	�X�e�[�^�X
		add_extern_io(0x155, 1, 0, 1, 0, 8, 10, 0xFF, DEF_IOPAT_XX);		//P	�V�t�g���o�[�ʒu���
		add_extern_io(0x167, 1, 0, 1, 0, 8, 11, 0xFF, DEF_IOPAT_XX);		//P	�A�N�Z���^�G���W����]�֘A
		add_extern_io(0x196, 1, 0, 1, 0, 8, 12, 0xFF, DEF_IOPAT_XX);		//P	�L�[�֘A
		add_extern_io(0x1A0, 1, 0, 1, 0, 8, 13, 0xFF, DEF_IOPAT_XX);		//P	�G���W���֘A
		add_extern_io(0x1AE, 1, 0, 1, 0, 8, 14, 0xFF, DEF_IOPAT_XX);		//P	�L�[�֘A
		add_extern_io(0x207, 1, 0, 1, 0, 8, 15, 0xFF, DEF_IOPAT_XX);		//P	�V�[�g�x���g�x�������	�T�C�h�u���[�L���		�h�A�J���(���E���L)
		add_extern_io(0x2DF, 1, 0, 1, 0, 8, 16, 0xFF, DEF_IOPAT_XX);		//P	�A�N�Z���^�u���[�L�֘A
	}
	else
	if(SELECT_ECU_UNIT == ECU_UNIT_CHASSIS)
	{
		//�V���V�[
		add_extern_io(0x0EF, 1, 0, 1, 0, 8,  0, 0xFF, DEF_IOPAT_XX);		//C	�󒲃C���p�l�{�^������		0.bit3		�X�e�[�^�X
		add_extern_io(0x108, 5, 0, 1, 0, 8,  1, 0xFF, DEF_IOPAT_XX);		//C	�󒲕��ʒ������x��			byte.pat[8]	�C���p�l����0�`6
		add_extern_io(0x113, 1, 0, 1, 0, 8,  4, 0xFF, DEF_IOPAT_XX);		//C	���ADEF ON/OFF				0.bit7		�C���p�l����
		add_extern_io(0x113, 1, 1, 1, 0, 8,  5, 0xFF, DEF_IOPAT_XX);		//C	A/C ON/OFF					1.bit3		�C���p�l����
		add_extern_io(0x13F, 1, 0, 1, 0, 8,  6, 0xFF, DEF_IOPAT_XX);		//C	�u���[�L�����v,�y�_��ON/OFF���
		add_extern_io(0x1E1, 1, 0, 1, 0, 8,  7, 0xFF, DEF_IOPAT_XX);		//C	�������[�^�[�֘A
		add_extern_io(0x21F, 1, 0, 1, 0, 8,  8, 0xFF, DEF_IOPAT_XX);		//C	�h�A���b�N�֘A
		add_extern_io(0x261, 1, 0, 1, 0, 8,  9, 0xFF, DEF_IOPAT_XX);		//C	�h�A���b�N�֘A
		add_extern_io(0x31C, 1, 0, 1, 0, 8, 10, 0xFF, DEF_IOPAT_XX);		//C	�V�t�g���o�[(P���)
		add_extern_io(0x3FB, 1, 0, 1, 0, 8, 11, 0xFF, DEF_IOPAT_XX);		//C	�t�@������
		add_extern_io(0x433, 1, 0, 1, 0, 8, 12, 0xFF, DEF_IOPAT_XX);		//C	�G���W����]���i��j
		add_extern_io(0x434, 1, 0, 1, 0, 8, 13, 0xFF, DEF_IOPAT_XX);		//C	�ԑ� (��)
	}
	else
	if(SELECT_ECU_UNIT == ECU_UNIT_BODY)
	{
		//�{�f�B�[
		add_extern_io(0x0C7, 1, 0, 3, 0, 8,  0, 0xFF, DEF_IOPAT_XX);		//B	�t�����gDEF ON/OFF			0.bit7		�C���p�l����
		add_extern_io(0x0C7, 1, 0, 3, 0, 8,  1, 0xFF, DEF_IOPAT_XX);		//B	��AUTO/OFF�X�C�b�`		1.bit7		�C���p�l����
		add_extern_io(0x185, 1, 0, 1, 0, 8,  2, 0xFF, DEF_IOPAT_XX);		//B	�h�A���b�N�֘A
		add_extern_io(0x224, 1, 0, 1, 0, 8,  3, 0xFF, DEF_IOPAT_XX);		//B	���C�g�_�����
		add_extern_io(0x27B, 1, 0, 1, 0, 8,  4, 0xFF, DEF_IOPAT_XX);		//B	�h�A���b�N�֘A
		add_extern_io(0x2B1, 1, 0, 1, 0, 8,  5, 0xFF, DEF_IOPAT_XX);		//B	�E�B���h�E������(���E���L)	�h�A���b�N�֘A
		add_extern_io(0x322, 1, 0, 1, 0, 8,  6, 0xFF, DEF_IOPAT_XX);		//B	�h�A���b�N	�n�U�[�h	�e�[�������v,�⏕�����v	�w�b�h���C�g(Lo)	�w�b�h���C�g(Hi)	OFF
		add_extern_io(0x441, 1, 0, 1, 0, 8,  7, 0xFF, DEF_IOPAT_XX);		//B	�E�B���J�[����(�͋[)
		add_extern_io(0x449, 1, 0, 1, 0, 8,  8, 0xFF, DEF_IOPAT_XX);		//B	�^�[���V�O�i���_��(�͋[)
		add_extern_io(0x452, 1, 0, 1, 0, 8,  9, 0xFF, DEF_IOPAT_XX);		//B	�z�[��
		add_extern_io(0x501, 1, 0, 1, 0, 8, 10, 0xFF, DEF_IOPAT_XX);		//B	�t�����g���C�p�[	�Ԍ��ELOW�EH/MST�E�E�H�b�V���[
		add_extern_io(0x503, 1, 0, 1, 0, 8, 14, 0xFF, DEF_IOPAT_XX);		//B	���A���C�p�[	���A�E�H�b�V���[
	}
	else
	if(SELECT_ECU_UNIT == ECU_UNIT_CGW)
	{
		add_extern_io(0x7FF, 1, 1, 1, 0, 8,  0, 0xFF, DEF_IOPAT_XX);		//G	
	}
#endif
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : �ʐM�o�R�O�����o�͒�`�����l
//  
//  ����   : ����
//  
//  ����   : �O�����o�͂̏����l�ݒ�
//  
//  �߂�l : ����
//  
//  ���l   : �f�t�H���g�ł́A���o�͂�LF74���s��ECU�����ڎQ�Ƃ���|�[�g�͖���
//  
//---------------------------------------------------------------------------------------
void defset_extlist_ex(void)
{
	memset(ext_list, 0, sizeof(ext_list));	//	ECU���o�̓`�F�b�N���X�g������
	ext_list_count = 0;						//	�`�F�b�N���X�g�����Z�b�g
	/*
		�o�^�֐�
		int add_extern_io(int id, int mode, int neg, int size, int bpos, int dlc, int nom, int msk, unsigned char *pat)

		����
		int				id		�t���[��ID�ԍ�		0�`2047(000�`7FF)
		int				mode	I/O�������[�h�ݒ�	���� 0:bit / 1:byte / 2:word / 3:long  �o�� 4:bit / 5:byte / 6:word / 7:long
		int				neg		�f�[�^���]�w��		[0]����
		int				size	�A�N�Z�X�T�C�Y		[0]����
		int				bpos	�o�C�g�ʒu			0�`7�i�ʏ�=0�j
		int				dlc		�f�[�^�o�C�g��		[0]����
		int				nom		�K�p�|�[�g�ԍ�		0�`63
		int				msk		�}�X�N�p�^�[��		00�`FF
		unsigned char	*pat	�p�^�[���f�[�^		[0]=�g�p���Ȃ�
	*/
#if	1
	if(SELECT_ECU_UNIT == ECU_UNIT_POWERTRAIN)
	{	//	�p���g���̓V���V�[����̏��̂ݏo�͂���
		add_extern_io(0x01A, 6, 0, 0, 0, 0, 0, 1, 0);		//C	�u���[�L�����
		add_extern_io(0x02F, 6, 0, 0, 0, 0, 1, 1, 0);		//C	�A�N�Z�������
		add_extern_io(0x058, 6, 0, 0, 0, 0, 2, 1, 0);		//C	�n���h������ʒu
		add_extern_io(0x06D, 5, 0, 0, 0, 0, 3, 1, 0);		//C	�V�t�g�|�W�V�����X�C�b�`
//		add_extern_io(0x083, 5, 0, 0, 0, 0, 4, 1, 0);		//C	�E�B���J�[���E�E�n�U�[�h�X�C�b�`
//		add_extern_io(0x098, 5, 0, 0, 0, 0, 5, 1, 0);		//C	�N���N�V�����X�C�b�`
//		add_extern_io(0x1A7, 5, 0, 0, 0, 0, 6, 1, 0);		//C	�|�W�V�����E�w�b�h���C�g�E�n�C�r�[���X�C�b�`
//		add_extern_io(0x1B1, 5, 0, 0, 0, 0, 7, 1, 0);		//C	�p�b�V���O�X�C�b�`
		add_extern_io(0x1B8, 5, 0, 0, 0, 0, 8, 1, 0);		//C	�G���W���X�^�[�g�{�^��
		add_extern_io(0x1C9, 5, 0, 0, 0, 0, 9, 1, 0);		//C	�T�C�h�u���[�L
//		add_extern_io(0x25C, 6, 0, 0, 0, 0,10, 1, 0);		//C	�t�����g���C�p�[�E�Ԍ��ELOW�EHIGH�E�E�H�b�V���[�X�C�b�`�E�Ԍ��^�C�}�[
//		add_extern_io(0x271, 5, 0, 0, 0, 0,11, 1, 0);		//C	���A���C�p�[�E�E�H�b�V���[�X�C�b�`
//		add_extern_io(0x286, 5, 0, 0, 0, 0,12, 1, 0);		//C	�h�A���b�N�X�C�b�`�E�A�����b�N�X�C�b�`
//		add_extern_io(0x29C, 5, 0, 0, 0, 0,13, 1, 0);		//C	�E�h�A�E�E�B���h�E���~�X�C�b�`
//		add_extern_io(0x2B1, 5, 0, 0, 0, 0,14, 1, 0);		//C	���h�A�E�E�B���h�E���~�X�C�b�`
		//	�p���g���̓��͏��
		add_extern_io(0x024, 2, 0, 0, 0, 0,15, 1, 0);		//P	�u���[�L�o�͗�
		add_extern_io(0x039, 2, 0, 0, 0, 0,16, 1, 0);		//P	�X���b�g���ʒu
		add_extern_io(0x043, 3, 0, 0, 0, 0,17, 1, 0);		//P	�G���W����]��/���x
		add_extern_io(0x062, 3, 0, 0, 0, 0,18, 1, 0);		//P	�p���X�e�o�͗�/�g���N
		add_extern_io(0x077, 1, 0, 0, 0, 0,19, 1, 0);		//P	�V�t�g�|�W�V�����ʒu
		add_extern_io(0x146, 1, 0, 0, 0, 0,20, 1, 0);		//P	�u���[�L�I�C����
	//X	add_extern_io(0x150, 1, 0, 0, 0, 0,21, 1, 0);		//P	�u���[�L���Ռx���E�A�C�X�o�[���x��
		add_extern_io(0x15A, 1, 0, 0, 0, 0,22, 1, 0);		//P	�A���`���b�N�u���[�L�쓮
	//X	add_extern_io(0x164, 2, 0, 0, 0, 0,23, 1, 0);		//P	�u���[�L�p�b�h���x�E�^�C�����x
		add_extern_io(0x16F, 2, 0, 0, 0, 0,24, 1, 0);		//P	�X���b�g������
	//X	add_extern_io(0x179, 2, 0, 0, 0, 0,25, 1, 0);		//P	�R������E�����C�䗦
		add_extern_io(0x183, 1, 0, 0, 0, 0,26, 1, 0);		//P	�G���W����p�����x
		add_extern_io(0x18D, 1, 0, 0, 0, 0,27, 1, 0);		//P	�G���W���̏�
		add_extern_io(0x198, 1, 0, 0, 0, 0,28, 1, 0);		//P	�p���X�e�̏�
		add_extern_io(0x19A, 1, 0, 0, 0, 0,29, 1, 0);		//P	�G���W���X�^�[�^�쓮
	//X	add_extern_io(0x1A2, 1, 0, 0, 0, 0,30, 1, 0);		//P	������h�~����
	//X	add_extern_io(0x1AD, 1, 0, 0, 0, 0,31, 1, 0);		//P	�~�b�V�����̏�
		add_extern_io(0x1D3, 1, 0, 0, 0, 0,32, 1, 0);		//P	�T�C�h�u���[�L������
	//X	add_extern_io(0x39E, 1, 0, 0, 0, 0,33, 1, 0);		//P	�񐶃u���[�L���d��
	//X	add_extern_io(0x3A9, 2, 0, 0, 0, 0,34, 1, 0);		//P	�O�C���x�E�r�C���x
	//X	add_extern_io(0x3B3, 2, 0, 0, 0, 0,35, 1, 0);		//P	�L�Q�r�C�K�X�Z�x�E���q�󕨎��Z�x
		add_extern_io(0x3BD, 1, 0, 0, 0, 0,36, 1, 0);		//P	�G���W���I�C����
	//X	add_extern_io(0x3C7, 1, 0, 0, 0, 0,37, 1, 0);		//P	�_�Εs�ǁE�_�΃^�C�~���O�ُ�
		add_extern_io(0x3D4, 1, 0, 0, 0, 0,38, 1, 0);		//P	�R���c��
		add_extern_io(0x3DE, 1, 0, 0, 0, 0,39, 1, 0);		//P	�o�b�e���[�x��
	//X	add_extern_io(0x42B, 1, 0, 0, 0, 0,40, 1, 0);		//P	�T�C�h�u���[�L�x��
		add_extern_io(0x482, 1, 0, 0, 0, 0,41, 1, 0);		//P	�G�R�h���C�u����
	}
	else
	if(SELECT_ECU_UNIT == ECU_UNIT_CHASSIS)
	{	//	�V���V�[�̓���
		add_extern_io(0x01A, 2, 0, 0, 0, 0, 0, 1, 0);		//C	�u���[�L�����
		add_extern_io(0x02F, 2, 0, 0, 0, 0, 1, 1, 0);		//C	�A�N�Z�������
		add_extern_io(0x058, 2, 0, 0, 0, 0, 2, 1, 0);		//C	�n���h������ʒu
		add_extern_io(0x06D, 1, 0, 0, 0, 0, 3, 1, 0);		//C	�V�t�g�|�W�V�����X�C�b�`
		add_extern_io(0x083, 1, 0, 0, 0, 0, 4, 1, 0);		//C	�E�B���J�[���E�E�n�U�[�h�X�C�b�`
		add_extern_io(0x098, 1, 0, 0, 0, 0, 5, 1, 0);		//C	�N���N�V�����X�C�b�`
		add_extern_io(0x1A7, 1, 0, 0, 0, 0, 6, 1, 0);		//C	�|�W�V�����E�w�b�h���C�g�E�n�C�r�[���X�C�b�`
		add_extern_io(0x1B1, 1, 0, 0, 0, 0, 7, 1, 0);		//C	�p�b�V���O�X�C�b�`
		add_extern_io(0x1B8, 1, 0, 0, 0, 0, 8, 1, 0);		//C	�G���W���X�^�[�g�{�^��
		add_extern_io(0x1C9, 1, 0, 0, 0, 0, 9, 1, 0);		//C	�T�C�h�u���[�L
		add_extern_io(0x25C, 2, 0, 0, 0, 0,10, 1, 0);		//C	�t�����g���C�p�[�E�Ԍ��ELOW�EHIGH�E�E�H�b�V���[�X�C�b�`�E�Ԍ��^�C�}�[
		add_extern_io(0x271, 1, 0, 0, 0, 0,11, 1, 0);		//C	���A���C�p�[�E�E�H�b�V���[�X�C�b�`
		add_extern_io(0x286, 1, 0, 0, 0, 0,12, 1, 0);		//C	�h�A���b�N�X�C�b�`�E�A�����b�N�X�C�b�`
		add_extern_io(0x29C, 1, 0, 0, 0, 0,13, 1, 0);		//C	�E�h�A�E�E�B���h�E���~�X�C�b�`
		add_extern_io(0x2B1, 1, 0, 0, 0, 0,14, 1, 0);		//C	���h�A�E�E�B���h�E���~�X�C�b�`
		//	�V���V�[�̓C���p�l�ɓ]�����邽�߂̏���S�ďo�͂���
		//	�p���g�����̏o��
//		add_extern_io(0x024, 6, 0, 0, 0, 0,15, 1, 0);		//P	�u���[�L�o�͗�
//		add_extern_io(0x039, 6, 0, 0, 0, 0,16, 1, 0);		//P	�X���b�g���ʒu
		add_extern_io(0x043, 7, 0, 0, 0, 0,17, 1, 0);		//P	�G���W����]��/���x
		add_extern_io(0x062, 7, 0, 0, 0, 0,18, 1, 0);		//P	�p���X�e�o�͗�/�g���N
		add_extern_io(0x077, 5, 0, 0, 0, 0,19, 1, 0);		//P	�V�t�g�|�W�V�����ʒu
		add_extern_io(0x146, 5, 0, 0, 0, 0,20, 1, 0);		//P	�u���[�L�I�C����
	//X	add_extern_io(0x150, 5, 0, 0, 0, 0,21, 1, 0);		//P	�u���[�L���Ռx���E�A�C�X�o�[���x��
//		add_extern_io(0x15A, 5, 0, 0, 0, 0,22, 1, 0);		//P	�A���`���b�N�u���[�L�쓮
	//X	add_extern_io(0x164, 6, 0, 0, 0, 0,23, 1, 0);		//P	�u���[�L�p�b�h���x�E�^�C�����x
//		add_extern_io(0x16F, 6, 0, 0, 0, 0,24, 1, 0);		//P	�X���b�g������
	//X	add_extern_io(0x179, 6, 0, 0, 0, 0,25, 1, 0);		//P	�R������E�����C�䗦
		add_extern_io(0x183, 5, 0, 0, 0, 0,26, 1, 0);		//P	�G���W����p�����x
		add_extern_io(0x18D, 5, 0, 0, 0, 0,27, 1, 0);		//P	�G���W���̏�
//		add_extern_io(0x198, 5, 0, 0, 0, 0,28, 1, 0);		//P	�p���X�e�̏�
		add_extern_io(0x19A, 5, 0, 0, 0, 0,29, 1, 0);		//P	�G���W���X�^�[�^�쓮
	//X	add_extern_io(0x1A2, 5, 0, 0, 0, 0,30, 1, 0);		//P	������h�~����
	//X	add_extern_io(0x1AD, 5, 0, 0, 0, 0,31, 1, 0);		//P	�~�b�V�����̏�
		add_extern_io(0x1D3, 5, 0, 0, 0, 0,32, 1, 0);		//P	�T�C�h�u���[�L������
	//X	add_extern_io(0x39E, 5, 0, 0, 0, 0,33, 1, 0);		//P	�񐶃u���[�L���d��
	//X	add_extern_io(0x3A9, 6, 0, 0, 0, 0,34, 1, 0);		//P	�O�C���x�E�r�C���x
	//X	add_extern_io(0x3B3, 6, 0, 0, 0, 0,35, 1, 0);		//P	�L�Q�r�C�K�X�Z�x�E���q�󕨎��Z�x
		add_extern_io(0x3BD, 5, 0, 0, 0, 0,36, 1, 0);		//P	�G���W���I�C����
	//X	add_extern_io(0x3C7, 5, 0, 0, 0, 0,37, 1, 0);		//P	�_�Εs�ǁE�_�΃^�C�~���O�ُ�
		add_extern_io(0x3D4, 5, 0, 0, 0, 0,38, 1, 0);		//P	�R���c��
		add_extern_io(0x3DE, 5, 0, 0, 0, 0,39, 1, 0);		//P	�o�b�e���[�x��
	//X	add_extern_io(0x42B, 5, 0, 0, 0, 0,40, 1, 0);		//P	�T�C�h�u���[�L�x��
		add_extern_io(0x482, 5, 0, 0, 0, 0,41, 1, 0);		//P	�G�R�h���C�u����
		//	�{�f�B�[���̏o��
		add_extern_io(0x08D, 5, 0, 0, 0, 0,42, 1, 0);		//B	�E�B���J�[���E�_�����
		add_extern_io(0x0A2, 5, 0, 0, 0, 0,43, 1, 0);		//B	�N���N�V������
		add_extern_io(0x0B4, 5, 0, 0, 0, 0,44, 1, 0);		//B	�G�A�o�b�O�쓮�X�C�b�`
		add_extern_io(0x1BB, 5, 0, 0, 0, 0,45, 1, 0);		//B	�|�W�V�����E�w�b�h���C�g�E�n�C�r�[���_�����
		add_extern_io(0x266, 6, 0, 0, 0, 0,46, 1, 0);		//B	�t�����g���C�p�[�E�Ԍ��ELOW�EHIGH�E�E�H�b�V���[�����ԁE�Ԍ��^�C�}�[
		add_extern_io(0x27B, 5, 0, 0, 0, 0,47, 1, 0);		//B	���A���C�p�[�E�E�H�b�V���[������
		add_extern_io(0x290, 5, 0, 0, 0, 0,48, 1, 0);		//B	�h�A�J�E�{�����
//		add_extern_io(0x2A6, 6, 0, 0, 0, 0,49, 1, 0);		//B	�E�h�A�E�E�B���h�E�ʒu�E���~�b�g�X�C�b�`���
//		add_extern_io(0x2BB, 6, 0, 0, 0, 0,50, 1, 0);		//B	���h�A�E�E�B���h�E�ʒu�E���~�b�g�X�C�b�`���
	//X	add_extern_io(0x3E9, 5, 0, 0, 0, 0,51, 1, 0);		//B	�E�B���J�[���؂�x��
	//X	add_extern_io(0x3F4, 5, 0, 0, 0, 0,52, 1, 0);		//B	�N���N�V�����̏�
	//X	add_extern_io(0x3FF, 5, 0, 0, 0, 0,53, 1, 0);		//B	�|�W�V�����E�w�b�h���C�g�E�n�C�r�[�����؂�E�o���u����̏�
	//X	add_extern_io(0x40A, 5, 0, 0, 0, 0,54, 1, 0);		//B	�t�����g���C�p�[�E�Ԍ��ELOW�EHIGH�E�E�H�b�V���[���[�^�E�|���v�̏�
	//X	add_extern_io(0x415, 5, 0, 0, 0, 0,55, 1, 0);		//B	���A���C�p�[�E�E�H�b�V���[���[�^�E�|���v�̏�
//		add_extern_io(0x420, 5, 0, 0, 0, 0,56, 1, 0);		//B	�h�A���b�N�쓮���u�̏�
	//X	add_extern_io(0x436, 5, 0, 0, 0, 0,57, 1, 0);		//B	�E�h�A�E�E�B���h�E���[�^�̏�
	//X	add_extern_io(0x441, 5, 0, 0, 0, 0,58, 1, 0);		//B	���h�A�E�E�B���h�E���[�^�̏�
	//X	add_extern_io(0x44C, 5, 0, 0, 0, 0,59, 1, 0);		//B	�G�A�o�b�O�̏�
//		add_extern_io(0x457, 5, 0, 0, 0, 0,60, 1, 0);		//B	�V�[�g�x���g�Z���T�[
		add_extern_io(0x461, 5, 0, 0, 0, 0,61, 1, 0);		//B	�V�[�g�x���g�x��
		add_extern_io(0x46C, 5, 0, 0, 0, 0,62, 1, 0);		//B	�{���l�b�g�J�X�C�b�`
		add_extern_io(0x477, 5, 0, 0, 0, 0,63, 1, 0);		//B	�g�����N�J�X�C�b�`
	}
	else
	if(SELECT_ECU_UNIT == ECU_UNIT_BODY)
	{	//	�{�f�B�[�̓V���V�[����̏��̂ݏo�͂���
		add_extern_io(0x01A, 6, 0, 0, 0, 0, 0, 1, 0);		//C	�u���[�L�����
//		add_extern_io(0x02F, 6, 0, 0, 0, 0, 1, 1, 0);		//C	�A�N�Z�������
//		add_extern_io(0x058, 6, 0, 0, 0, 0, 2, 1, 0);		//C	�n���h������ʒu
//		add_extern_io(0x06D, 5, 0, 0, 0, 0, 3, 1, 0);		//C	�V�t�g�|�W�V�����X�C�b�`
		add_extern_io(0x083, 5, 0, 0, 0, 0, 4, 1, 0);		//C	�E�B���J�[���E�E�n�U�[�h�X�C�b�`
		add_extern_io(0x098, 5, 0, 0, 0, 0, 5, 1, 0);		//C	�N���N�V�����X�C�b�`
		add_extern_io(0x1A7, 5, 0, 0, 0, 0, 6, 1, 0);		//C	�|�W�V�����E�w�b�h���C�g�E�n�C�r�[���X�C�b�`
		add_extern_io(0x1B1, 5, 0, 0, 0, 0, 7, 1, 0);		//C	�p�b�V���O�X�C�b�`
//		add_extern_io(0x1B8, 5, 0, 0, 0, 0, 8, 1, 0);		//C	�G���W���X�^�[�g�{�^��
//		add_extern_io(0x1C9, 5, 0, 0, 0, 0, 9, 1, 0);		//C	�T�C�h�u���[�L
		add_extern_io(0x25C, 6, 0, 0, 0, 0,10, 1, 0);		//C	�t�����g���C�p�[�E�Ԍ��ELOW�EHIGH�E�E�H�b�V���[�X�C�b�`�E�Ԍ��^�C�}�[
		add_extern_io(0x271, 5, 0, 0, 0, 0,11, 1, 0);		//C	���A���C�p�[�E�E�H�b�V���[�X�C�b�`
		add_extern_io(0x286, 5, 0, 0, 0, 0,12, 1, 0);		//C	�h�A���b�N�X�C�b�`�E�A�����b�N�X�C�b�`
		add_extern_io(0x29C, 5, 0, 0, 0, 0,13, 1, 0);		//C	�E�h�A�E�E�B���h�E���~�X�C�b�`
		add_extern_io(0x2B1, 5, 0, 0, 0, 0,14, 1, 0);		//C	���h�A�E�E�B���h�E���~�X�C�b�`
		//	�{�f�B�[�̓���
		add_extern_io(0x08D, 1, 0, 0, 0, 0,42, 1, 0);		//B	�E�B���J�[���E�_�����
		add_extern_io(0x0A2, 1, 0, 0, 0, 0,43, 1, 0);		//B	�N���N�V������
		add_extern_io(0x0B4, 1, 0, 0, 0, 0,44, 1, 0);		//B	�G�A�o�b�O�쓮�X�C�b�`
		add_extern_io(0x1BB, 1, 0, 0, 0, 0,45, 1, 0);		//B	�|�W�V�����E�w�b�h���C�g�E�n�C�r�[���_�����
		add_extern_io(0x266, 2, 0, 0, 0, 0,46, 1, 0);		//B	�t�����g���C�p�[�E�Ԍ��ELOW�EHIGH�E�E�H�b�V���[�����ԁE�Ԍ��^�C�}�[
		add_extern_io(0x27B, 1, 0, 0, 0, 0,47, 1, 0);		//B	���A���C�p�[�E�E�H�b�V���[������
		add_extern_io(0x290, 1, 0, 0, 0, 0,48, 1, 0);		//B	�h�A�J�E�{�����
		add_extern_io(0x2A6, 2, 0, 0, 0, 0,49, 1, 0);		//B	�E�h�A�E�E�B���h�E�ʒu�E���~�b�g�X�C�b�`���
		add_extern_io(0x2BB, 2, 0, 0, 0, 0,50, 1, 0);		//B	���h�A�E�E�B���h�E�ʒu�E���~�b�g�X�C�b�`���
	//X	add_extern_io(0x3E9, 1, 0, 0, 0, 0,51, 1, 0);		//B	�E�B���J�[���؂�x��
	//X	add_extern_io(0x3F4, 1, 0, 0, 0, 0,52, 1, 0);		//B	�N���N�V�����̏�
	//X	add_extern_io(0x3FF, 1, 0, 0, 0, 0,53, 1, 0);		//B	�|�W�V�����E�w�b�h���C�g�E�n�C�r�[�����؂�E�o���u����̏�
	//X	add_extern_io(0x40A, 1, 0, 0, 0, 0,54, 1, 0);		//B	�t�����g���C�p�[�E�Ԍ��ELOW�EHIGH�E�E�H�b�V���[���[�^�E�|���v�̏�
	//X	add_extern_io(0x415, 1, 0, 0, 0, 0,55, 1, 0);		//B	���A���C�p�[�E�E�H�b�V���[���[�^�E�|���v�̏�
		add_extern_io(0x420, 1, 0, 0, 0, 0,56, 1, 0);		//B	�h�A���b�N�쓮���u�̏�
	//X	add_extern_io(0x436, 1, 0, 0, 0, 0,57, 1, 0);		//B	�E�h�A�E�E�B���h�E���[�^�̏�
	//X	add_extern_io(0x441, 1, 0, 0, 0, 0,58, 1, 0);		//B	���h�A�E�E�B���h�E���[�^�̏�
	//X	add_extern_io(0x44C, 1, 0, 0, 0, 0,59, 1, 0);		//B	�G�A�o�b�O�̏�
		add_extern_io(0x457, 1, 0, 0, 0, 0,60, 1, 0);		//B	�V�[�g�x���g�Z���T�[
		add_extern_io(0x461, 1, 0, 0, 0, 0,61, 1, 0);		//B	�V�[�g�x���g�x��
		add_extern_io(0x46C, 1, 0, 0, 0, 0,62, 1, 0);		//B	�{���l�b�g�J�X�C�b�`
		add_extern_io(0x477, 1, 0, 0, 0, 0,63, 1, 0);		//B	�g�����N�J�X�C�b�`
	}
	else
	if(SELECT_ECU_UNIT == ECU_UNIT_CGW)
	{
		//	����
	}
#endif
}

//---------------------------------------------------------------------------------------
//  
//  �@�\   : CAN�t���[���f�[�^�o�b�t�@�����l
//  
//  ����   : ����
//  
//  ����   : �t���[���f�[�^�̏����l�ݒ�
//  
//  �߂�l : ����
//  
//  ���l   : 
//  
//---------------------------------------------------------------------------------------
void set_frame_data(int id, int dlc, unsigned char *dat);

void defset_framedat(void)
{
	int				i;
	DEF_FRAME_DATA	*vp;
	
	for(i = 0; ; i++)
	{
		vp = &DEF_FRAME_BUFFER[i];
		if(vp->ID < 0) break;
		set_frame_data(vp->ID, 8, vp->DAT);
//		can_chainge[vp->ID] = 0;
	}
//	can_chainge_cnt = 0;
}

#endif		/*__ECU_DEFAULT_SETTING__*/
