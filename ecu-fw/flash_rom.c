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
//  LFY-RX63N1  �v���O����ROM�G���A��������
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

#include <sysio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iodefine.h"
#include "timer.h"
#include "flash_rom.h"

/*
    ROM�u���b�N�\��(2MByte)

    Block
    69      FFE0 0000   64K     �̈�3   ��f�[�^�̈�1
    68      FFE1 0000   64K     �̈�3   512K
    67      FFE2 0000   64K     �̈�3
    66      FFE3 0000   64K     �̈�3
    65      FFE4 0000   64K     �̈�3
    64      FFE5 0000   64K     �̈�3
    63      FFE6 0000   64K     �̈�3
    62      FFE7 0000   64K     �̈�3
    
    61      FFE8 0000   64K     �̈�2   ��f�[�^�̈�2
    60      FFE9 0000   64K     �̈�2   512K
    59      FFEA 0000   64K     �̈�2
    58      FFEB 0000   64K     �̈�2
    57      FFEC 0000   64K     �̈�2
    56      FFED 0000   64K     �̈�2
    55      FFEE 0000   64K     �̈�2
    54      FFEF 0000   64K     �̈�2

    53      FFF0 0000   32K     �̈�1   �A�v���P�[�V�����̈�1
    52      FFF0 8000   32K     �̈�1   256K
    51      FFF1 0000   32K     �̈�1
    50      FFF1 8000   32K     �̈�1
    49      FFF2 0000   32K     �̈�1
    48      FFF2 8000   32K     �̈�1
    47      FFF3 0000   32K     �̈�1
    46      FFF3 8000   32K     �̈�1

    45      FFF4 0000   32K     �̈�1   �A�v���P�[�V�����̈�2
    44      FFF4 8000   32K     �̈�1   256K
    43      FFF5 0000   32K     �̈�1
    42      FFF5 8000   32K     �̈�1
    41      FFF6 0000   32K     �̈�1
    40      FFF6 8000   32K     �̈�1
    39      FFF7 0000   32K     �̈�1
    38      FFF7 8000   32K     �̈�1

    37..8   FFF8 0000   16K*30  �̈�0   �����[�g���j�^�{�u�[�g���[�_
    7..0    FFFF 8000   4K*8    �̈�0   512K
*/

#define     ROM_WRITE_TOP   0x0E000000      /*  ROM�������ݗp�A�h���X   */

#define MIN(a, b)   ((a) < (b) ? (a) : (b))

#define T_FCUR  (35)
#define T_PCKA  (120)
#define T_P128  (33600)
#define T_E16K  (576000)

#define FCU_RAM_ADDR    0x007F8000      /*  FCU��pRAM�̈�      */
#define FCU_FRM_ADDR    0xFEFFE000      /*  FCU�t�@�[���E�F�A   */
#define FCU_FRM_SIZE    0x00002000      /*  FW�T�C�Y            */

static const struct {
    unsigned long   start_addr;
    unsigned long   end_addr;
    int             area_no;
} block_info[] = {
     /* 4 KiB * 8 blocks */
    {   0xfffff000, 0xffffffff, 0   },
    {   0xffffe000, 0xffffefff, 0   },
    {   0xffffd000, 0xffffdfff, 0   },
    {   0xffffc000, 0xffffcfff, 0   },
    {   0xffffb000, 0xffffbfff, 0   },
    {   0xffffa000, 0xffffafff, 0   },
    {   0xffff9000, 0xffff9fff, 0   },
    {   0xffff8000, 0xffff8fff, 0   },

    /* 16 KiB * 22 blocks */
    {   0xffff4000, 0xffff7fff, 0   },
    {   0xffff0000, 0xffff3fff, 0   },
    {   0xfffec000, 0xfffeffff, 0   },
    {   0xfffe8000, 0xfffebfff, 0   },
    {   0xfffe4000, 0xfffe7fff, 0   },
    {   0xfffe0000, 0xfffe3fff, 0   },
    {   0xfffdc000, 0xfffdffff, 0   },
    {   0xfffd8000, 0xfffdbfff, 0   },
    {   0xfffd4000, 0xfffd7fff, 0   },
    {   0xfffd0000, 0xfffd3fff, 0   },
    {   0xfffcc000, 0xfffcffff, 0   },
    {   0xfffc8000, 0xfffcbfff, 0   },
    {   0xfffc4000, 0xfffc7fff, 0   },
    {   0xfffc0000, 0xfffc3fff, 0   },
    {   0xfffbc000, 0xfffbffff, 0   },
    {   0xfffb8000, 0xfffbbfff, 0   },
    {   0xfffb4000, 0xfffb7fff, 0   },
    {   0xfffb0000, 0xfffb3fff, 0   },
    {   0xfffac000, 0xfffaffff, 0   },
    {   0xfffa8000, 0xfffabfff, 0   },
    {   0xfffa4000, 0xfffa7fff, 0   },
    {   0xfffa0000, 0xfffa3fff, 0   },

    /* 16 KiB * 8 blocks */
    {   0xfff9c000, 0xfff9ffff, 0   },
    {   0xfff98000, 0xfff9bfff, 0   },
    {   0xfff94000, 0xfff97fff, 0   },
    {   0xfff90000, 0xfff93fff, 0   },
    {   0xfff8c000, 0xfff8ffff, 0   },
    {   0xfff88000, 0xfff8bfff, 0   },
    {   0xfff84000, 0xfff87fff, 0   },
    {   0xfff80000, 0xfff83fff, 0   },

     /* 32 KiB * 8 blocks */
    {   0xfff78000, 0xfff7ffff, 1   },
    {   0xfff70000, 0xfff77fff, 1   },
    {   0xfff68000, 0xfff6ffff, 1   },
    {   0xfff60000, 0xfff67fff, 1   },
    {   0xfff58000, 0xfff5ffff, 1   },
    {   0xfff50000, 0xfff57fff, 1   },
    {   0xfff48000, 0xfff4ffff, 1   },
    {   0xfff40000, 0xfff47fff, 1   },

     /* 32 KiB * 8 blocks */
    {   0xfff38000, 0xfff3ffff, 1   },
    {   0xfff30000, 0xfff37fff, 1   },
    {   0xfff28000, 0xfff2ffff, 1   },
    {   0xfff20000, 0xfff27fff, 1   },
    {   0xfff18000, 0xfff1ffff, 1   },
    {   0xfff10000, 0xfff17fff, 1   },
    {   0xfff08000, 0xfff0ffff, 1   },
    {   0xfff00000, 0xfff07fff, 1   },

     /* 64 KiB * 8 blocks */
    {   0xffef0000, 0xffefffff, 2   },
    {   0xffee0000, 0xffeeffff, 2   },
    {   0xffed0000, 0xffedffff, 2   },
    {   0xffec0000, 0xffecffff, 2   },
    {   0xffeb0000, 0xffebffff, 2   },
    {   0xffea0000, 0xffeaffff, 2   },
    {   0xffe90000, 0xffe9ffff, 2   },
    {   0xffe80000, 0xffe8ffff, 2   },

     /* 64 KiB * 8 blocks */
    {   0xffe70000, 0xffe7ffff, 3   },
    {   0xffe60000, 0xffe6ffff, 3   },
    {   0xffe50000, 0xffe5ffff, 3   },
    {   0xffe40000, 0xffe4ffff, 3   },
    {   0xffe30000, 0xffe3ffff, 3   },
    {   0xffe20000, 0xffe2ffff, 3   },
    {   0xffe10000, 0xffe1ffff, 3   },
    {   0xffe00000, 0xffe0ffff, 3   }
};
/*
//  �|�C���^�ϊ�
typedef volatile union  __pointer_union__   {
    volatile void           *vp;
    volatile unsigned long  *lp;
    volatile unsigned short *wp;
    volatile unsigned char  *bp;
    volatile unsigned long  val;
}   MIX_POINTER;
*/
//________________________________________________________________________________________
//
//  �u���b�N���擾
//----------------------------------------------------------------------------------------
//  �@�\����
//      ROM�u���b�N���擾
//  ����
//      addr        �ΏۃA�h���X
//      *start      �J�n�Ԓn�ۑ���
//      *end        �I���Ԓn�ۑ���
//      *area_no    �G���A�ԍ��ۑ���
//      *block_no   �u���b�N�ԍ��ۑ���
//  �߂�
//      int         0:���� / ��:�G���[
//________________________________________________________________________________________
//
int flash_get_block_info(unsigned long addr, unsigned long *start, unsigned long *end, int *area_no, int *block_no)
{
    int i;

    for (i = 0; i < sizeof(block_info) / sizeof(block_info[0]); i++)
    {
        if (addr >= block_info[i].start_addr && addr <= block_info[i].end_addr)
        {
            if (start != NULL)
            {
                *start = block_info[i].start_addr;
            }
            if (end != NULL)
            {
                *end = block_info[i].end_addr;
            }
            if (area_no != NULL)
            {
                *area_no = block_info[i].area_no;
            }
            if (block_no != NULL)
            {
                *block_no = i;
            }
            return 0;
        }
    }
    //  ROM�G���A�O
    return -1;
}

//________________________________________________________________________________________
//
//  FCU���W���[��������
//----------------------------------------------------------------------------------------
//  �@�\����
//      FCU�̏�����
//  ����
//      ����
//  �߂�
//      ����
//________________________________________________________________________________________
//
void    reset_fcu(void)
{
    /* forcibly terminate programming/erasure operations */
    FLASH.FRESETR.WORD = 0xCC01;

    //  35usec�҂�
    cmt1_start(T_FCUR * CMT1_1US, 0);   //  *1usec
    while(cmt1_ni_check() != 0);

    /* release reset state */
    FLASH.FRESETR.WORD = 0xCC00;
}

//________________________________________________________________________________________
//
//  FCU���W���[���X�e�[�^�X�`�F�b�N
//----------------------------------------------------------------------------------------
//  �@�\����
//      FCU�̏�Ԋm�F
//  ����
//      *peaddr     P/E�A�h���X
//  �߂�
//      int         0:���� / ��:�G���[
//________________________________________________________________________________________
//
int check_fstatr(unsigned long peaddr)
{
    MIX_POINTER p;
    p.val = peaddr;

    if (FLASH.FSTATR1.BIT.FCUERR)
    {   //  FCU�G���[
        reset_fcu();    //  �ď�����
        return -1;
    }
    
    if (FLASH.FSTATR0.BIT.ILGLERR)
    {   //  �R�}���h�G���[
        if (FLASH.FASTAT.BIT.CMDLK)
        {   //  �R�}���h���b�N���� FASTAT.ROMAE / DLFAE / DFLRPE  / DFLWPE
            FLASH.FASTAT.BYTE = 0x10;
        }
        *p.lp = 0x50;   //  �X�e�[�^�X�N���A�R�}���h
        return -1;
    }

    if (FLASH.FSTATR0.BIT.ERSERR || FLASH.FSTATR0.BIT.PRGERR)   // (FLASH.FSTATR0.BYTE & 0x30)
    {   //  �������͏������݃G���[
        *p.lp = 0x50;   //  �X�e�[�^�X�N���A�R�}���h
        return -1;
    }
    //  ����
    return 0;
}

//________________________________________________________________________________________
//
//  FCU���W���[�����҂�
//----------------------------------------------------------------------------------------
//  �@�\����
//      FCU�̏����҂�
//  ����
//      timeout     �Œ��҂�����
//  �߂�
//      int         0:���� / ��:�G���[
//________________________________________________________________________________________
//
int wait_frdy(unsigned long timeout)
{
    int frdy = 0;

    timeout = (timeout + 99) / 100; //  100usec�P�ʂɊۂ߂�
    while (timeout-- > 0 && frdy == 0)
    {
        cmt1_start(100 * CMT1_1US, 0);  //  100*1usec
        while(cmt1_ni_check() != 0)
        {
            if (FLASH.FSTATR0.BIT.FRDY)
            {
                frdy = 1;
                break;
            }
        }
    }
    return frdy ? 0 : -1;
}

//________________________________________________________________________________________
//
//  FCU���W���[�������[�h���[�h�ֈڍs����
//----------------------------------------------------------------------------------------
//  ����
//      *peaddr     P/E�A�h���X
//  �߂�
//      int         0:���� / ��:�G���[
//________________________________________________________________________________________
//
int switch_to_read(unsigned long peaddr)
{
    int ret;

    //  ���҂�
    if (wait_frdy((unsigned long)(T_E16K)) != 0)
    {   //  �^�C���A�E�g
        reset_fcu();
        return -1;
    }

    //  �G���[�m�F
    ret = check_fstatr(peaddr);

    //  FENTRYR.FENTRY0..3�N���A */
    FLASH.FENTRYR.WORD = 0xAA00;
    while (FLASH.FENTRYR.WORD != 0x0000);

    //  �֎~��
    FLASH.FWEPROR.BIT.FLWE = 2;     //  0,2,3 = �֎~(DEF) / 1 = P/E�A���b�N�r�b�g��P/E�A���b�N�r�b�g�̓Ǐo���A�u�����N�`�F�b�N�̋���
    return ret;
}

//________________________________________________________________________________________
//
//  FCU���W���[���̃N���b�N�ݒ�
//----------------------------------------------------------------------------------------
//  ����
//      *peaddr     P/E�A�h���X
//  �߂�
//      int         0:���� / ��:�G���[
//________________________________________________________________________________________
//
int notify_fclk(unsigned long peaddr)
{
    MIX_POINTER p;
    p.val = peaddr;
    //  96MHz/4 : FCLK = 24MHz
    FLASH.PCKAR.BIT.PCKA = 24;

    /* write 0xE9 to the destination address
       for peripheral clock notification */
    *p.bp = 0xE9;

    /* write 0x03 to the destination address
       for peripheral clock notification */
    *p.bp = 0x03;

    /* write 0x0f0f to the destination address
       for peripheral clock notification three times(as a word) */
    *p.wp = 0x0F0F;
    *p.wp = 0x0F0F;
    *p.wp = 0x0F0F;

    /* write 0xd0 to the destination address 
       for peripheral clock notification */
    *p.bp = 0xD0;

    /* FRDY bit check(tWAIT = tPCKA) */
    if (wait_frdy((unsigned long)T_PCKA) != 0)
    {   //  �^�C���A�E�g
        reset_fcu();
        return -1;
    }
    //  �G���[�`�F�b�N
    return check_fstatr(peaddr);
}

//________________________________________________________________________________________
//
//  FCU���W���[����P/E���[�h�ֈڍs����
//----------------------------------------------------------------------------------------
//  ����
//      *peaddr     P/E�A�h���X
//  �߂�
//      int         0:���� / ��:�G���[
//________________________________________________________________________________________
//
int switch_to_pe(unsigned long peaddr)
{
    static int  fclk_notified = 0;
    int         area_no;

    //  �G���A���擾
    if (flash_get_block_info(peaddr | 0xff000000, NULL, NULL, &area_no, NULL) != 0) return -1;

    //  P/E����
    FLASH.FWEPROR.BIT.FLWE = 1;     //  0,2,3 = �֎~(DEF) / 1 = P/E�A���b�N�r�b�g��P/E�A���b�N�r�b�g�̓Ǐo���A�u�����N�`�F�b�N�̋���

    //  �G���A�r�b�g�N���A
    FLASH.FENTRYR.WORD = 0xAA00;
    while (FLASH.FENTRYR.WORD != 0x0000);

    //  �G���A���ݒ�
    FLASH.FENTRYR.WORD = 0xAA00 | (0x0001 << area_no);

    //  �G���[�`�F�b�N
    if (check_fstatr(peaddr)) return -1;

    //  �N���b�N���ݒ�m�F
    if (!fclk_notified)
    {   //  FCLK�ݒ�
        if (notify_fclk(peaddr)) return -1;
        fclk_notified = 1;
    }
    return 0;
}

//________________________________________________________________________________________
//
//  �C�j�V�����C�Y
//----------------------------------------------------------------------------------------
//  ����
//      ����
//  �߂�
//      ����
//________________________________________________________________________________________
//
void    flash_init(void)
{
    //  �������̈惊�[�h�����Ń��b�N�r�b�g���[�h(1)
    FLASH.FMODR.BIT.FRDMD = 0;

    //  ���f�B�[���荞�݋֎~
    FLASH.FRDYIE.BIT.FRDYIE = 0;

    //  �G���[���荞�݋֎~
    FLASH.FAEINT.BYTE = 0x00;

    //  �G���A�I������
    FLASH.FENTRYR.WORD = 0xAA00;

    //  �������݃v���e�N�g
    FLASH.FWEPROR.BIT.FLWE = 0;

    //  E2�A�N�Z�X�ݒ�
    FLASH.DFLRE0.WORD = 0x2DFF;     //  E2�Ǐo������/�֎~
    FLASH.DFLRE1.WORD = 0xD2FF;     //  E2�Ǐo������/�֎~
    FLASH.DFLWE0.WORD = 0x1EFF;     //  E2�����݋���/�֎~
    FLASH.DFLWE1.WORD = 0xE1FF;     //  E2�����݋���/�֎~

    //  FCU-RAM�g�p����
    FLASH.FCURAME.WORD = 0xC401;

    //  FCU-FW��ROM����RAM�ɃR�s�[
    memcpy((void *)FCU_RAM_ADDR, (void *)FCU_FRM_ADDR, FCU_FRM_SIZE);
}

//________________________________________________________________________________________
//
//  ��������
//----------------------------------------------------------------------------------------
//  ����
//      dst         �������ݐ�ROM�A�h���X
//      src         �ǂݏo����RAM�A�h���X
//      len         �����݃o�C�g��
//  �߂�
//      int         0:���� / ��:�G���[
//________________________________________________________________________________________
//
int flash_write_rom(unsigned long dst, const unsigned long src, int len)
{
    MIX_POINTER     ra, wa;
    unsigned long   npad_lead, npad_trail;
    unsigned long   l, m;
    unsigned short  *p = (unsigned short *)src;
    int             odd = 0;

    if (((unsigned long)dst & 0xFF000000) != 0xFF000000) return -1; //  ROM�͈͊O

    //  128�o�C�g���ɏ�������
    while (len > 0)
    {
        //  �������݃A�h���X
        ra.val = dst & 0x00FFFFFF;              //  �������݃A�h���X
        wa.val = ra.val & ~0x0000007F;          //  ����128byte�����}�X�N

        //  �擪��FF�o�C�g��
        m = MIN(128 - (ra.val - wa.val), len);  //  ��菬������    m = 1..128  �������݂Ȃ��̃o�C�g��
        l = m;
        npad_lead = ra.val - wa.val;            //  �擪�������݃o�C�g�[��  0..127
        npad_trail = 128 - (npad_lead + m);     //  ����FF�o�C�g��        0..127

        //  P/E���[�h�J�n
        if (switch_to_pe(wa.val) != 0) return -1;

        //  ���b�N�r�b�g����
        FLASH.FPROTR.WORD = 0x5501;
        //  �R�}���h
        *wa.bp = 0xE8;  //  1st
        *wa.bp = 0x40;  //  2nd

        //  �擪FF
        while (npad_lead > 1)
        {
            *wa.wp = 0xFFFF;
            npad_lead -= 2;
        }
        if (npad_lead == 1 && m >= 1)
        {   //  �[���o�C�g
            *wa.wp = (*p << 8) | 0x00FF;
            m--;
            odd = 1;
        }

        if (odd)
        {   //  ���[���[�h
            while (m > 1)
            {
                *wa.wp = (*p >> 8) | (*(p + 1) << 8);
                p++;
                m -= 2;
            }
        }
        else
        {   //  ���[�h�]��
            while (m > 1)
            {
                *wa.wp = *p++;
                m -= 2;
            }
        }
        
        //  ���̔��[�o�C�g
        if (m == 1)
        {
            *wa.wp = *p++ | 0xFF00;
            npad_trail--;
        }
        //  �㕔FF
        while (npad_trail > 0)
        {
            *wa.wp = 0xFFFF;
            npad_trail -= 2;
        }
        //  �I�[
        *wa.bp = 0xD0;      //  67th

        //  ���������҂�
        if (wait_frdy((unsigned long)(T_P128 * 11 / 10)) != 0)
        {   //  �^�C���A�E�g
            reset_fcu();
            return -1;
        }

        //  ���[�h���[�h
        if (switch_to_read(wa.val) != 0) return -1;

        dst += l;
        len -= l;
    }
    return 0;
}

//________________________________________________________________________________________
//
//  �u���b�N����
//----------------------------------------------------------------------------------------
//  ����
//      dst         �����A�h���X
//  �߂�
//      int         0:���� / ��:�G���[
//________________________________________________________________________________________
//
int flash_erase_rom(unsigned long dst)
{
    MIX_POINTER ba;

    if ((dst & 0xFF000000) != 0xFF000000) return -1;    //  ROM�͈͊O

    //  �����A�h���X
    ba.val = dst & 0x00FFFFFF;

    //  P/E���[�h
    if (switch_to_pe(ba.val) == -1) return -1;

    //  ���b�N�r�b�g����
    FLASH.FPROTR.WORD = 0x5501;
    //  �R�}���h
    *ba.bp = 0x20;  //  1st
    *ba.bp = 0xD0;  //  2nd     �����J�n

    //  �����҂�
    if (wait_frdy((unsigned long)(T_E16K * 11 / 10)) != 0)
    {   //  �^�C���A�E�g
        reset_fcu();
        return -1;
    }

    //  ���[�h���[�h
    return switch_to_read(ba.val);
}

