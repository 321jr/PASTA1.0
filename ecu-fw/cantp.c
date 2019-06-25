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

#include <sysio.h>
#include <string.h>
#include <stdio.h>
#include "iodefine.h"
#include "timer.h"
#include "ecu.h"			/*	ECU ���ʒ�`			*/
#include "can3_spi2.h"		/*	CAN3 ��`				*/
#include "cantp.h"			/*	CAN-TP ��`				*/
#include "obd2.h"			/*	CAN-OBDII ��`			*/
#include "uds.h"			/*	CAN-UDS ��`			*/

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
#if	0
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
#define		TP_TIMER_ID				1			/*	�������ԃ^�C�}�[ID								*/
#define		DTC_TIMER_ID			2			/*	DTC�p�����ԃ^�C�}�[ID							*/

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
#endif
CAN_TP_PACK		tp_pack;	//	TP����ϐ�

//----------------------------------------------------------------------------------------
//	CAN-TP�ϐ�������
//----------------------------------------------------------------------------------------
void	can_tp_init(void)
{
	memset(&tp_pack, 0, sizeof(CAN_TP_PACK));
}

//----------------------------------------------------------------------------------------
//	CAN-TP �f�[�^�ςݏグ����
//----------------------------------------------------------------------------------------
int	can_tp_build(unsigned char *dp, int sz)
{
	int	i;
	int	f = 0;
	//	�f�[�^�ςݏグ
	if(sz > 0)
	{	//	�f�[�^�L��
		for(i = 0; i < sz && tp_pack.RXD.WPOS < tp_pack.SIZE; i++)
		{
			tp_pack.RXD.BUF[tp_pack.RXD.WPOS++] = *dp++;
		}
		if(tp_pack.RXD.WPOS == tp_pack.SIZE)
		{	//	�S�f�[�^��M����
			if(tp_pack.RXD.BUF[0] < 0x10)
			{	//	OBD2�v���g�R��
				f = obd2_job(tp_pack.RXD.BUF, tp_pack.RXD.WPOS, tp_pack.TXD.BUF);
			}
			else
			{	//	UDS�v���g�R��
				f = uds_job(tp_pack.RXD.BUF, tp_pack.RXD.WPOS, tp_pack.TXD.BUF);
			}
			if(f > 0)
			{
				tp_pack.TXD.RPOS = 0;
				tp_pack.TXD.WPOS = f;
			}
		}
	}
	return f;
}

//----------------------------------------------------------------------------------------
//	CAN-TP �f�[�^���M����
//----------------------------------------------------------------------------------------
int	can_tp_send(void)
{
	int				i, sz;
	unsigned char	*dp;
	//	�f�[�^�ςݏグ
	if(tp_pack.TXD.RPOS < tp_pack.TXD.WPOS)
	{	//	�f�[�^�L��
		memset(&tp_pack.TXF, 0, sizeof(CAN_TP_FRAME));
		if(tp_pack.TXD.WPOS < 8)
		{	//	�V���O���t���[���ő��M
			tp_pack.TXF.SINGLE.FRAME.PCI.HEAD.CODE = CAN_TP_SINGLE;
			tp_pack.TXF.SINGLE.FRAME.PCI.HEAD.SIZE = tp_pack.TXD.WPOS;
			tp_pack.SIZE = tp_pack.TXD.WPOS;
			tp_pack.INDEX = 1;
			tp_pack.BC = 0;
			tp_pack.MODE = CANTP_MODE_SEND;
			dp = tp_pack.TXF.SINGLE.FRAME.DATA;
			sz = 7;
		}
		else
		if(tp_pack.TXD.RPOS == 0)
		{	//	�}���`�t���[���ő��M�A�擪
			tp_pack.TXF.FIRST.FRAME.PCI.HEAD.CODE = CAN_TP_FIRST;
			tp_pack.TXF.FIRST.FRAME.PCI.HEAD.SIZE = (tp_pack.TXD.WPOS >> 8) & 0x0F;
			tp_pack.TXF.FIRST.FRAME.SIZEL = tp_pack.TXD.WPOS & 0xFF;
			tp_pack.SIZE = tp_pack.TXD.WPOS;
			tp_pack.BC = 0;
			tp_pack.INDEX = 1;
			tp_pack.MODE = CANTP_MODE_SEND | CANTP_MODE_WFL;	//	�t���[�҂�
			dp = tp_pack.TXF.FIRST.FRAME.DATA;
			sz = 6;
		}
		else
		{	//	�}���`�t���[���ő��M�A�p��
			tp_pack.TXF.CONSEC.FRAME.PCI.HEAD.CODE = CAN_TP_CONT;
			tp_pack.TXF.CONSEC.FRAME.PCI.HEAD.INDEX = tp_pack.INDEX++;
			tp_pack.INDEX &= 15;
			tp_pack.BC++;
			tp_pack.MODE = CANTP_MODE_SEND | CANTP_MODE_WTE;	//	���M�����҂�
			if(tp_pack.BC >= tp_pack.BS && tp_pack.BS > 0)
			{	//	�A���u���b�N�����B
				tp_pack.BC = 0;
				tp_pack.FC = CANTP_FC_WAIT;
				tp_pack.MODE |= CANTP_MODE_WFL;	//	�t���[�҂�
			}
			dp = tp_pack.TXF.CONSEC.FRAME.DATA;
			sz = tp_pack.TXD.WPOS - tp_pack.TXD.RPOS;
			if(sz > 7) sz = 7;
		}
		//	�f�[�^�R�s�[
		for(i = 0; i < sz && tp_pack.TXD.RPOS < tp_pack.TXD.WPOS; i++)
		{
			*dp++ = tp_pack.TXD.BUF[tp_pack.TXD.RPOS++];
		}
		if(tp_pack.TXD.RPOS == tp_pack.SIZE)
		{	//	�S�f�[�^���M����
			tp_pack.MODE = 0;
		}
		return 1;	//	���M�L��
	}
	return 0;	//	���M����
}

//----------------------------------------------------------------------------------------
//	CAN-TP �t���[���M����
//----------------------------------------------------------------------------------------
int	can_tp_flow(void)
{
	//	�f�[�^�ςݏグ
	memset(&tp_pack.TXF, 0, sizeof(CAN_TP_FRAME));
	tp_pack.TXF.FLOW.FRAME.PCI.HEAD.CODE = CAN_TP_FLOW;	//	�t���[�R���g���[��
	tp_pack.TXF.FLOW.FRAME.PCI.HEAD.FC = CANTP_FC_CTS;	//	���M����
	tp_pack.TXF.FLOW.FRAME.BS = 1;						//	�u���b�N�T�C�Y(0=�A��)
	tp_pack.TXF.FLOW.FRAME.ST = 0;						//	�t���[����������(1ms)
	tp_pack.BS = tp_pack.TXF.FLOW.FRAME.BS;
	tp_pack.BC = 0;
	tp_pack.MODE = 0;
	return 1;
}

//----------------------------------------------------------------------------------------
//	�p�����M����
//----------------------------------------------------------------------------------------
void	can_tp_consecutive(void)
{
	int				id = SELECT_ECU_UNIT + 0x7E8;
	int				f = 0;
	
	switch(tp_pack.FC)
	{
	case CANTP_FC_CTS:	//	���M����
		if(tp_pack.MODE & CANTP_MODE_SEND)
		{	//	���M��
			if(tp_pack.MODE & CANTP_MODE_WTE)
			{	//	���M�����҂�
				return;
			}
			if(tp_pack.MODE & CANTP_MODE_WTU)
			{
				if(tp_pack.ST > 0)
				{	//	�^�C�}�[�L��
					if(check_timer(TP_TIMER_ID) == 0)
					{	//	�^�C���A�b�v�҂�
						return;
					}
				}
				tp_pack.MODE ^= CANTP_MODE_WTU;
			}
			//	�p�����M�t���[������
			f = can_tp_send();
		}
		break;
	case CANTP_FC_WAIT:	//	���҂�
		if(tp_pack.ST > 0)
		{	//	�^�C�}�[�L��
			if(check_timer(TP_TIMER_ID))
			{	//	�^�C���I�[�o�[
				tp_pack.MODE = 0;
				return;
			}
		}
		break;
	case CANTP_FC_ABORT:	//	���~
		tp_pack.MODE = 0;	//	�ҋ@
		tp_pack.CH = -1;
		tp_pack.ID = -1;
		break;
	}
	if(f != 0)
	{	//	�ԐM
		memcpy(&can_buf.ID[id], tp_pack.TXF.B, 8);
		add_mbox_frame(tp_pack.CH, 8, CAN_DATA_FRAME, id);	//	���M�҂��o�b�t�@�ςݏグ
	}
}

//----------------------------------------------------------------------------------------
//	CAN-TP���M��������(main����Ăяo��)
//----------------------------------------------------------------------------------------
void	can_tp_txendreq(void)
{
	if(tp_pack.ST > 0)
	{	//	���̑��M�܂ł̎��Ԑݒ�
		if(tp_pack.FC == 0 && (tp_pack.BS > tp_pack.BC || tp_pack.BS == 0))
		{	//	���M����
			tp_pack.MODE |= CANTP_MODE_WTU;	//	�^�C���A�b�v�҂�
			after_call(TP_TIMER_ID, tp_pack.ST, can_tp_consecutive);	//	�^�C�}�[�Z�b�g
		}
	}
	else
	{	//	�������Ԗ���
		if(tp_pack.FC == 0 && (tp_pack.BS > tp_pack.BC || tp_pack.BS == 0))
		{	//	���M����
			can_tp_consecutive();
		}
	}
}

//----------------------------------------------------------------------------------------
//	CAN-TP���M�����҂������`�F�b�N
//----------------------------------------------------------------------------------------
void	can_tp_txecheck(int ch, int id)
{
	if(tp_pack.CH == ch && tp_pack.TXID == id)
	{
		if(tp_pack.MODE & CANTP_MODE_WTE)
		{	//	���M�����҂�
			tp_pack.MODE ^= CANTP_MODE_WTE;	//	�҂�����
			tp_pack.TXIF = 1;				//	���M���������v���t���O
		}
	}
}

//----------------------------------------------------------------------------------------
//	CAN-TP����
//----------------------------------------------------------------------------------------
int can_tp_job(int ch, int id, void *frame)
{
	int				sw = SELECT_ECU_UNIT + 0x7E0;
	int				f = 0;
	int				sz, i;
	unsigned char	*dp;
	
	if(id != 0x7DF && id != sw) return 0;
	
	memcpy(&tp_pack.RXF, frame, sizeof(CAN_TP_FRAME));
	memset(&tp_pack.TXF, 0x00, sizeof(CAN_TP_FRAME));
	
	switch(tp_pack.RXF.SINGLE.FRAME.PCI.HEAD.CODE)
	{
	case CAN_TP_SINGLE:	//	�V���O���t���[��
		if(tp_pack.MODE == 0)
		{	//	�ҋ@��
			sz = tp_pack.RXF.SINGLE.FRAME.PCI.HEAD.SIZE;
			tp_pack.CH = ch;
			tp_pack.ID = id;
			tp_pack.INDEX = 0;
			tp_pack.SIZE = sz;
			memset(&tp_pack.RXD, 0, sizeof(CAN_TP_BUF));
			dp = tp_pack.RXF.SINGLE.FRAME.DATA;
			f = can_tp_build(dp, sz);
			if(f > 0)
			{	//	���M����
				if(can_tp_send() == 0) f = 0;
			}
		}
		break;
	case CAN_TP_FIRST:	//	�}���`�擪�t���[��
		if(tp_pack.MODE == 0)
		{	//	�ҋ@��
			sz = (tp_pack.RXF.FIRST.FRAME.PCI.HEAD.SIZE) << 8;
			sz |= (tp_pack.RXF.FIRST.FRAME.SIZEL) & 0xFF;
			tp_pack.CH = ch;
			tp_pack.ID = id;
			tp_pack.INDEX = 1;
			tp_pack.SIZE = sz;
			memset(&tp_pack.RXD, 0, sizeof(CAN_TP_BUF));
			dp = tp_pack.RXF.FIRST.FRAME.DATA;
			sz = 6;
			f = can_tp_build(dp, sz);
			if(f > 0)
			{	//	���M����
				if(can_tp_send() == 0) f = 0;
			}
			else
			{	//	�������M
				f = can_tp_flow();
			}
		}
		break;
	case CAN_TP_CONT:	//	�}���`�p���t���[��
		if(tp_pack.MODE == 0)
		{	//	�ҋ@��
			if(tp_pack.CH == ch && tp_pack.ID == id)
			{	//	�|�[�g�AID��v
				i = tp_pack.RXF.CONSEC.FRAME.PCI.HEAD.INDEX;
				if(i != tp_pack.INDEX)
				{	//	�C���f�b�N�X����v���Ȃ�
					tp_pack.TXF.FLOW.FRAME.PCI.HEAD.CODE = CAN_TP_CONT;			//	�t���[����
					tp_pack.TXF.FLOW.FRAME.PCI.HEAD.FC = CANTP_FC_ABORT;		//	���~
					tp_pack.TXF.FLOW.FRAME.BS = 0;								//	
					tp_pack.TXF.FLOW.FRAME.ST = 0;								//	
					tp_pack.TXF.FLOW.FRAME.DATA[0] = tp_pack.INDEX;				//	���݂̃C���f�b�N�X
					f = 1;
				}
				else
				{	//	�C���f�b�N�X��v
					tp_pack.BC++;
					tp_pack.INDEX++;
					tp_pack.INDEX &= 15;
					dp = tp_pack.RXF.CONSEC.FRAME.DATA;
					sz = 7;
					f = can_tp_build(dp, sz);
					if(f > 0)
					{	//	���M����
						if(can_tp_send() == 0) f = 0;
					}
					else
					if(tp_pack.BC >= tp_pack.BS && tp_pack.BS > 0)
					{	//	�u���b�N�����B�@�t���[���䑗�M
						tp_pack.TXF.FLOW.FRAME.PCI.HEAD.CODE = CAN_TP_FLOW;	//	�t���[�R���g���[��
						tp_pack.TXF.FLOW.FRAME.PCI.HEAD.FC = CANTP_FC_CTS;	//	���M����
						tp_pack.TXF.FLOW.FRAME.BS = tp_pack.BS;				//	�u���b�N�T�C�Y(0=�A��)
						tp_pack.TXF.FLOW.FRAME.ST = 0;						//	�t���[����������(1ms)
						tp_pack.BC = 0;
						f = 1;
					}
				}
			}
		}
		break;
	case CAN_TP_FLOW:	//	�t���[����t���[����M
		if(tp_pack.MODE & CANTP_MODE_WFL)
		{	//	�t���[�ҋ@��
			tp_pack.MODE ^= CANTP_MODE_WFL;	//	�ҋ@����
			if(tp_pack.CH == ch && tp_pack.ID == id)
			{	//	�|�[�g�AID��v
				//	�t���[����
				tp_pack.BC = 0;
				tp_pack.BS = 0;
				tp_pack.ST = 0;
				tp_pack.FC = tp_pack.RXF.FLOW.FRAME.PCI.HEAD.FC;
				switch(tp_pack.FC)
				{
				case CANTP_FC_CTS:		//	���M�p��
					tp_pack.BS = tp_pack.RXF.FLOW.FRAME.BS;		//	�u���b�N�T�C�Y
					if(tp_pack.BS > 0)
					{	//	�w��u���b�N���A���{�x���L��
						tp_pack.ST = tp_pack.RXF.FLOW.FRAME.ST;		//	��������(ms)
						if(tp_pack.ST > 0xF0) tp_pack.ST = (tp_pack.ST - 0xF0) * 100;	//	100�`900ms
					}
					f = can_tp_send();
					break;
				case CANTP_FC_WAIT:		//	���M�҂�
					tp_pack.MODE |= CANTP_MODE_WFL | CANTP_MODE_WTU;
					tp_pack.BS = -1;	//	���M�֎~
					tp_pack.ST = 10000;	//	�^�C���A�E�g�K��(10sec)
					after_call(TP_TIMER_ID, tp_pack.ST, can_tp_consecutive);
					break;
				default:				//	�G���[���~(�I�[�o�[�t���[�A�A�{�[�g)
					tp_pack.MODE = 0;	//	�ҋ@
					tp_pack.CH = -1;
					tp_pack.ID = -1;
					break;
				}
			}
		}
		break;
	default:	//	���Ή����[�h
		return 0;
	}
	if(tp_pack.TXF.B[0] != 0 && f != 0)
	{	//	�����ԐM
		sw += 8;
		tp_pack.TXID = sw;
		memcpy(&can_buf.ID[sw], tp_pack.TXF.B, 8);
		if(ch >= 0) add_mbox_frame(ch, 8, CAN_DATA_FRAME, sw);	//	���M�҂��o�b�t�@�ςݏグ
		return sw;
	}
	return 0;
}



