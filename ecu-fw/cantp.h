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
//	CAN-TP �g�����X�|�[�g�w�v���g�R������
//
//----------------------------------------------------------------------------------------
//	�J������
//
//	2017/08/01	�R�[�f�B���O�J�n�i�k�j
//
//----------------------------------------------------------------------------------------
//	T.Tachibana
//	��L&F
//________________________________________________________________________________________
//

//#include "cantp.h"			/*	CAN-TP ��`				*/

#ifndef		__CAN_TRANSE_PORT_PROTOCOL__
#define		__CAN_TRANSE_PORT_PROTOCOL__

/*
	CAN-TP �̊T�v

	TP�̓V���O�����͕����p�P�b�g��A������ʑw�ɓn������ʑw����󂯎�聨�V���O�����͕����p�P�b�g�ɕ������ē]������B

	CAN-TP�t���[����`
	
	�K�p
	�u���[�h�L���X�g	CAN-ID	0x7DF
	�w��ECU����			CAN-ID	0x7E0�`0x7E7
	�w��ECU����			CAN-ID	0x7E8�`0x7EF
	
	�v���t���[��
		
						+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		Byte			|			0			|			1			|			2			|			3			|			4			|			5			|			6			|			7			|
						+-----------+-----------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		Bit(LE)			|	7..4	|	3..0	|		  15..8			|		  23..16		|		  31..24		|						|						|						|						|
						+-----------+-----------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		Single			|	Type 0	| Size 0..7	|		 Data A			|		  Data B		|		  Data C		|		  Data D		|		  Data E		|		  Data F		|		  Data G		|
						+-----------+-----------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		First			|	Type 1	| Size 8..4095						|		  Data A		|		  Data B		|		  Data C		|		  Data D		|		  Data E		|		  Data F		|
						+-----------+-----------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		Consecutive		|	Type 2	|Index 0..15|		 Data A			|		  Data B		|		  Data C		|		  Data D		|		  Data E		|		  Data F		|		  Data G		|
						+-----------+-----------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		Flow			|	Type 3	|FCflag0,1,2|		Block Size		|			ST			|						|						|						|						|						|
						+-----------+-----------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		�\��			|	4..15	|	0..15	|		 Data A			|		  Data B		|		  Data C		|		  Data D		|		  Data E		|		  Data F		|		  Data G		|
						+-----------+-----------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+

	����(�v��CAN-ID + 0x008)
						+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		Byte			|			0			|			1			|			2			|			3			|			4			|			5			|			6			|			7			|
						+-----------+-----------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		Bit(LE)			|	7..4	|	3..0	|		  15..8			|		  23..16		|		  31..24		|						|						|						|						|
						+-----------+-----------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		Single			|	Type 0	| Size 0..7	|		 Data A			|		  Data B		|		  Data C		|		  Data D		|		  Data E		|		  Data F		|		  Data G		|
						+-----------+-----------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		First			|	Type 1	| Size 8..4095						|		  Data A		|		  Data B		|		  Data C		|		  Data D		|		  Data E		|		  Data F		|
						+-----------+-----------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		Consecutive		|	Type 2	|Index 0..15|		 Data A			|		  Data B		|		  Data C		|		  Data D		|		  Data E		|		  Data F		|		  Data G		|
						+-----------+-----------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		Flow			|	Type 3	|FCflag0,1,2|		Block Size		|			ST			|						|						|						|						|						|
						+-----------+-----------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
	

*/

//----------------------------------------------------------------------------------------
//	TYPE�R�[�h��`
//----------------------------------------------------------------------------------------
#define		CAN_TP_SINGLE			0x00		/*	�V���O���t���[��								*/
#define		CAN_TP_FIRST			0x01		/*	�}���`�t���[���̐擪							*/
#define		CAN_TP_CONT				0x02		/*	�}���`�t���[���̌p��							*/
#define		CAN_TP_FLOW				0x03		/*	�t���[����										*/

//----------------------------------------------------------------------------------------
//	�^�C�}�[ID�ݒ�
//----------------------------------------------------------------------------------------
//#define		TP_TIMER_ID				1			/*	�������ԃ^�C�}�[ID								*/
//#define		DTC_TIMER_ID			2			/*	DTC�p�����ԃ^�C�}�[ID							*/

//----------------------------------------------------------------------------------------
//	���[�h�t���O�r�b�g����
//----------------------------------------------------------------------------------------
#define		CANTP_MODE_RECV			1			/*	TP���샂�[�h(��M��)							*/
#define		CANTP_MODE_SEND			2			/*	TP���샂�[�h(���M��)							*/
#define		CANTP_MODE_WFL			4			/*	TP���샂�[�h(�t���[�����҂�)					*/
#define		CANTP_MODE_WTE			8			/*	TP���샂�[�h(���M�����҂�)						*/
#define		CANTP_MODE_WTU			16			/*	TP���샂�[�h(�^�C���A�b�v�҂�)					*/

//----------------------------------------------------------------------------------------
//	�o�b�t�@�T�C�Y
//----------------------------------------------------------------------------------------
#define		CAN_TP_BUF_SIZE		256

//	�}���`�t���[���o�b�t�@�^��`
typedef	struct	__can_tp_buffer__
{
	int				RPOS;	//	�ǂݏo���ʒu
	int				WPOS;	//	�������݈ʒu
	unsigned char	BUF[CAN_TP_BUF_SIZE];	//	�o�b�t�@
}	CAN_TP_BUF;

//	�V���O���t���[���^��`
typedef	union	__can_tp_single__
{
	unsigned long	L[2];
	unsigned short	W[4];
	unsigned char	B[8];
	struct	{
		union	{
			unsigned char	BYTE;
			struct	{
				unsigned char	CODE	:	4;	//	�t���[���^�C�v(0)
				unsigned char	SIZE	:	4;	//	�f�[�^�o�C�g��(0�`7)
			}	HEAD;
		}	PCI;
		unsigned char	DATA[7];	//	�f�[�^
	}	FRAME;
}	TP_FRM_SINGL;

//	�}���`�t���[���擪�^��`
typedef	union	__can_tp_first__
{
	unsigned long	L[2];
	unsigned short	W[4];
	unsigned char	B[8];
	struct	{
		union	{
			unsigned char	BYTE;
			struct	{
				unsigned char	CODE	:	4;	//	�t���[���^�C�v(1)
				unsigned char	SIZE	:	4;	//	�f�[�^�o�C�g�����4�r�b�g(0�`F)
			}	HEAD;
		}	PCI;
		unsigned char	SIZEL;		//	�f�[�^�o�C�g������8�r�b�g(00�`FF)
		unsigned char	DATA[6];	//	�f�[�^
	}	FRAME;
}	TP_FRM_FIRST;

//	�}���`�t���[���㑱�^��`
typedef	union	__can_tp_consec__
{
	unsigned long	L[2];
	unsigned short	W[4];
	unsigned char	B[8];
	struct	{
		union	{
			unsigned char	BYTE;
			struct	{
				unsigned char	CODE	:	4;	//	�t���[���^�C�v(2)
				unsigned char	INDEX	:	4;	//	�t���[���C���f�b�N�X�ԍ�(0�`15)
			}	HEAD;
		}	PCI;
		unsigned char	DATA[7];	//	�f�[�^
	}	FRAME;
}	TP_FRM_CONSEC;

//	�}���`�t���[���t���[����^��`
typedef	union	__can_tp_flow__
{
	unsigned long	L[2];
	unsigned short	W[4];
	unsigned char	B[8];
	struct	{
		union	{
			unsigned char	BYTE;
			struct	{
				unsigned char	CODE	:	4;	//	�t���[���^�C�v(3)
				unsigned char	FC		:	4;	//	�t���[����(0=���M����,1=WAIT,2=�I�[�o�[�t���[)
			}	HEAD;
		}	PCI;
		unsigned char	BS;			//	�u���b�N�T�C�Y(0=��M�x������,1�`��M�\�t���[����)
		unsigned char	ST;			//	�t���[����������(0�`127msec,0xF1�`0xF9=100�`900msec)
		unsigned char	DATA[5];	//	����
	}	FRAME;
}	TP_FRM_FLOW;
#define		CANTP_FC_CTS		0	/*	���M����				*/
#define		CANTP_FC_WAIT		1	/*	���M�҂�				*/
#define		CANTP_FC_OVER		2	/*	�o�b�t�@�I�[�o�[�t���[	*/
#define		CANTP_FC_ABORT		2	/*	���~					*/
#define		CANTP_FC_INDEX		3	/*	�C���f�b�N�X�s��v		*/

//	�t���[�����p�̒�`
typedef	union	__can_tp_frame__
{
	unsigned long	L[2];
	unsigned short	W[4];
	unsigned char	B[8];
	TP_FRM_SINGL	SINGLE;		//	�V���O���t���[��
	TP_FRM_FIRST	FIRST;		//	�}���`�t���[���擪
	TP_FRM_CONSEC	CONSEC;		//	�}���`�t���[���㑱
	TP_FRM_FLOW		FLOW;		//	�}���`�t���[���t���[����
}	CAN_TP_FRAME;

//	TP����o�b�t�@�^��`
typedef	struct	__can_tp_packet__
{
	int				MODE;	//	TP���샂�[�h(0:�v���҂�,�r�b�g����(1:��M��,2:���M��,4:�t���[�����҂�,8:���M�����҂�,16:�^�C���A�b�v�҂�))
	int				TIME;	//	DTC�p������(0=����,0<�p����)������I�Ɍp���v�����󂯍X�V����
	int				CH;		//	CAN�|�[�g�ԍ�
	int				ID;		//	CAN Frame ID
	int				INDEX;	//	�}���`�t���[���C���f�b�N�X�ԍ�
	int				SIZE;	//	�}���`�t���[���f�[�^�T�C�Y
	int				BC;		//	�u���b�N�J�E���^
	int				FC;		//	�t���[����(0=���M����,1=WAIT,2=�I�[�o�[�t���[)
	int				BS;		//	�u���b�N�T�C�Y(0=��M�x������,1�`��M�\�t���[����)
	int				ST;		//	�t���[����������(0�`127msec,0xF1�`0xF9=100�`900msec)
	int				TXIF;	//	���M���������v���t���O
	int				TXID;	//	���MCAN-ID�ێ�
	CAN_TP_FRAME	RXF;	//	��M�t���[��
	CAN_TP_FRAME	TXF;	//	���M�t���[��
	CAN_TP_BUF		RXD;	//	��M�o�b�t�@
	CAN_TP_BUF		TXD;	//	���M�o�b�t�@
}	CAN_TP_PACK;

extern	CAN_TP_PACK		tp_pack;	//	TP����ϐ�

//----------------------------------------------------------------------------------------
//	CAN-TP�ϐ�������
//----------------------------------------------------------------------------------------
extern	void	can_tp_init(void);
//----------------------------------------------------------------------------------------
//	CAN-TP �f�[�^�ςݏグ����
//----------------------------------------------------------------------------------------
extern	int	can_tp_build(unsigned char *dp, int sz);
//----------------------------------------------------------------------------------------
//	CAN-TP �f�[�^���M����
//----------------------------------------------------------------------------------------
extern	int	can_tp_send(void);
//----------------------------------------------------------------------------------------
//	�p�����M����
//----------------------------------------------------------------------------------------
extern	void	can_tp_consecutive(void);
//----------------------------------------------------------------------------------------
//	CAN-TP���M��������
//----------------------------------------------------------------------------------------
extern	void	can_tp_txendreq(void);
//----------------------------------------------------------------------------------------
//	CAN-TP���M�����҂������`�F�b�N
//----------------------------------------------------------------------------------------
extern	void	can_tp_txecheck(int ch, int id);
//----------------------------------------------------------------------------------------
//	CAN-TP����
//----------------------------------------------------------------------------------------
extern	int can_tp_job(int ch, int id, void *frame);

#endif		/*__CAN_TRANSE_PORT_PROTOCOL__*/

