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
// $RCSfile: ecu.h,v $
// $Revision: 1.00 $
// $Date: 2016/12/15 14:14:48 $
// 
// Copyright (c) 2016 LandF Corporation.
//
// History:
//


#ifndef		__CAN_ECU_CGW__
#define		__CAN_ECU_CGW__

//	LFY_RX63N����g�p����CAN�|�[�g1�̎����p�ɐ�������Ȃ牺�L��L���ɂ���
//#define		__LFY_RX63N__

//	LFY-RX63N��USB�R���\�[���L��
#ifdef		__LFY_RX63N__
//#define		__USE_LFY_USB__
#endif

//---------------------------------------------------------------------------------------
//	ECU���j�b�g�R�[�h�̎擾
//---------------------------------------------------------------------------------------
#ifdef		__LFY_RX63N__
//	LFY������
#define		SELECT_ECU_UNIT			0	/*	�����@�\�I��	0:�p���g�� / 1:�V���V�[ / 2:�{�f�B�[	*/
#define		ECU_UNIT_POWERTRAIN		0
#define		ECU_UNIT_CHASSIS		1
#define		ECU_UNIT_BODY			2
#define		ECU_UNIT_CGW			7
#else
//	CAN2ECU��
#define		DPSW_ROM_BOOT			(PORTF.PIDR.BIT.B5)					/*	DipSwitch S1-8		*/
#define		DPSW_BOOTCOPY			(PORTJ.PIDR.BIT.B5)					/*	DipSwitch S1-7		*/
#define		SELECT_ECU_UNIT			((~PORT5.PIDR.BYTE >> 1) & 0x07)	/*	DipSwitch S1-6,5,4	*/
#define		ECU_UNIT_POWERTRAIN		0
#define		ECU_UNIT_CHASSIS		1
#define		ECU_UNIT_BODY			2
#define		ECU_UNIT_CGW			7
#endif

//---------------------------------------------------------------------------------------
//	CAN���b�Z�[�W�o�b�t�@��`
//---------------------------------------------------------------------------------------
#define		CAN_ID_MAX			0x800
#define		CAN_ID_MASK			0x7FF
#define		MESSAGE_MAX			0x100
#define		MESSAGE_MSK			0x0FF
#define		MESSAGE_END			0x1FF
#define		MESSAGE_BOXS		3

//	�|�[�g���ݒ�
#define		CAN_CH_MAX			4	/*	CAN2ECU���CAN-4ch	*/
#define		CAN_TEST_LFY_CH		1	/*	LFY-RX63N��CAN1�̂ݗL��	*/

#define		DLC_MASK			0x0F

#define		CAN_DATA_FRAME		0
#define		CAN_REMOTE_FRAME	1

//	MBOX�I���w��
#define		MBOX_POINT_1		0x200
#define		MBOX_POINT_2		0x400

//	�O�����o�̓`�F�b�N���X�g�ő吔
#define		ECU_EXT_MAX			64
//	�O��I/O�Ǘ���
#define		EX_IO_MAX			64

//	���M�҂����X�g��ID�D��x�ŕ��בւ�
#define		SORT_TXWAITLIST_ENABLE

//	���[�e�B���O�}�b�v��`�\����(E2DATA�ɕۑ�)
typedef	struct	__routing_map__
{
	union	{
		unsigned char	BYTE;
		struct {
			unsigned char	RE3	:	1;	//	CAN-Port3 ��M	1=����/0=�֎~
			unsigned char	RE2	:	1;	//	CAN-Port2 ��M	1=����/0=�֎~
			unsigned char	RE1	:	1;	//	CAN-Port1 ��M	1=����/0=�֎~
			unsigned char	RE0	:	1;	//	CAN-Port0 ��M	1=����/0=�֎~
			unsigned char	TE3	:	1;	//	CAN-Port3 ���M	1=����/0=�֎~
			unsigned char	TE2	:	1;	//	CAN-Port2 ���M	1=����/0=�֎~
			unsigned char	TE1	:	1;	//	CAN-Port1 ���M	1=����/0=�֎~
			unsigned char	TE0	:	1;	//	CAN-Port0 ���M	1=����/0=�֎~
		}	BIT;
	}	ID[CAN_ID_MAX];
}	ECU_ROUT_MAP;

//	CGW�̃|�[�g���[�e�B���O�}�b�v����r�b�g��`
#define		EX_R_BIT	0x80	/*	�O��CAN	��M	*/
#define		CS_R_BIT	0x40	/*	�V���V�[��M	*/
#define		BD_R_BIT	0x20	/*	�{�f�B�[��M	*/
#define		PT_R_BIT	0x10	/*	�p���g����M	*/
#define		EX_W_BIT	0x08	/*	�O��CAN	���M	*/
#define		CS_W_BIT	0x04	/*	�V���V�[���M	*/
#define		BD_W_BIT	0x02	/*	�{�f�B�[���M	*/
#define		PT_W_BIT	0x01	/*	�p���g�����M	*/

#define		CS_TO_PT	(CS_R_BIT|PT_W_BIT)						/*	�V���V�[����p���g��	*/
#define		CS_TO_BD	(CS_R_BIT|BD_W_BIT)						/*	�V���V�[����{�f�B�[	*/
#define		PT_TO_CS	(PT_R_BIT|CS_W_BIT)						/*	�p���g������V���V�[	*/
#define		BD_TO_CS	(BD_R_BIT|CS_W_BIT)						/*	�{�f�B�[����V���V�[	*/
#define		PT_TO_BD	(PT_R_BIT|BD_W_BIT)						/*	�p���g������{�f�B�[	*/
#define		BD_TO_PT	(BD_R_BIT|PT_W_BIT)						/*	�{�f�B�[����p���g��	*/
#define		CS_TO_AL	(CS_R_BIT|PT_W_BIT|BD_W_BIT)			/*	�V���V�[����S��		*/
#define		PT_TO_AL	(PT_R_BIT|CS_W_BIT|BD_W_BIT)			/*	�p���g������S��		*/
#define		BD_TO_AL	(BD_R_BIT|PT_W_BIT|CS_W_BIT)			/*	�{�f�B�[����S��		*/
#define		EX_TO_AL	(EX_R_BIT|PT_W_BIT|CS_W_BIT|BD_W_BIT)	/*	�O��CAN	����S��		*/
#define		EX_TO_PT	(EX_R_BIT|PT_W_BIT)						/*	�O��CAN	����p���g��	*/
#define		EX_TO_CS	(EX_R_BIT|CS_W_BIT)						/*	�O��CAN	����{�f�B�[	*/
#define		EX_TO_BD	(EX_R_BIT|BD_W_BIT)						/*	�O��CAN	����V���V�[	*/
#define		PT_TO_EX	(PT_R_BIT|EX_W_BIT)						/*	�p���g������O��CAN		*/
#define		BD_TO_EX	(BD_R_BIT|EX_W_BIT)						/*	�{�f�B�[����O��CAN		*/
#define		CS_TO_EX	(CS_R_BIT|EX_W_BIT)						/*	�V���V�[����O��CAN		*/
#define		AL_TO_EX	(PT_R_BIT|BD_R_BIT|CS_R_BIT|EX_W_BIT)	/*	�S�Ă���O��CAN			*/
#define		AL_TO_AL	(EX_R_BIT|PT_R_BIT|BD_R_BIT|CS_R_BIT|EX_W_BIT|PT_W_BIT|BD_W_BIT|CS_W_BIT)	/*	�S�ē]��	*/

//	CAN-ID���p��
typedef	union	__can_id_form__
{
	unsigned long	LONG;
	struct	{
		unsigned short	H;
		unsigned short	L;
	}	WORD;
	struct {
		unsigned char	HH;
		unsigned char	HL;
		unsigned char	LH;
		unsigned char	LL;
	}	BYTE;
	struct {
		unsigned long	IDE	:	1;	//	[0]
		unsigned long	RTR	:	1;	//	�����[�g���M�v���r�b�g(0=�f�[�^�t���[��/1=�����[�g�t���[��)
		unsigned long		:	1;	//	[0]
		unsigned long	SID	:	11;	//	�W��ID�r�b�g
		unsigned long		:	2;	//	[0]
		unsigned long	DLC	:	4;	//	�t���[���f�[�^���w��(0�`8)
		unsigned long	ENB	:	1;	//	�����C�l�[�u���t���O(0=����/1=�L��)
		unsigned long	REP	:	1;	//	���s�[�g�t���O(0=�C�x���g/1=����)
		unsigned long		:	1;	//	[0]
		unsigned long	NXT	:	9;	//	���̎����E�C�x���g�ԍ�(���s���̂ݗL���F0�`255�^�p�������F256�`511)
	}	BIT;
}	CAN_ID_FORM;

//	�����E�C�x���g��`�\����(E2DATA�ɕۑ�)
typedef	struct	__cycle_event_str__
{
	CAN_ID_FORM	ID;		//	�Ώ�ID�ԍ�
	union	{
		long	LONG;
		struct	{
			short	TIME;	//	�������Ԗ��̓C�x���g�x������(ms)
			short	CNT;	//	�J�E���^���̓C�x���g�x����������(ms)
		}	WORD;
	}	TIMER;
}	ECU_CYC_EVE;

//	�����E�C�x���g�o�^���
typedef	struct	__cycle_event_info__
{
	int				WP;					//	�������݃|�C���^
	int				TOP;				//	�擪�|�C���^
	int				CNT;				//	�o�^��
	ECU_CYC_EVE		LIST[MESSAGE_MAX];	//	�����E�C�x���g���
}	CYCLE_EVENTS;

//	���b�Z�[�W�{�b�N�X�g�pID�͈͐ݒ�
typedef	struct	__mbox_select_id__
{
	struct	{
		unsigned short	MB1;	//	���b�Z�[�W�{�b�N�X0�ɓK�p����ID�͈� 0�`MB1
		unsigned short	MB2;	//	���b�Z�[�W�{�b�N�X0�ɓK�p����ID�͈� MB1�`MB2
	}	CH[CAN_CH_MAX];
}	MBOX_SELECT_ID;

//	���M�҂��t���[���Ǘ��\����
typedef	struct	__send_wait_frame__
{
	CAN_ID_FORM		ID;		//	���b�Z�[�WID�ԍ�(0�`2047)
	union	{
		unsigned long	LONG[2];
		unsigned short	WORD[4];
		unsigned char	BYTE[8];
	}	FD;					//	�t���[���f�[�^
}	SEND_WAIT_FLAME;

//	���M�҂��o�b�t�@��`�\����
typedef	struct	__send_wait_buffer__
{
	struct	{
		int				WP;					//	�o�^�|�C���^(0�`MESSAGE_MAX-1)
		int				TOP;				//	�擪�|�C���^(0�`MESSAGE_MAX-1)
		int				CNT;				//	���M�҂����b�Z�[�W��(0�`MESSAGE_MAX)
		SEND_WAIT_FLAME	MSG[MESSAGE_MAX];	//	�ێ����b�Z�[�W�o�b�t�@(�ő�256��)
	}	BOX[MESSAGE_BOXS];
}	SEND_WAIT_BUF;

//	CAN�t���[���f�[�^���p��
typedef	union	__can_frame_data__
{
	unsigned long	LONG[2];
	unsigned short	WORD[4];
	unsigned char	BYTE[8];
}	CAN_DATA_BYTE;

//	�t���[���f�[�^�o�b�t�@��`�\����
typedef	struct	__can_frame_buffer__
{
	CAN_DATA_BYTE	ID[CAN_ID_MAX];
}	CAN_FRAME_BUF;

//	CAN�o�X���b�Z�[�W�{�b�N�X��`�\����
typedef	struct	__can_message_box__
{
	union {
		unsigned long LONG;
		struct {
			unsigned short H;
			unsigned short L;
		} WORD;
		struct {
			unsigned char HH;
			unsigned char HL;
			unsigned char LH;
			unsigned char LL;
		} BYTE;
		struct {
			unsigned long IDE	:	1;		//	[0]
			unsigned long RTR	:	1;		//	�����[�g���M�v���r�b�g(0=�f�[�^�t���[��/1=�����[�g�t���[��)
			unsigned long 		:	1;		//	[0]
			unsigned long SID	:	11;		//	�W��ID�r�b�g
			unsigned long EID	:	18;		//	[0]
		} BIT;
	} ID;
	unsigned short DLC;						//	�f�[�^��(0�`8)
	unsigned char  DATA[8];					//	�f�[�^�{��(8byte)
	unsigned short TS;						//	�^�C���X�^���v(�ǂݏo���̂�)
}	CAN_MBOX;

//	�O��I/O��`�\����	4+4+24=32byte
typedef struct	__externul_io_str__
{
	int				SID;			//	ID�ԍ�				-1:���� / 000�`7FF:�L��
	union {
		unsigned long	LONG;
		struct	{
			unsigned long			:	1;	//	�\��				0
			unsigned long	MODE	:	3;	//	I/O�������[�h�ݒ�	0:���� / 1:DI / 2:DO / 3:AI / 4:AO / 5:2bit�l / 6:3bit�l / 7:4bit�l
			unsigned long	NEG		:	1;	//	�f�[�^���]�w��		�l�K�e�B�u����
			unsigned long	SIZE	:	3;	//	�A�N�Z�X�T�C�Y		0:BIT / 1�`7:nBYTE
			unsigned long	BPOS	:	4;	//	�o�C�g�ʒu			0�`7
			unsigned long	DLC		:	4;	//	�f�[�^�o�C�g��		0�`8
			unsigned long	NOM		:	8;	//	�|�[�g�ԍ�			0�`33
			unsigned long	MSK		:	8;	//	�}�X�N				00�`FF
		}	BIT;
	}	PORT;
	unsigned char	FRMCNT;			//	�t���[���J�E���^
	unsigned char	PAT[23];		//	�p�^�[���f�[�^	size>0�̎��A�����r�b�g�Q�Ƃɂ��R�s�[���ƂȂ�
}	EXTERNUL_IO;

//	CAN-ID -> EX-I/O-ID �ϊ��e�[�u��
extern	unsigned char	can_to_exio[CAN_ID_MAX];	//	CAN-ID�̎����f�[�^���O��I/O�Ǘ��ԍ��ƂȂ�B00�`3F=�K�p�AFF=�Ή�����

//	�|�C���^�}���`�A�N�Z�X���p�̒�`
typedef	union __pointer_multi_access__
{
	unsigned long	LONG;
	void			*VP;
	signed char		*SB;
	unsigned char	*UB;
	signed short	*SW;
	unsigned short	*UW;
	signed int		*SI;
	unsigned int	*UI;
	signed long		*SL;
	unsigned long	*UL;
	ECU_CYC_EVE		*CYE;
	ECU_ROUT_MAP	*MAP;
	CYCLE_EVENTS	*CONF;
	CYCLE_EVENTS	*WAIT;
	SEND_WAIT_BUF	*SMSG;
	CAN_FRAME_BUF	*CAN;
	MBOX_SELECT_ID	*MBOX;
	EXTERNUL_IO		*EXL;
}	POINTER_MULTI_ACCESS;

//	E2DATA�t���b�V���ۑ��ϐ�
//	���[�e�B���O�}�b�v
extern	ECU_ROUT_MAP	rout_map;	//	�}�b�v�ϐ�
//	��`�ێ��o�b�t�@
extern	CYCLE_EVENTS	conf_ecu;	//	�����E�C�x���g�E�����[�g�Ǘ���`�ϐ�
//	ECU���o�̓`�F�b�N���X�g	32*64=2048byte
extern	EXTERNUL_IO		ext_list[ECU_EXT_MAX];
extern	int				ext_list_count;		//	�O�����o�͏����̓o�^��

//	RAM��̕ϐ�
//	�^�C���A�b�v�҂��o�b�t�@
extern	CYCLE_EVENTS	wait_tup;	//	�����E�C�x���g�҂��ϐ�
//	���b�Z�[�W�{�b�N�X���̑��M�҂��o�b�t�@
extern	SEND_WAIT_BUF	send_msg[CAN_CH_MAX];
//	CAN�f�[�^�o�b�t�@�ϐ�
extern	CAN_FRAME_BUF	can_buf;
extern	CAN_FRAME_BUF	can_random_mask;

/*
//	CAN�f�[�^�ω��t���O
extern	unsigned char	can_chainge[CAN_ID_MAX];
extern	int				can_chainge_cnt;
*/
//	���b�Z�[�W�{�b�N�X�͈�
extern	MBOX_SELECT_ID	mbox_sel;
//	���b�Z�[�W�{�b�N�XCAN�t���[�����M�o�b�t�@�ςݏグ����
extern	void add_mbox_frame(int ch, int dlc, int rtr, int id);

//	���v�����[�h�t���O
extern	int	repro_mode;			//	0=�ʏ탂�[�h / 1=���v�����[�h

//	��M�T�u�o�b�t�@
#define		RX_MB_BUF_MAX	32
typedef	struct	__rx_mb_buffer__
{
	CAN_MBOX	MB[RX_MB_BUF_MAX];
	int			WP;
	int			RP;
}	RX_MB_BUF;

extern	RX_MB_BUF	rxmb_buf[3];	//	��M�T�u�o�b�t�@

//extern	int				ds_conect_active;	//	�h���C�r���O�V�~�����[�^�ڑ��t���O

//	LED���j�^�����OID�ݒ�
extern	int				led_monit_id;				//	���j�^�[ID
extern	unsigned char	led_monit_ch;				//	���j�^�[CH�r�b�g�Z�b�g
extern	int				led_monit_first;			//	�ŒZ����
extern	int				led_monit_slow;				//	�Œ�����
extern	int				led_monit_time;				//	���ώ���
extern	int				led_monit_count;			//	���ω���
extern	int				led_monit_sample;			//	�T���v����

//	E2DATA�t���b�V����`
#define		ADDRESS_OF_ROOTMAP		0x00100000		/*	���[�g�}�b�v	2048byte	0x00100000�`0x001007FF	*/
#define		ADDRESS_OF_CYCEVE		0x00100800		/*	�����E�C�x���g	2048byte	0x00100800�`0x00100FFF	*/
#define		ADDRESS_OF_IOLIST		0x00101000		/*	���o�̓`�F�b�N	 272byte	0x00101000�`0x001017FF	*/

//---------------------------------------------------------------------------------------
//  �@�\   : ���ǂ��Ǘ�����ID�����`�F�b�N����
//  ����   : int id		����ID�ԍ�
//  ����   : ��`�o�b�t�@����w��ID�ԍ�����������
//  �߂�l : int		0�ȏ�F��`�o�b�t�@�ԍ� / -1�F�ΏۊOID
//---------------------------------------------------------------------------------------
extern	int search_target_id(int id);
//---------------------------------------------------------------------------------------
//  �@�\   : �����C�x���g�f�[�^�ǉ�
//  ����   : int rtr			�����[�g�t���[���w��	0/1
//           int id				CAN���b�Z�[�W�ԍ�		0�`2047
//           int dlc			�f�[�^�o�C�g��			0�`8
//           int enb			�������t���O			0/1
//           int rep			�����t���[���w��		0/1
//           int time			�������Ԗ��͒x������(ms)0�`65535
//           int cnt			�x����������(ms)		0�`65535
//  ����   : �����E�C�x���g�̓o�^
//  �߂�l : �o�b�t�@�̒ǉ��ʒu
//---------------------------------------------------------------------------------------
extern	int	add_cyceve_list(int rtr, int id, int dlc, int enb, int rep, int time, int cnt);
//---------------------------------------------------------------------------------------
//  �@�\   : �����C�x���g�f�[�^�̓r���ǉ�
//  ����   : int mi		�o�b�t�@�ԍ�		0�`255
//  ����   : �����E�C�x���g�̓o�^�ςݏ���A������
//  �߂�l : �o�b�t�@�̒ǉ��ʒu
//---------------------------------------------------------------------------------------
extern	void insert_cyceve_list(int mi);
//---------------------------------------------------------------------------------------
//  �@�\   : �����C�x���g�f�[�^�폜
//---------------------------------------------------------------------------------------
extern	void delete_cyceve_list(int id);
//---------------------------------------------------------------------------------------
//  �@�\   : �f�[�^�X�V�C�x���g��������
//  ����   : int id		�ω���������ID�ԍ�
//         : int tp		�ǉ��҂�����(ms)
//  ����   : CAN�ȊO�̊O���@�킩��̃f�[�^�X�V�v���ɂ���ČĂяo�����
//           �C�x���g�����Ώۂ�ID�̓^�C���A�b�v�҂��s��ɒǉ�����
//  �߂�l : �����͓o�^���s / 0�ȏ�͑�������(ms)
//---------------------------------------------------------------------------------------
extern	int can_id_event(int id, int tp);
//---------------------------------------------------------------------------------------
//  �@�\   : �����C�x���g�^�C���A�b�v�҂��f�[�^�폜
//---------------------------------------------------------------------------------------
extern	void delete_waiting_list(int id);
//----------------------------------------------------------------------------------------
//	BootCopy ����
//----------------------------------------------------------------------------------------
extern	int bootcopy(void);
extern	int bootclear(void);
//----------------------------------------------------------------------------------------
//	ECU�^�p�f�[�^�̈ꊇ�ۑ�
//----------------------------------------------------------------------------------------
extern	int ecu_data_write(void);
//----------------------------------------------------------------------------------------
//	ECU�^�p�f�[�^�̈ꊇ����
//----------------------------------------------------------------------------------------
extern	int ecu_data_erase(void);
//----------------------------------------------------------------------------------------
//	ECU�^�p�f�[�^�̏������ݏ�Ԋm�F
//----------------------------------------------------------------------------------------
extern	int ecu_data_check(void);

//----------------------------------------------------------------------------------------
//	�󑗐M�Ԏ����v���֐�
//----------------------------------------------------------------------------------------
extern	void monit_timeover(void);

#endif		/*__CAN_ECU_CGW__*/

