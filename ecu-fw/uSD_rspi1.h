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
//	RX63N	uSD�p	RSPI1-I/F �ʐM
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

//#include "uSD_rspi1.h"

#ifndef __CAN2ECU_uSD_IF__
#define __CAN2ECU_uSD_IF__

/*
	�|�[�g�ݒ�

			Port		SCI			I2C			SPI			�K�p
	----------------------------------------------------------------------------
	RSPI1	PE7									MISOB		<RSPI>		uSD
			PE6									MOSIB		<RSPI>		uSD
			PE5									RSPCKB		<RSPI>		uSD
			PE4									SSLB0		<RSPI>		uSD
*/

#define		RSPI1_ACTIVATE

#ifdef		RSPI1_ACTIVATE

//	uSD�p RSPI�Ǘ��\����
typedef struct	__spi_module__ {
	int				err;				//	�G���[�t���O
	void			*rx_proc;			//	��M�������荞�ݎ��Ăяo���֐�
	void			*tx_proc;			//	���M�������荞�ݎ��Ăяo���֐�
	void			*ti_proc;			//	�A�C�h�����O���荞�ݎ��Ăяo���֐�
	void			*err_proc;			//	�G���[�������荞�ݎ��Ăяo���֐�
}	SPI_MODULE;

extern	SPI_MODULE		usd_spi_com;

//�ԐڌĂяo���̃v���g�^�C�v(�����P��)
typedef	void 			(*USD_PROC_CALL)(void *);

//	���O�@�\
void	logging(char *fmt, ...);

//________________________________________________________________________________________
//
//	rspi1_init
//----------------------------------------------------------------------------------------
//	�@�\����
//		RSPI1������
//					Port		SCI			I2C			SPI			�K�p
//			----------------------------------------------------------------------------
//			RSPI1	PE7									MISOB		<RSPI>		uSD
//					PE6									MOSIB		<RSPI>		uSD
//					PE5									RSPCKB		<RSPI>		uSD
//					PE4									SSLB0		<RSPI>		uSD
//	����
//		speed		�ʐM���x	100,000�`10,000,000
//	�߂�
//		����
//________________________________________________________________________________________
//
extern	void rspi1_init(long bps);

#endif		/*RSPI1_ACTIVATE*/
#endif		/*__CAN2ECU_uSD_IF__*/
