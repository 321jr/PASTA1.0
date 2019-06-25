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

#include <sysio.h>
#include <string.h>
#include <stdio.h>
#include "iodefine.h"
#include "timer.h"
#include "ecu.h"			/*	ECU ���ʒ�`			*/
#include "can3_spi2.h"		/*	CAN3 ��`				*/
#include "cantp.h"			/*	CAN-TP ��`				*/
#include "obd2.h"			/*	CAN-OBDII ��`			*/

#if	0

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

#endif

//----------------------------------------------------------------------------------------
//	�ϐ���`
//----------------------------------------------------------------------------------------
OBD2_QUERY_FRAME	obd2_req;	//	�v���f�[�^
OBD2_QUERY_FRAME	obd2_ret;	//	�����f�[�^

int		obd2_ret_counter = 0;

//----------------------------------------------------------------------------------------
//	MODE1	����		0x0C,	0x0D,	0x1C,	0x2F,	0x31,	0x49,	0x51			
//----------------------------------------------------------------------------------------
int obd2_mode1(int len)
{
	int	d;
	//	�X�^���_�[�h�v��
	if(len >= 2)
	{
		len = 2;								//	�f�t�H���g2�o�C�g
		obd2_ret.SAE_ECU.MODE = obd2_req.SAE_OBD.MODE + 0x40;	//	�����t���O
		obd2_ret.SAE_ECU.PID = obd2_req.SAE_OBD.PID;			//	�p�����[�^ID�R�s�[
		//	PID���̏���
		switch(obd2_req.SAE_OBD.PID)
		{
		//------------------------------------------------------
		case 0x00:	//	�T�|�[�gPID��� [01 - 20]
		//------------------------------------------------------
			len += 4;
			obd2_ret.SAE_ECU.VAL[0] = 0x00;
			obd2_ret.SAE_ECU.VAL[1] = 0x18;
			obd2_ret.SAE_ECU.VAL[2] = 0x00;
			obd2_ret.SAE_ECU.VAL[3] = 0x11;	//	0x0C,0x0D,0x1C,(0x20)
			break;
		case 0x01:	//	Monitor status since DTCs cleared. (Includes malfunction indicator lamp (MIL) status and number of DTCs.)
			len += 4;
			obd2_ret.SAE_ECU.VAL[0] = 0x00;		//(A)	bit7:MIL�����`�F�b�N / bit6�`0:DTC_CNT
			obd2_ret.SAE_ECU.VAL[1] = 0x00;		//(B)	bit7:[0] / bit3:0=�_�΃v���O��,1=�f�B�[�[�� / bit2:�\���e�X�g,bit6:�s���S / bit1:�R���v�e�X�g,bit5:�s���S /bit0:�s���e�X�g,bit4:�s���S
			obd2_ret.SAE_ECU.VAL[2] = 0x00;		//(C)	bit7:EGR�V�X�e���e�X�g / bit6:�_�f�Z���T�[�E�q�[�^�e�X�g / bit5:�_�f�Z���T�[�e�X�g / bit4:AC��}�e�X�g / bit3:��2��C�V�X�e���e�X�g / bit2:�����V�X�e���e�X�g / bit1:���M���i�܃e�X�g / bit0:���i�܃e�X�g
			obd2_ret.SAE_ECU.VAL[3] = 0x00;		//(D)	bit7:EGR�V�X�e���s���S / bit6:�_�f�Z���T�[�E�q�[�^�s���S / bit5:�_�f�Z���T�[�s���S / bit4:AC��}�s���S / bit3:��2��C�V�X�e���s���S / bit2:�����V�X�e���s���S / bit1:���M���i�ܕs���S / bit0:���i�ܕs���S
			break;
		case 0x02:	//	Freeze DTC (DTC�̕ۑ����~����)
			break;
		case 0x03:	//	Fuel system status
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 2;	//	1:�G���W���ቷ�J���[�v / 2:������t�B�[�h�o�b�N���[�v / 4:�������[�v / 8:�̏�J���[�v / 16:���[�v����
			break;
		case 0x04:	//	Calculated engine load
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//	0�`100[%]�G���W���g���N�l		100/255*A
			break;
		case 0x05:	//	Engine coolant temperature
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = can_buf.ID[0x183].BYTE[0];	//	-40�`215[��]�G���W����p�܉��x	A-40
			break;
		case 0x06:	//	Short term fuel trim-Bank 1
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//	-100�`99.2[%] �Z���R���o���N1	100/128*A-100
			break;
		case 0x07:	//	Long term fuel trim-Bank 1
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//	-100�`99.2[%] �����R���o���N1	100/128*A-100
			break;
		case 0x08:	//	Short term fuel trim-Bank 2
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//	-100�`99.2[%] �Z���R���o���N2	100/128*A-100
			break;
		case 0x09:	//	Long term fuel trim-Bank 2
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//	-100�`99.2[%] �����R���o���N2	100/128*A-100
			break;
		case 0x0A:	//	Fuel pressure (gauge pressure)
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//	0�`765[kPa](/3) �R����			3*A
			break;
		case 0x0B:	//	Intake manifold absolute pressure
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//	0�`255[kPa] �r�C����Έ�		A
			break;
		case 0x0C:	//	Engine RPM
			len += 2;
			d = ((((int)can_buf.ID[0x043].BYTE[0]) << 8) | (((int)can_buf.ID[0x043].BYTE[1]) & 0xFF)) * 4;
			obd2_ret.SAE_ECU.VAL[0] = (unsigned char)(d >> 8);		//	0�`16383.75[rpm] �G���W����]��	(256A+B)/4
			obd2_ret.SAE_ECU.VAL[1] = (unsigned char)(d & 0xFF);	//	������8bit
			break;
		case 0x0D:	//	Vehicle speed
			len += 1;
			d = ((((int)can_buf.ID[0x043].BYTE[2]) << 8) | (((int)can_buf.ID[0x043].BYTE[3]) & 0xFF));
			if(d & 0x8000) d = 0x10000 - d;
			obd2_ret.SAE_ECU.VAL[0] = d;	//	0�`255[km/h] �ԑ�				A
			break;
		case 0x0E:	//	Timing advance
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//	-64�`63.5[��] �O�i�^�C�~���O	A/2-64
			break;
		case 0x0F:	//	Intake air temperature
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//	-40�`215[��] �z�C���x			A-40
			break;
		case 0x10:	//	 MAF air flow rate 
			len += 2;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//	0�`655.35[g/s] �����C���[�g		(256A+B)/100
			obd2_ret.SAE_ECU.VAL[1] = 0;	//	������8bit
			break;
		case 0x11:	//	Throttle position
			len += 1;
			d = ((((int)can_buf.ID[0x02F].BYTE[0]) << 8) | (((int)can_buf.ID[0x02F].BYTE[1]) & 0xFF)) * 255 / 1023;
			obd2_ret.SAE_ECU.VAL[0] = (unsigned char)(d & 0xFF);	//	0�`100[%] �X���b�g���ʒu		100A/255
			break;
		case 0x12:	//	Commanded secondary air status
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 1;	//	1:�㗬 / 2:�G�}�� / 4:�O�C / 8:�f�f��
			break;
		case 0x13:	//	Oxygen sensors present (in 2 banks)
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 1;	//	bit0�`3:Bank1,�Z���T�[1�`4 / bit4�`7:Bank2,�Z���T�[1�`4	�_�f�Z���T�[���݃t���O
			break;
		case 0x14:	//	Oxygen Sensor 1  A: Voltage  B: Short term fuel trim
			len += 2;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//	0�`1.275[V] �_�f�Z���T�[1�d��	A/200
			obd2_ret.SAE_ECU.VAL[1] = 0;	//	-100�`99.2[%] �Z���R������		100/128B-100
			break;
		case 0x15:	//	Oxygen Sensor 2  A: Voltage  B: Short term fuel trim
			len += 2;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//	0�`1.275[V] �_�f�Z���T�[2�d��	A/200
			obd2_ret.SAE_ECU.VAL[1] = 0;	//	-100�`99.2[%] �Z���R������		100/128B-100
			break;
		case 0x16:	//	Oxygen Sensor 3  A: Voltage  B: Short term fuel trim
			len += 2;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//	0�`1.275[V] �_�f�Z���T�[3�d��	A/200
			obd2_ret.SAE_ECU.VAL[1] = 0;	//	-100�`99.2[%] �Z���R������		100/128B-100
			break;
		case 0x17:	//	Oxygen Sensor 4  A: Voltage  B: Short term fuel trim
			len += 2;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//	0�`1.275[V] �_�f�Z���T�[4�d��	A/200
			obd2_ret.SAE_ECU.VAL[1] = 0;	//	-100�`99.2[%] �Z���R������		100/128B-100
			break;
		case 0x18:	//	Oxygen Sensor 5  A: Voltage  B: Short term fuel trim
			len += 2;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//	0�`1.275[V] �_�f�Z���T�[5�d��	A/200
			obd2_ret.SAE_ECU.VAL[1] = 0;	//	-100�`99.2[%] �Z���R������		100/128B-100
			break;
		case 0x19:	//	Oxygen Sensor 6  A: Voltage  B: Short term fuel trim
			len += 2;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//	0�`1.275[V] �_�f�Z���T�[6�d��	A/200
			obd2_ret.SAE_ECU.VAL[1] = 0;	//	-100�`99.2[%] �Z���R������		100/128B-100
			break;
		case 0x1A:	//	Oxygen Sensor 7  A: Voltage  B: Short term fuel trim
			len += 2;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//	0�`1.275[V] �_�f�Z���T�[7�d��	A/200
			obd2_ret.SAE_ECU.VAL[1] = 0;	//	-100�`99.2[%] �Z���R������		100/128B-100
			break;
		case 0x1B:	//	Oxygen Sensor 8  A: Voltage  B: Short term fuel trim
			len += 2;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//	0�`1.275[V] �_�f�Z���T�[8�d��	A/200
			obd2_ret.SAE_ECU.VAL[1] = 0;	//	-100�`99.2[%] �Z���R������		100/128B-100
			break;
		case 0x1C:	//	OBD standards this vehicle conforms to
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 1;	//	1:CARB�d�l��OBD2 / 11:JOBD,JOBD2 / 13:JOBD,EOBD,OBD2 ��
			break;
		case 0x1D:	//	Oxygen sensors present (in 4 banks)
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 1;	//	bit0�`1:Bank1,�Z���T�[1�`2 / bit2�`3:Bank2,�Z���T�[1�`2 / bit4�`5:Bank3,�Z���T�[1�`2 / bit6�`7:Bank4,�Z���T�[1�`2	�_�f�Z���T�[���݃t���O
			break;
		case 0x1E:	//	Auxiliary input status
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 1;	//	bit0:PTO(Power take Off) 1=�L��,0=���� / bit7�`1:[0]	�⏕���̓X�e�[�^�X
			break;
		case 0x1F:	//	Run time since engine start
			len += 2;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`65535[�b] �G���W���X�^�[�g����̌o�ߎ���	256*A+B
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		//------------------------------------------------------
		case 0x20:	//	PIDs supported [21 - 40]
		//------------------------------------------------------
			len += 4;
			obd2_ret.SAE_ECU.VAL[0] = 0x00;
			obd2_ret.SAE_ECU.VAL[1] = 0x02;
			obd2_ret.SAE_ECU.VAL[2] = 0x80;
			obd2_ret.SAE_ECU.VAL[3] = 0x01;	//	0x2F,0x31,(0x40)
			break;
		case 0x21:	//	Distance traveled with malfunction indicator lamp (MIL) on
			len += 2;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`65535[km] MIL�x�񂩂�̑��s����	256*A+B
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		case 0x22:	//	Fuel Rail Pressure (relative to manifold vacuum)
			len += 2;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`5177.265[kPa] �R�����[����	0.079*(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		case 0x23:	//	Fuel Rail Gauge Pressure (diesel, or gasoline direct injection)
			len += 2;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`655350[kPa] �R�����[���Q�[�W��	10*(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		case 0x24:	//	Oxygen Sensor 1  AB: Fuel-Air Equivalence Ratio CD: Voltage
			len += 2;		//	�_�f�Z���T�[1
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`2[�䗦] �R��������	2/65536*(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			obd2_ret.SAE_ECU.VAL[2] = 0;	//(C)	0�`8[V] �Z���T�[�d��	8/65536*(256*C+D)
			obd2_ret.SAE_ECU.VAL[3] = 0;	//(D)	������8bit
			break;
		case 0x25:	//	Oxygen Sensor 2  AB: Fuel-Air Equivalence Ratio CD: Voltage
			len += 2;		//	�_�f�Z���T�[2
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`2[�䗦] �R��������	2/65536*(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			obd2_ret.SAE_ECU.VAL[2] = 0;	//(C)	0�`8[V] �Z���T�[�d��	8/65536*(256*C+D)
			obd2_ret.SAE_ECU.VAL[3] = 0;	//(D)	������8bit
			break;
		case 0x26:	//	Oxygen Sensor 3  AB: Fuel-Air Equivalence Ratio CD: Voltage
			len += 2;		//	�_�f�Z���T�[3
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`2[�䗦] �R��������	2/65536*(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			obd2_ret.SAE_ECU.VAL[2] = 0;	//(C)	0�`8[V] �Z���T�[�d��	8/65536*(256*C+D)
			obd2_ret.SAE_ECU.VAL[3] = 0;	//(D)	������8bit
			break;
		case 0x27:	//	Oxygen Sensor 4  AB: Fuel-Air Equivalence Ratio CD: Voltage
			len += 2;		//	�_�f�Z���T�[4
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`2[�䗦] �R��������	2/65536*(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			obd2_ret.SAE_ECU.VAL[2] = 0;	//(C)	0�`8[V] �Z���T�[�d��	8/65536*(256*C+D)
			obd2_ret.SAE_ECU.VAL[3] = 0;	//(D)	������8bit
			break;
		case 0x28:	//	Oxygen Sensor 5  AB: Fuel-Air Equivalence Ratio CD: Voltage
			len += 2;		//	�_�f�Z���T�[5
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`2[�䗦] �R��������	2/65536*(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			obd2_ret.SAE_ECU.VAL[2] = 0;	//(C)	0�`8[V] �Z���T�[�d��	8/65536*(256*C+D)
			obd2_ret.SAE_ECU.VAL[3] = 0;	//(D)	������8bit
			break;
		case 0x29:	//	Oxygen Sensor 6  AB: Fuel-Air Equivalence Ratio CD: Voltage
			len += 2;		//	�_�f�Z���T�[6
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`2[�䗦] �R��������	2/65536*(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			obd2_ret.SAE_ECU.VAL[2] = 0;	//(C)	0�`8[V] �Z���T�[�d��	8/65536*(256*C+D)
			obd2_ret.SAE_ECU.VAL[3] = 0;	//(D)	������8bit
			break;
		case 0x2A:	//	Oxygen Sensor 7  AB: Fuel-Air Equivalence Ratio CD: Voltage
			len += 2;		//	�_�f�Z���T�[7
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`2[�䗦] �R��������	2/65536*(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			obd2_ret.SAE_ECU.VAL[2] = 0;	//(C)	0�`8[V] �Z���T�[�d��	8/65536*(256*C+D)
			obd2_ret.SAE_ECU.VAL[3] = 0;	//(D)	������8bit
			break;
		case 0x2B:	//	Oxygen Sensor 8  AB: Fuel-Air Equivalence Ratio CD: Voltage
			len += 2;		//	�_�f�Z���T�[8
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`2[�䗦] �R��������	2/65536*(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			obd2_ret.SAE_ECU.VAL[2] = 0;	//(C)	0�`8[V] �Z���T�[�d��	8/65536*(256*C+D)
			obd2_ret.SAE_ECU.VAL[3] = 0;	//(D)	������8bit
			break;
		case 0x2C:	//	Commanded EGR
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`100[%] EGR����		100/255*A
			break;
		case 0x2D:	//	EGR Error
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	-100�`99.2[%] EGR�G���[	100/128*A-100
			break;
		case 0x2E:	//	Commanded evaporative purge
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`100[%] �C���ʖ���	100/255*A
			break;
		case 0x2F:	//	Fuel Tank Level Input
			len += 1;
			d = ((int)can_buf.ID[0x3D4].BYTE[0]) * 255 / 40;	//	0..40���b�g��
			if(d > 255) d = 255;
			obd2_ret.SAE_ECU.VAL[0] = (unsigned char)d;	//(A)	0�`100[%] �R���c��		100/255*A
			break;
		case 0x30:	//	Warm-ups since codes cleared
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`255[count] �E�H�[���A�b�v�J�E���g	A
			break;
		case 0x31:	//	Distance traveled since codes cleared
			len += 2;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`65535[km] �v���Z�b�g���s�����v	256*A+B
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		case 0x32:	//	Evap. System Vapor Pressure
			len += 2;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	-8192�`8191.75[Pa] �V�X�e�����C��	(256*A+B)/4
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		case 0x33:	//	Absolute Barometric Pressure
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`255[kPa] ��΋C��	A
			break;
		case 0x34:	//	Oxygen Sensor 1  AB: Fuel-Air Equivalence Ratio CD: Current
			len += 4;		//	�_�f�Z���T�[1
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`2[�䗦] �R��������	2/65536*(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			obd2_ret.SAE_ECU.VAL[2] = 0;	//(C)	-128�`128[mA] �Z���T�[�d��	(256*C+D)/256-128
			obd2_ret.SAE_ECU.VAL[3] = 0;	//(D)	������8bit
			break;
		case 0x35:	//	Oxygen Sensor 2  AB: Fuel-Air Equivalence Ratio CD: Current
			len += 4;		//	�_�f�Z���T�[2
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`2[�䗦] �R��������	2/65536*(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			obd2_ret.SAE_ECU.VAL[2] = 0;	//(C)	-128�`128[mA] �Z���T�[�d��	(256*C+D)/256-128
			obd2_ret.SAE_ECU.VAL[3] = 0;	//(D)	������8bit
			break;
		case 0x36:	//	Oxygen Sensor 3  AB: Fuel-Air Equivalence Ratio CD: Current
			len += 4;		//	�_�f�Z���T�[3
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`2[�䗦] �R��������	2/65536*(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			obd2_ret.SAE_ECU.VAL[2] = 0;	//(C)	-128�`128[mA] �Z���T�[�d��	(256*C+D)/256-128
			obd2_ret.SAE_ECU.VAL[3] = 0;	//(D)	������8bit
			break;
		case 0x37:	//	Oxygen Sensor 4  AB: Fuel-Air Equivalence Ratio CD: Current
			len += 4;		//	�_�f�Z���T�[4
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`2[�䗦] �R��������	2/65536*(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			obd2_ret.SAE_ECU.VAL[2] = 0;	//(C)	-128�`128[mA] �Z���T�[�d��	(256*C+D)/256-128
			obd2_ret.SAE_ECU.VAL[3] = 0;	//(D)	������8bit
			break;
		case 0x38:	//	Oxygen Sensor 5  AB: Fuel-Air Equivalence Ratio CD: Current
			len += 4;		//	�_�f�Z���T�[5
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`2[�䗦] �R��������	2/65536*(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			obd2_ret.SAE_ECU.VAL[2] = 0;	//(C)	-128�`128[mA] �Z���T�[�d��	(256*C+D)/256-128
			obd2_ret.SAE_ECU.VAL[3] = 0;	//(D)	������8bit
			break;
		case 0x39:	//	Oxygen Sensor 6  AB: Fuel-Air Equivalence Ratio CD: Current
			len += 4;		//	�_�f�Z���T�[6
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`2[�䗦] �R��������	2/65536*(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			obd2_ret.SAE_ECU.VAL[2] = 0;	//(C)	-128�`128[mA] �Z���T�[�d��	(256*C+D)/256-128
			obd2_ret.SAE_ECU.VAL[3] = 0;	//(D)	������8bit
			break;
		case 0x3A:	//	Oxygen Sensor 7  AB: Fuel-Air Equivalence Ratio CD: Current
			len += 4;		//	�_�f�Z���T�[7
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`2[�䗦] �R��������	2/65536*(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			obd2_ret.SAE_ECU.VAL[2] = 0;	//(C)	-128�`128[mA] �Z���T�[�d��	(256*C+D)/256-128
			obd2_ret.SAE_ECU.VAL[3] = 0;	//(D)	������8bit
			break;
		case 0x3B:	//	Oxygen Sensor 8  AB: Fuel-Air Equivalence Ratio CD: Current
			len += 4;		//	�_�f�Z���T�[8
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`2[�䗦] �R��������	2/65536*(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			obd2_ret.SAE_ECU.VAL[2] = 0;	//(C)	-128�`128[mA] �Z���T�[�d��	(256*C+D)/256-128
			obd2_ret.SAE_ECU.VAL[3] = 0;	//(D)	������8bit
			break;
		case 0x3C:	//	Catalyst Temperature: Bank 1, Sensor 1
			len += 2;		//	���i�܉��x�o���N1�Z���T�[1
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	-40�`6513.5[��] ���x	(256*A+B)/10-40
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		case 0x3D:	//	Catalyst Temperature: Bank 2, Sensor 1
			len += 2;		//	���i�܉��x�o���N2�Z���T�[1
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	-40�`6513.5[��] ���x	(256*A+B)/10-40
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		case 0x3E:	//	Catalyst Temperature: Bank 1, Sensor 2
			len += 2;		//	���i�܉��x�o���N1�Z���T�[2
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	-40�`6513.5[��] ���x	(256*A+B)/10-40
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		case 0x3F:	//	Catalyst Temperature: Bank 2, Sensor 2
			len += 2;		//	���i�܉��x�o���N2�Z���T�[2
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	-40�`6513.5[��] ���x	(256*A+B)/10-40
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		//------------------------------------------------------
		case 0x40:	//	PIDs supported [41 - 60]
		//------------------------------------------------------
			len += 4;
			obd2_ret.SAE_ECU.VAL[0] = 0x00;
			obd2_ret.SAE_ECU.VAL[1] = 0x80;
			obd2_ret.SAE_ECU.VAL[2] = 0x80;
			obd2_ret.SAE_ECU.VAL[3] = 0x00;	//	0x49,0x51
			break;
		case 0x41:	//	Monitor status this drive cycle
			len += 4;
			obd2_ret.SAE_ECU.VAL[0] = 0x00;		//(A)	[0]
			obd2_ret.SAE_ECU.VAL[1] = 0x00;		//(B)	bit7:[0] / bit3:0=�_�΃v���O��,1=�f�B�[�[�� / bit2:�\���e�X�g,bit6:�s���S / bit1:�R���v�e�X�g,bit5:�s���S /bit0:�s���e�X�g,bit4:�s���S
			obd2_ret.SAE_ECU.VAL[2] = 0x00;		//(C)	bit7:EGR�V�X�e���e�X�g / bit6:�_�f�Z���T�[�E�q�[�^�e�X�g / bit5:�_�f�Z���T�[�e�X�g / bit4:AC��}�e�X�g / bit3:��2��C�V�X�e���e�X�g / bit2:�����V�X�e���e�X�g / bit1:���M���i�܃e�X�g / bit0:���i�܃e�X�g
			obd2_ret.SAE_ECU.VAL[3] = 0x00;		//(D)	bit7:EGR�V�X�e���s���S / bit6:�_�f�Z���T�[�E�q�[�^�s���S / bit5:�_�f�Z���T�[�s���S / bit4:AC��}�s���S / bit3:��2��C�V�X�e���s���S / bit2:�����V�X�e���s���S / bit1:���M���i�ܕs���S / bit0:���i�ܕs���S
			break;
		case 0x42:	//	Control module voltage
			len += 2;		//	���䃂�W���[���d��
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`65.535[V] �d��	(256*A+B)/1000
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		case 0x43:	//	Absolute load value
			len += 2;		//	��Ύd����
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`25700[%] �d����	100/255*(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		case 0x44:	//	Fuel-Air commanded equivalence ratio
			len += 2;		//	����������w��
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`2[�䗦] �R��������	2/65536*(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		case 0x45:	//	Relative throttle position
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`100[%] �X���b�g�����Έʒu	100/255*A
			break;
		case 0x46:	//	Ambient air temperature
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	-40�`215[��] ���͉��x	A-40
			break;
		case 0x47:	//	Absolute throttle position B
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`100[%] �X���b�g����ΈʒuB	100/255*A
			break;
		case 0x48:	//	Absolute throttle position C
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`100[%] �X���b�g����ΈʒuC	100/255*A
			break;
		case 0x49:	//	Accelerator pedal position D
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`100[%] �A�N�Z���ʒuD	100/255*A
			break;
		case 0x4A:	//	Accelerator pedal position E
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`100[%] �A�N�Z���ʒuE	100/255*A
			break;
		case 0x4B:	//	Accelerator pedal position F
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`100[%] �A�N�Z���ʒuF	100/255*A
			break;
		case 0x4C:	//	Commanded throttle actuator
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`100[%] �X���b�g���N���w��	100/255*A
			break;
		case 0x4D:	//	Time run with MIL on
			len += 2;		//	MIL����o�ߎ���
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`65535[��] �o�ߎ���	256*A+B
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		case 0x4E:	//	Time since trouble codes cleared
			len += 2;		//	�x���������̌o�ߎ���
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`65535[��] �o�ߎ���	256*A+B
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		case 0x4F:	//	Maximum value for Fuel-Air equivalence ratio, oxygen sensor voltage, oxygen sensor current, and intake manifold absolute pressure
			len += 4;
			obd2_ret.SAE_ECU.VAL[0] = 0x00;		//(A)	0�`255[�䗦] �����C�䗦		A
			obd2_ret.SAE_ECU.VAL[1] = 0x00;		//(B)	0�`255[V] �_�f�Z���T�[�d��	B
			obd2_ret.SAE_ECU.VAL[2] = 0x00;		//(C)	0�`255[mA] �_�f�Z���T�[�d��	C
			obd2_ret.SAE_ECU.VAL[3] = 0x00;		//(D)	0�`2550[kPa] �z�C��			D*10
			break;
		case 0x50:	//	Maximum value for air flow rate from mass air flow sensor
			len += 4;
			obd2_ret.SAE_ECU.VAL[0] = 0x00;		//(A)	0�`2550[g/s] �ő�C����		A*10
			obd2_ret.SAE_ECU.VAL[1] = 0x00;		//(B)	�\��
			obd2_ret.SAE_ECU.VAL[2] = 0x00;		//(C)	�\��
			obd2_ret.SAE_ECU.VAL[3] = 0x00;		//(D)	�\��
			break;
		case 0x51:	//	Fuel Type
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 1;	//(A)	1:�K�\���� / 2:���^�m�[�� / 3:�G�^�m�[�� / 4:�f�B�[�[�� / 5:LPG / 6:CNG / 7:�v���p�� / 8:�d�C / 21:�����^�p���d�C�Ɠ��R�@��(�v���E�X)
			break;
		case 0x52:	//	Ethanol fuel %
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`100[%] �G�^�m�[���R��	100/255*A
			break;
		case 0x53:	//	Absolute Evap system Vapor Pressure
			len += 2;		//	��΃V�X�e�����C��
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`327.675[kPa] �C��	(256*A+B)/200
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		case 0x54:	//	Evap system vapor pressure
			len += 2;		//	�V�X�e�����C��
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	-32767�`32767[Pa] �C��	(256*A+B)-32767
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		case 0x55:	//	Short term secondary oxygen sensor trim, A: bank 1, B: bank 3
			len += 2;		//	�Z�����_�f�Z���T�[����
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	-100�`99.2[%] �o���N1�����l		100/128*A-100
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	-100�`99.2[%] �o���N3�����l		100/128*A-100
			break;
		case 0x56:	//	Long term secondary oxygen sensor trim, A: bank 1, B: bank 3
			len += 2;		//	�������_�f�Z���T�[����
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	-100�`99.2[%] �o���N1�����l		100/128*A-100
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	-100�`99.2[%] �o���N3�����l		100/128*A-100
			break;
		case 0x57:	//	Short term secondary oxygen sensor trim, A: bank 2, B: bank 4
			len += 2;		//	�Z�����_�f�Z���T�[����
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	-100�`99.2[%] �o���N2�����l		100/128*A-100
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	-100�`99.2[%] �o���N4�����l		100/128*A-100
			break;
		case 0x58:	//	Long term secondary oxygen sensor trim, A: bank 2, B: bank 4
			len += 2;		//	�������_�f�Z���T�[����
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	-100�`99.2[%] �o���N2�����l		100/128*A-100
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	-100�`99.2[%] �o���N4�����l		100/128*A-100
			break;
		case 0x59:	//	Fuel rail absolute pressure
			len += 2;		//	�R�����[����Έ�
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`655350[kPa] �C��		10(256*A+B)
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		case 0x5A:	//	Relative accelerator pedal position
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`100[%] �A�N�Z�����Έʒu	100/255*A
			break;
		case 0x5B:	//	Hybrid battery pack remaining life
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`100[%] �n�C�u���b�h�o�b�e���[�c��	100/255*A
			break;
		case 0x5C:	//	Engine oil temperature
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//	-40�`210[��] �G���W���I�C�����x		A-40
			break;
		case 0x5D:	//	Fuel injection timing
			len += 2;		//	�R�����˃^�C�~���O
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	-210.00�`301.992[��] �^�C�~���O	(256*A+B)/128-210
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		case 0x5E:	//	Engine fuel rate
			len += 2;		//	�G���W���R����
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`3276.75[L/h] �����R������	(256*A+B)/20
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		case 0x5F:	//	Emission requirements to which vehicle is designed
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	�ԗ����o�����݌v�l�H
			break;
		//------------------------------------------------------
		case 0x60:	//	PIDs supported [61 - 80]
		//------------------------------------------------------
			len += 4;
			obd2_ret.SAE_ECU.VAL[0] = 0x00;
			obd2_ret.SAE_ECU.VAL[1] = 0x00;
			obd2_ret.SAE_ECU.VAL[2] = 0x00;
			obd2_ret.SAE_ECU.VAL[3] = 0x00;
			break;
		case 0x61:	//	Driver's demand engine - percent torque
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	-125�`125[%] �h���C�o�[���v�g���N	A-125
			break;
		case 0x62:	//	Actual engine - percent torque
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	-125�`125[%] ���ۂ̃G���W���g���N	A-125
			break;
		case 0x63:	//	Engine reference torque
			len += 2;		//	�Q�ƃG���W���g���N
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	0�`655.35[Nm] �g���N	256*A+B
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	������8bit
			break;
		case 0x64:	//	Engine percent torque data
			len += 5;
			obd2_ret.SAE_ECU.VAL[0] = 0;	//(A)	-125�`125[%] �A�C�h���g���N	A-125
			obd2_ret.SAE_ECU.VAL[1] = 0;	//(B)	-125�`125[%] �G���W���_1	B-125
			obd2_ret.SAE_ECU.VAL[2] = 0;	//(C)	-125�`125[%] �G���W���_2	C-125
			obd2_ret.SAE_ECU.VAL[3] = 0;	//(D)	-125�`125[%] �G���W���_3	D-125
			obd2_ret.SAE_ECU.VAL[4] = 0;	//(E)	-125�`125[%] �G���W���_4	E-125
			break;
		case 0x65:	//	Auxiliary input / output supported
			break;
		case 0x66:	//	Mass air flow sensor
			break;
		case 0x67:	//	Engine coolant temperature
			break;
		case 0x68:	//	Intake air temperature sensor
			break;
		case 0x69:	//	Commanded EGR and EGR Error
			break;
		case 0x6A:	//	Commanded Diesel intake air flow control and relative intake air flow position
			break;
		case 0x6B:	//	Exhaust gas recirculation temperature
			break;
		case 0x6C:	//	Commanded throttle actuator control and relative throttle position
			break;
		case 0x6D:	//	Fuel pressure control system
			break;
		case 0x6E:	//	Injection pressure control system
			break;
		case 0x6F:	//	Turbocharger compressor inlet pressure
			break;
		case 0x70:	//	Boost pressure control
			break;
		case 0x71:	//	Variable Geometry turbo (VGT) control
			break;
		case 0x72:	//	Wastegate control
			break;
		case 0x73:	//	Exhaust pressure
			break;
		case 0x74:	//	Turbocharger RPM
			break;
		case 0x75:	//	Turbocharger temperature
			break;
		case 0x76:	//	Turbocharger temperature
			break;
		case 0x77:	//	Charge air cooler temperature (CACT)
			break;
		case 0x78:	//	Exhaust Gas temperature (EGT) Bank 1
			len += 4;		//	�o���N1 �r�C�K�X���x
			switch(obd2_ret_counter)
			{
			case 0:
				len = 1;
				obd2_ret.SAE_ECU.VAL[0] = 0x01;		//	1:EGT�Z���T�[1 / 2:EGT�Z���T�[2 / 4:EGT�Z���T�[3 / 8:EGT�Z���T�[4
				break;
			case 1:	//	EGT�Z���T�[1, EGT�Z���T�[2
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			case 2:	//	EGT�Z���T�[3, EGT�Z���T�[4
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			}
			obd2_ret_counter++;
			if(obd2_ret_counter >= 3) obd2_ret_counter = 0;
			break;
		case 0x79:	//	Exhaust Gas temperature (EGT) Bank 2
			len += 4;		//	�o���N2 �r�C�K�X���x
			switch(obd2_ret_counter)
			{
			case 0:
				len = 1;
				obd2_ret.SAE_ECU.VAL[0] = 0x01;		//	1:EGT�Z���T�[1 / 2:EGT�Z���T�[2 / 4:EGT�Z���T�[3 / 8:EGT�Z���T�[4
				break;
			case 1:	//	EGT�Z���T�[1, EGT�Z���T�[2
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			case 2:	//	EGT�Z���T�[3, EGT�Z���T�[4
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			}
			obd2_ret_counter++;
			if(obd2_ret_counter >= 3) obd2_ret_counter = 0;
			break;
		case 0x7A:	//	Diesel particulate filter (DPF)
			break;
		case 0x7B:	//	Diesel particulate filter (DPF)
			break;
		case 0x7C:	//	Diesel Particulate filter (DPF) temperature
			break;
		case 0x7D:	//	NOx NTE control area status
			break;
		case 0x7E:	//	PM NTE control area status
			break;
		case 0x7F:	//	Engine run time
			break;
		//------------------------------------------------------
		case 0x80:	//	PIDs supported [81 - A0]
		//------------------------------------------------------
			len += 4;
			obd2_ret.SAE_ECU.VAL[0] = 0x00;
			obd2_ret.SAE_ECU.VAL[1] = 0x00;
			obd2_ret.SAE_ECU.VAL[2] = 0x00;
			obd2_ret.SAE_ECU.VAL[3] = 0x00;
			break;
		case 0x81:	//	Engine run time for Auxiliary Emissions Control Device(AECD)
			break;
		case 0x82:	//	Engine run time for Auxiliary Emissions Control Device(AECD)
			break;
		case 0x83:	//	NOx sensor
			break;
		case 0x84:	//	Manifold surface temperature
			break;
		case 0x85:	//	NOx reagent system
			break;
		case 0x86:	//	Particulate matter (PM) sensor
			break;
		case 0x87:	//	Intake manifold absolute pressure
			break;
		//------------------------------------------------------
		case 0xA0:	//	PIDs supported [A1 - C0]
		//------------------------------------------------------
			len += 4;
			obd2_ret.SAE_ECU.VAL[0] = 0x00;
			obd2_ret.SAE_ECU.VAL[1] = 0x00;
			obd2_ret.SAE_ECU.VAL[2] = 0x00;
			obd2_ret.SAE_ECU.VAL[3] = 0x00;
			break;
		//------------------------------------------------------
		case 0xC0:	//	PIDs supported [C1 - E0]
		//------------------------------------------------------
			len += 4;
			obd2_ret.SAE_ECU.VAL[0] = 0x00;
			obd2_ret.SAE_ECU.VAL[1] = 0x00;
			obd2_ret.SAE_ECU.VAL[2] = 0x00;
			obd2_ret.SAE_ECU.VAL[3] = 0x00;
			break;
		//------------------------------------------------------
		case 0xE0:	//	PIDs supported [E1 - FF]
		//------------------------------------------------------
			len += 4;
			obd2_ret.SAE_ECU.VAL[0] = 0x00;
			obd2_ret.SAE_ECU.VAL[1] = 0x00;
			obd2_ret.SAE_ECU.VAL[2] = 0x00;
			obd2_ret.SAE_ECU.VAL[3] = 0x00;
			break;
		}
	}
	else
	{
		return 0;
	}
	return len;
}

//----------------------------------------------------------------------------------------
//	MODE2	����(DTC���R�[�h�ԐM�v��)
//----------------------------------------------------------------------------------------
int obd2_mode2(int len)
{
	if(len == 2)
	{	//	�X�^���_�[�h�v��
		len = 2;
		obd2_ret.SAE_ECU.MODE = obd2_req.SAE_OBD.MODE + 0x40;
		obd2_ret.SAE_ECU.PID = obd2_req.SAE_OBD.PID;
		switch(obd2_req.SAE_OBD.PID)
		{
		//------------------------------------------------------
		case 0x02:	//	DTC that caused freeze frame to be stored. 
		//------------------------------------------------------
			obd2_ret.SAE_ECU.VAL[0] = 0x00;
			obd2_ret.SAE_ECU.VAL[1] = 0x00;
			obd2_ret.SAE_ECU.VAL[2] = 0x00;
			obd2_ret.SAE_ECU.VAL[3] = 0x00;
			break;
		}
	}
	else
	{
		return 0;
	}
	return len;
}

//----------------------------------------------------------------------------------------
//	MODE3	����(�ۑ��ς�DTC���R�[�h��ԐM����)
//----------------------------------------------------------------------------------------
int obd2_mode3(int len)
{
	if(len == 2)
	{	//	�X�^���_�[�h�v��
		len = 7;
		obd2_ret.MD3_ECU.MODE = obd2_req.SAE_OBD.MODE + 0x40;
		obd2_ret.MD3_ECU.VAL[0] = 0x00;
		obd2_ret.MD3_ECU.VAL[1] = 0x00;
		obd2_ret.MD3_ECU.VAL[2] = 0x00;
		obd2_ret.MD3_ECU.VAL[3] = 0x00;
		obd2_ret.MD3_ECU.VAL[4] = 0x00;
		obd2_ret.MD3_ECU.VAL[5] = 0x00;
	}
	else
	{
		return 0;
	}
	return len;
}

//----------------------------------------------------------------------------------------
//	MODE4	����(DTC���R�[�h����������)
//----------------------------------------------------------------------------------------
int obd2_mode4(int len)
{
	if(len == 2)
	{	//	�X�^���_�[�h�v��
		len = 1;
		obd2_ret.MD3_ECU.MODE = obd2_req.SAE_OBD.MODE + 0x40;
	}
	else
	{
		return 0;
	}
	return len;
}

//----------------------------------------------------------------------------------------
//	MODE5	����
//----------------------------------------------------------------------------------------
int obd2_mode5(int len)
{
	if(len == 3)
	{	//	�X�^���_�[�h�v��
		len = 6;
		obd2_ret.VS_ECU.MODE = obd2_req.VS_OBD.MODE + 0x40;
		obd2_ret.VS_ECU.PIDH = obd2_req.VS_OBD.PIDH;
		obd2_ret.VS_ECU.PIDL = obd2_req.VS_OBD.PIDL;
		switch(obd2_req.VS_OBD.PIDH)
		{
		case 0x01:	//	
			switch(obd2_req.VS_OBD.PIDL)
			{
			//------------------------------------------------------
			case 0x00:	//	PIDs supported [01 - 20]
			//------------------------------------------------------
				obd2_ret.VS_ECU.VAL[0] = 0x00;
				obd2_ret.VS_ECU.VAL[1] = 0x00;
				obd2_ret.VS_ECU.VAL[2] = 0x00;
				obd2_ret.VS_ECU.VAL[3] = 0x00;
				break;
			case 0x01:	//	O2 Sensor Monitor Bank 1 Sensor 1
				break;
			case 0x02:	//	O2 Sensor Monitor Bank 1 Sensor 2
				break;
			case 0x03:	//	O2 Sensor Monitor Bank 1 Sensor 3
				break;
			case 0x04:	//	O2 Sensor Monitor Bank 1 Sensor 4
				break;
			case 0x05:	//	O2 Sensor Monitor Bank 2 Sensor 1
				break;
			case 0x06:	//	O2 Sensor Monitor Bank 2 Sensor 2
				break;
			case 0x07:	//	O2 Sensor Monitor Bank 2 Sensor 3
				break;
			case 0x08:	//	O2 Sensor Monitor Bank 2 Sensor 4
				break;
			case 0x09:	//	O2 Sensor Monitor Bank 3 Sensor 1
				break;
			case 0x0A:	//	O2 Sensor Monitor Bank 3 Sensor 2
				break;
			case 0x0B:	//	O2 Sensor Monitor Bank 3 Sensor 3
				break;
			case 0x0C:	//	O2 Sensor Monitor Bank 3 Sensor 4
				break;
			case 0x0D:	//	O2 Sensor Monitor Bank 4 Sensor 1
				break;
			case 0x0E:	//	O2 Sensor Monitor Bank 4 Sensor 2
				break;
			case 0x0F:	//	O2 Sensor Monitor Bank 4 Sensor 3
				break;
			case 0x10:	//	O2 Sensor Monitor Bank 4 Sensor 4
				break;
			}
			break;
		case 0x02:	//	
			switch(obd2_req.VS_OBD.PIDL)
			{
			case 0x01:	//	O2 Sensor Monitor Bank 1 Sensor 1
				break;
			case 0x02:	//	O2 Sensor Monitor Bank 1 Sensor 2
				break;
			case 0x03:	//	O2 Sensor Monitor Bank 1 Sensor 3
				break;
			case 0x04:	//	O2 Sensor Monitor Bank 1 Sensor 4
				break;
			case 0x05:	//	O2 Sensor Monitor Bank 2 Sensor 1
				break;
			case 0x06:	//	O2 Sensor Monitor Bank 2 Sensor 2
				break;
			case 0x07:	//	O2 Sensor Monitor Bank 2 Sensor 3
				break;
			case 0x08:	//	O2 Sensor Monitor Bank 2 Sensor 4
				break;
			case 0x09:	//	O2 Sensor Monitor Bank 3 Sensor 1
				break;
			case 0x0A:	//	O2 Sensor Monitor Bank 3 Sensor 2
				break;
			case 0x0B:	//	O2 Sensor Monitor Bank 3 Sensor 3
				break;
			case 0x0C:	//	O2 Sensor Monitor Bank 3 Sensor 4
				break;
			case 0x0D:	//	O2 Sensor Monitor Bank 4 Sensor 1
				break;
			case 0x0E:	//	O2 Sensor Monitor Bank 4 Sensor 2
				break;
			case 0x0F:	//	O2 Sensor Monitor Bank 4 Sensor 3
				break;
			case 0x10:	//	O2 Sensor Monitor Bank 4 Sensor 4
				break;
			}
			break;
		}
	}
	else
	{
		return 0;
	}
	return len;
}

//----------------------------------------------------------------------------------------
//	MODE6	�������ʁiCAN�̂ݎ_�f�Z���T�[�Ď��j
//----------------------------------------------------------------------------------------
int obd2_mode6(int len)
{
	if(len == 2)
	{	//	�X�^���_�[�h�v��
		len = 1;
		obd2_ret.MD3_ECU.MODE = obd2_req.SAE_OBD.MODE + 0x40;
	}
	else
	{
		return 0;
	}
	return len;
}

//----------------------------------------------------------------------------------------
//	MODE7	���m�̐f�f�g���u���R�[�h���擾
//----------------------------------------------------------------------------------------
int obd2_mode7(int len)
{
	if(len == 2)
	{	//	�X�^���_�[�h�v��
		len = 1;
		obd2_ret.MD3_ECU.MODE = obd2_req.SAE_OBD.MODE + 0x40;
	}
	else
	{
		return 0;
	}
	return len;
}

//----------------------------------------------------------------------------------------
//	MODE8	�I���{�[�h�V�X�e���x��
//----------------------------------------------------------------------------------------
int obd2_mode8(int len)
{
	if(len == 2)
	{	//	�X�^���_�[�h�v��
		len = 1;
		obd2_ret.MD3_ECU.MODE = obd2_req.SAE_OBD.MODE + 0x40;
	}
	else
	{
		return 0;
	}
	return len;
}

//----------------------------------------------------------------------------------------
//	MODE9	����
//----------------------------------------------------------------------------------------
int obd2_mode9(int len)
{
	if(len >= 2)
	{	//	�X�^���_�[�h�v��
		len = 2;
		obd2_ret.SAE_ECU.MODE = obd2_req.SAE_OBD.MODE + 0x40;
		obd2_ret.SAE_ECU.PID = obd2_req.SAE_OBD.PID;
		switch(obd2_req.SAE_OBD.PID)
		{
		//------------------------------------------------------
		case 0x00:	//	Mode 9 supported PIDs (01 to 20)
		//------------------------------------------------------
			len += 4;
			obd2_ret.SAE_ECU.VAL[0] = 0x00;
			obd2_ret.SAE_ECU.VAL[1] = 0xC0;
			obd2_ret.SAE_ECU.VAL[2] = 0x00;
			obd2_ret.SAE_ECU.VAL[3] = 0x00;
			break;
		case 0x01:	//	VIN Message Count in PID 02. Only for ISO 9141-2, ISO 14230-4 and SAE J1850.
			break;
		case 0x02:	//	Vehicle Identification Number (VIN)
			break;
		case 0x03:	//	Calibration ID message count for PID 04. Only for ISO 9141-2, ISO 14230-4 and SAE J1850.
			break;
		case 0x04:	//	Calibration ID
			break;
		case 0x05:	//	Calibration verification numbers (CVN) message count for PID 06. Only for ISO 9141-2, ISO 14230-4 and SAE J1850.
			break;
		case 0x06:	//	Calibration Verification Numbers (CVN)
			break;
		case 0x07:	//	In-use performance tracking message count for PID 08 and 0B. Only for ISO 9141-2, ISO 14230-4 and SAE J1850.
			break;
		case 0x08:	//	In-use performance tracking for spark ignition vehicles
			len += 4;
			switch(obd2_ret_counter)
			{
			case 0:	//	OBDCOND, IGNCNTR
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			case 1:	//	HCCATCOMP, HCCATCOND
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			case 2:	//	NCATCOMP, NCATCOND
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			case 3:	//	NADSCOMP, NADSCOND
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			case 4:	//	PMCOMP, PMCOND
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			case 5:	//	EGSCOMP, EGSCOND
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			case 6:	//	EGRCOMP, EGRCOND
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			case 7:	//	BPCOMP, BPCOND
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			case 8:	//	FUELCOMP, FUELCOND
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			}
			obd2_ret_counter++;
			if(obd2_ret_counter >= 9) obd2_ret_counter = 0;
			break;
		case 0x09:	//	ECU name message count for PID 0A
			len += 1;
			obd2_ret.SAE_ECU.VAL[0] = 5;	//(A)	ECU���̕�����
			break;
		case 0x0A:	//	ECU name
			len += 5;
			switch(SELECT_ECU_UNIT)
			{
			case 0:	//(A)	ECU���̕���
				obd2_ret.SAE_ECU.VAL[0] = 'P';
				obd2_ret.SAE_ECU.VAL[1] = 'O';
				obd2_ret.SAE_ECU.VAL[2] = 'W';
				obd2_ret.SAE_ECU.VAL[3] = 'E';
				break;
			case 1:	//(A)	ECU���̕���
				obd2_ret.SAE_ECU.VAL[0] = 'C';
				obd2_ret.SAE_ECU.VAL[1] = 'H';
				obd2_ret.SAE_ECU.VAL[2] = 'A';
				obd2_ret.SAE_ECU.VAL[3] = 'S';
				break;
			case 2:	//(A)	ECU���̕���
				obd2_ret.SAE_ECU.VAL[0] = 'B';
				obd2_ret.SAE_ECU.VAL[1] = 'O';
				obd2_ret.SAE_ECU.VAL[2] = 'D';
				obd2_ret.SAE_ECU.VAL[3] = 'Y';
				break;
			case 3:	//(A)	ECU���̕���
				obd2_ret.SAE_ECU.VAL[0] = 'E';
				obd2_ret.SAE_ECU.VAL[1] = 'C';
				obd2_ret.SAE_ECU.VAL[2] = 'U';
				obd2_ret.SAE_ECU.VAL[3] = '3';
				break;
			case 4:	//(A)	ECU���̕���
				obd2_ret.SAE_ECU.VAL[0] = 'E';
				obd2_ret.SAE_ECU.VAL[1] = 'C';
				obd2_ret.SAE_ECU.VAL[2] = 'U';
				obd2_ret.SAE_ECU.VAL[3] = '4';
				break;
			case 5:	//(A)	ECU���̕���
				obd2_ret.SAE_ECU.VAL[0] = 'E';
				obd2_ret.SAE_ECU.VAL[1] = 'C';
				obd2_ret.SAE_ECU.VAL[2] = 'U';
				obd2_ret.SAE_ECU.VAL[3] = '5';
				break;
			case 6:	//(A)	ECU���̕���
				obd2_ret.SAE_ECU.VAL[0] = 'E';
				obd2_ret.SAE_ECU.VAL[1] = 'C';
				obd2_ret.SAE_ECU.VAL[2] = 'U';
				obd2_ret.SAE_ECU.VAL[3] = '6';
				break;
			case 7:	//(A)	ECU���̕���
				obd2_ret.SAE_ECU.VAL[0] = 'C';
				obd2_ret.SAE_ECU.VAL[1] = 'G';
				obd2_ret.SAE_ECU.VAL[2] = 'W';
				obd2_ret.SAE_ECU.VAL[3] = '1';
				break;
			default:	//(A)	ECU���̕���
				obd2_ret.SAE_ECU.VAL[0] = '?';
				obd2_ret.SAE_ECU.VAL[1] = '?';
				obd2_ret.SAE_ECU.VAL[2] = '?';
				obd2_ret.SAE_ECU.VAL[3] = '?';
				break;
			}
			break;
		case 0x0B:	//	In-use performance tracking for compression ignition vehicles
			len += 4;
			switch(obd2_ret_counter)
			{
			case 0:	//	OBDCOND, IGNCNTR
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			case 1:	//	HCCATCOMP, HCCATCOND
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			case 2:	//	NCATCOMP, NCATCOND
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			case 3:	//	NADSCOMP, NADSCOND
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			case 4:	//	PMCOMP, PMCOND
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			case 5:	//	EGSCOMP, EGSCOND
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			case 6:	//	EGRCOMP, EGRCOND
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			case 7:	//	BPCOMP, BPCOND
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			case 8:	//	FUELCOMP, FUELCOND
			default:
				obd2_ret.SAE_ECU.VAL[0] = 0x00;
				obd2_ret.SAE_ECU.VAL[1] = 0x00;
				obd2_ret.SAE_ECU.VAL[2] = 0x00;
				obd2_ret.SAE_ECU.VAL[3] = 0x00;
				break;
			}
			obd2_ret_counter++;
			if(obd2_ret_counter >= 9) obd2_ret_counter = 0;
			break;
		}
	}
	else
	{
		return 0;
	}
	return len;
}

//----------------------------------------------------------------------------------------
//	MODE10	�i��DTC���
//----------------------------------------------------------------------------------------
int obd2_modeA(int len)
{
	if(len == 2)
	{	//	�X�^���_�[�h�v��
		len = 1;
		obd2_ret.MD3_ECU.MODE = obd2_req.SAE_OBD.MODE + 0x40;
	}
	else
	{
		return 0;
	}
	return len;
}

//----------------------------------------------------------------------------------------
//	OBD2����
//----------------------------------------------------------------------------------------
int obd2_job(unsigned char *msg, int len, unsigned char *res)	//int ch, int id, void *frame)
{
	memset(&obd2_ret, 0x00, sizeof(obd2_ret));
	memset(&obd2_req, 0x00, sizeof(obd2_req));
	memcpy(&obd2_req.BYTE[0], msg, len);
	switch(obd2_req.SAE_OBD.MODE)
	{
	case SHOW_CURRENT_DATA:		//	���ݒl
		len = obd2_mode1(len);
		break;
	case SHOW_FREEZE_FDATA:		//	��~�t���[��
		len = obd2_mode2(len);
		break;
	case SHOW_STORED_DTC:		//	�ۑ�DTC�擾
		len = obd2_mode3(len);
		break;
	case CLEAR_STORED_DTC:		//	�ۑ�DTC����
		len = obd2_mode4(len);
		break;
	case TEST_RESULT_NON_CAN:	//	�r�C�Ď�(NON-CAN)
		len = obd2_mode5(len);
		break;
	case TEST_RESULT_ONLY_CAN:	//	���̑��̃��j�^�A�r�C�Ď�(CAN)
		len = obd2_mode6(len);
		break;
	case SHOW_PENDING_DTC:		//	�ŏIDTC���
		len = obd2_mode7(len);
		break;
	case CTRL_OPERATION_SYS:	//	����n����
		len = obd2_mode8(len);
		break;
	case REQUEST_VEHICLE_INFO:	//	�ԗ����
		len = obd2_mode9(len);
		break;
	case PERMANENT_DTC:			//	�i�vDTC���
		len = obd2_modeA(len);
		break;
	default:	//	���Ή����[�h
		len = 3;
		obd2_ret.NOT_ECU.X7F = 0x7F;
		obd2_ret.NOT_ECU.MODE = obd2_req.SAE_OBD.MODE;
		obd2_ret.NOT_ECU.X31 = 0x31;
		break;
	}
	if(len > 0)
	{	//	�����ԐM
		memcpy(&res[0], &obd2_ret.BYTE[0], len);
		return len;
	//	sw += 8;
	//	memcpy(&can_buf.ID[sw], obd2_ret.BYTE, 8);
	//	if(ch >= 0) add_mbox_frame(ch, 8, CAN_DATA_FRAME, sw);	//	���M�҂��o�b�t�@�ςݏグ
	//	return sw;	//	�]������
	}
	return 0;
}



