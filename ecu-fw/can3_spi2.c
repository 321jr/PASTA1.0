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
//	MCP2515 CAN�R���g���[�� RSPI2-I/F �ʐM
//
//----------------------------------------------------------------------------------------
//	�J������
//
//	2016/12/01	�R�[�f�B���O�J�n�i�k�j
//
//----------------------------------------------------------------------------------------
//	T.Tachibana
//	��L&F
//________________________________________________________________________________________
//

#include	<sysio.h>
#include	<string.h>
#include	<stdio.h>
#include	"iodefine.h"
#include	"timer.h"
#include	"ecu.h"				/*	ECU ���ʒ�`			*/
#include	"can3_spi2.h"		/*	CAN3 ��`				*/
#include	"cantp.h"			/*	CAN-TP ��`				*/

//	���[���{�b�N�X�̎g�p�͈͂��Œ肵���p�@�̏ꍇ�L���ɂ���
#define		MB_LOCKED_TYPE
//	���[���{�b�N�X���P�����g�p����ꍇ�L���ɂ���
//#define		MB_USED_ONLYONE

/*
	�|�[�g�ݒ�

			Port		SCI			I2C			SPI			�K�p
	----------------------------------------------------------------------------
	RSPI2	PD2									MISOC		<RSPI>		CAN3
			PD1									MOSIC		<RSPI>		CAN3
			PD3									RSPCKC		<RSPI>		CAN3
			PD4									SSLC0		<RSPI>		CAN3
			PD0									IRQ0		<- CINT		CAN3
			PD6									IRQ6		<- CRX0BF	CAN3
			PD7									IRQ7		<- CRX1BF	CAN3
			P90												-> CTX0RTS	CAN3
			P91												-> CTX1RTS	CAN3
			P92												-> CTX2RTS	CAN3
			P93												-> CRSET	CAN3
			P07												<- CSOF		CAN3

	DTC�ݒ�
	
	DTC�x�N�^�x�[�X	DTCVBR     $0003E000-$0003EFFF
	
	��MDTC�x�N�^	DTCE_RSPI2_SPRI2	45		0003E0B4
	���MDTC�x�N�^	DTCE_RSPI2_SPTI2	46		0003E0B8


*/

//	���O�@�\
void	logging(char *fmt, ...);

#ifdef		RSPI2_ACTIVATE

//________________________________________________________________________________________
//
//	CAN3�|�[�g��p�ϐ���`
//________________________________________________________________________________________
//

//	����M���N�G�X�g�`�F�[���ϐ��Q��
RSPI_DTC_REQ	*can3_now;			//	�]�����̗v���o�b�t�@�|�C���^
RSPI_REQUESTS	*rspi_req;			//	�v���o�b�t�@
int				rspi_req_WP = 0;	//	�������݈ʒu
int				rspi_req_RP = 0;	//	�ǂݏo���ʒu
int				rspi_req_PP = 0;	//	�����ʒu


//	�V�[�P���X
int				can3_job_id = CAN3_JOB_INIT;	//	��������
int				stat_event_flag = 0;			//	�X�e�[�^�X��M�C�x���g�t���O

//	���M�҂��t���O
int				tx_act[3] = {0,0,0};
int				tx_act_timer[3] = {0,0,0};

//	�v���g�^�C�v
int can_recv_frame(int ch, void *mbox);
void can_tx_mb(int ch, int mb);
RSPI_DTC_REQ *can3_request(int cmd, int adr, int txlen, int rxlen, void *proc, void *data);

unsigned long	*dtc_table = DTC_VECT_TOP;

void	dtc_init(void)
{
	SYSTEM.PRCR.WORD = 0xA503;		//	�v���e�N�g����
	MSTP_DTC = 0;					//	DTC���W���[���X�g�b�v����
	SYSTEM.PRCR.WORD = 0xA500;		//	�v���e�N�g

	DTC.DTCST.BIT.DTCST = 0;		//	DTC���W���[���N���r�b�g
									//	0�FDTC���W���[����~
									//	1�FDTC���W���[������
	memset(dtc_table, 0, 0x1000);	//	�x�N�^������
	DTC.DTCVBR = DTC_VECT_TOP;		//	DTC�x�N�^�x�[�X�A�h���X�ݒ�	(0x0003E000)
	DTC.DTCCR.BIT.RRS = 0;			//	DTC�]����񃊁[�h�X�L�b�v���r�b�g
									//	0�F�]����񃊁[�h�X�L�b�v���s��Ȃ�
									//	1�F�x�N�^�ԍ��̒l����v�����Ƃ��A�]����񃊁[�h�X�L�b�v���s��
	DTC.DTCADMOD.BIT.SHORT = 0;		//	�V���[�g�A�h���X���[�h�ݒ�r�b�g
									//	0�F�t���A�h���X���[�h
									//	1�F�V���[�g�A�h���X���[�h
	DTC.DTCST.BIT.DTCST = 1;		//	DTC���W���[���N���r�b�g
									//	0�FDTC���W���[����~
									//	1�FDTC���W���[������
//	a = DTC.DTCSTS.BIT.ACT;			//	[R]�A�N�e�B�u�x�N�^�ԍ����j�^(1:���쒆)
//	v = DTC.DTCSTS.BIT.VECN;		//	[R]�A�N�e�B�u�t���O(�]�����̃x�N�^�ԍ�)
}

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
//					PD6												<- CRX0BF	CAN3
//					PD7												<- CRX1BF	CAN3
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
void rspi2_init(long bps)
{
	SYSTEM.PRCR.WORD = 0xA503;	//	�v���e�N�g����
	MSTP_RSPI2 = 0;				//	RSPI2���W���[���X�g�b�v����

	//	RSPI2 ���荞�ݗv���֎~
	ICU.IER[IER_ICU_GROUPL0].BIT.IEN_ICU_GROUPL0 = 0;	//	�O���[�v12 �G���[���荞�݋֎~
	ICU.IER[IER_RSPI2_SPRI2].BIT.IEN_RSPI2_SPRI2 = 0;	//	��M�o�b�t�@�t�����荞�݋֎~
	ICU.IER[IER_RSPI2_SPTI2].BIT.IEN_RSPI2_SPTI2 = 0;	//	���M�G���v�e�B���荞�݋֎~
	ICU.IER[IER_RSPI2_SPII2].BIT.IEN_RSPI2_SPII2 = 0;	//	���M�A�C�h�����荞�݋֎~
	ICU.IER[IER_ICU_IRQ0].BIT.IEN_ICU_IRQ0 = 0;			//	���W���[�����荞�݋���
	ICU.IER[IER_ICU_IRQ6].BIT.IEN_ICU_IRQ6 = 0;			//	���W���[�����荞�݋���
	ICU.IER[IER_ICU_IRQ7].BIT.IEN_ICU_IRQ7 = 0;			//	���W���[�����荞�݋���

	RSPI2.SPCR.BYTE = 0;		//	���W���[��������

	PORTD.PMR.BIT.B2 = 0;		//	���Ӌ@�\	MISOC
	PORTD.PMR.BIT.B1 = 0;		//	���Ӌ@�\	MOSIC
	PORTD.PMR.BIT.B3 = 0;		//	���Ӌ@�\	RSPCKC
	PORTD.PMR.BIT.B4 = 0;		//	���Ӌ@�\	SSLC0
	PORTD.PMR.BIT.B0 = 0;		//	���Ӌ@�\	CINT
	PORTD.PMR.BIT.B6 = 0;		//	PD6	<- CRX0BF
	PORTD.PMR.BIT.B7 = 0;		//	PD7	<- CRX1BF
	PORT9.PMR.BIT.B0 = 0;		//	P90 -> CTX0RTS
	PORT9.PMR.BIT.B1 = 0;		//	P91 -> CTX1RTS
	PORT9.PMR.BIT.B2 = 0;		//	P92 -> CTX2RTS
	PORT9.PMR.BIT.B3 = 0;		//	P93 -> CRSET
	PORT0.PMR.BIT.B7 = 0;		//	P07 <- CSOF

	// CAN3�̒[�q�ݒ�(RSPI2�o�R)
	PORTD.PODR.BYTE = 0x1F;		//	�|�[�g�����l
	PORT9.PODR.BYTE = 0x07;		//	�|�[�g�����l
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
	PORT9.PODR.BIT.B3 = 0;		//	P93 -> CRSET

	MPC.PWPR.BIT.B0WI = 0;		//	
	MPC.PWPR.BIT.PFSWE = 1;		//	

	MPC.PD2PFS.BYTE = 0x0D;		//	MISOC		SO
	MPC.PD1PFS.BYTE = 0x0D;		//	MOSIC		SI
	MPC.PD3PFS.BYTE = 0x0D;		//	RSPCKC		SCK
	MPC.PD4PFS.BYTE = 0x0D;		//	SSLC0		/CS
	MPC.PD0PFS.BYTE = 0x40;		//	IRQ0		/INT
	MPC.PD6PFS.BYTE = 0x40;		//	IRQ6		/RX0BF
	MPC.PD7PFS.BYTE = 0x40;		//	IRQ7		/RX1BF
	MPC.P90PFS.BYTE = 0x00;		//	port		/TX0RTS
	MPC.P91PFS.BYTE = 0x00;		//	port		/TX1RTS
	MPC.P92PFS.BYTE = 0x00;		//	port		/TX2RTS
	MPC.P93PFS.BYTE = 0x00;		//	port		/RES
	MPC.P07PFS.BYTE = 0x00;		//	port		/CSOF
	
	MPC.PWPR.BIT.PFSWE = 0;		//	
	MPC.PWPR.BIT.B0WI = 1;		//	

	PORTD.PMR.BIT.B2 = 1;		//	���Ӌ@�\	MISOC
	PORTD.PMR.BIT.B1 = 1;		//	���Ӌ@�\	MOSIC
	PORTD.PMR.BIT.B3 = 1;		//	���Ӌ@�\	RSPCKC
	PORTD.PMR.BIT.B4 = 0;		//	���Ӌ@�\	SSLC0
	PORTD.PMR.BIT.B0 = 1;		//	���Ӌ@�\	CINT
	PORTD.PMR.BIT.B6 = 1;		//	PD6	<- CRX0BF
	PORTD.PMR.BIT.B7 = 1;		//	PD7	<- CRX1BF
	PORT9.PMR.BIT.B0 = 0;		//	P90 -> CTX0RTS
	PORT9.PMR.BIT.B1 = 0;		//	P91 -> CTX1RTS
	PORT9.PMR.BIT.B2 = 0;		//	P92 -> CTX2RTS
	PORT9.PMR.BIT.B3 = 0;		//	P93 -> CRSET
	PORT0.PMR.BIT.B7 = 0;		//	P07 <- CSOF

//	//	CAN�h���C�oS�[�q����|�[�g
//	PORT6.PDR.BIT.B0 = 1;		//	P60 -- Port-out CAN0S
//	PORT6.PDR.BIT.B1 = 1;		//	P61 -- Port-out CAN1S
//	PORT6.PDR.BIT.B2 = 1;		//	P62 -- Port-out CAN2S
//	PORT6.PDR.BIT.B3 = 1;		//	P63 -- Port-out CAN3S
//	PORT6.PODR.BYTE = 0x0F;		//	�|�[�g������

	SYSTEM.PRCR.WORD = 0xA500;	//	�|�[�g�ݒ�֎~
	
	//	RSPI2�̐ݒ�(�V���O���}�X�^���[�h)
	RSPI2.SSLP.BYTE = 0;		//	SSLnP �̓A�N�e�B�uLow
	RSPI2.SPPCR.BYTE = 0x20;	//	MOSI�̃A�C�h���o�͂�Low
	RSPI2.SPSR.BYTE &= 0;		//	�G���[�t���O����
	RSPI2.SPSCR.BYTE = 0;		//	�V�[�P���X�����l

	//	Set baud rate to 1Mbps	N�l(BRDV[1:0])=0 �Œ�	�ŏ�=93,750bps
	//	n = (PCLK Frequency) / (2 * 2^N * Bit Rate) - 1
	//	n = (48,000,000) / (2 * 2^0 * 1,000,000) - 1
	//	n = 24
	RSPI2.SPBR.BYTE = 48000000 / (2 * bps) - 1;
	RSPI2.SPDCR.BYTE = 0x00;	//	SPDR�̓��[�h�A�N�Z�X / ��M�o�b�t�@�ǂݏo�� / 1�t���[��
	RSPI2.SPCKD.BYTE = 1;		//	�N���b�N�x�� 1RSPCK
	RSPI2.SSLND.BYTE = 0;		//	SSL�l�Q�[�g�x�� 1RSPCK
	RSPI2.SPND.BYTE = 0;		//	���A�N�Z�X�x�� 1RSPCK + 2PCLK
	RSPI2.SPCR2.BYTE = 0;		//	�p���e�B���� / �A�C�h�����荞�݋֎~
	
	//	�R�}���h���W�X�^������
	//	RSPCK�ʑ��ݒ�r�b�g
	RSPI2.SPCMD0.BIT.CPHA = 0;		//	0 : ��G�b�W�Ńf�[�^�T���v���A�����G�b�W�Ńf�[�^�ω�
									//	1 : ��G�b�W�Ńf�[�^�ω��A�����G�b�W�Ńf�[�^�T���v��
	//	RSPCK�ɐ��ݒ�r�b�g
	RSPI2.SPCMD0.BIT.CPOL = 0;		//	0�F�A�C�h������RSPCK��Low
									//	1�F�A�C�h������RSPCK��High
	//	�r�b�g���[�g�����ݒ�r�b�g
	RSPI2.SPCMD0.BIT.BRDV = 0;		//	b3 b2
									//	0 0�F�x�[�X�̃r�b�g���[�g��I��
									//	0 1�F�x�[�X�̃r�b�g���[�g��2������I��
									//	1 0�F�x�[�X�̃r�b�g���[�g��4������I��
									//	1 1�F�x�[�X�̃r�b�g���[�g��8������I��
	//	SSL�M���A�T�[�g�ݒ�r�b�g
	RSPI2.SPCMD0.BIT.SSLA = 0;		//	b6 b4
									//	0 0 0�FSSL0
									//	0 0 1�FSSL1
									//	0 1 0�FSSL2
									//	0 1 1�FSSL3
									//	1 x x�F�ݒ肵�Ȃ��ł�������
									//	x�FDon�ft care
	//	SSL�M�����x���ێ��r�b�g
	RSPI2.SPCMD0.BIT.SSLKP = 0;		//	0�F�]���I�����ɑSSSL�M�����l�Q�[�g
									//	1�F�]���I���ォ�玟�A�N�Z�X�J�n�܂�SSL�M�����x����ێ�
	//	RSPI�f�[�^���ݒ�r�b�g
	RSPI2.SPCMD0.BIT.SPB = 0xF;		//	b11 b8
									//	0100�`0111 �F8�r�b�g
									//	1 0 0 0�F9�r�b�g
									//	1 0 0 1�F10�r�b�g
									//	1 0 1 0�F11�r�b�g
									//	1 0 1 1�F12�r�b�g
									//	1 1 0 0�F13�r�b�g
									//	1 1 0 1�F14�r�b�g
									//	1 1 1 0�F15�r�b�g
									//	1 1 1 1�F16�r�b�g
									//	0 0 0 0�F20�r�b�g
									//	0 0 0 1�F24�r�b�g
									//	0010�A0011 �F32�r�b�g
	//	RSPI LSB�t�@�[�X�g�r�b�g
	RSPI2.SPCMD0.BIT.LSBF = 0;		//	0�FMSB�t�@�[�X�g
									//	1�FLSB�t�@�[�X�g
	//	RSPI���A�N�Z�X�x�����r�b�g
	RSPI2.SPCMD0.BIT.SPNDEN = 0;	//	0�F���A�N�Z�X�x����1RSPCK�{2PCLK
									//	1�F���A�N�Z�X�x����RSPI���A�N�Z�X�x�����W�X�^�iSPND�j�̐ݒ�l
	//	SSL�l�Q�[�g�x���ݒ苖�r�b�g
	RSPI2.SPCMD0.BIT.SLNDEN = 0;	//	0�FSSL�l�Q�[�g�x����1RSPCK
									//	1�FSSL�l�Q�[�g�x����RSPI�X���[�u�Z���N�g�l�Q�[�g�x�����W�X�^�iSSLND�j�̐ݒ�l
	//	RSPCK�x���ݒ苖�r�b�g
	RSPI2.SPCMD0.BIT.SCKDEN = 0;	//	0�FRSPCK�x����1RSPCK
									//	1�FRSPCK�x����RSPI�N���b�N�x�����W�X�^�iSPCKD�j�̐ݒ�l

	//	�ݒ�R�s�[
	RSPI2.SPCMD1.WORD = RSPI2.SPCMD0.WORD;
	RSPI2.SPCMD2.WORD = RSPI2.SPCMD0.WORD;
	RSPI2.SPCMD3.WORD = RSPI2.SPCMD0.WORD;
	RSPI2.SPCMD4.WORD = RSPI2.SPCMD0.WORD;
	RSPI2.SPCMD5.WORD = RSPI2.SPCMD0.WORD;
	RSPI2.SPCMD6.WORD = RSPI2.SPCMD0.WORD;
	RSPI2.SPCMD7.WORD = RSPI2.SPCMD0.WORD;
	
	//	���싖��
	//	RSPI���[�h�I���r�b�g
	RSPI2.SPCR.BIT.SPMS = 1;		//	0�FSPI����i4�����j
									//	1�F�N���b�N����������i3�����j
	//	�ʐM���샂�[�h�I���r�b�g
	RSPI2.SPCR.BIT.TXMD = 0;		//	0�F�S��d�������V���A���ʐM
									//	1�F���M����݂̂̃V���A���ʐM
	//	���[�h�t�H���g�G���[���o���r�b�g
	RSPI2.SPCR.BIT.MODFEN = 0;		//	0�F���[�h�t�H���g�G���[���o���֎~
									//	1�F���[�h�t�H���g�G���[���o������
	//	RSPI�}�X�^/�X���[�u���[�h�I���r�b�g
	RSPI2.SPCR.BIT.MSTR = 1;		//	0�F�X���[�u���[�h
									//	1�F�}�X�^���[�h
	//	RSPI�G���[���荞�݋��r�b�g
	RSPI2.SPCR.BIT.SPEIE = 0;		//	0�FRSPI�G���[���荞�ݗv���̔������֎~
									//	1�FRSPI�G���[���荞�ݗv���̔���������
	//	RSPI���M���荞�݋��r�b�g
	RSPI2.SPCR.BIT.SPTIE = 0;		//	0�FRSPI���M���荞�ݗv���̔������֎~
									//	1�FRSPI���M���荞�ݗv���̔���������
	//	RSPI��M���荞�݋��r�b�g
	RSPI2.SPCR.BIT.SPRIE = 0;		//	0�FRSPI��M���荞�ݗv���̔������֎~
									//	1�FRSPI��M���荞�ݗv���̔���������
	//	RSPI�A�C�h�����荞�݋��r�b�g
	RSPI2.SPCR2.BIT.SPIIE = 0;		//	0�F�A�C�h�����荞�ݗv���̔������֎~
									//	1�F�A�C�h�����荞�ݗv���̔���������
	//	RSPI�@�\���r�b�g
	RSPI2.SPCR.BIT.SPE = 0;			//	0�FRSPI�@�\�͖���
									//	1�FRSPI�@�\���L��

	//	IRQ�R���g���[��
	ICU.IRQCR[0].BIT.IRQMD = 1;		//	/INT �M��
	ICU.IRQCR[6].BIT.IRQMD = 1;		//	/RX0BF ��M���荞�ݐM��
	ICU.IRQCR[7].BIT.IRQMD = 1;		//	/RX1BF ��M���荞�ݐM��
									//	b3 b2
									//	0 0�FLow
									//	0 1�F����������G�b�W
									//	1 0�F�����オ��G�b�W
									//	1 1�F���G�b�W

	ICU.IPR[IPR_ICU_IRQ0].BIT.IPR = 10;					//	���荞�݃��x���ݒ�
	ICU.IPR[IPR_ICU_IRQ6].BIT.IPR = 9;					//	���荞�݃��x���ݒ�
	ICU.IPR[IPR_ICU_IRQ7].BIT.IPR = 9;					//	���荞�݃��x���ݒ�
	ICU.IR[IR_ICU_IRQ0].BIT.IR = 0;						//	���荞�݃t���O�N���A
	ICU.IR[IR_ICU_IRQ6].BIT.IR = 0;						//	���荞�݃t���O�N���A
	ICU.IR[IR_ICU_IRQ7].BIT.IR = 0;						//	���荞�݃t���O�N���A
										

	//	���荞�ݗD�揇�ʐݒ�
	ICU.IPR[IPR_RSPI2_].BIT.IPR = 8;					//	���荞�݃��x���ݒ�
	//	���荞�݃t���O�N���A
	ICU.IR[IR_RSPI2_SPRI2].BIT.IR = 0;
	ICU.IR[IR_RSPI2_SPTI2].BIT.IR = 0;
	ICU.IR[IR_RSPI2_SPII2].BIT.IR = 0;
	//	GROUP12���荞�ݐݒ�
	ICU.GEN[GEN_RSPI2_SPEI2].BIT.EN_RSPI2_SPEI2 = 1;	//	�O���[�v12 RSPI1��M�G���[���荞�݋���
	ICU.IPR[IPR_ICU_GROUPL0].BIT.IPR = 2;				//	�O���\�v12 ���荞�݃��x���ݒ�
	ICU.IR[IR_ICU_GROUPL0].BIT.IR = 0;					//	�O���[�v12 ���荞�݃t���O�N���A
	//	���荞�݋��ݒ�
	ICU.IER[IER_RSPI2_SPRI2].BIT.IEN_RSPI2_SPRI2 = 1;	//	��M�o�b�t�@�t�����荞�݋���
	ICU.IER[IER_RSPI2_SPTI2].BIT.IEN_RSPI2_SPTI2 = 1;	//	���M�G���v�e�B���荞�݋���
	ICU.IER[IER_RSPI2_SPII2].BIT.IEN_RSPI2_SPII2 = 0;	//	���M�A�C�h�����荞�݋���
	ICU.IER[IER_ICU_GROUPL0].BIT.IEN_ICU_GROUPL0 = 1;	//	�O���[�v12 ���荞�݋���
	ICU.IER[IER_ICU_IRQ0].BIT.IEN_ICU_IRQ0 = 0;			//	���W���[�����荞�݋���
	ICU.IER[IER_ICU_IRQ6].BIT.IEN_ICU_IRQ6 = 0;			//	���W���[�����荞�݋���
	ICU.IER[IER_ICU_IRQ7].BIT.IEN_ICU_IRQ7 = 0;			//	���W���[�����荞�݋���
	
	//	MCP2515-RESET ����
//	PORT9.PODR.BIT.B3 = 1;

	dtc_init();	//	DTC������
}

//________________________________________________________________________________________
//
//	can3_init
//----------------------------------------------------------------------------------------
//	�@�\����
//		MCP2515(RSPI2�o�R)�̏�����
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
//		����
//	�߂�
//		����
//________________________________________________________________________________________
//
void can3_init(void)
{
	rspi2_init(8000000);	//	8Mbps�ŏ�����

	rspi_req = (RSPI_REQUESTS *)DTC_REQUEST_TOP;
	memset(rspi_req, 0, sizeof(RSPI_REQUESTS));

	can3_now = 0;			//	���݂̏�������

	CAN3_TX0RTS_PORT = 1;	//	TXB0���M�v������
	CAN3_TX1RTS_PORT = 1;	//	TXB1���M�v������
	CAN3_TX2RTS_PORT = 1;	//	TXB2���M�v������
	
	//	RSPI�G���[���荞�݋��r�b�g
	RSPI2.SPCR.BIT.SPEIE = 0;		//	0�FRSPI�G���[���荞�ݗv���̔������֎~
									//	1�FRSPI�G���[���荞�ݗv���̔���������
	//	RSPI���M���荞�݋��r�b�g
	RSPI2.SPCR.BIT.SPTIE = 1;		//	0�FRSPI���M���荞�ݗv���̔������֎~
									//	1�FRSPI���M���荞�ݗv���̔���������
	//	RSPI��M���荞�݋��r�b�g
	RSPI2.SPCR.BIT.SPRIE = 1;		//	0�FRSPI��M���荞�ݗv���̔������֎~
									//	1�FRSPI��M���荞�ݗv���̔���������
	//	RSPI�A�C�h�����荞�݋��r�b�g
	RSPI2.SPCR2.BIT.SPIIE = 0;		//	0�F�A�C�h�����荞�ݗv���̔������֎~
									//	1�F�A�C�h�����荞�ݗv���̔���������
	RSPI2.SPCR.BIT.SPE = 0;			//	0�FRSPI�@�\�͖���
	DTC.DTCST.BIT.DTCST = 0;		//	���W���[����~

	ICU.IR[IR_ICU_IRQ0].BIT.IR = 0;						//	���荞�݃t���O�N���A
	ICU.IR[IR_ICU_IRQ6].BIT.IR = 0;						//	���荞�݃t���O�N���A
	ICU.IR[IR_ICU_IRQ7].BIT.IR = 0;						//	���荞�݃t���O�N���A
	ICU.IER[IER_ICU_IRQ0].BIT.IEN_ICU_IRQ0 = 0;			//	���W���[�����荞�݋���
	ICU.IER[IER_ICU_IRQ6].BIT.IEN_ICU_IRQ6 = 0;			//	���W���[�����荞�݋���
	ICU.IER[IER_ICU_IRQ7].BIT.IEN_ICU_IRQ7 = 0;			//	���W���[�����荞�݋���
}
int can3_txcheck(void);
void can3_send_nextbyte(void);
void can3_stat_event(MCP2515REG_STATUS *sts);

//	�X�e�[�^�X���䃌�W�X�^���
MCP2515REG_STATCTRL		mcp_statctrl;
//	�|�[�g�ݒ�
MCP2515REG_BFPRTS_CTRL	mcp_bfprts;
//	�R���t�B�M�����[�V����
MCP2515REG_CONFIG		mcp_config;
//	���M�o�b�t�@
MCP2515REG_TXBUF		mcp_txb[3];
//	��M�o�b�t�@
MCP2515REG_RXBUF		mcp_rxb[2];
//	�r�b�g�ύX
MCP2515REG_BITX			mcp_bitx;

//	��M�o�b�t�@
CAN_MBOX				mcp_mbx[16];
int						mcp_mbx_wp = 0;
int						mcp_mbx_rp = 0;

//----------------------------------------------------------------------------------------
//	MCP2515�X�e�[�^�X�E�R���g���[���ǂݏo�������֐�
//----------------------------------------------------------------------------------------
void can3_getstat_callback(MCP2515REG_STATCTRL *rxd)
{
	mcp_statctrl = *rxd;
}

//---------------------------------------------------------------------------------------
//	�����҂��d������
//---------------------------------------------------------------------------------------
void can3_recv_call(void)
{
	int	i;
	if(mcp_mbx_wp != mcp_mbx_rp)
	{
		i = mcp_mbx_rp++;
		mcp_mbx_rp &= 15;
		can_recv_frame(3, (void *)&mcp_mbx[i]);	//	��M�f�[�^����
	}
}
void can3_procwait(void)
{
	RSPI_DTC_REQ	*w;
	int				i;
	unsigned short	sw;

	if(rspi_req_RP != rspi_req_PP)
	{
		w = &rspi_req->REQ[rspi_req_PP];
		if(w->TXL >= 0) return;		//	������
		if(w->TXL == -2) return;	//	�������M��
		rspi_req_PP++;
		if(rspi_req_PP >= CAN3_REQUEST_DTC_MAX) rspi_req_PP = 0;
		if(w->TXL == -1)
		{	//	���M����
			if(w->RXL > 0)
			{	//	��M�f�[�^�̃X���b�v���{
				for(i = 0; i < w->RXL; i++)
				{
					sw = w->RXP[i];
					w->RXP[i] = ((sw >> 8) & 0x00FF) | ((sw << 8) & 0xFF00);
				}
			}
			if(w->CALL)
			{	//	���[�U�[�����֐��Ăяo��
				((CAN3_PROC_CALL)w->CALL)((void *)w->RXP);
			}
		}
	}
}

//________________________________________________________________________________________
//
//	can3_job
//----------------------------------------------------------------------------------------
//	�@�\����
//		MCP2515�N�������������Ƒ҂�����
//	����
//		����
//	�߂�
//		����
//________________________________________________________________________________________
//
int can3_job(void)
{
	can3_txcheck();
	can3_procwait();
	can3_recv_call();
	
	switch(can3_job_id)
	{
	case CAN3_JOB_INIT:	//	�f�o�C�X������
		can3_job_id++;
		memset(&mcp_mbx, 0, sizeof(mcp_mbx));
		can3_request(MCP2515CMD_READ, MCP2515AD_CANSTAT, 0, 1, can3_getstat_callback, 0);		//	�X�e�[�^�X�E�R���g���[���擾
		break;
	case CAN3_JOB_IW1:
		if(can3_now == 0)
		{
			can3_job_id++;
			CAN3_RESET_PORT = 1;	//	���Z�b�g����
		}
		break;
	case CAN3_JOB_IW2:
		can3_job_id++;
		can3_request(MCP2515CMD_READ, MCP2515AD_CANSTAT, 0, 1, can3_getstat_callback, 0);		//	�X�e�[�^�X�E�R���g���[���擾
		break;
	case CAN3_JOB_IW3:
		if(can3_now == 0)
		{
			if(mcp_statctrl.BYTE.STAT.BIT.OPMOD == 4 && (mcp_statctrl.BYTE.CTRL.BIT.REQOP == 15 || mcp_statctrl.BYTE.CTRL.BIT.REQOP == 4))
			{	//	���Z�b�g����̏�Ԃƈ�v
				can3_job_id++;
			}
			else
			{
				can3_job_id = CAN3_JOB_IW2;	//	�Ď擾
			}
		}
		break;
	case CAN3_JOB_IW4:
		can3_job_id++;
		mcp_bitx.BYTE.MSK.BYTE = 0xE0;
		mcp_bitx.BYTE.PAT.BYTE = 0x80;
		can3_request(MCP2515CMD_BITX, MCP2515AD_CANCTRL, 1, 0, 0, &mcp_bitx);				//	���[�h�؂�ւ�
		can3_request(MCP2515CMD_READ, MCP2515AD_CANSTAT, 0, 1, can3_getstat_callback, 0);	//	�X�e�[�^�X�E�R���g���[���擾
		break;
	case CAN3_JOB_IW5:
		if(can3_now == 0)
		{
			if(mcp_statctrl.BYTE.STAT.BIT.OPMOD == 4 && mcp_statctrl.BYTE.CTRL.BIT.REQOP != 4)
			{	//	�R���t�B�O�ւ̕ω��҂�
				can3_job_id = CAN3_JOB_IW4;
			}
			else
			{	//	�R���t�B�O���[�h�J�n
				can3_job_id++;
			}
		}
		break;
	case CAN3_JOB_IW6:	//	�p�����[�^�ݒ�
		can3_job_id++;
		memset(&mcp_config, 0, sizeof(mcp_config));
		memset(&mcp_bfprts, 0, sizeof(mcp_bfprts));
		//	���o�̓|�[�g�@�\�ݒ�
		mcp_bfprts.BYTE.BFP.BYTE = 0x0F;	//	RXnBF = ��M�o�b�t�@�t�����荞�ݏo��
		mcp_bfprts.BYTE.RTS.BYTE = 0x07;	//	TXnRTS = TXBnREQ���M�v������
		can3_request(MCP2515CMD_WRITE, MCP2515AD_BFPCTRL, 1, 0, 0, &mcp_bfprts);	//	�|�[�g�ݒ�
		
		//	�{�[���[�g�ݒ�(Fosc=20MHz : Tosc=50ns(1s/20M) : CAN=500Kbps)
		//			   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _   _
		//	tosc	|_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| |_| 
		//			   _0_     _1_     _2_     _3_     _4_     _5_     _6_     _7_     _8_     _9_     _0_     _1_     _
		//	TBRPCLK	__|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___|   |___| 
		//			   _______ _______________________________________________________________________                  
		//	tBIT	__|__Sync_|_____PropSeg___|__________PS1__________|______________PS2______________|_________________
		//			   _______________________________________________|_________________________________________________
		//	tTQ		__|_______|_______|_______|_______|_______|_______|_______|_______|_______|_______|_______|_______|_
		//			  |                                               |                               |                 
		//	CANBIT	->|<----------------------------------------------A------------------------------>|<----------------
		//			                                               (Sample)                                             
		//	CAN1Bit:10TQ �Ƃ��� TQ=1/500K/10=200ns : BRP=TQ/(2*Tosc)-1=200ns/100ns-1=2-1 �]��BRP=1 : CANBIT=10TQ
		//	���� : PropSeg + PS1 >= PS2 : PropSeg + PS1 >= TDELAY : PS2 > SJW
		//	SyncSeg(1) + PropSeg(2) + PS1(3) + PS2(4) = 10TQ : SJW = PS2 - 2 = 2TQ : TDELAY = 2TQ
		mcp_config.BYTE.CNF1.BIT.BRP = 2-1;			//	BRP = TQ / (2 * Tosc)	Tosc=50ns(20MHz)	TQ=	200ns
		mcp_config.BYTE.CNF1.BIT.SJW = 3-1;			//	SJW = (SJW+1)*TQ		�ē����W�����v��
		mcp_config.BYTE.CNF2.BIT.PHSEG = 2-1;		//	PropSeg = (PHSEG+1)*TQ	�`���Z�O�����g			400ns
		mcp_config.BYTE.CNF2.BIT.PHSEG1 = 3-1;		//	PS1 = (PHSEG1+1)*TQ		�t�F�[�Y�Z�O�����g�P	600ns
		mcp_config.BYTE.CNF2.BIT.SAM = 0;			//	0=1��T���v��(1=3��T���v��)
		mcp_config.BYTE.CNF2.BIT.BTLMODE = 1;		//	PHSEG2�Ō���
		mcp_config.BYTE.CNF3.BIT.PHSEG2 = 4-1;		//	PS2 = (PHSEG2+1)*TQ		�t�F�[�Y�Z�O�����g�Q	800ns
		mcp_config.BYTE.CNF3.BIT.WAKFIL = 0;		//	�E�F�C�N�A�b�v�t�B���^����
		mcp_config.BYTE.CNF3.BIT.SOF = 0;			//	SOF�o��
		
		//	(/INT �o��)���荞�݋��ݒ�
		mcp_config.BYTE.CANINTE.BYTE = 0x1F;		//	MERRE / WAKIE �֎~ : ERRIE / TX2IE / TX1IE / TX0IE / RX1IE / RX0IE ����
		can3_request(MCP2515CMD_WRITE, MCP2515AD_CNFIG3, 2, 0, 0, &mcp_config);	//	�{�[���[�g�ݒ�{���荞�݋���
		
		//	���M�o�b�t�@�̐ݒ�
		memset(&mcp_txb[0], 0, 14);
		mcp_txb[0].REG.TXB.CTRL.BYTE = 0;	//	�D��x�u���v
		can3_request(MCP2515CMD_WRITE, MCP2515AD_TXB0CTRL, 7, 0, 0, &mcp_txb[0]);	//	TXB0�ݒ�ۑ�
		memset(&mcp_txb[1], 0, 14);
		mcp_txb[1].REG.TXB.CTRL.BYTE = 0;	//	�D��x�u���v
		can3_request(MCP2515CMD_WRITE, MCP2515AD_TXB1CTRL, 7, 0, 0, &mcp_txb[1]);	//	TXB1�ݒ�ۑ�
		memset(&mcp_txb[2], 0, 14);
		mcp_txb[2].REG.TXB.CTRL.BYTE = 0;	//	�D��x�u��v
		can3_request(MCP2515CMD_WRITE, MCP2515AD_TXB2CTRL, 7, 0, 0, &mcp_txb[2]);	//	TXB2�ݒ�ۑ�
		
		//	��M�o�b�t�@�̐ݒ�
		memset(&mcp_rxb[0], 0, 14);
		mcp_rxb[0].REG.RXB.CTRL.BYTE = 0x64;	//	�S���b�Z�[�W��M & �؂�ւ����r�b�g
		can3_request(MCP2515CMD_WRITE, MCP2515AD_RXB0CTRL, 7, 0, 0, &mcp_rxb[0]);	//	RXB0�R���g���[��
		memset(&mcp_rxb[1], 0, 14);
		mcp_rxb[1].REG.RXB.CTRL.BYTE = 0x60;	//	�S���b�Z�[�W��M
		can3_request(MCP2515CMD_WRITE, MCP2515AD_RXB1CTRL, 7, 0, 0, &mcp_rxb[1]);	//	RXB1�R���g���[��
		break;
	case CAN3_JOB_IW7:	//	�p�����[�^�ݒ芮���҂�
		if(can3_now == 0)
		{
			can3_job_id++;
		}
		break;
	case CAN3_JOB_IW8:	//	CAN�X�^�[�g
		can3_job_id++;
		mcp_bitx.BYTE.MSK.BYTE = 0xE0;	//	���[�h�I��
		mcp_bitx.BYTE.PAT.BYTE = 0x00;	//	�ʏ탂�[�h(0)
		can3_request(MCP2515CMD_BITX, MCP2515AD_CANCTRL, 1, 0, 0, &mcp_bitx);				//	���[�h�؂�ւ�
		can3_request(MCP2515CMD_READ, MCP2515AD_CANSTAT, 0, 1, can3_getstat_callback, 0);	//	�X�e�[�^�X�擾
		break;
	case CAN3_JOB_IW9:	//	�p�����[�^�ݒ芮���҂�
		if(can3_now == 0)
		{
			if(mcp_statctrl.BYTE.STAT.BIT.OPMOD != 0)
			{	//	�R���t�B�O�ւ̕ω��҂�
				can3_job_id = CAN3_JOB_IW8;		//	�Đݒ�
			}
			else
			{	//	�ݒ芮��
				can3_job_id++;
			}
		}
		break;

	case CAN3_JOB_WAIT:	//	����҂�
		if(rspi_req_RP == rspi_req_WP)
		{
			can3_job_id = CAN3_JOB_WW1;
			stat_event_flag = 100;
			can3_request(MCP2515CMD_STATUS, 0, 0, 1, can3_stat_event, 0);	//	�X�e�[�^�X�擾
		}
		break;
	case CAN3_JOB_WW1:	//	�X�e�[�^�X�擾�҂�
		if(rspi_req_RP == rspi_req_WP)
		{	//	���M�ς�
			if(stat_event_flag == 0)
			{	//	��M����
				can3_job_id = CAN3_JOB_WAIT;
			}
			else
			{	//	�^�C���A�E�g���o
				stat_event_flag--;
			}
		}
		break;
	}
	return ((can3_job_id >= CAN3_JOB_WAIT) ? 1 : 0);	//	����������0��Ԃ�
}

//________________________________________________________________________________________
//
//	���M����
//________________________________________________________________________________________
//
int can3_txcheck(void)
{
	for(;can3_now == 0 && rspi_req_RP != rspi_req_WP;)
	{	//	�҂��L��
		can3_now = &rspi_req->REQ[rspi_req_RP++];
		if(rspi_req_RP >= CAN3_REQUEST_DTC_MAX) rspi_req_RP = 0;
		if(can3_now->TXL < 0)
		{	//	���M�ς݁A�p�X����
			can3_now = 0;
			continue;
		}
		CAN3_SPI_CS_PORT = 1;				//	CS����
		DTC.DTCST.BIT.DTCST = 0;			//	���W���[����~
		RSPI2.SPCR.BIT.SPE = 0;				//	0�FRSPI�@�\�͖���
		RSPI2.SPCR.BIT.SPRIE = 0;			//	1�FRSPI��M���荞�ݗv���̔���������
		RSPI2.SPCR.BIT.SPTIE = 0;			//	1�FRSPI���M���荞�ݗv���̔���������
		ICU.IR[IR_RSPI2_SPRI2].BIT.IR = 0;	//	���荞�݃t���O�N���A
		ICU.IR[IR_RSPI2_SPTI2].BIT.IR = 0;	//	���荞�݃t���O�N���A
		dtc_table[DTCE_RSPI2_SPTI2] = (unsigned long)&can3_now->DTCTX.LONG[0];
		dtc_table[DTCE_RSPI2_SPRI2] = (unsigned long)&can3_now->DTCRX.LONG[0];
		ICU.DTCER[ DTCE_RSPI2_SPRI2 ].BIT.DTCE = 1;			//	RSPI��M���荞�݂ɂ��DTC�N������
		ICU.DTCER[ DTCE_RSPI2_SPTI2 ].BIT.DTCE = 1;			//	RSPI���M���荞�݂ɂ��DTC�N������
		RSPI2.SPCR.BIT.SPRIE = 1;			//	1�FRSPI��M���荞�ݗv���̔���������
		RSPI2.SPCR.BIT.SPTIE = 1;			//	1�FRSPI���M���荞�ݗv���̔���������
		ICU.IR[IR_RSPI2_SPRI2].BIT.IR = 0;	//	���荞�݃t���O�N���A
		ICU.IR[IR_RSPI2_SPTI2].BIT.IR = 0;	//	���荞�݃t���O�N���A
		DTC.DTCST.BIT.DTCST = 1;			//	DTC����
		CAN3_SPI_CS_PORT = 0;				//	CS����
		RSPI2.SPCR.BIT.SPE = 1;				//	SPI����
		return 1;	//	�d�|�L��
	}
	return 0;	//	�d�|����
}

//---------------------------------------------------------------------------------------
//	SPRI2 ��M�o�b�t�@�t��	SPI��M�o�b�t�@�Ƀf�[�^����
//---------------------------------------------------------------------------------------
void interrupt __vectno__{VECT_RSPI2_SPRI2} RSPI2_SPRI2_ISR(void)
{
//	int	i;
//	unsigned short sw;
	//	CS����
	CAN3_SPI_CS_PORT = 1;
	RSPI2.SPCR.BIT.SPRIE = 0;		//	0�FRSPI��M���荞�ݗv���̔������֎~
	RSPI2.SPCR.BIT.SPE = 0;			//	RSPI2��~
	DTC.DTCST.BIT.DTCST = 0;		//	DTC��~
//	if(can3_now->RXL == 0 && can3_now->CALL != 0)
//	{	//	���M�݂̂̃R�[���o�b�N�L��͏�������
//		((CAN3_PROC_CALL)can3_now->CALL)(0);
//	}
	can3_now->TXL = -1;				//	�I���}�[�N
	can3_now = 0;
}

//---------------------------------------------------------------------------------------
//	SPTI2 ���M�o�b�t�@�G���v�e�B	SPI���M�o�b�t�@���󂢂�
//---------------------------------------------------------------------------------------
void interrupt __vectno__{VECT_RSPI2_SPTI2} RSPI2_SPTI2_ISR(void)
{
	RSPI2.SPCR.BIT.SPTIE = 0;		//	0�FRSPI���M���荞�ݗv���̔������֎~
}

//---------------------------------------------------------------------------------------
//	SPII2 �A�C�h��	���M�f�[�^�̍ŏI�������݊������Ɋ��荞�ݔ����i���M�����j
//---------------------------------------------------------------------------------------
void interrupt __vectno__{VECT_RSPI2_SPII2} RSPI2_SPII2_ISR(void)
{
	RSPI2.SPCR2.BIT.SPIIE = 0;		//	0�F�A�C�h�����荞�ݗv���̔������֎~
	logging("IDLE\r");
}

//________________________________________________________________________________________
//
//	can3_request
//----------------------------------------------------------------------------------------
//	�@�\����
//		SPI�ʐM�v���̐ςݏグ
//	����
//		int cmd		�R�}���h�R�[�h
//		int adr		�]����A�h���X
//		int txlen	���M�o�C�g��
//		int rxlen	��M�o�C�g��
//		void *proc	�������Ăяo����֐�
//		void *data	���M�f�[�^�|�C���^
//	�߂�
//		����
//________________________________________________________________________________________
//
RSPI_DTC_REQ *can3_request(int cmd, int adr, int txlen, int rxlen, void *proc, void *data)
{
	int				i, j;
	RSPI_DTC_REQ	*act;
	unsigned short	*dp, sw;
	
	//	�o�b�t�@�擾
	act = &rspi_req->REQ[rspi_req_WP++];
	if(rspi_req_WP >= CAN3_REQUEST_DTC_MAX) rspi_req_WP = 0;
	dp = (unsigned short *)data;
	i = 0;	//	���M�f�[�^�|�C���^
	//	�R�}���h�o�^
	act->TXL = txlen;		//	���M�f�[�^��
	act->RXL = rxlen;		//	��M�f�[�^��
	act->DAT[i++] = (unsigned short)((cmd << 8) | adr);	//	�R�}���h�R�[�h
	//	���M�f�[�^�R�s�[
	for(j = 0; j < txlen; j++)
	{
		sw = dp[j];
		act->DAT[i++] = ((sw >> 8) & 0x00FF) | ((sw << 8) & 0xFF00);
	}
	act->RXP = &act->DAT[i];
	//	��M�f�[�^�_�~�[
	for(; j < rxlen; j++) act->DAT[i++] = 0;
	//	�������Ăяo����o�^
	act->CALL = proc;
	//	DTC�v�����ݒ�
	//	���M
	act->DTCTX.REG.MR.LONG = 0x18000000;
	act->DTCTX.REG.SAR = (unsigned long)&act->DAT[0];	//	�]�����F���M�f�[�^�o�b�t�@
	act->DTCTX.REG.DAR = (unsigned long)&RSPI2.SPDR;	//	�]����FSPI�f�[�^���W�X�^
	act->DTCTX.REG.CR.NOR.A = i;
	act->DTCTX.REG.CR.NOR.B = i;
	//	��M
	act->DTCRX.REG.MR.LONG = 0x10080000;
	act->DTCRX.REG.SAR = (unsigned long)&RSPI2.SPDR;	//	�]�����FSPI�f�[�^���W�X�^
	act->DTCRX.REG.DAR = (unsigned long)&act->DAT[0];	//	�]����F��M�f�[�^�o�b�t�@
	act->DTCRX.REG.CR.NOR.A = i;
	act->DTCRX.REG.CR.NOR.B = i;
	return act;
}

void CAN3_GetRx0(void);
void CAN3_GetRx1(void);
//---------------------------------------------------------------------------------------
//	��M�f�[�^�擾�R�[���o�b�N
//---------------------------------------------------------------------------------------
void CAN3_CallbackRx0(MCP2515REG_RXBUF *rxd)
{
	CAN_MBOX	*mbx;
	mbx = &mcp_mbx[mcp_mbx_wp++];
	mcp_mbx_wp &= 15;
//	mcp_mbx[0].ID.LONG = 0;
//	mcp_mbx[0].ID.BIT.IDE = rxd->BYTE.RXB.SIDL.BIT.IDE;
	mbx->ID.BIT.RTR = rxd->REG.RXB.SIDL.BIT.RTR;
	mbx->ID.BIT.SID = ((((unsigned long)rxd->REG.RXB.SIDH.BYTE) << 3) & 0x7F8) | ((((unsigned long)rxd->REG.RXB.SIDL.BYTE) >> 5) & 0x007);
	mbx->DLC = rxd->REG.RXB.DLC.BIT.DLC;
	memcpy(mbx->DATA, rxd->REG.RXB.DATA, 8);
//	mcp_mbx[mcp_mbx_wp].TS++;
//	can_recv_frame(3, (void *)&mbx);	//	ECU��M�f�[�^�擾
//	can3_request(MCP2515CMD_STATUS, 0, 0, 1, can3_stat_event, 0);	//	�X�e�[�^�X�擾
//	if(mbx->ID.BIT.SID == led_monit_id && (led_monit_ch & 8) != 0) PORTE.PODR.BIT.B0 = 0;
	if(mbx->ID.BIT.SID == led_monit_id && (led_monit_ch & 0x80) != 0)
	{
		led_monit_ch &= 0x8F;
		PORTE.PODR.BIT.B0 = 0;
		cmt1_start(1000000, monit_timeover);
	}
}
void CAN3_CallbackRx1(MCP2515REG_RXBUF *rxd)
{
	CAN_MBOX	*mbx;
	mbx = &mcp_mbx[mcp_mbx_wp++];
	mcp_mbx_wp &= 15;
//	mbx.ID.LONG = 0;
//	mbx.ID.BIT.IDE = rxd->REG.RXB.SIDL.BIT.IDE;
	mbx->ID.BIT.RTR = rxd->REG.RXB.SIDL.BIT.RTR;
	mbx->ID.BIT.SID = ((((unsigned long)rxd->REG.RXB.SIDH.BYTE) << 3) & 0x7F8) | ((((unsigned long)rxd->REG.RXB.SIDL.BYTE) >> 5) & 0x007);
	mbx->DLC = rxd->REG.RXB.DLC.BIT.DLC;
	memcpy(mbx->DATA, rxd->REG.RXB.DATA, 8);
//	mcp_mbx[mcp_mbx_wp].TS++;
//	can_recv_frame(3, (void *)&mbx);	//	ECU��M�f�[�^�擾
//	can3_request(MCP2515CMD_STATUS, 0, 0, 1, can3_stat_event, 0);	//	�X�e�[�^�X�擾
//	if(mbx->ID.BIT.SID == led_monit_id && (led_monit_ch & 8) != 0) PORTE.PODR.BIT.B0 = 0;
	if(mbx->ID.BIT.SID == led_monit_id && (led_monit_ch & 0x80) != 0)
	{
		led_monit_ch &= 0x8F;
		PORTE.PODR.BIT.B0 = 0;
		cmt1_start(1000000, monit_timeover);
	}
}

//---------------------------------------------------------------------------------------
//	���M���[���{�b�N�X�������݃R�[���o�b�N�i���M�J�n�v���j
//---------------------------------------------------------------------------------------
void CAN3_CallbackTxSet0(MCP2515REG_TXBUF *rxd)	//MCP2515REG_TXBUF *txr)
{
	if(memcmp(&mcp_txb[0].BYTE[1], &rxd->BYTE[1], 13) == 0)
	{
		CAN3_TX0RTS_PORT = 0;	//	TX0���M�v��
		tx_act[0] = 2;
		CAN3_TX0RTS_PORT = 1;	//	TX0���M�v��
	}
	else
	{
		logging("CAN3 TX0-Readback Error\r");
		can3_request(MCP2515CMD_WRITE, MCP2515AD_TXB0CTRL, 7, 0, 0, &mcp_txb[0]);
		can3_request(MCP2515CMD_READ, MCP2515AD_TXB0CTRL, 0, 7, CAN3_CallbackTxSet0, 0);
	}
}
void CAN3_CallbackTxSet1(MCP2515REG_TXBUF *rxd)	//MCP2515REG_TXBUF *txr)
{
	if(memcmp(&mcp_txb[1].BYTE[1], &rxd->BYTE[1], 13) == 0)
	{
		CAN3_TX1RTS_PORT = 0;	//	TX1���M�v��
		tx_act[1] = 2;
		CAN3_TX1RTS_PORT = 1;	//	TX1���M�v��
	}
	else
	{
		logging("CAN3 TX1-Readback Error\r");
		can3_request(MCP2515CMD_WRITE, MCP2515AD_TXB1CTRL, 7, 0, 0, &mcp_txb[1]);
		can3_request(MCP2515CMD_READ, MCP2515AD_TXB1CTRL, 0, 7, CAN3_CallbackTxSet1, 0);
	}
}
void CAN3_CallbackTxSet2(MCP2515REG_TXBUF *rxd)	//MCP2515REG_TXBUF *txr)
{
	if(memcmp(&mcp_txb[2].BYTE[1], &rxd->BYTE[1], 13) == 0)
	{
		CAN3_TX2RTS_PORT = 0;	//	TX2���M�v��
		tx_act[2] = 2;
		CAN3_TX2RTS_PORT = 1;	//	TX2���M�v��
	}
	else
	{
		logging("CAN3 TX2-Readback Error\r");
		can3_request(MCP2515CMD_WRITE, MCP2515AD_TXB2CTRL, 7, 0, 0, &mcp_txb[2]);
		can3_request(MCP2515CMD_READ, MCP2515AD_TXB2CTRL, 0, 7, CAN3_CallbackTxSet2, 0);
	}
}

//---------------------------------------------------------------------------------------
//	���M���[���{�b�N�X�󂫏�Ԏ擾
//---------------------------------------------------------------------------------------
int CAN3_GetTxMCTL(int mb)
{
#ifdef	MB_USED_ONLYONE
	if(tx_act[0] == 0) return 0;
	//	�^�C���A�E�gMB�̌���
	if(tx_act_timer[0] > 0) tx_act_timer[0]--;
	if(tx_act_timer[0] == 0)
	{
		tx_act[0] = 0;
		logging("CAN3 TXMB0:Timeout\r");
		return 0;	//	�^�C���A�E�g��
	}
#else
	int	rc;
	rc = rspi_req_WP - rspi_req_RP;
	if(rc < 0) rc += CAN3_REQUEST_DTC_MAX;
	if(rc > (CAN3_REQUEST_DTC_MAX - 8))
	{	//	�o�b�t�@�댯
		return 0xC0;	//	�o�b�t�@�󂫕s��
	}

#ifdef	MB_LOCKED_TYPE
	if(tx_act[mb] == 0) return 0;
	//	�^�C���A�E�gMB�̌���
	if(tx_act_timer[mb] > 0) tx_act_timer[mb]--;
	if(tx_act_timer[mb] == 0)
	{
		tx_act[mb] = 0;
	//	logging("CAN3 TXMB%d:Timeout\r", mb);
		return mb;	//	�^�C���A�E�g��
	}
#else
	if(tx_act[0] == 0) return 0;
	if(tx_act[1] == 0) return 1;
	if(tx_act[2] == 0) return 2;
	
	//	�^�C���A�E�gMB�̌���
	if(tx_act_timer[0] > 0) tx_act_timer[0]--;
	if(tx_act_timer[0] == 0)
	{
		tx_act[0] = 0;
	//	logging("CAN3 TXMB0:Timeout\r");
		return 0;	//	�^�C���A�E�g��
	}
	if(tx_act_timer[1] > 0) tx_act_timer[1]--;
	if(tx_act_timer[1] == 0)
	{
		tx_act[1] = 0;
	//	logging("CAN3 TXMB1:Timeout\r");
		return 1;	//	�^�C���A�E�g��
	}
	if(tx_act_timer[2] > 0) tx_act_timer[2]--;
	if(tx_act_timer[2] == 0)
	{
		tx_act[2] = 0;
	//	logging("CAN3 TXMB2:Timeout\r");
		return 2;	//	�^�C���A�E�g��
	}
#endif
#endif
	return 0xC0;	//	�󂫖���
}

//---------------------------------------------------------------------------------------
//	���M���[���{�b�N�X��������
//---------------------------------------------------------------------------------------
int CAN3_TxSet(int mb, SEND_WAIT_FLAME *act)
{
	unsigned char		ctrl = 3 - mb;
	//	���N�G�X�g�̗L���`�F�b�N
#ifdef	MB_LOCKED_TYPE
	if(tx_act[mb] != 0) return -1;	//	�󂫖���
#else
	mb = 0;
	if(tx_act[0] != 0)
	{
		mb++;
		if(tx_act[1] != 0)
		{
			mb++;
			if(tx_act[2] != 0)
			{
				return -1;	//	�󂫖���
			}
		}
	}
#endif
	tx_act[mb] = 1;
	tx_act_timer[mb] = 1000;
	//	�o�b�t�@�]��
	mcp_txb[mb].REG.TXB.CTRL.BYTE = ctrl;	//	�D��x
	mcp_txb[mb].REG.TXB.SIDH.BYTE = (unsigned char)(act->ID.BIT.SID >> 3);
	mcp_txb[mb].REG.TXB.SIDL.BYTE = (act->ID.BIT.SID << 5) & 0xE0;
	mcp_txb[mb].REG.TXB.EID8.BYTE = 0;
	mcp_txb[mb].REG.TXB.EID0.BYTE = 0;
	mcp_txb[mb].REG.TXB.DLC.BIT.RTR = act->ID.BIT.RTR;
	mcp_txb[mb].REG.TXB.DLC.BIT.DLC = act->ID.BIT.DLC;
	memcpy(mcp_txb[mb].REG.TXB.DATA, act->FD.BYTE, 8);
	//	SPI���M����
	switch(mb)
	{
	case 0:
		can3_request(MCP2515CMD_WRITE, MCP2515AD_TXB0CTRL, 7, 0, 0, &mcp_txb[mb]);
		can3_request(MCP2515CMD_READ, MCP2515AD_TXB0CTRL, 0, 7, CAN3_CallbackTxSet0, 0);
		break;
	case 1:
		can3_request(MCP2515CMD_WRITE, MCP2515AD_TXB1CTRL, 7, 0, 0, &mcp_txb[mb]);
		can3_request(MCP2515CMD_READ, MCP2515AD_TXB1CTRL, 0, 7, CAN3_CallbackTxSet1, 0);
		break;
	case 2:
		can3_request(MCP2515CMD_WRITE, MCP2515AD_TXB2CTRL, 7, 0, 0, &mcp_txb[mb]);
		can3_request(MCP2515CMD_READ, MCP2515AD_TXB2CTRL, 0, 7, CAN3_CallbackTxSet2, 0);
		break;
	}
//	can3_request(MCP2515CMD_STATUS, 0, 0, 1, can3_stat_event, 0);	//	�X�e�[�^�X�擾
	return 0;
}


//---------------------------------------------------------------------------------------
// �����X�e�[�^�X�擾���Callback����
//---------------------------------------------------------------------------------------
void can3_stat_event(MCP2515REG_STATUS *sts)
{
	int	id;
	unsigned short	buf = 0x0000;
	
	stat_event_flag = 0;
	
#if 1
	if(sts->BIT.RX0IF != 0)
	{	//	��M�o�b�t�@0�t��
		can3_request(MCP2515CMD_READ, MCP2515AD_RXB0CTRL, 0, 7, CAN3_CallbackRx0, 0);	//	RXB0�ǂݏo��
		buf |= 0x01;
	}
	if(sts->BIT.RX1IF != 0)
	{	//	��M�o�b�t�@1�t��
		can3_request(MCP2515CMD_READ, MCP2515AD_RXB1CTRL, 0, 7, CAN3_CallbackRx1, 0);	//	RXB1�ǂݏo��
		buf |= 0x02;
	}
#endif
	//	TXB0
	if(sts->BIT.TXB0R != 0)
	{
		if(tx_act[0] == 2) tx_act[0] = 3;
	}
	else
	{
		if(tx_act[0] == 3)
		{
			tx_act[0] = 0;
			id = ((((unsigned long)mcp_txb[0].REG.TXB.SIDH.BYTE) << 3) & 0x7F8) | ((((unsigned long)mcp_txb[0].REG.TXB.SIDL.BYTE) >> 5) & 0x007);
			if(led_monit_ch & 8)
			{
			//	if(id == led_monit_id) PORTE.PODR.BIT.B0 = 1;
				if(id == led_monit_id && (led_monit_ch & 0x08) != 0)
				{
					led_monit_ch &= 0xF8;
					PORTE.PODR.BIT.B0 = 1;
					monit_timeover();
				}
			}
			can_tp_txecheck(3, id);	//	TP���M�����m�F
		}
	}
	if(sts->BIT.TX0IF != 0)
	{	//	�I�����Ă���
		if(sts->BIT.TXB0R == 0 && tx_act[0] == 2)
		{
			tx_act[0] = 0;
			id = ((((unsigned long)mcp_txb[0].REG.TXB.SIDH.BYTE) << 3) & 0x7F8) | ((((unsigned long)mcp_txb[0].REG.TXB.SIDL.BYTE) >> 5) & 0x007);
			if(led_monit_ch & 8)
			{
			//	if(id == led_monit_id) PORTE.PODR.BIT.B0 = 1;
				if(id == led_monit_id && (led_monit_ch & 0x08) != 0)
				{
					led_monit_ch &= 0xF8;
					PORTE.PODR.BIT.B0 = 1;
					monit_timeover();
				}
			}
			can_tp_txecheck(3, id);	//	TP���M�����m�F
		}
		buf |= 0x04;
	}
	
	//	TXB1
	if(sts->BIT.TXB1R != 0)
	{
		if(tx_act[1] == 2) tx_act[1] = 3;
	}
	else
	{
		if(tx_act[1] == 3)
		{
			tx_act[1] = 0;
			id = ((((unsigned long)mcp_txb[1].REG.TXB.SIDH.BYTE) << 3) & 0x7F8) | ((((unsigned long)mcp_txb[1].REG.TXB.SIDL.BYTE) >> 5) & 0x007);
			if(led_monit_ch & 8)
			{
			//	if(id == led_monit_id) PORTE.PODR.BIT.B0 = 1;
				if(id == led_monit_id && (led_monit_ch & 0x08) != 0)
				{
					led_monit_ch &= 0xF8;
					PORTE.PODR.BIT.B0 = 1;
					monit_timeover();
				}
			}
			can_tp_txecheck(3, id);	//	TP���M�����m�F
		}
	}
	if(sts->BIT.TX1IF != 0)
	{	//	�I�����Ă���
		if(sts->BIT.TXB1R == 0 && tx_act[1] == 2)
		{
			tx_act[1] = 0;
			id = ((((unsigned long)mcp_txb[1].REG.TXB.SIDH.BYTE) << 3) & 0x7F8) | ((((unsigned long)mcp_txb[1].REG.TXB.SIDL.BYTE) >> 5) & 0x007);
			if(led_monit_ch & 8)
			{
			//	if(id == led_monit_id) PORTE.PODR.BIT.B0 = 1;
				if(id == led_monit_id && (led_monit_ch & 0x08) != 0)
				{
					led_monit_ch &= 0xF8;
					PORTE.PODR.BIT.B0 = 1;
					monit_timeover();
				}
			}
			can_tp_txecheck(3, id);	//	TP���M�����m�F
		}
		buf |= 0x08;
	}
	
	//	TXB2
	if(sts->BIT.TXB2R != 0)
	{
		if(tx_act[2] == 2) tx_act[2] = 3;
	}
	else
	{
		if(tx_act[2] == 3)
		{
			tx_act[2] = 0;
			id = ((((unsigned long)mcp_txb[2].REG.TXB.SIDH.BYTE) << 3) & 0x7F8) | ((((unsigned long)mcp_txb[2].REG.TXB.SIDL.BYTE) >> 5) & 0x007);
			if(led_monit_ch & 8)
			{
			//	if(id == led_monit_id) PORTE.PODR.BIT.B0 = 1;
				if(id == led_monit_id && (led_monit_ch & 0x08) != 0)
				{
					led_monit_ch &= 0xF8;
					PORTE.PODR.BIT.B0 = 1;
					monit_timeover();
				}
			}
			can_tp_txecheck(3, id);	//	TP���M�����m�F
		}
	}
	if(sts->BIT.TX2IF != 0)
	{	//	�I�����Ă���
		if(sts->BIT.TXB2R == 0 && tx_act[2] == 2)
		{
			tx_act[2] = 0;
			id = ((((unsigned long)mcp_txb[2].REG.TXB.SIDH.BYTE) << 3) & 0x7F8) | ((((unsigned long)mcp_txb[2].REG.TXB.SIDL.BYTE) >> 5) & 0x007);
			if(led_monit_ch & 8)
			{
			//	if(id == led_monit_id) PORTE.PODR.BIT.B0 = 1;
				if(id == led_monit_id && (led_monit_ch & 0x08) != 0)
				{
					led_monit_ch &= 0xF8;
					PORTE.PODR.BIT.B0 = 1;
					monit_timeover();
				}
			}
			can_tp_txecheck(3, id);	//	TP���M�����m�F
		}
		buf |= 0x10;
	}

	//	�t���O�N���A
	if(buf != 0)
	{
		buf |= 0xE0;
		can3_request(MCP2515CMD_BITX, MCP2515AD_CANINTF, 1, 0, 0, &buf);					//	RX0IF�N���A
	}
}

//---------------------------------------------------------------------------------------
//  ���荞�݌�X�e�[�^�X�擾���Callback����
//---------------------------------------------------------------------------------------
void can3_sts_event(MCP2515REG_INTERR *rxd)
{
	MCP2515REG_BITX	bc, ec;

	bc.BYTE.MSK.BYTE = rxd->BYTE.CANINTF.BYTE & 0xFC;
	bc.BYTE.PAT.BYTE = 0;

#if	1
	if(rxd->BYTE.CANINTF.BYTE == 0) return;
	if(rxd->BYTE.CANINTF.BIT.RX0IF)
	{	//	��M�o�b�t�@0�t��
		can3_request(MCP2515CMD_READ, MCP2515AD_RXB0CTRL, 0, 7, CAN3_CallbackRx0, 0);	//	RXB0�ǂݏo��
	}
	if(rxd->BYTE.CANINTF.BIT.RX1IF)
	{	//	��M�o�b�t�@1�t��
		can3_request(MCP2515CMD_READ, MCP2515AD_RXB1CTRL, 0, 7, CAN3_CallbackRx1, 0);	//	RXB1�ǂݏo��
	}
#else
	if(bc.BYTE.MSK.BYTE == 0) return;
#endif
	//	���M�����`�F�b�N
	if(rxd->BYTE.CANINTF.BIT.TX0IF)
	{	//	���M�o�b�t�@0�G���v�e�B
		tx_act[0] = 0;
	}
	if(rxd->BYTE.CANINTF.BIT.TX1IF)
	{	//	���M�o�b�t�@1�G���v�e�B
		tx_act[1] = 0;
	}
	if(rxd->BYTE.CANINTF.BIT.TX2IF)
	{	//	���M�o�b�t�@2�G���v�e�B
		tx_act[2] = 0;
	}
#if	1
	if(rxd->BYTE.CANINTF.BIT.ERRIF)
	{	//	�G���[���荞��
	//	tx_act[0] = 0;
	//	tx_act[1] = 0;
	//	tx_act[2] = 0;
		logging("CAN3:Error %02X\r", (int)rxd->BYTE.EFLG.BYTE);
		ec.BYTE.MSK.BYTE = rxd->BYTE.EFLG.BYTE;
		ec.BYTE.PAT.BYTE = 0;
		can3_request(MCP2515CMD_BITX, MCP2515AD_CANINTF, 1, 0, 0, &ec);	//	���荞�ݗv���t���O�N���A
	}
#endif
#if	1
	if(rxd->BYTE.CANINTF.BIT.WAKIF)
	{	//	�E�F�C�N�A�b�v���荞��
		logging("CAN3:Wakeup\r");
	}
	if(rxd->BYTE.CANINTF.BIT.MERRF)
	{	//	���b�Z�[�W�G���[���荞��
		logging("CAN3:MsgErr\r");
	}
#endif
	can3_request(MCP2515CMD_BITX, MCP2515AD_CANINTF, 1, 0, 0, &bc);	//	���荞�ݗv���t���O�N���A
}

//---------------------------------------------------------------------------------------
//  �@�\   : �O��CAN���W���[�����荞��(MCP2515 - INT)
//---------------------------------------------------------------------------------------
void interrupt __vectno__{VECT_ICU_IRQ0} ICU_IRQ0_ISR(void)
{
	can3_request(MCP2515CMD_STATUS, 0, 0, 1, can3_stat_event, 0);	//	�X�e�[�^�X�擾
//	can3_request(MCP2515CMD_READ, MCP2515AD_CANINTF, 0, 1, can3_sts_event, 0);		//	�X�e�[�^�X�����擾
}

//---------------------------------------------------------------------------------------
//  �@�\   : �O��CAN���W���[�����荞��(MCP2515 - RX0BF)
//---------------------------------------------------------------------------------------
void interrupt __vectno__{VECT_ICU_IRQ6} ICU_IRQ6_ISR(void)
{
	const unsigned short	buf = 0x0001;
	can3_request(MCP2515CMD_READ, MCP2515AD_RXB0CTRL, 0, 7, CAN3_CallbackRx0, 0);	//	RXB0�ǂݏo��
	can3_request(MCP2515CMD_BITX, MCP2515AD_CANINTF, 1, 0, 0, &buf);				//	RX0IF�N���A
}

//---------------------------------------------------------------------------------------
//  �@�\   : �O��CAN���W���[�����荞��(MCP2515 - RX1BF)
//---------------------------------------------------------------------------------------
void interrupt __vectno__{VECT_ICU_IRQ7} ICU_IRQ7_ISR(void)
{
	const unsigned short	buf = 0x0002;
	can3_request(MCP2515CMD_READ, MCP2515AD_RXB1CTRL, 0, 7, CAN3_CallbackRx1, 0);	//	RXB1�ǂݏo��
	can3_request(MCP2515CMD_BITX, MCP2515AD_CANINTF, 1, 0, 0, &buf);				//	RX0IF�N���A
}

#endif

