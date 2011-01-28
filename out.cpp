/*
 *      PS3SPU module for the Interactive disassembler (IDA).
 *
 *      updates, fixes and bugreports welcomed (you know where i am)
 *
 *      (w)2011 by Jack Chen, aks anita999
 */

#include "ps3spu.hpp"
#include <fpro.h>
#include <diskio.hpp>

static void out_bad_address(ea_t addr)
{
  out_tagon(COLOR_ERROR);
  OutLong(addr, 16);
  out_tagoff(COLOR_ERROR);
  QueueMark(Q_noName, cmd.ea);
}
//----------------------------------------------------------------------
static void vlog(const char *format, va_list va)
{
  static FILE *fp = NULL;
  if ( fp == NULL ) fp = fopenWT("debug_log");
  qvfprintf(fp, format, va);
  qflush(fp);
}
//----------------------------------------------------------------------
inline void log(const char *format, ...)
{
  va_list va;
  va_start(va, format);
  vlog(format, va);
  va_end(va);
}
//----------------------------------------------------------------------
inline void OutReg(int rgnum)
{
  out_register(ph.regNames[rgnum]);
}

//----------------------------------------------------------------------
// generate the text representation of an operand


//----------------------------------------------------------------------
// generate a text representation of an instruction
// the information about the instruction is in the 'cmd' structure

void out(void)
{
  char buf[MAXSTR];

  init_output_buffer(buf, sizeof(buf)); // setup the output pointer
  OutMnem();                            // output instruction mnemonics

/*  out_one_operand(0);                   // output the first operand

  if ( cmd.Op2.type != o_void)
  {
    out_symbol(',');
    OutChar(' ');
    out_one_operand(1);                 // output the second operand
  }

  if ( cmd.Op3.type != o_void)
  {
    out_symbol(',');
    OutChar(' ');
    out_one_operand(2);                 // output the third operand
  }

  if ( cmd.Op3.type != o_void)
  {
    out_symbol(',');
    OutChar(' ');
    out_one_operand(3);                 // output the fourth operand
  }
*/
	switch (cmd.itype)
	{

//    SU64 immval;


    case ps3spu_lqd :  /*  1 , lqd rt, s14(ra)    lqd Load Quadword (d-form) 32 , op2=I10 extends with sign, then shift left 4 bits */     	
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutValue(cmd.Op2, OOFS_IFSIGN|OOF_SIGNED|OOFW_16);out_symbol('(');//out_long(cmd.Op2.value, 16);out_symbol('(');;
		OutReg(cmd.Op3.value);out_symbol(')');
    	break;
    case ps3spu_lqx :  /*  2 , lqx rt, ra, rb    lqx Load Quadword (x-form) 33  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	break;
    case ps3spu_lqa :  /*  3 , lqa rt, s18    lqa Load Quadword (a-form) 34  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
     	if ( !out_name_expr(cmd.Op2, cmd.Op2.value, cmd.Op2.value) )out_bad_address(cmd.Op2.value);//out_long(cmd.Op2.value, 16);;
    	;
    	break;
    case ps3spu_lqr :  /*  4 , lqr rt, s18    lqr Load Quadword Instruction Relative (a-form) 35  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
     	if ( !out_name_expr(cmd.Op2, cmd.Op2.value, cmd.Op2.value) )out_bad_address(cmd.Op2.value);//out_long(cmd.Op2.value, 16);;
    	;
    	break;
    case ps3spu_stqd :  /*  5 , stqd rt, s14(ra)    stqd Store Quadword (d-form) 36  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutValue(cmd.Op2, OOFS_IFSIGN|OOF_SIGNED|OOFW_16);out_symbol('(');//out_long(cmd.Op2.value, 16);out_symbol('(');;
		OutReg(cmd.Op3.value);out_symbol(')');
    	break;
    case ps3spu_stqx :  /*  6 , stqx rc, ra, rb    stqx Store Quadword (x-form) 37  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	break;
    case ps3spu_stqa :  /*  7 , stqa rt, s18    stqa Store Quadword (a-form) 38  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
     	if ( !out_name_expr(cmd.Op2, cmd.Op2.value, cmd.Op2.value) )out_bad_address(cmd.Op2.value);//out_long(cmd.Op2.value, 16);;
    	;
    	break;
    case ps3spu_stqr :  /*  8 , stqr rt, s18    stqr Store Quadword Instruction Relative (a-form) 39  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
     	if ( !out_name_expr(cmd.Op2, cmd.Op2.value, cmd.Op2.value) )out_bad_address(cmd.Op2.value);//out_long(cmd.Op2.value, 16);;
    	;
    	break;
    case ps3spu_cbd :  /*  9 , cbd rt, u7(ra)    cbd Generate Controls for Byte Insertion (d-form) 40  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutValue(cmd.Op2, OOFW_8);;out_symbol('(');;
		OutReg(cmd.Op3.value);out_symbol(')');
    	break;
    case ps3spu_cbx :  /*  10 , cbx rt, ra, rb    cbx Generate Controls for Byte Insertion (x-form) 41  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_chd :  /*  11 , chd rt, u7(ra)    chd Generate Controls for Halfword Insertion (d-form) 42  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutValue(cmd.Op2, OOFW_8);//out_long(cmd.Op2.value, 16);out_symbol('(');;
		OutReg(cmd.Op3.value);out_symbol(')');
    	;
    	break;
    case ps3spu_chx :  /*  12 , chx rt, ra, rb    chx Generate Controls for Halfword Insertion (x-form) 43  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_cwd :  /*  13 , cwd rt, u7(ra)    cwd Generate Controls for Word Insertion (d-form) 44  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutValue(cmd.Op2,OOFW_8);out_symbol('(');//out_long(cmd.Op2.value, 16);out_symbol('(');;
		OutReg(cmd.Op3.value);out_symbol(')');
    	;
    	break;
    case ps3spu_cwx :  /*  14 , cwx rt, ra, rb    cwx Generate Controls for Word Insertion (x-form) 45  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_cdd :  /*  15 , cdd rt, u7(ra)    cdd Generate Controls for Doubleword Insertion (d-form) 46  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutValue(cmd.Op2,OOFW_8);out_symbol('(');//out_long(cmd.Op2.value, 16);out_symbol('(');;
		OutReg(cmd.Op3.value);out_symbol(')');
    	;
    	break;
    case ps3spu_cdx :  /*  16 , cdx rt, ra, rb    cdx Generate Controls for Doubleword Insertion (x-form) 47  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_ilh :  /*  17 , ilh rt, u16    ilh Immediate Load Halfword 50  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutValue(cmd.Op2, OOFS_IFSIGN|OOF_SIGNED|OOFW_16);//out_long(cmd.Op2.value, 16);;
		;
    	break;
    case ps3spu_ilhu :  /*  18 , ilhu rt, u16    ilhu Immediate Load Halfword Upper 51  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutValue(cmd.Op2, OOFS_IFSIGN|OOF_SIGNED|OOFW_16);//out_long(cmd.Op2.value, 16);;
		;
    	break;
    case ps3spu_il :  /*  19 , il rt, s16    il Immediate Load Word 52  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutValue(cmd.Op2, OOFS_IFSIGN|OOF_SIGNED|OOFW_32);//out_long(cmd.Op2.value, 16);;
		;
    	break;
    case ps3spu_ila :  /*  20 , ila rt, u18    ila Immediate Load Address 53  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	if ( !out_name_expr(cmd.Op2, cmd.Op2.value, cmd.Op2.value) )out_bad_address(cmd.Op2.value);//out_long(cmd.Op2.value, 16);;
    	;
    	break;
    case ps3spu_iohl :  /*  21 , iohl rt, u16    iohl Immediate Or Halfword Lower 54  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutValue(cmd.Op2, OOFS_IFSIGN|OOF_SIGNED|OOFW_16);//out_long(cmd.Op2.value, 16);;
		;
    	break;
    case ps3spu_fsmbi :  /*  22 , fsmbi rt, u16    fsmbi Form Select Mask for Bytes Immediate 55  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutValue(cmd.Op2, OOFW_8);//out_long(cmd.Op2.value, 16);;
		;
    	break;
    case ps3spu_ah :  /*  23 , ah rt, ra, rb    ah Add Halfword 58  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	break;
    case ps3spu_ahi :  /*  24 , ahi rt, ra, s10    ahi Add Halfword Immediate 59  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFS_IFSIGN|OOF_SIGNED|OOFW_16);//out_long(cmd.Op3.value, 16);
    	break;
    case ps3spu_a :  /*  25 , a rt, ra, rb    a Add Word 60  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_ai :  /*  26 , ai rt, ra, s10    ai Add Word Immediate 61  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFS_IFSIGN|OOF_SIGNED|OOFW_32);//out_long(cmd.Op3.value, 16);
    	;
    	break;
    case ps3spu_sfh :  /*  27 , sfh rt, ra, rb    sfh Subtract from Halfword 62  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_sfhi :  /*  28 , sfhi rt, ra, s10    sfhi Subtract from Halfword Immediate 63  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFS_IFSIGN|OOF_SIGNED|OOFW_16);//out_long(cmd.Op3.value, 16);
    	;
    	break;
    case ps3spu_sf :  /*  29 , sf rt, ra, rb    sf Subtract from Word 64  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_sfi :  /*  30 , sfi rt, ra, s10    sfi Subtract from Word Immediate 65  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFS_IFSIGN|OOF_SIGNED|OOFW_32);//out_long(cmd.Op3.value, 16);
    	;
    	break;
    case ps3spu_addx :  /*  31 , addx rt, ra, rb    addx Add Extended 66  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_cg :  /*  32 , cg rt, ra, rb    cg Carry Generate 67  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_cgx :  /*  33 , cgx rt, ra, rb    cgx Carry Generate Extended 68  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_sfx :  /*  34 , sfx rt, ra, rb    sfx Subtract from Extended 69  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_bg :  /*  35 , bg rt, ra, rb    bg Borrow Generate 70  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_bgx :  /*  36 , bgx rt, ra, rb    bgx Borrow Generate Extended 71  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_mpy :  /*  37 , mpy rt, ra, rb    mpy Multiply 72  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_mpyu :  /*  38 , mpyu rt, ra, rb    mpyu Multiply Unsigned 73  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_mpyi :  /*  39 , mpyi rt, ra, s10    mpyi Multiply Immediate 74  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFS_IFSIGN|OOF_SIGNED|OOFW_32);//out_long(cmd.Op3.value, 16);
    	;
    	break;
    case ps3spu_mpyui :  /*  40 , mpyui rt, ra, s10    mpyui Multiply Unsigned Immediate 75  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_32);//out_long(cmd.Op3.value, 16);
    	;
    	break;
    case ps3spu_mpya :  /*  41 , mpya rt, ra, rb, rc    mpya Multiply and Add 76  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op4.value);
    	break;
    case ps3spu_mpyh :  /*  42 , mpyh rt, ra, rb    mpyh Multiply High 77  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_mpys :  /*  43 , mpys rt, ra, rb    mpys Multiply and Shift Right 78  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_mpyhh :  /*  44 , mpyhh rt, ra, rb    mpyhh Multiply High High 79  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_mpyhha :  /*  45 , mpyhha rt, ra, rb    mpyhha Multiply High High and Add 80  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_mpyhhu :  /*  46 , mpyhhu rt, ra, rb    mpyhhu Multiply High High Unsigned 81  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_mpyhhau :  /*  47 , mpyhhau rt, ra, rb    mpyhhau Multiply High High Unsigned and Add 82  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_clz :  /*  48 , clz rt, ra    clz Count Leading Zeros 83  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);;
		;
    	;
    	break;
    case ps3spu_cntb :  /*  49 , cntb rt, ra    cntb Count Ones in Bytes 84  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);;
		;
    	;
    	break;
    case ps3spu_fsmb :  /*  50 , fsmb rt, ra    fsmb Form Select Mask for Bytes 85  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);;
		;
    	;
    	break;
    case ps3spu_fsmh :  /*  51 , fsmh rt, ra    fsmh Form Select Mask for Halfwords 86  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);;
		;
    	;
    	break;
    case ps3spu_fsm :  /*  52 , fsm rt, ra    fsm Form Select Mask for Words 87  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);;
		;
    	;
    	break;
    case ps3spu_gbb :  /*  53 , gbb rt, ra    gbb Gather Bits from Bytes 88  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);;
		;
    	;
    	break;
    case ps3spu_gbh :  /*  54 , gbh rt, ra    gbh Gather Bits from Halfwords 89  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);;
		;
    	;
    	break;
    case ps3spu_gb :  /*  55 , gb rt, ra    gb Gather Bits from Words 90  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);;
		;
    	;
    	break;
    case ps3spu_avgb :  /*  56 , avgb rt, ra, rb    avgb Average Bytes 91  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_absdb :  /*  57 , absdb rt, ra, rb    absdb Absolute Differences of Bytes 92  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_sumb :  /*  58 , sumb rt, ra, rb    sumb Sum Bytes into Halfwords 93  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_xsbh :  /*  59 , xsbh rt, ra    xsbh Extend Sign Byte to Halfword 94  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);;
		;
    	;
    	break;
    case ps3spu_xshw :  /*  60 , xshw rt, ra    xshw Extend Sign Halfword to Word 95  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);;
		;
    	;
    	break;
    case ps3spu_xswd :  /*  61 , xswd rt, ra    xswd Extend Sign Word to Doubleword 96  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);;
		;
    	;
    	break;
    case ps3spu_and :  /*  62 , and rt, ra, rb    and And 97  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_andc :  /*  63 , andc rt, ra, rb    andc And with Complement 98  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_andbi :  /*  64 , andbi rt, ra, s10    andbi And Byte Immediate 99  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_8);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_andhi :  /*  65 , andhi rt, ra, s10    andhi And Halfword Immediate 100  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_16);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_andi :  /*  66 , andi rt, ra, s10    andi And Word Immediate 101  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_32);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_or :  /*  67 , or rt, ra, rb    or Or 102  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_orc :  /*  68 , orc rt, ra, rb    orc Or with Complement 103  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_orbi :  /*  69 , orbi rt, ra, s10    orbi Or Byte Immediate 104  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_8);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_orhi :  /*  70 , orhi rt, ra, s10    orhi Or Halfword Immediate 105  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_16);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_ori :  /*  71 , ori rt, ra, s10    ori Or Word Immediate 106  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_32);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_orx :  /*  72 , orx rt, ra    orx Or Across 107  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);;
    	;
    	;
    	break;
    case ps3spu_xor :  /*  73 , xor rt, ra, rb    xor Exclusive Or 108  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_xorbi :  /*  74 , xorbi rt, ra, s10    xorbi Exclusive Or Byte Immediate 109  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_8);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_xorhi :  /*  75 , xorhi rt, ra, s10    xorhi Exclusive Or Halfword Immediate 110  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_16);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_xori :  /*  76 , xori rt, ra, s10    xori Exclusive Or Word Immediate 111  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_32);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_nand :  /*  77 , nand rt, ra, rb    nand Nand 112  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_nor :  /*  78 , nor rt, ra, rb    nor Nor 113  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_eqv :  /*  79 , eqv rt, ra, rb    eqv Equivalent 114  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_selb :  /*  80 , selb rt, ra, rb, rc    selb Select Bits 115  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op4.value);
    	break;
    case ps3spu_shufb :  /*  81 , shufb rt, ra, rb, rc    shufb Shuffle Bytes 116  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op4.value);
    	break;
    case ps3spu_shlh :  /*  82 , shlh rt, ra, rb    shlh Shift Left Halfword 118  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_shlhi :  /*  83 , shlhi rt, ra, u5    shlhi Shift Left Halfword Immediate 119  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_8);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_shl :  /*  84 , shl rt, ra, rb    shl Shift Left Word 120  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_shli :  /*  85 , shli rt, ra, u6    shli Shift Left Word Immediate 121  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_8);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_shlqbi :  /*  86 , shlqbi rt, ra, rb    shlqbi Shift Left Quadword by Bits 122  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_shlqbii :  /*  87 , shlqbii rt, ra, u3    shlqbii Shift Left Quadword by Bits Immediate 123  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_8);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_shlqby :  /*  88 , shlqby rt, ra, rb    shlqby Shift Left Quadword by Bytes 124  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_shlqbyi :  /*  89 , shlqbyi rt, ra, u5    shlqbyi Shift Left Quadword by Bytes Immediate 125  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_8);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_shlqbybi :  /*  90 , shlqbybi rt, ra, rb    shlqbybi Shift Left Quadword by Bytes from Bit Shift Count 126  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_roth :  /*  91 , roth rt, ra, rb    roth Rotate Halfword 127  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_rothi :  /*  92 , rothi rt, ra, s7    rothi Rotate Halfword Immediate 128  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_8);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_rot :  /*  93 , rot rt, ra, rb    rot Rotate Word 129  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_roti :  /*  94 , roti rt, ra, s7    roti Rotate Word Immediate 130  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_8);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_rotqby :  /*  95 , rotqby rt, ra, rb    rotqby Rotate Quadword by Bytes 131  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_rotqbyi :  /*  96 , rotqbyi rt, ra, s7    rotqbyi Rotate Quadword by Bytes Immediate 132  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_8);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_rotqbybi :  /*  97 , rotqbybi rt, ra, rb    rotqbybi Rotate Quadword by Bytes from Bit Shift Count 133  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_rotqbi :  /*  98 , rotqbi rt, ra, rb    rotqbi Rotate Quadword by Bits 134  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_rotqbii :  /*  99 , rotqbii rt, ra, u3    rotqbii Rotate Quadword by Bits Immediate 135  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_8);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_rothm :  /*  100 , rothm rt, ra, rb    rothm Rotate and Mask Halfword 136  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_rothmi :  /*  101 , rothmi rt, ra, s6    rothmi Rotate and Mask Halfword Immediate 137  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_8);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_rotm :  /*  102 , rotm rt, ra, rb    rotm Rotate and Mask Word 138  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_rotmi :  /*  103 , rotmi rt, ra, s7    rotmi Rotate and Mask Word Immediate 139  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_8);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_rotqmby :  /*  104 , rotqmby rt, ra, rb    rotqmby Rotate and Mask Quadword by Bytes 140  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_rotqmbyi :  /*  105 , rotqmbyi rt,ra,value    rotqmbyi Rotate and Mask Quadword by Bytes Immediate 141  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_8);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_rotqmbybi :  /*  106 , rotqmbybi rt, ra, rb    rotqmbybi Rotate and Mask Quadword Bytes from Bit Shift Count 142  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_rotqmbi :  /*  107 , rotqmbi rt, ra, rb    rotqmbi Rotate and Mask Quadword by Bits 143  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_rotqmbii :  /*  108 , rotqmbii rt, ra, s3    rotqmbii Rotate and Mask Quadword by Bits Immediate 144  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_8);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_rotmah :  /*  109 , rotmah rt, ra, rb    rotmah Rotate and Mask Algebraic Halfword 145  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_rotmahi :  /*  110 , rotmahi rt, ra, s6    rotmahi Rotate and Mask Algebraic Halfword Immediate 146  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_8);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_rotma :  /*  111 , rotma rt, ra, rb    rotma Rotate and Mask Algebraic Word 147  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
    	;
    	break;
    case ps3spu_rotmai :  /*  112 , rotmai rt, ra, s7    rotmai Rotate and Mask Algebraic Word Immediate 148  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_8);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_heq :  /*  113 , heq ra,rb    heq Halt If Equal 150  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);;
		;
    	;
    	break;
    case ps3spu_heqi :  /*  114 , heqi ra,symbol    heqi Halt If Equal Immediate 151  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	out_long(cmd.Op2.value, 16);;
    	;
    	;
    	break;
    case ps3spu_hgt :  /*  115 , hgt ra,rb    hgt Halt If Greater Than 152  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);
		;
    	;
    	break;
    case ps3spu_hgti :  /*  116 , hgti ra,symbol    hgti Halt If Greater Than Immediate 153  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	out_long(cmd.Op2.value, 16);;
    	;
    	;
    	break;
    case ps3spu_hlgt :  /*  117 , hlgt ra,rb    hlgt Halt If Logically Greater Than 154  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);
		;
    	;
    	break;
    case ps3spu_hlgti :  /*  118 , hlgti ra,symbol    hlgti Halt If Logically Greater Than Immediate 155  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	out_long(cmd.Op2.value, 16);;
    	;
    	;
    	break;
    case ps3spu_ceqb :  /*  119 , ceqb rt,ra,rb    ceqb Compare Equal Byte 156  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_ceqbi :  /*  120 , ceqbi rt,ra,value    ceqbi Compare Equal Byte Immediate 157  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_8);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_ceqh :  /*  121 , ceqh rt,ra,rb    ceqh Compare Equal Halfword 158  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_ceqhi :  /*  122 , ceqhi rt,ra,value    ceqhi Compare Equal Halfword Immediate 159  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_16);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_ceq :  /*  123 , ceq rt,ra,rb    ceq Compare Equal Word 160  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_ceqi :  /*  124 , ceqi rt,ra,value    ceqi Compare Equal Word Immediate 161  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3, OOFW_32);//out_long(cmd.Op3.value, 16);;
    	;
    	;
    	break;
    case ps3spu_cgtb :  /*  125 , cgtb rt,ra,rb    cgtb Compare Greater Than Byte 162  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_cgtbi :  /*  126 , cgtbi rt,ra,value    cgtbi Compare Greater Than Byte Immediate 163  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3,OOFS_IFSIGN| OOF_SIGNED | OOFW_8);//out_long(cmd.Op3.value, 16);;
    	;
    	;
    	break;
    case ps3spu_cgth :  /*  127 , cgth rt,ra,rb    cgth Compare Greater Than Halfword 164  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_cgthi :  /*  128 , cgthi rt,ra,value    cgthi Compare Greater Than Halfword Immediate 165  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3,OOFS_IFSIGN| OOF_SIGNED | OOFW_16);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_cgt :  /*  129 , cgt rt,ra,rb    cgt Compare Greater Than Word 166  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_cgti :  /*  130 , cgti rt,ra,value    cgti Compare Greater Than Word Immediate 167  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3,OOFS_IFSIGN| OOF_SIGNED | OOFW_32);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_clgtb :  /*  131 , clgtb rt,ra,rb    clgtb Compare Logical Greater Than Byte 168  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_clgtbi :  /*  132 , clgtbi rt,ra,value    clgtbi Compare Logical Greater Than Byte Immediate 169  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3,OOFW_8);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_clgth :  /*  133 , clgth rt,ra,rb    clgth Compare Logical Greater Than Halfword 170  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_clgthi :  /*  134 , clgthi rt,ra,value    clgthi Compare Logical Greater Than Halfword Immediate 171  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3,OOFW_16);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_clgt :  /*  135 , clgt rt,ra,rb    clgt Compare Logical Greater Than Word 172  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_clgti :  /*  136 , clgti rt,ra,value    clgti Compare Logical Greater Than Word Immediate 173  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3,OOFW_32);//out_long(cmd.Op3.value, 16);;
    	;
    	break;
    case ps3spu_br :  /*  137 , br symbol    br Branch Relative 174  */ 
    	if ( !out_name_expr(cmd.Op1, cmd.Op1.value, cmd.Op1.value) )out_bad_address(cmd.Op1.value);//out_long(cmd.Op2.value, 16);;
    	;
    	;
    	;
    	break;
    case ps3spu_bra :  /*  138 , bra symbol    bra Branch Absolute 175  */ 
    	if ( !out_name_expr(cmd.Op1, cmd.Op1.value, cmd.Op1.value) )out_bad_address(cmd.Op1.value);//out_long(cmd.Op2.value, 16);;
    	;
    	;
    	;
    	break;
    case ps3spu_brsl :  /*  139 , brsl rt,symbol    brsl Branch Relative and Set Link in rt 176  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	if ( !out_name_expr(cmd.Op2, cmd.Op2.value, cmd.Op2.value) )out_bad_address(cmd.Op2.value);//out_long(cmd.Op2.value, 16);;
    	;
    	;
    	break;
    case ps3spu_brasl :  /*  140 , brasl rt,symbol    brasl Branch Absolute and Set Link in rt 177  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	if ( !out_name_expr(cmd.Op2, cmd.Op2.value, cmd.Op2.value) )out_bad_address(cmd.Op2.value);//out_long(cmd.Op2.value, 16);;
    	;
    	;
    	break;
    case ps3spu_bi :  /*  141 , bi ra    bi Branch Indirect 178  */ 
    	OutReg(cmd.Op1.value);;
    	;
    	;
    	;
    	break;
    case ps3spu_iret :  /*  142 , iret ra    iret Interrupt Return 179  */ 
    	OutReg(cmd.Op1.value);;
    	;
    	;
    	;
    	break;
    case ps3spu_bisled :  /*  143 , bisled rt,ra    bisled Branch Indirect and Set Link if External Data 180  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);
		;
    	;
    	break;
    case ps3spu_bisl :  /*  144 , bisl rt,ra    bisl Branch Indirect and Set Link 181  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);
		;
    	;
    	break;
    case ps3spu_brnz :  /*  145 , brnz rt,symbol    brnz Branch If Not Zero Word 182  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	if ( !out_name_expr(cmd.Op2, cmd.Op2.value, cmd.Op2.value) )out_bad_address(cmd.Op2.value);//out_long(cmd.Op2.value, 16);;
    	;
    	;
    	break;
    case ps3spu_brz :  /*  146 , brz rt,symbol    brz Branch If Zero Word 183  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	if ( !out_name_expr(cmd.Op2, cmd.Op2.value, cmd.Op2.value) )out_bad_address(cmd.Op2.value);//out_long(cmd.Op2.value, 16);;
    	;
    	;
    	break;
    case ps3spu_brhnz :  /*  147 , brhnz rt,symbol    brhnz Branch If Not Zero Halfword 184  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	if ( !out_name_expr(cmd.Op2, cmd.Op2.value, cmd.Op2.value) )out_bad_address(cmd.Op2.value);//out_long(cmd.Op2.value, 16);;
    	;
    	;
    	break;
    case ps3spu_brhz :  /*  148 , brhz rt,symbol    brhz Branch If Zero Halfword 185  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	if ( !out_name_expr(cmd.Op2, cmd.Op2.value, cmd.Op2.value) )out_bad_address(cmd.Op2.value);//out_long(cmd.Op2.value, 16);;
    	;
    	;
    	break;
    case ps3spu_biz :  /*  149 , biz rt,ra    biz Branch Indirect If Zero 186  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);
		;
    	;
    	break;
    case ps3spu_binz :  /*  150 , binz rt,ra    binz Branch Indirect If Not Zero 187  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);
		;
    	;
    	break;
    case ps3spu_bihz :  /*  151 , bihz rt,ra    bihz Branch Indirect If Zero Halfword 188  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);
		;
    	;
    	break;
    case ps3spu_bihnz :  /*  152 , bihnz rt,ra    bihnz Branch Indirect If Not Zero Halfword 189  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);
		;
    	;
    	break;
    case ps3spu_hbr :  /*  153 , hbr brinst,brtarg    hbr Hint for Branch (r-form) 192  */ 
    	if ( !out_name_expr(cmd.Op1, cmd.Op1.value, cmd.Op1.value) )out_bad_address(cmd.Op1.value);//out_long(cmd.Op2.value, 16);;
    	out_symbol(',');OutChar(' ');OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutLine(" P = ");out_long(cmd.Op3.value,16);
		;
    	break;
    case ps3spu_hbra :  /*  154 , hbra brinst,brtarg    hbra Hint for Branch (a-form) 193  */ 
    	if ( !out_name_expr(cmd.Op1, cmd.Op1.value, cmd.Op1.value) )out_bad_address(cmd.Op1.value);;out_symbol(',');OutChar(' ');;
    	if ( !out_name_expr(cmd.Op2, cmd.Op2.value, cmd.Op2.value) )out_bad_address(cmd.Op2.value);//out_long(cmd.Op2.value, 16);;
    	;
    	;
    	break;
    case ps3spu_hbrr :  /*  155 , hbrr brinst,brtarg    hbrr Hint for Branch Relative 194  */ 
    	if ( !out_name_expr(cmd.Op1, cmd.Op1.value, cmd.Op1.value) )out_bad_address(cmd.Op1.value);;out_symbol(',');OutChar(' ');;
    	if ( !out_name_expr(cmd.Op2, cmd.Op2.value, cmd.Op2.value) )out_bad_address(cmd.Op2.value);//out_long(cmd.Op2.value, 16);;
    	;
    	;
    	break;
    case ps3spu_fa :  /*  156 , fa rt,ra,rb    fa Floating Add 202  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_dfa :  /*  157 , dfa rt,ra,rb    dfa Double Floating Add 203  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_fs :  /*  158 , fs rt,ra,rb    fs Floating Subtract 204  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_dfs :  /*  159 , dfs rt,ra,rb    dfs Double Floating Subtract 205  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_fm :  /*  160 , fm rt,ra,rb    fm Floating Multiply 206  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_dfm :  /*  161 , dfm rt,ra,rb    dfm Double Floating Multiply 207  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_fma :  /*  162 , fma rt,ra,rb,rc    fma Floating Multiply and Add 208  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op4.value);
    	break;
    case ps3spu_dfma :  /*  163 , dfma rt,ra,rb    dfma Double Floating Multiply and Add 209  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_fnms :  /*  164 , fnms rt,ra,rb,rc    fnms Floating Negative Multiply and Subtract 210  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op4.value);
    	break;
    case ps3spu_dfnms :  /*  165 , dfnms rt,ra,rb    dfnms Double Floating Multiply and Subtract 211  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_fms :  /*  166 , fms rt,ra,rb,rc    fms Floating Multiply and Subtract 212  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op4.value);
    	break;
    case ps3spu_dfms :  /*  167 , dfms rt,ra,rb    dfms Double Floating Multiply and Subtract 213  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_dfnma :  /*  168 , dfnma rt,ra,rb    dfnma Double Floating Negative Multiply and Add 214  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_frest :  /*  169 , frest rt,ra    frest Floating Reciprocal Estimate 215  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);
    	;
    	;
    	break;
    case ps3spu_frsqest :  /*  170 , frsqest rt,ra    frsqest Floating Reciprocal Absolute Square Root Estimate 217  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);
    	;
    	;
    	break;
    case ps3spu_fi :  /*  171 , fi rt,ra,rb    fi Floating Interpolate 219  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_csflt :  /*  172 , csflt rt,ra,scale    csflt Convert Signed Integer to Floating 220  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		out_long(cmd.Op3.value,16);
		;
    	break;
    case ps3spu_cflts :  /*  173 , cflts rt,ra,scale    cflts Convert Floating to Signed Integer 221  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		out_long(cmd.Op3.value,16);
		;
    	break;
    case ps3spu_cuflt :  /*  174 , cuflt rt,ra,scale    cuflt Convert Unsigned Integer to Floating 222  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		out_long(cmd.Op3.value,16);
		;
    	break;
    case ps3spu_cfltu :  /*  175 , cfltu rt,ra,scale    cfltu Convert Floating to Unsigned Integer 223  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		out_long(cmd.Op3.value,16);
		;
    	break;
    case ps3spu_frds :  /*  176 , frds rt,ra    frds Floating Round Double to Single 224  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);;
    	;
    	;
    	break;
    case ps3spu_fesd :  /*  177 , fesd rt,ra    fesd Floating Extend Single to Double 225  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);;
    	;
    	;
    	break;
    case ps3spu_dfceq :  /*  178 , dfceq rt,ra,rb    dfceq Double Floating Compare Equal 226  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_dfcmeq :  /*  179 , dfcmeq rt,ra,rb    dfcmeq Double Floating Compare Magnitude Equal 227  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_dfcgt :  /*  180 , dfcgt rt,ra,rb    dfcgt Double Floating Compare Greater Than 228  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_dfcmgt :  /*  181 , dfcmgt rt,ra,rb    dfcmgt Double Floating Compare Magnitude Greater Than 229  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_dftsv :  /*  182 , dftsv rt,ra,value    dftsv Double Floating Test Special Value 230  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutValue(cmd.Op3,OOFW_8);//out_long(cmd.Op3.value,16);
		;
    	break;
    case ps3spu_fceq :  /*  183 , fceq rt,ra,rb    fceq Floating Compare Equal 231  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_fcmeq :  /*  184 , fcmeq rt,ra,rb    fcmeq Floating Compare Magnitude Equal 232  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_fcgt :  /*  185 , fcgt rt,ra,rb    fcgt Floating Compare Greater Than 233  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_fcmgt :  /*  186 , fcmgt rt,ra,rb    fcmgt Floating Compare Magnitude Greater Than 234  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op3.value);
		;
    	break;
    case ps3spu_fscrrd :  /*  187 , fscrwr ra    fscrrd Floating-Point Status and Control Register Write 235  */ 
    	OutReg(cmd.Op1.value);
    	;
    	;
    	;
    	break;
    case ps3spu_fscrwr :  /*  188 , fscrrd rt    fscrwr Floating-Point Status and Control Register Read 236  */ 
    	OutReg(cmd.Op1.value);
    	;
    	;
    	;
    	break;
    case ps3spu_stop :  /*  189 , stop    stop Stop and Signal 238  */ 
    	;
    	;
    	;
    	;
    	break;
    case ps3spu_stopd :  /*  190 , stopd    stopd Stop and Signal with Dependencies 239  */ 
    	;
    	;
    	;
    	;
    	break;
    case ps3spu_lnop :  /*  191 , lnop    lnop No Operation (Load) 240  */ 
    	;
    	;
    	;
    	;
    	break;
    case ps3spu_nop :  /*  192 , nop    nop No Operation (Execute) 241  */ 
    	;
    	;
    	;
    	;
    	break;
    case ps3spu_sync :  /*  193 , sync    sync Synchronize 242  */ 
    	;
    	;
    	;
    	;
    	break;
    case ps3spu_dsync :  /*  194 , dsync    dsync Synchronize Data 243  */ 
    	;
    	;
    	;
    	;
    	break;
    case ps3spu_mfspr :  /*  195 , mfspr rt,sa    mfspr Move from Special-Purpose Register 244  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value+128);
		;
    	;
    	break;
    case ps3spu_mtspr :  /*  196 , mtspr sa, rt    mtspr Move to Special-Purpose Register 245  */ 
    	OutReg(cmd.Op1.value+128);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);
		;
    	;
    	break;
    case ps3spu_rdch :  /*  197 , rdch rt,ca    rdch Read Channel 248  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value+256);//OutLine("channel ");out_long(cmd.Op2.value,16);
		;
    	;
    	break;
    case ps3spu_rchcnt :  /*  198 , rchcnt rt,ca    rchcnt Read Channel Count 249  */ 
    	OutReg(cmd.Op1.value);out_symbol(',');OutChar(' ');
		OutReg(cmd.Op2.value+256);//OutLine("channel ");OutValue(cmd.Op2,OOFW_8);//out_long(cmd.Op2.value,16);;
    	;
    	;
    	break;
    case ps3spu_wrch :  /*  199 , wrch ca,rt    wrch Write Channel 250  */ 
    	OutReg(cmd.Op1.value+256);out_symbol(',');OutChar(' ');//OutLine("channel ");OutValue(cmd.Op2,OOFW_8);out_symbol(',');OutChar(' ');//out_long(cmd.Op1.value,16);;out_symbol(',');OutChar(' ');
    	OutReg(cmd.Op2.value);
		;
    	;
    	break;



	

	default:
		warning("%a: bad logic (out.cpp:switch)", cmd.ea);
		return ;//0;
	}	



  // output a character representation of the immediate values
  // embedded in the instruction as comments

//  if ( isVoid(cmd.ea,uFlag,0) ) OutImmChar(cmd.Op1);
//  if ( isVoid(cmd.ea,uFlag,1) ) OutImmChar(cmd.Op2);
//  if ( isVoid(cmd.ea,uFlag,2) ) OutImmChar(cmd.Op3);

  term_output_buffer();                 // terminate the output string
  gl_comm = 1;                          // ask to attach a possible user-
                                        // defined comment to it
  MakeLine(buf);                        // pass the generated line to the
                                        // kernel
}
// codes below are not used at all, shall be removed later.
//--------------------------------------------------------------------------
// generate start of the disassembly
bool outop(op_t &x)
{
  switch ( x.type )
  {

    case o_void:
      return 0;

    case o_reg:
      if ( cmd.ea != BADADDR )
      {
        char buf[MAXSTR];
        const char *name = get_name(cmd.ea, cmd.ea+x.value, buf, sizeof(buf));//need revised
        if ( name != NULL )
        {
          out_register(name);
          break;
        }
      }
      out_register(ph.regNames[x.reg]);
      break;

    case o_imm:
      OutValue(x, OOFS_IFSIGN|OOF_SIGNED|OOFW_8);
      break;

    case o_near:
/*      {
        ea_t ea = toEA(cmd.cs, x.addr);
        if ( !out_name_expr(x, ea, x.addr) )
          out_bad_address(x.addr);
      }
 */     break;

    case o_mem:
      /*{
        ea_t ea = toEA(dataSeg(), x.addr);
        if ( !out_name_expr(x, ea, x.addr) )
          out_bad_address(x.addr);
      }*/
      break;

    default:
      warning("out: %a: bad optype %d", cmd.ea, x.type);
      break;
  }
  return 1;
}
//--------------------------------------------------------------------------
void segstart(ea_t ea)
{
}

//--------------------------------------------------------------------------
void segend(ea_t)
{
}

//--------------------------------------------------------------------------
void header(void)
{
  gen_cmt_line("Processor       : %-8.8s", inf.procName);
  //gen_cmt_line("Target assembler: %s", ash.name);
  gen_cmt_line("Byte sex        : %s", inf.mf ? "Big endian" : "Little endian");
  gen_cmt_line("By Jack Chen, aka anita999. credits to Bluemimmosa.");
  if ( ash.header != NULL )
    for ( const char **ptr=ash.header; *ptr != NULL; ptr++ )
      printf_line(0,COLSTR("%s",SCOLOR_ASMDIR),*ptr);
}

//--------------------------------------------------------------------------
void footer(void)
{
  char name[MAXSTR];
  get_name(BADADDR, inf.beginEA, name, sizeof(name));
  printf_line(inf.indent,COLSTR("%s",SCOLOR_ASMDIR)
                " "
                COLSTR("%s %s",SCOLOR_AUTOCMT), ash.end, ash.cmnt, name);
}
