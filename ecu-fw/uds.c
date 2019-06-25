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
//	UDS-CAN �v���g�R������
//
//----------------------------------------------------------------------------------------
//	�J������
//
//	2017/08/13	�R�[�f�B���O�J�n�i�k�j
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
#include "uds.h"			/*	CAN-UDS ��`			*/
#include "altypes.h"
#include "r_flash_api_rx600_config.h"
#include "mcu_info.h"
#include "r_flash_api_rx600.h"
#include "r_flash_api_rx600_private.h"

/*
	UDS�i���j�t�@�C�h�E�_�C�A�O�m�X�e�B�b�N�X�E�T�[�r�X�j �����̊T�v
	
	UDS�͉ϒ��p�P�b�g�𑗎�M���铝���f�f�T�[�r�X��񋟂���v���g�R���ł��B
	�{F/W�ł͈ꕔ�̃T�[�r�X�̂ݎ������Ă��܂��B
	
	UDS�T�[�r�X�ꗗ(X=������/O=�������A���ŏ���)
	+-------------------+-------+-------+-----------------------------------------------+-------+
	| Function group	|Request|Respons|	   Service					 				| ����	|
	+-------------------+-------+-------+-----------------------------------------------+-------+
	|	Diagnostic and	|	10	|	50	|	Diagnostic Session Control					|	O	|
	+	Communications	+-------+-------+-----------------------------------------------+-------+
	|	Management		|	11	|	51	|	ECU Reset									|	O	|
	+					+-------+-------+-----------------------------------------------+-------+
	|					|	27	|	67	|	Security Access								|	O	|
	+					+-------+-------+-----------------------------------------------+-------+
	|					|	28	|	68	|	Communication Control						|	X	|
	+					+-------+-------+-----------------------------------------------+-------+
	|					|	3E	|	7E	|	Tester Present								|	O	|
	+					+-------+-------+-----------------------------------------------+-------+
	|					|	83	|	C3	|	Access Timing Parameters					|	X	|
	+					+-------+-------+-----------------------------------------------+-------+
	|					|	84	|	C4	|	Secured Data Transmission					|	X	|
	+					+-------+-------+-----------------------------------------------+-------+
	|					|	85	|	C5	|	Control DTC Setting							|	X	|
	+					+-------+-------+-----------------------------------------------+-------+
	|					|	86	|	C6	|	Response On Event							|	X	|
	+					+-------+-------+-----------------------------------------------+-------+
	|					|	87	|	C7	|	Link Control								|	X	|
	+-------------------+-------+-------+-----------------------------------------------+-------+
	|	Data			|	22	|	62	|	Read Data By Identifier						|	O	|
	+	Transmission	+-------+-------+-----------------------------------------------+-------+
	|					|	23	|	63	|	Read Memory By Address						|	O	|
	+					+-------+-------+-----------------------------------------------+-------+
	|					|	24	|	64	|	Read Scaling Data By Identifier				|	X	|
	+					+-------+-------+-----------------------------------------------+-------+
	|					|	2A	|	6A	|	Read Data By Identifire Periodic			|	X	|
	+					+-------+-------+-----------------------------------------------+-------+
	|					|	2C	|	6C	|	Dynamically Define Data Identifire			|	X	|
	+					+-------+-------+-----------------------------------------------+-------+
	|					|	2E	|	6E	|	Write Data By Identifire					|	O	|
	+					+-------+-------+-----------------------------------------------+-------+
	|					|	3D	|	7D	|	Write Memory By Address						|	O	|
	+-------------------+-------+-------+-----------------------------------------------+-------+
	|	Stored Data		|	14	|	54	|	Clear Diagnostic Information				|	X	|
	+	Transmission	+-------+-------+-----------------------------------------------+-------+
	|					|	19	|	59	|	Read DTC Information						|	X	|
	+-------------------+-------+-------+-----------------------------------------------+-------+
	|	I/O Control		|	2F	|	6F	|	Input Output Control By Identifire			|	X	|
	+-------------------+-------+-------+-----------------------------------------------+-------+
	|	Remote			|		|		|												|		|
	|	Activation of	|	31	|	71	|	Routine Control								|	X	|
	|	Routine			|		|		|												|		|
	+-------------------+-------+-------+-----------------------------------------------+-------+
	|	Upload /		|	34	|	74	|	Request Download							|	O	|
	+	Download		+-------+-------+-----------------------------------------------+-------+
	|					|	35	|	75	|	Request Upload								|	X	|
	+					+-------+-------+-----------------------------------------------+-------+
	|					|	36	|	76	|	Transfer Data								|	O	|
	+					+-------+-------+-----------------------------------------------+-------+
	|					|	37	|	77	|	Request Transfer Exit						|	O	|
	+					+-------+-------+-----------------------------------------------+-------+
	|					|	38	|	78	|	Request File Transfer						|	X	|
	+-------------------+-------+-------+-----------------------------------------------+-------+
*/
#if	0
//----------------------------------------------------------------------------------------
//	UDS�p�P�b�g���p�̒�`
//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
//	Diagnostic and Communications Management
//----------------------------------------------------------------------------------------
//	�_�C�A�O�Z�b�V�����R���g���[���T�[�r�X
typedef	struct	__uds_10_diagnostics_sec__
{
	unsigned char	SID;	//	0x10
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char		:	2;	//	���g�p
			unsigned char	FC	:	6;	//	�Z�b�V�����R�[�h 01:�f�t�H���g / 02:�v���O���~���O / 03:�g���_�C�A�O
		}	BIT;
	}	SUB;
	unsigned char	DATA[UDS_BUFFER_MAX-2];
}	UDS_DIAG_SC;

//	���Z�b�g�T�[�r�X
typedef	struct	__uds_11_ecu_reset__
{
	unsigned char	SID;	//	0x11
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char		:	2;	//	���g�p
			unsigned char	FC	:	6;	//	�Z�b�V�����R�[�h 01:�n�[�h / 02:KeyOnOff / 03:�\�t�g / 04:�����V���b�g�_�E������ / 05:�����V���b�g�_�E���֎~
		}	BIT;
	}	SUB;
	unsigned char	DATA[UDS_BUFFER_MAX-2];
}	UDS_ECU_RES;

//	�Z�L�����e�B�[�A�N�Z�X�T�[�r�X
typedef	struct	__uds_27_security_access__
{
	unsigned char	SID;	//	0x27
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char		:	2;	//	���g�p
			unsigned char	FC	:	6;	//	�Z�b�V�����R�[�h 01:�V�[�h�v�� / 02:Key���M / 03�`�:�V�[�h�v�� / 04�`����:Key���M
		}	BIT;
	}	SUB;
	unsigned char	DATA[UDS_BUFFER_MAX-2];
}	UDS_SEC_ACC;

//	�R�~���j�P�[�V�����R���g���[���T�[�r�X
typedef	struct	__uds_28_communication_control__
{
	unsigned char	SID;	//	0x28
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char		:	2;	//	���g�p
			unsigned char	FC	:	6;	//	�Z�b�V�����R�[�h 00:����M���� / 01:��M���� / 02:���M���� / 03:����M�֎~
		}	BIT;
	}	SUB;
	unsigned char	CTYPE;	//	�R�~���j�P�[�V�����^�C�v 1:�A�v���P�[�V���� / 2:�l�b�g���[�N�}�l�[�W�����g
	unsigned char	DATA[UDS_BUFFER_MAX-3];
}	UDS_COM_CNT;

//	�e�X�^�[�v���[���g�T�[�r�X
typedef	struct	__uds_3e_tester_present__
{
	unsigned char	SID;	//	0x3E
	unsigned char	ZERO;	//	�[���T�u�t�@���N�V���� 0:�Œ�
	unsigned char	DATA[UDS_BUFFER_MAX-2];
}	UDS_TES_PRE;

//----------------------------------------------------------------------------------------
//	Data Transmission
//----------------------------------------------------------------------------------------
//	���ʎ��ʎq�f�[�^�ǂݎ��T�[�r�X
typedef	struct	__uds_22_read_data_ident__
{
	unsigned char	SID;	//	0x22
	unsigned char	RCIDHB;	//	���R�[�hCID��ʃo�C�g
	unsigned char	RCIDLB;	//	���R�[�hCID���ʃo�C�g
	unsigned char	TXM;	//	���M���[�h 01,02,03,04,05=single,slow,medium,fast,stop
	unsigned char	MNORTS;	//	�����d���ő咷
	unsigned char	DATA[UDS_BUFFER_MAX];
}	UDS_RDB_IDE;
//	�w��A�h���X�ǂݎ��T�[�r�X
typedef	struct	__uds_23_read_memory_addr__
{
	unsigned char	SID;	//	0x23
	unsigned char	MEMAHB;		//	�������A�h���X���
	unsigned char	MEMAMB;		//	�������A�h���X����
	unsigned char	MEMALB;		//	�������A�h���X����
	unsigned char	MEMSIZE;	//	�������T�C�Y
	unsigned char	TXM;	//	���M���[�h 01,02,03,04,05=single,slow,medium,fast,stop
	unsigned char	MNORTS;	//	�����d���ő咷
	unsigned char	DATA[UDS_BUFFER_MAX];
}	UDS_RDM_ADR;
//	���ʎ��ʎq�f�[�^�������݃T�[�r�X
typedef	struct	__uds_2e_write_data_ident__
{
	unsigned char	SID;	//	0x2E
	unsigned char	RECCIDHB;	//	���R�[�hCID��ʃo�C�g
	unsigned char	RECCIDLB;	//	���R�[�hCID���ʃo�C�g
	unsigned char	DATA[UDS_BUFFER_MAX];	//	���R�[�h�f�[�^
}	UDS_WDB_IDE;
//	�w��A�h���X�������݃T�[�r�X
typedef	struct	__uds_3d_write_memory_addr__
{
	unsigned char	SID;	//	0x3D
	unsigned char	MEMAHB;		//	�������A�h���X���
	unsigned char	MEMAMB;		//	�������A�h���X����
	unsigned char	MEMALB;		//	�������A�h���X����
	unsigned char	MEMSIZE;	//	�������T�C�Y
	unsigned char	DATA[UDS_BUFFER_MAX];	//	�������ރf�[�^
}	UDS_WRM_ADR;

//----------------------------------------------------------------------------------------
//	Upload / Download
//----------------------------------------------------------------------------------------
//	�_�E�����[�h�v���i�����̓��[�J�ŗL�j
typedef	struct	__uds_34_request_download__
{
	unsigned char	SID;	//	0x34
	unsigned char	CMP;	//	0x00	���k�̕����w��	00������
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char	ADDR	:	4;	//	�A�h���X�o�C�g��
			unsigned char	SIZE	:	4;	//	�T�C�Y�o�C�g��
		}	BIT;
	}	CONFIG;
	unsigned char	DATA[8];	//	�A�h���X���A�T�C�Y���
}	UDS_REQ_DL;
//	�_�E�����[�h�����i�����̓��[�J�ŗL�j
typedef	struct	__uds_74_response_download__
{
	unsigned char	SID;	//	0x74
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char	SIZE	:	4;	//	2 �T�C�Y�o�C�g��
			unsigned char			:	4;	//	0
		}	BIT;
	}	CONFIG;
	unsigned char	TXBHB;		//	�]���u���b�N�T�C�Y�����(0x00�`0x0F)
	unsigned char	TXBLB;		//	�]���u���b�N�T�C�Y��񉺈�(0x00�`0xFF)
	unsigned char	DATA[8];	//
}	UDS_RES_DL;
//	�A�b�v���[�h�v���i�����̓��[�J�ŗL�j
typedef	struct	__uds_35_request_upload__
{
	unsigned char	SID;	//	0x35
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char	ADDR	:	4;
			unsigned char	SIZE	:	4;
		}	BIT;
	}	CONFIG;
	unsigned char	DATA[8];
}	UDS_REQ_UL;
//	�A�b�v���[�h�����i�����̓��[�J�ŗL�j
typedef	struct	__uds_75_response_download__
{
	unsigned char	SID;	//	0x75
	union	{
		unsigned char	BYTE;
		struct	{
			unsigned char	SIZE	:	4;	//	2 �T�C�Y�o�C�g��
			unsigned char			:	4;	//	0
		}	BIT;
	}	CONFIG;
	unsigned char	TXBHB;		//	�]���u���b�N�T�C�Y�����(0x00�`0x0F)
	unsigned char	TXBLB;		//	�]���u���b�N�T�C�Y��񉺈�(0x00�`0xFF)
	unsigned char	DATA[8];	//
}	UDS_RES_DL;
//	�f�[�^�]��
typedef	struct	__uds_36_transfer_data__
{
	unsigned char	SID;	//	0x36
	unsigned char	DATA[UDS_BUFFER_MAX];
}	UDS_TR_DATA;
//	�]�����f
typedef	struct	__uds_37_request_transfer_exit__
{
	unsigned char	SID;	//	0x37
	unsigned char	DATA[2];
}	UDS_REQ_EXT;

//----------------------------------------------------------------------------------------
//	�R�[�h��`
//----------------------------------------------------------------------------------------
#define		UDS_ERR_SID				0x7F		/*	�G���[�T�[�r�XID								*/
#define		UDS_RES_SID				0x40		/*	���탌�X�|���X���ZID							*/

//----------------------------------------------------------------------------------------
//	�G���[���X�|���X�R�[�h(SID=0x7F�ɓY�t)
//----------------------------------------------------------------------------------------
#define		UDS_EC_NONE				0x00		/*	�G���[����														*/
#define		UDS_EC_GR				0x10		/*	��ʋ���														*/
#define		UDS_EC_SNS				0x11		/*	�T�|�[�g����Ă��Ȃ��T�[�r�X									*/
#define		UDS_EC_SFNS				0x12		/*	�T�u�@�\�̓T�|�[�g����Ă��܂���								*/
#define		UDS_EC_IML_IF			0x13		/*	�s�K�؂ȃ��b�Z�[�W�̒����܂��͖����Ȍ`��						*/
#define		UDS_EC_RTL				0x14		/*	�������������܂�												*/
#define		UDS_EC_BRR				0x21		/*	�r�W�[���s�[�g���N�G�X�g										*/
#define		UDS_EC_CNC				0x22		/*	�������������Ȃ�												*/
#define		UDS_EC_RSE				0x24		/*	���N�G�X�g�V�[�P���X�G���[										*/
#define		UDS_EC_NRFSNC			0x25		/*	�T�u�l�b�g�R���|�[�l���g����̉���������܂���					*/
#define		UDS_EC_FPERA			0x26		/*	���s�́A�v�����ꂽ�A�N�V�����̎��s��h���܂�					*/
#define		UDS_EC_ROOR				0x31		/*	�͈͊O�̃��N�G�X�g												*/
#define		UDS_EC_SAD				0x33		/*	�Z�L�����e�B�A�N�Z�X�����ۂ���܂���							*/
#define		UDS_EC_IK				0x35		/*	�����L�[														*/
#define		UDS_EC_ENOA				0x36		/*	���s�񐔒���													*/
#define		UDS_EC_RTDNE			0x37		/*	���v���Ԃ������؂�ɂȂ��Ă��Ȃ�								*/
#define		UDS_EC_UDNA				0x70		/*	�A�b�v���[�h/�_�E�����[�h�ł��܂���								*/
#define		UDS_EC_TDS				0x71		/*	�]���f�[�^�̈ꎞ��~											*/
#define		UDS_EC_GPF				0x72		/*	��ʓI�ȃv���O���~���O�̎��s									*/
#define		UDS_EC_WBSC				0x73		/*	�Ԉ�����u���b�N�V�[�P���X�J�E���^								*/
#define		UDS_EC_RCR				0x78		/*	�v���͐�������M����܂������A�����͕ۗ����ł�					*/
#define		UDS_EC_SFNSAS			0x7E		/*	�T�u�Z�b�V�������A�N�e�B�u�Z�b�V�����ŃT�|�[�g����Ă��܂���	*/
#define		UDS_EC_SNSAS			0x7F		/*	�A�N�e�B�u�ȃZ�b�V�����ł̓T�[�r�X���T�|�[�g����Ă��܂���		*/

//	�_�E�����[�h�E�A�b�v���[�h�Ǘ��\����
#define			UDS_TD_NONE			0			/*	�A�b�v���[�h�A�_�E�����[�h�v���҂�					*/
#define			UDS_TD_DOWNLOAD		1			/*	�_�E�����[�h��										*/
#define			UDS_TD_UPLOAD		2			/*	�A�b�v���[�h��										*/
typedef	struct	__uds_load_control_str__
{
		int				MODE;		//	�]�����[�h
		unsigned long	ADDR;		//	�A�h���X
		int				SIZE;		//	�T�C�Y
		int				BLKL;		//	�u���b�N�T�C�Y
		int				CNT;		//	�]���J�E���^
}	UDS_LOAD_STR;

#endif
//----------------------------------------------------------------------------------------
//	�ϐ���`
//----------------------------------------------------------------------------------------
int				uds_diag_session = 0;			//	�Z�b�V�����R���g���[��
int				uds_p2_can_server_max = 0;		//	P2 Time
int				uds_p2e_can_server_max = 0;		//	P2E Time

int				uds_reset_request = 0;			//	���Z�b�g�v��

int				uds_security_access = 0;		//	�Z�L�����e�B�[�A�N�Z�X

UDS_LOAD_STR	uds_load;						//	�_�E�����[�h�E�A�b�v���[�h�Ǘ�

/*
	���v���K��
	
	ROM�̈�ݒ�
	E2DataFlash
	0x00100000�`0x00107FFF	 8K*4	Parameter		<--- 0x00100000�`0x00107FFF	�_�E�����[�h���� / Erase size 0x00002000
	Program Block
	0xFFE00000�`0xFFEFFFFF	64K*16	Data Area		<--- 0xFFE00000�`0xFFEFFFFF	�_�E�����[�h���� / Erase size 0x00010000
	0xFFF00000�`0xFFF3FFFF	32K*8	Download F/W	<--- 0xFFF00000�`0xFFF3FFFF	�_�E�����[�h���� / Erase size 0x00008000
	0xFFF40000�`0xFFF7FFFF	32K*8	Default F/W		<--- 0xFFF40000�`0xFFF7FFFF	�_�E�����[�h�֎~ / Erase size 0x00008000
	0xFFF80000�`0xFFFF7FFF	16K*29	Bootloader F/W	<--- 0xFFF80000�`0xFFFEFFFF	�_�E�����[�h�֎~ / Erase size 0x00004000
	0xFFFF8000�`0xFFFFFFFF	 4K*8	Configuration	<--- 0xFFFF0000�`0xFFFFFFFF	�_�E�����[�h�֎~ / Erase size 0x00001000
*/

//----------------------------------------------------------------------------------------
//	CAN-UDS �ϐ�������
//----------------------------------------------------------------------------------------
void	can_uds_init(void)
{
	memset(&uds_load, 0, sizeof(UDS_LOAD_STR));
}

//----------------------------------------------------------------------------------------
//	UDS �p�����ԃ^�C���A�b�v
//----------------------------------------------------------------------------------------
void uds_timeup(void)
{
	uds_diag_session = 0;			//	�Z�b�V�����R���g���[��
	uds_security_access = 0;		//	�Z�L�����e�B�[�A�N�Z�X
	can_uds_init();
}

//----------------------------------------------------------------------------------------
//	UDS 0x10	Diagnostic Session Control
//----------------------------------------------------------------------------------------
int uds_sid_10(unsigned char *req, int sz, unsigned char *res, int *len)
{
	//	�Z�b�V�����R�[�h 01:�f�t�H���g / 02:�v���O���~���O / 03:�g���_�C�A�O
	switch(req[1] & 0x3F)
	{
	case 1:	//	�f�t�H���g�Z�b�V����
		uds_diag_session = 1;
		break;
	case 2:	//	ECU�v���O���~���O�Z�b�V����
		uds_diag_session = 2;
		break;
	case 3:	//	ECU�g���f�f�Z�b�V����
		uds_diag_session = 3;
		break;
	default:
		return UDS_EC_SFNS;
	}
	res[0] = req[0] | UDS_RES_SID;
	res[1] = req[1];
//	res[2] = (unsigned char)(uds_p2_can_server_max >> 8);
//	res[3] = (unsigned char)(uds_p2_can_server_max & 0xFF);
//	res[4] = (unsigned char)(uds_p2e_can_server_max >> 8);
//	res[5] = (unsigned char)(uds_p2e_can_server_max & 0xFF);
	*len = 2;
	return UDS_EC_NONE;
}

//----------------------------------------------------------------------------------------
//	UDS 0x11	ECU Reset
//----------------------------------------------------------------------------------------
int uds_sid_11(unsigned char *req, int sz, unsigned char *res, int *len)
{
	if(uds_diag_session < 2)
	{	//	�Z�b�V�����Ⴂ
		return UDS_EC_GR;	//	��ʋ���
	}
	//	�Z�b�V�����R�[�h 01:�n�[�h / 02:KeyOnOff / 03:�\�t�g / 04:�����V���b�g�_�E������ / 05:�����V���b�g�_�E���֎~
	switch(req[1] & 0x3F)
	{
	case 1:	//	�n�[�h���Z�b�g
		uds_reset_request = 1;	//	�n�[�h���Z�b�g
		break;
	case 2:	//	Key-On/Off
		uds_reset_request = 2;	//	�L�[���Z�b�g
		break;
	case 3:	//	�\�t�g���Z�b�g
		uds_reset_request = 3;	//	�\�t�g���Z�b�g
		break;
	default:
		return UDS_EC_SFNS;
	}
	res[0] = req[0] | UDS_RES_SID;
	res[1] = req[1];
	*len = 2;
	return UDS_EC_NONE;
}

//----------------------------------------------------------------------------------------
//	UDS 0x27	Security Access
//----------------------------------------------------------------------------------------
int uds_sid_27(unsigned char *req, int sz, unsigned char *res, int *len)
{
	if(uds_diag_session < 2)
	{	//	�Z�b�V�����Ⴂ
		return UDS_EC_GR;	//	��ʋ���
	}
	//	�Z�b�V�����R�[�h 01:�V�[�h�v�� / 02:Key���M / 03�`�:�V�[�h�v�� / 04�`����:Key���M
	switch(req[1] & 0x3F)
	{
	case 1:	//	�V�[�h�v��
		if(uds_security_access == 0)
		{
			res[2] = 0x12;
			res[3] = 0x34;
		}
		else
		{
			res[2] = 0x00;
			res[3] = 0x00;
		}
		*len = 4;
		break;
	case 2:	//	�L�[�ݒ�
		if(req[2] == 0x17 && req[3] == 0xC0 && uds_security_access == 0)
		{	//	0x17C0 ����
			uds_security_access = 1;	//	Unlocked
			*len = 2;
		}
		else
		{	//	�L�[�s��v
			uds_security_access = 0;	//	Locked
			return UDS_EC_IK;
		}
		break;
	case 3:	//	�V�[�h�v��
		if(uds_security_access == 1)
		{
			res[2] = 0x34;
			res[3] = 0x56;
			*len = 4;
		}
		else
		{
			return UDS_EC_SAD;
		}
		break;
	case 4:	//	�L�[�ݒ�
		if(req[2] == 0x17 && req[3] == 0xC1 && uds_security_access == 1)
		{	//	0x17C1 ����
			uds_security_access = 2;	//	Unlocked
			*len = 2;
		}
		else
		{	//	�L�[�s��v
			return UDS_EC_IK;
		}
		break;
	case 5:	//	�V�[�h�v��
		if(uds_security_access == 2)
		{
			res[2] = 0x56;
			res[3] = 0x78;
			*len = 4;
		}
		else
		{
			return UDS_EC_SAD;
		}
		break;
	case 6:	//	�L�[�ݒ�
		if(req[2] == 0x17 && req[3] == 0xC2 && uds_security_access == 2)
		{	//	0x17C2 ����
			uds_security_access = 3;	//	Unlocked
			*len = 2;
		}
		else
		{	//	�L�[�s��v
			return UDS_EC_IK;
		}
		break;
	default:
		uds_security_access = 0;	//	Locked
		return UDS_EC_SFNS;
	}
	res[0] = req[0] | UDS_RES_SID;
	res[1] = req[1];
	return UDS_EC_NONE;
}

//----------------------------------------------------------------------------------------
//	UDS 0x3E	Tester Present
//----------------------------------------------------------------------------------------
int uds_sid_3e(unsigned char *req, int sz, unsigned char *res, int *len)
{
	//	�e�X�^�[�ڑ����̈ێ�
	res[0] = req[0] | UDS_RES_SID;
	*len = 1;
	return UDS_EC_NONE;
}

//----------------------------------------------------------------------------------------
//	UDS 0x22	Read Data By Identifier
//----------------------------------------------------------------------------------------
extern	const char	def_ecu_corp[];	//	16] = "TOYOTA";
extern	const char	def_ecu_name[];	//	16] = "CAN2ECU";
extern	const char	def_ecu_vars[];	//	16] = "Ver1.3.0";
extern	const char	def_ecu_date[];	//	16] = __DATE__;
extern	const char	def_ecu_time[];	//	16] = __TIME__;
int uds_sid_22(unsigned char *req, int sz, unsigned char *res, int *len)
{
	int				i, k, b;
	unsigned long	d;

	res[0] = req[0] | UDS_RES_SID;
	res[1] = req[1];	//	CID
	res[2] = req[2];	//	CID
	i = 3;
	switch(req[1])
	{
	default:
		return UDS_EC_SFNS;
	case 0xF1:	//	ECU���
		switch(req[2])
		{
		default:
			return UDS_EC_SNS;
		case 0x00:	//	���[�J��
			memcpy(&res[3], &def_ecu_corp[0], 16);
			i += 16;
			break;
		case 0x01:	//	�ԗ��R�[�h
			memcpy(&res[3], &def_ecu_name[0], 16);
			i += 16;
			break;
		case 0x02:	//	ECU�o�[�W����
			memcpy(&res[3], &def_ecu_vars[0], 16);
			i += 16;
			break;
		case 0x03:	//	F/W���t
			memcpy(&res[3], &def_ecu_date[0], 16);
			i += 16;
			break;
		case 0x04:	//	F/W����
			memcpy(&res[3], &def_ecu_time[0], 16);
			i += 16;
			break;
		}
		break;
	case 0xF2:	//	�������}�b�v���
		switch(req[2])
		{
		default:
			return UDS_EC_SNS;
		case 0x00:	//	���[�e�B���O�}�b�v�A�h���X
			d = (unsigned long)&rout_map;
			b = 1;
			break;
		case 0x01:	//	�����E�C�x���g�E�����[�g�Ǘ���`�ϐ�
			d = (unsigned long)&conf_ecu;
			b = sizeof(ECU_CYC_EVE);
			break;
		case 0x02:	//	ECU���o�̓`�F�b�N���X�g
			d = (unsigned long)&ext_list;
			b = sizeof(EXTERNUL_IO);
			break;
		case 0x03:	//	CAN-ID -> EX-I/O-ID �ϊ��e�[�u��
			d = (unsigned long)&can_to_exio;
			b = 1;
			break;
		}
		res[3] = (d >> 16);		//	�A�h���XH
		res[4] = (d >> 8);		//	�A�h���XM
		res[5] = (d & 0xFF);	//	�A�h���XL
		res[6] = (b & 0xFF);	//	�A�N�Z�X�T�C�Y(byte)
		i += 4;
		break;
	case 0xF3:	//	�p�����[�^�A�N�Z�X
		k = ((int)req[3] << 8) | ((int)req[4] & 0xFF);
		res[3] = req[3];
		res[4] = req[4];
		i += 2;
		switch(req[2])
		{
		default:
			return UDS_EC_SNS;
		case 0x00:	//	���[�e�B���O�}�b�v
			if(k >= CAN_ID_MAX) return UDS_EC_ROOR;
			res[5] = rout_map.ID[k].BYTE;
			i++;
			break;
		case 0x01:	//	�����E�C�x���g�E�����[�g�Ǘ���`�ϐ�
			if(k >= MESSAGE_MAX) return UDS_EC_ROOR;
			memcpy(&res[5], &conf_ecu.LIST[k], sizeof(ECU_CYC_EVE));
			i += sizeof(ECU_CYC_EVE);
			break;
		case 0x02:	//	ECU���o�̓`�F�b�N���X�g
			if(k >= ECU_EXT_MAX) return UDS_EC_ROOR;
			memcpy(&res[5], &ext_list[k], sizeof(EXTERNUL_IO));
			i += sizeof(EXTERNUL_IO);
			break;
		case 0x03:	//	CAN-ID -> EX-I/O-ID �ϊ��e�[�u��
			if(k >= CAN_ID_MAX) return UDS_EC_ROOR;
			res[5] = can_to_exio[k];
			i++;
			break;
		}
		break;
	case 0xF5:	//	�f�[�^�t���b�V������
		switch(req[2])
		{
		default:
			return UDS_EC_SNS;
		case 0x00:	//	�������ݏ�Ԃ̊m�F
			k = ecu_data_check();
			res[3] = (unsigned char)(k >> 8);
			res[4] = (unsigned char)(k & 0xFF);
			i += 2;
			break;
		}
		break;
	}
	*len = i;
	return UDS_EC_NONE;
}

//----------------------------------------------------------------------------------------
//	UDS 0x23	Read Memory By Address
//----------------------------------------------------------------------------------------
int uds_sid_23(unsigned char *req, int sz, unsigned char *res, int *len)
{
	int	z;
	unsigned char	*p;

	p = (unsigned char *)(((int)req[1] << 16) | ((int)req[2] << 8) | ((int)req[3] & 0xFF));
	z = (int)req[4];
	if(z > 64)
	{	//	�ꊇ�ǂݏo���I�[�o�[
		return UDS_EC_IML_IF;
	}
	res[0] = req[0] | UDS_RES_SID;
	memcpy(&res[1], p, z);
	*len = z + 1;
	return UDS_EC_NONE;
}

//----------------------------------------------------------------------------------------
//	UDS 0x2E	Write Data By Identifier
//----------------------------------------------------------------------------------------
int uds_sid_2e(unsigned char *req, int sz, unsigned char *res, int *len)
{
	int				i, k;
	unsigned long	d;

	res[0] = req[0] | UDS_RES_SID;
	res[1] = req[1];	//	CID
	res[2] = req[2];	//	CID
	i = 3;
	switch(req[1])
	{
	default:
		return UDS_EC_SFNS;
/*	case 0xF1:	//	ECU���
		switch(req[2])
		{
		default:
			return UDS_EC_SNS;
		case 0x00:	//	���[�J��
			memcpy(&res[3], def_ecu_corp, 16);
			i += 16;
			break;
		case 0x01:	//	�ԗ��R�[�h
			memcpy(&res[3], def_ecu_name, 16);
			i += 16;
			break;
		case 0x02:	//	ECU�o�[�W����
			memcpy(&res[3], def_ecu_vars, 16);
			i += 16;
			break;
		case 0x03:	//	F/W���t
			memcpy(&res[3], def_ecu_date, 16);
			i += 16;
			break;
		case 0x04:	//	F/W����
			memcpy(&res[3], def_ecu_time, 16);
			i += 16;
			break;
		}
	case 0xF2:	//	�������}�b�v���
		switch(req[2])
		{
		default:
			return UDS_EC_SNS;
		case 0x00:	//	���[�e�B���O�}�b�v�A�h���X
			d = (unsigned long)&rout_map;
			break;
		case 0x01:	//	�����E�C�x���g�E�����[�g�Ǘ���`�ϐ�
			d = (unsigned long)&conf_ecu;
			break;
		case 0x02:	//	ECU���o�̓`�F�b�N���X�g
			d = (unsigned long)&ext_list;
			break;
		case 0x03:	//	CAN-ID -> EX-I/O-ID �ϊ��e�[�u��
			d = (unsigned long)&can_to_exio;
			break;
		}
		res[3] = (d >> 24);
		res[4] = (d >> 16);
		res[5] = (d >> 8);
		res[6] = (d & 0xFF);
		i += 4;
		break;*/
	case 0xF3:	//	�p�����[�^�A�N�Z�X
		if(uds_diag_session < 2)
		{	//	�Z�b�V�����Ⴂ
			return UDS_EC_GR;	//	��ʋ���
		}
		if(uds_security_access < 1)
		{	//	������Ă��Ȃ�
			return UDS_EC_SAD;	//	�Z�L�����e�B�[����
		}
		k = ((int)req[3] << 8) | ((int)req[4] & 0xFF);
		res[3] = req[3];
		res[4] = req[4];
		i += 2;
		switch(req[2])
		{
		default:
			return UDS_EC_SNS;
		case 0x00:	//	���[�e�B���O�}�b�v
			if(k >= CAN_ID_MAX) return UDS_EC_ROOR;
			rout_map.ID[k].BYTE = req[5];
			res[5] = rout_map.ID[k].BYTE;
			i++;
			break;
		case 0x01:	//	�����E�C�x���g�E�����[�g�Ǘ���`�ϐ�
			if(k >= MESSAGE_MAX) return UDS_EC_ROOR;
			memcpy(&conf_ecu.LIST[k], &req[5], sizeof(ECU_CYC_EVE));
			memcpy(&res[5], &conf_ecu.LIST[k], sizeof(ECU_CYC_EVE));
			i += sizeof(ECU_CYC_EVE);
			break;
		case 0x02:	//	ECU���o�̓`�F�b�N���X�g
			if(k >= ECU_EXT_MAX) return UDS_EC_ROOR;
			memcpy(&ext_list[k], &req[5], sizeof(EXTERNUL_IO));
			memcpy(&res[5], &ext_list[k], sizeof(EXTERNUL_IO));
			i += sizeof(EXTERNUL_IO);
			break;
		case 0x03:	//	CAN-ID -> EX-I/O-ID �ϊ��e�[�u��
			if(k >= CAN_ID_MAX) return UDS_EC_ROOR;
			can_to_exio[k] = req[5];
			res[5] = can_to_exio[k];
			i++;
			break;
		}
		break;
	case 0xF5:	//	�f�[�^�t���b�V������
		switch(req[2])
		{
		default:
			return UDS_EC_SNS;
		case 0x01:	//	�ۑ�
			k = ecu_data_write();
			res[3] = (unsigned char)(k >> 8);
			res[4] = (unsigned char)(k & 0xFF);
			i += 2;
			break;
		case 0x02:	//	����
			k = ecu_data_erase();
			res[3] = (unsigned char)(k >> 8);
			res[4] = (unsigned char)(k & 0xFF);
			i += 2;
			break;
		}
		break;
	}
	*len = i;
	return UDS_EC_NONE;
}

//----------------------------------------------------------------------------------------
//	UDS 0x3D	Write Memory By Address
//----------------------------------------------------------------------------------------
int uds_sid_3d(unsigned char *req, int sz, unsigned char *res, int *len)
{
	int	i;
	int	z;
	unsigned char	*p, *r;

	if(uds_diag_session < 2)
	{	//	�Z�b�V�����Ⴂ
		return UDS_EC_GR;	//	��ʋ���
	}
	if(uds_security_access < 1)
	{	//	������Ă��Ȃ�
		return UDS_EC_SAD;	//	�Z�L�����e�B�[����
	}
	p = (unsigned char *)(((int)req[1] << 16) | ((int)req[2] << 8) | ((int)req[3] & 0xFF));
	z = (int)req[4];
	if(z > 64)
	{	//	�ꊇ�������݃I�[�o�[
		return UDS_EC_IML_IF;
	}
	memcpy(p, &req[5], z);
	memcpy(res, req, 5);
	res[0] |= UDS_RES_SID;
	*len = 5;
	return UDS_EC_NONE;
}

//----------------------------------------------------------------------------------------
//	UDS 0x34	Request Download
//----------------------------------------------------------------------------------------
int uds_sid_34(unsigned char *req, int sz, unsigned char *res, int *len)
{
	int	i, j, sb, eb, bs;
	int	siz;
	unsigned long	adr;

	if(uds_diag_session < 2)
	{	//	�Z�b�V�����Ⴂ
		return UDS_EC_GR;	//	��ʋ���
	}
	if(uds_security_access == 0)
	{	//	���b�N���
		return UDS_EC_UDNA;
	}
	if(uds_load.MODE)
	{	//	���s���G���[
		return UDS_EC_BRR;
	}
	if(req[1] != 0x00)
	{	//	���k�̓T�|�[�g����
		return UDS_EC_SFNS;
	}
	switch(req[2])
	{	//	�t�H�[�}�b�g
	case 0x44:
		for(adr = 0, i = 3; i < 7; i++)
		{
			adr <<= 8;
			adr |= (unsigned long)req[i] & 0xFF;
		}
		for(siz = 0, i = 7; i < 11; i++)
		{
			siz <<= 8;
			siz |= (int)req[i] & 0xFF;
		}
		break;
	default:
		return UDS_EC_SFNS;
	}
	if(adr >= 0x00000000ul && adr <= 0x0003FFFF)
	{	//	RAM
		return UDS_EC_CNC;	//	�͈̓G���[
	//	if(siz > (0x40000 - adr) || siz < 0)
	//	{	//	�T�C�Y�G���[
	//		return UDS_EC_UDNA;
	//	}
	}
	else
	if(adr >= 0x00100000ul && adr <= 0x00107FFF)
	{	//	E2Data
		if(siz > (0x00108000 - adr) || siz < 0)
		{	//	�T�C�Y�G���[
			return UDS_EC_UDNA;
		}
		if((adr & 0x00001FFF) == 0)
		{	//	����
			i = BLOCK_DB0 + (adr >> 13);	//	�����u���b�N
			for(j = siz; j > 0; j -= 0x2000, i--)
			{	//	�������ރu���b�N���s���ď�������
				if(R_FlashDataAreaBlankCheck(g_flash_BlockAddresses[i], BLANK_CHECK_ENTIRE_BLOCK) == FLASH_NOT_BLANK)
				{	//	����
					while(R_FlashGetStatus() != FLASH_SUCCESS);
					if(R_FlashErase(i) != FLASH_SUCCESS)
					{
						return UDS_EC_GPF;
					}
					while(R_FlashGetStatus() != FLASH_SUCCESS);
					if(R_FlashDataAreaBlankCheck(g_flash_BlockAddresses[i], BLANK_CHECK_ENTIRE_BLOCK) != FLASH_BLANK)
					{	//	�������s
						return UDS_EC_GPF;
					}
				}
			}
		}
	}
	else
	if(adr >= 0xFFE00000ul)
	{	//	Program Flash ROM
		if(uds_security_access < 2 && (adr >= 0xFFF40000ul || (adr + (unsigned long)siz) >= 0xFFF40000ul)) return UDS_EC_CNC;	//	�͈̓G���[
		if(uds_security_access < 3 && (adr >= 0xFFF80000ul || (adr + (unsigned long)siz) >= 0xFFF80000ul)) return UDS_EC_CNC;	//	�͈̓G���[
		if(siz > (0x40000 - (adr & 0x0003FFFF)) || siz < 0)
		{	//	�T�C�Y�G���[
			return UDS_EC_UDNA;
		}
		if((adr & 0x0001FFFF) == 0)
		{	//	����
			if(adr < 0xFFF00000ul)
			{	//	64K�u���b�N(�f�[�^ROM�̈�)
				sb = BLOCK_69 - ((adr - 0xFFE00000ul) >> 16);
				eb = sb - ((siz + 0xFFFF) >> 16);
				bs = 0x10000;
				if(eb < BLOCK_53) return UDS_EC_CNC;	//	�͈̓G���[
			}
			else
			if(adr < 0xFFF80000ul)
			{	//	32K�u���b�N(�v���O����ROM�̈�)
				sb = BLOCK_53 - ((adr - 0xFFF00000ul) >> 15);
				eb = sb - ((siz + 0x7FFF) >> 15);
				bs = 0x8000;
				if(eb < BLOCK_37) return UDS_EC_CNC;	//	�͈̓G���[
			}
			else
			if(adr < 0xFFFF8000ul)
			{	//	16K�u���b�N(�u�[�g���[�_ROM�̈�)
				sb = BLOCK_37 - ((adr - 0xFFF80000ul) >> 14);
				eb = sb - ((siz + 0x3FFF) >> 14);
				bs = 0x4000;
				if(eb < BLOCK_7) return UDS_EC_CNC;	//	�͈̓G���[
			}
			else
			{	//	4K�u���b�N(�N��������ROM�̈�)
				sb = BLOCK_7 - ((adr - 0xFFFF8000ul) >> 12);
				eb = sb - ((siz + 0x0FFF) >> 12);
				bs = 0x1000;
				if(eb < -1) return UDS_EC_CNC;	//	�͈̓G���[
			}
			for(i = sb, j = siz; j > 0; j -= bs, i--)
			{	//	�������ރu���b�N���s���ď�������
				_di();
				if(R_FlashErase(i) != FLASH_SUCCESS)
				{
					_ei();
					return UDS_EC_GPF;
				}
				_ei();
			}
		}
	}
	else
	{	//	�v���O�����͈̓G���[
		return UDS_EC_CNC;
	}
	//	�_�E�����[�h���
	uds_load.ADDR = adr;
	uds_load.SIZE = siz;
	uds_load.MODE = UDS_TD_DOWNLOAD;
	uds_load.BLKL = 1 + 128;	//	SID + Data[128]
	uds_load.CNT = 0;
	//	�u���b�N�T�C�Y��ʒm
	res[0] = req[0] | UDS_RES_SID;
	res[1] = 0x20;	//	word block size record
	res[2] = (uds_load.BLKL >> 8);
	res[3] = (uds_load.BLKL & 0xFF);
	*len = 4;
	return UDS_EC_NONE;
}

//----------------------------------------------------------------------------------------
//	UDS 0x35	Request Upload
//----------------------------------------------------------------------------------------
int uds_sid_35(unsigned char *req, int sz, unsigned char *res, int *len)
{
	int	i, blk;
	int	siz;
	unsigned long	adr;

	if(uds_diag_session < 2)
	{	//	�Z�b�V�����Ⴂ
		return UDS_EC_GR;	//	��ʋ���
	}
	if(uds_security_access == 0)
	{	//	���b�N���
		return UDS_EC_UDNA;
	}
	if(uds_load.MODE)
	{	//	���s���G���[
		return UDS_EC_BRR;
	}
	if(req[1] != 0x00)
	{	//	���k�̓T�|�[�g����
		return UDS_EC_SFNS;
	}
	switch(req[2])
	{	//	�t�H�[�}�b�g
	case 0x44:
		for(adr = 0, i = 3; i < 7; i++)
		{
			adr <<= 8;
			adr |= (unsigned long)req[i] & 0xFF;
		}
		for(siz = 0, i = 7; i < 11; i++)
		{
			siz <<= 8;
			siz |= (int)req[i] & 0xFF;
		}
	//	adr = (unsigned long)(((unsigned long)req[3] << 24) | ((unsigned long)req[4] << 16) | ((unsigned long)req[5] << 8) | (unsigned long)req[6]);
	//	siz = (int)(((unsigned long)req[7] << 24) | ((unsigned long)req[8] << 16) | ((unsigned long)req[9] << 8) | (unsigned long)req[10]);
		if(sz > 12)
		{	//	�w��T�C�Y
			blk = (((int)req[11]) << 8) & 0xFF00;
			blk |= ((int)req[12]) & 0xFF;;
		}
		else
		{	//	F/W�T�C�Y
			blk = 129;
		}
		break;
	default:
		return UDS_EC_SFNS;
	}
	if(blk > 0xFFF && blk < 6)
	{	//	�u���b�N�T�C�Y�ُ�
		return UDS_EC_UDNA;
	}
	if(siz > 0x40000 || siz < 0)
	{	//	�T�C�Y�G���[
		return UDS_EC_UDNA;
	}
	//	�_�E�����[�h���
	uds_load.ADDR = adr;
	uds_load.SIZE = siz;
	uds_load.MODE = UDS_TD_UPLOAD;
	uds_load.BLKL = 1 + 128;	//	SID + Data[128]
	uds_load.CNT = 0;
	if(blk < uds_load.BLKL) uds_load.BLKL = blk;
	//	�u���b�N�T�C�Y��ʒm
	res[0] = req[0] | UDS_RES_SID;
	res[1] = 0x20;	//	word record
	res[2] = (uds_load.BLKL >> 8);
	res[3] = (uds_load.BLKL & 0xFF);
	*len = 4;
	return UDS_EC_NONE;
}

//----------------------------------------------------------------------------------------
//	UDS 0x36	Transfer Data
//----------------------------------------------------------------------------------------
int uds_sid_36(unsigned char *req, int sz, unsigned char *res, int *len)
{
	int	i;
	unsigned long	adr;
	unsigned char	*p;

	if(uds_diag_session < 2)
	{	//	�Z�b�V�����Ⴂ
		return UDS_EC_GR;	//	��ʋ���
	}
	if(uds_security_access == 0)
	{	//	���b�N���
		return UDS_EC_UDNA;
	}
	if(uds_load.MODE == UDS_TD_DOWNLOAD)
	{	//	�_�E�����[�h���i�c�[����ECU�j
		if(sz > uds_load.BLKL)
		{	//	�T�C�Y�I�[�o�[
			uds_load.MODE = UDS_TD_NONE;	//	�]�����~
			return UDS_EC_IML_IF;
		}
		sz--;	//	�R�}���h�R�[�h�o�C�g�����炷
		if((uds_load.CNT + sz) > uds_load.SIZE) sz = uds_load.SIZE - uds_load.CNT;
		if((uds_load.ADDR & 0xFFFC0000) == 0)
		{	//	RAM�֏�������
			p = (unsigned char *)uds_load.ADDR;
			uds_load.ADDR += sz;
			uds_load.CNT += sz;
			memcpy(p, &req[1], sz);
		}
		else
		if((uds_load.ADDR & 0xFFFF8000ul) == 0x00100000ul)
		{	//	E2Data
			i = sz;
			if((i & (DF_PROGRAM_SIZE_SMALL - 1)) != 0)
			{	//	�ŏ��������݃o�C�g�ɓ��B���Ă��Ȃ�����00�Ŗ��߂�
				for(;(i & (DF_PROGRAM_SIZE_SMALL - 1)) != 0; i++) req[1 + i] = 0;
			}
			if(R_FlashWrite(uds_load.ADDR, (int)&req[1], i) != FLASH_SUCCESS)
			{	//	�������ݎ��s
				uds_load.MODE = UDS_TD_NONE;
				return UDS_EC_GPF;
			}
			uds_load.ADDR += sz;
			uds_load.CNT += sz;
		}
		else
		if(uds_load.ADDR >= 0xFFE00000ul)
		{	//	Program Flash ROM
			i = sz;
			if(i < ROM_PROGRAM_SIZE)
			{	//	�v���O�����P�ʂ̕s������FF�Ŗ��߂�
				for(;i < ROM_PROGRAM_SIZE; i++) req[1 + i] = 0xFF;
			}
			_di();
			if(R_FlashWrite(uds_load.ADDR, (int)&req[1], i) != FLASH_SUCCESS)
			{	//	�������ݎ��s
				_ei();
				uds_load.MODE = UDS_TD_NONE;
				return UDS_EC_GPF;
			}
			_ei();
			uds_load.ADDR += sz;
			uds_load.CNT += sz;
		}
		if(uds_load.CNT >= uds_load.SIZE)
		{	//	�_�E�����[�h����
			uds_load.MODE = UDS_TD_NONE;
		}
		i = (int)(uds_load.SIZE - uds_load.CNT);
		//	���퉞��(�c��̃o�C�g����ʒm)
		res[0] = req[0] | UDS_RES_SID;
		res[1] = 0x04;	//	long counter record
		res[2] = (i >> 24);
		res[3] = (i >> 16);
		res[4] = (i >> 8);
		res[5] = (i & 0xFF);
		*len = 6;
		return UDS_EC_NONE;
	}
	else
	if(uds_load.MODE == UDS_TD_UPLOAD)
	{	//	�A�b�v���[�h���iECU���c�[���j
		i = uds_load.BLKL;	//	�T�C�Y����
	//	if((uds_load.CNT + sz) > uds_load.SIZE) sz = uds_load.SIZE - uds_load.CNT;
		//	���퉞��(�c��̃o�C�g����ʒm)
		res[0] = req[0] | UDS_RES_SID;
		p = (unsigned char *)uds_load.ADDR;
		sz = (int)(uds_load.SIZE - uds_load.CNT) + 1;
		if(sz > uds_load.BLKL) sz = uds_load.BLKL;
	//	uds_load.ADDR += sz;
	//	uds_load.CNT += sz;
		for(i = 1; i < sz; i++)
		{
			res[i] = *p++;
			uds_load.ADDR++;
			uds_load.CNT++;
		}
		*len = i;
		return UDS_EC_NONE;
	}
	return UDS_EC_RSE;	//	�葱���G���[
}

//----------------------------------------------------------------------------------------
//	UDS 0x37	Request Transfer Exit
//----------------------------------------------------------------------------------------
int uds_sid_37(unsigned char *req, int sz, unsigned char *res, int *len)
{
	int	i;
	if(uds_diag_session < 2)
	{	//	�Z�b�V�����Ⴂ
		return UDS_EC_GR;	//	��ʋ���
	}
	if(uds_security_access == 0)
	{	//	���b�N���
		return UDS_EC_UDNA;
	}
	if(uds_load.MODE != UDS_TD_NONE)
	{
		if(uds_load.MODE == UDS_TD_DOWNLOAD)
		{	//	�_�E�����[�h���i�c�[����ECU�j
			if(uds_load.ADDR >= 0xFFF00000ul && uds_load.ADDR < 0xFFF20000ul)
			{	//	���[�U�[�t�@�[���E�F�A�̈�𒆒f����ꍇ��ROM��������
				R_FlashErase(BLOCK_53);
			}
		}
		uds_load.MODE = UDS_TD_NONE;
		i = (int)(uds_load.SIZE - uds_load.CNT);
		//	���f���̎c��̃o�C�g����ʒm
		res[0] = req[0] | UDS_RES_SID;
		res[1] = 0x04;	//	long counter record
		res[2] = (i >> 24);
		res[3] = (i >> 16);
		res[4] = (i >> 8);
		res[5] = (i & 0xFF);
		*len = 6;
	}
	else
	{	//	�I��
		res[0] = req[0] | UDS_RES_SID;
		res[1] = 0x00;
		*len = 2;
	}
	return UDS_EC_NONE;
}

//----------------------------------------------------------------------------------------
//	UDS����
//----------------------------------------------------------------------------------------
int uds_job(unsigned char *msg, int len, unsigned char *res)	//int ch, int id, void *frame)
{
	int	size = 0;
	int	ercd = UDS_EC_NONE;
	//	�T�[�r�X���s
	switch(msg[0])
	{
	default:	//	�T�[�r�X���T�|�[�g
		res[0] = UDS_ERR_SID;
		res[1] = msg[0];
		res[2] = UDS_EC_SNS;
		return 3;
	case 0x10:	//	Diagnostic Session Control
		ercd = uds_sid_10(msg, len, res, &size);
		break;
	case 0x11:	//	ECU Reset
		ercd = uds_sid_11(msg, len, res, &size);
		break;
	case 0x27:	//	Security Access
		ercd = uds_sid_27(msg, len, res, &size);
		break;
	case 0x3E:	//	Tester Present
		ercd = uds_sid_3e(msg, len, res, &size);
		break;
	case 0x22:	//	Read Data By Identifier
		ercd = uds_sid_22(msg, len, res, &size);
		break;
	case 0x23:	//	Read Memory By Address
		ercd = uds_sid_23(msg, len, res, &size);
		break;
	case 0x2E:	//	Write Data By Identifier
		ercd = uds_sid_2e(msg, len, res, &size);
		break;
	case 0x3D:	//	Write Memory By Address
		ercd = uds_sid_3d(msg, len, res, &size);
		break;
	case 0x34:	//	Request Download
		ercd = uds_sid_34(msg, len, res, &size);
		break;
	case 0x35:	//	Request Upload
		ercd = uds_sid_35(msg, len, res, &size);
		break;
	case 0x36:	//	Transfer Data
		ercd = uds_sid_36(msg, len, res, &size);
		break;
	case 0x37:	//	Request Transfer Exit
		ercd = uds_sid_37(msg, len, res, &size);
		break;
	}
	if(ercd != UDS_EC_NONE)
	{	//	�G���[�L��
		res[0] = UDS_ERR_SID;
		res[1] = msg[0];
		res[2] = ercd;
		return 3;
	}
	after_call(DTC_TIMER_ID, 10000, uds_timeup);	//	�ڑ��ێ�10�b�^�C�}�[
	return size;
}

