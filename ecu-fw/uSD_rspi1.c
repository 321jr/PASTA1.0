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
//	RX63N	uSD�p	RSPI1-I/F �ʐM
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

#include <sysio.h>
#include <string.h>
#include <stdio.h>
#include "iodefine.h"
#include "ecu.h"			/*	ECU ���ʒ�`			*/
#include "uSD_rspi1.h"

/*
	�|�[�g�ݒ�

			Port		SCI			I2C			SPI			�K�p
	----------------------------------------------------------------------------
	RSPI1	PE7									MISOB		<RSPI>		uSD
			PE6									MOSIB		<RSPI>		uSD
			PE5									RSPCKB		<RSPI>		uSD
			PE4									SSLB0		<RSPI>		uSD
*/

#ifdef		RSPI1_ACTIVATE

/*
//	uSD�p RSPI�Ǘ��\����
typedef struct	__spi_module__ {
	int				err;				//	�G���[�t���O
	void			*rx_proc;			//	��M�������荞�ݎ��Ăяo���֐�
	void			*tx_proc;			//	���M�������荞�ݎ��Ăяo���֐�
	void			*ti_proc;			//	�A�C�h�����O���荞�ݎ��Ăяo���֐�
	void			*err_proc;			//	�G���[�������荞�ݎ��Ăяo���֐�
}	SPI_MODULE;

*/
SPI_MODULE		usd_spi_com;

//	���O�@�\
void	logging(char *fmt, ...);

//________________________________________________________________________________________
//
//	rspi1_init
//----------------------------------------------------------------------------------------
//	�@�\����
//		RSPI1������
//					Port		SCI			I2C			SPI			�K�p
//			----------------------------------------------------------------------------
//			RSPI1	PE7									MISOB		<RSPI>		uSD
//					PE6									MOSIB		<RSPI>		uSD
//					PE5									RSPCKB		<RSPI>		uSD
//					PE4									SSLB0		<RSPI>		uSD
//	����
//		speed		�ʐM���x	100,000�`10,000,000
//	�߂�
//		����
//________________________________________________________________________________________
//
void rspi1_init(long bps)
{
	memset(&usd_spi_com, 0, sizeof(SPI_MODULE));

	SYSTEM.PRCR.WORD = 0xA502;	//	�v���e�N�g����
	MSTP_RSPI1 = 0;				//	RSPI1���W���[���X�g�b�v����

	//	RSPI1 ���荞�ݗv���֎~
	ICU.IER[IER_ICU_GROUPL0].BIT.IEN_ICU_GROUPL0 = 0;	//	�O���[�v12 �G���[���荞�݋֎~
	ICU.IER[IER_RSPI1_SPRI1].BIT.IEN_RSPI1_SPRI1 = 0;	//	��M�o�b�t�@�t�����荞�݋֎~
	ICU.IER[IER_RSPI1_SPTI1].BIT.IEN_RSPI1_SPTI1 = 0;	//	���M�G���v�e�B���荞�݋֎~
	ICU.IER[IER_RSPI1_SPII1].BIT.IEN_RSPI1_SPII1 = 0;	//	���M�A�C�h�����荞�݋֎~

	RSPI1.SPCR.BYTE = 0;		//	���W���[��������

	// CAN3�̒[�q�ݒ�(RSPI2�o�R)
	PORTE.PODR.BYTE = 0x70;		//	�|�[�g������
	PORTE.PDR.BIT.B7 = 0;		//	PB7 <- MISOB
	PORTE.PDR.BIT.B6 = 1;		//	PB6 -> MOSIB
	PORTE.PDR.BIT.B5 = 1;		//	PB5 -> RSPCKB
	PORTE.PDR.BIT.B4 = 1;		//	PB4 -> SSLB0

	PORTE.PMR.BIT.B7 = 1;		//	���Ӌ@�\	MISOB
	PORTE.PMR.BIT.B6 = 1;		//	���Ӌ@�\	MOSIB
	PORTE.PMR.BIT.B5 = 1;		//	���Ӌ@�\	RSPCKB
	PORTE.PMR.BIT.B4 = 1;		//	���Ӌ@�\	SSLB0

	MPC.PWPR.BIT.B0WI = 0;		//	
	MPC.PWPR.BIT.PFSWE = 1;		//	

	MPC.PE7PFS.BYTE = 0x0D;		//	MISOB		SO
	MPC.PE6PFS.BYTE = 0x0D;		//	MOSIB		SI
	MPC.PE5PFS.BYTE = 0x0D;		//	RSPCKB		SCK
	MPC.PE4PFS.BYTE = 0x0D;		//	SSLB0		/CS

	MPC.PWPR.BIT.PFSWE = 0;		//	
	MPC.PWPR.BIT.B0WI = 1;		//	

	SYSTEM.PRCR.WORD = 0xA500;	//	�|�[�g�ݒ�֎~
	
	//	RSPI1�̐ݒ�(�V���O���}�X�^���[�h)
	RSPI1.SSLP.BYTE = 0;		//	SSLnP �̓A�N�e�B�uLow
	RSPI1.SPPCR.BYTE = 0x20;	//	MOSI�̃A�C�h���o�͂�Low
	RSPI1.SPSR.BYTE &= 0;		//	�G���[�t���O����
	RSPI1.SPSCR.BYTE = 0;		//	�V�[�P���X�����l

	//	Set baud rate to 1Mbps	N�l(BRDV[1:0])=0 �Œ�	�ŏ�=93,750bps
	//	n = (PCLK Frequency) / (2 * 2^N * Bit Rate) - 1
	//	n = (48,000,000) / (2 * 2^0 * 1,000,000) - 1
	//	n = 24
	RSPI1.SPBR.BYTE = 48000000 / (2 * bps) - 1;
	RSPI1.SPDCR.BYTE = 0x20;	//	SPDR�̓����O���[�h�A�N�Z�X / ��M�o�b�t�@�ǂݏo�� / 1�t���[��
	RSPI1.SPCKD.BYTE = 0;		//	�N���b�N�x�� 1RSPCK
	RSPI1.SSLND.BYTE = 0;		//	SSL�l�Q�[�g�x�� 1RSPCK
	RSPI1.SPND.BYTE = 0;		//	���A�N�Z�X�x�� 1RSPCK + 2PCLK
	RSPI1.SPCR2.BYTE = 0;		//	�p���e�B���� / �A�C�h�����荞�݋֎~
	
	//	�R�}���h���W�X�^������
	//	RSPCK�ʑ��ݒ�r�b�g
	RSPI1.SPCMD0.BIT.CPHA = 0;		//	0 : ��G�b�W�Ńf�[�^�T���v���A�����G�b�W�Ńf�[�^�ω�
									//	1 : ��G�b�W�Ńf�[�^�ω��A�����G�b�W�Ńf�[�^�T���v��
	//	RSPCK�ɐ��ݒ�r�b�g
	RSPI1.SPCMD0.BIT.CPOL = 0;		//	0�F�A�C�h������RSPCK��Low
									//	1�F�A�C�h������RSPCK��High
	//	�r�b�g���[�g�����ݒ�r�b�g
	RSPI1.SPCMD0.BIT.BRDV = 0;		//	b3 b2
									//	0 0�F�x�[�X�̃r�b�g���[�g��I��
									//	0 1�F�x�[�X�̃r�b�g���[�g��2������I��
									//	1 0�F�x�[�X�̃r�b�g���[�g��4������I��
									//	1 1�F�x�[�X�̃r�b�g���[�g��8������I��
	//	SSL�M���A�T�[�g�ݒ�r�b�g
	RSPI1.SPCMD0.BIT.SSLA = 0;		//	b6 b4
									//	0 0 0�FSSL0
									//	0 0 1�FSSL1
									//	0 1 0�FSSL2
									//	0 1 1�FSSL3
									//	1 x x�F�ݒ肵�Ȃ��ł�������
									//	x�FDon�ft care
	//	SSL�M�����x���ێ��r�b�g
	RSPI1.SPCMD0.BIT.SSLKP = 0;		//	0�F�]���I�����ɑSSSL�M�����l�Q�[�g
									//	1�F�]���I���ォ�玟�A�N�Z�X�J�n�܂�SSL�M�����x����ێ�
	//	RSPI�f�[�^���ݒ�r�b�g
	RSPI1.SPCMD0.BIT.SPB = 4;		//	b11 b8
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
	RSPI1.SPCMD0.BIT.LSBF = 0;		//	0�FMSB�t�@�[�X�g
									//	1�FLSB�t�@�[�X�g
	//	RSPI���A�N�Z�X�x�����r�b�g
	RSPI1.SPCMD0.BIT.SPNDEN = 0;	//	0�F���A�N�Z�X�x����1RSPCK�{2PCLK
									//	1�F���A�N�Z�X�x����RSPI���A�N�Z�X�x�����W�X�^�iSPND�j�̐ݒ�l
	//	SSL�l�Q�[�g�x���ݒ苖�r�b�g
	RSPI1.SPCMD0.BIT.SLNDEN = 0;	//	0�FSSL�l�Q�[�g�x����1RSPCK
									//	1�FSSL�l�Q�[�g�x����RSPI�X���[�u�Z���N�g�l�Q�[�g�x�����W�X�^�iSSLND�j�̐ݒ�l
	//	RSPCK�x���ݒ苖�r�b�g
	RSPI1.SPCMD0.BIT.SCKDEN = 0;	//	0�FRSPCK�x����1RSPCK
									//	1�FRSPCK�x����RSPI�N���b�N�x�����W�X�^�iSPCKD�j�̐ݒ�l

	//	�ݒ�R�s�[
	RSPI1.SPCMD1.WORD = RSPI1.SPCMD0.WORD;
	RSPI1.SPCMD2.WORD = RSPI1.SPCMD0.WORD;
	RSPI1.SPCMD3.WORD = RSPI1.SPCMD0.WORD;
	RSPI1.SPCMD4.WORD = RSPI1.SPCMD0.WORD;
	RSPI1.SPCMD5.WORD = RSPI1.SPCMD0.WORD;
	RSPI1.SPCMD6.WORD = RSPI1.SPCMD0.WORD;
	RSPI1.SPCMD7.WORD = RSPI1.SPCMD0.WORD;
	
	//	���싖��
	//	RSPI���[�h�I���r�b�g
	RSPI1.SPCR.BIT.SPMS = 0;		//	0�FSPI����i4�����j
									//	1�F�N���b�N����������i3�����j
	//	�ʐM���샂�[�h�I���r�b�g
	RSPI1.SPCR.BIT.TXMD = 0;		//	0�F�S��d�������V���A���ʐM
									//	1�F���M����݂̂̃V���A���ʐM
	//	���[�h�t�H���g�G���[���o���r�b�g
	RSPI1.SPCR.BIT.MODFEN = 0;		//	0�F���[�h�t�H���g�G���[���o���֎~
									//	1�F���[�h�t�H���g�G���[���o������
	//	RSPI�}�X�^/�X���[�u���[�h�I���r�b�g
	RSPI1.SPCR.BIT.MSTR = 1;		//	0�F�X���[�u���[�h
									//	1�F�}�X�^���[�h
	//	RSPI�G���[���荞�݋��r�b�g
	RSPI1.SPCR.BIT.SPEIE = 0;		//	0�FRSPI�G���[���荞�ݗv���̔������֎~
									//	1�FRSPI�G���[���荞�ݗv���̔���������
	//	RSPI���M���荞�݋��r�b�g
	RSPI1.SPCR.BIT.SPTIE = 0;		//	0�FRSPI���M���荞�ݗv���̔������֎~
									//	1�FRSPI���M���荞�ݗv���̔���������
	//	RSPI��M���荞�݋��r�b�g
	RSPI1.SPCR.BIT.SPRIE = 0;		//	0�FRSPI��M���荞�ݗv���̔������֎~
									//	1�FRSPI��M���荞�ݗv���̔���������
	//	RSPI�A�C�h�����荞�݋��r�b�g
	RSPI1.SPCR2.BIT.SPIIE = 0;		//	0�F�A�C�h�����荞�ݗv���̔������֎~
									//	1�F�A�C�h�����荞�ݗv���̔���������
	//	RSPI�@�\���r�b�g
	RSPI1.SPCR.BIT.SPE = 1;			//	0�FRSPI�@�\�͖���
									//	1�FRSPI�@�\���L��

	//	���荞�ݗD�揇�ʐݒ�
	ICU.IPR[IPR_RSPI1_].BIT.IPR = 2;					//	���荞�݃��x���ݒ�
	//	���荞�݃t���O�N���A
	ICU.IR[IR_RSPI1_SPRI1].BIT.IR = 0;
	ICU.IR[IR_RSPI1_SPTI1].BIT.IR = 0;
	ICU.IR[IR_RSPI1_SPII1].BIT.IR = 0;
	//	GROUP12���荞�ݐݒ�
	ICU.GEN[GEN_RSPI1_SPEI1].BIT.EN_RSPI1_SPEI1 = 1;	//	�O���[�v12 RSPI1��M�G���[���荞�݋���
	ICU.IPR[IPR_ICU_GROUPL0].BIT.IPR = 1;				//	�O���\�v12 ���荞�݃��x���ݒ�
	ICU.IR[IR_ICU_GROUPL0].BIT.IR = 0;					//	�O���[�v12 ���荞�݃t���O�N���A
	//	���荞�݋��ݒ�
	ICU.IER[IER_RSPI1_SPRI1].BIT.IEN_RSPI1_SPRI1 = 1;	//	��M�o�b�t�@�t�����荞�݋���
	ICU.IER[IER_RSPI1_SPTI1].BIT.IEN_RSPI1_SPTI1 = 1;	//	���M�G���v�e�B���荞�݋���
	ICU.IER[IER_RSPI1_SPII1].BIT.IEN_RSPI1_SPII1 = 1;	//	���M�A�C�h�����荞�݋���
	ICU.IER[IER_ICU_GROUPL0].BIT.IEN_ICU_GROUPL0 = 1;	//	�O���[�v12 ���荞�݋���
}

//---------------------------------------------------------------------------------------
//	SPRI1 ��M�o�b�t�@�t��	SPI��M�o�b�t�@�Ƀf�[�^����
//---------------------------------------------------------------------------------------
void interrupt __vectno__{VECT_RSPI1_SPRI1} RSPI1_SPRI1_ISR(void)
{
	USD_PROC_CALL	proc;
	//	RSPI��M���荞�݋��r�b�g
	RSPI2.SPCR.BIT.SPRIE = 0;		//	0�FRSPI��M���荞�ݗv���̔������֎~
									//	1�FRSPI��M���荞�ݗv���̔���������
	if(usd_spi_com.rx_proc)
	{	//	���[�U�[�����֐��Ăяo��
		proc = (USD_PROC_CALL)usd_spi_com.rx_proc;
		proc(0);
	}
}

//---------------------------------------------------------------------------------------
//	SPTI1 ���M�o�b�t�@�G���v�e�B	SPI���M�o�b�t�@���󂢂�
//---------------------------------------------------------------------------------------
void interrupt __vectno__{VECT_RSPI1_SPTI1} RSPI1_SPTI1_ISR(void)
{
	USD_PROC_CALL	proc;
	//	RSPI���M���荞�݋��r�b�g
	RSPI2.SPCR.BIT.SPTIE = 0;		//	0�FRSPI���M���荞�ݗv���̔������֎~
									//	1�FRSPI���M���荞�ݗv���̔���������
	if(usd_spi_com.tx_proc)
	{	//	���[�U�[�����֐��Ăяo��
		proc = (USD_PROC_CALL)usd_spi_com.tx_proc;
		proc(0);
	}
}

//---------------------------------------------------------------------------------------
//	SPII1 �A�C�h��	���M�f�[�^�̍ŏI�������݊������Ɋ��荞�ݔ����i���M�����j
//---------------------------------------------------------------------------------------
void interrupt __vectno__{VECT_RSPI1_SPII1} RSPI1_SPII1_ISR(void)
{
	USD_PROC_CALL	proc;
	//	RSPI�A�C�h�����荞�݋��r�b�g
	RSPI1.SPCR2.BIT.SPIIE = 0;		//	0�F�A�C�h�����荞�ݗv���̔������֎~
									//	1�F�A�C�h�����荞�ݗv���̔���������
	if(usd_spi_com.ti_proc)
	{	//	���[�U�[�����֐��Ăяo��
		proc = (USD_PROC_CALL)usd_spi_com.ti_proc;
		proc(0);
	}
	else
	{	//	���荞�ݏ����N���A
		if(usd_spi_com.tx_proc)
		{	//	���[�U�[�����֐��Ăяo��
			proc = (USD_PROC_CALL)usd_spi_com.tx_proc;
			proc(0);
		}
	}
}

#endif		/*RSPI1_ACTIVATE*/
