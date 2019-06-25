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
//  LFY-RX63N1  CMT �^�C�}�[����
//
//----------------------------------------------------------------------------------------
//  �J������
//
//  2016/02/10  �R�[�f�B���O�J�n�i�k�j
//
//----------------------------------------------------------------------------------------
//  T.Tachibana
//  ��L&F
//________________________________________________________________________________________
//

#include	<string.h>
#include	"iodefine.h"
#include	"timer.h"

//  �ԐڌĂяo���̃v���g�^�C�v(�����P��)
//typedef   void	(*TIMER_CALL)(void);
//
//#define		CMT0_COUNT_NUM	6000				// 1ms�^�C�}�[�ݒ�
//#define		CMT0_COUNT_VAL	CMT0_COUNT_NUM-1	// 1ms�^�C�}�[�ݒ�l
//#define		TIMER_AN		4					//�@�^�C�}�[��
//  1msec�P�ʔėp�^�C�}�[
int				cmt0_time[TIMER_AN];				//  �_�E���J�E���^�[
int				cmt0_tupf[TIMER_AN];				//  �^�C���A�b�v�t���O
TIMER_CALL		cmt0_call[TIMER_AN];				//  �A�b�v��Ăяo���֐�
unsigned int	freerun_timer;						//	�t���[�����^�C�}�[

//#define	   CMT1_COUNT_NUM	6					// 1us�^�C�}�[�ݒ�
//#define	   CMT1_COUNT_VAL	CMT1_COUNT_NUM-1	// 1us�^�C�}�[�ݒ�l
//  1usec�P�ʒZ���҂��^�C�}�[
int				cmt1_tupf;							//	�^�C���A�b�v�t���O
int				cmt1_msec;							//	msec�I�[�_�[�c�J�E���^
int				cmt1_last;							//	�ŏI�J�E���g�l
int				cmt1_time;							//	�ώZ����
TIMER_CALL		cmt1_call;							//	�A�b�v��Ăяo���֐�

//________________________________________________________________________________________
//
//  cmt0_init
//----------------------------------------------------------------------------------------
//  �@�\����
//	  CMT0������
//  ����
//	  ����
//  �߂�
//	  ����
//________________________________________________________________________________________
//
void cmt0_init(void)
{
	freerun_timer = 0;
	memset(cmt0_time, 0, sizeof(cmt0_time));
	memset(cmt0_tupf, 0, sizeof(cmt0_tupf));
	memset(cmt0_call, 0, sizeof(cmt0_call));
	SYSTEM.PRCR.WORD = 0xA502;		// CLK�֘A���W�X�^�v���e�N�g����
	ICU.IER[0x03].BIT.IEN4 = 1;		// 0=�֎~�@1=���荞�݋���
	ICU.IPR[004].BIT.IPR = 7;		// ���荞�݃��x���i15���ō��j
	MSTP_CMT0 = 0;					// CMT���j�b�g0 ���W���[���d��ON
	CMT0.CMCR.BIT.CKS = 0;			// 0=PCLK/8�@1=PCLK/32 2=PCLK/128 3=PCLK/512
	CMT0.CMCR.BIT.CMIE = 1;			// ���荞�݋���
	CMT0.CMCOR = CMT0_COUNT_VAL;	// 1msec�����荞��
	CMT.CMSTR0.BIT.STR0 = 1;		// 0=��~�A1=�J�n
}

//________________________________________________________________________________________
//
//  start_timer
//----------------------------------------------------------------------------------------
//  �@�\����
//	  �w��^�C�}�[�v���J�n
//  ����
//	  tch		 �`�����l���ԍ�
//	  interval	1msec�P�ʂ̑҂����Ԑݒ�
//  �߂�
//	  ����
//________________________________________________________________________________________
//
void start_timer(int tch, int interval )
{
	cmt0_time[tch] = interval;
	cmt0_tupf[tch] = 0;
	cmt0_call[tch] = 0;
}

//________________________________________________________________________________________
//
//  after_call
//----------------------------------------------------------------------------------------
//  �@�\����
//	  �w��^�C�}�[�v���J�n
//  ����
//	  tch		 �`�����l���ԍ�
//	  interval	1msec�P�ʂ̑҂����Ԑݒ�
//	  *proc	   �A�b�v��Ăяo����
//  �߂�
//	  ����
//________________________________________________________________________________________
//
void after_call(int tch, int interval, void *proc )
{
	if(interval < 0)
	{	//	�����R�[��
		cmt0_time[tch] = -interval;
		cmt0_tupf[tch] = -1;
		cmt0_call[tch] = (TIMER_CALL)proc;
	}
	else
	{	//	�ᑬ�R�[��
		cmt0_time[tch] = interval;
		cmt0_tupf[tch] = 0;
		cmt0_call[tch] = (TIMER_CALL)proc;
	}
}

//________________________________________________________________________________________
//
//  check_timer
//----------------------------------------------------------------------------------------
//  �@�\����
//	  �w��^�C�}�[�A�b�v�m�F
//  ����
//	  tch		 �`�����l���ԍ�
//  �߂�
//	  int		 �c����(msec)
//________________________________________________________________________________________
//
int check_timer(int tch)
{
	return (cmt0_time[tch] > 0) ? 0: 1;
}

//________________________________________________________________________________________
//
//  stop_timer
//----------------------------------------------------------------------------------------
//  �@�\����
//	  �w��^�C�}�[��~
//  ����
//	  tch		 �`�����l���ԍ�
//  �߂�
//	  ����
//________________________________________________________________________________________
//
void stop_timer(int tch)
{
	cmt0_time[tch] = 0;
}

//________________________________________________________________________________________
//
//  wait
//----------------------------------------------------------------------------------------
//  �@�\����
//	  �w��msec�ԑ҂�
//  ����
//	  msec		�҂�����
//	  *loop	   �҂����Ԓ��̊֐��Ăяo��
//  �߂�
//	  ����
//________________________________________________________________________________________
//
void wait(int msec, void *loop)
{
	TIMER_CALL  p = (TIMER_CALL)loop;
	cmt0_time[(TIMER_AN-1)] = msec;
	while(cmt0_time[(TIMER_AN-1)] > 0)
	{
		if(p != 0) p();	//	�҂������s
	}
}

//________________________________________________________________________________________
//
//  cmt0_job
//----------------------------------------------------------------------------------------
//  �@�\����
//	  �^�C���A�b�v�^�C�}�[�̊֐��Ăяo��
//  ����
//	  ����
//  �߂�
//	  ����
//________________________________________________________________________________________
//
void cmt0_job(void)
{
	int			c;
	TIMER_CALL	p;

	for( c = 0; c < TIMER_AN; c++)
	{
		if(cmt0_tupf[c] == 1)
		{  	//	�^�C���A�b�v
			cmt0_tupf[c] = 0;
			p = cmt0_call[c]; 
			if(p != 0)
			{  	//	�֐��Ăяo��
				p();
			}
		}
	}
}

//________________________________________________________________________________________
//
//  cmt0_int
//----------------------------------------------------------------------------------------
//  �@�\����
//	  CMT0���荞��(1msec)
//  ����
//	  ����
//  �߂�
//	  ����
//________________________________________________________________________________________
//
void interrupt __vectno__{VECT_CMT0_CMI0} cmt0_int(void)
{
	int			c;
	TIMER_CALL	p;

	freerun_timer++;	//  1msec�t���[�����J�E���^

	for( c = 0; c < TIMER_AN; c++)
	{
		if(cmt0_time[c] > 0)
		{  	//	�c�J�E���g����
			cmt0_time[c]--;
			if(cmt0_time[c] == 0)
			{  	//	�^�C���A�b�v
				p = cmt0_call[c]; 
				if(p != 0 && cmt0_tupf[c] < 0)
				{  	//	�֐��Ăяo��
					p();
				}
				else
				{
					cmt0_tupf[c] = 1;
				}
			}
		}
	}
}

//________________________________________________________________________________________
//
//  cmt1_init
//----------------------------------------------------------------------------------------
//  �@�\����
//	  CMT1������
//  ����
//	  ����
//  �߂�
//	  ����
//________________________________________________________________________________________
//
void cmt1_init(void)
{
	cmt1_tupf = 0;
	cmt1_last = 0;
	cmt1_msec = 0;
	cmt1_call = 0;
	cmt1_time = 0;
	SYSTEM.PRCR.WORD = 0xA502;						// CLK�֘A���W�X�^�v���e�N�g����
	ICU.IER[IER_CMT1_CMI1].BIT.IEN_CMT1_CMI1 = 0;	// 0=�֎~�@1=���荞�݋���
	ICU.IPR[IPR_CMT1_CMI1].BIT.IPR = 0;				// ���荞�݃��x��   0=�֎~ / 1�`15=����
	CMT.CMSTR0.BIT.STR1 = 0;						// 0=��~�A1=�J�n
	MSTP_CMT1 = 0;									// CMT���j�b�g1 ���W���[���d��ON
	CMT1.CMCR.BIT.CKS = 0;							// PCLK=48MHz : 0=PCLK/8�@1=PCLK/32 2=PCLK/128 3=PCLK/512
	CMT1.CMCR.BIT.CMIE = 0;							// ���荞�� 1:���� / 0:�֎~
	CMT1.CMCOR = 0xFFFF;							// 6MHz / 65536
	CMT1.CMCNT = 0;									// �J�E���^
}

//________________________________________________________________________________________
//
//  cmt1_start
//----------------------------------------------------------------------------------------
//  �@�\����
//	  CMT1�v���J�n(1usec)
//  ����
//	  count	   �҂����� 0�`10000 : 10.000ms : 0.01sec
//	  *proc	   �A�b�v��Ăяo����
//  �߂�
//	  ����
//________________________________________________________________________________________
//
void cmt1_start(int count, void *proc)
{
	cmt1_tupf = 0;
	cmt1_time = 0;
	cmt1_msec = count / 10000;
	cmt1_last = count - cmt1_msec * 10000;
	if(cmt1_msec > 0)
	{
		count = 10000;
	}
	cmt1_call = (TIMER_CALL)proc;
	CMT.CMSTR0.BIT.STR1 = 0;						//  0=��~�A1=�J�n
	ICU.IR[IR_CMT1_CMI1].BIT.IR = 0;				//  ���荞�݃t���O�N���A
	CMT1.CMCR.BIT.CKS = 0;							//	PCLK=48MHz : 0=PCLK/8�@1=PCLK/32 2=PCLK/128 3=PCLK/512
	CMT1.CMCR.BIT.CMIE = 1;							//	���荞�� 1:���� / 0:�֎~
	CMT1.CMCOR = count * CMT1_1US - 1; 				//	6MHz/count
	CMT1.CMCNT = 0;									//	�J�E���^
	ICU.IPR[IPR_CMT1_CMI1].BIT.IPR = 1;				// ���荞�݃��x��   0=�֎~ / 1�`15=����
	ICU.IER[IER_CMT1_CMI1].BIT.IEN_CMT1_CMI1 = 1;	// 0=�֎~�@1=���荞�݋���
	CMT.CMSTR0.BIT.STR1 = 1;						//  0=��~�A1=�J�n
}

//________________________________________________________________________________________
//
//  cmt1_check
//----------------------------------------------------------------------------------------
//  �@�\����
//	  CMT1�^�C���A�b�v�m�F
//  ����
//	  ����
//  �߂�
//	  int	 0=��~�� / 1=�v����
//________________________________________________________________________________________
//
int cmt1_ni_check(void)
{
	int count;
	if(CMT.CMSTR0.BIT.STR1 != 0)
	{
		if(ICU.IR[IR_CMT1_CMI1].BIT.IR != 0)
		{  	//	�J�E���g�A�b�v
			CMT.CMSTR0.BIT.STR1 = 0;			// 0=��~�A1=�J�n
			ICU.IR[IR_CMT1_CMI1].BIT.IR = 0;	//  ���荞�݃t���O�N���A
			if(cmt1_msec > 0)
			{
				cmt1_msec--;
				if(cmt1_msec > 0)
				{
					count = 10000;
				}
				else
				{
					count = cmt1_last;
				}
				CMT1.CMCR.BIT.CKS = 0;			 	//	PCLK=48MHz : 0=PCLK/8�@1=PCLK/32 2=PCLK/128 3=PCLK/512
				CMT1.CMCR.BIT.CMIE = 1;				//	���荞�� 1:���� / 0:�֎~
				CMT1.CMCOR = count * CMT1_1US - 1; 	//	6MHz/count
				CMT1.CMCNT = 0;						//	�J�E���^
				CMT.CMSTR0.BIT.STR1 = 1;			//  0=��~�A1=�J�n
			}
			else
			{
				cmt1_tupf++;
				if(cmt1_call != 0) cmt1_call();
				ICU.IER[IER_CMT1_CMI1].BIT.IEN_CMT1_CMI1 = 0;	// 0=�֎~�@1=���荞�݋���
				ICU.IPR[IPR_CMT1_CMI1].BIT.IPR = 0;				// ���荞�݃��x��   0=�֎~ / 1�`15=����
			}
		}
	}
	return (CMT.CMSTR0.BIT.STR1);	  	//	0=��~ / 1=�v��
}
int cmt1_check(void)
{
	return (CMT.CMSTR0.BIT.STR1);	  	//	0=��~ / 1=�v��
}

//________________________________________________________________________________________
//
//  cmt1_stop
//----------------------------------------------------------------------------------------
//  �@�\����
//	  CMT1��~�ƌo�ߎ��Ԏ擾
//  ����
//	  ����
//  �߂�
//	  �o�ߎ���(1us�P��)
//________________________________________________________________________________________
//
int	cmt1_stop(void)
{
	if(CMT.CMSTR0.BIT.STR1)
	{
		CMT.CMSTR0.BIT.STR1 = 0;						//	0=��~�A1=�J�n
		ICU.IR[IR_CMT1_CMI1].BIT.IR = 0;				//  ���荞�݃t���O�N���A
		ICU.IER[IER_CMT1_CMI1].BIT.IEN_CMT1_CMI1 = 0;	//	0=�֎~�@1=���荞�݋���
		ICU.IPR[IPR_CMT1_CMI1].BIT.IPR = 0;				//	���荞�݃��x��   0=�֎~ / 1�`15=����
		cmt1_time += CMT1.CMCNT / CMT1_1US;
	}
	return cmt1_time;
}

//________________________________________________________________________________________
//
//  cmt1_int
//----------------------------------------------------------------------------------------
//  �@�\����
//	  CMT1���荞��(1usec)
//  ����
//	  ����
//  �߂�
//	  ����
//________________________________________________________________________________________
//
void interrupt __vectno__{VECT_CMT1_CMI1} cmt1_int(void)
{
	int count;
	CMT.CMSTR0.BIT.STR1 = 0;			// 0=��~�A1=�J�n
	ICU.IR[IR_CMT1_CMI1].BIT.IR = 0;	//  ���荞�݃t���O�N���A
	cmt1_time += ((int)CMT1.CMCOR + 1) / CMT1_1US;
	if(cmt1_msec > 0)
	{
		cmt1_msec--;
		if(cmt1_msec > 0)
		{
			count = 10000;
		}
		else
		{
			count = cmt1_last;
		}
		CMT1.CMCR.BIT.CKS = 0;			 	//	PCLK=48MHz : 0=PCLK/8�@1=PCLK/32 2=PCLK/128 3=PCLK/512
		CMT1.CMCR.BIT.CMIE = 1;				//	���荞�� 1:���� / 0:�֎~
		CMT1.CMCOR = count * CMT1_1US - 1; 	//	6MHz/count
		CMT1.CMCNT = 0;						//	�J�E���^
		CMT.CMSTR0.BIT.STR1 = 1;			//  0=��~�A1=�J�n
	}
	else
	{
		ICU.IER[IER_CMT1_CMI1].BIT.IEN_CMT1_CMI1 = 0;	// 0=�֎~�@1=���荞�݋���
		ICU.IPR[IPR_CMT1_CMI1].BIT.IPR = 0;				// ���荞�݃��x��   0=�֎~ / 1�`15=����
		cmt1_tupf++;
		if(cmt1_call != 0) cmt1_call();
	}
}

//________________________________________________________________________________________
//
//  swait
//----------------------------------------------------------------------------------------
//  �@�\����
//	  �V���[�g�E�F�C�g	usec�ԑ҂�
//  ����
//	  usec		�҂�����
//	  *loop	   �҂����Ԓ��̊֐��Ăяo��
//  �߂�
//	  ����
//________________________________________________________________________________________
//
void swait(int usec, void *loop)
{
	int ms = usec / 10000;
	TIMER_CALL  p = (TIMER_CALL)loop;
	for(; ms > 0 && usec > 65535; ms--)
	{
		usec -= 10000;
		cmt1_start(10000, 0);
		while(cmt1_tupf == 0)
		{
			if(p != 0) p();	//	�҂������s
		}
	}
	cmt1_start(usec, 0);
	while(cmt1_tupf == 0)
	{
		if(p != 0) p();	//	�҂������s
	}
}


