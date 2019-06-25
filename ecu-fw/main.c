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
//  CAN2ECU ���C������
//
//----------------------------------------------------------------------------------------
//  �J������
//
//  2017/12/10  �R�[�f�B���O�J�n�i�k�j
//
//----------------------------------------------------------------------------------------
//  T.Tachibana
//  ��L&F
//________________________________________________________________________________________
//

#include	<sysio.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"altypes.h"
#include	"iodefine.h"
#include	"sci.h"
#include	"rtc.h"
#include	"timer.h"
#include	"ecu.h"
#include	"flash_data.h"
#include	"flash_rom.h"
#include	"r_Flash_API_RX600.h"
#include	"usb.h"
#include	"can3_spi2.h"
#include	"uSD_rspi1.h"
#include	"cantp.h"			/*	CAN-TP ��`				*/
#include	"uds.h"				/*	CAN-UDS ��`			*/


//________________________________________________________________________________________
//
//  CAN2ECU Main	�ϐ���`
//________________________________________________________________________________________
//
#define		COMMAND_BUF_MAX		512
typedef	struct	__console_command_buffer__	{
	int		WP;
	char	BUF[COMMAND_BUF_MAX];
}	CONSOLE_CTRL;

CONSOLE_CTRL	sci_console;
#ifdef	SCI2_ACTIVATE
CONSOLE_CTRL	sci2_console;
#endif
#if	defined(USB_ACTIVATE) && defined(__LFY_RX63N__)
CONSOLE_CTRL	usb_console;
#endif
int		retport = 0;
#ifdef	__LFY_RX63N__
int		console_port = 1;
#else
int		console_port = 0;
#endif

const char	def_ecu_corp[16] = "TOYOTA-ITC";
const char	def_ecu_name[16] = "CAN2ECU";
const char	def_ecu_vars[16] = "Ver2.4.1";
const char	def_ecu_date[16] = __DATE__;
const char	def_ecu_time[16] = __TIME__;

//	�ʐM�ꎞ�ۊǃf�[�^�o�b�t�@
#define		RAM_BUFFER_MAX		128
unsigned char	comm_ram_buffer[RAM_BUFFER_MAX];

//  ���o�^���荞�ݏ���
interrupt void Dummy_Interrupt(void)
{
}

//	ECU����
void ecu_job(void);					//	ECU�^�]
void ecu_status(char *cmd);			//	�p�����[�^��Ԋm�F
void ecu_get_command(char *cmd);	//	�t���[���f�[�^�擾
void ecu_set_command(char *cmd);	//	�t���[���f�[�^��������
void ecu_put_command(char *cmd);	//	�t���[�����ڑ��M
void ecu_input_update(char *cmd);	//	�ʐM�o�RI/O���X�V

//________________________________________________________________________________________
//
//	iwdt_refresh	�E�H�b�`�h�b�N�̃��t���b�V��
//----------------------------------------------------------------------------------------
//	����
//		����
//	�߂�
//		����
//________________________________________________________________________________________
//
void iwdt_refresh(void)
{
	unsigned short	cnt = IWDT.IWDTSR.WORD;
	if((cnt & 0xC000) != 0 || cnt == 0) return;	//	���Z�b�g�����ς݂Ɩ������͖�������
	if(cnt > 0x2FFF) return;					//	3FFF �� 75% �ȏ�͔͈͊O
	//	IWDT ���t���b�V��
	IWDT.IWDTRR = 0x00;
	IWDT.IWDTRR = 0xFF;
}

//________________________________________________________________________________________
//
//	wdt_init	�E�H�b�`�h�b�N�ɂ��ċN�����d�|����
//----------------------------------------------------------------------------------------
//	����
//		����
//	�߂�
//		����
//________________________________________________________________________________________
//
void wdt_init(void)
{
	WDT.WDTCR.BIT.TOPS = 3;		//	�^�C���A�E�g���� 0:1024  1:4096  2:8192  3:16384
	WDT.WDTCR.BIT.CKS = 8;		//	�N���b�N�����@1:PCLK/4  4:PCLK/64  16:PCLK/128  6:PCLK/512 7:PCLK/2048  8:PCLK/8192
	WDT.WDTCR.BIT.RPES = 3;		//	�E�B���h�E�I���ʒu�@0:75% 1:50% 2:75% 3:100%
	WDT.WDTCR.BIT.RPSS = 3;		//	�E�B���h�E�J�n�ʒu�@0:75% 1:50% 2:75% 3:100%
	WDT.WDTRR = 0x00;			//	���W�X�^�X�^�[�g1
	WDT.WDTRR = 0xff;			//	���W�X�^�X�^�[�g2
}

//________________________________________________________________________________________
//
//	hex_to_byte
//----------------------------------------------------------------------------------------
//	����
//		*p	  HEX������
//		*d	  �o�C�g��i�[��
//	�߂�
//		int  �ϊ��o�C�g��
//________________________________________________________________________________________
//
int hex_to_byte(char *p, unsigned char *d)
{
	int  i = 0;
	char	c;
	
	while(*p != 0 && *(p + 1) != 0 && i < 32)
	{
		c = *p++;
		if((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
		{
			if(c > '9') c -= 7;
			d[i] = ((int)c << 4) & 0xF0;
			c = *p++;
			if((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
			{
				if(c > '9') c -= 7;
				d[i] |= c & 0x0F;
				i++;
			}
			else break;
		}
		else break;
	}
	return i;
}

//________________________________________________________________________________________
//
//	byte_to_ulong
//----------------------------------------------------------------------------------------
//	����
//		unsigned char *data	�o�C�g��(�r�b�O�G���f�B�A��)
//		int index			�J�n�ʒu
//		int size			�Q�ƃo�C�g��
//	�߂�
//		unsigned long		�擾�l
//________________________________________________________________________________________
//
unsigned long   byte_to_ulong(unsigned char *data, int index, int size)
{
	int		  i;
	unsigned long   d = 0;
	unsigned char   *p = data + index;

	for(i = 0; i < size; i++)
	{
		d <<= 8;
		d |= ((unsigned long)p[i] & 0x00FF);
	}
	return d;
}

//________________________________________________________________________________________
//
//  send_var
//----------------------------------------------------------------------------------------
//  �@�\����
//	  ���u����COM�|�[�g�֑��M����
//  ����
//	  ch		���M��COM�`�����l���ԍ�
//  �߂�
//	  ����
//________________________________________________________________________________________
//
void send_var(int ch)
{
	char	s[256];
//	sprintf(s, "%s %s %s %s %s\r", def_ecu_corp, def_ecu_name, def_ecu_vars, def_ecu_date, def_ecu_time);
	sprintf(s, "%s %s %s %s\r", def_ecu_name, def_ecu_vars, def_ecu_date, def_ecu_time);
	sci_puts(ch, s);
}

//________________________________________________________________________________________
//
//  SendPC
//----------------------------------------------------------------------------------------
//  �@�\����
//	  ��M�R�}���h��ԐM
//  ����
//	  *msg		�ԐM�d��
//  �߂�
//	  ����
//________________________________________________________________________________________
//
void SendPC(char *msg)
{
	switch(retport)
	{
		case 0: //  COM0
#ifdef	SCI0_ACTIVATE
			sci_puts(0, msg);
			break;
#endif
		case 1: //  COM1
#if	defined(SCI1_ACTIVATE) || defined(__LFY_RX63N__)
			sci_puts(1, msg);
			break;
#endif
		case 2: //  COM2
#ifdef	SCI2_ACTIVATE
			sci_puts(2, msg);
			break;
#endif
		case 3: //  COM3
#ifdef	SCI3_ACTIVATE
			sci_puts(3, msg);
			break;
#endif
		case 4: //  USB
#if	defined(USB_ACTIVATE) && defined(__LFY_RX63N__)
			usb_puts(msg);
			usb_flush();
#endif
			break;
	}
}

//________________________________________________________________________________________
//
//	�֐���		���O�o��
//----------------------------------------------------------------------------------------
//	����		void	logging(char *fmt, ...)
//	����		bp=��Ɨp�o�b�t�@�|�C���^
//				fmt=�`���t�o��
//	����		printf�֐��Ɠ��l�̏����w��\�ȃ��O�o�͂̃N���C�A���g�B
//	�߂�l		����
//________________________________________________________________________________________
//
void	logging(char *fmt, ...)
{
	va_list		args;
	int			n;
	char		bp[256];

	va_start(args, fmt);
	strcpy(bp, "");
	vsprintf(bp, fmt, args);
	va_end(args);
	SendPC(bp);
}

//________________________________________________________________________________________
//
//  PortInit
//----------------------------------------------------------------------------------------
//  ����
//      ����
//  �߂�
//      ����
//________________________________________________________________________________________
//
void    PortInit(void)
{
	unsigned char   d;

	SYSTEM.PRCR.WORD = 0xA502;      //  ���샂�[�h�A����d�͒ጸ�@�\�֘A���W�X�^�v���e�N�g����
	SYSTEM.SYSCR0.WORD = 0x5A01;	//	����ROM�L���A�O���o�X����
	while((SYSTEM.SYSCR0.WORD & 3) != 1);

//  MSTP_RIIC0 = 0;                 //  I2C0        �X�g�b�v��ԉ���
//  MSTP_SCI0 = 0;                  //  SCI0        �X�g�b�v��ԉ���
//  MSTP_SCI1 = 0;                  //  SCI1        �X�g�b�v��ԉ�����YellowREM-MON�Ƌ���
//  MSTP_TPU0 = 0;                  //  TPU0..5  �X�g�b�v��ԉ���
//  MSTP_MTU = 0;                   //  MTU0..5  �X�g�b�v��ԉ���
//  MSTP_S12AD = 0;                 //  AN000..003  �X�g�b�v��ԉ���

	MPC.PWPR.BIT.B0WI = 0;          //  �n�߂�B0WI��0������
	MPC.PWPR.BIT.PFSWE = 1;         //  PFS���W�X�^�ւ̏������݂�����

	MPC.PFAOE0.BYTE = 0x00;
	MPC.PFAOE1.BYTE = 0x00;

	memset(&sci_console, 0, sizeof(CONSOLE_CTRL));
#ifdef	SCI2_ACTIVATE
	memset(&sci2_console, 0, sizeof(CONSOLE_CTRL));
#endif
#if	defined(USB_ACTIVATE) && defined(__LFY_RX63N__)
	memset(&usb_console, 0, sizeof(CONSOLE_CTRL));
#endif
//	TransitSuperMode();
	WriteINTB(0);
//	TransitUserMode();
}

//________________________________________________________________________________________
//
//  RTS �R�}���h����
//----------------------------------------------------------------------------------------
//  ����
//      char *cmd	�R�}���h������
//  �߂�
//      ����
//________________________________________________________________________________________
//
void rtc_command_job(char *cmd)
{
	int			year, mon, day, hour, min, sec;
	time_bcd_t	tm;
	//---------------------------------------------------
	//  �R�}���h���
	//---------------------------------------------------
	if( strlen(cmd) == 0 )
	{   // RTC�f�[�^���[�h
		rtc_time_read(&tm);
		logging("RTC=%04X/%02X/%02X %02X:%02X:%02X\r"
			,(int)tm.year, (int)tm.month , (int)tm.day, (int)tm.hour, (int)tm.minute,(int)tm.second );
	}
	else
	if( strlen(cmd) >= 17 )
	{   // RTC�f�[�^�Z�b�g
		if(strlen(cmd) == 17)
		{
			if(6 != sscanf(&cmd[3],"%02X/%02X/%02X %02X:%02X:%02X",&year ,&mon ,&day ,&hour ,&min ,&sec)) return;
		}
		else
		{
			if(6 != sscanf(&cmd[3],"%04X/%02X/%02X %02X:%02X:%02X",&year ,&mon ,&day ,&hour ,&min ,&sec)) return;
		}
		tm.year = 0x00ff & (char)year;
		tm.month = (char)mon;
		tm.day = (char)day;
		tm.hour = (char)hour;
		tm.minute = (char)min;
		tm.second = (char)sec;
		rtc_init(&tm);
		rtc_time_read(&tm);
		logging("RTC=%04X/%02X/%02X %02X:%02X:%02X\r"
			,(int)tm.year, (int)tm.month , (int)tm.day, (int)tm.hour, (int)tm.minute,(int)tm.second );
	}
}

//________________________________________________________________________________________
//
//  SCI/USB �R�}���h��M����
//----------------------------------------------------------------------------------------
//  ����
//      ����
//  �߂�
//      ����
//________________________________________________________________________________________
//
void command_job(char *cmd)
{
	int		id, dt;
	int		db[8];
	char	*p;
	//---------------------------------------------------
	//  �R�}���h���
	//---------------------------------------------------
	switch(*cmd++)
	{
	case '?':	//	�f�o�C�X�R�[�h
		switch(SELECT_ECU_UNIT)
		{
		case ECU_UNIT_POWERTRAIN:
			logging("ECUPT\r");
			break;
		case ECU_UNIT_CHASSIS:
			logging("ECUIP\r");
			break;
		case ECU_UNIT_BODY:
			logging("ECUBD\r");
			break;
		case ECU_UNIT_CGW:
			logging("ECUGW\r");
			break;
		default:
			logging("ECUX%d\r", (int)(SELECT_ECU_UNIT));
			break;
		}
		break;
	case 'B':	//	ROM����R�}���h
		if(strncmp(cmd, "OOTCOPY", 7) == 0)
		{	//	���s���̃v���O������ROM�ɃR�s�[����
			if(bootcopy() == 1)
			{	//	����
				logging("BootCopy OK\r");
			}
			else
			{	//	���s
				logging("BootCopy NG\r");
			}
		}
		else
		if(strncmp(cmd, "OOTCLEAR", 8) == 0)
		{	//	ROM�ۑ��̈���N���A����
			if(bootclear() == 1)
			{	//	����
				logging("BootClear OK\r");
			}
			else
			{	//	���s
				logging("BootClear NG\r");
			}
		}
		break;
	case 'C':	//	�����E�C�x���g���X�g
		switch(*cmd++)
		{
		case 'C':	//	���X�g��S�č폜
			if(cmd[0] == 'A' && cmd[1] == 'L' && cmd[2] == 'L')
			{	//	[CCALL]�R�}���h
				//	�[��������
				memset(&wait_tup, 0, sizeof(wait_tup));			//	�����E�C�x���g�҂�������
				wait_tup.TOP = -1;
				memset(&conf_ecu, 0, sizeof(conf_ecu));			//	�����E�C�x���g�E�����[�g�Ǘ���`������
				conf_ecu.TOP = -1;
				logging("CCALL OK\r");
			}
			break;
		case 'R':	//	���X�g����ID����
			while(*cmd == ' ') cmd++;
			if(sscanf(cmd, "%x", &id) == 1)
			{	//	�ݒ�l�擾	[CR id]
				if(id >= 0 && id < CAN_ID_MAX)
				{
					delete_cyceve_list(id);		//	�Ǘ�����ID���폜
					delete_waiting_list(id);	//	���ԑ҂�ID���폜
					logging("CR %03X OK\r", id);
				}
			}
			break;
		case 'A':	//	���X�g��ID�ǉ�
			while(*cmd == ' ') cmd++;
			if(sscanf(cmd, "%x %d %d %d %d %d %d",&id, &db[0], &db[1], &db[2], &db[3], &db[4], &db[5]) == 7)
			{	//	�ݒ�l�擾	[CA id rtr dlc enb rep time cnt]
				if(id >= 0 && id < CAN_ID_MAX)
				{
					dt = add_cyceve_list(db[0], id, db[1], db[2], db[3], db[4], db[5]);
					can_id_event(dt, 0);		//	�C�x���g�o�^
					logging("CA %03X OK\r", id);
				}
			}
			break;
		}
		break;
	case 'M':
		if(cmd[0] == 'A')
		{	//	MA�`
			if(cmd[1] == 'P')
			{	//	MAP�R�}���h
				cmd += 2;
				while(*cmd == ' ') cmd++;
				if(sscanf(cmd, "%x %x", &id, &dt) == 2)
				{	//	�ݒ�l�擾
					if(id >= 0 && id < CAN_ID_MAX)
					{
						rout_map.ID[id].BYTE = (unsigned char)dt;
					}
				}
			}
		}
		else
		if(cmd[0] == 'O' && cmd[1] == 'N')
		{	//	MON�R�}���h(�w��ID�̎�M���瑗�M�����܂ł̎����𓾂�)
			cmd += 2;
			while(*cmd == ' ') cmd++;
			db[0] = sscanf(cmd, "%x %x %d", &id, &dt, &db[1]);
			if(db[0] >= 2)
			{	//	�ݒ�l�擾
				cmt1_stop();
				led_monit_id = id;				//	����ID
				led_monit_ch = dt;				//	�����`�����l��
				led_monit_first = 0x7FFFFFFF;	//	�ŒZ����
				led_monit_slow = 0;				//	�Œ�����
				led_monit_time = 0;				//	���ώ���
				led_monit_count = 0;			//	���ω���
				led_monit_sample = (db[0] == 3) ? db[1] : 50;
			}
		}
		break;
	case 'E':	//	ECU�R�}���h
		if(cmd[0] == 'X' && cmd[1] == 'D')
		{	//	EXD	�ʐM�o�RI/O���̓R�}���h
			ecu_input_update(&cmd[2]);
		}
		else
		{	//	�������ԐM�v��
			ecu_status(cmd);
		}
		break;
	case 'G':	//	ECU�w��ID�t���[���擾�R�}���h
		if(cmd[0] == 'E' && cmd[1] == 'T')
		{
			ecu_get_command(&cmd[2]);
		}
		break;
	case 'S':	//	ECU�w��ID�t���[���ݒ�R�}���h
		if(cmd[0] == 'E' && cmd[1] == 'T')
		{
			ecu_set_command(&cmd[2]);
		}
		break;
	case 'P':	//	ECU�w��ID�t���[�����M�R�}���h
		if(cmd[0] == 'U' && cmd[1] == 'T')
		{
			ecu_put_command(&cmd[2]);
		}
		break;
	case 'R':	//	R
		if(strncmp(cmd, "EBOOT", 5) == 0)
		{	//	�\�t�g���Z�b�g�ɂ��ċN��
			logging("ReBoot OK\r");
			wdt_init();
		}
		else
		if(cmd[0] == 'T' && cmd[1] == 'C')
		{	//	[RTC]���A���^�C���N���b�N����R�}���h
			while(*cmd == ' ') cmd++;
			rtc_command_job(cmd);
		}
		else
		if(cmd[0] == 'B' && cmd[1] == 'U')
		{	//	[RBU]RAM�ςݏグ�R�}���h	RBU pointer length data.. 1��ōő�32�o�C�g
			while(*cmd == ' ') cmd++;
			db[0] = 0;	//	�����J�E���^
			db[1] = 0;	//	�|�C���^
			db[2] = 0;	//	����
			while(*cmd != 0)
			{
				p = cmd;
				while(*cmd != ' ' && *cmd != 0) cmd++;
				switch(db[0])
				{
				case 0:	//	�|�C���^�ݒ�
					if(sscanf(p, "%d", &db[1]) != 1)
					{
						logging("RBU Error 1\r");
						return;
					}
					if(db[1] < 0 || db[1] >= RAM_BUFFER_MAX)
					{
						logging("RBU Over 1\r");
						return;
					}
					break;
				case 1:	//	�����ݒ�
					if(sscanf(p, "%d", &db[2]) != 1)
					{
						logging("RBU Error 2\r");
						return;
					}
					if((db[1] + db[2]) >= RAM_BUFFER_MAX)
					{
						logging("RBU Over 2\r");
						return;
					}
					break;
				default:	//	�������݃f�[�^
					if(sscanf(p, "%x", &db[3]) != 1)
					{
						logging("RBU Error %d\r", db[0]);
						return;
					}
					if((db[0] - 2 + db[1]) < RAM_BUFFER_MAX)
					{	//	�o�b�t�@�͈͓�
						comm_ram_buffer[(db[0] - 2 + db[1])] = (unsigned char)db[3];
					}
					break;
				}
				db[0]++;
				if((db[0] - 2) >= db[2] || (db[0] - 2 + db[1]) >= RAM_BUFFER_MAX)
				{
					break;
				}
			}
			if((db[0] - 2) < db[2])
			{	//	�\��f�[�^���s��
				logging("RBU Lost %d\r", db[0]);
				return;
			}
		}
		else
		if(cmd[0] == 'W' && cmd[1] == 'L')
		{	//	[RWL]ROM�������݃R�}���h	RWL address length
			while(*cmd == ' ') cmd++;
			if(sscanf(cmd, "%x %d", &db[0], &db[1]) == 2)
			{	//	�p�����[�^����v
				_di();
				if(R_FlashWrite((unsigned long)db[0], (unsigned long)&comm_ram_buffer, (unsigned short)db[1]) != FLASH_SUCCESS)
				{	//	�������ݎ��s
					_ei();
					logging("RWL Error\r");
					return;
				}
				_ei();
				logging("RWL Success %X\r", db[0]);
			}
		}
		break;
	case 'W':	//	�f�[�^�t���b�V���֕ۑ�
		if(cmd[0] == 'D' && cmd[1] == 'F')
		{	//	[WDF]�R�}���h
			id = ecu_data_write();
			if(id == 7)
			{	//	�ۑ�����
				logging("WDF OK\r");
			}
			else
			{	//	�ۑ����s
				logging("WDF NG %d\r", id);
			}
		}
		break;
	default:
		logging("Command Error !\r");
		break;
	}
}

//________________________________________________________________________________________
//
//  SCI/USB �R�}���h��M����
//----------------------------------------------------------------------------------------
//  ����
//      ����
//  �߂�
//      ����
//________________________________________________________________________________________
//
void comm_job(void)
{
	int			i, n;
	int			numBytes;
	int			year, mon, day, hour, min, sec;
	time_bcd_t	tm;
	unsigned short d;
	char		buffer[64];
	char		c;

	//  RS-232C
	numBytes = sci_get_check(console_port);
	if(numBytes != 0)
	{   //  COM1��M����
		for(i = 0; i < numBytes; i++)
		{
			c = sci_get_char(console_port);
		//	sci_putc(console_port, c); //  �G�R�[�o�b�N
			if(c == 0x0D)
			{   //  [CR]
				sci_console.BUF[sci_console.WP] = 0;
				sci_console.WP = 0;
				retport = console_port;
				command_job(sci_console.BUF);
			}
			else
			if(c == 0x08 || c == 0x7F)
			{	//	1�����폜
				if(sci_console.WP > 0) sci_console.WP--;
			}
			else
			{
				if(c >= 0x61 && c <= 0x7a)
				{   // �������͑啶���ɕϊ�����
					sci_console.BUF[sci_console.WP] = c - 0x20;
				}
				else
				{
					sci_console.BUF[sci_console.WP] = c;
				}
				sci_console.WP++;
				if(sci_console.WP >= COMMAND_BUF_MAX) sci_console.WP = 0;
			}
		}
	}
#ifdef	SCI2_ACTIVATE
	//  RS-232C(COM2)
	numBytes = sci_get_check(2);
	if(numBytes != 0)
	{   //  COM1��M����
		for(i = 0; i < numBytes; i++)
		{
			c = sci_get_char(2);
		//	sci_putc(console_port, c); //  �G�R�[�o�b�N
			if(c == 0x0D)
			{   //  [CR]
				sci2_console.BUF[sci2_console.WP] = 0;
				sci2_console.WP = 0;
				retport = 2;
				command_job(sci2_console.BUF);
			}
			else
			if(c == 0x08 || c == 0x7F)
			{	//	1�����폜
				if(sci2_console.WP > 0) sci2_console.WP--;
			}
			else
			{
				if(c >= 0x61 && c <= 0x7a)
				{   // �������͑啶���ɕϊ�����
					sci2_console.BUF[sci2_console.WP] = c - 0x20;
				}
				else
				{
					sci2_console.BUF[sci2_console.WP] = c;
				}
				sci2_console.WP++;
				if(sci2_console.WP >= COMMAND_BUF_MAX) sci2_console.WP = 0;
			}
		}
	}
#endif
#ifdef	__LFY_RX63N__
#ifdef	__USE_LFY_USB__
	//  USB
	for(numBytes = 0; numBytes < sizeof(buffer); numBytes++)
	{
		n = usb_getch();
		if(n < 0) break;
		buffer[numBytes] = (char)n;
	}
	if(numBytes != 0)
	{
		for(i = 0; i < numBytes; i++)
		{
			c = buffer[i];
			if(c == 0x0D)
			{   //  [CR]
				usb_console.BUF[usb_console.WP] = 0;
				usb_console.WP = 0;
				retport = 4;	//	USB
				command_job(usb_console.BUF);
			}
			else
			if(c != 0)
			{
				if(c >= 0x61 && c <= 0x7a)
				{   // �������͑啶���ɕϊ�����
					usb_console.BUF[usb_console.WP] = c - 0x20;
				}
				else
				if(c == 0x08 || c == 0x7F)
				{	//	1�����폜
					if(usb_console.WP > 0) usb_console.WP--;
				}
				else
				{
					usb_console.BUF[usb_console.WP] = c;
				}
				usb_console.WP++;
				if(usb_console.WP >= COMMAND_BUF_MAX) usb_console.WP = 0;
			}
		}
	}
#endif
#endif
}

//________________________________________________________________________________________
//
//	CAN���䕔
//----------------------------------------------------------------------------------------
//	�@�\����
//		CAN�|�[�g��ECU�����Ăяo��
//	����
//		����
//	�߂�
//		����
//________________________________________________________________________________________
//
void can_ctrl(void)
{
		//	CAN�Ăяo��
#ifndef	__LFY_RX63N__
#ifdef	RSPI2_ACTIVATE
#if	(CAN_CH_MAX==4)
	if(can3_job())	//	CAN3��RSPI2�o�R�ł̐���Ȃ̂Ő�p�������K�v
#endif
#endif
#endif
	ecu_job();		//	ECU ����(CAN0�`2�̐����ECU����)
}

//________________________________________________________________________________________
//
//	CAN2ECU ���C��
//----------------------------------------------------------------------------------------
//  �@�\����
//	  �������{���C�����[�`��
//  ����
//	  ����
//  �߂�
//	  ����
//________________________________________________________________________________________
//
int main(void)
{
	//	�N��������
	PortInit();						//  I/O�|�[�g������
	cmt0_init();					//  CMT0���W���[���ݒ�
	cmt1_init();					//  CMT1���W���[���ݒ�
#ifdef	__LFY_RX63N__

#ifdef	SCI1_ACTIVATE
	sci1_init(38400, 8 ,1 ,0);		//  SCI1���W���[���ݒ�(RS-232C) <--> FWRITE2(debug-port)
#endif

#else	/*__LFY_RX63N__*/

#ifdef	SCI0_ACTIVATE
	sci0_init(9600, 8 ,2 ,0);		//  SCI0���W���[���ݒ�(RS-232C) <--> LF74
#endif
#ifdef	SCI1_ACTIVATE
	sci1_init(9600, 8 ,2 ,0);		//  SCI1���W���[���ݒ�(RS-232C) <--> FWRITE2(debug-port)
#endif
#ifdef	SCI2_ACTIVATE
	sci2_init(9600, 8 ,2 ,0);		//  SCI2���W���[���ݒ�(RS-232C) <--> EXTERNUL-LF74
#endif
#ifdef	SCI3_ACTIVATE
	sci3_init(9600, 8 ,2 ,0);		//  SCI3���W���[���ݒ�(RS-232C) <--> LF62(USB������)
#endif

#endif	/*__LFY_RX63N__*/

	 _ei() ; /* ���荞�݋��֐��iY�X�R�[�v�����荞�݂��g�p����j�@*/

#ifdef	__LFY_RX63N__
#ifdef	__USE_LFY_USB__
	usb_init();						//  USB0���W���[���ݒ�
#endif
#endif
//  ADC_SD_Init(1);
//  rtc_init();

	//  �N���ʒm
#ifdef	__LFY_RX63N__
	retport = 1;
	console_port = 1;
#ifdef	SCI1_ACTIVATE
	sci_puts(1, " \r\n \r\n \r\n");
	send_var(1);
//	sci_puts(1, VERSION_INFO);
#endif
#else
	retport = 0;		//	COMx
	console_port = 0;
#ifdef	SCI0_ACTIVATE
	sci_puts(0, " \r\n \r\n \r\n");
	send_var(0);
//	sci_puts(0, VERSION_INFO);
#endif
#ifdef	SCI1_ACTIVATE
	sci_puts(1, " \r\n \r\n \r\n");
	send_var(1);
//	sci_puts(1, VERSION_INFO);
#endif
#ifdef	SCI2_ACTIVATE
	sci_puts(2, " \r\n \r\n \r\n");
	send_var(2);
//	sci_puts(2, VERSION_INFO);
#endif
#ifdef	SCI3_ACTIVATE
	sci_puts(3, " \r\n \r\n \r\n");
	send_var(3);
//	usb_puts(VERSION_INFO);
#endif
#endif

	//	ROM���쏉����
	reset_fcu();	//  FCU���Z�b�g
	flash_init();	//  FCU�C�j�V�����C�Y
	can_tp_init();	//	CAN-TP������
	can_uds_init();	//	CAN-UDS������

	//	�N�������ꏈ��		���@S1-7,8 ����[ON]��Ԃ�YScope����F/W�N�������ꍇ��ROM��
	if(DPSW_ROM_BOOT == 0)
	{	//	REM-MON �N��
		if(DPSW_BOOTCOPY == 0)
		{	//	F/W����J�n��ROM������
			if((*((unsigned long *)0x00000064)) < 0x00020000)
			{	//	F/W�̋N������REM-MON�Ȃ�ROM������
				command_job("BOOTCLEAR");	//	ROM����
				command_job("BOOTCOPY");	//	ROM��������
			}
		}
	}

#ifndef	__LFY_RX63N__
	//	MCP2515������
	can3_init();
#endif
	
	//	���C�����[�`��
	for(;;)
	{
		iwdt_refresh();	//	IWDT���t���b�V��
		cmt0_job();		//	�^�C���A�b�v�Ăяo��
		can_ctrl();		//	CAN �R���g���[��
		comm_job();		//	SCI/USB �R�}���h����
		if(tp_pack.TXIF)
		{	//	���M���������v���L��
			tp_pack.TXIF = 0;	//	�v������
			can_tp_txendreq();	//	CAN-TP���M���������Ăяo��
		}
		if(uds_reset_request != 0)
		{	//	ECU�ċN��
			switch(uds_reset_request)
			{
			case 1:	//	�n�[�h���Z�b�g
				wdt_init();
				break;
			case 2:	//	��ԃN���A
				memset(&can_buf, 0, sizeof(can_buf));
				break;
			case 3:	//	�\�t�g���Z�b�g
				SYSTEM.PRCR.WORD = 0xA502;
				SYSTEM.SWRR = 0xA501;
				break;
			}
			uds_reset_request = 0;
		}
	}
	return 0;
}


