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

//#include	"sci.h"

#ifndef __CAN2ECU_SCI_IF__
#define __CAN2ECU_SCI_IF__

#include	"ecu.h"			/*	ECU ���ʒ�`			*/

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

//	�g�p����SCI�|�[�g�̑I��

#define		SCI0_ACTIVATE
//#define		SCI1_ACTIVATE
#define		SCI2_ACTIVATE
//#define		SCI3_ACTIVATE
//#define		SCI4_ACTIVATE
//#define		SCI5_ACTIVATE
//#define		SCI6_ACTIVATE


//	SCI�o�b�t�@�T�C�Y
#define		BUFSIZE		1024
//	SCI0��RS-485����d�Ƃ��Ďg�p
//#define		SCI0_RS485
//	SCI1��nRTS,nCTS���g�p
//#define		SCI1_FLOW
//  SCI3��nCTS��P24�|�[�g�Ɋ��蓖��
//#define     SCI3_nCTS
//	SCI1��nRTS/nCTS��SCI6�|�[�g�Ƃ��Ďg�p
//#define		SCI6_ACTIVE
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

//�ԐڌĂяo���̃v���g�^�C�v(�����P��)
typedef	void 			(*PROC_CALL)(void *);

//	SCI0,SCI1,SCI2,SCI3,SCI5,SCI6	SCI1=�C�G���[�X�R�[�v�Ŏg�p
//extern	CONSOLE_CTRL		sci_com;
//extern	CONSOLE_CTRL		usb_com;

//________________________________________________________________________________________
//
//	sci0_init
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI0������
//							Port		SCI			I2C			SPI			
//				--------------------------------------------------------
//				SCI0	P20			TXD0(*)		SDA0		SMOSI0
//							P21			RXD0(*)		SCL0		SMISO0
//							P22			DE/nRE(*)<RS-485>
//	����
//		speed		�ʐM���x	300�`115200
//		datalen		�f�[�^��	7,8
//		stoplen		�X�g�b�v��	1,2
//		parity		�p���e�B�[	0=���� / 1=� / 2=����
//	�߂�
//		����
//________________________________________________________________________________________
//
extern	void sci0_init(long bps, int datalen, int stoplen, int parity);
#define		SCI0_RTS_PORT			PORT8.PODR.BIT.B6	/*	out 0=Enable / 1=Disable	*/
#define		SCI0_CTS_PORT			PORT8.PIDR.BIT.B7	/*	in	0=Enable / 1=Disable	*/
#define		SCI0_TXOSDN_PORT		PORT7.PODR.BIT.B0	/*	0=RX / 1=TX					*/

//________________________________________________________________________________________
//
//	sci1_init
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI1������
//							Port		SCI			I2C			SPI			
//				--------------------------------------------------------
//				SCI1	P26			TXD1(*) <REM-MON>	
//							P30			RXD1(*) <REM-MON>	
//							P00			nRTS1(*)<REM-MON>	
//							P01			nCTS1(*)<REM-MON>	
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
extern	void sci1_init(long bps, int datalen, int stoplen, int parity);
#define		SCI1_RTS_PORT			PORTE.PODR.BIT.B1	/*	out 0=Enable / 1=Disable	*/
#define		SCI1_CTS_PORT			PORTE.PIDR.BIT.B2	/*	in	0=Enable / 1=Disable	*/

//________________________________________________________________________________________
//
//	sci2_init
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI2������
//							Port		SCI			RIIC		SPI			
//				--------------------------------------------------------
//				SCI2	P13			TXD2		SDA0
//							P12			RXD2		SCL0
//	����
//		speed		�ʐM���x	300�`115200
//		datalen		�f�[�^��	7,8
//		stoplen		�X�g�b�v��	1,2
//		parity		�p���e�B�[	0=���� / 1=� / 2=����
//	�߂�
//		����
//________________________________________________________________________________________
//
extern	void sci2_init(long bps, int datalen, int stoplen, int parity);
#define		SCI2_RTS_PORT			PORT1.PODR.BIT.B5	/*	out 0=Enable / 1=Disable	*/
#define		SCI2_CTS_PORT			PORT1.PIDR.BIT.B7	/*	in	0=Enable / 1=Disable	*/
#define		SCI2_TXOSDN_PORT		PORT7.PODR.BIT.B3	/*	0=RX / 1=TX					*/

//________________________________________________________________________________________
//
//	sci3_init
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI3������
//							Port		SCI			I2C			SPI			
//				--------------------------------------------------------
//				SCI3	P23			TXD3(*)		
//							P24			RXD3(*)		
//	����
//		speed		�ʐM���x	300�`115200
//		datalen		�f�[�^��	7,8
//		stoplen		�X�g�b�v��	1,2
//		parity		�p���e�B�[	0=���� / 1=� / 2=����
//	�߂�
//		����
//________________________________________________________________________________________
//
extern	void sci3_init(long bps, int datalen, int stoplen, int parity);
#define		SCI3_RTS_PORT			PORT2.PODR.BIT.B2	/*	out 0=Enable / 1=Disable	*/
#define		SCI3_CTS_PORT			PORT2.PIDR.BIT.B4	/*	in	0=Enable / 1=Disable	*/
#define		SCI3_EXRES_PORT			PORT5.PODR.BIT.B6	/*	�O�����Z�b�g�o�� 0=RESET	*/

//________________________________________________________________________________________
//
//	sci5_init
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI5������
//							Port		SCI			I2C			SPI			
//				--------------------------------------------------------
//				SCI5	PC3			TXD5(*)		
//							PC2			RXD5(*)		
//	����
//		speed		�ʐM���x	300�`115200
//		datalen		�f�[�^��	7,8
//		stoplen		�X�g�b�v��	1,2
//		parity		�p���e�B�[	0=���� / 1=� / 2=����
//	�߂�
//		����
//________________________________________________________________________________________
//
extern	void sci5_init(long bps, int datalen, int stoplen, int parity);
#define		SCI5_SS0_PORT			PORTC.PODR.BIT.B5	/*	�O���I����o�� 0=Select		*/
#define		SCI5_SS1_PORT			PORTC.PODR.BIT.B6	/*	�O���I����o�� 0=Select		*/

//________________________________________________________________________________________
//
//	sci6_init
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI6������
//						Port		SCI			I2C			SPI			
//				--------------------------------------------------------
//				SCI6	P00			TXD6        						<TTL>		COM6
//						P01			RXD6        						<TTL>		COM6
//						P02			nRTS6								<TTL>		COM6
//						PJ3			nCTS6								<TTL>		COM6
//	����
//		speed		�ʐM���x	300�`115200
//		datalen		�f�[�^��	7,8
//		stoplen		�X�g�b�v��	1,2
//		parity		�p���e�B�[	0=���� / 1=� / 2=����
//	�߂�
//		����
//________________________________________________________________________________________
//
extern	void sci6_init(long bps, int datalen, int stoplen, int parity);
#define		SCI6_RTS_PORT			PORT0.PODR.BIT.B2	/*	out 0=Enable / 1=Disable	*/
#define		SCI6_CTS_PORT			PORTJ.PIDR.BIT.B3	/*	in	0=Enable / 1=Disable	*/

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
extern	int sci_putcheck(int ch);

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
extern	int sci_txbytes(int ch);

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
extern	void	sci0_putb(unsigned char *buf, int size);
extern	void	sci1_putb(unsigned char *buf, int size);
extern	void	sci2_putb(unsigned char *buf, int size);
extern	void	sci3_putb(unsigned char *buf, int size);
extern	void	sci5_putb(unsigned char *buf, int size);
extern	void	sci6_putb(unsigned char *buf, int size);

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
extern	void sci_puts(int ch, char *str);

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
extern	void sci_putb(int ch, unsigned char *buf, int len);

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
extern	void sci_putc(int ch, char data);

//________________________________________________________________________________________
//
//	sci_txint
//----------------------------------------------------------------------------------------
//	�@�\����
//		SCI���M���荞�ݏ���
//	����
//		ch			SCI�`�����l��
//	�߂�
//		����
//________________________________________________________________________________________
//
extern	void sci_txint(int ch);

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
/*
interrupt void sci0_txi(void);
interrupt void sci1_txi(void);
interrupt void sci2_txi(void);
interrupt void sci3_txi(void);
interrupt void sci5_txi(void);
interrupt void sci6_txi(void);
*/
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
/*
interrupt void sci0_tei(void);
interrupt void sci1_tei(void);
interrupt void sci2_tei(void);
interrupt void sci3_tei(void);
interrupt void sci5_tei(void);
interrupt void sci6_tei(void);
*/
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
extern	void	sci_load(int ch, unsigned char err, unsigned char data);

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
//		���荞�݃x�N�^(VECT_ICU_GROUP12) 114
//________________________________________________________________________________________
//
//interrupt void sci_err(void);

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
/*
interrupt void sci0_rxi(void);
interrupt void sci1_rxi(void);
interrupt void sci2_rxi(void);
interrupt void sci3_rxi(void);
interrupt void sci5_rxi(void);
interrupt void sci6_rxi(void);
*/
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
extern	int sci_get_check(int ch);

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
extern	int sci_get_char(int ch);

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
extern	int sci_get_buf(int ch, unsigned char *buf, int size);

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
extern	int sci_get_string(int ch, char *str, int size);

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
extern	int sci_get_line(int ch, char *str, int size, char echar);

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
extern	void	sci_clear(int ch);

#endif	/*__CAN2ECU_SCI_IF__*/

