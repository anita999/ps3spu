/*
 *      ps3spu module for the Interactive disassembler (IDA).
 *
 *      updates, fixes and bugreports welcomed (you know where i am)
 *
 *      (w)2011 by Jack Chen, aka anita999
 */

#include "ps3spu.hpp"


void emu_data_xref(long addr, int dtype,int loading)
{
   	ea_t dea = (ea_t)addr;//map_addr(x.addr, x.n, true);
    ua_dodata(dea, dtype);
    if ( !loading ) doVar(dea);     // write access
    ua_add_dref(0, dea, loading ? dr_R : dr_W);;
}

int flow=0; // flow==1 for execution flow could go through next adjacent opcode, flow==0 for execution must stop here, the flow might be
            // either stopped or jump to other address
void emu_code_xref(long addr, int faraddr)//faraddr==1 for far call or jmp, ==0 for near call or jmp.
{
        ea_t ea = addr;
        int iscall = InstrIsSet(cmd.itype, CF_CALL);
        if(faraddr==1)ua_add_cref(0, ea, iscall ? fl_CF : fl_JF);
		else ua_add_cref(0, ea, iscall ? fl_CN : fl_JN);
        if ( flow && iscall )
        {
          func_t *pfn = get_func(ea);
          if ( pfn != NULL && (pfn->flags & FUNC_NORET) ) flow = false;
        }
}  
//----------------------------------------------------------------------
// Emulate an instruction
// This function should:
//      - create all xrefs from the instruction
//      - perform any additional analysis of the instruction/program
//        and convert the instruction operands, create comments, etc.
//      - create stack variables
//      - analyse the delayed branches and similar constructs
// The kernel calls ana() before calling emu(), so you may be sure that
// the 'cmd' structure contains a valid and up-to-date information.
// You are not allowed to modify the 'cmd' structure.
// Upon entering this function, the 'uFlag' variable contains the flags of
// cmd.ea. If you change the characteristics of the current instruction, you
// are required to refresh 'uFlag'.
// Usually the kernel calls emu() with consecutive addresses in cmd.ea but
// you can't rely on this - for example, if the user asks to analyse an
// instruction at arbirary address, his request will be handled immediately,
// thus breaking the normal sequence of emulation.
// If you need to analyse the surroundings of the current instruction, you
// are allowed to save the contents of the 'cmd' structure and call ana().
// For example, this is a very common pattern:
//  {
//    insn_t saved = cmd;
//    if ( decode_prev_insn(cmd.ea) != BADADDR )
//    {
//      ....
//    }
//    cmd = saved;
//  }
//
// This sample emu() function is a very simple emulation engine.

int emu(void)
{
  int Feature = Instructions[cmd.itype].feature;
  flow = ((Feature & CF_STOP) == 0);

 // if the execution flow is not stopped here, then create
  // a xref to the next instruction.
  // Thus we plan to analyse the next instruction.

  if ( flow )
  {
	  ua_add_cref(0,cmd.ea+cmd.size,fl_F);
  }

	switch (cmd.itype)
	{

//    SU64 immval;

    case ps3spu_lqd :  /*  1 , lqd rt, s14(ra)    lqd Load Quadword (d-form) 32 , op2=I10 extends with sign, then shift left 4 bits */     	
    	break;
    case ps3spu_lqx :  /*  2 , lqx rt, ra, rb    lqx Load Quadword (x-form) 33  */ 
    	break;
    case ps3spu_lqa :  /*  3 , lqa rt, s18    lqa Load Quadword (a-form) 34  */ 
    	emu_data_xref(cmd.Op2.value, dt_dword,1); //1 for loading, 0 for writing
    	break;
    case ps3spu_lqr :  /*  4 , lqr rt, s18    lqr Load Quadword Instruction Relative (a-form) 35  */ 
    	emu_data_xref(cmd.Op2.value, dt_dword,1); //1 for loading, 0 for writing
    	break;
    case ps3spu_stqd :  /*  5 , stqd rt, s14(ra)    stqd Store Quadword (d-form) 36  */ 
    	break;
    case ps3spu_stqx :  /*  6 , stqx rc, ra, rb    stqx Store Quadword (x-form) 37  */ 
    	break;
    case ps3spu_stqa :  /*  7 , stqa rt, s18    stqa Store Quadword (a-form) 38  */ 
    	emu_data_xref(cmd.Op2.value, dt_dword,0); //1 for loading, 0 for writing
    	break;
    case ps3spu_stqr :  /*  8 , stqr rt, s18    stqr Store Quadword Instruction Relative (a-form) 39  */ 
    	emu_data_xref(cmd.Op2.value, dt_dword,0); //1 for loading, 0 for writing
    	break;
    case ps3spu_cbd :  /*  9 , cbd rt, u7(ra)    cbd Generate Controls for Byte Insertion (d-form) 40  */ 
    	break;
    case ps3spu_cbx :  /*  10 , cbx rt, ra, rb    cbx Generate Controls for Byte Insertion (x-form) 41  */ 
    	break;
    case ps3spu_chd :  /*  11 , chd rt, u7(ra)    chd Generate Controls for Halfword Insertion (d-form) 42  */ 
    	break;
    case ps3spu_chx :  /*  12 , chx rt, ra, rb    chx Generate Controls for Halfword Insertion (x-form) 43  */ 
    	break;
    case ps3spu_cwd :  /*  13 , cwd rt, u7(ra)    cwd Generate Controls for Word Insertion (d-form) 44  */ 
    	break;
    case ps3spu_cwx :  /*  14 , cwx rt, ra, rb    cwx Generate Controls for Word Insertion (x-form) 45  */ 
    	break;
    case ps3spu_cdd :  /*  15 , cdd rt, u7(ra)    cdd Generate Controls for Doubleword Insertion (d-form) 46  */ 
    	break;
    case ps3spu_cdx :  /*  16 , cdx rt, ra, rb    cdx Generate Controls for Doubleword Insertion (x-form) 47  */ 
    	break;
    case ps3spu_ilh :  /*  17 , ilh rt, u16    ilh Immediate Load Halfword 50  */ 
    	break;
    case ps3spu_ilhu :  /*  18 , ilhu rt, u16    ilhu Immediate Load Halfword Upper 51  */ 
    	break;
    case ps3spu_il :  /*  19 , il rt, s16    il Immediate Load Word 52  */ 
    	break;
    case ps3spu_ila :  /*  20 , ila rt, u18    ila Immediate Load Address 53  */ 
    	emu_data_xref(cmd.Op2.value, dt_dword,1); //1 for loading, 0 for writing
    	;
    	break;
    case ps3spu_iohl :  /*  21 , iohl rt, u16    iohl Immediate Or Halfword Lower 54  */ 
    	break;
    case ps3spu_fsmbi :  /*  22 , fsmbi rt, u16    fsmbi Form Select Mask for Bytes Immediate 55  */ 
    	break;
    case ps3spu_ah :  /*  23 , ah rt, ra, rb    ah Add Halfword 58  */ 
    	break;
    case ps3spu_ahi :  /*  24 , ahi rt, ra, s10    ahi Add Halfword Immediate 59  */ 
    	break;
    case ps3spu_a :  /*  25 , a rt, ra, rb    a Add Word 60  */ 
    	break;
    case ps3spu_ai :  /*  26 , ai rt, ra, s10    ai Add Word Immediate 61  */ 
    	break;
    case ps3spu_sfh :  /*  27 , sfh rt, ra, rb    sfh Subtract from Halfword 62  */ 
    	break;
    case ps3spu_sfhi :  /*  28 , sfhi rt, ra, s10    sfhi Subtract from Halfword Immediate 63  */ 
    	break;
    case ps3spu_sf :  /*  29 , sf rt, ra, rb    sf Subtract from Word 64  */ 
    	break;
    case ps3spu_sfi :  /*  30 , sfi rt, ra, s10    sfi Subtract from Word Immediate 65  */ 
    	break;
    case ps3spu_addx :  /*  31 , addx rt, ra, rb    addx Add Extended 66  */ 
    	break;
    case ps3spu_cg :  /*  32 , cg rt, ra, rb    cg Carry Generate 67  */ 
    	break;
    case ps3spu_cgx :  /*  33 , cgx rt, ra, rb    cgx Carry Generate Extended 68  */ 
    	break;
    case ps3spu_sfx :  /*  34 , sfx rt, ra, rb    sfx Subtract from Extended 69  */ 
    	break;
    case ps3spu_bg :  /*  35 , bg rt, ra, rb    bg Borrow Generate 70  */ 
    	break;
    case ps3spu_bgx :  /*  36 , bgx rt, ra, rb    bgx Borrow Generate Extended 71  */ 
    	break;
    case ps3spu_mpy :  /*  37 , mpy rt, ra, rb    mpy Multiply 72  */ 
    	break;
    case ps3spu_mpyu :  /*  38 , mpyu rt, ra, rb    mpyu Multiply Unsigned 73  */ 
    	break;
    case ps3spu_mpyi :  /*  39 , mpyi rt, ra, s10    mpyi Multiply Immediate 74  */ 
    	break;
    case ps3spu_mpyui :  /*  40 , mpyui rt, ra, s10    mpyui Multiply Unsigned Immediate 75  */ 
    	break;
    case ps3spu_mpya :  /*  41 , mpya rt, ra, rb, rc    mpya Multiply and Add 76  */ 
    	break;
    case ps3spu_mpyh :  /*  42 , mpyh rt, ra, rb    mpyh Multiply High 77  */ 
    	break;
    case ps3spu_mpys :  /*  43 , mpys rt, ra, rb    mpys Multiply and Shift Right 78  */ 
    	break;
    case ps3spu_mpyhh :  /*  44 , mpyhh rt, ra, rb    mpyhh Multiply High High 79  */ 
    	break;
    case ps3spu_mpyhha :  /*  45 , mpyhha rt, ra, rb    mpyhha Multiply High High and Add 80  */ 
    	break;
    case ps3spu_mpyhhu :  /*  46 , mpyhhu rt, ra, rb    mpyhhu Multiply High High Unsigned 81  */ 
    	break;
    case ps3spu_mpyhhau :  /*  47 , mpyhhau rt, ra, rb    mpyhhau Multiply High High Unsigned and Add 82  */ 
    	break;
    case ps3spu_clz :  /*  48 , clz rt, ra    clz Count Leading Zeros 83  */ 
    	break;
    case ps3spu_cntb :  /*  49 , cntb rt, ra    cntb Count Ones in Bytes 84  */ 
    	break;
    case ps3spu_fsmb :  /*  50 , fsmb rt, ra    fsmb Form Select Mask for Bytes 85  */ 
    	break;
    case ps3spu_fsmh :  /*  51 , fsmh rt, ra    fsmh Form Select Mask for Halfwords 86  */ 
    	break;
    case ps3spu_fsm :  /*  52 , fsm rt, ra    fsm Form Select Mask for Words 87  */ 
    	break;
    case ps3spu_gbb :  /*  53 , gbb rt, ra    gbb Gather Bits from Bytes 88  */ 
    	break;
    case ps3spu_gbh :  /*  54 , gbh rt, ra    gbh Gather Bits from Halfwords 89  */ 
    	break;
    case ps3spu_gb :  /*  55 , gb rt, ra    gb Gather Bits from Words 90  */ 
    	break;
    case ps3spu_avgb :  /*  56 , avgb rt, ra, rb    avgb Average Bytes 91  */ 
    	break;
    case ps3spu_absdb :  /*  57 , absdb rt, ra, rb    absdb Absolute Differences of Bytes 92  */ 
    	break;
    case ps3spu_sumb :  /*  58 , sumb rt, ra, rb    sumb Sum Bytes into Halfwords 93  */ 
    	break;
    case ps3spu_xsbh :  /*  59 , xsbh rt, ra    xsbh Extend Sign Byte to Halfword 94  */ 
    	break;
    case ps3spu_xshw :  /*  60 , xshw rt, ra    xshw Extend Sign Halfword to Word 95  */ 
    	break;
    case ps3spu_xswd :  /*  61 , xswd rt, ra    xswd Extend Sign Word to Doubleword 96  */ 
    	break;
    case ps3spu_and :  /*  62 , and rt, ra, rb    and And 97  */ 
    	break;
    case ps3spu_andc :  /*  63 , andc rt, ra, rb    andc And with Complement 98  */ 
    	break;
    case ps3spu_andbi :  /*  64 , andbi rt, ra, s10    andbi And Byte Immediate 99  */ 
    	break;
    case ps3spu_andhi :  /*  65 , andhi rt, ra, s10    andhi And Halfword Immediate 100  */ 
    	break;
    case ps3spu_andi :  /*  66 , andi rt, ra, s10    andi And Word Immediate 101  */ 
    	break;
    case ps3spu_or :  /*  67 , or rt, ra, rb    or Or 102  */ 
    	break;
    case ps3spu_orc :  /*  68 , orc rt, ra, rb    orc Or with Complement 103  */ 
    	break;
    case ps3spu_orbi :  /*  69 , orbi rt, ra, s10    orbi Or Byte Immediate 104  */ 
    	break;
    case ps3spu_orhi :  /*  70 , orhi rt, ra, s10    orhi Or Halfword Immediate 105  */ 
    	break;
    case ps3spu_ori :  /*  71 , ori rt, ra, s10    ori Or Word Immediate 106  */ 
    	break;
    case ps3spu_orx :  /*  72 , orx rt, ra    orx Or Across 107  */ 
    	break;
    case ps3spu_xor :  /*  73 , xor rt, ra, rb    xor Exclusive Or 108  */ 
    	break;
    case ps3spu_xorbi :  /*  74 , xorbi rt, ra, s10    xorbi Exclusive Or Byte Immediate 109  */ 
    	break;
    case ps3spu_xorhi :  /*  75 , xorhi rt, ra, s10    xorhi Exclusive Or Halfword Immediate 110  */ 
    	break;
    case ps3spu_xori :  /*  76 , xori rt, ra, s10    xori Exclusive Or Word Immediate 111  */ 
    	break;
    case ps3spu_nand :  /*  77 , nand rt, ra, rb    nand Nand 112  */ 
    	break;
    case ps3spu_nor :  /*  78 , nor rt, ra, rb    nor Nor 113  */ 
    	break;
    case ps3spu_eqv :  /*  79 , eqv rt, ra, rb    eqv Equivalent 114  */ 
    	break;
    case ps3spu_selb :  /*  80 , selb rt, ra, rb, rc    selb Select Bits 115  */ 
    	break;
    case ps3spu_shufb :  /*  81 , shufb rt, ra, rb, rc    shufb Shuffle Bytes 116  */ 
    	break;
    case ps3spu_shlh :  /*  82 , shlh rt, ra, rb    shlh Shift Left Halfword 118  */ 
    	break;
    case ps3spu_shlhi :  /*  83 , shlhi rt, ra, u5    shlhi Shift Left Halfword Immediate 119  */ 
    	break;
    case ps3spu_shl :  /*  84 , shl rt, ra, rb    shl Shift Left Word 120  */ 
    	break;
    case ps3spu_shli :  /*  85 , shli rt, ra, u6    shli Shift Left Word Immediate 121  */ 
    	break;
    case ps3spu_shlqbi :  /*  86 , shlqbi rt, ra, rb    shlqbi Shift Left Quadword by Bits 122  */ 
    	break;
    case ps3spu_shlqbii :  /*  87 , shlqbii rt, ra, u3    shlqbii Shift Left Quadword by Bits Immediate 123  */ 
    	break;
    case ps3spu_shlqby :  /*  88 , shlqby rt, ra, rb    shlqby Shift Left Quadword by Bytes 124  */ 
    	break;
    case ps3spu_shlqbyi :  /*  89 , shlqbyi rt, ra, u5    shlqbyi Shift Left Quadword by Bytes Immediate 125  */ 
    	break;
    case ps3spu_shlqbybi :  /*  90 , shlqbybi rt, ra, rb    shlqbybi Shift Left Quadword by Bytes from Bit Shift Count 126  */ 
    	break;
    case ps3spu_roth :  /*  91 , roth rt, ra, rb    roth Rotate Halfword 127  */ 
    	break;
    case ps3spu_rothi :  /*  92 , rothi rt, ra, s7    rothi Rotate Halfword Immediate 128  */ 
    	break;
    case ps3spu_rot :  /*  93 , rot rt, ra, rb    rot Rotate Word 129  */ 
    	break;
    case ps3spu_roti :  /*  94 , roti rt, ra, s7    roti Rotate Word Immediate 130  */ 
    	break;
    case ps3spu_rotqby :  /*  95 , rotqby rt, ra, rb    rotqby Rotate Quadword by Bytes 131  */ 
    	break;
    case ps3spu_rotqbyi :  /*  96 , rotqbyi rt, ra, s7    rotqbyi Rotate Quadword by Bytes Immediate 132  */ 
    	break;
    case ps3spu_rotqbybi :  /*  97 , rotqbybi rt, ra, rb    rotqbybi Rotate Quadword by Bytes from Bit Shift Count 133  */ 
    	break;
    case ps3spu_rotqbi :  /*  98 , rotqbi rt, ra, rb    rotqbi Rotate Quadword by Bits 134  */ 
    	break;
    case ps3spu_rotqbii :  /*  99 , rotqbii rt, ra, u3    rotqbii Rotate Quadword by Bits Immediate 135  */ 
    	break;
    case ps3spu_rothm :  /*  100 , rothm rt, ra, rb    rothm Rotate and Mask Halfword 136  */ 
    	break;
    case ps3spu_rothmi :  /*  101 , rothmi rt, ra, s6    rothmi Rotate and Mask Halfword Immediate 137  */ 
    	break;
    case ps3spu_rotm :  /*  102 , rotm rt, ra, rb    rotm Rotate and Mask Word 138  */ 
    	break;
    case ps3spu_rotmi :  /*  103 , rotmi rt, ra, s7    rotmi Rotate and Mask Word Immediate 139  */ 
    	break;
    case ps3spu_rotqmby :  /*  104 , rotqmby rt, ra, rb    rotqmby Rotate and Mask Quadword by Bytes 140  */ 
    	break;
    case ps3spu_rotqmbyi :  /*  105 , rotqmbyi rt,ra,value    rotqmbyi Rotate and Mask Quadword by Bytes Immediate 141  */ 
    	break;
    case ps3spu_rotqmbybi :  /*  106 , rotqmbybi rt, ra, rb    rotqmbybi Rotate and Mask Quadword Bytes from Bit Shift Count 142  */ 
    	break;
    case ps3spu_rotqmbi :  /*  107 , rotqmbi rt, ra, rb    rotqmbi Rotate and Mask Quadword by Bits 143  */ 
    	break;
    case ps3spu_rotqmbii :  /*  108 , rotqmbii rt, ra, s3    rotqmbii Rotate and Mask Quadword by Bits Immediate 144  */ 
    	break;
    case ps3spu_rotmah :  /*  109 , rotmah rt, ra, rb    rotmah Rotate and Mask Algebraic Halfword 145  */ 
    	break;
    case ps3spu_rotmahi :  /*  110 , rotmahi rt, ra, s6    rotmahi Rotate and Mask Algebraic Halfword Immediate 146  */ 
    	break;
    case ps3spu_rotma :  /*  111 , rotma rt, ra, rb    rotma Rotate and Mask Algebraic Word 147  */ 
    	break;
    case ps3spu_rotmai :  /*  112 , rotmai rt, ra, s7    rotmai Rotate and Mask Algebraic Word Immediate 148  */ 
    	break;
    case ps3spu_heq :  /*  113 , heq ra,rb    heq Halt If Equal 150  */ 
    	break;
    case ps3spu_heqi :  /*  114 , heqi ra,symbol    heqi Halt If Equal Immediate 151  */ 
    	break;
    case ps3spu_hgt :  /*  115 , hgt ra,rb    hgt Halt If Greater Than 152  */ 
    	break;
    case ps3spu_hgti :  /*  116 , hgti ra,symbol    hgti Halt If Greater Than Immediate 153  */ 
    	break;
    case ps3spu_hlgt :  /*  117 , hlgt ra,rb    hlgt Halt If Logically Greater Than 154  */ 
    	break;
    case ps3spu_hlgti :  /*  118 , hlgti ra,symbol    hlgti Halt If Logically Greater Than Immediate 155  */ 
    	break;
    case ps3spu_ceqb :  /*  119 , ceqb rt,ra,rb    ceqb Compare Equal Byte 156  */ 
    	break;
    case ps3spu_ceqbi :  /*  120 , ceqbi rt,ra,value    ceqbi Compare Equal Byte Immediate 157  */ 
    	break;
    case ps3spu_ceqh :  /*  121 , ceqh rt,ra,rb    ceqh Compare Equal Halfword 158  */ 
    	break;
    case ps3spu_ceqhi :  /*  122 , ceqhi rt,ra,value    ceqhi Compare Equal Halfword Immediate 159  */ 
    	break;
    case ps3spu_ceq :  /*  123 , ceq rt,ra,rb    ceq Compare Equal Word 160  */ 
    	break;
    case ps3spu_ceqi :  /*  124 , ceqi rt,ra,value    ceqi Compare Equal Word Immediate 161  */ 
    	break;
    case ps3spu_cgtb :  /*  125 , cgtb rt,ra,rb    cgtb Compare Greater Than Byte 162  */ 
    	break;
    case ps3spu_cgtbi :  /*  126 , cgtbi rt,ra,value    cgtbi Compare Greater Than Byte Immediate 163  */ 
    	break;
    case ps3spu_cgth :  /*  127 , cgth rt,ra,rb    cgth Compare Greater Than Halfword 164  */ 
    	break;
    case ps3spu_cgthi :  /*  128 , cgthi rt,ra,value    cgthi Compare Greater Than Halfword Immediate 165  */ 
    	break;
    case ps3spu_cgt :  /*  129 , cgt rt,ra,rb    cgt Compare Greater Than Word 166  */ 
    	break;
    case ps3spu_cgti :  /*  130 , cgti rt,ra,value    cgti Compare Greater Than Word Immediate 167  */ 
    	break;
    case ps3spu_clgtb :  /*  131 , clgtb rt,ra,rb    clgtb Compare Logical Greater Than Byte 168  */ 
    	break;
    case ps3spu_clgtbi :  /*  132 , clgtbi rt,ra,value    clgtbi Compare Logical Greater Than Byte Immediate 169  */ 
    	break;
    case ps3spu_clgth :  /*  133 , clgth rt,ra,rb    clgth Compare Logical Greater Than Halfword 170  */ 
    	break;
    case ps3spu_clgthi :  /*  134 , clgthi rt,ra,value    clgthi Compare Logical Greater Than Halfword Immediate 171  */ 
    	break;
    case ps3spu_clgt :  /*  135 , clgt rt,ra,rb    clgt Compare Logical Greater Than Word 172  */ 
    	break;
    case ps3spu_clgti :  /*  136 , clgti rt,ra,value    clgti Compare Logical Greater Than Word Immediate 173  */ 
    	break;
    case ps3spu_br :  /*  137 , br symbol    br Branch Relative 174  */ 
    	 emu_code_xref(cmd.Op1.value, 0);
    	;
    	break;
    case ps3spu_bra :  /*  138 , bra symbol    bra Branch Absolute 175  */ 
    	 emu_code_xref(cmd.Op1.value, 1);
		 ;
    	break;
    case ps3spu_brsl :  /*  139 , brsl rt,symbol    brsl Branch Relative and Set Link in rt 176  */ 
    	 emu_code_xref(cmd.Op2.value, 0);
    	;
    	break;
    case ps3spu_brasl :  /*  140 , brasl rt,symbol    brasl Branch Absolute and Set Link in rt 177  */ 
    	emu_code_xref(cmd.Op2.value, 1);
    	;
    	break;
    case ps3spu_bi :  /*  141 , bi ra    bi Branch Indirect 178  */ 
    	break;
    case ps3spu_iret :  /*  142 , iret ra    iret Interrupt Return 179  */ 
    	break;
    case ps3spu_bisled :  /*  143 , bisled rt,ra    bisled Branch Indirect and Set Link if External Data 180  */ 
    	break;
    case ps3spu_bisl :  /*  144 , bisl rt,ra    bisl Branch Indirect and Set Link 181  */ 
    	break;
    case ps3spu_brnz :  /*  145 , brnz rt,symbol    brnz Branch If Not Zero Word 182  */ 
    	emu_code_xref(cmd.Op2.value, 0);
    	;
    	break;
    case ps3spu_brz :  /*  146 , brz rt,symbol    brz Branch If Zero Word 183  */ 
    	emu_code_xref(cmd.Op2.value, 0);
    	;
    	break;
    case ps3spu_brhnz :  /*  147 , brhnz rt,symbol    brhnz Branch If Not Zero Halfword 184  */ 
    	emu_code_xref(cmd.Op2.value, 0);
    	;
    	break;
    case ps3spu_brhz :  /*  148 , brhz rt,symbol    brhz Branch If Zero Halfword 185  */ 
    	emu_code_xref(cmd.Op2.value, 0);
    	;
    	break;
    case ps3spu_biz :  /*  149 , biz rt,ra    biz Branch Indirect If Zero 186  */ 
    	break;
    case ps3spu_binz :  /*  150 , binz rt,ra    binz Branch Indirect If Not Zero 187  */ 
    	break;
    case ps3spu_bihz :  /*  151 , bihz rt,ra    bihz Branch Indirect If Zero Halfword 188  */ 
    	break;
    case ps3spu_bihnz :  /*  152 , bihnz rt,ra    bihnz Branch Indirect If Not Zero Halfword 189  */ 
    	break;
    case ps3spu_hbr :  /*  153 , hbr brinst,brtarg    hbr Hint for Branch (r-form) 192  */ 
    	emu_code_xref(cmd.Op1.value, 0);
		break;
    case ps3spu_hbra :  /*  154 , hbra brinst,brtarg    hbra Hint for Branch (a-form) 193  */ 
    	emu_code_xref(cmd.Op1.value, 0);
		emu_code_xref(cmd.Op2.value, 1);
		
		break;
    case ps3spu_hbrr :  /*  155 , hbrr brinst,brtarg    hbrr Hint for Branch Relative 194  */ 
    	emu_code_xref(cmd.Op1.value, 0);
		emu_code_xref(cmd.Op2.value, 0);
		break;
    case ps3spu_fa :  /*  156 , fa rt,ra,rb    fa Floating Add 202  */ 
    	break;
    case ps3spu_dfa :  /*  157 , dfa rt,ra,rb    dfa Double Floating Add 203  */ 
    	break;
    case ps3spu_fs :  /*  158 , fs rt,ra,rb    fs Floating Subtract 204  */ 
    	break;
    case ps3spu_dfs :  /*  159 , dfs rt,ra,rb    dfs Double Floating Subtract 205  */ 
    	break;
    case ps3spu_fm :  /*  160 , fm rt,ra,rb    fm Floating Multiply 206  */ 
    	break;
    case ps3spu_dfm :  /*  161 , dfm rt,ra,rb    dfm Double Floating Multiply 207  */ 
    	break;
    case ps3spu_fma :  /*  162 , fma rt,ra,rb,rc    fma Floating Multiply and Add 208  */ 
    	break;
    case ps3spu_dfma :  /*  163 , dfma rt,ra,rb    dfma Double Floating Multiply and Add 209  */ 
    	break;
    case ps3spu_fnms :  /*  164 , fnms rt,ra,rb,rc    fnms Floating Negative Multiply and Subtract 210  */ 
    	break;
    case ps3spu_dfnms :  /*  165 , dfnms rt,ra,rb    dfnms Double Floating Multiply and Subtract 211  */ 
    	break;
    case ps3spu_fms :  /*  166 , fms rt,ra,rb,rc    fms Floating Multiply and Subtract 212  */ 
    	break;
    case ps3spu_dfms :  /*  167 , dfms rt,ra,rb    dfms Double Floating Multiply and Subtract 213  */ 
    	break;
    case ps3spu_dfnma :  /*  168 , dfnma rt,ra,rb    dfnma Double Floating Negative Multiply and Add 214  */ 
    	break;
    case ps3spu_frest :  /*  169 , frest rt,ra    frest Floating Reciprocal Estimate 215  */ 
    	break;
    case ps3spu_frsqest :  /*  170 , frsqest rt,ra    frsqest Floating Reciprocal Absolute Square Root Estimate 217  */ 
    	break;
    case ps3spu_fi :  /*  171 , fi rt,ra,rb    fi Floating Interpolate 219  */ 
    	break;
    case ps3spu_csflt :  /*  172 , csflt rt,ra,scale    csflt Convert Signed Integer to Floating 220  */ 
    	break;
    case ps3spu_cflts :  /*  173 , cflts rt,ra,scale    cflts Convert Floating to Signed Integer 221  */ 
    	break;
    case ps3spu_cuflt :  /*  174 , cuflt rt,ra,scale    cuflt Convert Unsigned Integer to Floating 222  */ 
    	break;
    case ps3spu_cfltu :  /*  175 , cfltu rt,ra,scale    cfltu Convert Floating to Unsigned Integer 223  */ 
    	break;
    case ps3spu_frds :  /*  176 , frds rt,ra    frds Floating Round Double to Single 224  */ 
    	break;
    case ps3spu_fesd :  /*  177 , fesd rt,ra    fesd Floating Extend Single to Double 225  */ 
    	break;
    case ps3spu_dfceq :  /*  178 , dfceq rt,ra,rb    dfceq Double Floating Compare Equal 226  */ 
    	break;
    case ps3spu_dfcmeq :  /*  179 , dfcmeq rt,ra,rb    dfcmeq Double Floating Compare Magnitude Equal 227  */ 
    	break;
    case ps3spu_dfcgt :  /*  180 , dfcgt rt,ra,rb    dfcgt Double Floating Compare Greater Than 228  */ 
    	break;
    case ps3spu_dfcmgt :  /*  181 , dfcmgt rt,ra,rb    dfcmgt Double Floating Compare Magnitude Greater Than 229  */ 
    	break;
    case ps3spu_dftsv :  /*  182 , dftsv rt,ra,value    dftsv Double Floating Test Special Value 230  */ 
    	break;
    case ps3spu_fceq :  /*  183 , fceq rt,ra,rb    fceq Floating Compare Equal 231  */ 
    	break;
    case ps3spu_fcmeq :  /*  184 , fcmeq rt,ra,rb    fcmeq Floating Compare Magnitude Equal 232  */ 
    	break;
    case ps3spu_fcgt :  /*  185 , fcgt rt,ra,rb    fcgt Floating Compare Greater Than 233  */ 
    	break;
    case ps3spu_fcmgt :  /*  186 , fcmgt rt,ra,rb    fcmgt Floating Compare Magnitude Greater Than 234  */ 
    	break;
    case ps3spu_fscrrd :  /*  187 , fscrwr ra    fscrrd Floating-Point Status and Control Register Write 235  */ 
    	break;
    case ps3spu_fscrwr :  /*  188 , fscrrd rt    fscrwr Floating-Point Status and Control Register Read 236  */ 
    	break;
    case ps3spu_stop :  /*  189 , stop    stop Stop and Signal 238  */ 
    	;
		break;
    case ps3spu_stopd :  /*  190 , stopd    stopd Stop and Signal with Dependencies 239  */ 
    	;
		break;
    case ps3spu_lnop :  /*  191 , lnop    lnop No Operation (Load) 240  */ 
    	break;
    case ps3spu_nop :  /*  192 , nop    nop No Operation (Execute) 241  */ 
    	break;
    case ps3spu_sync :  /*  193 , sync    sync Synchronize 242  */ 
    	break;
    case ps3spu_dsync :  /*  194 , dsync    dsync Synchronize Data 243  */ 
    	break;
    case ps3spu_mfspr :  /*  195 , mfspr rt,sa    mfspr Move from Special-Purpose Register 244  */ 
    	break;
    case ps3spu_mtspr :  /*  196 , mtspr sa, rt    mtspr Move to Special-Purpose Register 245  */ 
    	break;
    case ps3spu_rdch :  /*  197 , rdch rt,ca    rdch Read Channel 248  */ 
    	break;
    case ps3spu_rchcnt :  /*  198 , rchcnt rt,ca    rchcnt Read Channel Count 249  */ 
    	break;
    case ps3spu_wrch :  /*  199 , wrch ca,rt    wrch Write Channel 250  */ 
    	break;



	

	default:
		warning("%a: bad logic (emu.cpp:switch)", cmd.ea);
		return 0;
	}	



  return 1;    // actually the return value is unimportant, but let's it be so
}
