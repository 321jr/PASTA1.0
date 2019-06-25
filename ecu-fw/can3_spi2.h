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
//	LFY-RX63N1	SCI-I/F �ʐM
//
//----------------------------------------------------------------------------------------
//	�J������
//
//	2016/02/10	�R�[�f�B���O�J�n�i�k�j
//
//----------------------------------------------------------------------------------------
//	T.Tachibana
//	��L&F
//________________________________________________________________________________________
//

//#include	"can3_spi2.h"

#ifndef __CAN2ECU_CAN3RSPI2_IF__
#define __CAN2ECU_CAN3RSPI2_IF__

#include	"ecu.h"			/*	ECU ���ʒ�`			*/

/*
	�|�[�g�ݒ�

			Port		SCI			I2C			SPI			�K�p
	----------------------------------------------------------------------------
	RSPI2	PD2									MISOC		<RSPI>		CAN3
			PD1									MOSIC		<RSPI>		CAN3
			PD3									RSPCKC		<RSPI>		CAN3
			PD4									SSLC0		<RSPI>		CAN3
			PD0									IRQ0		<- CINT		CAN3
			PD6												<- CRX0BF	CAN3
			PD7												<- CRX1BF	CAN3
			P90												-> CTX0RTS	CAN3
			P91												-> CTX1RTS	CAN3
			P92												-> CTX2RTS	CAN3
			P93												-> CRSET	CAN3
			P07												<- CSOF		CAN3


*/

#define		RSPI2_ACTIVATE

//�ԐڌĂяo���̃v���g�^�C�v(�����P��)
typedef	void 			(*CAN3_PROC_CALL)(void *);

#ifdef	RSPI2_ACTIVATE

//________________________________________________________________________________________
//
//	DTC��`		(RSPI��8MBPS�ŉ^�p���邽�߂�DTC�����K�{)
//________________________________________________________________________________________
//

#define		DTC_VECT_TOP	0x0003E000
#define		DTC_REQUEST_TOP	0x0003D000

extern	unsigned long	*dtc_table;

typedef	union	__dtc_fulladdressmode__	{
	unsigned long	LONG[4];
	unsigned short	WORD[8];
	unsigned char	BYTE[16];
	struct	{
		union	{
			unsigned long	LONG;
			struct	{	//	���[�h���W�X�^
				//	MRA
				unsigned long	MD		:	2;		//	DTC�]�����[�h�I���r�b�g
													//	b7 b6
													//	0 0�F�m�[�}���]�����[�h
													//	0 1�F���s�[�g�]�����[�h
													//	1 0�F�u���b�N�]�����[�h
													//	1 1�F�ݒ肵�Ȃ��ł�������
				unsigned long	SZ		:	2;		//	DTC�f�[�^�g�����X�t�@�T�C�Y�r�b�g
													//	DTC�f�[�^�g�����X�t�@�T�C�Y�r�b�g
													//	b5 b4
													//	0 0�F8�r�b�g�i�o�C�g�j�]��
													//	0 1�F16�r�b�g�i���[�h�j�]��
													//	1 0�F32�r�b�g�i�����O���[�h�j�]��
													//	1 1�F�ݒ肵�Ȃ��ł�������
				unsigned long	SM		:	2;		//	�]�����A�h���X�A�h���b�V���O���[�h�r�b�g
													//	b3 b2
													//	0 0�FSAR���W�X�^�̓A�h���X�Œ�iSAR���W�X�^�̃��C�g�o�b�N�̓X�L�b�v����܂��j
													//	0 1�FSAR���W�X�^�̓A�h���X�Œ�iSAR���W�X�^�̃��C�g�o�b�N�̓X�L�b�v����܂��j
													//	1 0�F�]����SAR���W�X�^���C���N�������g�iSZ[1:0]�r�b�g���g00b�h�̂Ƃ�+1 �A�g01b�h�̂Ƃ�+2�A�g10b�h�̂Ƃ�+4�j
													//	1 1�F�]����SAR���W�X�^���f�N�������g�iSZ[1:0]�r�b�g���g00b�h�̂Ƃ�-1 �A�g01b�h�̂Ƃ�-2�A�g10b�h�̂Ƃ�-4�j
				unsigned long			:	2;		//	[0]
				//	MRB
				unsigned long	CHNE	:	1;		//	DTC�`�F�[���]�����r�b�g
													//	0�F�`�F�[���]���֎~
													//	1�F�`�F�[���]������
				unsigned long	CHNS	:	1;		//	DTC�`�F�[���]���I���r�b�g
													//	0�F�A�����ă`�F�[���]�����s��
													//	1�F�]���J�E���^��1��0�A�܂���1��CRAH�ƂȂ����Ƃ��A�`�F�[���]�����s��
				unsigned long	DISEL	:	1;		//	DTC���荞�ݑI���r�b�g
													//	0�F�w�肳�ꂽ�f�[�^�]���I�����ACPU�ւ̊��荞�݂�����
													//	1�FDTC�f�[�^�]���̂��тɁACPU�ւ̊��荞�݂�����
				unsigned long	DTS		:	1;		//	DTC�]�����[�h�I���r�b�g
													//	0�F�]���悪���s�[�g�̈�܂��̓u���b�N�̈�
													//	1�F�]���������s�[�g�̈�܂��̓u���b�N�̈�
				unsigned long	DM		:	2;		//	�]����A�h���X�A�h���b�V���O���[�h�r�b�g
													//	b3 b2
													//	0 0�FDAR���W�X�^�̓A�h���X�Œ�iDAR���W�X�^�̃��C�g�o�b�N�̓X�L�b�v����܂��j
													//	0 1�FDAR���W�X�^�̓A�h���X�Œ�iDAR���W�X�^�̃��C�g�o�b�N�̓X�L�b�v����܂��j
													//	1 0�F�]����ADAR���W�X�^���C���N�������g�iMAR.SZ[1:0]�r�b�g���g00b�h�̂Ƃ�+1�A�g01b�h�̂Ƃ�+2�A�g10b�h�̂Ƃ�+4�j
													//	1 1�F�]����DAR���W�X�^���f�N�������g�iMAR.SZ[1:0]�r�b�g���g00b�h�̂Ƃ�-1�A�g01b�h�̂Ƃ�-2�A�g10b�h�̂Ƃ�-4�j
				unsigned long			:	2;		//	[0]
				//	�\��(0x0000���C�g)
				unsigned long	RES		:	16;		//	[0]
			}	BIT;
		}	MR;
		unsigned long	SAR;	//	�]�����A�h���X
		unsigned long	DAR;	//	�]����A�h���X
		union	{
			unsigned long	LONG;
			struct	{	//	�m�[�}���]�����[�h
				unsigned long	A		:	16;		//	DTC �]���J�E���g���W�X�^A
				unsigned long	B		:	16;		//	DTC �]���J�E���g���W�X�^B�i�u���b�N�]�����[�h���̃u���b�N�]���񐔂��w�肷�郌�W�X�^�j
			}	NOR;
			struct	{	//	���s�[�g�]�����[�h
				unsigned long	AH		:	8;		//	DTC �]���񐔕ێ�
				unsigned long	AL		:	8;		//	DTC �]���񐔃J�E���^
				unsigned long	B		:	16;		//	DTC �]���J�E���g���W�X�^B�i�u���b�N�]�����[�h���̃u���b�N�]���񐔂��w�肷�郌�W�X�^�j
			}	REP;
			struct	{	//	�u���b�N�]�����[�h
				unsigned long	AH		:	8;		//	DTC �u���b�N�T�C�Y�ێ�
				unsigned long	AL		:	8;		//	DTC �u���b�N�T�C�Y�񐔃J�E���^
				unsigned long	B		:	16;		//	DTC �]���J�E���g���W�X�^B�i�u���b�N�]�����[�h���̃u���b�N�]���񐔂��w�肷�郌�W�X�^�j
			}	BLK;
		}	CR;
	}	REG;
}	DTC_FAMD_STR;

#define		CAN3_REQUEST_DTC_MAX	64

//	����M���N�G�X�g�\���̒�`
typedef	struct	__rspi_dtc_request__	{	//	64byte*128=4096byte = 0x1000 (3D000�`3DFFF)
	unsigned short	DAT[8];		//	16	���M��M�o�b�t�@
	DTC_FAMD_STR	DTCTX;		//	16	DTC���M�v���\����
	DTC_FAMD_STR	DTCRX;		//	16	DTC��M�v���\����
	int				TXL;		//	4	���M�o�C�g��
	int				RXL;		//	4	��M�o�C�g��
	unsigned short	*RXP;		//	4	�L����M�f�[�^�|�C���^
	void			*CALL;		//	4	����M�������Ăяo����
}	RSPI_DTC_REQ;

//	����M���N�G�X�g�`�F�[���\���̒�`
typedef	struct	__rspi_dtc_request_list__	{
	RSPI_DTC_REQ	REQ[CAN3_REQUEST_DTC_MAX];	//	���N�G�X�g�`�F�[��
}	RSPI_REQUESTS;

//________________________________________________________________________________________
//
//	rspi2_init
//----------------------------------------------------------------------------------------
//	�@�\����
//		RSPI2������
//					Port		SCI			I2C			SPI			�K�p
//			----------------------------------------------------------------------------
//			RSPI2	PD2									MISOC		<RSPI>		CAN3
//					PD1									MOSIC		<RSPI>		CAN3
//					PD3									RSPCKC		<RSPI>		CAN3
//					PD4									SSLC0		<RSPI>		CAN3
//					PD0									IRQ0		<- CINT		CAN3
//					PD6									IRQ6		<- CRX0BF	CAN3
//					PD7									IRQ7		<- CRX1BF	CAN3
//					P90												-> CTX0RTS	CAN3
//					P91												-> CTX1RTS	CAN3
//					P92												-> CTX2RTS	CAN3
//					P93												-> CRSET	CAN3
//					P07												<- CSOF		CAN3
//	����
//		speed		�ʐM���x	100,000�`10,000,000(�ő�10Mbps)
//	�߂�
//		����
//________________________________________________________________________________________
//
extern	void			rspi2_init(long bps);		//	RSPI2������

//________________________________________________________________________________________
//
//	MCP2515�����`
//________________________________________________________________________________________
//
//	�|�[�g��`
#define		CAN3_RESET_PORT		PORT9.PODR.BIT.B3	/*	�`�b�v���Z�b�g�M��			*/
#define		CAN3_SPI_CS_PORT	PORTD.PODR.BIT.B4	/*	�`�b�v�Z���N�g�M��			*/
#define		CAN3_MCPINT_PORT	PORTD.PIDR.BIT.B0	/*	���荞�� 1��0=���x��		*/
#define		CAN3_RX0BF_PORT		PORTD.PIDR.BIT.B6	/*	��M���� 1��0=���x��		*/
#define		CAN3_RX1BF_PORT		PORTD.PIDR.BIT.B7	/*	��M���� 1��0=���x��		*/
#define		CAN3_TX0RTS_PORT	PORT9.PODR.BIT.B0	/*	���M�v�� 1��0=�G�b�W		*/
#define		CAN3_TX1RTS_PORT	PORT9.PODR.BIT.B1	/*	���M�v�� 1��0=�G�b�W		*/
#define		CAN3_TX2RTS_PORT	PORT9.PODR.BIT.B2	/*	���M�v�� 1��0=�G�b�W		*/

//	MCP2515�R�[�h��`
//#define		MCP2515CMD_RESET	0xC0		/*	MCP2515 �������Z�b�g					*/
#define		MCP2515CMD_READ		0x03		/*	�I���A�h���X���珇�Ƀ��W�X�^��ǂݏo��	*/
//#define		MCP2515CMD_RXB0_RH	0x90		/*	��M�o�b�t�@0SIDH�ǂݏo��	0x61		*/
//#define		MCP2515CMD_RXB0_RD	0x92		/*	��M�o�b�t�@0D0�ǂݏo��		0x66		*/
//#define		MCP2515CMD_RXB1_RH	0x94		/*	��M�o�b�t�@1SIDH�ǂݏo��	0x71		*/
//#define		MCP2515CMD_RXB1_RD	0x96		/*	��M�o�b�t�@1D0�ǂݏo��		0x76		*/
#define		MCP2515CMD_WRITE	0x02		/*	�I���A�h���X���珇�Ƀ��W�X�^�֏�������	*/
//#define		MCP2515CMD_TXB0_WH	0x40		/*	���M�o�b�t�@0SIDH��������	0x31		*/
//#define		MCP2515CMD_TXB0_WD	0x41		/*	���M�o�b�t�@0D0��������		0x36		*/
//#define		MCP2515CMD_TXB1_WH	0x42		/*	���M�o�b�t�@1SIDH��������	0x41		*/
//#define		MCP2515CMD_TXB1_WD	0x43		/*	���M�o�b�t�@1D0��������		0x46		*/
//#define		MCP2515CMD_TXB2_WH	0x44		/*	���M�o�b�t�@2SIDH��������	0x51		*/
//#define		MCP2515CMD_TXB2_WD	0x45		/*	���M�o�b�t�@2D0��������		0x56		*/
//#define		MCP2515CMD_RTS		0x80		/*	���M�o�b�t�@0�̑��M�v��		0x81�`0x87	*/
//#define		MCP2515CMD_RTS0		0x81		/*	���M�o�b�t�@0�̑��M�v��					*/
//#define		MCP2515CMD_RTS1		0x82		/*	���M�o�b�t�@1�̑��M�v��					*/
//#define		MCP2515CMD_RTS2		0x84		/*	���M�o�b�t�@2�̑��M�v��					*/
#define		MCP2515CMD_STATUS	0xA0		/*	��ԃr�b�g�ǂݏo��						*/
//#define		MCP2515CMD_RXSTS	0xB0		/*	��M���b�Z�[�W�t�B���^��v�E�^�C�v�擾	*/
#define		MCP2515CMD_BITX		0x05		/*	���背�W�X�^�̃r�b�g�ύX				*/

//----------------------------------------------------------------------------------------
//	MCP2515�����A�h���X��`
enum	__mcp2515_ram_address__	{
	//	�t�B���^
	MCP2515AD_RXF0SIDH=0x00,MCP2515AD_RXF0SIDL,MCP2515AD_RXF0EID8,MCP2515AD_RXF0EID0,
	MCP2515AD_RXF1SIDH=0x04,MCP2515AD_RXF1SIDL,MCP2515AD_RXF1EID8,MCP2515AD_RXF1EID0,
	MCP2515AD_RXF2SIDH=0x08,MCP2515AD_RXF2SIDL,MCP2515AD_RXF2EID8,MCP2515AD_RXF2EID0,
	//	�|�[�g����
	MCP2515AD_BFPCTRL=0x0C,
	MCP2515AD_TXRTSCTRL=0x0D,
	//	�X�e�[�^�X�ǂݎ���p
	MCP2515AD_CANSTAT = 0x0E,
	MCP2515AD_CANCTRL = 0x0F,
	//	�t�B���^
	MCP2515AD_RXF3SIDH=0x10,MCP2515AD_RXF3SIDL,MCP2515AD_RXF3EID8,MCP2515AD_RXF3EID0,
	MCP2515AD_RXF4SIDH=0x14,MCP2515AD_RXF4SIDL,MCP2515AD_RXF4EID8,MCP2515AD_RXF4EID0,
	MCP2515AD_RXF5SIDH=0x18,MCP2515AD_RXF5SIDL,MCP2515AD_RXF5EID8,MCP2515AD_RXF5EID0,
	//	�G���[�J�E���^
	MCP2515AD_TEC=0x1C,MCP2515AD_REC,MCP2515AD_CANSTAT1,MCP2515AD_CANCTRL1,
	//	�}�X�N
	MCP2515AD_RXM0SIDH=0x20,MCP2515AD_RXM0SIDL,MCP2515AD_RXM0EID8,MCP2515AD_RXM0EID0,
	MCP2515AD_RXM1SIDH=0x24,MCP2515AD_RXM1SIDL,MCP2515AD_RXM1EID8,MCP2515AD_RXM1EID0,
	//	�R���t�B�M�����[�V����1
	MCP2515AD_CNFIG3=0x28,MCP2515AD_CNFIG2,MCP2515AD_CNFIG1,
	//	���荞�݋���
	MCP2515AD_CANINTE=0x2B,MCP2515AD_CANINTF,MCP2515AD_EFLG,MCP2515AD_CANSTAT2,MCP2515AD_CANCTRL2,
	//	���M�o�b�t�@
	MCP2515AD_TXB0CTRL=0x30,MCP2515AD_TXB0SIDH,MCP2515AD_TXB0SIDL,MCP2515AD_TXB0EID8,MCP2515AD_TXB0EID0,MCP2515AD_TXB0DLC,MCP2515AD_TXB0D0,MCP2515AD_TXB0D1,MCP2515AD_TXB0D2,MCP2515AD_TXB0D3,MCP2515AD_TXB0D4,MCP2515AD_TXB0D5,MCP2515AD_TXB0D6,MCP2515AD_TXB0D7,MCP2515AD_CANSTAT3,MCP2515AD_CANCTRL3,
	MCP2515AD_TXB1CTRL=0x40,MCP2515AD_TXB1SIDH,MCP2515AD_TXB1SIDL,MCP2515AD_TXB1EID8,MCP2515AD_TXB1EID0,MCP2515AD_TXB1DLC,MCP2515AD_TXB1D0,MCP2515AD_TXB1D1,MCP2515AD_TXB1D2,MCP2515AD_TXB1D3,MCP2515AD_TXB1D4,MCP2515AD_TXB1D5,MCP2515AD_TXB1D6,MCP2515AD_TXB1D7,MCP2515AD_CANSTAT4,MCP2515AD_CANCTRL4,
	MCP2515AD_TXB2CTRL=0x50,MCP2515AD_TXB2SIDH,MCP2515AD_TXB2SIDL,MCP2515AD_TXB2EID8,MCP2515AD_TXB2EID0,MCP2515AD_TXB2DLC,MCP2515AD_TXB2D0,MCP2515AD_TXB2D1,MCP2515AD_TXB2D2,MCP2515AD_TXB2D3,MCP2515AD_TXB2D4,MCP2515AD_TXB2D5,MCP2515AD_TXB2D6,MCP2515AD_TXB2D7,MCP2515AD_CANSTAT5,MCP2515AD_CANCTRL5,
	//	��M�o�b�t�@
	MCP2515AD_RXB0CTRL=0x60,MCP2515AD_RXB0SIDH,MCP2515AD_RXB0SIDL,MCP2515AD_RXB0EID8,MCP2515AD_RXB0EID0,MCP2515AD_RXB0DLC,MCP2515AD_RXB0D0,MCP2515AD_RXB0D1,MCP2515AD_RXB0D2,MCP2515AD_RXB0D3,MCP2515AD_RXB0D4,MCP2515AD_RXB0D5,MCP2515AD_RXB0D6,MCP2515AD_RXB0D7,MCP2515AD_CANSTAT6,MCP2515AD_CANCTRL6,
	MCP2515AD_RXB1CTRL=0x70,MCP2515AD_RXB1SIDH,MCP2515AD_RXB1SIDL,MCP2515AD_RXB1EID8,MCP2515AD_RXB1EID0,MCP2515AD_RXB1DLC,MCP2515AD_RXB1D0,MCP2515AD_RXB1D1,MCP2515AD_RXB1D2,MCP2515AD_RXB1D3,MCP2515AD_RXB1D4,MCP2515AD_RXB1D5,MCP2515AD_RXB1D6,MCP2515AD_RXB1D7,MCP2515AD_CANSTAT7,MCP2515AD_CANCTRL7
};

//----------------------------------------------------------------------------------------
//	�f�o�C�X��Ԓ�`	�R�}���h(MCP2515CMD_STATUS)�ւ̉���
typedef	union	__mcp2515_status__
{
	unsigned char	BYTE;
	struct	{
		unsigned char	TX2IF	:	1;		//	���M2���荞�݃t���O
		unsigned char	TXB2R	:	1;		//	���M2���N�G�X�g
		unsigned char	TX1IF	:	1;		//	���M1���荞�݃t���O
		unsigned char	TXB1R	:	1;		//	���M1���N�G�X�g
		unsigned char	TX0IF	:	1;		//	���M0���荞�݃t���O
		unsigned char	TXB0R	:	1;		//	���M0���N�G�X�g
		unsigned char	RX1IF	:	1;		//	��M1���荞�݃t���O
		unsigned char	RX0IF	:	1;		//	��M0���荞�݃t���O
	}	BIT;
}	MCP2515REG_STATUS;

//----------------------------------------------------------------------------------------
//	��M���b�Z�[�W��Ԓ�`	�R�}���h(MCP2515CMD_RXSTS)�ւ̉���
typedef	union	__mcp2515_rx_status__
{
	unsigned char	BYTE;
	struct	{
		unsigned char	MSG		:	2;		//	���b�Z�[�W	0=���� / 1=RXB0�L�� / 2=RXB1�L�� / 3=�����ɗL��
		unsigned char			:	1;		//	
		unsigned char	TYPE	:	2;		//	�^�C�v		0=�W���f�[�^ / 1=�W�������[�g / 2=�g���f�[�^ / 3=�g�������[�g
		unsigned char	FILT	:	3;		//	��v�t�B���^0�`5=RXF0�`5 / 6=RXF0(RXB1�ɓ]��) / 7=RXF1(RXB1�ɓ]��)
	}	BIT;
}	MCP2515REG_RX_STS;

//----------------------------------------------------------------------------------------
//	RXnBF�s������E��Ԓʒm���W�X�^��`		Address = 0x0C
typedef	union	__mcp2515_bfp_ctrl__
{
	unsigned char	BYTE;
	struct	{
		unsigned char			:	2;		//	[00]
		unsigned char	B1BFS	:	1;		//	�o�̓��[�h��/RX1BF�s���̏�� / ��M���荞�݃��[�h����[0]
		unsigned char	B0BFS	:	1;		//	�o�̓��[�h��/RX0BF�s���̏�� / ��M���荞�݃��[�h����[0]
		unsigned char	B1BFE	:	1;		//	/RX1BF�s���@�\�L����	1=�L�� / 0=����(HiZ)
		unsigned char	B0BFE	:	1;		//	/RX0BF�s���@�\�L����	1=�L�� / 0=����(HiZ)
		unsigned char	B1BFM	:	1;		//	/RX1BF�s�����[�h�ݒ�	1=RXB1��M�����荞�ݏo�� / 0=B1BFS�l�o��
		unsigned char	B0BFM	:	1;		//	/RX0BF�s�����[�h�ݒ�	1=RXB0��M�����荞�ݏo�� / 0=B1BFS�l�o��
	}	BIT;
}	MCP2515REG_BFP_CTRL;

//----------------------------------------------------------------------------------------
//	TXnRTS�s������E��Ԓʒm���W�X�^��`	Address = 0x0D
typedef	union	__mcp2515_txrts_ctrl__
{
	unsigned char	BYTE;
	struct	{
		unsigned char			:	2;		//	[00]
		unsigned char	B2RTS	:	1;		//	���̓��[�h��/TX2RTS�s���̏�� / ���M�v�����[�h����[0]
		unsigned char	B1RTS	:	1;		//	���̓��[�h��/TX1RTS�s���̏�� / ���M�v�����[�h����[0]
		unsigned char	B0RTS	:	1;		//	���̓��[�h��/TX0RTS�s���̏�� / ���M�v�����[�h����[0]
		unsigned char	B2RTSM	:	1;		//	/TX2RTS�s�����[�h�ݒ�	1=TXB2���M�v������(���G�b�W) / 0=����
		unsigned char	B1RTSM	:	1;		//	/TX1RTS�s�����[�h�ݒ�	1=TXB1���M�v������(���G�b�W) / 0=����
		unsigned char	B0RTSM	:	1;		//	/TX0RTS�s�����[�h�ݒ�	1=TXB0���M�v������(���G�b�W) / 0=����
	}	BIT;
}	MCP2515REG_TXRTS_CTRL;

//----------------------------------------------------------------------------------------
//	CAN��ԃ��W�X�^							Address = 0xXE
typedef	union	__mcp2515_stat__
{
	unsigned char	BYTE;
	struct	{
		unsigned char	OPMOD	:	3;		//	���샂�[�h	0=�ʏ� / 1=�X���[�v / 2=���[�v�o�b�N / 3=���b�X�� / 4=�R���t�B�O
		unsigned char			:	1;		//	[0]
		unsigned char	ICOD	:	3;		//	���荞�݃t���O�R�[�h	0=���� / 1=�G���[ / 2=�E�F�C�N�A�b�v / 3=TXB0 / 4=TXB1 / 5=TXB2 / 6=RXB0 / 7=RXB1
		unsigned char			:	1;		//	[0]
	}	BIT;
}	MCP2515REG_STAT;

//----------------------------------------------------------------------------------------
//	CAN���䃌�W�X�^							Address = 0xXF
typedef	union	__mcp2515_ctrl__
{
	unsigned char	BYTE;
	struct	{
		unsigned char	REQOP	:	3;		//	���샂�[�h�v���r�b�g	0=�ʏ� / 1=�X���[�v / 2=���[�v�o�b�N / 3=���b�X�� / 4=�R���t�B�O
		unsigned char	ABAT	:	1;		//	�S�Ă̑��M�̒�~		1=�S�Ă̑��M�o�b�t�@�̑��M��~�v�� / 0=��~�̏I���v��
		unsigned char	OSM		:	1;		//	�����V���b�g���[�h		0=�v���̓s�x���M / 1=1�񂾂����M
		unsigned char	CLKEN	:	1;		//	CLKOUT�s���L����		0=���� / 1=�L��
		unsigned char	CLKPRE	:	2;		//	CLKOUT�s�������ݒ�		0=x/1 / 1=x/2 / 2=x/4 / 3=x/8
	}	BIT;
}	MCP2515REG_CTRL;

//----------------------------------------------------------------------------------------
//	CNF3�R���t�B�M�����[�V����1				Address = 0x28
typedef	union	__mcp2515_cnf3_config__
{
	unsigned char	BYTE;
	struct	{
		unsigned char	SOF		:	1;		//	�X�^�[�g�I�u�t���[��CLKOUT/SOF�s��	0=�N���b�N�o�� / 1=SOF�M��
		unsigned char	WAKFIL	:	1;		//	�E�F�C�N�A�b�v�t�B���^				0=���� / 1=�L��
		unsigned char			:	3;		//	[000]
		unsigned char	PHSEG2	:	3;		//	PS2����				0�`7	(PHSEG2+1)*TQ
	}	BIT;
}	MCP2515REG_CNF3;

//----------------------------------------------------------------------------------------
//	CNF2�R���t�B�M�����[�V����1				Address = 0x29
typedef	union	__mcp2515_cnf2_config__
{
	unsigned char	BYTE;
	struct	{
		unsigned char	BTLMODE	:	1;		//	PS2�r�b�g�^�C����	0=PS2�̒�����PS1�y��IPT(2TQ)���傫������ / 1=PS2�̒�����CNF3��PHSEG2�Ō���
		unsigned char	SAM		:	1;		//	�T���v���_�R���t�B�O	0=1�� / 1=3��@�T���v��
		unsigned char	PHSEG1	:	3;		//	PS1����				0�`7	(PHSEG1+1)*TQ
		unsigned char	PHSEG	:	3;		//	�`�d�Z�O�����g����	0�`7	(PHSEG+1)*TQ
	}	BIT;
}	MCP2515REG_CNF2;

//----------------------------------------------------------------------------------------
//	CNF1�R���t�B�M�����[�V����1				Address = 0x2A
typedef	union	__mcp2515_cnf1_config__
{
	unsigned char	BYTE;
	struct	{
		unsigned char	SJW		:	2;		//	�ē����W�����v������	(SJW+1)*TQ
		unsigned char	BRP		:	6;		//	�{�[���[�g������		TQ=2*(BRP+1)/Fosc
	}	BIT;
}	MCP2515REG_CNF1;

//----------------------------------------------------------------------------------------
//	���荞�݋���							Address = 0x2B
typedef	union	__mcp2515_intf__
{
	unsigned char	BYTE;
	struct	{
		unsigned char	MERRE	:	1;		//	���b�Z�[�W�G���[���荞��		0=�֎~ / 1=����
		unsigned char	WAKIE	:	1;		//	�E�F�C�N�A�b�v���荞��			0=�֎~ / 1=����
		unsigned char	ERRIE	:	1;		//	�G���[���荞��					0=�֎~ / 1=����
		unsigned char	TX2IE	:	1;		//	���M�o�b�t�@2�G���v�e�B���荞��	0=�֎~ / 1=����
		unsigned char	TX1IE	:	1;		//	���M�o�b�t�@1�G���v�e�B���荞��	0=�֎~ / 1=����
		unsigned char	TX0IE	:	1;		//	���M�o�b�t�@0�G���v�e�B���荞��	0=�֎~ / 1=����
		unsigned char	RX1IE	:	1;		//	��M�o�b�t�@1�t�����荞��		0=�֎~ / 1=����
		unsigned char	RX0IE	:	1;		//	��M�o�b�t�@0�t�����荞��		0=�֎~ / 1=����
	}	BIT;
}	MCP2515REG_CANINTE;

//----------------------------------------------------------------------------------------
//	���荞�݃t���O							Address = 0x2C
typedef	union	__mcp2515_intr__
{
	unsigned char	BYTE;
	struct	{
		unsigned char	MERRF	:	1;		//	���b�Z�[�W�G���[���荞��		0=���� / 1=���荞�ݑ҂�
		unsigned char	WAKIF	:	1;		//	�E�F�C�N�A�b�v���荞��			0=���� / 1=���荞�ݑ҂�
		unsigned char	ERRIF	:	1;		//	�G���[���荞��					0=���� / 1=���荞�ݑ҂�
		unsigned char	TX2IF	:	1;		//	���M�o�b�t�@2�G���v�e�B���荞��	0=���� / 1=���荞�ݑ҂�
		unsigned char	TX1IF	:	1;		//	���M�o�b�t�@1�G���v�e�B���荞��	0=���� / 1=���荞�ݑ҂�
		unsigned char	TX0IF	:	1;		//	���M�o�b�t�@0�G���v�e�B���荞��	0=���� / 1=���荞�ݑ҂�
		unsigned char	RX1IF	:	1;		//	��M�o�b�t�@1�t�����荞��		0=���� / 1=���荞�ݑ҂�
		unsigned char	RX0IF	:	1;		//	��M�o�b�t�@0�t�����荞��		0=���� / 1=���荞�ݑ҂�
	}	BIT;
}	MCP2515REG_CANINTF;

//----------------------------------------------------------------------------------------
//	�G���[�t���O							Address = 0x2D
typedef	union	__mcp2515_errfalg__
{
	unsigned char	BYTE;
	struct	{
		unsigned char	RX1OVR	:	1;		//	��M�o�b�t�@1�I�[�o�[�t���[		1=����
		unsigned char	RX0OVR	:	1;		//	��M�o�b�t�@1�I�[�o�[�t���[		1=����
		unsigned char	TXBO	:	1;		//	�o�X�I�t�G���[�t���O			1=����
		unsigned char	TXEP	:	1;		//	���M�G���[�E�p�b�V�u�G���[		1=����
		unsigned char	RXEP	:	1;		//	��M�G���[�E�p�b�V�u�G���[		1=����
		unsigned char	TXWAR	:	1;		//	���M�G���[�x��					1=����
		unsigned char	RXWAR	:	1;		//	��M�G���[�x��					1=����
		unsigned char	EWARN	:	1;		//	TXWAR����RXWAR������			1=����
	}	BIT;
}	MCP2515REG_EFLG;

//----------------------------------------------------------------------------------------
//	���M�o�b�t�@���䃌�W�X�^��`			Address = 0x30 / 0x40 / 0x50
typedef	union	__mcp2515_txb_ctrl__
{
	unsigned char	BYTE;
	struct	{
		unsigned char			:	1;		//	[0]
		unsigned char	ABTF	:	1;		//	���b�Z�[�W��~�t���O	1=���b�Z�[�W�͒�~���ꂽ / 0=���b�Z�[�W���M���튮��
		unsigned char	MLOA	:	1;		//	�A�[�r�g���[�V��������	1=���� / 0=����
		unsigned char	TXERR	:	1;		//	���M�G���[���o�r�b�g	1=�o�X�G���[���� / 0=����
		unsigned char	TXREQ	:	1;		//	���M�v���r�b�g			1=���M�҂� / 0=�҂�����
		unsigned char			:	1;		//	[0]
		unsigned char	TXP		:	2;		//	���M�D�揇��			0=�Œ� �` 3=�ō�
	}	BIT;
}	MCP2515REG_TXB_CTRL;

//----------------------------------------------------------------------------------------
//	TXBn ���M�f�[�^�o�b�t�@�\���̒�`		Address = 0x31�`0x3D / 0x41�`0x4D / 0x51�`0x5D
typedef	struct	__mcp2515_txb__
{
	//	���M�o�b�t�@���䃌�W�X�^��`			Address = 0x30 / 0x40 / 0x50
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char			:	1;		//	[0]
			unsigned char	ABTF	:	1;		//	���b�Z�[�W��~�t���O	1=���b�Z�[�W�͒�~���ꂽ / 0=���b�Z�[�W���M���튮��
			unsigned char	MLOA	:	1;		//	�A�[�r�g���[�V��������	1=���� / 0=����
			unsigned char	TXERR	:	1;		//	���M�G���[���o�r�b�g	1=�o�X�G���[���� / 0=����
			unsigned char	TXREQ	:	1;		//	���M�v���r�b�g			1=���M�҂� / 0=�҂�����
			unsigned char			:	1;		//	[0]
			unsigned char	TXP		:	2;		//	���M�D�揇��			0=�Œ� �` 3=�ō�
		}	BIT;
	}	CTRL;
	//	TXBnSIDH���W�X�^��`	Address = 0x31 / 0x41 / 0x51
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char	SID10	:	1;		//	
			unsigned char	SID9	:	1;		//	
			unsigned char	SID8	:	1;		//	
			unsigned char	SID7	:	1;		//	
			unsigned char	SID6	:	1;		//	
			unsigned char	SID5	:	1;		//	
			unsigned char	SID4	:	1;		//	
			unsigned char	SID3	:	1;		//	
		}	BIT;
	}	SIDH;
	//	TXBnSIDL���W�X�^��`	Address = 0x32 / 0x42 / 0x52
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char	SID2	:	1;		//	
			unsigned char	SID1	:	1;		//	
			unsigned char	SID0	:	1;		//	
			unsigned char			:	1;		//	[0]
			unsigned char	EXIDE	:	1;		//	�g�����ʎq�C�l�[�u���r�b�g	1=�g�� / 0=�W��
			unsigned char			:	1;		//	[0]
			unsigned char	EID17	:	1;		//	
			unsigned char	EID16	:	1;		//	
		}	BIT;
	}	SIDL;
	//	TXBnEID8���W�X�^��`	Address = 0x33 / 0x43 / 0x53
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char	EID15	:	1;		//	
			unsigned char	EID14	:	1;		//	
			unsigned char	EID13	:	1;		//	
			unsigned char	EID12	:	1;		//	
			unsigned char	EID11	:	1;		//	
			unsigned char	EID10	:	1;		//	
			unsigned char	EID9	:	1;		//	
			unsigned char	EID8	:	1;		//	
		}	BIT;
	}	EID8;
	//	TXBnEID0���W�X�^��`	Address = 0x34 / 0x44 / 0x54
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char	EID7	:	1;		//	
			unsigned char	EID6	:	1;		//	
			unsigned char	EID5	:	1;		//	
			unsigned char	EID4	:	1;		//	
			unsigned char	EID3	:	1;		//	
			unsigned char	EID2	:	1;		//	
			unsigned char	EID1	:	1;		//	
			unsigned char	EID0	:	1;		//	
		}	BIT;
	}	EID0;
	//	TXBnDLC���W�X�^��`		Address = 0x35 / 0x45 / 0x55
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char			:	1;		//	[0]
			unsigned char	RTR		:	1;		//	�����[�g���M�v��	1=�����[�g�ő��M / 0=�f�[�^�ő��M
			unsigned char			:	2;		//	[00]
			unsigned char	DLC		:	4;		//	�f�[�^��			0�`8�o�C�g
		}	BIT;
	}	DLC;
	//	TXBnDATA���W�X�^��`		Address = 0x36�`3D / 0x46�`4D / 0x56�`5D
	unsigned char	DATA[8];
}	MCP2515REG_TXB;

//----------------------------------------------------------------------------------------
//	RXBnCTRL���W�X�^��`		Address = 0x60 / 0x70
typedef	union	__mcp2515_rxb_ctrl__
{
	unsigned char	BYTE;
	struct	{
		unsigned char			:	1;		//	[0]
		unsigned char	RXM		:	2;		//	�o�b�t�@���샂�[�h	3=�S���b�Z�[�W��M / 2=�g����v / 1=�W����v / 0=�W���E�g���ǂ��炩�Ɉ�v
		unsigned char			:	1;		//	[0]
		unsigned char	RTR		:	1;		//	�����[�g���M�v��	1=�����[�g�v����M / 0=�����[�g�v������
		unsigned char	BUKT	:	1;		//	�؂�ւ�����		1=RXB0���t���Ȃ�RXB1�Ɏ�M
		unsigned char	BUKT1	:	1;		//	[R] ����̓����g�p
		unsigned char	FILHIT0	:	1;		//	�t�B���^��v		1=RXF1 / 0=RXF0
	}	BIT0;
	struct	{
		unsigned char			:	1;		//	[0]
		unsigned char	RXM		:	2;		//	�o�b�t�@���샂�[�h	3=�S���b�Z�[�W��M / 2=�g����v / 1=�W����v / 0=�W���E�g���ǂ��炩�Ɉ�v
		unsigned char			:	1;		//	[0]
		unsigned char	RTR		:	1;		//	�����[�g���M�v��	1=�����[�g�v����M / 0=�����[�g�v������
		unsigned char	FILHIT	:	3;		//	�t�B���^��v		5�`0=RXF5�`RXF0
	}	BIT1;
}	MCP2515REG_RXB_CTRL;

//----------------------------------------------------------------------------------------
//	RXBn ��M�f�[�^�o�b�t�@�\���̒�`	Address = 0x61�`0x6D / 0x71�`0x7D
typedef	struct	__mcp2515_rxb__
{
	//	RXBnCTRL���W�X�^��`		Address = 0x60 / 0x70
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char			:	1;		//	[0]
			unsigned char	RXM		:	2;		//	�o�b�t�@���샂�[�h	3=�S���b�Z�[�W��M / 2=�g����v / 1=�W����v / 0=�W���E�g���ǂ��炩�Ɉ�v
			unsigned char			:	1;		//	[0]
			unsigned char	RTR		:	1;		//	�����[�g���M�v��	1=�����[�g�v����M / 0=�����[�g�v������
			unsigned char	BUKT	:	1;		//	�؂�ւ�����		1=RXB0���t���Ȃ�RXB1�Ɏ�M
			unsigned char	BUKT1	:	1;		//	[R] ����̓����g�p
			unsigned char	FILHIT0	:	1;		//	�t�B���^��v		1=RXF1 / 0=RXF0
		}	BIT0;
		struct	{
			unsigned char			:	1;		//	[0]
			unsigned char	RXM		:	2;		//	�o�b�t�@���샂�[�h	3=�S���b�Z�[�W��M / 2=�g����v / 1=�W����v / 0=�W���E�g���ǂ��炩�Ɉ�v
			unsigned char			:	1;		//	[0]
			unsigned char	RTR		:	1;		//	�����[�g���M�v��	1=�����[�g�v����M / 0=�����[�g�v������
			unsigned char	FILHIT	:	3;		//	�t�B���^��v		5�`0=RXF5�`RXF0
		}	BIT1;
	}	CTRL;
	//	RXBnSIDH���W�X�^��`	Address = 0x61 / 0x71
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char	SID10	:	1;		//	
			unsigned char	SID9	:	1;		//	
			unsigned char	SID8	:	1;		//	
			unsigned char	SID7	:	1;		//	
			unsigned char	SID6	:	1;		//	
			unsigned char	SID5	:	1;		//	
			unsigned char	SID4	:	1;		//	
			unsigned char	SID3	:	1;		//	
		}	BIT;
	}	SIDH;
	//	RXBnSIDL���W�X�^��`	Address = 0x62 / 0x72
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char	SID2	:	1;		//	
			unsigned char	SID1	:	1;		//	
			unsigned char	SID0	:	1;		//	
			unsigned char	RTR		:	1;		//	�W���t���[���̃����[�g�v��	1=�����[�g���M�v����M / 0=�f�[�^�t���[����M
			unsigned char	IDE		:	1;		//	�g�����ʎq�C�l�[�u���r�b�g	1=�g�� / 0=�W��
			unsigned char			:	1;		//	[0]
			unsigned char	EID17	:	1;		//	
			unsigned char	EID16	:	1;		//	
		}	BIT;
	}	SIDL;
	//	RXBnEID8���W�X�^��`	Address = 0x63 / 0x73
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char	EID15	:	1;		//	
			unsigned char	EID14	:	1;		//	
			unsigned char	EID13	:	1;		//	
			unsigned char	EID12	:	1;		//	
			unsigned char	EID11	:	1;		//	
			unsigned char	EID10	:	1;		//	
			unsigned char	EID9	:	1;		//	
			unsigned char	EID8	:	1;		//	
		}	BIT;
	}	EID8;
	//	RXBnEID0���W�X�^��`	Address = 0x64 / 0x74
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char	EID7	:	1;		//	
			unsigned char	EID6	:	1;		//	
			unsigned char	EID5	:	1;		//	
			unsigned char	EID4	:	1;		//	
			unsigned char	EID3	:	1;		//	
			unsigned char	EID2	:	1;		//	
			unsigned char	EID1	:	1;		//	
			unsigned char	EID0	:	1;		//	
		}	BIT;
	}	EID0;
	//	RXBnDLC���W�X�^��`		Address = 0x65 / 0x75
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char			:	1;		//	[0]
			unsigned char	ERTR	:	1;		//	�g���t���[�������[�g���M�v��	1=�����[�g�ő��M / 0=�f�[�^�ő��M
			unsigned char	RB		:	2;		//	�\��r�b�g
			unsigned char	DLC		:	4;		//	�f�[�^��			0�`8�o�C�g
		}	BIT;
	}	DLC;
	//	RXBnDATA���W�X�^��`		Address = 0x66�`6D / 0x76�`7D
	unsigned char	DATA[8];
}	MCP2515REG_RXB;

//----------------------------------------------------------------------------------------
//	RXFn �t�B���^���W�X�^��`	Address = 0x00�`0x03 / 0x04�`0x07 / 0x08�`0x0B / 0x10�`0x13 / 0x14�`0x17 / 0x18�`0x1B
typedef	struct	__mcp2515_rxfn__
{
	//	RXFnSIDH���W�X�^��`	Address = 0x00 / 0x04 / 0x08 / 0x10 / 0x14 / 0x18
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char	SID10	:	1;		//	
			unsigned char	SID9	:	1;		//	
			unsigned char	SID8	:	1;		//	
			unsigned char	SID7	:	1;		//	
			unsigned char	SID6	:	1;		//	
			unsigned char	SID5	:	1;		//	
			unsigned char	SID4	:	1;		//	
			unsigned char	SID3	:	1;		//	
		}	BIT;
	}	SIDH;
	//	RXFnSIDL���W�X�^��`	Address = 0x01 / 0x05 / 0x09 / 0x11 / 0x15 / 0x19
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char	SID2	:	1;		//	
			unsigned char	SID1	:	1;		//	
			unsigned char	SID0	:	1;		//	
			unsigned char			:	1;		//	[0]
			unsigned char	EXIDE	:	1;		//	�g�����ʎq�C�l�[�u���r�b�g	1=�g���̂� / 0=�W���̂�
			unsigned char			:	1;		//	[0]
			unsigned char	EID17	:	1;		//	
			unsigned char	EID16	:	1;		//	
		}	BIT;
	}	SIDL;
	//	RXFnEID8���W�X�^��`	Address = 0x02 / 0x06 / 0x0A / 0x12 / 0x16 / 0x1A
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char	EID15	:	1;		//	
			unsigned char	EID14	:	1;		//	
			unsigned char	EID13	:	1;		//	
			unsigned char	EID12	:	1;		//	
			unsigned char	EID11	:	1;		//	
			unsigned char	EID10	:	1;		//	
			unsigned char	EID9	:	1;		//	
			unsigned char	EID8	:	1;		//	
		}	BIT;
	}	EID8;
	//	RXFnEID0���W�X�^��`	Address = 0x03 / 0x07 / 0x0B / 0x13 / 0x17 / 0x1B
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char	EID7	:	1;		//	
			unsigned char	EID6	:	1;		//	
			unsigned char	EID5	:	1;		//	
			unsigned char	EID4	:	1;		//	
			unsigned char	EID3	:	1;		//	
			unsigned char	EID2	:	1;		//	
			unsigned char	EID1	:	1;		//	
			unsigned char	EID0	:	1;		//	
		}	BIT;
	}	EID0;
}	MCP2515REG_RXF;

//----------------------------------------------------------------------------------------
//	RXMn ��M�}�X�N���W�X�^��`	Address = 0x20�`0x23 / 0x24�`0x27
typedef	struct	__mcp2515_rxm__
{
	//	RXMnSIDH���W�X�^��`	Address = 0x20 / 0x24
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char	SID10	:	1;		//	
			unsigned char	SID9	:	1;		//	
			unsigned char	SID8	:	1;		//	
			unsigned char	SID7	:	1;		//	
			unsigned char	SID6	:	1;		//	
			unsigned char	SID5	:	1;		//	
			unsigned char	SID4	:	1;		//	
			unsigned char	SID3	:	1;		//	
		}	BIT;
	}	SIDH;
	//	RXMnSIDL���W�X�^��`	Address = 0x21 / 0x25
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char	SID2	:	1;		//	
			unsigned char	SID1	:	1;		//	
			unsigned char	SID0	:	1;		//	
			unsigned char			:	1;		//	[0]
			unsigned char			:	1;		//	[0]
			unsigned char			:	1;		//	[0]
			unsigned char	EID17	:	1;		//	
			unsigned char	EID16	:	1;		//	
		}	BIT;
	}	SIDL;
	//	RXMnEID8���W�X�^��`	Address = 0x22 / 0x26
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char	EID15	:	1;		//	
			unsigned char	EID14	:	1;		//	
			unsigned char	EID13	:	1;		//	
			unsigned char	EID12	:	1;		//	
			unsigned char	EID11	:	1;		//	
			unsigned char	EID10	:	1;		//	
			unsigned char	EID9	:	1;		//	
			unsigned char	EID8	:	1;		//	
		}	BIT;
	}	EID8;
	//	RXMnEID0���W�X�^��`	Address = 0x23 / 0x27
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char	EID7	:	1;		//	
			unsigned char	EID6	:	1;		//	
			unsigned char	EID5	:	1;		//	
			unsigned char	EID4	:	1;		//	
			unsigned char	EID3	:	1;		//	
			unsigned char	EID2	:	1;		//	
			unsigned char	EID1	:	1;		//	
			unsigned char	EID0	:	1;		//	
		}	BIT;
	}	EID0;
}	MCP2515REG_RXM;

//----------------------------------------------------------------------------------------
//	MCP2515�������W�X�^�\���̒�`
//----------------------------------------------------------------------------------------
typedef	union	__mcp2515_reg__
{
	unsigned long			LONG[64];
	unsigned short			WORD[128];
	unsigned char			BYTE[256];
	struct	{
		//	�t�B���^0�`2							Address = 0x00�`0x0B
		MCP2515REG_RXF			RXF0[3];
		//	RXnBF�s������E��Ԓʒm���W�X�^��`		Address = 0x0C
		MCP2515REG_BFP_CTRL		BFP_CTRL;
		//	TXnRTS�s������E��Ԓʒm���W�X�^��`	Address = 0x0D
		MCP2515REG_TXRTS_CTRL	TXRTS_CTRL;
		//	CAN��ԃ��W�X�^							Address = 0x0E
		MCP2515REG_STAT			CANSTAT0;
		//	CAN���䃌�W�X�^							Address = 0x0F
		MCP2515REG_CTRL			CANCTRL0;
		//	�t�B���^3�`5							Address = 0x10�`0x1B
		MCP2515REG_RXF			RXF1[3];
		//	���M�G���[�J�E���^						Address = 0x1C
		unsigned char			TEC;
		//	��M�G���[�J�E���^						Address = 0x1D
		unsigned char			REC;
		//	CAN��ԃ��W�X�^							Address = 0x1E
		MCP2515REG_STAT			CANSTAT1;
		//	CAN���䃌�W�X�^							Address = 0x1F
		MCP2515REG_CTRL			CANCTRL1;
		//	�}�X�N0�`1								Address = 0x20�`0x27
		MCP2515REG_RXM			RXM[2];
		//	CNF3�R���t�B�M�����[�V����1				Address = 0x28
		MCP2515REG_CNF3			CNF3;
		//	CNF2�R���t�B�M�����[�V����1				Address = 0x29
		MCP2515REG_CNF2			CNF2;
		//	CNF1�R���t�B�M�����[�V����1				Address = 0x2A
		MCP2515REG_CNF1			CNF1;
		//	���荞�݋���							Address = 0x2B
		MCP2515REG_CANINTE		CANINTE;
		//	���荞�݃t���O							Address = 0x2C
		MCP2515REG_CANINTF		CANINTF;
		//	�G���[�t���O							Address = 0x2D
		MCP2515REG_CANINTF		EFLG;
		//	CAN��ԃ��W�X�^							Address = 0x2E
		MCP2515REG_STAT			CANSTAT2;
		//	CAN���䃌�W�X�^							Address = 0x2F
		MCP2515REG_CTRL			CANCTRL2;
		//	���M�o�b�t�@0�`2						Address = 0x30�`0x5F
		struct	{
			//	���M�o�b�t�@���䃌�W�X�^			Address = 0x30 / 0x40 / 0x50
			MCP2515REG_TXB_CTRL		TXBCTRL;
			//	���M�o�b�t�@�t���[��				Address = 0x31�`0x3D / 0x41�`0x4D / 0x51�`0x5D
			MCP2515REG_TXB			FRAME;
			//	CAN��ԃ��W�X�^						Address = 0xXE
			MCP2515REG_STAT			STAT;
			//	CAN���䃌�W�X�^						Address = 0xXF
			MCP2515REG_CTRL			CTRL;
		}	TXB[3];
		//	��M�o�b�t�@0,1		Address = 0x60�`0x7F
		struct	{
			//	��M�o�b�t�@���䃌�W�X�^			Address = 0x60 / 0x70
			MCP2515REG_RXB_CTRL		RXBCTRL;
			//	��M�o�b�t�@�t���[��				Address = 0x61�`0x6D / 0x71�`0x7D
			MCP2515REG_RXB			FRAME;
			//	CAN��ԃ��W�X�^						Address = 0xXE
			MCP2515REG_STAT			STAT;
			//	CAN���䃌�W�X�^						Address = 0xXF
			MCP2515REG_CTRL			CTRL;
		}	RXB[2];
	}	REG;
}	MCP2515_REGMAP;

//----------------------------------------------------------------------------------------
//	TXnRTS/RXnBF�s�����䃌�W�X�^��`		Address = 0x0C / 0x0D
typedef	union	__mcp2515_bfp_rts_ctrl__
{
	unsigned short	WORD;
	struct	{
		MCP2515REG_BFP_CTRL		BFP;			//	RXnBF�s������E��Ԓʒm���W�X�^��`		Address = 0x0C
		MCP2515REG_TXRTS_CTRL	RTS;			//	TXnRTS�s������E��Ԓʒm���W�X�^��`	Address = 0x0D
	}	BYTE;
}	MCP2515REG_BFPRTS_CTRL;

//----------------------------------------------------------------------------------------
//	CAN���䃌�W�X�^							Address = 0xXE / 0xXF
typedef	union	__mcp2515_stat_ctrl__
{
	unsigned short	WORD;
	struct	{
		MCP2515REG_STAT		STAT;	//	�X�e�[�^�X
		MCP2515REG_CTRL		CTRL;	//	�R���g���[��
	}	BYTE;
}	MCP2515REG_STATCTRL;

//----------------------------------------------------------------------------------------
//	�ʐM�G���[�J�E���^						Address = 0x1C / 0x1D
typedef	union	__mcp2515_errcnt__
{
	unsigned short	WORD;
	struct	{
		//	���M�G���[�J�E���^	Address = 0x1C
		unsigned char	TEC;
		//	��M�G���[�J�E���^	Address = 0x1D
		unsigned char	REC;
	}	BYTE;
}	MCP2515REG_ERRCNT;


//----------------------------------------------------------------------------------------
//	CAN �R���t�B�M�����[�V����1				Address = 0x28�`0x2B
typedef	union	__mcp2515_config__
{
	unsigned short	WORD[2];
	struct	{
		//	CNF3�R���t�B�M�����[�V����1			Address = 0x28
		MCP2515REG_CNF3		CNF3;
		//	CNF2�R���t�B�M�����[�V����1			Address = 0x29
		MCP2515REG_CNF2		CNF2;
		//	CNF1�R���t�B�M�����[�V����1			Address = 0x2A
		MCP2515REG_CNF1		CNF1;
		//	���荞�݋���						Address = 0x2B
		MCP2515REG_CANINTE	CANINTE;
		//	���荞�݃t���O						Address = 0x2C
		MCP2515REG_CANINTF	CANINTF;
		//	�G���[�t���O						Address = 0x2D
		MCP2515REG_EFLG		EFLG;
	}	BYTE;
}	MCP2515REG_CONFIG;

//----------------------------------------------------------------------------------------
//	CAN �R���t�B�M�����[�V����1				Address = 0x28�`0x2B
typedef	union	__mcp2515_intr_eflg__
{
	unsigned short	WORD;
	struct	{
		//	���荞�݃t���O						Address = 0x2C
		MCP2515REG_CANINTF	CANINTF;
		//	�G���[�t���O						Address = 0x2D
		MCP2515REG_EFLG		EFLG;
	}	BYTE;
}	MCP2515REG_INTERR;

//----------------------------------------------------------------------------------------
//	TXBnCTRL���W�X�^��`		Address = 0x30 / 0x40 / 0x50
typedef	union	__mcp2515_txb_ctrl_buf__
{
	unsigned short	WORD[7];
	unsigned char	BYTE[14];
	struct	{
	//	MCP2515REG_TXB_CTRL		CTRL;		//	���M�o�b�t�@����
		MCP2515REG_TXB			TXB;		//	���M�o�b�t�@
	}	REG;
}	MCP2515REG_TXBUF;

//----------------------------------------------------------------------------------------
//	RXBnCTRL���W�X�^��`		Address = 0x60 / 0x70
typedef	union	__mcp2515_rxb_ctrl_buf__
{
	unsigned short	WORD[7];
	unsigned char	BYTE[14];
	struct	{
	//	MCP2515REG_RXB_CTRL		CTRL;		//	���M�o�b�t�@����
		MCP2515REG_RXB			RXB;		//	���M�o�b�t�@
	}	REG;
}	MCP2515REG_RXBUF;

//----------------------------------------------------------------------------------------
//	�r�b�g�Z�b�g�N���A���b�Z�[�W��`	�R�}���h(MCP2515CMD_BITX)�f�[�^
typedef	union	__mcp2515_bit_set_clear__
{
	unsigned short	WORD;
	struct	{
		union	{
			unsigned char			BYTE;		//	�}�X�N�p�^�[��(1=�ύX / 0=�ێ�)
			MCP2515REG_TXB_CTRL		TXBCTRL;	//	���M�o�b�t�@����
			MCP2515REG_RXB_CTRL		RXBCTRL;	//	��M�o�b�t�@����
			MCP2515REG_CNF3			CNF3;		//	CNF3�R���t�B�M�����[�V����1		Address = 0x28
			MCP2515REG_CNF2			CNF2;		//	CNF2�R���t�B�M�����[�V����1		Address = 0x29
			MCP2515REG_CNF1			CNF1;		//	CNF1�R���t�B�M�����[�V����1		Address = 0x2A
			MCP2515REG_CANINTE		INTE;		//	���荞�݋��t���O
			MCP2515REG_CANINTF		INTF;		//	���荞�݃t���O
			MCP2515REG_EFLG			EFLG;		//	�G���[�t���O
			MCP2515REG_BFP_CTRL		BFP;		//	RXnBF�s������E��Ԓʒm���W�X�^��`	Address = 0x0C
			MCP2515REG_TXRTS_CTRL	RTS;		//	TXnRTS�s������E��Ԓʒm���W�X�^��`	Address = 0x0D
			MCP2515REG_CTRL			CTRL;		//	CAN���䃌�W�X�^
		}	MSK;
		union	{
			unsigned char			BYTE;		//	�r�b�g�p�^�[��
			MCP2515REG_TXB_CTRL		TXBCTRL;	//	���M�o�b�t�@����
			MCP2515REG_RXB_CTRL		RXBCTRL;	//	��M�o�b�t�@����
			MCP2515REG_CNF3			CNF3;		//	CNF3�R���t�B�M�����[�V����1		Address = 0x28
			MCP2515REG_CNF2			CNF2;		//	CNF2�R���t�B�M�����[�V����1		Address = 0x29
			MCP2515REG_CNF1			CNF1;		//	CNF1�R���t�B�M�����[�V����1		Address = 0x2A
			MCP2515REG_CANINTE		INTE;		//	���荞�݋��t���O
			MCP2515REG_CANINTF		INTF;		//	���荞�݃t���O
			MCP2515REG_EFLG			EFLG;		//	�G���[�t���O
			MCP2515REG_BFP_CTRL		BFP;		//	RXnBF�s������E��Ԓʒm���W�X�^��`	Address = 0x0C
			MCP2515REG_TXRTS_CTRL	RTS;		//	TXnRTS�s������E��Ԓʒm���W�X�^��`	Address = 0x0D
			MCP2515REG_CTRL			CTRL;		//	CAN���䃌�W�X�^
		}	PAT;
	}	BYTE;
}	MCP2515REG_BITX;

//----------------------------------------------------------------------------------------
//	MCP2515����JOB�ԍ���`
//----------------------------------------------------------------------------------------
enum	mcp2515_job	{
	CAN3_JOB_INIT=0,	//	�f�o�C�X������
	CAN3_JOB_IW1,		//	
	CAN3_JOB_IW2,		//	
	CAN3_JOB_IW3,		//	
	CAN3_JOB_IW4,		//	
	CAN3_JOB_IW5,		//	
	CAN3_JOB_IW6,		//	
	CAN3_JOB_IW7,		//	
	CAN3_JOB_IW8,		//	
	CAN3_JOB_IW9,		//	
	
	CAN3_JOB_WAIT,		//	����҂�
	CAN3_JOB_WW1,		//	
	CAN3_JOB_WW2,		//	
	CAN3_JOB_WW3,		//	
	CAN3_JOB_WW4,		//	
	CAN3_JOB_WW5,		//	
	
	CAN3_JOB_CHECK,		//	�X�e�[�^�X�`�F�b�N
	
	
	CAN3_JOB_OVER
};

//________________________________________________________________________________________
//
//	can3_init
//----------------------------------------------------------------------------------------
//	�@�\����
//		MCP2515(RSPI2�o�R)�̏�����
//	����
//		����
//	�߂�
//		����
//________________________________________________________________________________________
//
extern	RSPI_REQUESTS	*rspi_req;					//	����M���N�G�X�g�`�F�[���ϐ��Q��
extern	RSPI_DTC_REQ	*can3_now;					//	����M�����N�G�X�g
extern	int				can3_job_id;				//	�����ԍ�

extern	void			can3_init(void);			//	CAN3�|�[�g������
extern	int				can3_job(void);				//	������JOB

//	���M���[���{�b�N�X��������
extern	int				CAN3_TxSet(int mb, SEND_WAIT_FLAME *act);
extern	int				CAN3_GetTxMCTL(int mb);		//	���M�o�b�t�@�󂫊m�F

//---------------------------------------------------------------------------------------
//  ���荞�݌�X�e�[�^�X�擾���Callback����
//---------------------------------------------------------------------------------------
extern	void			can3_sts_event(MCP2515REG_INTERR *rxd);

#endif	/*RSPI2_ACTIVATE*/

#endif	/*__CAN2ECU_CAN3RSPI2_IF__*/

