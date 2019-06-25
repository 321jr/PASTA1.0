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
//	RX63N	SCI-I/F �ʐM
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
#include "sci.h"

/*
	�|�[�g�ݒ�

			Port		SCI			I2C			SPI			�K�p
	----------------------------------------------------------------------------
	SCI0	P20			TXD0		SDA0		SMOSI0		<RS-232C>	COM0
			P21			RXD0		SCL0		SMISO0		<RS-232C>	COM0
			P86			nRTS0								<RS-232C>	COM0
			P87			nCTS0								<RS-232C>	COM0
			P70			TX0SDN								<RS-232C>	���M����
															
	SCI1	P26			TXD1								<REM-MON>	COM1
			P30			RXD1								<REM-MON>	COM1
			PE1			nRTS1/(TXD12)						<RS-232C>	COM1/(COM12)
			PE2			nCTS1/(RXD12)						<RS-232C>	COM1/(COM12)
															
	SCI2	P13			TXD2		SDA0					<RS-232C>	COM2
			P12			RXD2		SCL0					<RS-232C>	COM2
			P15			nRTS2								<RS-232C>	COM2
			P17			nCTS2								<RS-232C>	COM2
			P73			TX2SDN								<RS-232C>	���M����
															
	SCI3	P23			TXD3								<RS-232C>	COM3
			P25			RXD3								<RS-232C>	COM3
			P22			nRTS3								<RS-232C>	COM3
			P24			nCTS3								<RS-232C>	COM3
			P56			nEXRES								</RESET>	�O�����W���[�����Z�b�g�M��
															
	SCI5	PC3			TXD5		SSDA5		SMOSI5		<SPI/I2C>	�O���g��
			PC2			RXD5		SSCL5		SMISO5		<SPI/I2C>	�O���g��
			PC4									SCK5		<SPI>		�O���g��
			PC5									SS0			<SPI>		�O���g��
			PC6									SS1			<SPI>		�O���g��
															
	SCI6	P00			TXD6		SSDA6		SMISO6		<TTL>		COM6
			P01			RXD6		SSCL6		SMOSI6		<TTL>		COM6
			P02			nRTS6					SCK6		<TTL>		COM6
			PJ3			nCTS6					SS6			<TTL>		COM6
*/

/*
//	SCI�o�b�t�@�T�C�Y
#define		BUFSIZE		1024
//	SCI0��RS-485����d�Ƃ��Ďg�p
#define		SCI0_RS485
//	SCI1��nRTS,nCTS���g�p
#define		SCI1_FLOW
//	SCI3��nCTS��P24�|�[�g�Ɋ��蓖��
#define		SCI3_nCTS
//	SCI1��nRTS/nCTS��SCI6�|�[�g�Ƃ��Ďg�p
#define		SCI6_ACTIVE
//	SCI�Ǘ��\����
typedef struct	__sci_module__ {
	unsigned char	txbuf[BUFSIZE];		//	���M�o�b�t�@
	unsigned char	rxbuf[BUFSIZE];		//	��M�o�b�t�@
	int				txwp;				//	���M�������݃|�C���^
	int				txrp;				//	���M�ǂݏo���|�C���^
	int				rxwp;				//	��M�������݃|�C���^
	int				rxrp;				//	��M�ǂݏo���|�C���^
	int				err;				//	���G���[�J�E���^
	int				perr;				//	�p���e�B�[�G���[�J�E���^
	int				ferr;				//	�t���[�~���O�G���[�J�E���^
	int				oerr;				//	�I�[�o�[�����G���[�J�E���^
}	SCI_MODULE;
*/
//	SCI0,SCI1,SCI2,SCI3,SCI5,SCI6	SCI1=�C�G���[�X�R�[�v�Ŏg�p
SCI_MODULE		sci_com[7];

//	���O�@�\
void	logging(char *fmt, ...);

//________________________________________________________________________________________
//
//	sci0_init
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI0������
//					Port		SCI			I2C			SPI			�K�p
//			----------------------------------------------------------------------------
//			SCI0	P20			TXD0		SDA0		SMOSI0		<RS-232C>	COM0
//					P21			RXD0		SCL0		SMISO0		<RS-232C>	COM0
//					P86			nRTS0								<RS-232C>	COM0
//					P87			nCTS0								<RS-232C>	COM0
//					P70			TX0SDN								<RS-232C>	���M����
//	����
//		speed		�ʐM���x	300�`115200
//		datalen		�f�[�^��	7,8
//		stoplen		�X�g�b�v��	1,2
//		parity		�p���e�B�[	0=���� / 1=� / 2=����
//	�߂�
//		����
//________________________________________________________________________________________
//
void sci0_init(long bps, int datalen, int stoplen, int parity)
{
	SCI_MODULE	*com = &sci_com[0];
	memset(com, 0, sizeof(SCI_MODULE));

#ifdef		SCI0_ACTIVATE

	SYSTEM.PRCR.WORD = 0xA502;		//	�v���e�N�g����
	MSTP_SCI0 = 0;					//	SCI���W���[���X�g�b�v����

	//	SCI ���荞�ݗv���֎~
	ICU.IER[IER_ICU_GROUPL0].BIT.IEN_ICU_GROUPL0 = 0;	//	�O���[�v12 ���荞�݋֎~
	ICU.IER[IER_SCI0_RXI0].BIT.IEN_SCI0_RXI0 = 0;		//	��M���荞�݋֎~
	ICU.IER[IER_SCI0_TXI0].BIT.IEN_SCI0_TXI0 = 0;		//	���M�������荞�݋֎~
	ICU.IER[IER_SCI0_TEI0].BIT.IEN_SCI0_TEI0 = 0;		//	���M�G���v�e�B���荞�݋֎~

	//	Enable write protection(�v���e�N�g�|����)
	SYSTEM.PRCR.WORD = 0xA500;		//	�v���e�N�g

	//	SCR - Serial Control Register
	//		b7		TIE	- Transmit Interrupt Enable		- A TXI interrupt request is disabled
	//		b6		RIE	- Receive Interrupt Enable		- RXI and ERI interrupt requests are disabled
	//		b5		TE	- Transmit Enable				- Serial transmission is disabled
	//		b4		RE	- Receive Enable				- Serial reception is disabled
	//		b2		TEIE - Transmit End Interrupt Enable - A TEI interrupt request is disabled
	SCI0.SCR.BYTE = 0x00;

	while (0x00 != (SCI0.SCR.BYTE & 0xF0))
	{
		//	Confirm that bit is actually 0
	}

	//	Set the I/O port functions

	//	�ėp�|�[�gP20:TXD0�AP21:RXD0�A(P22:DE/RE)
	PORT2.PODR.BIT.B0 = 1;		//	TXD
	PORT8.PODR.BIT.B6 = 1;		//	RTS
	//	Set port direction - TXDn is output port, RXDn is input port(�|�[�g���o�͐ݒ�)
	PORT2.PDR.BIT.B0 = 1;		//	�o��	TXD
	PORT2.PDR.BIT.B1 = 0;		//	����	RXD
	PORT8.PDR.BIT.B6 = 1;		//	�o��	RTS
	PORT8.PDR.BIT.B7 = 0;		//	����	CTS
	
	//	Set port mode - Use pin as general I/O port
	PORT2.PMR.BIT.B0 = 0;		//	�ėpIO�|�[�g�ݒ�
	PORT2.PMR.BIT.B1 = 0;		//	�ėpIO�|�[�g�ݒ�
	PORT8.PMR.BIT.B6 = 0;		//	�ėpIO�|�[�g�ݒ�
	PORT8.PMR.BIT.B7 = 0;		//	�ėpIO�|�[�g�ݒ�

	PORT7.PDR.BIT.B0 = 1;		//	�o��
	PORT7.PODR.BIT.B0 = 1;		//	0=SDN / 1=�ʏ�
	PORT7.PMR.BIT.B0 = 0;		//	�ėpIO�|�[�g�ݒ�

	//	PWPR - Write-Protect Register(�������݃v���e�N�g���W�X�^)
	//		b7		B0WI		- PFSWE Bit Write Disable	- PFSWE�֎~
	//		b6		PFSWE	- PFS Register Write Enable - PFS����
	//		b5:b0	Reserved - These bits are read as 0. The write value should be 0.
	MPC.PWPR.BIT.B0WI = 0;			//	���0�ɂ���
	MPC.PWPR.BIT.PFSWE = 1;			//	���1�ɂ���

	//	PFS - Pin Function Control Register(�s���t�@���N�V�������W�X�^�ݒ�)
	//		b3:b0	PSEL - Pin Function Select - RXDn, TXDn
	MPC.P20PFS.BYTE = 0x0A;			//	assign I/O pin to SCI0 TxD0
	MPC.P21PFS.BYTE = 0x0A;			//	assign I/O pin to SCI0 RxD0
	MPC.P86PFS.BYTE = 0x00;			//	assign I/O pin to Port
	MPC.P87PFS.BYTE = 0x00;			//	assign I/O pin to Port
	MPC.P07PFS.BYTE = 0x00;			//	assign I/O pin to Port
	//	�������݃v���e�N�g��������
	MPC.PWPR.BIT.PFSWE = 0;
	MPC.PWPR.BIT.B0WI = 1;

	//	Use pin as I/O port for peripheral functions(IO�s���@�\�ݒ�)
	PORT2.PMR.BIT.B0 = 1;			//	���Ӌ@�\�ݒ�
	PORT2.PMR.BIT.B1 = 1;			//	���Ӌ@�\�ݒ�

	//	Initialization of SCI
	//	�S���W���[���N���b�N�X�g�b�v���[�h�֎~
	SYSTEM.MSTPCRA.BIT.ACSE = 0;
	//	SCI0���W���[���X�g�b�v��Ԃ̉���
	MSTP_SCI0 = 0;


	//	Select an On-chip baud rate generator to the clock source
	SCI0.SCR.BIT.CKE = 0;

	//	SMR - Serial Mode Register
	//	b7		CM	- Communications Mode	- Asynchronous mode
	//	b6		CHR	- Character Length		- Selects 8 bits as the data length
	//	b5		PE	- Parity Enable			- When transmitting : Parity bit addition is not performed
	//								When receiving	: Parity bit checking is not performed
	//	b3		STOP - Stop Bit Length		- 2 stop bits
	//	b2		MP	- Multi-Processor Mode	- Multi-processor communications function is disabled
	//	b1:b0	CKS	- Clock Select			- PCLK clock (n = 0)
	SCI0.SMR.BYTE = 0x08;

	//	SCMR - Smart Card Mode Register
	//	b6:b4	Reserved - The write value should be 1.
	//	b3		SDIR		- Transmitted/Received Data Transfer Direction - Transfer with LSB-first
	//	b2		SINV		- Transmitted/Received Data Invert	- TDR contents are transmitted as they are. 
	//												Receive data is stored as it is in RDR.
	//	b1		Reserved - The write value should be 1.
	//	b0		SMIF		- Smart Card Interface Mode Select	- Serial communications interface mode
	SCI0.SCMR.BYTE = 0xF2;

	//	SEMR - Serial Extended Mode Register
	//	b7:b6	Reserved - The write value should be 0.
	//	b5		NFEN		- Digital Noise Filter Function Enable	- Noise cancellation function 
	//													for the RXDn input signal is disabled.
	//	b4		ABCS		- Asynchronous Mode Base Clock Select	- Selects 16 base clock cycles for 1-bit period
	//	b3:b1	Reserved - The write value should be 0.
	SCI0.SEMR.BYTE = 0x00;

	//	Set data transfer format in Serial Mode Register (SMR)*/ 
	//	-Asynchronous Mode`
	//	-8 bits
	//	-no parity
	//	-1 stop bit
	//	-PCLK clock (n = 0)
	SCI0.SMR.BYTE = 0x00;		//	0=PCLK, 1=PCLK/4, 2=PCLK/16, 3=PCLK/64

	//	BRR - Bit Rate Register
	//	Bit Rate: (48MHz/(64*2^(-1)*57600bps))-1=25.04
	if(stoplen == 1)
	{
		SCI0.SMR.BIT.STOP = 1;
	}
	else
	{
		SCI0.SMR.BIT.STOP = 0;
	}

	if(parity == 0)
	{
		SCI0.SMR.BIT.PE = 0;
	}
	else
	if(parity == 1)
	{		//�@��p���e�B
		SCI0.SMR.BIT.PE = 1;
		SCI0.SMR.BIT.PM = 1;
	}
	else
	if(parity == 2)
	{	//	�����p���e�B
		SCI0.SMR.BIT.PE = 1;
		SCI0.SMR.BIT.PM = 0;
	}

	if(datalen == 7)
	{		//	7bit��
		SCI0.SMR.BIT.CHR = 1;
	}
	else
	if(datalen == 8)
	{	//	8bit��
		SCI0.SMR.BIT.CHR = 0;
	}

	//	Set baud rate to 115200
	//	N = (PCLK Frequency) / (64 * 2^(2*n - 1) * Bit Rate) - 1
	//	N = (48,000,000) / (64 * 2^(2*0 - 1) * 115200) - 1
	//	N = 12
	SCI0.BRR = 48000000 / ((64/2) * bps) - 1;

	//	SCI���荞�ݗD�揇�ʐݒ�
	ICU.IPR[IPR_SCI0_].BIT.IPR = 1;						//	���荞�݃��x���ݒ�

	//	SCI0 ���荞�ݐݒ�
	ICU.IER[IER_SCI0_RXI0].BIT.IEN_SCI0_RXI0 = 1;		//	��M���荞��
	ICU.IER[IER_SCI0_TXI0].BIT.IEN_SCI0_TXI0 = 1;		//	���M�������荞��
	ICU.IER[IER_SCI0_TEI0].BIT.IEN_SCI0_TEI0 = 1;		//	���M�G���v�e�B���荞��

	//	���荞�݃t���O�N���A
	ICU.IR[IR_SCI0_RXI0].BIT.IR = 0;
	ICU.IR[IR_SCI0_TXI0].BIT.IR = 0;
	ICU.IR[IR_SCI0_TEI0].BIT.IR = 0;

	//	GROUP12���荞�ݐݒ�
	ICU.IER[IER_ICU_GROUPL0].BIT.IEN_ICU_GROUPL0 = 1;	//	�O���[�v12 ���荞�݋���
	ICU.GEN[GEN_SCI0_ERI0].BIT.EN_SCI0_ERI0 = 1;		//	�O���[�v12 SCI0��M�G���[���荞�݋���
	ICU.IPR[IPR_ICU_GROUPL0].BIT.IPR = 1;				//	�O���\�v12 ���荞�݃��x���ݒ�
	ICU.IR[IR_ICU_GROUPL0].BIT.IR = 0;					//	�O���[�v12 ���荞�݃t���O�N���A

	SCI0.SCR.BIT.RIE = 1;
	SCI0.SCR.BIT.RE = 1;

#endif		/*SCI0_ACTIVATE*/
}

//________________________________________________________________________________________
//
//	sci1_init
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI1������
//					Port		SCI			I2C			SPI			�K�p
//			----------------------------------------------------------------------------
//			SCI1	P26			TXD1								<REM-MON>	COM1
//					P30			RXD1								<REM-MON>	COM1
//					PE1			TXD12/nRTS1							<RS-232C>	COM1/12
//					PE2			RXD12/nCTS1							<RS-232C>	COM1/12
//	����
//		speed		�ʐM���x	300�`115200
//		datalen		�f�[�^��	7,8
//		stoplen		�X�g�b�v��	1,2
//		parity		�p���e�B�[	0=���� / 1=� / 2=����
//	�߂�
//		����
//	���l
//		SCI1�̓C�G���[�X�R�[�v�Ŏg�p
//________________________________________________________________________________________
//
#ifdef		SCI1_ACTIVATE
#ifdef	__YIDE_REM_DEBUG__
//	217
void interrupt sci1_rxi(void);
//	218
void interrupt sci1_txi(void);
//	219
void interrupt sci1_tei(void);
#endif
#endif		/*SCI1_ACTIVATE*/
void sci1_init(long bps, int datalen, int stoplen, int parity)
{
#ifdef		SCI1_ACTIVATE
#ifdef	__YIDE_REM_DEBUG__
	static unsigned long	*x_intb;
#endif
	unsigned short	i;
#endif		/*SCI1_ACTIVATE*/
	SCI_MODULE		*com = &sci_com[1];

	memset(com, 0, sizeof(SCI_MODULE));

#ifdef		SCI1_ACTIVATE

	SYSTEM.PRCR.WORD = 0xA502;		//	�v���e�N�g����
	MSTP_SCI1 = 0;					//	SCI���W���[���X�g�b�v����

	//	SCI ���荞�ݗv���֎~
	ICU.IER[IER_ICU_GROUPL0].BIT.IEN_ICU_GROUPL0 = 0;	//	�O���[�vL0 ���荞�݋֎~
	ICU.IER[IER_SCI1_RXI1].BIT.IEN_SCI1_RXI1 = 0;		//	��M���荞�݋֎~
	ICU.IER[IER_SCI1_TXI1].BIT.IEN_SCI1_TXI1 = 0;		//	���M�������荞�݋֎~
	ICU.IER[IER_SCI1_TEI1].BIT.IEN_SCI1_TEI1 = 0;		//	���M�G���v�e�B���荞�݋֎~

#ifdef	__YIDE_REM_DEBUG__
	//	���荞�݃x�N�^�̓��e�����������A�����[�g���j�^��������������
	_asm	extern	_x_intb
	_asm	MVFC	INTB, r0
	_asm	MOV.L	#_x_intb, r1
	_asm	MOV.L	r0, [r1]
	x_intb[VECT_SCI1_RXI1] = (unsigned long)sci1_rxi;
	x_intb[VECT_SCI1_TXI1] = (unsigned long)sci1_txi;
	x_intb[VECT_SCI1_TEI1] = (unsigned long)sci1_tei;
	
#endif

	//	Enable write protection(�v���e�N�g�|����)
	SYSTEM.PRCR.WORD = 0xA500;		//	�v���e�N�g

	//	SCR - Serial Control Register
	//	b7		TIE	- Transmit Interrupt Enable		- A TXI interrupt request is disabled
	//	b6		RIE	- Receive Interrupt Enable		- RXI and ERI interrupt requests are disabled
	//	b5		TE	- Transmit Enable				- Serial transmission is disabled
	//	b4		RE	- Receive Enable				- Serial reception is disabled
	//	b2		TEIE - Transmit End Interrupt Enable - A TEI interrupt request is disabled
	SCI1.SCR.BYTE = 0x00;

	while (0x00 != (SCI1.SCR.BYTE & 0xF0))
	{
		//	Confirm that bit is actually 0
	}

	//	Set the I/O port functions

	//	�ėp�|�[�gP26:TXD1�AP30:RXD1�A(PE1:RTS1�APE2:CTS1)
	PORT2.PODR.BIT.B6 = 1;		//	TXD
	PORTE.PODR.BIT.B1 = 1;		//	RTS=Disable
	//	Set port direction - TXDn is output port, RXDn is input port(�|�[�g���o�͐ݒ�)
	PORT2.PDR.BIT.B6 = 1;		//	�o��	TXD
	PORT3.PDR.BIT.B0 = 0;		//	����	RXD
	PORTE.PDR.BIT.B1 = 1;		//	�o��	RTS
	PORTE.PDR.BIT.B2 = 0;		//	����	CTS

	//	Set port mode - Use pin as general I/O port
	PORT2.PMR.BIT.B6 = 0;		//	�ėpIO�|�[�g�ݒ�
	PORT3.PMR.BIT.B0 = 0;		//	�ėpIO�|�[�g�ݒ�
	PORTE.PMR.BIT.B1 = 0;		//	�ėpIO�|�[�g�ݒ�
	PORTE.PMR.BIT.B2 = 0;		//	�ėpIO�|�[�g�ݒ�

	//	PWPR - Write-Protect Register(�������݃v���e�N�g���W�X�^)
	//	b7		B0WI		- PFSWE Bit Write Disable	- PFSWE�֎~
	//	b6		PFSWE	- PFS Register Write Enable - PFS����
	//	b5:b0	Reserved - These bits are read as 0. The write value should be 0.
	MPC.PWPR.BIT.B0WI = 0;			//	���0�ɂ���
	MPC.PWPR.BIT.PFSWE = 1;			//	���1�ɂ���
	
	MPC.P26PFS.BYTE = 0x0A;		//	assign I/O pin to SCI1 TxD1
	MPC.P30PFS.BYTE = 0x0A;		//	assign I/O pin to SCI1 RxD1
	MPC.PE1PFS.BYTE = 0x00;		//	assign I/O pin to Port
	MPC.PE2PFS.BYTE = 0x00;		//	assign I/O pin to Port

	//	�������݃v���e�N�g��������
	MPC.PWPR.BIT.PFSWE = 0;
	MPC.PWPR.BIT.B0WI = 1;

	PORT2.PMR.BIT.B6 = 1;		//	���Ӌ@�\�Ƃ��Ďg�p
	PORT3.PMR.BIT.B0 = 1;		//	���Ӌ@�\�Ƃ��Ďg�p

	//	�S���W���[���N���b�N�X�g�b�v���[�h�֎~
	SYSTEM.MSTPCRA.BIT.ACSE = 0;
	//	SCI1���W���[���X�g�b�v��Ԃ̉���
	MSTP_SCI1 = 0;
	
	SCI1.SCR.BYTE = 0x00;		//	Disable Tx/Rx and set clock to internal
	//	Set data transfer format in Serial Mode Register (SMR)
	//		-Asynchronous Mode`
	//		-8 bits
	//		-no parity
	//		-1 stop bit
	//		-PCLK clock (n = 0)
	SCI1.SMR.BYTE = 0x00;		//	0=PCLK, 1=PCLK/4, 2=PCLK/16, 3=PCLK/64

	if(stoplen == 1)
	{
		SCI1.SMR.BIT.STOP = 1;
	}
	else
	{
		SCI1.SMR.BIT.STOP = 0;
	}

	if(parity == 0)
	{
		SCI1.SMR.BIT.PE = 0;
	}
	else
	if(parity == 1)
	{		//�@��p���e�B
		SCI1.SMR.BIT.PE = 1;
		SCI1.SMR.BIT.PM = 1;
	}
	else
	if(parity == 2)
	{	//	�����p���e�B
		SCI1.SMR.BIT.PE = 1;
		SCI1.SMR.BIT.PM = 0;
	}

	if(datalen == 7)
	{		//	7bit��
		SCI1.SMR.BIT.CHR = 1;
	}
	else
	if(datalen == 8)
	{	//	8bit��
		SCI1.SMR.BIT.CHR = 0;
	}

	//	Set baud rate to 115200
	//		N = (PCLK Frequency) / (64 * 2^(2*n - 1) * Bit Rate) - 1
	//		N = (48,000,000) / (64 * 2^(2*0 - 1) * 115200) - 1
	//		N = 12

	SCI1.BRR = 48000000 / ((64/2) * bps) - 1;
	//			���N���b�N�����ݒ�ɂ���ĕύX����

	//	Wait at least one bit interval
	for ( i = 0; i < 5000; i++ );	//	assume minimum of 2 instructions at 98MHz ?

	//	SCI���荞�ݗD�揇�ʐݒ�
	ICU.IPR[IPR_SCI1_].BIT.IPR = 1;		//	���荞�݃��x���ݒ�

	//	SCI1 ���荞�ݐݒ�
	ICU.IER[IER_SCI1_RXI1].BIT.IEN_SCI1_RXI1 = 1;		//	��M���荞��
	ICU.IER[IER_SCI1_TXI1].BIT.IEN_SCI1_TXI1 = 1;		//	���M�������荞��
	ICU.IER[IER_SCI1_TEI1].BIT.IEN_SCI1_TEI1 = 1;		//	���M�G���v�e�B���荞��

	//	���荞�݃t���O�N���A
	ICU.IR[IR_SCI1_RXI1].BIT.IR = 0;
	ICU.IR[IR_SCI1_TXI1].BIT.IR = 0;
	ICU.IR[IR_SCI1_TEI1].BIT.IR = 0;

	//	GROUP12���荞�ݐݒ�
	ICU.IER[IER_ICU_GROUPL0].BIT.IEN_ICU_GROUPL0 = 1;	//	�O���[�v12 ���荞�݋���
	ICU.GEN[GEN_SCI1_ERI1].BIT.EN_SCI1_ERI1 = 1;		//	�O���[�v12 SCI1��M�G���[���荞�݋���
	ICU.IPR[IPR_ICU_GROUPL0].BIT.IPR = 1;				//	�O���\�v12 ���荞�݃��x���ݒ�
	ICU.IR[IR_ICU_GROUPL0].BIT.IR = 0;					//	�O���[�v12 ���荞�݃t���O�N���A

	SCI1.SCR.BIT.RIE = 1;
	SCI1.SCR.BIT.RE = 1;

#ifdef	SCI1_FLOW
	PORTE.PODR.BIT.B1 = 0;		//	RTS=Enable
#endif

#endif		/*SCI1_ACTIVATE*/

}

//________________________________________________________________________________________
//
//	sci2_init
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI2������
//					Port		SCI			I2C			SPI			�K�p
//			----------------------------------------------------------------------------
//			SCI2	P13			TXD2		SDA0					<RS-232C>	COM2
//					P12			RXD2		SCL0					<RS-232C>	COM2
//					P15			nRTS2								<RS-232C>	COM2
//					P17			nCTS2								<RS-232C>	COM2
//					P73			TX2SDN								<RS-232C>	���M����
//	����
//		speed		�ʐM���x	300�`115200
//		datalen		�f�[�^��	7,8
//		stoplen		�X�g�b�v��	1,2
//		parity		�p���e�B�[	0=���� / 1=� / 2=����
//	�߂�
//		����
//________________________________________________________________________________________
//
void sci2_init(long bps, int datalen, int stoplen, int parity)
{
	SCI_MODULE	*com = &sci_com[2];
	memset(com, 0, sizeof(SCI_MODULE));

#ifdef		SCI2_ACTIVATE

	SYSTEM.PRCR.WORD = 0xA502;		//	�v���e�N�g����
	MSTP_SCI2 = 0;					//	SCI���W���[���X�g�b�v����

	//	SCI ���荞�ݗv���֎~
	ICU.IER[IER_ICU_GROUPL0].BIT.IEN_ICU_GROUPL0 = 0;	//	�O���[�v12 ���荞�݋֎~
	ICU.IER[IER_SCI2_RXI2].BIT.IEN_SCI2_RXI2 = 0;		//	��M���荞�݋֎~
	ICU.IER[IER_SCI2_TXI2].BIT.IEN_SCI2_TXI2 = 0;		//	���M�������荞�݋֎~
	ICU.IER[IER_SCI2_TEI2].BIT.IEN_SCI2_TEI2 = 0;		//	���M�G���v�e�B���荞�݋֎~

	//	Enable write protection(�v���e�N�g�|����)
	SYSTEM.PRCR.WORD = 0xA500;		//	�v���e�N�g

	//	SCR - Serial Control Register
	//		b7		TIE	- Transmit Interrupt Enable		- A TXI interrupt request is disabled
	//		b6		RIE	- Receive Interrupt Enable		- RXI and ERI interrupt requests are disabled
	//		b5		TE	- Transmit Enable				- Serial transmission is disabled
	//		b4		RE	- Receive Enable				- Serial reception is disabled
	//		b2		TEIE - Transmit End Interrupt Enable - A TEI interrupt request is disabled
	SCI2.SCR.BYTE = 0x00;

	while (0x00 != (SCI2.SCR.BYTE & 0xF0))
	{
		//	Confirm that bit is actually 0
	}

	//	Set the I/O port functions

	//	�ėp�|�[�gP13:TXD2�AP12:RXD2
	PORT1.PODR.BIT.B3 = 1;	//	TXD
	PORT1.PODR.BIT.B5 = 1;	//	RTS=Disable
	//	Set port direction - TXDn is output port, RXDn is input port(�|�[�g���o�͐ݒ�)
	PORT1.PDR.BIT.B3 = 1;		//	�o��	TXD
	PORT1.PDR.BIT.B2 = 0;		//	����	RXD
	PORT1.PDR.BIT.B5 = 1;		//	�o��	RTS
	PORT1.PDR.BIT.B7 = 0;		//	����	CTS

	//	Set port mode - Use pin as general I/O port
	PORT1.PMR.BIT.B3 = 0;		//	�ėpIO�|�[�g�ݒ�
	PORT1.PMR.BIT.B2 = 0;		//	�ėpIO�|�[�g�ݒ�
	PORT1.PMR.BIT.B5 = 0;		//	�ėpIO�|�[�g�ݒ�
	PORT1.PMR.BIT.B7 = 0;		//	�ėpIO�|�[�g�ݒ�

	//	PWPR - Write-Protect Register(�������݃v���e�N�g���W�X�^)
	//		b7		B0WI		- PFSWE Bit Write Disable	- PFSWE�֎~
	//		b6		PFSWE	- PFS Register Write Enable - PFS����
	//		b5:b0	Reserved - These bits are read as 0. The write value should be 0.
	MPC.PWPR.BIT.B0WI = 0;			//	���0�ɂ���
	MPC.PWPR.BIT.PFSWE = 1;			//	���1�ɂ���

	//	PFS - Pin Function Control Register(�s���t�@���N�V�������W�X�^�ݒ�)
	//		b3:b0	PSEL - Pin Function Select - RXDn, TXDn
	MPC.P13PFS.BYTE = 0x0A;		//	assign I/O pin to SCI0 TxD3
	MPC.P12PFS.BYTE = 0x0A;		//	assign I/O pin to SCI0 RxD3
	MPC.P15PFS.BYTE = 0x00;		//	assign I/O pin to Port
	MPC.P17PFS.BYTE = 0x00;		//	assign I/O pin to Port
	//	�������݃v���e�N�g��������
	MPC.PWPR.BIT.PFSWE = 0;
	MPC.PWPR.BIT.B0WI = 1;

	//	Use pin as I/O port for peripheral functions(IO�s���@�\�ݒ�)
	PORT1.PMR.BIT.B3 = 1;			//	���Ӌ@�\�ݒ�
	PORT1.PMR.BIT.B2 = 1;			//	���Ӌ@�\�ݒ�

	//	Initialization of SCI
	//	�S���W���[���N���b�N�X�g�b�v���[�h�֎~
	SYSTEM.MSTPCRA.BIT.ACSE = 0;
	//	SCI2���W���[���X�g�b�v��Ԃ̉���
	MSTP_SCI2 = 0;


	//	Select an On-chip baud rate generator to the clock source
	SCI2.SCR.BIT.CKE = 0;

	//	SMR - Serial Mode Register
	//		b7		CM	- Communications Mode	- Asynchronous mode
	//		b6		CHR	- Character Length		- Selects 8 bits as the data length
	//		b5		PE	- Parity Enable			- When transmitting : Parity bit addition is not performed
	//									When receiving	: Parity bit checking is not performed
	//		b3		STOP - Stop Bit Length		- 2 stop bits
	//		b2		MP	- Multi-Processor Mode	- Multi-processor communications function is disabled
	//		b1:b0	CKS	- Clock Select			- PCLK clock (n = 0)
	SCI2.SMR.BYTE = 0x08;

	//	SCMR - Smart Card Mode Register
	//		b6:b4	Reserved - The write value should be 1.
	//		b3		SDIR		- Transmitted/Received Data Transfer Direction - Transfer with LSB-first
	//		b2		SINV		- Transmitted/Received Data Invert	- TDR contents are transmitted as they are. 
	//													Receive data is stored as it is in RDR.
	//		b1		Reserved - The write value should be 1.
	//		b0		SMIF		- Smart Card Interface Mode Select	- Serial communications interface mode
	SCI2.SCMR.BYTE = 0xF2;

	//	SEMR - Serial Extended Mode Register
	//		b7:b6	Reserved - The write value should be 0.
	//		b5		NFEN		- Digital Noise Filter Function Enable	- Noise cancellation function 
	//														for the RXDn input signal is disabled.
	//		b4		ABCS		- Asynchronous Mode Base Clock Select	- Selects 16 base clock cycles for 1-bit period
	//		b3:b1	Reserved - The write value should be 0.
	SCI2.SEMR.BYTE = 0x00;

	//	Set data transfer format in Serial Mode Register (SMR)
	//		-Asynchronous Mode`
	//		-8 bits
	//		-no parity
	//		-1 stop bit
	//		-PCLK clock (n = 0)
	SCI2.SMR.BYTE = 0x00;		//	0=PCLK, 1=PCLK/4, 2=PCLK/16, 3=PCLK/64

	//	BRR - Bit Rate Register
	//		Bit Rate: (48MHz/(64*2^(-1)*57600bps))-1=25.04
	if(stoplen == 1)
	{
		SCI2.SMR.BIT.STOP = 1;
	}
	else
	{
		SCI2.SMR.BIT.STOP = 0;
	}

	if(parity == 0)
	{
		SCI2.SMR.BIT.PE = 0;
	}
	else
	if(parity == 1)
	{		//�@��p���e�B
		SCI2.SMR.BIT.PE = 1;
		SCI2.SMR.BIT.PM = 1;
	}
	else
	if(parity == 2)
	{	//	�����p���e�B
		SCI2.SMR.BIT.PE = 1;
		SCI2.SMR.BIT.PM = 0;
	}

	if(datalen == 7)
	{		//	7bit��
		SCI2.SMR.BIT.CHR = 1;
	}
	else
	if(datalen == 8)
	{	//	8bit��
		SCI2.SMR.BIT.CHR = 0;
	}

	//	Set baud rate to 115200
	//		N = (PCLK Frequency) / (64 * 2^(2*n - 1) * Bit Rate) - 1
	//		N = (48,000,000) / (64 * 2^(2*0 - 1) * 115200) - 1
	//		N = 12
	SCI2.BRR = 48000000 / ((64/2) * bps) - 1;

	//	SCI���荞�ݗD�揇�ʐݒ�
	ICU.IPR[IPR_SCI2_].BIT.IPR = 1;						//	���荞�݃��x���ݒ�

	//	SCI2 ���荞�ݐݒ�
	ICU.IER[IER_SCI2_RXI2].BIT.IEN_SCI2_RXI2 = 1;		//	��M���荞��
	ICU.IER[IER_SCI2_TXI2].BIT.IEN_SCI2_TXI2 = 1;		//	���M�������荞��
	ICU.IER[IER_SCI2_TEI2].BIT.IEN_SCI2_TEI2 = 1;		//	���M�G���v�e�B���荞��

	//	���荞�݃t���O�N���A
	ICU.IR[IR_SCI2_RXI2].BIT.IR = 0;
	ICU.IR[IR_SCI2_TXI2].BIT.IR = 0;
	ICU.IR[IR_SCI2_TEI2].BIT.IR = 0;

	//	GROUP12���荞�ݐݒ�
	ICU.IER[IER_ICU_GROUPL0].BIT.IEN_ICU_GROUPL0 = 1;	//	�O���[�v12 ���荞�݋���
	ICU.GEN[GEN_SCI2_ERI2].BIT.EN_SCI2_ERI2 = 1;		//	�O���[�v12 SCI2��M�G���[���荞�݋���
	ICU.IPR[IPR_ICU_GROUPL0].BIT.IPR = 1;				//	�O���\�v12 ���荞�݃��x���ݒ�
	ICU.IR[IR_ICU_GROUPL0].BIT.IR = 0;					//	�O���[�v12 ���荞�݃t���O�N���A

	SCI2.SCR.BIT.RIE = 1;
	SCI2.SCR.BIT.RE = 1;

	PORT1.PODR.BIT.B5 = 0;	//	RTS=Enable

#endif		/*SCI2_ACTIVATE*/
}

//________________________________________________________________________________________
//
//	sci3_init
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI3������
//					Port		SCI			I2C			SPI			�K�p
//			----------------------------------------------------------------------------
//			SCI3	P23			TXD3								<RS-232C>	COM3
//					P25			RXD3								<RS-232C>	COM3
//					P22			nRTS3								<RS-232C>	COM3
//					P24			nCTS3								<RS-232C>	COM3
//					P56			nEXRES								</RESET>	�O�����W���[�����Z�b�g�M��
//	����
//		speed		�ʐM���x	300�`115200
//		datalen		�f�[�^��	7,8
//		stoplen		�X�g�b�v��	1,2
//		parity		�p���e�B�[	0=���� / 1=� / 2=����
//	�߂�
//		����
//________________________________________________________________________________________
//
void sci3_init(long bps, int datalen, int stoplen, int parity)
{
	SCI_MODULE	*com = &sci_com[3];
	memset(com, 0, sizeof(SCI_MODULE));

#ifdef		SCI3_ACTIVATE

	SYSTEM.PRCR.WORD = 0xA502;		//	�v���e�N�g����
	MSTP_SCI3 = 0;					//	SCI���W���[���X�g�b�v����

	//	SCI ���荞�ݗv���֎~
	ICU.IER[IER_ICU_GROUPL0].BIT.IEN_ICU_GROUPL0 = 0;	//	�O���[�v12 ���荞�݋֎~
	ICU.IER[IER_SCI3_RXI3].BIT.IEN_SCI3_RXI3 = 0;		//	��M���荞�݋֎~
	ICU.IER[IER_SCI3_TXI3].BIT.IEN_SCI3_TXI3 = 0;		//	���M�������荞�݋֎~
	ICU.IER[IER_SCI3_TEI3].BIT.IEN_SCI3_TEI3 = 0;		//	���M�G���v�e�B���荞�݋֎~

	//	Enable write protection(�v���e�N�g�|����)
	SYSTEM.PRCR.WORD = 0xA500;		//	�v���e�N�g

	//	SCR - Serial Control Register
	//	b7		TIE	- Transmit Interrupt Enable		- A TXI interrupt request is disabled
	//	b6		RIE	- Receive Interrupt Enable		- RXI and ERI interrupt requests are disabled
	//	b5		TE	- Transmit Enable				- Serial transmission is disabled
	//	b4		RE	- Receive Enable				- Serial reception is disabled
	//	b2		TEIE - Transmit End Interrupt Enable - A TEI interrupt request is disabled
	SCI3.SCR.BYTE = 0x00;

	while (0x00 != (SCI3.SCR.BYTE & 0xF0))
	{
		//	Confirm that bit is actually 0
	}

	//	Set the I/O port functions

	//	�ėp�|�[�gP23:TXD3�AP24:RXD3
	PORT2.PODR.BIT.B3 = 1;		//	TXD
	PORT2.PODR.BIT.B2 = 1;		//	RTS=Disable
	//	Set port direction - TXDn is output port, nCTS/RXDn is input port(�|�[�g���o�͐ݒ�)
	PORT2.PDR.BIT.B3 = 1;		//	�o��	TXD
	PORT2.PDR.BIT.B5 = 0;		//	����	RXD
	PORT2.PDR.BIT.B2 = 1;		//	�o��	RTS
	PORT2.PDR.BIT.B4 = 0;		//	����	CTS

	//	Set port mode - Use pin as general I/O port
	PORT2.PMR.BIT.B3 = 0;		//	�ėpIO�|�[�g�ݒ�
	PORT2.PMR.BIT.B5 = 0;		//	�ėpIO�|�[�g�ݒ�
	PORT2.PMR.BIT.B2 = 0;		//	�ėpIO�|�[�g�ݒ�
	PORT2.PMR.BIT.B4 = 0;		//	�ėpIO�|�[�g�ݒ�

	//	PWPR - Write-Protect Register(�������݃v���e�N�g���W�X�^)
	//	b7		B0WI		- PFSWE Bit Write Disable	- PFSWE�֎~
	//	b6		PFSWE	- PFS Register Write Enable - PFS����
	//	b5:b0	Reserved - These bits are read as 0. The write value should be 0.
	MPC.PWPR.BIT.B0WI = 0;			//	���0�ɂ���
	MPC.PWPR.BIT.PFSWE = 1;			//	���1�ɂ���

	//	PFS - Pin Function Control Register(�s���t�@���N�V�������W�X�^�ݒ�)
	//	b3:b0	PSEL - Pin Function Select - RXDn, TXDn
	MPC.P23PFS.BYTE = 0x0A;		//	assign I/O pin to SCI3 TxD3
	MPC.P25PFS.BYTE = 0x0A;		//	assign I/O pin to SCI3 RxD3
	MPC.P22PFS.BYTE = 0x00;		//	assign I/O pin to Port
	MPC.P24PFS.BYTE = 0x00;		//	assign I/O pin to Port
	//	�������݃v���e�N�g��������
	MPC.PWPR.BIT.PFSWE = 0;
	MPC.PWPR.BIT.B0WI = 1;

	//	Use pin as I/O port for peripheral functions(IO�s���@�\�ݒ�)
	PORT2.PMR.BIT.B3 = 1;			//	���Ӌ@�\�ݒ�
	PORT2.PMR.BIT.B5 = 1;			//	���Ӌ@�\�ݒ�

	//	Initialization of SCI


	//	Select an On-chip baud rate generator to the clock source
	SCI3.SCR.BIT.CKE = 0;

	//	SMR - Serial Mode Register
	//	b7		CM	- Communications Mode	- Asynchronous mode
	//	b6		CHR	- Character Length		- Selects 8 bits as the data length
	//	b5		PE	- Parity Enable			- When transmitting : Parity bit addition is not performed
	//								When receiving	: Parity bit checking is not performed
	//	b3		STOP - Stop Bit Length		- 2 stop bits
	//	b2		MP	- Multi-Processor Mode	- Multi-processor communications function is disabled
	//	b1:b0	CKS	- Clock Select			- PCLK clock (n = 0)
	SCI3.SMR.BYTE = 0x08;

	//	SCMR - Smart Card Mode Register
	//	b6:b4	Reserved - The write value should be 1.
	//	b3		SDIR		- Transmitted/Received Data Transfer Direction - Transfer with LSB-first
	//	b2		SINV		- Transmitted/Received Data Invert	- TDR contents are transmitted as they are. 
	//												Receive data is stored as it is in RDR.
	//	b1		Reserved - The write value should be 1.
	//	b0		SMIF		- Smart Card Interface Mode Select	- Serial communications interface mode
	SCI3.SCMR.BYTE = 0xF2;

	//	SEMR - Serial Extended Mode Register
	//	b7:b6	Reserved - The write value should be 0.
	//	b5		NFEN		- Digital Noise Filter Function Enable	- Noise cancellation function 
	//													for the RXDn input signal is disabled.
	//	b4		ABCS		- Asynchronous Mode Base Clock Select	- Selects 16 base clock cycles for 1-bit period
	//	b3:b1	Reserved - The write value should be 0.
	SCI3.SEMR.BYTE = 0x00;

	//	Set data transfer format in Serial Mode Register (SMR)
	//		-Asynchronous Mode`
	//		-8 bits
	//		-no parity
	//		-1 stop bit
	//		-PCLK clock (n = 0)
	SCI3.SMR.BYTE = 0x00;		//	0=PCLK, 1=PCLK/4, 2=PCLK/16, 3=PCLK/64

	//	BRR - Bit Rate Register
	//	Bit Rate: (48MHz/(64*2^(-1)*57600bps))-1=25.04
	if(stoplen == 1)
	{
		SCI3.SMR.BIT.STOP = 1;
	}
	else
	{
		SCI3.SMR.BIT.STOP = 0;
	}

	if(parity == 0)
	{
		SCI3.SMR.BIT.PE = 0;
	}
	else
	if(parity == 1)
	{		//�@��p���e�B
		SCI3.SMR.BIT.PE = 1;
		SCI3.SMR.BIT.PM = 1;
	}
	else
	if(parity == 2)
	{	//	�����p���e�B
		SCI3.SMR.BIT.PE = 1;
		SCI3.SMR.BIT.PM = 0;
	}

	if(datalen == 7)
	{		//	7bit��
		SCI3.SMR.BIT.CHR = 1;
	}
	else
	if(datalen == 8)
	{	//	8bit��
		SCI3.SMR.BIT.CHR = 0;
	}

	//	Set baud rate to 115200
	//	N = (PCLK Frequency) / (64 * 2^(2*n - 1) * Bit Rate) - 1
	//	N = (48,000,000) / (64 * 2^(2*0 - 1) * 115200) - 1
	//	N = 12
	SCI3.BRR = 48000000 / ((64/2) * bps) - 1;

	//	SCI���荞�ݗD�揇�ʐݒ�
	ICU.IPR[IPR_SCI3_].BIT.IPR = 1;						//	���荞�݃��x���ݒ�

	//	SCI3 ���荞�ݐݒ�
	ICU.IER[IER_SCI3_RXI3].BIT.IEN_SCI3_RXI3 = 1;		//	��M���荞��
	ICU.IER[IER_SCI3_TXI3].BIT.IEN_SCI3_TXI3 = 1;		//	���M�������荞��
	ICU.IER[IER_SCI3_TEI3].BIT.IEN_SCI3_TEI3 = 1;		//	���M�G���v�e�B���荞��

	//	���荞�݃t���O�N���A
	ICU.IR[IR_SCI3_RXI3].BIT.IR = 0;
	ICU.IR[IR_SCI3_TXI3].BIT.IR = 0;
	ICU.IR[IR_SCI3_TEI3].BIT.IR = 0;

	//	GROUP12���荞�ݐݒ�
	ICU.IER[IER_ICU_GROUPL0].BIT.IEN_ICU_GROUPL0 = 1;	//	�O���[�v12 ���荞�݋���
	ICU.GEN[GEN_SCI3_ERI3].BIT.EN_SCI3_ERI3 = 1;		//	�O���[�v12 SCI3��M�G���[���荞�݋���
	ICU.IPR[IPR_ICU_GROUPL0].BIT.IPR = 1;				//	�O���\�v12 ���荞�݃��x���ݒ�
	ICU.IR[IR_ICU_GROUPL0].BIT.IR = 0;					//	�O���[�v12 ���荞�݃t���O�N���A

	SCI3.SCR.BIT.RIE = 1;
	SCI3.SCR.BIT.RE = 1;

	PORT2.PODR.BIT.B2 = 0;		//	RTS=Enable

#endif		/*SCI3_ACTIVATE*/
}

//________________________________________________________________________________________
//
//	spi5_init
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI5���Ȉ�SPI���[�h�ŏ�����
//					Port		SCI			I2C			SPI			�K�p
//			----------------------------------------------------------------------------
//			SCI5	PC3			TXD5					SMOSI5		<SPI>		�O���g��
//					PC2			RXD5					SMISO5		<SPI>		�O���g��
//					PC4									SCK5		<SPI>		�O���g��
//					PC5									SS0			<SPI>		�O���g��
//					PC6									SS1			<SPI>		�O���g��
//	����
//		speed		�ʐM���x
//	�߂�
//		����
//________________________________________________________________________________________
//
void spi5_init(long bps)
{
	SCI_MODULE	*com = &sci_com[5];
	memset(com, 0, sizeof(SCI_MODULE));

#ifdef		SCI5_ACTIVATE

	SYSTEM.PRCR.WORD = 0xA502;		//	�v���e�N�g����
	MSTP_SCI5 = 0;					//	SCI���W���[���X�g�b�v����

	//	SCI ���荞�ݗv���֎~
	ICU.IER[IER_ICU_GROUPL0].BIT.IEN_ICU_GROUPL0 = 0;	//	�O���[�v12 ���荞�݋֎~
	ICU.IER[IER_SCI5_RXI5].BIT.IEN_SCI5_RXI5 = 0;		//	��M���荞�݋֎~
	ICU.IER[IER_SCI5_TXI5].BIT.IEN_SCI5_TXI5 = 0;		//	���M�������荞�݋֎~
	ICU.IER[IER_SCI5_TEI5].BIT.IEN_SCI5_TEI5 = 0;		//	���M�G���v�e�B���荞�݋֎~

	//	Enable write protection(�v���e�N�g�|����)
	SYSTEM.PRCR.WORD = 0xA500;		//	�v���e�N�g

	//	SCR - Serial Control Register
	//	b7		TIE	- Transmit Interrupt Enable		- A TXI interrupt request is disabled
	//	b6		RIE	- Receive Interrupt Enable		- RXI and ERI interrupt requests are disabled
	//	b5		TE	- Transmit Enable				- Serial transmission is disabled
	//	b4		RE	- Receive Enable				- Serial reception is disabled
	//	b2		TEIE - Transmit End Interrupt Enable - A TEI interrupt request is disabled
	SCI5.SCR.BYTE = 0x00;

	while (0x00 != (SCI5.SCR.BYTE & 0xF0))
	{
		//	Confirm that bit is actually 0
	}

	//	Set the I/O port functions

	//	�ėp�|�[�gPC3:TXD5�APC2:RXD5
	PORTC.PODR.BIT.B3 = 1;		//	SMOSI5
	PORTC.PODR.BIT.B4 = 1;		//	SCK5
	PORTC.PODR.BIT.B5 = 1;		//	SS0=Disable
	PORTC.PODR.BIT.B6 = 1;		//	SS1=Disable
	//	Set port direction - TXDn is output port, RXDn is input port(�|�[�g���o�͐ݒ�)
	PORTC.PDR.BIT.B3 = 1;		//	�o��	MOSI
	PORTC.PDR.BIT.B2 = 0;		//	����	MISO
	PORTC.PDR.BIT.B4 = 1;		//	�o��	SCK5
	PORTC.PDR.BIT.B5 = 1;		//	�o��	SS0
	PORTC.PDR.BIT.B6 = 1;		//	�o��	SS1

	//	Set port mode - Use pin as general I/O port
	PORTC.PMR.BIT.B3 = 0;		//	�ėpIO�|�[�g�ݒ�
	PORTC.PMR.BIT.B2 = 0;		//	�ėpIO�|�[�g�ݒ�
	PORTC.PMR.BIT.B4 = 0;		//	�ėpIO�|�[�g�ݒ�
	PORTC.PMR.BIT.B5 = 0;		//	�ėpIO�|�[�g�ݒ�
	PORTC.PMR.BIT.B6 = 0;		//	�ėpIO�|�[�g�ݒ�

	//	PWPR - Write-Protect Register(�������݃v���e�N�g���W�X�^)
	//	b7		B0WI		- PFSWE Bit Write Disable	- PFSWE�֎~
	//	b6		PFSWE	- PFS Register Write Enable - PFS����
	//	b5:b0	Reserved - These bits are read as 0. The write value should be 0.
	MPC.PWPR.BIT.B0WI = 0;			//	���0�ɂ���
	MPC.PWPR.BIT.PFSWE = 1;			//	���1�ɂ���

	//	PFS - Pin Function Control Register(�s���t�@���N�V�������W�X�^�ݒ�)
	//	b3:b0	PSEL - Pin Function Select - RXDn, TXDn
	MPC.PC3PFS.BYTE = 0x0A;		//	assign I/O pin to SCI5 TXD5
	MPC.PC2PFS.BYTE = 0x0A;		//	assign I/O pin to SCI5 RXD5
	MPC.PC4PFS.BYTE = 0x0A;		//	assign I/O pin to SCI5 SCK5
	MPC.PC5PFS.BYTE = 0x00;		//	assign I/O pin to port
	MPC.PC6PFS.BYTE = 0x00;		//	assign I/O pin to port
	//	�������݃v���e�N�g��������
	MPC.PWPR.BIT.PFSWE = 0;
	MPC.PWPR.BIT.B0WI = 1;

	//	Use pin as I/O port for peripheral functions(IO�s���@�\�ݒ�)
	PORTC.PMR.BIT.B3 = 1;			//	���Ӌ@�\�ݒ�
	PORTC.PMR.BIT.B2 = 1;			//	���Ӌ@�\�ݒ�
	PORTC.PMR.BIT.B4 = 1;			//	���Ӌ@�\�ݒ�

	//	Initialization of SCI


	//	Select an On-chip baud rate generator to the clock source
	SCI5.SCR.BIT.CKE = 0;

	//	SMR - Serial Mode Register
	//	b7		CM	- Communications Mode	- Asynchronous mode
	//	b6		CHR	- Character Length		- Selects 8 bits as the data length
	//	b5		PE	- Parity Enable			- When transmitting : Parity bit addition is not performed
	//								When receiving	: Parity bit checking is not performed
	//	b3		STOP - Stop Bit Length		- 2 stop bits
	//	b2		MP	- Multi-Processor Mode	- Multi-processor communications function is disabled
	//	b1:b0	CKS	- Clock Select			- PCLK clock (n = 0)
	SCI5.SMR.BYTE = 0x88;

	//	SCMR - Smart Card Mode Register
	//	b6:b4	Reserved - The write value should be 1.
	//	b3		SDIR		- Transmitted/Received Data Transfer Direction - Transfer with LSB-first
	//	b2		SINV		- Transmitted/Received Data Invert	- TDR contents are transmitted as they are. 
	//												Receive data is stored as it is in RDR.
	//	b1		Reserved - The write value should be 1.
	//	b0		SMIF		- Smart Card Interface Mode Select	- Serial communications interface mode
	SCI5.SCMR.BYTE = 0xF2;

	//	SEMR - Serial Extended Mode Register
	//	b7:b6	Reserved - The write value should be 0.
	//	b5		NFEN		- Digital Noise Filter Function Enable	- Noise cancellation function 
	//													for the RXDn input signal is disabled.
	//	b4		ABCS		- Asynchronous Mode Base Clock Select	- Selects 16 base clock cycles for 1-bit period
	//	b3:b1	Reserved - The write value should be 0.
	SCI5.SEMR.BYTE = 0x00;

	//	Set data transfer format in Serial Mode Register (SMR)*/ 
	//		-Asynchronous Mode`
	//		-8 bits
	//		-no parity
	//		-1 stop bit
	//		-PCLK clock (n = 0)
	SCI5.SMR.BYTE = 0x00;		//	0=PCLK, 1=PCLK/4, 2=PCLK/16, 3=PCLK/64

	//	BRR - Bit Rate Register
	//	Bit Rate: (48MHz/(64*2^(-1)*57600bps))-1=25.04
	//	Set baud rate to 115200
	//	N = (PCLK Frequency) / (64 * 2^(2*n - 1) * Bit Rate) - 1
	//	N = (48,000,000) / (64 * 2^(2*0 - 1) * 115200) - 1
	//	N = 12
	SCI5.BRR = 48000000 / ((64/2) * bps) - 1;

	//	SCI���荞�ݗD�揇�ʐݒ�
	ICU.IPR[IPR_SCI5_].BIT.IPR = 1;						//	���荞�݃��x���ݒ�

	//	SCI4 ���荞�ݐݒ�
	ICU.IER[IER_SCI5_RXI5].BIT.IEN_SCI5_RXI5 = 1;		//	��M���荞��
	ICU.IER[IER_SCI5_TXI5].BIT.IEN_SCI5_TXI5 = 1;		//	���M�������荞��
	ICU.IER[IER_SCI5_TEI5].BIT.IEN_SCI5_TEI5 = 1;		//	���M�G���v�e�B���荞��

	//	���荞�݃t���O�N���A
	ICU.IR[IR_SCI5_RXI5].BIT.IR = 0;
	ICU.IR[IR_SCI5_TXI5].BIT.IR = 0;
	ICU.IR[IR_SCI5_TEI5].BIT.IR = 0;

	//	GROUP12���荞�ݐݒ�
	ICU.IER[IER_ICU_GROUPL0].BIT.IEN_ICU_GROUPL0 = 1;	//	�O���[�v12 ���荞�݋���
	ICU.GEN[GEN_SCI5_ERI5].BIT.EN_SCI5_ERI5 = 1;		//	�O���[�v12 SCI5��M�G���[���荞�݋���
	ICU.IPR[IPR_ICU_GROUPL0].BIT.IPR = 1;				//	�O���\�v12 ���荞�݃��x���ݒ�
	ICU.IR[IR_ICU_GROUPL0].BIT.IR = 0;					//	�O���[�v12 ���荞�݃t���O�N���A

	//	�Ȉ�SPI���[�h�ɋ���
	SCI5.SCMR.BIT.SMIF = 0;		//	
	SCI5.SIMR1.BIT.IICM = 0;	//	
	SCI5.SMR.BIT.CM = 1;		//	�������[�h
	SCI5.SPMR.BIT.SSE = 0;		//	�V���O���}�X�^

	SCI5.SCR.BIT.RIE = 1;
	SCI5.SCR.BIT.RE = 1;

#endif		/*SCI5_ACTIVATE*/
}

//________________________________________________________________________________________
//
//	sci6_init
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI6������
//					Port		SCI			I2C			SPI			�K�p
//			----------------------------------------------------------------------------
//			SCI6	P00			TXD6								<TTL>		COM6
//					P01			RXD6								<TTL>		COM6
//					P02			nRTS6								<TTL>		COM6
//					PJ3			nCTS6								<TTL>		COM6
//	����
//		speed		�ʐM���x	300�`115200
//		datalen		�f�[�^��	7,8
//		stoplen		�X�g�b�v��	1,2
//		parity		�p���e�B�[	0=���� / 1=� / 2=����
//	�߂�
//		����
//________________________________________________________________________________________
//
void sci6_init(long bps, int datalen, int stoplen, int parity)
{
	SCI_MODULE	*com = &sci_com[6];
	memset(com, 0, sizeof(SCI_MODULE));

#ifdef		SCI6_ACTIVATE

	SYSTEM.PRCR.WORD = 0xA502;		//	�v���e�N�g����
	MSTP_SCI6 = 0;					//	SCI���W���[���X�g�b�v����

	//	SCI ���荞�ݗv���֎~
	ICU.IER[IER_ICU_GROUPL0].BIT.IEN_ICU_GROUPL0 = 0;	//	�O���[�v12 ���荞�݋֎~
	ICU.IER[IER_SCI6_RXI6].BIT.IEN_SCI6_RXI6 = 0;		//	��M���荞�݋֎~
	ICU.IER[IER_SCI6_TXI6].BIT.IEN_SCI6_TXI6 = 0;		//	���M�������荞�݋֎~
	ICU.IER[IER_SCI6_TEI6].BIT.IEN_SCI6_TEI6 = 0;		//	���M�G���v�e�B���荞�݋֎~

	//	Enable write protection(�v���e�N�g�|����)
	SYSTEM.PRCR.WORD = 0xA500;		//	�v���e�N�g

	//	SCR - Serial Control Register
	//	b7		TIE	- Transmit Interrupt Enable		- A TXI interrupt request is disabled
	//	b6		RIE	- Receive Interrupt Enable		- RXI and ERI interrupt requests are disabled
	//	b5		TE	- Transmit Enable				- Serial transmission is disabled
	//	b4		RE	- Receive Enable				- Serial reception is disabled
	//	b2		TEIE - Transmit End Interrupt Enable - A TEI interrupt request is disabled
	SCI6.SCR.BYTE = 0x00;

	while (0x00 != (SCI6.SCR.BYTE & 0xF0))
	{
		//	Confirm that bit is actually 0
	}

	//	Set the I/O port functions
	//	�ėp�|�[�gP00:TXD6�AP01:RXD6
	PORT0.PODR.BIT.B0 = 1;		//	TXD
	PORT0.PODR.BIT.B2 = 1;		//	RTS=Disable
	//	Set port direction - TXDn is output port, RXDn is input port(�|�[�g���o�͐ݒ�)
	PORT0.PDR.BIT.B0 = 1;		//	�o��	TXD
	PORT0.PDR.BIT.B1 = 0;		//	����	RXD
	PORT0.PDR.BIT.B2 = 1;		//	�o��	RTS
	PORTJ.PDR.BIT.B3 = 0;		//	����	CTS

	//	Set port mode - Use pin as general I/O port
	PORT0.PMR.BIT.B0 = 0;		//	�ėpIO�|�[�g�ݒ�
	PORT0.PMR.BIT.B1 = 0;		//	�ėpIO�|�[�g�ݒ�
	PORT0.PMR.BIT.B2 = 0;		//	�ėpIO�|�[�g�ݒ�
	PORTJ.PMR.BIT.B3 = 0;		//	�ėpIO�|�[�g�ݒ�

	//	PWPR - Write-Protect Register(�������݃v���e�N�g���W�X�^)
	//	b7		B0WI		- PFSWE Bit Write Disable	- PFSWE�֎~
	//	b6		PFSWE	- PFS Register Write Enable - PFS����
	//	b5:b0	Reserved - These bits are read as 0. The write value should be 0.
	MPC.PWPR.BIT.B0WI = 0;			//	���0�ɂ���
	MPC.PWPR.BIT.PFSWE = 1;			//	���1�ɂ���

	//	PFS - Pin Function Control Register(�s���t�@���N�V�������W�X�^�ݒ�)
	//	b3:b0	PSEL - Pin Function Select - RXDn, TXDn
	MPC.P00PFS.BYTE = 0x0A;		//	assign I/O pin to SCI6 TXD6
	MPC.P01PFS.BYTE = 0x0A;		//	assign I/O pin to SCI6 RXD6
	MPC.P02PFS.BYTE = 0x00;		//	assign I/O pin to port
	MPC.PJ3PFS.BYTE = 0x00;		//	assign I/O pin to port
	//	�������݃v���e�N�g��������
	MPC.PWPR.BIT.PFSWE = 0;
	MPC.PWPR.BIT.B0WI = 1;

	//	Use pin as I/O port for peripheral functions(IO�s���@�\�ݒ�)
	PORT0.PMR.BIT.B0 = 1;			//	���Ӌ@�\�ݒ�
	PORT0.PMR.BIT.B1 = 1;			//	���Ӌ@�\�ݒ�
	//	Initialization of SCI


	//	Select an On-chip baud rate generator to the clock source
	SCI6.SCR.BIT.CKE = 0;

	//	SMR - Serial Mode Register
	//	b7		CM	- Communications Mode	- Asynchronous mode
	//	b6		CHR	- Character Length		- Selects 8 bits as the data length
	//	b5		PE	- Parity Enable			- When transmitting : Parity bit addition is not performed
	//								When receiving	: Parity bit checking is not performed
	//	b3		STOP - Stop Bit Length		- 2 stop bits
	//	b2		MP	- Multi-Processor Mode	- Multi-processor communications function is disabled
	//	b1:b0	CKS	- Clock Select			- PCLK clock (n = 0)
	SCI6.SMR.BYTE = 0x08;

	//	SCMR - Smart Card Mode Register
	//	b6:b4	Reserved - The write value should be 1.
	//	b3		SDIR		- Transmitted/Received Data Transfer Direction - Transfer with LSB-first
	//	b2		SINV		- Transmitted/Received Data Invert	- TDR contents are transmitted as they are. 
	//												Receive data is stored as it is in RDR.
	//	b1		Reserved - The write value should be 1.
	//	b0		SMIF		- Smart Card Interface Mode Select	- Serial communications interface mode
	SCI6.SCMR.BYTE = 0xF2;

	//	SEMR - Serial Extended Mode Register
	//	b7:b6	Reserved - The write value should be 0.
	//	b5		NFEN		- Digital Noise Filter Function Enable	- Noise cancellation function 
	//													for the RXDn input signal is disabled.
	//	b4		ABCS		- Asynchronous Mode Base Clock Select	- Selects 16 base clock cycles for 1-bit period
	//	b3:b1	Reserved - The write value should be 0.
	SCI6.SEMR.BYTE = 0x00;

	//	Set data transfer format in Serial Mode Register (SMR)
	//		-Asynchronous Mode`
	//		-8 bits
	//		-no parity
	//		-1 stop bit
	//		-PCLK clock (n = 0)
	SCI6.SMR.BYTE = 0x00;		//	0=PCLK, 1=PCLK/4, 2=PCLK/16, 3=PCLK/64

	//	BRR - Bit Rate Register
	//	Bit Rate: (48MHz/(64*2^(-1)*57600bps))-1=25.04
	if(stoplen == 1)
	{
		SCI6.SMR.BIT.STOP = 1;
	}
	else
	{
		SCI6.SMR.BIT.STOP = 0;
	}

	if(parity == 0)
	{
		SCI6.SMR.BIT.PE = 0;
	}
	else
	if(parity == 1)
	{		//�@��p���e�B
		SCI6.SMR.BIT.PE = 1;
		SCI6.SMR.BIT.PM = 1;
	}
	else
	if(parity == 2)
	{	//	�����p���e�B
		SCI6.SMR.BIT.PE = 1;
		SCI6.SMR.BIT.PM = 0;
	}

	if(datalen == 7)
	{		//	7bit��
		SCI6.SMR.BIT.CHR = 1;
	}
	else
	if(datalen == 8)
	{	//	8bit��
		SCI6.SMR.BIT.CHR = 0;
	}

	//	Set baud rate to 115200
	//	N = (PCLK Frequency) / (64 * 2^(2*n - 1) * Bit Rate) - 1
	//	N = (48,000,000) / (64 * 2^(2*0 - 1) * 115200) - 1
	//	N = 12
	SCI6.BRR = 48000000 / ((64/2) * bps) - 1;

	//	SCI���荞�ݗD�揇�ʐݒ�
	ICU.IPR[IPR_SCI6_].BIT.IPR = 1;						//	���荞�݃��x���ݒ�

	//	SCI6 ���荞�ݐݒ�
	ICU.IER[IER_SCI6_RXI6].BIT.IEN_SCI6_RXI6 = 1;		//	��M���荞��
	ICU.IER[IER_SCI6_TXI6].BIT.IEN_SCI6_TXI6 = 1;		//	���M�������荞��
	ICU.IER[IER_SCI6_TEI6].BIT.IEN_SCI6_TEI6 = 1;		//	���M�G���v�e�B���荞��

	//	���荞�݃t���O�N���A
	ICU.IR[IR_SCI6_RXI6].BIT.IR = 0;
	ICU.IR[IR_SCI6_TXI6].BIT.IR = 0;
	ICU.IR[IR_SCI6_TEI6].BIT.IR = 0;

	//	GROUP12���荞�ݐݒ�
	ICU.IER[IER_ICU_GROUPL0].BIT.IEN_ICU_GROUPL0 = 1;	//	�O���[�v12 ���荞�݋���
	ICU.GEN[GEN_SCI6_ERI6].BIT.EN_SCI6_ERI6 = 1;		//	�O���[�v12 SCI6��M�G���[���荞�݋���
	ICU.IPR[IPR_ICU_GROUPL0].BIT.IPR = 1;				//	�O���\�v12 ���荞�݃��x���ݒ�
	ICU.IR[IR_ICU_GROUPL0].BIT.IR = 0;					//	�O���[�v12 ���荞�݃t���O�N���A

	SCI6.SCR.BIT.RIE = 1;
	SCI6.SCR.BIT.RE = 1;

	PORT0.PODR.BIT.B2 = 0;		//	RTS=Enable

#endif		/*SCI6_ACTIVATE*/
}

//________________________________________________________________________________________
//
//	sci_putcheck
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI�̑��M�o�b�t�@�̋󂫗e�ʂ����߂�
//	����
//		ch			SCI�`�����l��
//	�߂�
//		int			�󂫃o�C�g��
//________________________________________________________________________________________
//
int sci_putcheck(int ch)
{
	SCI_MODULE	*com = &sci_com[ch];
	int sz = (com->txwp - com->txrp);
	if(sz < 0) sz += BUFSIZE;
	return (BUFSIZE - sz);
}

//________________________________________________________________________________________
//
//	sci_txbytes
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI�̑��M�o�b�t�@�̖����M�o�C�g�������߂�
//	����
//		ch			SCI�`�����l��
//	�߂�
//		int			�����M�o�C�g��
//________________________________________________________________________________________
//
int sci_txbytes(int ch)
{
	SCI_MODULE	*com = &sci_com[ch];
	int sz = (com->txwp - com->txrp);
	if(sz < 0) sz += BUFSIZE;
	return sz;
}

//________________________________________________________________________________________
//
//	sci(n)_putb
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI�փf�[�^��𑗐M����
//	����
//		*buf		���M�o�b�t�@
//		size		���M�o�C�g��
//	�߂�
//		����
//________________________________________________________________________________________
//
//----------------------------------------------------------------------------------------
//	SCI0	SEND
//----------------------------------------------------------------------------------------
void	sci0_putb(unsigned char *buf, int size)
{
	int			i, ch = 0;
	SCI_MODULE	*com = &sci_com[ch];
	while(size > 0)
	{
#ifdef		SCI0_ACTIVATE
#ifdef	SCI0_FLOW
		if(SCI0_CTS_PORT != 0)
		{	//	CTS=Disable
			if(sci_putcheck(ch) < 2) return;
		}
#endif
		while( sci_putcheck(ch) < 2)
		{	//	�o�b�t�@���󂭂܂ő҂�
			if(SCI0.SCR.BIT.TE == 0)
			{	//	���M�J�n����
#ifdef	SCI0_FLOW
				if(SCI0_CTS_PORT == 0)
				{	//	CTS=Enable
#endif
					SCI0.SCR.BIT.TIE = 1;				//	���荞�݃C�l�[�u��
					SCI0.SCR.BIT.TE = 1;				//	���M�C�l�u�[��
#ifdef	SCI0_FLOW
				}
#endif
			}
		}
#endif
		com->txbuf[com->txwp++] = *buf++;
		size--;
		if(com->txwp >= BUFSIZE) com->txwp = 0;
#ifdef		SCI0_ACTIVATE
		if(SCI0.SCR.BIT.TE == 0)
		{	//	���M�J�n����
#ifdef	SCI0_FLOW
			if(SCI0_CTS_PORT == 0)
			{	//	CTS=Enable
#endif
				SCI0.SCR.BIT.TIE = 1;				//	���荞�݃C�l�[�u��
				SCI0.SCR.BIT.TE = 1;				//	���M�C�l�u�[��
#ifdef	SCI0_FLOW
			}
#endif
		}
#endif
	}
#ifdef		SCI0_ACTIVATE
	if(SCI0.SCR.BIT.TE == 0)
	{	//	���M�J�n����
#ifdef	SCI0_FLOW
		if(SCI0_CTS_PORT == 0)
		{	//	CTS=Enable
#endif
			SCI0.SCR.BIT.TIE = 1;				//	���荞�݃C�l�[�u��
			SCI0.SCR.BIT.TE = 1;				//	���M�C�l�u�[��
#ifdef	SCI0_FLOW
		}
#endif
	}
#endif
}
//----------------------------------------------------------------------------------------
//	SCI1	SEND
//----------------------------------------------------------------------------------------
void	sci1_putb(unsigned char *buf, int size)
{
	int			i, ch = 1;
	SCI_MODULE	*com = &sci_com[ch];
	while(size > 0)
	{
#ifdef		SCI1_ACTIVATE
#ifdef	SCI1_FLOW
		if(SCI1_CTS_PORT != 0)
		{	//	CTS=Disable
			if(sci_putcheck(ch) < 2) return;
		}
#endif
		while( sci_putcheck(ch) < 2)
		{	//	�o�b�t�@���󂭂܂ő҂�
			if(SCI1.SCR.BIT.TE == 0)
			{	//	���M�J�n����
#ifdef	SCI1_FLOW
				if(SCI1_CTS_PORT == 0)
				{	//	CTS=Enable
#endif
					SCI1.SCR.BIT.TIE = 1;				//	���荞�݃C�l�[�u��
					SCI1.SCR.BIT.TE = 1;				//	���M�C�l�u�[��
#ifdef	SCI1_FLOW
				}
#endif
			}
		}
#endif
		com->txbuf[com->txwp++] = *buf++;
		size--;
		if(com->txwp >= BUFSIZE) com->txwp = 0;
	#ifdef		SCI1_ACTIVATE
		if(SCI1.SCR.BIT.TE == 0)
		{	//	���M�J�n����
	#ifdef	SCI1_FLOW
			if(SCI1_CTS_PORT == 0)
			{	//	CTS=Enable
	#endif
				SCI1.SCR.BIT.TIE = 1;				//	���荞�݃C�l�[�u��
				SCI1.SCR.BIT.TE = 1;				//	���M�C�l�u�[��
	#ifdef	SCI1_FLOW
			}
	#endif
		}
	#endif
	}
#ifdef		SCI1_ACTIVATE
	if(SCI1.SCR.BIT.TE == 0)
	{	//	���M�J�n����
#ifdef	SCI1_FLOW
		if(SCI1_CTS_PORT == 0)
		{	//	CTS=Enable
#endif
			SCI1.SCR.BIT.TIE = 1;				//	���荞�݃C�l�[�u��
			SCI1.SCR.BIT.TE = 1;				//	���M�C�l�u�[��
#ifdef	SCI1_FLOW
		}
#endif
	}
#endif
}
//----------------------------------------------------------------------------------------
//	SCI2	SEND
//----------------------------------------------------------------------------------------
void	sci2_putb(unsigned char *buf, int size)
{
	int			i, ch = 2;
	SCI_MODULE	*com = &sci_com[ch];
	while(size > 0)
	{
#ifdef		SCI2_ACTIVATE
#ifdef	SCI2_FLOW
		if(SCI2_CTS_PORT != 0)
		{	//	CTS=Disable
			if(sci_putcheck(ch) < 2) return;
		}
#endif
		while( sci_putcheck(ch) < 2)
		{	//	�o�b�t�@���󂭂܂ő҂�
			if(SCI2.SCR.BIT.TE == 0)
			{	//	���M�J�n����
#ifdef	SCI2_FLOW
				if(SCI2_CTS_PORT == 0)
				{	//	CTS=Enable
#endif
					SCI2.SCR.BIT.TIE = 1;				//	���荞�݃C�l�[�u��
					SCI2.SCR.BIT.TE = 1;				//	���M�C�l�u�[��
#ifdef	SCI2_FLOW
				}
#endif
			}
		}
#endif
		com->txbuf[com->txwp++] = *buf++;
		size--;
		if(com->txwp >= BUFSIZE) com->txwp = 0;
	#ifdef		SCI2_ACTIVATE
		if(SCI2.SCR.BIT.TE == 0)
		{	//	���M�J�n����
	#ifdef	SCI2_FLOW
			if(SCI2_CTS_PORT == 0)
			{	//	CTS=Enable
	#endif
				SCI2.SCR.BIT.TIE = 1;				//	���荞�݃C�l�[�u��
				SCI2.SCR.BIT.TE = 1;				//	���M�C�l�u�[��
	#ifdef	SCI2_FLOW
			}
	#endif
		}
	#endif
	}
#ifdef		SCI2_ACTIVATE
	if(SCI2.SCR.BIT.TE == 0)
	{	//	���M�J�n����
#ifdef	SCI2_FLOW
		if(SCI2_CTS_PORT == 0)
		{	//	CTS=Enable
#endif
			SCI2.SCR.BIT.TIE = 1;				//	���荞�݃C�l�[�u��
			SCI2.SCR.BIT.TE = 1;				//	���M�C�l�u�[��
#ifdef	SCI2_FLOW
		}
#endif
	}
#endif
}
//----------------------------------------------------------------------------------------
//	SCI3	SEND
//----------------------------------------------------------------------------------------
void	sci3_putb(unsigned char *buf, int size)
{
	int			i, ch = 3;
	SCI_MODULE	*com = &sci_com[ch];
	while(size > 0)
	{
#ifdef		SCI3_ACTIVATE
#ifdef	SCI3_FLOW
		if(SCI3_CTS_PORT != 0)
		{	//	CTS=Disable
			if(sci_putcheck(ch) < 2) return;
		}
#endif
		while( sci_putcheck(ch) < 2)
		{	//	�o�b�t�@���󂭂܂ő҂�
			if(SCI3.SCR.BIT.TE == 0)
			{	//	���M�J�n����
#ifdef	SCI3_FLOW
				if(SCI3_CTS_PORT == 0)
				{	//	CTS=Enable
#endif
					SCI3.SCR.BIT.TIE = 1;				//	���荞�݃C�l�[�u��
					SCI3.SCR.BIT.TE = 1;				//	���M�C�l�u�[��
#ifdef	SCI3_FLOW
				}
#endif
			}
		}
#endif
		com->txbuf[com->txwp++] = *buf++;
		size--;
		if(com->txwp >= BUFSIZE) com->txwp = 0;
	#ifdef		SCI3_ACTIVATE
		if(SCI3.SCR.BIT.TE == 0)
		{	//	���M�J�n����
	#ifdef	SCI3_FLOW
			if(SCI3_CTS_PORT == 0)
			{	//	CTS=Enable
	#endif
				SCI3.SCR.BIT.TIE = 1;				//	���荞�݃C�l�[�u��
				SCI3.SCR.BIT.TE = 1;				//	���M�C�l�u�[��
	#ifdef	SCI3_FLOW
			}
	#endif
		}
	#endif
	}
#ifdef		SCI3_ACTIVATE
	if(SCI3.SCR.BIT.TE == 0)
	{	//	���M�J�n����
#ifdef	SCI3_FLOW
		if(SCI3_CTS_PORT == 0)
		{	//	CTS=Enable
#endif
			SCI3.SCR.BIT.TIE = 1;				//	���荞�݃C�l�[�u��
			SCI3.SCR.BIT.TE = 1;				//	���M�C�l�u�[��
#ifdef	SCI3_FLOW
		}
#endif
	}
#endif
}
//----------------------------------------------------------------------------------------
//	SCI5	SEND
//----------------------------------------------------------------------------------------
void	sci5_putb(unsigned char *buf, int size)
{
	int			i, ch = 5;
	SCI_MODULE	*com = &sci_com[ch];
	while(size > 0)
	{
#ifdef		SCI5_ACTIVATE
		while( sci_putcheck(ch) < 2)
		{	//	�o�b�t�@���󂭂܂ő҂�
			if(SCI5.SCR.BIT.TE == 0)
			{	//	���M�J�n����
				SCI5.SCR.BIT.TIE = 1;				//	���荞�݃C�l�[�u��
				SCI5.SCR.BIT.TE = 1;				//	���M�C�l�u�[��
			}
		}
#endif
		com->txbuf[com->txwp++] = *buf++;
		size--;
		if(com->txwp >= BUFSIZE) com->txwp = 0;
	#ifdef		SCI5_ACTIVATE
		if(SCI5.SCR.BIT.TE == 0)
		{	//	���M�J�n����
			SCI5.SCR.BIT.TIE = 1;				//	���荞�݃C�l�[�u��
			SCI5.SCR.BIT.TE = 1;				//	���M�C�l�u�[��
		}
	#endif
	}
#ifdef		SCI5_ACTIVATE
	if(SCI5.SCR.BIT.TE == 0)
	{	//	���M�J�n����
		SCI5.SCR.BIT.TIE = 1;				//	���荞�݃C�l�[�u��
		SCI5.SCR.BIT.TE = 1;				//	���M�C�l�u�[��
	}
#endif
}
//----------------------------------------------------------------------------------------
//	SCI6	SEND
//----------------------------------------------------------------------------------------
void	sci6_putb(unsigned char *buf, int size)
{
	int			i, ch = 6;
	SCI_MODULE	*com = &sci_com[ch];
	while(size > 0)
	{
#ifdef		SCI6_ACTIVATE
#ifdef	SCI6_FLOW
		if(SCI6_CTS_PORT != 0)
		{	//	CTS=Disable
			if(sci_putcheck(ch) < 2) return;
		}
#endif
		while( sci_putcheck(ch) < 2)
		{	//	�o�b�t�@���󂭂܂ő҂�
			if(SCI6.SCR.BIT.TE == 0)
			{	//	���M�J�n����
#ifdef	SCI6_FLOW
				if(SCI6_CTS_PORT == 0)
				{	//	CTS=Enable
#endif
					SCI6.SCR.BIT.TIE = 1;				//	���荞�݃C�l�[�u��
					SCI6.SCR.BIT.TE = 1;				//	���M�C�l�u�[��
#ifdef	SCI6_FLOW
				}
#endif
			}
		}
#endif
		com->txbuf[com->txwp++] = *buf++;
		size--;
		if(com->txwp >= BUFSIZE) com->txwp = 0;
	#ifdef		SCI6_ACTIVATE
		if(SCI6.SCR.BIT.TE == 0)
		{	//	���M�J�n����
	#ifdef	SCI6_FLOW
			if(SCI6_CTS_PORT == 0)
			{	//	CTS=Enable
	#endif
				SCI6.SCR.BIT.TIE = 1;				//	���荞�݃C�l�[�u��
				SCI6.SCR.BIT.TE = 1;				//	���M�C�l�u�[��
	#ifdef	SCI6_FLOW
			}
	#endif
		}
	#endif
	}
#ifdef		SCI6_ACTIVATE
	if(SCI6.SCR.BIT.TE == 0)
	{	//	���M�J�n����
#ifdef	SCI6_FLOW
		if(SCI6_CTS_PORT == 0)
		{	//	CTS=Enable
#endif
			SCI6.SCR.BIT.TIE = 1;				//	���荞�݃C�l�[�u��
			SCI6.SCR.BIT.TE = 1;				//	���M�C�l�u�[��
#ifdef	SCI6_FLOW
		}
#endif
	}
#endif
}
//________________________________________________________________________________________
//
//	sci_puts
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI�֕�����𑗐M����
//	����
//		ch			SCI�`�����l��
//		*str		���M������
//	�߂�
//		int			�󂫃o�C�g��
//________________________________________________________________________________________
//
void sci_puts(int ch, char *str)
{
	int len = 0;
	for(len = 0; str[len] != 0 && len < 256; len++);
	switch(ch)
	{
	case 0:
		sci0_putb((unsigned char *)str, len);
		break;
	case 1:
		sci1_putb((unsigned char *)str, len);
		break;
	case 2:
		sci2_putb((unsigned char *)str, len);
		break;
	case 3:
		sci3_putb((unsigned char *)str, len);
		break;
	case 5:
		sci5_putb((unsigned char *)str, len);
		break;
	case 6:
		sci6_putb((unsigned char *)str, len);
		break;
	}
}

//________________________________________________________________________________________
//
//	sci_putb
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI�փo�C�g��𑗐M����
//	����
//		ch			SCI�`�����l��
//		*buf		���M�o�C�g��
//		len			�o�C�g��
//	�߂�
//		int			�󂫃o�C�g��
//________________________________________________________________________________________
//
void sci_putb(int ch, unsigned char *buf, int len)
{
	switch(ch)
	{
	case 0:
		sci0_putb(buf, len);
		break;
	case 1:
		sci1_putb(buf, len);
		break;
	case 2:
		sci2_putb(buf, len);
		break;
	case 3:
		sci3_putb(buf, len);
		break;
	case 5:
		sci5_putb(buf, len);
		break;
	case 6:
		sci6_putb(buf, len);
		break;
	}
}

//________________________________________________________________________________________
//
//	sci_putc
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI��1�������M����
//	����
//		ch			SCI�`�����l��
//		data		���M����
//	�߂�
//		����
//________________________________________________________________________________________
//
void sci_putc(int ch, char data)
{
	char	str[2];
	str[0] = data;
	str[1] = 0;
	switch(ch)
	{
	case 0:
		sci0_putb((unsigned char *)str, 1);
		break;
	case 1:
		sci1_putb((unsigned char *)str, 1);
		break;
	case 2:
		sci2_putb((unsigned char *)str, 1);
		break;
	case 3:
		sci3_putb((unsigned char *)str, 1);
		break;
	case 5:
		sci5_putb((unsigned char *)str, 1);
		break;
	case 6:
		sci6_putb((unsigned char *)str, 1);
		break;
	}
}

//________________________________________________________________________________________
//
//	sci(n)_txi
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI0,1,2,3,5,6 ���M�o�b�t�@����ɂȂ�Ɣ������銄�荞�݊֐�
//	����
//		����
//	�߂�
//		����
//	���l
//		���荞�݃x�N�^(VECT_SCI0_TXI0�`VECT_SCI6_TXI6) 215,218,221,224,230,233
//________________________________________________________________________________________
//
//----------------------------------------------------------------------------------------
#ifdef		SCI0_ACTIVATE
//----------------------------------------------------------------------------------------
//	215
void interrupt __vectno__{VECT_SCI0_TXI0} sci0_txi(void)
{
	int	i;
	SCI_MODULE	*com = &sci_com[0];
	if( com->txrp != com->txwp )
	{	//	�o�b�t�@�Ƀf�[�^���c���Ă���
#ifdef	SCI0_TXOSDN
		SCI0_TXOSDN_PORT = 1;		//	0=��M�̂� / 1=���M�\
#endif
#ifdef	SCI0_FLOW
		if(SCI0_CTS_PORT == 0)
		{	//	CTS=Enable
#endif
			i = com->txrp++;
			if(com->txrp >= BUFSIZE) com->txrp = 0;
			SCI0.TDR = com->txbuf[i];
#ifdef	SCI0_FLOW
		}
		else
		{	//	CTS�ɂ�著�M��~
			SCI0.SCR.BIT.TIE = 0;		//	TXI���荞�݋֎~
			SCI0.SCR.BIT.TEIE = 1;		//	TEI���荞�݋���
		}
#endif
	}
	else
	{	//	���M�o�b�t�@����
		SCI0.SCR.BIT.TIE = 0;		//	TXI���荞�݋֎~
		SCI0.SCR.BIT.TEIE = 1;		//	TEI���荞�݋���
	}
}
#endif
//----------------------------------------------------------------------------------------
#ifdef		SCI1_ACTIVATE
//----------------------------------------------------------------------------------------
//	218
#ifndef __YIDE_REM_DEBUG__
void interrupt __vectno__{VECT_SCI1_TXI1} sci1_txi(void)
#else
void interrupt sci1_txi(void)
#endif
{
	int	i;
	SCI_MODULE	*com = &sci_com[1];
	if( com->txrp != com->txwp )
	{	//	�o�b�t�@�Ƀf�[�^���c���Ă���
#ifdef	SCI1_FLOW
		if(SCI1_CTS_PORT == 0)
		{	//	CTS=Enable
#endif
			i = com->txrp++
			if(com->txrp >= BUFSIZE) com->txrp = 0;
			SCI1.TDR = com->txbuf[i];
#ifdef	SCI1_FLOW
		}
		else
		{	//	CTS�ɂ�著�M��~
			SCI1.SCR.BIT.TIE = 0;		//	TXI���荞�݋֎~
			SCI1.SCR.BIT.TEIE = 1;		//	TEI���荞�݋���
		}
#endif
	}
	else
	{	//	���M�o�b�t�@����
		SCI1.SCR.BIT.TIE = 0;		//	TXI���荞�݋֎~
		SCI1.SCR.BIT.TEIE = 1;		//	TEI���荞�݋���
	}
}
#endif
//----------------------------------------------------------------------------------------
#ifdef		SCI2_ACTIVATE
//----------------------------------------------------------------------------------------
//	221
void interrupt __vectno__{VECT_SCI2_TXI2} sci2_txi(void)
{
	int	i;
	SCI_MODULE	*com = &sci_com[2];
	if( com->txrp != com->txwp )
	{	//	�o�b�t�@�Ƀf�[�^���c���Ă���
#ifdef	SCI2_TXOSDN
		SCI2_TXOSDN_PORT = 1;		//	0=��M�̂� / 1=���M�\
#endif
#ifdef	SCI2_FLOW
		if(SCI2_CTS_PORT == 0)
		{	//	CTS=Enable
#endif
			i = com->txrp++;
			if(com->txrp >= BUFSIZE) com->txrp = 0;
			SCI2.TDR = com->txbuf[i];
#ifdef	SCI2_FLOW
		}
		else
		{	//	CTS�ɂ�著�M��~
			SCI2.SCR.BIT.TIE = 0;		//	TXI���荞�݋֎~
			SCI2.SCR.BIT.TEIE = 1;		//	TEI���荞�݋���
		}
#endif
	}
	else
	{	//	���M�o�b�t�@����
		SCI2.SCR.BIT.TIE = 0;		//	TXI���荞�݋֎~
		SCI2.SCR.BIT.TEIE = 1;		//	TEI���荞�݋���
	}
}
#endif
//----------------------------------------------------------------------------------------
#ifdef		SCI3_ACTIVATE
//----------------------------------------------------------------------------------------
//	224
void interrupt __vectno__{VECT_SCI3_TXI3} sci3_txi(void)
{
	int	i;
	SCI_MODULE	*com = &sci_com[3];
	if( com->txrp != com->txwp )
	{	//	�o�b�t�@�Ƀf�[�^���c���Ă���
#ifdef	SCI3_FLOW
		if(SCI3_CTS_PORT == 0)
		{	//	CTS=Enable
#endif
			i = com->txrp++
			if(com->txrp >= BUFSIZE) com->txrp = 0;
			SCI3.TDR = com->txbuf[i];
#ifdef	SCI3_FLOW
		}
		else
		{	//	CTS�ɂ�著�M��~
			SCI3.SCR.BIT.TIE = 0;		//	TXI���荞�݋֎~
			SCI3.SCR.BIT.TEIE = 1;		//	TEI���荞�݋���
		}
#endif
	}
	else
	{	//	���M�o�b�t�@����
		SCI3.SCR.BIT.TIE = 0;		//	TXI���荞�݋֎~
		SCI3.SCR.BIT.TEIE = 1;		//	TEI���荞�݋���
	}
}
#endif
//----------------------------------------------------------------------------------------
#ifdef		SCI5_ACTIVATE
//----------------------------------------------------------------------------------------
//	230
void interrupt __vectno__{VECT_SCI5_TXI5} sci5_txi(void)
{
	int	i;
	SCI_MODULE	*com = &sci_com[5];
	if( com->txrp != com->txwp )
	{	//	�o�b�t�@�Ƀf�[�^���c���Ă���
#ifdef	SCI5_FLOW
		if(SCI5_CTS_PORT == 0)
		{	//	CTS=Enable
#endif
			i = com->txrp++
			if(com->txrp >= BUFSIZE) com->txrp = 0;
			SCI5.TDR = com->txbuf[i];
#ifdef	SCI5_FLOW
		}
		else
		{	//	CTS�ɂ�著�M��~
			SCI5.SCR.BIT.TIE = 0;		//	TXI���荞�݋֎~
			SCI5.SCR.BIT.TEIE = 1;		//	TEI���荞�݋���
		}
#endif
	}
	else
	{	//	���M�o�b�t�@����
		SCI5.SCR.BIT.TIE = 0;		//	TXI���荞�݋֎~
		SCI5.SCR.BIT.TEIE = 1;		//	TEI���荞�݋���
	}
}
#endif
//----------------------------------------------------------------------------------------
#ifdef		SCI6_ACTIVATE
//----------------------------------------------------------------------------------------
//	233
void interrupt __vectno__{VECT_SCI6_TXI6} sci6_txi(void)
{
	int	i;
	SCI_MODULE	*com = &sci_com[6];
	if( com->txrp != com->txwp )
	{	//	�o�b�t�@�Ƀf�[�^���c���Ă���
#ifdef	SCI6_FLOW
		if(SCI6_CTS_PORT == 0)
		{	//	CTS=Enable
#endif
			i = com->txrp++
			if(com->txrp >= BUFSIZE) com->txrp = 0;
			SCI6.TDR = com->txbuf[i];
#ifdef	SCI61_FLOW
		}
		else
		{	//	CTS�ɂ�著�M��~
			SCI6.SCR.BIT.TIE = 0;		//	TXI���荞�݋֎~
			SCI6.SCR.BIT.TEIE = 1;		//	TEI���荞�݋���
		}
#endif
	}
	else
	{	//	���M�o�b�t�@����
		SCI6.SCR.BIT.TIE = 0;		//	TXI���荞�݋֎~
		SCI6.SCR.BIT.TEIE = 1;		//	TEI���荞�݋���
	}
}
#endif

//________________________________________________________________________________________
//
//	sci(n)_tei
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI0,1,2,3,5,6 �V�t�g���W�X�^�����M��������Ɣ������銄�荞�݊֐�
//	����
//		����
//	�߂�
//		����
//	���l
//		���荞�݃x�N�^(VECT_SCI0_TEI0�`VECT_SCI6_TEI6) 216,219,222,225,231,234
//________________________________________________________________________________________
//
//----------------------------------------------------------------------------------------
#ifdef		SCI0_ACTIVATE
//----------------------------------------------------------------------------------------
//	216
void interrupt __vectno__{VECT_SCI0_TEI0} sci0_tei(void)
{
	int	i;
	SCI_MODULE	*com = &sci_com[0];
	SCI0.SCR.BIT.TEIE = 0;		//	TEI���荞�݃f�B�X�G�C�u��
#ifdef	SCI0_TXOSDN
	SCI0_TXOSDN_PORT = 0;		//	0=nRE(��M) / 1=DE(���M)
#endif
	if( com->txrp != com->txwp )
	{	//	�o�b�t�@�Ƀf�[�^���c���Ă���
		SCI0.SCR.BIT.TIE = 1;		//	���M����C�l�[�u��
		i = com->txrp++;
		if(com->txrp >= BUFSIZE) com->txrp = 0;
		SCI0.TDR = com->txbuf[i];
	}
	else
	{
		SCI0.SCR.BIT.TE = 0;		//	���M����f�B�X�G�C�u��
	}
//	logging("sci0_tei\r");
}
#endif
//----------------------------------------------------------------------------------------
#ifdef		SCI1_ACTIVATE
//----------------------------------------------------------------------------------------
//	219
#ifndef __YIDE_REM_DEBUG__
void interrupt __vectno__{VECT_SCI1_TEI1} sci1_tei(void)
#else
void interrupt sci1_tei(void)
#endif
{
	int	i;
	SCI_MODULE	*com = &sci_com[1];
	SCI1.SCR.BIT.TEIE = 0;		//	TEI���荞�݃f�B�X�G�C�u��
	if( com->txrp != com->txwp )
	{	//	�o�b�t�@�Ƀf�[�^���c���Ă���
		SCI1.SCR.BIT.TIE = 1;		//	���M����C�l�[�u��
		i = com->txrp++
		if(com->txrp >= BUFSIZE) com->txrp = 0;
		SCI1.TDR = com->txbuf[i];
	}
	else
	{
		SCI1.SCR.BIT.TE = 0;		//	���M����f�B�X�G�C�u��
	}
}
#endif
//----------------------------------------------------------------------------------------
#ifdef		SCI2_ACTIVATE
//----------------------------------------------------------------------------------------
//	222
void interrupt __vectno__{VECT_SCI2_TEI2} sci2_tei(void)
{
	int	i;
	SCI_MODULE	*com = &sci_com[2];
	SCI2.SCR.BIT.TEIE = 0;		//	TEI���荞�݃f�B�X�G�C�u��
#ifdef	SCI2_TXOSDN
	SCI2_TXOSDN_PORT = 0;		//	0=nRE(��M) / 1=DE(���M)
#endif
	if( com->txrp != com->txwp )
	{	//	�o�b�t�@�Ƀf�[�^���c���Ă���
		SCI2.SCR.BIT.TIE = 1;		//	���M����C�l�[�u��
		i = com->txrp++;
		if(com->txrp >= BUFSIZE) com->txrp = 0;
		SCI2.TDR = com->txbuf[i];
	}
	else
	{
		SCI2.SCR.BIT.TE = 0;		//	���M����f�B�X�G�C�u��
	}
}
#endif
//----------------------------------------------------------------------------------------
#ifdef		SCI3_ACTIVATE
//----------------------------------------------------------------------------------------
//	225
void interrupt __vectno__{VECT_SCI3_TEI3} sci3_tei(void)
{
	int	i;
	SCI_MODULE	*com = &sci_com[3];
	SCI3.SCR.BIT.TEIE = 0;		//	TEI���荞�݃f�B�X�G�C�u��
	if( com->txrp != com->txwp )
	{	//	�o�b�t�@�Ƀf�[�^���c���Ă���
		SCI3.SCR.BIT.TIE = 1;		//	���M����C�l�[�u��
		i = com->txrp++
		if(com->txrp >= BUFSIZE) com->txrp = 0;
		SCI3.TDR = com->txbuf[i];
	}
	else
	{
		SCI3.SCR.BIT.TE = 0;		//	���M����f�B�X�G�C�u��
	}
}
#endif
//----------------------------------------------------------------------------------------
#ifdef		SCI5_ACTIVATE
//----------------------------------------------------------------------------------------
//	231
void interrupt __vectno__{VECT_SCI5_TEI5} sci5_tei(void)
{
	int	i;
	SCI_MODULE	*com = &sci_com[5];
	SCI5.SCR.BIT.TEIE = 0;		//	TEI���荞�݃f�B�X�G�C�u��
	if( com->txrp != com->txwp )
	{	//	�o�b�t�@�Ƀf�[�^���c���Ă���
		SCI5.SCR.BIT.TIE = 1;		//	���M����C�l�[�u��
		i = com->txrp++
		if(com->txrp >= BUFSIZE) com->txrp = 0;
		SCI5.TDR = com->txbuf[i];
	}
	else
	{
		SCI5.SCR.BIT.TE = 0;		//	���M����f�B�X�G�C�u��
	}
}
#endif
//----------------------------------------------------------------------------------------
#ifdef		SCI6_ACTIVATE
//----------------------------------------------------------------------------------------
//	234
void interrupt __vectno__{VECT_SCI6_TEI6} sci6_tei(void)
{
	int	i;
	SCI_MODULE	*com = &sci_com[6];
	SCI6.SCR.BIT.TEIE = 0;		//	TEI���荞�݃f�B�X�G�C�u��
	if( com->txrp != com->txwp )
	{	//	�o�b�t�@�Ƀf�[�^���c���Ă���
		SCI6.SCR.BIT.TIE = 1;		//	���M����C�l�[�u��
		i = com->txrp++
		if(com->txrp >= BUFSIZE) com->txrp = 0;
		SCI6.TDR = com->txbuf[i];
	}
	else
	{
		SCI6.SCR.BIT.TE = 0;		//	���M����f�B�X�G�C�u��
	}
}
#endif

//________________________________________________________________________________________
//
//	sci_load
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI0,1,2,3,5,6 ��M�f�[�^���o�b�t�@�ɐςݏグ��
//	����
//		ch			SCI�`�����l��
//		err			�G���[�R�[�h
//		data		�f�[�^�{�G���[�t���O
//	�߂�
//		����
//________________________________________________________________________________________
//
void	sci_load(int ch, unsigned char err, unsigned char data)
{
	int	sz;
	SCI_MODULE	*com = &sci_com[ch];
	if(err != 0)
	{
		com->err++;
		if((err & 0x08) != 0)
		{	//	�p���e�B�G���[
			com->perr++;
		}
		if((err & 0x10) != 0)
		{	//	�t���[�~���O�G���[
			com->ferr++;
		}
		if((err & 0x20) != 0)
		{	//	�I�[�o�[�����G���[
			com->oerr++;
		}
	}
	//	�f�[�^�ۑ�
	com->rxbuf[com->rxwp++] = data;
	if(com->rxwp >= BUFSIZE) com->rxwp = 0; //	�|�C���^��0�ɖ߂�
	//	RTS�t���[����
	switch(ch)
	{
#ifdef		SCI0_ACTIVATE
#ifdef	SCI0_FLOW
	case 0:	//	COM0��p�t���[����
		sz = com->rxrp - com->rxwp;
		if(sz < 0) sz += BUFSIZE;
		if(sz < (BUFSIZE * 3 / 4))
		{	//	RTS=Disable
			SCI0_RTS_PORT = 1;
		}
		break;
#endif
#endif
#ifdef		SCI1_ACTIVATE
#ifdef	SCI1_FLOW
	case 1:	//	COM1��p�t���[����
		sz = com->rxrp - com->rxwp;
		if(sz < 0) sz += BUFSIZE;
		if(sz < (BUFSIZE * 3 / 4))
		{	//	RTS=Disable
			SCI1_RTS_PORT = 1;
		}
		break;
#endif
#endif
#ifdef		SCI2_ACTIVATE
#ifdef	SCI2_FLOW
	case 2:	//	COM2��p�t���[����
		sz = com->rxrp - com->rxwp;
		if(sz < 0) sz += BUFSIZE;
		if(sz < (BUFSIZE * 3 / 4))
		{	//	RTS=Disable
			SCI2_RTS_PORT = 1;
		}
		break;
#endif
#endif
#ifdef		SCI3_ACTIVATE
#ifdef	SCI3_FLOW
	case 3:	//	COM3��p�t���[����
		sz = com->rxrp - com->rxwp;
		if(sz < 0) sz += BUFSIZE;
		if(sz < (BUFSIZE * 3 / 4))
		{	//	RTS=Disable
			SCI3_RTS_PORT = 1;
		}
		break;
#endif
#endif
#ifdef		SCI6_ACTIVATE
#ifdef	SCI6_FLOW
	case 6:	//	COM6��p�t���[����
		sz = com->rxrp - com->rxwp;
		if(sz < 0) sz += BUFSIZE;
		if(sz < (BUFSIZE * 3 / 4))
		{	//	RTS=Disable
			SCI6_RTS_PORT = 1;
		}
		break;
#endif
#endif
	default:
		break;
	}
}

//________________________________________________________________________________________
//
//	sci_err
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI �G���[�������荞��(GROUP12) �G���[���͎�M�f�[�^���擾����
//	����
//		����
//	�߂�
//		����
//	���l
//		���荞�݃x�N�^(VECT_ICU_GROUPL0) 114
//________________________________________________________________________________________
//
void interrupt __vectno__{VECT_ICU_GROUPL0} sci_err(void)
{
	unsigned char	d, e;
	PROC_CALL		p;
	
#ifdef		SCI0_ACTIVATE
	//	SCI0
	if(ICU.GRP[GRP_SCI0_ERI0].BIT.IS_SCI0_ERI0)
	{	//	SCI0 Error�L��
		e = SCI0.SSR.BYTE;
		d = SCI0.RDR & 0x00FF;
		sci_load(0, e, d);
		SCI0.SSR.BYTE = 0;
	}
#endif
#ifdef		SCI1_ACTIVATE
	//	SCI1
	if(ICU.GRP[GRP_SCI1_ERI1].BIT.IS_SCI1_ERI1)
	{	//	SCI1 Error�L��
		e = SCI1.SSR.BYTE;
		d = SCI1.RDR & 0x00FF;
		sci_load(1, e, d);
		SCI1.SSR.BYTE = 0;
	}
#endif
#ifdef		SCI2_ACTIVATE
	//	SCI2
	if(ICU.GRP[GRP_SCI2_ERI2].BIT.IS_SCI2_ERI2)
	{	//	SCI2 Error�L��
		e = SCI2.SSR.BYTE;
		d = SCI2.RDR & 0x00FF;
		sci_load(2, e, d);
		SCI2.SSR.BYTE = 0;
	}
#endif
#ifdef		SCI3_ACTIVATE
	//	SCI3
	if(ICU.GRP[GRP_SCI3_ERI3].BIT.IS_SCI3_ERI3)
	{	//	SCI3 Error�L��
		e = SCI3.SSR.BYTE;
		d = SCI3.RDR & 0x00FF;
		sci_load(3, e, d);
		SCI3.SSR.BYTE = 0;
	}
#endif
#ifdef		SCI5_ACTIVATE
	//	SCI5
	if(ICU.GRP[GRP_SCI5_ERI5].BIT.IS_SCI5_ERI5)
	{	//	SCI5 Error�L��
		e = SCI5.SSR.BYTE;
		d = SCI5.RDR & 0x00FF;
		sci_load(5, e, d);
		SCI5.SSR.BYTE = 0;
	}
#endif
#ifdef		SCI6_ACTIVATE
	//	SCI6
	if(ICU.GRP[GRP_SCI6_ERI6].BIT.IS_SCI6_ERI6)
	{	//	SCI6 Error�L��
		e = SCI6.SSR.BYTE;
		d = SCI6.RDR & 0x00FF;
		sci_load(6, e, d);
		SCI6.SSR.BYTE = 0;
	}
#endif
#ifdef		RSPI1_ACTIVATE
	//	RSPI1
	if(ICU.GRP[GRP_RSPI1_SPEI1].BIT.IS_RSPI1_SPEI1)
	{	//	RSPI1 Error�L��
		e = (unsigned short)RSPI1.SPSR.BYTE;
		RSPI1.SPSR.BYTE = 0;
		logging("SPI1 Error %02X\r", (int)e);
	}
#endif
#if	1	/*#ifdef	RSPI2_ACTIVATE*/
	//	RSPI2
	if(ICU.GRP[GRP_RSPI2_SPEI2].BIT.IS_RSPI2_SPEI2)
	{	//	RSPI2 Error�L��
		e = (unsigned short)RSPI2.SPSR.BYTE;
		RSPI2.SPSR.BYTE = 0;
		logging("SPI2 Error %02X\r", (int)e);
	}
#endif
}

//________________________________________________________________________________________
//
//	sci(n)_rxi
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI ��M���荞�ݏ���
//	����
//		����
//	�߂�
//		����
//	���l
//		���荞�݃x�N�^(VECT_SCI0_RXI0�`VECT_SCI6_RXI6) 214,217,220,223,229,232
//________________________________________________________________________________________
//
#ifdef		SCI0_ACTIVATE
//---------------------------------------------------------------------------------------
//	214
//---------------------------------------------------------------------------------------
void interrupt __vectno__{VECT_SCI0_RXI0} sci0_rxi(void)
{
	sci_load(0, 0, SCI0.RDR);
}
#endif
#ifdef		SCI1_ACTIVATE
//---------------------------------------------------------------------------------------
//	217
//---------------------------------------------------------------------------------------
#ifndef __YIDE_REM_DEBUG__
void interrupt __vectno__{VECT_SCI1_RXI1} sci1_rxi(void)
#else
void interrupt sci1_rxi(void)
#endif
{
	sci_load(1, 0, SCI1.RDR);
}
#endif
#ifdef		SCI2_ACTIVATE
//---------------------------------------------------------------------------------------
//	220
//---------------------------------------------------------------------------------------
void interrupt __vectno__{VECT_SCI2_RXI2} sci2_rxi(void)
{
	sci_load(2, 0, SCI2.RDR);
}
#endif
#ifdef		SCI3_ACTIVATE
//---------------------------------------------------------------------------------------
//	223
//---------------------------------------------------------------------------------------
void interrupt __vectno__{VECT_SCI3_RXI3} sci3_rxi(void)
{
	sci_load(3, 0, SCI3.RDR);
}
#endif
#ifdef		SCI5_ACTIVATE
//---------------------------------------------------------------------------------------
//	229
//---------------------------------------------------------------------------------------
void interrupt __vectno__{VECT_SCI5_RXI5} sci5_rxi(void)
{
	sci_load(5, 0, SCI5.RDR);
}
#endif
#ifdef		SCI6_ACTIVATE
//---------------------------------------------------------------------------------------
//	232
//---------------------------------------------------------------------------------------
void interrupt __vectno__{VECT_SCI6_RXI6} sci6_rxi(void)
{
	sci_load(6, 0, SCI6.RDR);
}
#endif
//________________________________________________________________________________________
//
//	sci_get_check
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI ��M�o�C�g���擾
//	����
//		ch			SCI�`�����l���ԍ�
//	�߂�
//		int			��M�o�b�t�@�������o�C�g��
//________________________________________________________________________________________
//
int sci_get_check(int ch)
{
	SCI_MODULE	*com = &sci_com[ch];
	int sz = (com->rxwp - com->rxrp);
	if(sz < 0) sz += BUFSIZE;
	return sz;
}

//________________________________________________________________________________________
//
//	sci_get_check
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI ��M�o�C�g���擾
//	����
//		ch			SCI�`�����l���ԍ�
//	�߂�
//		int			��M�o�b�t�@�������o�C�g��
//________________________________________________________________________________________
//
void sci_rts_control(int ch)
{
	//	RTS�t���[����
	if(sci_get_check(ch) > (BUFSIZE / 4)) return;
	switch(ch)
	{
#ifdef		SCI0_ACTIVATE
#ifdef	SCI0_FLOW
	case 0:	//	COM0��p�t���[����
		SCI0_RTS_PORT = 0;
		break;
#endif
#endif
#ifdef		SCI1_ACTIVATE
#ifdef	SCI1_FLOW
	case 1:	//	COM1��p�t���[����
		SCI1_RTS_PORT = 0;
		break;
#endif
#endif
#ifdef		SCI2_ACTIVATE
#ifdef	SCI2_FLOW
	case 2:	//	COM2��p�t���[����
		SCI2_RTS_PORT = 0;
		break;
#endif
#endif
#ifdef		SCI3_ACTIVATE
#ifdef	SCI3_FLOW
	case 3:	//	COM3��p�t���[����
		SCI3_RTS_PORT = 0;
		break;
#endif
#endif
#ifdef		SCI6_ACTIVATE
#ifdef	SCI6_FLOW
	case 6:	//	COM6��p�t���[����
		SCI6_RTS_PORT = 0;
		break;
#endif
#endif
	default:
		break;
	}
}

//________________________________________________________________________________________
//
//	sci_get_char
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI ��M�o�C�g���擾
//	����
//		ch			SCI�`�����l���ԍ�
//	�߂�
//		int			�f�[�^	0x00�`0xFF:�L�� -1:����
//________________________________________________________________________________________
//
int sci_get_char(int ch)
{
	int data;
	SCI_MODULE	*com = &sci_com[ch];
	if(com->rxrp != com->rxwp)
	{
		data = (int)com->rxbuf[com->rxrp++] & 0x00FF;
		if(com->rxrp >= BUFSIZE) com->rxrp = 0;
		sci_rts_control(ch);
	}
	else
	{
		data = -1;
	}
	return data;
}

//________________________________________________________________________________________
//
//	sci_get_buf
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI ��M�f�[�^�擾
//	����
//		ch			SCI�`�����l���ԍ�
//		*buf		�R�s�[��o�b�t�@
//		size		�]���o�C�g��
//	�߂�
//		int			���]���o�C�g��
//________________________________________________________________________________________
//
int sci_get_buf(int ch, unsigned char *buf, int size)
{
	int i;
	SCI_MODULE	*com = &sci_com[ch];
	for(i = 0; i < size; i++)
	{
		if(com->rxrp != com->rxwp)
		{
			*buf++ = com->rxbuf[com->rxrp++];
			if(com->rxrp >= BUFSIZE) com->rxrp = 0;
		}
		else break;
	}
	sci_rts_control(ch);
	return i;
}

//________________________________________________________________________________________
//
//	sci_get_string
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI ��M������擾
//	����
//		ch			SCI�`�����l���ԍ�
//		*str		�R�s�[��o�b�t�@
//		size		�]�����������
//	�߂�
//		int			���]��������
//________________________________________________________________________________________
//
int sci_get_string(int ch, char *str, int size)
{
	int		i;
	char	c;
	SCI_MODULE	*com = &sci_com[ch];
	for(i = 0; i < size; i++)
	{
		if(com->rxrp != com->rxwp)
		{
			c = (char)com->rxbuf[com->rxrp++];
			if(com->rxrp >= BUFSIZE) com->rxrp = 0;
			*str++ = c;
			if(c == 0) return i;	//	������̏I�[
		}
		else break;
	}
	*str = 0;	//	NUL�t��
	sci_rts_control(ch);
	return i;
}

//________________________________________________________________________________________
//
//	sci_get_line
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI ��M������P�s�擾
//	����
//		ch			SCI�`�����l���ԍ�
//		*str		�R�s�[��o�b�t�@
//		size		�]�����������
//		echar		�s���L�����N�^
//	�߂�
//		int			���]�������� / ���̐��̓o�b�t�@�s���ʖ��͍s������
//________________________________________________________________________________________
//
int sci_get_line(int ch, char *str, int size, char echar)
{
	int		i, e;
	char	c;
	SCI_MODULE	*com = &sci_com[ch];
	
	for(e = 0, i = 0; i < size; )
	{
		if(com->rxrp != com->rxwp)
		{
			c = (char)com->rxbuf[com->rxrp++];
			if(com->rxrp >= BUFSIZE) com->rxrp = 0;
			if(c == echar)
			{	//	�s���R�[�h�ŏI��
				e++;
				break;
			}
			else
			{
				str[i++] = c;
			}
		}
		else break;
	}
	str[i] = 0; //	NUL�t��
	sci_rts_control(ch);
	if(e == 0) return -i;
	return i;
}

//________________________________________________________________________________________
//
//	sci_clear
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI ����ϐ����N���A����
//	����
//		ch			SCI�`�����l���ԍ�
//	�߂�
//		����
//________________________________________________________________________________________
//
void	sci_clear(int ch)
{
	SCI_MODULE	*com = &sci_com[ch];
	memset(com, 0, sizeof(SCI_MODULE));
	switch(ch)
	{
#ifdef		SCI0_ACTIVATE
#ifdef	SCI0_FLOW
	case 0:	//	COM0��p�t���[����
		SCI0_RTS_PORT = 0;
		break;
#endif
#endif
#ifdef		SCI1_ACTIVATE
#ifdef	SCI1_FLOW
	case 1:	//	COM1��p�t���[����
		SCI1_RTS_PORT = 0;
		break;
#endif
#endif
#ifdef		SCI2_ACTIVATE
#ifdef	SCI2_FLOW
	case 2:	//	COM2��p�t���[����
		SCI2_RTS_PORT = 0;
		break;
#endif
#endif
#ifdef		SCI3_ACTIVATE
#ifdef	SCI3_FLOW
	case 3:	//	COM3��p�t���[����
		SCI3_RTS_PORT = 0;
		break;
#endif
#endif
#ifdef		SCI6_ACTIVATE
#ifdef	SCI6_FLOW
	case 6:	//	COM6��p�t���[����
		SCI6_RTS_PORT = 0;
		break;
#endif
#endif
	default:
		break;
	}
}

