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
// $RCSfile: ecu_io.h,v $
// $Revision: 1.00 $
// $Date: 2016/12/15 14:14:48 $
// 
// Copyright (c) 2016 LandF Corporation.
//
// History:
//


#ifndef		__CAN_ECU_IO_MAP__
#define		__CAN_ECU_IO_MAP__

//#define		EX_IO_MAX	64

typedef	union	__ext_io_memory__
{
	int				INTE;
	unsigned long	LONG;
	unsigned short	WORD[2];
	unsigned char	BYTE[4];
	struct	{
		unsigned long	B31	:	1;
		unsigned long	B30	:	1;
		unsigned long	B29	:	1;
		unsigned long	B28	:	1;
		unsigned long	B27	:	1;
		unsigned long	B26	:	1;
		unsigned long	B25	:	1;
		unsigned long	B24	:	1;
		unsigned long	B23	:	1;
		unsigned long	B22	:	1;
		unsigned long	B21	:	1;
		unsigned long	B20	:	1;
		unsigned long	B19	:	1;
		unsigned long	B18	:	1;
		unsigned long	B17	:	1;
		unsigned long	B16	:	1;
		unsigned long	B15	:	1;
		unsigned long	B14	:	1;
		unsigned long	B13	:	1;
		unsigned long	B12	:	1;
		unsigned long	B11	:	1;
		unsigned long	B10	:	1;
		unsigned long	B9	:	1;
		unsigned long	B8	:	1;
		unsigned long	B7	:	1;
		unsigned long	B6	:	1;
		unsigned long	B5	:	1;
		unsigned long	B4	:	1;
		unsigned long	B3	:	1;
		unsigned long	B2	:	1;
		unsigned long	B1	:	1;
		unsigned long	B0	:	1;
	}	BIT;
}	EX_IO_MEM;

typedef	union	__ext_io_status__
{
	EX_IO_MEM	DATA[EX_IO_MAX];
	struct	{
		//	0	�C���p�l�FPCS�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	PCS;
		//	1	�C���p�l�F�G���W���}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	WAR_ENG;
		//	2	�C���p�l�F���i�I�j�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	WAR_Y;
		//	3	�C���p�l�F�w�b�h���C�g�㉺�����}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_HLUDC;
		//	4	�C���p�l�F(ABS)�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	WAR_ABS;
		//	5	�C���p�l�F���A�t�H�O�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_RFOG;
		//	6	�C���p�l�F�o�b�e���[�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	WAR_BATT;
		//	7	�C���p�l�F�ԁi�I�j�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	WAR_R;
		//	8	�C���p�l�F�h�A�J�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_DOP;
		//	9	�C���p�l�F�t�����g�t�H�O�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_FFOG;
		//	10	�C���p�l�F�G�A�o�b�O�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	WAR_ABG;
		//	11	�C���p�l�F���x�v�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_ETMP;
		//	12	�C���p�l�F�n���h���I�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	WAR_RUD;
		//	13	�C���p�l�F�|�W�V�����i�����j�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_POSL;
		//	14	�C���p�l�F�Z�L�����e�B�[�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	WAR_SCQ;
		//	15	�C���p�l�F�n�C�r�[���}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_HIB;
		//	16	�C���p�l�F�������x�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_ATS;
		//	17	�C���p�l�FSET�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_SET;
		//	18	�C���p�l�F�ԁ{���[�^�[�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_ATC;
		//	19	�C���p�l�F�V�[�g�x���g�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	WAR_SB;
		//	20	�C���p�l�F�X���b�v�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	WAR_SLP;
		//	21	�C���p�l�F�̏�I�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	EMG_R;
		//	22	�C���p�l�FREADY�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_READY;
		//	23	�C���p�l�F���E�B���J�[�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_LBRINKER;
		//	24	�C���p�l�FPWR MODE�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_PWRMODE;
		//	25	�C���p�l�FECO MODE�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_ECOMODE;
		//	26	�C���p�l�FEV MODE�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_EVMODE;
		//	27	�C���p�l�F�E�E�B���J�[�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_RBRINKER;
		//	28	�C���p�l�F�R��(km/L)�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	DTP_KMPL;
		//	29	�C���p�l�F����(km/h)�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	DTP_KMPH;
		//	30	�C���p�l�F�T�C�h�u���[�L�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_SIDEBK;
		//	31	�C���p�l�F�A�N�Z���}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_ACC;
		//	32	�C���p�l�F�V�t�g�|�W�V�����}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_SHIFTPOS;
		//	33	�C���p�l�F�R��Q�[�W
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	VAL_KMPL;
		//	34	�C���p�l�F�R���Q�[�W
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	VAL_FUIEL;
		//	35	�C���p�l�F���x�\��
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	VAL_SPEED;
		//	36	�C���p�l�F�p�b�V���O�X�C�b�`
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	SW_PASSING;
		//	37	�C���p�l�F�|�W�V�����i�����j�X�C�b�`
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	SW_POSITION;
		//	38	�C���p�l�F���[�r�[���X�C�b�`
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	SW_LOWBEAM;
		//	39	�C���p�l�F�n�C�r�[���X�C�b�`
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	SW_HIGHBEAM;
		//	40	�C���p�l�F�A�N�Z�����ݍ��ݗ�[%]
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	VAL_ACC;
		//	41	�C���p�l�F�u���[�L���ݍ��ݗ�[%]
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	VAL_BRK;
		//	42	�C���p�l�F�n���h���ʒu[�}%]
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	VAL_RUD;
		//	43	�C���p�l�F�G���W�����x[��]
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	VAL_ETMP;
		//	44	�C���p�l�F�E�h�A�E�B���h�E�ʒu[%]
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	VAL_RDWP;
		//	45	�C���p�l�F���h�A�E�B���h�E�ʒu[%]
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	VAL_LDWP;
		//	46	�C���p�l�F�E�h�A���b�N�E�A�����b�N[ON,OFF]
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	SW_RLOCK;
		//	47	�C���p�l�F���h�A���b�N�E�A�����b�N[ON,OFF]
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	SW_LLOCK;
		//	48	�C���p�l�F�t�����g�E�H�b�V���[�t�f�o�X�C�b�`
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	SW_FWASH;
		//	49	�C���p�l�F�t�����g���C�p�[�쓮�X�C�b�`
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	SW_FWIPER;
		//	50	�C���p�l�F���A�E�H�b�V���[�t�f�o�X�C�b�`
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	SW_RWASH;
		//	51	�C���p�l�F���A���C�p�[�쓮�X�C�b�`
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	SW_RWIPER;
		//	52	�C���p�l�F�E�h�A�E�B���h�E�㉺�X�C�b�`
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	SW_RWUD;
		//	53	�C���p�l�F���h�A�E�B���h�E�㉺�X�C�b�`
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	SW_LWUD;
		//	54	�C���p�l�F�G���W����]��
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	VAL_ERPM;
		//	55	�C���p�l�F�`���C���h���b�N�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_CHALOCK;
		//	56	�C���p�l�F�n���h���ڕW�ʒu�i�p���X�e�j
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	SYS_RUDTG;
		//	57	�C���p�l�F�n���h���g���N�i�p���X�e�j
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	SYS_RUDTQ;
		//	58	�C���p�l�F�G���W���X�^�[�g�X�C�b�`
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	SW_ESTART;
		//	59	�C���p�l�F�R����Z�o�t���O
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	FLG_ATKMPL;
		//	60	�C���p�l�F�R����������t���O
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	FLG_ATFL;
		//	61	�C���p�l�FPWR�EECO�EEV���[�h�����\���t���O
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	FLG_ATRMODE;
		//	62	�C���p�l�F�f�����X�g���[�V�������s���t���O
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	FLG_ATDEMO;
		//	63	�C���p�l�F�\�����[�h�ԍ�
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	SYS_DISPMODE;
		//	64	�C���p�l�F����؂�ւ����̓X�C�b�`
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	SYS_CNTSEL;
		//	65	�C���p�l�F�V�t�g���o�[UP/DOWN�X�C�b�`
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	SW_SHIFTUD;
		//	66	���g�p
		unsigned long	RES_66;
		//	67	���g�p
		unsigned long	RES_67;
		//	68	���g�p
		unsigned long	RES_68;
		//	69	���g�p
		unsigned long	RES_69;
		//	70	���g�p
		unsigned long	RES_70;
		//	71	���g�p
		unsigned long	RES_71;
		//	72	���g�p
		unsigned long	RES_72;
		//	73	���g�p
		unsigned long	RES_73;
		//	74	���g�p
		unsigned long	RES_74;
		//	75	���g�p
		unsigned long	RES_75;
		//	76	���g�p
		unsigned long	RES_76;
		//	77	���g�p
		unsigned long	RES_77;
		//	78	���g�p
		unsigned long	RES_78;
		//	79	�C���p�l�F�Ԍ����C�p�[�Ԋu�{�����[��
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	VAL_FWINT;
		//	80	�C���p�l�F�t�����g���C�p�[�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_FW;
		//	81	�C���p�l�F�t�����g���C�p�[AUTO�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_FWAT;
		//	82	�C���p�l�F�t�����g���C�p�[�E�H�b�V���}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_FWWASH;
		//	83	�C���p�l�F�t�����g�E�H�b�V���[�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_FWASH;
		//	84	�C���p�l�F���A���C�p�[�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_RW;
		//	85	�C���p�l�F���A���C�p�[AUTO�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_RWAT;
		//	86	�C���p�l�F���A���C�p�[�E�H�b�V���}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_RWWASH;
		//	87	�C���p�l�F���A�E�H�b�V���[�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_RWASH;
		//	88	���g�p
		unsigned long	RES_88;
		//	89	���g�p
		unsigned long	RES_89;
		//	90	�C���p�l�F�{���l�b�g�J�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_BCOP;
		//	91	�C���p�l�F�g�����N�J�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_TROP;
		//	92	���g�p
		unsigned long	RES_92;
		//	93	���g�p
		unsigned long	RES_93;
		//	94	�C���p�l�F�z�[���}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_HORN;
		//	95	�C���p�l�F�d���i�Z�j�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_LMP;
		//	96	�C���p�l�F�d���i�I�j�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	WAR_LMP;
		//	97	�C���p�l�F�d���i�~�j�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	EMG_LMP;
		//	98	�C���p�l�FBluetooth�}�[�N
		union	{
			unsigned long	L;
			struct	{
				unsigned long	VAL		:	16;		//	
				unsigned long	OUT		:	8;		//	
				unsigned long	IN		:	8;		//	
			}	BIT;
		}	INF_BTT;
		//	99	���g�p
		unsigned long	RES_99;
		//	�ȉ��A���g�p
		unsigned long	RES_XX[28];
	}	UNIT;
}	EXT_IO_STATUS;

//	�O�����o�͋@����
extern	EXT_IO_STATUS	exiosts;

//	�f�W�^���r�b�g����
#define		X_DB_0		PORTC.PIDR.BIT.B0	/*	CN6-19(IRQ14)	PC0	*/
#define		X_DB_1		PORTC.PIDR.BIT.B1	/*	CN6-20(IRQ12)	PC1	*/
#define		X_DB_2		PORTC.PIDR.BIT.B2	/*	CN6-21(SMISO5)	PC2	*/
#define		X_DB_3		PORTC.PIDR.BIT.B3	/*	CN6-22(SMOSI5)	PC3	*/
#define		X_DB_4		PORTC.PIDR.BIT.B4	/*	CN6-23(SCK5)	PC4	*/
#define		X_DB_5		PORTC.PIDR.BIT.B5	/*	CN6-24(SS0)		PC5	*/
#define		X_DB_6		PORTC.PIDR.BIT.B6	/*	CN6-25(SS1)		PC6	*/
#define		X_DB_7		PORT6.PIDR.BIT.B4	/*	CN6-26(EXP)		P64	*/

#define		X_DB_8		PORTB.PIDR.BIT.B2	/*	P1-1 (D8)		PB2	*/
#define		X_DB_9		PORTB.PIDR.BIT.B4	/*	P1-2 (D9)		PB4	*/
#define		X_DB_10		PORTB.PIDR.BIT.B5	/*	P1-3 (D10)		PB5	*/
#define		X_DB_11		PORTB.PIDR.BIT.B1	/*	P1-4 (D11)		PB1	*/
#define		X_DB_12		PORTB.PIDR.BIT.B0	/*	P1-5 (D12)		PB0	*/
#define		X_DB_13		PORTB.PIDR.BIT.B3	/*	P1-6 (D13)		PB3	*/
#define		X_DB_14		PORTB.PIDR.BIT.B7	/*	P1-9 (SDA)		PB7	*/
#define		X_DB_15		PORTB.PIDR.BIT.B6	/*	P1-10(SCL)		PB6	*/

#define		X_DB_16		PORTA.PIDR.BIT.B2	/*	P3-1 (D0)		PA2	*/
#define		X_DB_17		PORTA.PIDR.BIT.B4	/*	P3-2 (D1)		PA4	*/
#define		X_DB_18		PORTA.PIDR.BIT.B0	/*	P3-3 (D2)		PA0	*/
#define		X_DB_19		PORTA.PIDR.BIT.B1	/*	P3-4 (D3)		PA1	*/
#define		X_DB_20		PORTA.PIDR.BIT.B5	/*	P3-5 (D4)		PA5	*/
#define		X_DB_21		PORTA.PIDR.BIT.B3	/*	P3-6 (D5)		PA3	*/
#define		X_DB_22		PORTA.PIDR.BIT.B6	/*	P3-7 (D6)		PA6	*/
#define		X_DB_23		PORTA.PIDR.BIT.B7	/*	P3-8 (D7)		PA7	*/

//	�f�W�^���r�b�g�o��
#define		Y_DB_0		PORTC.PODR.BIT.B0	/*	CN6-19(IRQ14)	PC0	*/
#define		Y_DB_1		PORTC.PODR.BIT.B1	/*	CN6-20(IRQ12)	PC1	*/
#define		Y_DB_2		PORTC.PODR.BIT.B2	/*	CN6-21(SMISO5)	PC2	*/
#define		Y_DB_3		PORTC.PODR.BIT.B3	/*	CN6-22(SMOSI5)	PC3	*/
#define		Y_DB_4		PORTC.PODR.BIT.B4	/*	CN6-23(SCK5)	PC4	*/
#define		Y_DB_5		PORTC.PODR.BIT.B5	/*	CN6-24(SS0)		PC5	*/
#define		Y_DB_6		PORTC.PODR.BIT.B6	/*	CN6-25(SS1)		PC6	*/
#define		Y_DB_7		PORT6.PODR.BIT.B4	/*	CN6-26(EXP)		P64	*/

#define		Y_DB_8		PORTB.PODR.BIT.B2	/*	P1-1 (D8)		PB2	*/
#define		Y_DB_9		PORTB.PODR.BIT.B4	/*	P1-2 (D9)		PB4	*/
#define		Y_DB_10		PORTB.PODR.BIT.B5	/*	P1-3 (D10)		PB5	*/
#define		Y_DB_11		PORTB.PODR.BIT.B1	/*	P1-4 (D11)		PB1	*/
#define		Y_DB_12		PORTB.PODR.BIT.B0	/*	P1-5 (D12)		PB0	*/
#define		Y_DB_13		PORTB.PODR.BIT.B3	/*	P1-6 (D13)		PB3	*/
#define		Y_DB_14		PORTB.PODR.BIT.B7	/*	P1-9 (SDA)		PB7	*/
#define		Y_DB_15		PORTB.PODR.BIT.B6	/*	P1-10(SCL)		PB6	*/

#define		Y_DB_16		PORTA.PODR.BIT.B2	/*	P3-1 (D0)		PA2	*/
#define		Y_DB_17		PORTA.PODR.BIT.B4	/*	P3-2 (D1)		PA4	*/
#define		Y_DB_18		PORTA.PODR.BIT.B0	/*	P3-3 (D2)		PA0	*/
#define		Y_DB_19		PORTA.PODR.BIT.B1	/*	P3-4 (D3)		PA1	*/
#define		Y_DB_20		PORTA.PODR.BIT.B5	/*	P3-5 (D4)		PA5	*/
#define		Y_DB_21		PORTA.PODR.BIT.B3	/*	P3-6 (D5)		PA3	*/
#define		Y_DB_22		PORTA.PODR.BIT.B6	/*	P3-7 (D6)		PA6	*/
#define		Y_DB_23		PORTA.PODR.BIT.B7	/*	P3-8 (D7)		PA7	*/

//	�A�i���O����
#define		X_AN_0		S12AD.ADDR0			/*	CN6-15(AN000)	P40	*/
#define		X_AN_1		S12AD.ADDR1			/*	CN6-16(AN001)	P41	*/
#define		X_AN_2		S12AD.ADDR2			/*	P2-1  (AN002)	P42	*/
#define		X_AN_3		S12AD.ADDR3			/*	P2-2  (AN003)	P43	*/
#define		X_AN_4		S12AD.ADDR4			/*	P2-3  (AN004)	P44	*/
#define		X_AN_5		S12AD.ADDR5			/*	P2-4  (AN005)	P45	*/
#define		X_AN_6		S12AD.ADDR6			/*	P2-5  (AN006)	P46	*/
#define		X_AN_7		S12AD.ADDR7			/*	P2-6  (AN007)	P47	*/

//	�A�i���O�o��
#define		Y_AN_0		DA.DADR0			/*	CN6-17(DA0)		P03	*/
#define		Y_AN_1		DA.DADR1			/*	CN6-18(DA1)		P05	*/

#endif		/*__CAN_ECU_IO_MAP__*/
