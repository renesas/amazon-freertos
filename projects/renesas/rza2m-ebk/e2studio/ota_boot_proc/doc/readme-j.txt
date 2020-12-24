/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only
* intended for use with Renesas products. No other uses are authorized. This
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
* Copyright (C) 2019 Renesas Electronics Corporation. All rights reserved.
*******************************************************************************/
/******************************************************************************
* System Name : [RZ/A2M] Example of Initialization
* File Name   : readme-j.txt
* Rev        : Rev.1.40.00
* Date       : Nov. 20,2019
******************************************************************************/
/**********************************************************************************
*
* History     : Oct. 04,2018 Rev.1.00.00    �V�K�쐬
*             : Dec. 28,2018 Rev.1.10.00    �T���v���R�[�h���烍�[�_�v���O�����𕪗�
*             :                             ���[�_�v���O�����̏������ݕ��@���X�V
*             : Apr. 15,2019 Rev.1.20.00    CKIO�N���b�N���o�͂����ݒ�ɕύX
*             :                             ���Z�b�g�n���h���̏������C��
*             :                               - NEON�����VFP�̏����ݒ�̏�����ύX
*             :                               - DSFR���W�X�^��IOKEEP�r�b�g�̃N���A������ǉ�
*             : May. 17,2019 Rev.1.30.00    �v���W�F�N�g�̃t�H���_�\�������
*             :                             �R���p�C���I�v�V������ύX
*             : Jul. 12,2019 Rev.1.31.00    �T���v���R�[�h�̃\�[�X�t�@�C������уw�b�_�t�@�C����
*             :                             �L�q���e�ɂ��āA�C���f���g�A���s�Ȃǃ\�[�X�R�[�h��
*             :                             �X�^�C�����C���B
*             :                             �\�[�X�R�����g�̉��P���s���A�T���v���R�[�h���X�V�B
*             : Nov. 20,2019 Rev.1.40.00    R_SC_HardwareSetup�֐��̏�����ύX�B
*             :                               - Userdef_PreHardwareSetup�֐��ɁAIOKEEP�r�b�g��
*             :                                 �N���A���鏈�����ړ��B
*             :                               - Userdef_PostHardwareSetup�֐��ɁA�ێ��p����RAM��
*             :                                 �������݋֎~���������鏈�����ړ��B
***********************************************************************************/

1. �͂��߂�

  �{�T���v���R�[�h�́ARZ/A2M�O���[�v R7S921053VCBG�𓋍ڂ���
  RZ/A2M CPU�{�[�h(RTK7921053C00000BE)�����SUB�{�[�h(RTK79210XXB00000BE)��
  �g�p���ē���m�F���Ă��܂��B
  ���q�l�̃\�t�g�E�G�A�J�����ɋZ�p�Q�l�����Ƃ��Ă����p���������B

  ****************************** �� �� �� ******************************
   �{�T���v���R�[�h�͂��ׂĎQ�l�����ł���A���̓����ۏ؂������
   �ł͂���܂���B���ۂ̃V�X�e���ɑg�ݍ��ޏꍇ�̓V�X�e���S�̂�
   �\���ɕ]�����A���q�l�̐ӔC�ɂ����ēK�p�ۂ𔻒f���Ă��������B
  ****************************** �� �� �� ******************************


2. ����m�F��

  �T���v���R�[�h�͈ȉ��̊��œ�����m�F���Ă��܂��B

    CPU          : RZ/A2M
    �{�[�h       : RZ/A2M CPU�{�[�h(RTK7921053C00000BE)
                   RZ/A2M SUB�{�[�h(RTK79210XXB00000BE)
    �R���p�C��   : GNU Arm Embedded Toolchain 6-2017-q2-update
    �����J���� : e2 studio Version 7.6.0.
    �G�~�����[�^ : SEGGER��J-Link Base
                   (RZ/A2M�ɑΉ�����J-Link���i���������Ă��������B)


3. �T���v���R�[�h�̓��e

  �{�T���v���R�[�h�́A�ȉ��̏������s���܂��B

  (1) �A�v���P�[�V�����v���O���� [rza2m_blinky_sample_osless_gcc]
    RZ/A2M�̏����ݒ���s������Amain�֐��ɂāAUSB Micro-B�R�l�N�^(SUB�{�[�h��CN5)�o�R��
    �ڑ������z�X�gPC��̃^�[�~�i���ɕ�������o�͂��AOS�^�C�}���N�����܂��B
    500ms���Ɏ��s�����OS�^�C�}���荞�ݏ����ɂ��ACPU�{�[�h���LED1���_�ł��܂��B

    �A�v���P�[�V�����v���O�����́A���[�_�v���O�����ɂ��SPI�}���`I/O�o�X�R���g���[��(SPIBSC)��
    �V���A���t���b�V���������̃��W�X�^�ݒ菈�������s������ɁA���[�_�v���O����������s����܂��B
    ���[�_�v���O�����́A�ȉ��̃t�H���_�Ɋi�[���Ă��܂��B
      [rza2m_blinky_sample_osless_gcc\bootloader\rza2_qspi_flash_ddr_bootloader.elf]

    ���̃��[�_�v���O�����́AMacronix�Ђ�MX25L51245G�̎d�l�ɍ��킹�Ă��܂��B���̃V���A���t���b�V����������
    �g�p����ꍇ�́A�t���b�V���������̎d�l�ɍ��킹�āuRZ/A2M�O���[�v �V���A���t���b�V������������̃u�[�g��v
    �A�v���P�[�V�����m�[�g�ɕt���̃��[�_�v���O�����̃v���W�F�N�g[rza2m_sflash_boot_loader_gcc]�̃\�[�X�R�[�h��ύX���A
    ���[�h���W���[���𐶐����Ă��������B�������ꂽ���[�h���W���[���̃t�@�C������"rza2_qspi_flash_ddr_bootloader.elf"��
    �ύX���Ă��g�p���������B

  �T���v���R�[�h�̏ڍד��e�ɂ��ẮA�A�v���P�[�V�����m�[�g���Q�Ƃ��Ă��������B


4. �T���v���R�[�h�̓���m�F����

  (1) �u�[�g���[�h
    - �u�[�g���[�h3
      (�V���A���t���b�V��������3.3V����u�[�g)
      ����L�ȊO�̃u�[�g���[�h��ݒ肵���ꍇ�A�v���O�����͓��삵�܂���B

  (2) ������g��
    RZ/A2M CPU�{�[�h���RZ/A2M�̊e�N���b�N���ȉ��̎��g���ƂȂ�悤�ɁA
    RZ/A2M�̃N���b�N�p���X���U���ݒ肵�Ă��܂��B
    (RZ/A2M�̃N���b�N���[�h1�ŁAEXTAL�[�q��24MHz�̃N���b�N��
    ���͂���Ă����Ԃł̎��g���ł��B)
      - CPU�N���b�N(I��)     : 528MHz
      - �摜����(G��)        : 264MHz
      - �����o�X�N���b�N(B��): 132MHz
      - ���ӃN���b�N1(P1��)  :  66MHz
      - ���ӃN���b�N0(P0��)  :  33MHz
      - QSPI0_SPCLK          :  66MHz
      - CKIO                 : 132MHz

  (3) �����������ʐM�̐ݒ�
    - �r�b�g���[�g  : 115200 bps
    - �f�[�^�r�b�g  : 8 bits
    - �p���e�B�r�b�g: �Ȃ�
    - �X�g�b�v�r�b�g: 1 bit

  (4) �g�p����V���A���t���b�V��������
    - ���[�J  : Macronix��
    - �^��    : MX25L51245G

  (5) �L���b�V���̐ݒ�
    L1�����L2�L���b�V���̏����ݒ��MMU��ݒ肷�邱�Ƃɂ��s���Ă��܂��B
    L1�L���b�V�������L2�L���b�V���̗L���܂��͖����̗̈�ɂ��ẮA
    RZ/A2M�����ݒ��̃A�v���P�[�V�����m�[�g�́uMMU�̐ݒ�v���Q�Ƃ��Ă��������B


5. �T���v���R�[�h�̓���菇

  �{�T���v���R�[�h�𓮍삳����ꍇ�́A�ȉ��̎菇�ɏ]���Ă��������B

  (1) �f�B�b�v�X�C�b�`����уW�����p�̐ݒ�
     CPU�{�[�h�̃f�B�b�v�X�C�b�`����уW�����p���ȉ��̂Ƃ���ɐݒ肵�܂��B

     <<CPU�{�[�h�̐ݒ�>>
      - SW1-1  : ON  (SSCG���� OFF)
        SW1-2  : OFF (�N���b�N���[�h1(EXTAL���͂̓��͎��g����20�`24MHz�ɐݒ�))
        SW1-3  : ON  (MD_BOOT2 = L)
        SW1-4  : OFF (MD_BOOT1 = H)
        SW1-5  : OFF (MD_BOOT0 = H)
        SW1-6  : ON  (BSCANP �ʏ퓮��(CoreSight�f�o�b�O���[�h))
        SW1-7  : ON  (CLKTEST OFF)
        SW1-8  : ON  (TESTMD OFF)
      - JP1 :   1-2  (RZ/A2M��PVcc_SPI�����U2��3.3V������)
      - JP2 :   2-3  (RZ/A2M��PVcc_HO�����U3��1.8V������)
      - JP3 :   Open (USB ch0���t�@���N�V�������[�h�Ŏg�p����(VBUS0�d�����������Ȃ�))

     SUB�{�[�h�̃f�B�b�v�X�C�b�`����уW�����p���ȉ��̂Ƃ���ɐݒ肵�܂��B

     <<SUB�{�[�h�̐ݒ�>>
      - SW6-1  : OFF (P9_[7:0]�AP8_[7:1]�AP2_2�AP2_0�AP1_3�AP1_[1:0]�AP0_[6:0]�AP6_7�AP6_5�AP7_[1:0]�AP7[5:3]��
                       DRP�A�I�[�f�B�I�AUART�ACAN�����USB�C���^�t�F�[�X�[�q�Ƃ��Ďg�p)
        SW6-2  : OFF (P8_4�AP8_[7:6]�AP6_4�AP9_[6:3]���I�[�f�B�I�C���^�t�F�[�X�[�q�Ƃ��Ďg�p)
        SW6-3  : OFF (P9_[1:0]�AP1_0�AP7_5��UART�����USB�C���^�t�F�[�X�[�q�Ƃ��Ďg�p)
        SW6-4  : OFF (P6_[3:1]�APE_[6:0]��CEU�[�q�Ƃ��Ďg�p)
        SW6-5  : ON  (P3_[5:1]�APH_5�APK_[4:0]��Ethernet PHY2����[�q�Ƃ��Ďg�p)
        SW6-6  : ON  (PJ_[7:6]��VDC6�[�q�Ƃ��Ďg�p)
        SW6-7  : ON  (P7_[7:4]��VDC6�[�q�Ƃ��Ďg�p)
        SW6-8  : OFF (NC)
        SW6-9  : OFF (�ėp���̓|�[�gP5_3 = "H")
        SW6-10 : OFF (�ėp���̓|�[�gPC_2 = "H")

      - JP1 : 2-JP2  (PJ_1��IRQ0�X�C�b�`(SW3)�̊��荞�ݒ[�q�Ƃ��Ďg�p)

      �f�B�b�v�X�C�b�`����уW�����p�ݒ�̏ڍׂ́ACPU�{�[�h�����
      SUB�{�[�h�̃��[�U�[�Y�}�j���A�����Q�Ƃ��Ă��������B

  (2) �T���v���R�[�h�̃Z�b�g�A�b�v
    [rza2m_blinky_sample_osless_gcc] �̃f�B���N�g�����z�X�gPC��
    e2 studio���[�N�X�y�[�X�f�B���N�g��(��: "C:\e2studio_Workspace_v760")�ɃR�s�[���܂��B

  (3) �����J�����̋N��
    �����J����e2 studio���N�����܂��B

  (4) �A�v���P�[�V�����v���O����([rza2m_blinky_sample_osless_gcc]�v���W�F�N�g)�̃r���h
    e2 studio���j���[���A[rza2m_blinky_sample_osless_gcc]�v���W�F�N�g���C���|�[�g������A
    rza2m_blinky_sample_osless_gcc.elf�𐶐����܂��B

  (5) �G�~�����[�^�Ƃ̐ڑ�
    J-Link Base��CPU�{�[�h�̃R�l�N�^CN5���AJTAG�P�[�u���Őڑ����܂��B
    �Ȃ��A�ڑ��ɂ�SEGGER�Еϊ��A�_�v�^�uJ-Link 19-pin Cortex-M Adapter�v���K�v�ł��B

  (6) �T���v���R�[�h�̃_�E�����[�h
    e2 studio�́u���s�v���j���[���A�u�f�o�b�O�̍\���v��I�����A�u�f�o�b�O�\���v�_�C�A���O��
    �I�[�v�����܂��B���X�g����uRenesas GDB Hardware Debugging�v��I�����A���̃��X�g��\�����܂��B
    �A�v���P�[�V�����v���O�����̃f�o�b�O�\��[rza2m_blinky_sample_osless_gcc HardwareDebug]��I�����A
    �u�f�o�b�O�v�{�^���������ƁA(4)�Ő��������A�v���P�[�V�����v���O�����̎��s�t�@�C����
    ���[�_�v���O�������V���A���t���b�V���������Ƀ_�E�����[�h���܂��B

  (7) �T���v���R�[�h�̎��s
    �T���v���R�[�h�A�u�[�g���[�_�̃_�E�����[�h������A�T���v���R�[�h�����s���邽�߂ɁAe2 studio�̃��j���[��CPU���Z�b�g���s���܂��B
    �u�ĊJ�v���j���[�����ɂ��A�u�[�g���[�_���s��A�{�T���v���R�[�h�̏������s���܂��B


�ȏ�

