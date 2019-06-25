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
//	OBD2-CAN �v���g�R������
//
//----------------------------------------------------------------------------------------
//	�J������
//
//	2017/02/13	�R�[�f�B���O�J�n�i�k�j
//
//----------------------------------------------------------------------------------------
//	T.Tachibana
//	��L&F
//________________________________________________________________________________________
//

//	#include "obd2.h"			/*	CAN-OBDII ��`			*/

#ifndef		__CAN_OBD2_PROTOCOL__
#define		__CAN_OBD2_PROTOCOL__

/*
	OBD2 �����̊T�v

	OBD2��

	CAN�t���[����`
	
	�u���[�h�L���X�g
	CAN-ID	0x7DF
	
					ECU�N�G����M(0x7DF)�����ׂĂ�ECU�������ΏۂƂȂ�
						+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		PID-type / Byte	|			0			|			1			|			2			|			3			|			4			|			5			|			6			|			7			|
						+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		SAE standard	|Number of additional	|Mode					|PID code				|not used																												|
						| data bytes:2			|01=show current data	|(e.g.:05=Engine coolant|(may be 55h)																											|
						|						|02=freeze frame		| temperature)			|																														|
						|						|etc.					|						|																														|
						+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		Vehicle speific	|Number of additional	|Custom mode:			|PID code										|not used																						|
						| data bytes:3			|(e.g.:22=enhanced data	|(e.g.:4980h)									|(may be 00h or 55h)																			|
						|						|						|												|																								|
						|						|						|												|																								|
						+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
	
	�}���`�t���[��
	CAN-ID	0x7E0	ECU0(�p���g���n)�ւ̗v��
			0x7E1	ECU1(�{�f�B�[�n)�ւ̗v��
			0x7E2	ECU2(�V���V�[�n)�ւ̗v��
			0x7E3	ECU3(CGW-DTC)�ւ̗v��
			
					ECU�N�G����M(0x7E0�`7E7)����ECU�ɑ΂���OBD2�v��
						+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		PID-type / Byte	|			0			|			1			|			2			|			3			|			4			|			5			|			6			|			7			|
						+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		SAE standard	|Number of additional	|Mode					|PID code				|not used																												|
						| data bytes:2			|01=show current data	|(e.g.:05=Engine coolant|(may be 55h)																											|
						|						|02=freeze frame		| temperature)			|																														|
						|						|etc.					|						|																														|
						+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		Vehicle speific	|Number of additional	|Custom mode:			|PID code										|not used																						|
						| data bytes:3			|(e.g.:22=enhanced data	|(e.g.:4980h)									|(may be 00h or 55h)																			|
						|						|						|												|																								|
						|						|						|												|																								|
						+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
	
	CAN-ID	0x7E8	ECU0(�p���g���n)����̉���
			0x7E9	ECU1(�{�f�B�[�n)����̉���
			0x7EA	ECU2(�V���V�[�n)����̉���
			0x7EB	ECU3(CGW-DTC)����̉���
	
					ECU�N�G�����M(0x7E8�`7EF)
						+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		PID-type / Byte	|			0			|			1			|			2			|			3			|			4			|			5			|			6			|			7			|
						+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		SAE standard	|Number of additional	|Custom mode (+40h)		|PID code				|value of the			|value.					|value.					|value.					|not used				|
						| data bytes:3 to 6		|41=show current data	|(e.g.:05=Engine coolant|specified parameter.	|byte 1					|byte 2					|byte 3					|(may be 00h or 55h)	|
						|						|42=freeze frame		| temperature)			|byte 0					|(optional)				|(optional)				|(optional)				|						|
						|						|etc.					|						|						|						|						|						|						|
						+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		Vehicle speific	|Number of additional	|Custom mode (+40h)		|PID code										|value of the			|value.					|value.					|value.					|
						| data bytes:4 to 7		|(e.g.:62h=response to	|(e.g.:4980h)									|specified parameter.	|byte 1					|byte 2					|byte 3					|
						|						| mode 22h request)		|												|byte 0					|(optional)				|(optional)				|(optional)				|
						|						|						|												|						|						|						|						|
						+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
		Vehicle speific	|Number of additional	|7Fh this a general		|Custom mode:			|31h					|not used																						|
						| data bytes:3			|response usually		|(e.g.:22h=enhanced 	|						|(may be 00h)																					|
						|						|indicating the module	|diagnostic data by PID,|						|																								|
						|						| doesn't recognize the	|21h=enhanced data by	|						|																								|
						|						| request.				|offset					|						|																								|
						+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+-----------------------+
	

*/

//----------------------------------------------------------------------------------------
//	OBD2�p�P�b�g���p�̒�`
//----------------------------------------------------------------------------------------
typedef	union	__obd2_query_frame__	{
	unsigned char	BYTE[8];
	struct	{
		unsigned char	MODE;		//	���[�h
		unsigned char	PID;		//	�p�����[�^�ԍ�
		unsigned char	NU[6];		//	���g�p[55h]
	}	SAE_OBD;
	struct	{
		unsigned char	MODE;		//	���[�h�i0�`A�j
		unsigned char	PIDH;		//	�p�����[�^�ԍ����8�r�b�g
		unsigned char	PIDL;		//	�p�����[�^�ԍ�����8�r�b�g
		unsigned char	NU[5];		//	���g�p[55h]
	}	VS_OBD;
	struct	{
		unsigned char	MODE;		//	���[�h(+40h)
		unsigned char	PID;		//	�p�����[�^�ԍ�
		unsigned char	VAL[5];		//	�p�����[�^�l
		unsigned char	NU;			//	���g�p[00h or 55h]
	}	SAE_ECU;
	struct	{
		unsigned char	MODE;		//	���[�h(+40h)
		unsigned char	PIDH;		//	�p�����[�^�ԍ����8�r�b�g
		unsigned char	PIDL;		//	�p�����[�^�ԍ�����8�r�b�g
		unsigned char	VAL[5];		//	�p�����[�^�l
	}	VS_ECU;
	struct	{
		unsigned char	MODE;		//	���[�h(+40h)
		unsigned char	VAL[7];		//	�p�����[�^�l
	}	MD3_ECU;
	struct	{
		unsigned char	MODE;		//	���[�h(+40h)
		unsigned char	NU[7];		//	�p�����[�^�l
	}	MD4_ECU;
	struct	{
		unsigned char	X7F;		//	���Ή��v���R�[�h�F�Œ�[7Fh]
		unsigned char	MODE;		//	��M���[�h
		unsigned char	X31;		//	�Œ�[31h]
		unsigned char	NU[5];		//	���g�p[00h]
	}	NOT_ECU;
}	OBD2_QUERY_FRAME;

//----------------------------------------------------------------------------------------
//	MODE�R�[�h��`
//----------------------------------------------------------------------------------------
#define		SHOW_CURRENT_DATA		0x01		/*	���݂̒l�擾									*/
#define		SHOW_FREEZE_FDATA		0x02		/*	�ꎞ��~�t���[���̎擾							*/
#define		SHOW_STORED_DTC			0x03		/*	DTC(Diagnostic Trouble Codes)�̎擾(�f�f���O)	*/
#define		CLEAR_STORED_DTC		0x04		/*	���m��DTC���									*/
#define		TEST_RESULT_NON_CAN		0x05		/*	�������ʁiCAN�ł͋@�\���Ȃ��j					*/
#define		TEST_RESULT_ONLY_CAN	0x06		/*	�������ʁiCAN�̂ݎ_�f�Z���T�[�Ď��j				*/
#define		SHOW_PENDING_DTC		0x07		/*	���m�̐f�f�g���u���R�[�h���擾				*/
#define		CTRL_OPERATION_SYS		0x08		/*	�I���{�[�h�V�X�e���x��							*/
#define		REQUEST_VEHICLE_INFO	0x09		/*	�ԗ����̎擾									*/
#define		PERMANENT_DTC			0x0A		/*	�i��DTC���										*/

//----------------------------------------------------------------------------------------
//	�f�f�g���u���R�[�h(DTC)��`
//----------------------------------------------------------------------------------------
typedef	union	__obd2_dtc_str__	{
	unsigned char	BYTE[2];
	struct	{
		struct	{
			unsigned char	ECU		:	2;		//	1st DTC character	0:[P] Powertrain / 1:[C] Chassis / 2:[B] Body / 3:[U] Network
			unsigned char	CH		:	2;		//	2nd DTC character	0�`3
			unsigned char	CL		:	4;		//	3rd DTC character	0�`F
		}	A;
		struct	{
			unsigned char	CH		:	4;		//	4th DTC character	00�`FF
			unsigned char	CL		:	4;		//	5th DTC character	00�`FF
		}	B;
	}	BIT;
}	OBD2_DTC_STR;

#define		DTC_ECU_CODE_POW		0			/*	�p���g���f�f�R�[�h								*/
#define		DTC_ECU_CODE_CHA		1			/*	�V���V�[�f�f�R�[�h								*/
#define		DTC_ECU_CODE_BDY		2			/*	�{�f�B�[�f�f�R�[�h								*/
#define		DTC_ECU_CODE_NET		3			/*	�l�b�g���[�N�f�f�R�[�h							*/

//----------------------------------------------------------------------------------------
//	�ϐ���`
//----------------------------------------------------------------------------------------
extern	OBD2_QUERY_FRAME	obd2_req;	//	�v���f�[�^
extern	OBD2_QUERY_FRAME	obd2_ret;	//	�����f�[�^

extern	int		obd2_ret_counter;

//----------------------------------------------------------------------------------------
//	MODE1	����		0x0C,	0x0D,	0x1C,	0x2F,	0x31,	0x49,	0x51			
//----------------------------------------------------------------------------------------
extern	int obd2_mode1(int len);
//----------------------------------------------------------------------------------------
//	MODE2	����(DTC���R�[�h�ԐM�v��)
//----------------------------------------------------------------------------------------
extern	int obd2_mode2(int len);
//----------------------------------------------------------------------------------------
//	MODE3	����(�ۑ��ς�DTC���R�[�h��ԐM����)
//----------------------------------------------------------------------------------------
extern	int obd2_mode3(int len);
//----------------------------------------------------------------------------------------
//	MODE4	����(DTC���R�[�h����������)
//----------------------------------------------------------------------------------------
extern	int obd2_mode4(int len);
//----------------------------------------------------------------------------------------
//	MODE5	����
//----------------------------------------------------------------------------------------
extern	int obd2_mode5(int len);
//----------------------------------------------------------------------------------------
//	MODE6	�������ʁiCAN�̂ݎ_�f�Z���T�[�Ď��j
//----------------------------------------------------------------------------------------
extern	int obd2_mode6(int len);
//----------------------------------------------------------------------------------------
//	MODE7	���m�̐f�f�g���u���R�[�h���擾
//----------------------------------------------------------------------------------------
extern	int obd2_mode7(int len);
//----------------------------------------------------------------------------------------
//	MODE8	�I���{�[�h�V�X�e���x��
//----------------------------------------------------------------------------------------
extern	int obd2_mode8(int len);
//----------------------------------------------------------------------------------------
//	MODE9	����
//----------------------------------------------------------------------------------------
extern	int obd2_mode9(int len);
//----------------------------------------------------------------------------------------
//	MODE10	�i��DTC���
//----------------------------------------------------------------------------------------
extern	int obd2_modeA(int len);
//----------------------------------------------------------------------------------------
//	OBD2����
//----------------------------------------------------------------------------------------
extern	int obd2_job(unsigned char *msg, int len, unsigned char *res);

#endif		/*__CAN_OBD2_PROTOCOL__*/
