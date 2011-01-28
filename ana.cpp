/*
 *      ps3spu module for the Interactive disassembler (IDA).
 *
 *      updates, fixes and bugreports welcomed (you know where i am)
 *
 *      (w)201 by Jack Chen, aka anita999
 */

#include "ps3spu.hpp"

//----------------------------------------------------------------------


enum OpType
{
	opt_void,
	opt_rt,
	opt_ra,
	opt_rb,
	opt_rc,
	opt_u7,
	opt_s7,
	opt_u10,
	opt_s10,
	opt_u16,
	opt_s16
	
	
};

struct InsTableEntry
{
	unsigned int    mask;       // mask to be used to & with first two bytes of opcode
	unsigned int    value;      /* match when "(u16)opcode & mask == value" */
	enum nameNum    ins;        /* instruction */
  unsigned char           opbitlen;   // bit length
//  unsigned char           opcount;    // operand counts
//  optype_t op_type[4];
//  enum OpType [4]; 
//	struct OpGetDef Op1GetDef;  /* how to fetch operands */
//	struct OpGetDef Op2GetDef;
};




static struct InsTableEntry InsTable[] =
{
	// mask, value, instruction, opbitlen
//	{ 0xFFFF, 0x00, ps3spu_lqd, 0 },  //  0 , Null, dummy op
{ 0xFF00, 0x3400, ps3spu_lqd, 8 },  //  1 , lqd Load Quadword (d-form) 32
{ 0xFFE0, 0x3880, ps3spu_lqx, 11 },  //  2 , lqx Load Quadword (x-form) 33
{ 0xFF80, 0x3080, ps3spu_lqa, 9 },  //  3 , lqa Load Quadword (a-form) 34
{ 0xFF80, 0x3380, ps3spu_lqr, 9 },  //  4 , lqr Load Quadword Instruction Relative (a-form) 35
{ 0xFF00, 0x2400, ps3spu_stqd, 8 },  //  5 , stqd Store Quadword (d-form) 36
{ 0xFFE0, 0x2880, ps3spu_stqx, 11 },  //  6 , stqx Store Quadword (x-form) 37
{ 0xFF80, 0x2080, ps3spu_stqa, 9 },  //  7 , stqa Store Quadword (a-form) 38
{ 0xFF80, 0x2380, ps3spu_stqr, 9 },  //  8 , stqr Store Quadword Instruction Relative (a-form) 39
{ 0xFFE0, 0x3E80, ps3spu_cbd, 11 },  //  9 , cbd Generate Controls for Byte Insertion (d-form) 40
{ 0xFFE0, 0x3A80, ps3spu_cbx, 11 },  //  10 , cbx Generate Controls for Byte Insertion (x-form) 41
{ 0xFFE0, 0x3EA0, ps3spu_chd, 11 },  //  11 , chd Generate Controls for Halfword Insertion (d-form) 42
{ 0xFFE0, 0x3AA0, ps3spu_chx, 11 },  //  12 , chx Generate Controls for Halfword Insertion (x-form) 43
{ 0xFFE0, 0x3EC0, ps3spu_cwd, 11 },  //  13 , cwd Generate Controls for Word Insertion (d-form) 44
{ 0xFFE0, 0x3AC0, ps3spu_cwx, 11 },  //  14 , cwx Generate Controls for Word Insertion (x-form) 45
{ 0xFFE0, 0x3EE0, ps3spu_cdd, 11 },  //  15 , cdd Generate Controls for Doubleword Insertion (d-form) 46
{ 0xFFE0, 0x3AE0, ps3spu_cdx, 11 },  //  16 , cdx Generate Controls for Doubleword Insertion (x-form) 47
{ 0xFF80, 0x4180, ps3spu_ilh, 9 },  //  17 , ilh Immediate Load Halfword 50
{ 0xFF80, 0x4100, ps3spu_ilhu, 9 },  //  18 , ilhu Immediate Load Halfword Upper 51
{ 0xFF80, 0x4080, ps3spu_il, 9 },  //  19 , il Immediate Load Word 52
{ 0xFE00, 0x4200, ps3spu_ila, 7 },  //  20 , ila Immediate Load Address 53
{ 0xFF80, 0x6080, ps3spu_iohl, 9 },  //  21 , iohl Immediate Or Halfword Lower 54
{ 0xFF80, 0x3280, ps3spu_fsmbi, 9 },  //  22 , fsmbi Form Select Mask for Bytes Immediate 55
{ 0xFFE0, 0x1900, ps3spu_ah, 11 },  //  23 , ah Add Halfword 58
{ 0xFF00, 0x1D00, ps3spu_ahi, 8 },  //  24 , ahi Add Halfword Immediate 59
{ 0xFFE0, 0x1800, ps3spu_a, 11 },  //  25 , a Add Word 60
{ 0xFF00, 0x1C00, ps3spu_ai, 8 },  //  26 , ai Add Word Immediate 61
{ 0xFFE0, 0x0900, ps3spu_sfh, 11 },  //  27 , sfh Subtract from Halfword 62
{ 0xFF00, 0x0D00, ps3spu_sfhi, 8 },  //  28 , sfhi Subtract from Halfword Immediate 63
{ 0xFFE0, 0x0800, ps3spu_sf, 11 },  //  29 , sf Subtract from Word 64
{ 0xFF00, 0x0C00, ps3spu_sfi, 8 },  //  30 , sfi Subtract from Word Immediate 65
{ 0xFFE0, 0x6800, ps3spu_addx, 11 },  //  31 , addx Add Extended 66
{ 0xFFE0, 0x1840, ps3spu_cg, 11 },  //  32 , cg Carry Generate 67
{ 0xFFE0, 0x6840, ps3spu_cgx, 11 },  //  33 , cgx Carry Generate Extended 68
{ 0xFFE0, 0x6820, ps3spu_sfx, 11 },  //  34 , sfx Subtract from Extended 69
{ 0xFFE0, 0x0840, ps3spu_bg, 11 },  //  35 , bg Borrow Generate 70
{ 0xFFE0, 0x6860, ps3spu_bgx, 11 },  //  36 , bgx Borrow Generate Extended 71
{ 0xFFE0, 0x7880, ps3spu_mpy, 11 },  //  37 , mpy Multiply 72
{ 0xFFE0, 0x7980, ps3spu_mpyu, 11 },  //  38 , mpyu Multiply Unsigned 73
{ 0xFF00, 0x7400, ps3spu_mpyi, 8 },  //  39 , mpyi Multiply Immediate 74
{ 0xFF00, 0x7500, ps3spu_mpyui, 8 },  //  40 , mpyui Multiply Unsigned Immediate 75
{ 0xF000, 0xC000, ps3spu_mpya, 4 },  //  41 , mpya Multiply and Add 76
{ 0xFFE0, 0x78A0, ps3spu_mpyh, 11 },  //  42 , mpyh Multiply High 77
{ 0xFFE0, 0x78E0, ps3spu_mpys, 11 },  //  43 , mpys Multiply and Shift Right 78
{ 0xFFE0, 0x78C0, ps3spu_mpyhh, 11 },  //  44 , mpyhh Multiply High High 79
{ 0xFFE0, 0x68C0, ps3spu_mpyhha, 11 },  //  45 , mpyhha Multiply High High and Add 80
{ 0xFFE0, 0x79C0, ps3spu_mpyhhu, 11 },  //  46 , mpyhhu Multiply High High Unsigned 81
{ 0xFFE0, 0x69C0, ps3spu_mpyhhau, 11 },  //  47 , mpyhhau Multiply High High Unsigned and Add 82
{ 0xFFE0, 0x54A0, ps3spu_clz, 11 },  //  48 , clz Count Leading Zeros 83
{ 0xFFE0, 0x5680, ps3spu_cntb, 11 },  //  49 , cntb Count Ones in Bytes 84
{ 0xFFE0, 0x36C0, ps3spu_fsmb, 11 },  //  50 , fsmb Form Select Mask for Bytes 85
{ 0xFFE0, 0x36A0, ps3spu_fsmh, 11 },  //  51 , fsmh Form Select Mask for Halfwords 86
{ 0xFFE0, 0x3680, ps3spu_fsm, 11 },  //  52 , fsm Form Select Mask for Words 87
{ 0xFFE0, 0x3640, ps3spu_gbb, 11 },  //  53 , gbb Gather Bits from Bytes 88
{ 0xFFE0, 0x3620, ps3spu_gbh, 11 },  //  54 , gbh Gather Bits from Halfwords 89
{ 0xFFE0, 0x3600, ps3spu_gb, 11 },  //  55 , gb Gather Bits from Words 90
{ 0xFFE0, 0x1A60, ps3spu_avgb, 11 },  //  56 , avgb Average Bytes 91
{ 0xFFE0, 0x0A60, ps3spu_absdb, 11 },  //  57 , absdb Absolute Differences of Bytes 92
{ 0xFFE0, 0x4A60, ps3spu_sumb, 11 },  //  58 , sumb Sum Bytes into Halfwords 93
{ 0xFFE0, 0x56C0, ps3spu_xsbh, 11 },  //  59 , xsbh Extend Sign Byte to Halfword 94
{ 0xFFE0, 0x55C0, ps3spu_xshw, 11 },  //  60 , xshw Extend Sign Halfword to Word 95
{ 0xFFE0, 0x54C0, ps3spu_xswd, 11 },  //  61 , xswd Extend Sign Word to Doubleword 96
{ 0xFFE0, 0x1820, ps3spu_and, 11 },  //  62 , and And 97
{ 0xFFE0, 0x5820, ps3spu_andc, 11 },  //  63 , andc And with Complement 98
{ 0xFF00, 0x1600, ps3spu_andbi, 8 },  //  64 , andbi And Byte Immediate 99
{ 0xFF00, 0x1500, ps3spu_andhi, 8 },  //  65 , andhi And Halfword Immediate 100
{ 0xFF00, 0x1400, ps3spu_andi, 8 },  //  66 , andi And Word Immediate 101
{ 0xFFE0, 0x0820, ps3spu_or, 11 },  //  67 , or Or 102
{ 0xFFE0, 0x5920, ps3spu_orc, 11 },  //  68 , orc Or with Complement 103
{ 0xFF00, 0x0600, ps3spu_orbi, 8 },  //  69 , orbi Or Byte Immediate 104
{ 0xFF00, 0x0500, ps3spu_orhi, 8 },  //  70 , orhi Or Halfword Immediate 105
{ 0xFF00, 0x0400, ps3spu_ori, 8 },  //  71 , ori Or Word Immediate 106
{ 0xFFE0, 0x3E00, ps3spu_orx, 11 },  //  72 , orx Or Across 107
{ 0xFFE0, 0x4820, ps3spu_xor, 11 },  //  73 , xor Exclusive Or 108
{ 0xFF00, 0x4600, ps3spu_xorbi, 8 },  //  74 , xorbi Exclusive Or Byte Immediate 109
{ 0xFF00, 0x4500, ps3spu_xorhi, 8 },  //  75 , xorhi Exclusive Or Halfword Immediate 110
{ 0xFF00, 0x4400, ps3spu_xori, 8 },  //  76 , xori Exclusive Or Word Immediate 111
{ 0xFFE0, 0x1920, ps3spu_nand, 11 },  //  77 , nand Nand 112
{ 0xFFE0, 0x0920, ps3spu_nor, 11 },  //  78 , nor Nor 113
{ 0xFFE0, 0x4920, ps3spu_eqv, 11 },  //  79 , eqv Equivalent 114
{ 0xF000, 0x8000, ps3spu_selb, 4 },  //  80 , selb Select Bits 115
{ 0xF000, 0xB000, ps3spu_shufb, 4 },  //  81 , shufb Shuffle Bytes 116
{ 0xFFE0, 0x0BE0, ps3spu_shlh, 11 },  //  82 , shlh Shift Left Halfword 118
{ 0xFFE0, 0x0FE0, ps3spu_shlhi, 11 },  //  83 , shlhi Shift Left Halfword Immediate 119
{ 0xFFE0, 0x0B60, ps3spu_shl, 11 },  //  84 , shl Shift Left Word 120
{ 0xFFE0, 0x0F60, ps3spu_shli, 11 },  //  85 , shli Shift Left Word Immediate 121
{ 0xFFE0, 0x3B60, ps3spu_shlqbi, 11 },  //  86 , shlqbi Shift Left Quadword by Bits 122
{ 0xFFE0, 0x3F60, ps3spu_shlqbii, 11 },  //  87 , shlqbii Shift Left Quadword by Bits Immediate 123
{ 0xFFE0, 0x3BE0, ps3spu_shlqby, 11 },  //  88 , shlqby Shift Left Quadword by Bytes 124
{ 0xFFE0, 0x3FE0, ps3spu_shlqbyi, 11 },  //  89 , shlqbyi Shift Left Quadword by Bytes Immediate 125
{ 0xFFE0, 0x39E0, ps3spu_shlqbybi, 11 },  //  90 , shlqbybi Shift Left Quadword by Bytes from Bit Shift Count 126
{ 0xFFE0, 0x0B80, ps3spu_roth, 11 },  //  91 , roth Rotate Halfword 127
{ 0xFFE0, 0x0F80, ps3spu_rothi, 11 },  //  92 , rothi Rotate Halfword Immediate 128
{ 0xFFE0, 0x0B00, ps3spu_rot, 11 },  //  93 , rot Rotate Word 129
{ 0xFFE0, 0x0F00, ps3spu_roti, 11 },  //  94 , roti Rotate Word Immediate 130
{ 0xFFE0, 0x3B80, ps3spu_rotqby, 11 },  //  95 , rotqby Rotate Quadword by Bytes 131
{ 0xFFE0, 0x3F80, ps3spu_rotqbyi, 11 },  //  96 , rotqbyi Rotate Quadword by Bytes Immediate 132
{ 0xFFE0, 0x3980, ps3spu_rotqbybi, 11 },  //  97 , rotqbybi Rotate Quadword by Bytes from Bit Shift Count 133
{ 0xFFE0, 0x3B00, ps3spu_rotqbi, 11 },  //  98 , rotqbi Rotate Quadword by Bits 134
{ 0xFFE0, 0x3F00, ps3spu_rotqbii, 11 },  //  99 , rotqbii Rotate Quadword by Bits Immediate 135
{ 0xFFE0, 0x0BA0, ps3spu_rothm, 11 },  //  100 , rothm Rotate and Mask Halfword 136
{ 0xFFE0, 0x0FA0, ps3spu_rothmi, 11 },  //  101 , rothmi Rotate and Mask Halfword Immediate 137
{ 0xFFE0, 0x0B20, ps3spu_rotm, 11 },  //  102 , rotm Rotate and Mask Word 138
{ 0xFFE0, 0x0F20, ps3spu_rotmi, 11 },  //  103 , rotmi Rotate and Mask Word Immediate 139
{ 0xFFE0, 0x3BA0, ps3spu_rotqmby, 11 },  //  104 , rotqmby Rotate and Mask Quadword by Bytes 140
{ 0xFFE0, 0x3FA0, ps3spu_rotqmbyi, 11 },  //  105 , rotqmbyi Rotate and Mask Quadword by Bytes Immediate 141
{ 0xFFE0, 0x3FA0, ps3spu_rotqmbybi, 11 },  //  106 , rotqmbybi Rotate and Mask Quadword Bytes from Bit Shift Count 142
{ 0xFFE0, 0x3B20, ps3spu_rotqmbi, 11 },  //  107 , rotqmbi Rotate and Mask Quadword by Bits 143
{ 0xFFE0, 0x3F20, ps3spu_rotqmbii, 11 },  //  108 , rotqmbii Rotate and Mask Quadword by Bits Immediate 144
{ 0xFFE0, 0x0BC0, ps3spu_rotmah, 11 },  //  109 , rotmah Rotate and Mask Algebraic Halfword 145
{ 0xFFE0, 0x0FC0, ps3spu_rotmahi, 11 },  //  110 , rotmahi Rotate and Mask Algebraic Halfword Immediate 146
{ 0xFFE0, 0x0B40, ps3spu_rotma, 11 },  //  111 , rotma Rotate and Mask Algebraic Word 147
{ 0xFFE0, 0x0F40, ps3spu_rotmai, 11 },  //  112 , rotmai Rotate and Mask Algebraic Word Immediate 148
{ 0xFFE0, 0x7B00, ps3spu_heq, 11 },  //  113 , heq Halt If Equal 150
{ 0xFF00, 0x7F00, ps3spu_heqi, 8 },  //  114 , heqi Halt If Equal Immediate 151
{ 0xFFE0, 0x4B00, ps3spu_hgt, 11 },  //  115 , hgt Halt If Greater Than 152
{ 0xFF00, 0x4F00, ps3spu_hgti, 8 },  //  116 , hgti Halt If Greater Than Immediate 153
{ 0xFFE0, 0x5B00, ps3spu_hlgt, 11 },  //  117 , hlgt Halt If Logically Greater Than 154
{ 0xFF00, 0x5F00, ps3spu_hlgti, 8 },  //  118 , hlgti Halt If Logically Greater Than Immediate 155
{ 0xFFE0, 0x7A00, ps3spu_ceqb, 11 },  //  119 , ceqb Compare Equal Byte 156
{ 0xFF00, 0x7E00, ps3spu_ceqbi, 8 },  //  120 , ceqbi Compare Equal Byte Immediate 157
{ 0xFFE0, 0x7900, ps3spu_ceqh, 11 },  //  121 , ceqh Compare Equal Halfword 158
{ 0xFF00, 0x7D00, ps3spu_ceqhi, 8 },  //  122 , ceqhi Compare Equal Halfword Immediate 159
{ 0xFFE0, 0x7800, ps3spu_ceq, 11 },  //  123 , ceq Compare Equal Word 160
{ 0xFF00, 0x7C00, ps3spu_ceqi, 8 },  //  124 , ceqi Compare Equal Word Immediate 161
{ 0xFFE0, 0x4A00, ps3spu_cgtb, 11 },  //  125 , cgtb Compare Greater Than Byte 162
{ 0xFF00, 0x4E00, ps3spu_cgtbi, 8 },  //  126 , cgtbi Compare Greater Than Byte Immediate 163
{ 0xFFE0, 0x4900, ps3spu_cgth, 11 },  //  127 , cgth Compare Greater Than Halfword 164
{ 0xFF00, 0x4D00, ps3spu_cgthi, 8 },  //  128 , cgthi Compare Greater Than Halfword Immediate 165
{ 0xFFE0, 0x4800, ps3spu_cgt, 11 },  //  129 , cgt Compare Greater Than Word 166
{ 0xFF00, 0x4C00, ps3spu_cgti, 8 },  //  130 , cgti Compare Greater Than Word Immediate 167
{ 0xFFE0, 0x5A00, ps3spu_clgtb, 11 },  //  131 , clgtb Compare Logical Greater Than Byte 168
{ 0xFF00, 0x5E00, ps3spu_clgtbi, 8 },  //  132 , clgtbi Compare Logical Greater Than Byte Immediate 169
{ 0xFFE0, 0x5900, ps3spu_clgth, 11 },  //  133 , clgth Compare Logical Greater Than Halfword 170
{ 0xFF00, 0x5D00, ps3spu_clgthi, 8 },  //  134 , clgthi Compare Logical Greater Than Halfword Immediate 171
{ 0xFFE0, 0x5800, ps3spu_clgt, 11 },  //  135 , clgt Compare Logical Greater Than Word 172
{ 0xFF00, 0x5C00, ps3spu_clgti, 8 },  //  136 , clgti Compare Logical Greater Than Word Immediate 173
{ 0xFF80, 0x3200, ps3spu_br, 9 },  //  137 , br Branch Relative 174
{ 0xFF80, 0x3000, ps3spu_bra, 9 },  //  138 , bra Branch Absolute 175
{ 0xFF80, 0x3300, ps3spu_brsl, 9 },  //  139 , brsl Branch Relative and Set Link in rt 176
{ 0xFF80, 0x3100, ps3spu_brasl, 9 },  //  140 , brasl Branch Absolute and Set Link in rt 177
{ 0xFFE0, 0x3500, ps3spu_bi, 11 },  //  141 , bi Branch Indirect 178
{ 0xFFE0, 0x3540, ps3spu_iret, 11 },  //  142 , iret Interrupt Return 179
{ 0xFFE0, 0x3560, ps3spu_bisled, 11 },  //  143 , bisled Branch Indirect and Set Link if External Data 180
{ 0xFFE0, 0x3520, ps3spu_bisl, 11 },  //  144 , bisl Branch Indirect and Set Link 181
{ 0xFF80, 0x2100, ps3spu_brnz, 9 },  //  145 , brnz Branch If Not Zero Word 182
{ 0xFF80, 0x2000, ps3spu_brz, 9 },  //  146 , brz Branch If Zero Word 183
{ 0xFF80, 0x2300, ps3spu_brhnz, 9 },  //  147 , brhnz Branch If Not Zero Halfword 184
{ 0xFF80, 0x2200, ps3spu_brhz, 9 },  //  148 , brhz Branch If Zero Halfword 185
{ 0xFFE0, 0x2500, ps3spu_biz, 11 },  //  149 , biz Branch Indirect If Zero 186
{ 0xFFE0, 0x2520, ps3spu_binz, 11 },  //  150 , binz Branch Indirect If Not Zero 187
{ 0xFFE0, 0x2540, ps3spu_bihz, 11 },  //  151 , bihz Branch Indirect If Zero Halfword 188
{ 0xFFE0, 0x2560, ps3spu_bihnz, 11 },  //  152 , bihnz Branch Indirect If Not Zero Halfword 189
{ 0xFFE0, 0x3580, ps3spu_hbr, 11 },  //  153 , hbr Hint for Branch (r-form) 192
{ 0xFE00, 0x1000, ps3spu_hbra, 7 },  //  154 , hbra Hint for Branch (a-form) 193
{ 0xFE00, 0x1200, ps3spu_hbrr, 7 },  //  155 , hbrr Hint for Branch Relative 194
{ 0xFFE0, 0x5880, ps3spu_fa, 11 },  //  156 , fa Floating Add 202
{ 0xFFE0, 0x5980, ps3spu_dfa, 11 },  //  157 , dfa Double Floating Add 203
{ 0xFFE0, 0x58A0, ps3spu_fs, 11 },  //  158 , fs Floating Subtract 204
{ 0xFFE0, 0x59A0, ps3spu_dfs, 11 },  //  159 , dfs Double Floating Subtract 205
{ 0xFFE0, 0x58C0, ps3spu_fm, 11 },  //  160 , fm Floating Multiply 206
{ 0xFFE0, 0x59C0, ps3spu_dfm, 11 },  //  161 , dfm Double Floating Multiply 207
{ 0xF000, 0xE000, ps3spu_fma, 4 },  //  162 , fma Floating Multiply and Add 208
{ 0xFFE0, 0x6B80, ps3spu_dfma, 11 },  //  163 , dfma Double Floating Multiply and Add 209
{ 0xF000, 0xD000, ps3spu_fnms, 4 },  //  164 , fnms Floating Negative Multiply and Subtract 210
{ 0xFFE0, 0x6BC0, ps3spu_dfnms, 11 },  //  165 , dfnms Double Floating Multiply and Subtract 211
{ 0xF000, 0xF000, ps3spu_fms, 4 },  //  166 , fms Floating Multiply and Subtract 212
{ 0xFFE0, 0x6BA0, ps3spu_dfms, 11 },  //  167 , dfms Double Floating Multiply and Subtract 213
{ 0xFFE0, 0x6BE0, ps3spu_dfnma, 11 },  //  168 , dfnma Double Floating Negative Multiply and Add 214
{ 0xFFE0, 0x3700, ps3spu_frest, 11 },  //  169 , frest Floating Reciprocal Estimate 215
{ 0xFFE0, 0x3720, ps3spu_frsqest, 11 },  //  170 , frsqest Floating Reciprocal Absolute Square Root Estimate 217
{ 0xFFE0, 0x7A80, ps3spu_fi, 11 },  //  171 , fi Floating Interpolate 219
{ 0xFFC0, 0x7680, ps3spu_csflt, 10 },  //  172 , csflt Convert Signed Integer to Floating 220
{ 0xFFC0, 0x7600, ps3spu_cflts, 10 },  //  173 , cflts Convert Floating to Signed Integer 221
{ 0xFFC0, 0x76C0, ps3spu_cuflt, 10 },  //  174 , cuflt Convert Unsigned Integer to Floating 222
{ 0xFFC0, 0x7640, ps3spu_cfltu, 10 },  //  175 , cfltu Convert Floating to Unsigned Integer 223
{ 0xFFE0, 0x7720, ps3spu_frds, 11 },  //  176 , frds Floating Round Double to Single 224
{ 0xFFE0, 0x7700, ps3spu_fesd, 11 },  //  177 , fesd Floating Extend Single to Double 225
{ 0xFFE0, 0x7860, ps3spu_dfceq, 11 },  //  178 , dfceq Double Floating Compare Equal 226
{ 0xFFE0, 0x7960, ps3spu_dfcmeq, 11 },  //  179 , dfcmeq Double Floating Compare Magnitude Equal 227
{ 0xFFE0, 0x5860, ps3spu_dfcgt, 11 },  //  180 , dfcgt Double Floating Compare Greater Than 228
{ 0xFFE0, 0x5960, ps3spu_dfcmgt, 11 },  //  181 , dfcmgt Double Floating Compare Magnitude Greater Than 229
{ 0xFFE0, 0x77E0, ps3spu_dftsv, 11 },  //  182 , dftsv Double Floating Test Special Value 230
{ 0xFFE0, 0x7840, ps3spu_fceq, 11 },  //  183 , fceq Floating Compare Equal 231
{ 0xFFE0, 0x7940, ps3spu_fcmeq, 11 },  //  184 , fcmeq Floating Compare Magnitude Equal 232
{ 0xFFE0, 0x5840, ps3spu_fcgt, 11 },  //  185 , fcgt Floating Compare Greater Than 233
{ 0xFFE0, 0x5940, ps3spu_fcmgt, 11 },  //  186 , fcmgt Floating Compare Magnitude Greater Than 234
{ 0xFFE0, 0x7740, ps3spu_fscrrd, 11 },  //  187 , fscrrd Floating-Point Status and Control Register Write 235
{ 0xFFE0, 0x7300, ps3spu_fscrwr, 11 },  //  188 , fscrwr Floating-Point Status and Control Register Read 236
{ 0xFFE0, 0x0000, ps3spu_stop, 11 },  //  189 , stop Stop and Signal 238
{ 0xFFE0, 0x2800, ps3spu_stopd, 11 },  //  190 , stopd Stop and Signal with Dependencies 239
{ 0xFFE0, 0x0020, ps3spu_lnop, 11 },  //  191 , lnop No Operation (Load) 240
{ 0xFFE0, 0x4020, ps3spu_nop, 11 },  //  192 , nop No Operation (Execute) 241
{ 0xFFE0, 0x0040, ps3spu_sync, 11 },  //  193 , sync Synchronize 242
{ 0xFFE0, 0x0060, ps3spu_dsync, 11 },  //  194 , dsync Synchronize Data 243
{ 0xFFE0, 0x0180, ps3spu_mfspr, 11 },  //  195 , mfspr Move from Special-Purpose Register 244
{ 0xFFE0, 0x2180, ps3spu_mtspr, 11 },  //  196 , mtspr Move to Special-Purpose Register 245
{ 0xFFE0, 0x01A0, ps3spu_rdch, 11 },  //  197 , rdch Read Channel 248
{ 0xFFE0, 0x01E0, ps3spu_rchcnt, 11 },  //  198 , rchcnt Read Channel Count 249
{ 0xFFE0, 0x21A0, ps3spu_wrch, 11 },  //  199 , wrch Write Channel 250

	{ 0 }  // end of table
};


unsigned long select_hbr_p_field(unsigned long data)
{
	unsigned long i,j,k;
	
	i = data<<11;
	j = i>>31;			//for selecting single bit..
	k = j&0X1;			//masking by 0b1 ie in hex 0X1
	
	return k;
}

unsigned long select_ROH_hbr(unsigned long data)
{
	unsigned long i,j,k;
	
	i = data<<16;
	j = i>>30;			//for selecting two bits..
	k = j&0X3;			//masking by 0b11 ie in hex 0X3
	
	return k;
}

unsigned long select_ROH(unsigned long data)
{
	unsigned long i,j,k;
	
	i = data<<7;
	j = i>>30;			//for selecting two bits..
	k = j&0X3;			//masking by 0b11 ie in hex 0X3
	
	return k;
}	

unsigned long select_bit_13(unsigned long data)			//for interrupt enable boolean check..
{
	unsigned long i,j,k;
	
	i = data<<13;
	j = i>>31;			//only need single bit.
	k = j&0X1;
	
	return k;
}

unsigned long select_bit_12(unsigned long data)				//for interrupt disable boolean check.
{
	unsigned long i,j,k;
	
	i = data<<12;
	j = i>>31;			//only need single bit.
	k = j&0X1;
	return k;
}




unsigned long select_i18(unsigned long data,int oplen)				//for selecting immediate 18 (i18) field
{
	if(oplen!=7 )	warning("%a (%s): bad logic (ana.cpp/select_i18)", cmd.ea, Instructions[cmd.itype].name);
	unsigned long i,j,k;
	
	i = data<<7;
	j = i>>14;
	k = j&0X3FFFF;
	return k;
}

unsigned long select_i16(unsigned long data,int oplen)
{
	if(oplen!=9 )	warning("%a (%s): bad logic (ana.cpp/select_i16)", cmd.ea, Instructions[cmd.itype].name);
	unsigned long i,j,k;
	
	i = data<<9;
	j = i>>16;
	k = j&0XFFFF;
	return k;
}

unsigned long select_i10(unsigned long data,int oplen)
{
	if(oplen!=8 )	warning("%a (%s): bad logic (ana.cpp/select_i10)", cmd.ea, Instructions[cmd.itype].name);
	unsigned long i,j,k;
	
	i = data<<8;
	j = i>>22;
	k = j&0X3FF;
	return k;
}

unsigned long select_i7(unsigned long data,int oplen)
{
	if(oplen!=11 )	warning("%a (%s): bad logic (ana.cpp/select_i7)", cmd.ea, Instructions[cmd.itype].name);
	unsigned long i,j,k;
	
	i = data<<11;
	j = i>>25;
	k = j&0X7F;
	return k;
}

unsigned long select_i8(unsigned long data,int oplen)
{
	if(oplen!=10 )	warning("%a (%s): bad logic (ana.cpp/select_i8)", cmd.ea, Instructions[cmd.itype].name);
	unsigned long i,j,k;
	
	i = data<<10;
	j = i>>24;
	k = j&0XFF;
	return k;
}


//rc selection for inst.
unsigned long select_rc(unsigned long data,int oplen)
{
	if(oplen!=4 )	warning("%a (%s): bad logic (ana.cpp/select_rc)", cmd.ea, Instructions[cmd.itype].name);
	unsigned long i,j,k;
	
	i = data<<25;
	j = i>>25;
	k = j&0X7F;
	return k;

}


//rb selection for inst.
unsigned long select_rb(unsigned long data,int oplen)
{
	if(oplen!=4 && oplen!=11)	warning("%a (%s): bad logic (ana.cpp/select_rb)", cmd.ea, Instructions[cmd.itype].name);
	unsigned long i,j,k;
	
	i = data<<11;
	j = i>>25;
	k = j&0X7F;
	return k;

}



//ra selection for inst.
unsigned long select_ra(unsigned long data,int oplen)
{
	if(oplen==7||oplen==9)	warning("%a (%s): bad logic (ana.cpp/select_ra)", cmd.ea, Instructions[cmd.itype].name);
	unsigned long i,j,k;
	
	i = data<<18;
	j = i>>25;
	k = j&0X7F;
	return k;

}




//rt selection for inst., 
unsigned long select_rt(unsigned long data,int oplen)
{
	unsigned long i,j,k;
	
	if(oplen==4){//RRR form

		i = data<<4;
		j = i>>25;
		k = j&0X7F;
	
	}else{//any form other than RRR
		i = data<<25;
		j = i>>25;
		k = j&0X7F;

	}
	return k;
}
	
//rt selection for inst type rrr
unsigned long select_rt_rrr(unsigned long data,int oplen)//rt for RRR form, not necessary, integrated into select_rt()
{
	unsigned long i,j,k;
	
	i = data<<4;
	j = i>>25;
	k = j&0X7F;
	return k;
}	

union SU64 
{
	long sl;
	unsigned long ul;
}
;
unsigned long extend_sign_I7(unsigned long data)				//extend sign bit for I7,I8,I10,I16,I18
{
	unsigned long i,j,k;
	
	i = data<<25;       //sign bit
	j = i>>31;			//only need single bit.
	k = j&0X1;

	if(k==1)return data|0xFFFFFF80;   // k==1 for signed I7, then extend the sign bit.
	else    return data;
}

unsigned long extend_sign_I10(unsigned long data)				//extend sign bit for I7,I8,I10,I16,I18
{
	unsigned long i,j,k;
	
	i = data<<22;       //sign bit,shift 32-n bit
	j = i>>31;			//only need single bit.
	k = j&0X1;

	if(k==1)return data|0xFFFFFC00;   // k==1 for signed I10, then extend the sign bit.
	else    return data;
}
unsigned long extend_sign_I16(unsigned long data)				//extend sign bit for I7,I8,I10,I16,I18
{
	unsigned long i,j,k;
	
	i = data<<16;       //sign bit,shift 32-n bit
	j = i>>31;			//only need single bit.
	k = j&0X1;

	if(k==1)return data|0xFFFF0000;   // k==1 for signed I16, then extend the sign bit.
	else    return data;
}
unsigned long extend_sign_I18(unsigned long data)				//extend sign bit for I7,I8,I10,I16,I18
{
	unsigned long i,j,k;
	
	i = data<<14;       //sign bit,shift 32-n bit
	j = i>>31;			//only need single bit.
	k = j&0X1;

	if(k==1)return data|0xFFFC0000;   // k==1 for signed I18, then extend the sign bit.
	else    return data;
}
unsigned long extend_sign_ROHL(unsigned long data)				//extend sign bit for ROH<<7|ROL, 9 bits data
{
	unsigned long i,j,k;
	
	i = data<<23;       //sign bit,shift 32-n bit
	j = i>>31;			//only need single bit.
	k = j&0X1;

	if(k==1)return data|0xFFFFFE00;   // k==1 for signed I9, then extend the sign bit.
	else    return data;
}
//----------------------------------------------------------------------
// analyse an instruction
// returns size of command, or 0

//      error("%a: internal ana_extended() error, b1=%x", cmd.ea, b1);

int ana(void)
{
	cmd.itype = ps3spu_null; // default opcode
	// cmd.auxpref |= aux_1ext; ?
	// addr mode (o_imm, o_near, o_reg, o_mem, o_phrase, o_bit, o_bitnot
	// o_displ - register indirect with displacement
	cmd.Op1.type = o_void;
	cmd.Op2.type = o_void;
	cmd.Op3.type = o_void;
  cmd.Op4.type = o_void;
  
	cmd.Op1.offb = 0;
//	cmd.Op1.b251_bitneg = 0;
//	cmd.Op1.indreg = 0; // for o_phrase
	cmd.Op1.addr = 0; // o_displ
	cmd.Op1.reg = 0; // for o_reg
	cmd.Op1.value = 0; // for o_imm
	cmd.Op1.phrase = 0; // for o_phrase, o_displ

	// Default width of operand (not perfect, but better then nothing)
	// this should be modified, the GPR is 128bits, 
	cmd.Op1.dtyp = dt_dword;
	cmd.Op2.dtyp = dt_dword;
	cmd.Op3.dtyp = dt_dword;
	cmd.Op4.dtyp = dt_dword;

	cmd.size=0;  // all SPU instructions are 32 bits

  //get the 1st and 2nd bytes, reverse it to fix big endian
	unsigned int opcode = get_byte(cmd.ea)<<8  | get_byte(cmd.ea+1);
  unsigned long instr = get_byte(cmd.ea)<<24 | get_byte(cmd.ea+1)<<16 | get_byte(cmd.ea+2)<<8 | get_byte(cmd.ea+3);
	//searchInTable(opcode, InsTable);
	int optable_ind,oplen;
	for (optable_ind=0; InsTable[optable_ind].mask; optable_ind++)
	{
		if ((opcode & InsTable[optable_ind].mask) != InsTable[optable_ind].value)
			continue;
		cmd.itype = InsTable[optable_ind].ins;
		cmd.size=4;
		oplen=InsTable[optable_ind].opbitlen;
		break;
	}
//*********************************use cmd.itype to get each operand value and type. this code is generated from the cheat sheet

	switch (cmd.itype)
	{

    SU64 immval;


    case ps3spu_lqd :  /*  1 , lqd rt, s14(ra)    lqd Load Quadword (d-form) 32 , op2=I10 extends with sign, then shift left 4 bits */     	
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen))<<4;cmd.Op2.value=immval.sl;cmd.Op2.dtyp=dt_dword  ;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_ra(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_lqx :  /*  2 , lqx rt, ra, rb    lqx Load Quadword (x-form) 33  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_lqa :  /*  3 , lqa rt, s18    lqa Load Quadword (a-form) 34  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;immval.ul=(extend_sign_I16(select_i16(instr,oplen))<<2);cmd.Op2.value=immval.sl& 0xFFFFFFF0&LSLR;cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_lqr :  /*  4 , lqr rt, s18    lqr Load Quadword Instruction Relative (a-form) 35  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;immval.ul=(extend_sign_I16(select_i16(instr,oplen))<<2)& 0xFFFFFFF0;cmd.Op2.value=(immval.sl+cmd.ea)& 0xFFFFFFF0&LSLR;cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_stqd :  /*  5 , stqd rt, s14(ra)    stqd Store Quadword (d-form) 36  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen))<<4;cmd.Op2.value=immval.sl;cmd.Op2.dtyp=dt_dword  ;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_ra(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_stqx :  /*  6 , stqx rc, ra, rb    stqx Store Quadword (x-form) 37  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_stqa :  /*  7 , stqa rt, s18    stqa Store Quadword (a-form) 38  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;immval.ul=(extend_sign_I16(select_i16(instr,oplen))<<2);cmd.Op2.value=immval.sl& 0xFFFFFFF0&LSLR;cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_stqr :  /*  8 , stqr rt, s18    stqr Store Quadword Instruction Relative (a-form) 39  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;immval.ul=(extend_sign_I16(select_i16(instr,oplen))<<2)& 0xFFFFFFF0;cmd.Op2.value=(immval.sl+cmd.ea)& 0xFFFFFFF0&LSLR;cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_cbd :  /*  9 , cbd rt, u7(ra)    cbd Generate Controls for Byte Insertion (d-form) 40  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;immval.ul=extend_sign_I7(select_i7(instr,oplen));cmd.Op2.value=immval.sl;cmd.Op2.dtyp=dt_dword  ;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_ra(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_cbx :  /*  10 , cbx rt, ra, rb    cbx Generate Controls for Byte Insertion (x-form) 41  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_chd :  /*  11 , chd rt, u7(ra)    chd Generate Controls for Halfword Insertion (d-form) 42  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;immval.ul=extend_sign_I7(select_i7(instr,oplen));cmd.Op2.value=immval.sl;cmd.Op2.dtyp=dt_dword  ;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_ra(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_chx :  /*  12 , chx rt, ra, rb    chx Generate Controls for Halfword Insertion (x-form) 43  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_cwd :  /*  13 , cwd rt, u7(ra)    cwd Generate Controls for Word Insertion (d-form) 44  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;immval.ul=extend_sign_I7(select_i7(instr,oplen));cmd.Op2.value=immval.sl;cmd.Op2.dtyp=dt_dword  ;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_ra(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_cwx :  /*  14 , cwx rt, ra, rb    cwx Generate Controls for Word Insertion (x-form) 45  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_cdd :  /*  15 , cdd rt, u7(ra)    cdd Generate Controls for Doubleword Insertion (d-form) 46  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;immval.ul=extend_sign_I7(select_i7(instr,oplen));cmd.Op2.value=immval.sl;cmd.Op2.dtyp=dt_dword  ;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_ra(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_cdx :  /*  16 , cdx rt, ra, rb    cdx Generate Controls for Doubleword Insertion (x-form) 47  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_ilh :  /*  17 , ilh rt, u16    ilh Immediate Load Halfword 50  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;cmd.Op2.value=select_i16(instr,oplen);cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_ilhu :  /*  18 , ilhu rt, u16    ilhu Immediate Load Halfword Upper 51  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;cmd.Op2.value=select_i16(instr,oplen);cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_il :  /*  19 , il rt, s16    il Immediate Load Word 52  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;immval.ul=extend_sign_I16(select_i16(instr,oplen));cmd.Op2.value=immval.sl;cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_ila :  /*  20 , ila rt, u18    ila Immediate Load Address 53  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;cmd.Op2.value=select_i18(instr,oplen);cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_iohl :  /*  21 , iohl rt, u16    iohl Immediate Or Halfword Lower 54  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;cmd.Op2.value=select_i16(instr,oplen);cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_fsmbi :  /*  22 , fsmbi rt, u16    fsmbi Form Select Mask for Bytes Immediate 55  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;cmd.Op2.value=select_i16(instr,oplen);cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_ah :  /*  23 , ah rt, ra, rb    ah Add Halfword 58  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_ahi :  /*  24 , ahi rt, ra, s10    ahi Add Halfword Immediate 59  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op3.value=immval.sl;cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_a :  /*  25 , a rt, ra, rb    a Add Word 60  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_ai :  /*  26 , ai rt, ra, s10    ai Add Word Immediate 61  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op3.value=immval.sl;cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_sfh :  /*  27 , sfh rt, ra, rb    sfh Subtract from Halfword 62  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_sfhi :  /*  28 , sfhi rt, ra, s10    sfhi Subtract from Halfword Immediate 63  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op3.value=immval.sl;cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_sf :  /*  29 , sf rt, ra, rb    sf Subtract from Word 64  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_sfi :  /*  30 , sfi rt, ra, s10    sfi Subtract from Word Immediate 65  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op3.value=immval.sl;cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_addx :  /*  31 , addx rt, ra, rb    addx Add Extended 66  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_cg :  /*  32 , cg rt, ra, rb    cg Carry Generate 67  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_cgx :  /*  33 , cgx rt, ra, rb    cgx Carry Generate Extended 68  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_sfx :  /*  34 , sfx rt, ra, rb    sfx Subtract from Extended 69  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_bg :  /*  35 , bg rt, ra, rb    bg Borrow Generate 70  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_bgx :  /*  36 , bgx rt, ra, rb    bgx Borrow Generate Extended 71  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_mpy :  /*  37 , mpy rt, ra, rb    mpy Multiply 72  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_mpyu :  /*  38 , mpyu rt, ra, rb    mpyu Multiply Unsigned 73  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_mpyi :  /*  39 , mpyi rt, ra, s10    mpyi Multiply Immediate 74  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op3.value=immval.sl;cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_mpyui :  /*  40 , mpyui rt, ra, s10    mpyui Multiply Unsigned Immediate 75  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op3.value=immval.sl;cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_mpya :  /*  41 , mpya rt, ra, rb, rc    mpya Multiply and Add 76  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	cmd.Op4.type=o_reg;cmd.Op4.value=select_rc(instr,oplen);cmd.Op4.dtyp=dt_byte;;
    	break;
    case ps3spu_mpyh :  /*  42 , mpyh rt, ra, rb    mpyh Multiply High 77  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_mpys :  /*  43 , mpys rt, ra, rb    mpys Multiply and Shift Right 78  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_mpyhh :  /*  44 , mpyhh rt, ra, rb    mpyhh Multiply High High 79  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_mpyhha :  /*  45 , mpyhha rt, ra, rb    mpyhha Multiply High High and Add 80  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_mpyhhu :  /*  46 , mpyhhu rt, ra, rb    mpyhhu Multiply High High Unsigned 81  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_mpyhhau :  /*  47 , mpyhhau rt, ra, rb    mpyhhau Multiply High High Unsigned and Add 82  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_clz :  /*  48 , clz rt, ra    clz Count Leading Zeros 83  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_cntb :  /*  49 , cntb rt, ra    cntb Count Ones in Bytes 84  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_fsmb :  /*  50 , fsmb rt, ra    fsmb Form Select Mask for Bytes 85  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_fsmh :  /*  51 , fsmh rt, ra    fsmh Form Select Mask for Halfwords 86  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_fsm :  /*  52 , fsm rt, ra    fsm Form Select Mask for Words 87  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_gbb :  /*  53 , gbb rt, ra    gbb Gather Bits from Bytes 88  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_gbh :  /*  54 , gbh rt, ra    gbh Gather Bits from Halfwords 89  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_gb :  /*  55 , gb rt, ra    gb Gather Bits from Words 90  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_avgb :  /*  56 , avgb rt, ra, rb    avgb Average Bytes 91  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_absdb :  /*  57 , absdb rt, ra, rb    absdb Absolute Differences of Bytes 92  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_sumb :  /*  58 , sumb rt, ra, rb    sumb Sum Bytes into Halfwords 93  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_xsbh :  /*  59 , xsbh rt, ra    xsbh Extend Sign Byte to Halfword 94  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_xshw :  /*  60 , xshw rt, ra    xshw Extend Sign Halfword to Word 95  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_xswd :  /*  61 , xswd rt, ra    xswd Extend Sign Word to Doubleword 96  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_and :  /*  62 , and rt, ra, rb    and And 97  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_andc :  /*  63 , andc rt, ra, rb    andc And with Complement 98  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_andbi :  /*  64 , andbi rt, ra, s10    andbi And Byte Immediate 99  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=select_i10(instr,oplen);cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_andhi :  /*  65 , andhi rt, ra, s10    andhi And Halfword Immediate 100  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op3.value=immval.sl;cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_andi :  /*  66 , andi rt, ra, s10    andi And Word Immediate 101  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op3.value=immval.sl;cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_or :  /*  67 , or rt, ra, rb    or Or 102  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_orc :  /*  68 , orc rt, ra, rb    orc Or with Complement 103  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_orbi :  /*  69 , orbi rt, ra, s10    orbi Or Byte Immediate 104  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=select_i10(instr,oplen);cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_orhi :  /*  70 , orhi rt, ra, s10    orhi Or Halfword Immediate 105  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op3.value=immval.sl;cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_ori :  /*  71 , ori rt, ra, s10    ori Or Word Immediate 106  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op3.value=immval.sl;cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_orx :  /*  72 , orx rt, ra    orx Or Across 107  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_xor :  /*  73 , xor rt, ra, rb    xor Exclusive Or 108  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_xorbi :  /*  74 , xorbi rt, ra, s10    xorbi Exclusive Or Byte Immediate 109  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=select_i10(instr,oplen);cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_xorhi :  /*  75 , xorhi rt, ra, s10    xorhi Exclusive Or Halfword Immediate 110  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op3.value=immval.sl;cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_xori :  /*  76 , xori rt, ra, s10    xori Exclusive Or Word Immediate 111  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op3.value=immval.sl;cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_nand :  /*  77 , nand rt, ra, rb    nand Nand 112  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_nor :  /*  78 , nor rt, ra, rb    nor Nor 113  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_eqv :  /*  79 , eqv rt, ra, rb    eqv Equivalent 114  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_selb :  /*  80 , selb rt, ra, rb, rc    selb Select Bits 115  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	cmd.Op4.type=o_reg;cmd.Op4.value=select_rc(instr,oplen);cmd.Op4.dtyp=dt_byte;;
    	break;
    case ps3spu_shufb :  /*  81 , shufb rt, ra, rb, rc    shufb Shuffle Bytes 116  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	cmd.Op4.type=o_reg;cmd.Op4.value=select_rc(instr,oplen);cmd.Op4.dtyp=dt_byte;;
    	break;
    case ps3spu_shlh :  /*  82 , shlh rt, ra, rb    shlh Shift Left Halfword 118  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_shlhi :  /*  83 , shlhi rt, ra, u5    shlhi Shift Left Halfword Immediate 119  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=(0x1F&select_i7(instr,oplen));cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_shl :  /*  84 , shl rt, ra, rb    shl Shift Left Word 120  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_shli :  /*  85 , shli rt, ra, u6    shli Shift Left Word Immediate 121  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=(0x3F&select_i7(instr,oplen));cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_shlqbi :  /*  86 , shlqbi rt, ra, rb    shlqbi Shift Left Quadword by Bits 122  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_shlqbii :  /*  87 , shlqbii rt, ra, u3    shlqbii Shift Left Quadword by Bits Immediate 123  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=(0x07&select_i7(instr,oplen));cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_shlqby :  /*  88 , shlqby rt, ra, rb    shlqby Shift Left Quadword by Bytes 124  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_shlqbyi :  /*  89 , shlqbyi rt, ra, u5    shlqbyi Shift Left Quadword by Bytes Immediate 125  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=(0x1F&select_i7(instr,oplen));cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_shlqbybi :  /*  90 , shlqbybi rt, ra, rb    shlqbybi Shift Left Quadword by Bytes from Bit Shift Count 126  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_roth :  /*  91 , roth rt, ra, rb    roth Rotate Halfword 127  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_rothi :  /*  92 , rothi rt, ra, u4    rothi Rotate Halfword Immediate 128  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=(0x0F&select_i7(instr,oplen));cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_rot :  /*  93 , rot rt, ra, rb    rot Rotate Word 129  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_roti :  /*  94 , roti rt, ra, u5    roti Rotate Word Immediate 130  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=(0x1F&select_i7(instr,oplen));cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_rotqby :  /*  95 , rotqby rt, ra, rb    rotqby Rotate Quadword by Bytes 131  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_rotqbyi :  /*  96 , rotqbyi rt, ra, u4    rotqbyi Rotate Quadword by Bytes Immediate 132  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=(0x0F&select_i7(instr,oplen));cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_rotqbybi :  /*  97 , rotqbybi rt, ra, rb    rotqbybi Rotate Quadword by Bytes from Bit Shift Count 133  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_rotqbi :  /*  98 , rotqbi rt, ra, rb    rotqbi Rotate Quadword by Bits 134  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_rotqbii :  /*  99 , rotqbii rt, ra, u3    rotqbii Rotate Quadword by Bits Immediate 135  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=(0x07&select_i7(instr,oplen));cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_rothm :  /*  100 , rothm rt, ra, rb    rothm Rotate and Mask Halfword 136  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_rothmi :  /*  101 , rothmi rt, ra, u5    rothmi Rotate and Mask Halfword Immediate 137  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=(0x1F&select_i7(instr,oplen));cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_rotm :  /*  102 , rotm rt, ra, rb    rotm Rotate and Mask Word 138  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_rotmi :  /*  103 , rotmi rt, ra, u6    rotmi Rotate and Mask Word Immediate 139  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=(0x3F&select_i7(instr,oplen));cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_rotqmby :  /*  104 , rotqmby rt, ra, rb    rotqmby Rotate and Mask Quadword by Bytes 140  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_rotqmbyi :  /*  105 , rotqmbyi rt,ra,value   u5    rotqmbyi Rotate and Mask Quadword by Bytes Immediate 141  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=(0x1F&select_i7(instr,oplen));cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_rotqmbybi :  /*  106 , rotqmbybi rt, ra, rb    rotqmbybi Rotate and Mask Quadword Bytes from Bit Shift Count 142  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_rotqmbi :  /*  107 , rotqmbi rt, ra, rb    rotqmbi Rotate and Mask Quadword by Bits 143  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_rotqmbii :  /*  108 , rotqmbii rt, ra, u3    rotqmbii Rotate and Mask Quadword by Bits Immediate 144  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=(0x07&select_i7(instr,oplen));cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_rotmah :  /*  109 , rotmah rt, ra, rb    rotmah Rotate and Mask Algebraic Halfword 145  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_rotmahi :  /*  110 , rotmahi rt, ra, u6    rotmahi Rotate and Mask Algebraic Halfword Immediate 146  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=(0x1F&select_i7(instr,oplen));cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_rotma :  /*  111 , rotma rt, ra, rb    rotma Rotate and Mask Algebraic Word 147  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_rotmai :  /*  112 , rotmai rt, ra, u6    rotmai Rotate and Mask Algebraic Word Immediate 148  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=(0x3F&select_i7(instr,oplen));cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_heq :  /*  113 , heq ra,rb    heq Halt If Equal 150  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_ra(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_rb(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_heqi :  /*  114 , heqi ra,symbol    heqi Halt If Equal Immediate 151  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_ra(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op2.value=immval.sl;cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_hgt :  /*  115 , hgt ra,rb    hgt Halt If Greater Than 152  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_ra(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_rb(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_hgti :  /*  116 , hgti ra,symbol    hgti Halt If Greater Than Immediate 153  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_ra(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op2.value=immval.sl;cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_hlgt :  /*  117 , hlgt ra,rb    hlgt Halt If Logically Greater Than 154  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_ra(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_rb(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_hlgti :  /*  118 , hlgti ra,symbol    hlgti Halt If Logically Greater Than Immediate 155  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_ra(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op2.value=immval.ul;cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_ceqb :  /*  119 , ceqb rt,ra,rb    ceqb Compare Equal Byte 156  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_ceqbi :  /*  120 , ceqbi rt,ra,value    ceqbi Compare Equal Byte Immediate 157  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=select_i10(instr,oplen);cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_ceqh :  /*  121 , ceqh rt,ra,rb    ceqh Compare Equal Halfword 158  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_ceqhi :  /*  122 , ceqhi rt,ra,value    ceqhi Compare Equal Halfword Immediate 159  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op3.value=immval.ul;cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_ceq :  /*  123 , ceq rt,ra,rb    ceq Compare Equal Word 160  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_ceqi :  /*  124 , ceqi rt,ra,value    ceqi Compare Equal Word Immediate 161  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op3.value=immval.ul;cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_cgtb :  /*  125 , cgtb rt,ra,rb    cgtb Compare Greater Than Byte 162  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_cgtbi :  /*  126 , cgtbi rt,ra,value    cgtbi Compare Greater Than Byte Immediate 163  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=select_i10(instr,oplen);cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_cgth :  /*  127 , cgth rt,ra,rb    cgth Compare Greater Than Halfword 164  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_cgthi :  /*  128 , cgthi rt,ra,value    cgthi Compare Greater Than Halfword Immediate 165  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op3.value=immval.sl;cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_cgt :  /*  129 , cgt rt,ra,rb    cgt Compare Greater Than Word 166  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_cgti :  /*  130 , cgti rt,ra,value    cgti Compare Greater Than Word Immediate 167  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op3.value=immval.sl;cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_clgtb :  /*  131 , clgtb rt,ra,rb    clgtb Compare Logical Greater Than Byte 168  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_clgtbi :  /*  132 , clgtbi rt,ra,value    clgtbi Compare Logical Greater Than Byte Immediate 169  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op3.value=immval.ul;cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_clgth :  /*  133 , clgth rt,ra,rb    clgth Compare Logical Greater Than Halfword 170  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_clgthi :  /*  134 , clgthi rt,ra,value    clgthi Compare Logical Greater Than Halfword Immediate 171  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op3.value=immval.ul;cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_clgt :  /*  135 , clgt rt,ra,rb    clgt Compare Logical Greater Than Word 172  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_clgti :  /*  136 , clgti rt,ra,value    clgti Compare Logical Greater Than Word Immediate 173  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;immval.ul=extend_sign_I10(select_i10(instr,oplen));cmd.Op3.value=immval.ul;cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_br :  /*  137 , br symbol    br Branch Relative 174  */ 
    	cmd.Op1.type=o_imm;immval.ul=extend_sign_I16(select_i16(instr,oplen))<<2;cmd.Op1.value=(cmd.ea+immval.sl)& LSLR & 0xFFFFFFFC;cmd.Op1.dtyp=dt_dword  ;;
    	;
    	;
    	;
    	break;
    case ps3spu_bra :  /*  138 , bra symbol    bra Branch Absolute 175  */ 
    	cmd.Op1.type=o_imm;immval.ul=extend_sign_I16(select_i16(instr,oplen))<<2;cmd.Op1.value=immval.ul& LSLR & 0xFFFFFFFC;cmd.Op1.dtyp=dt_dword  ;;
    	;
    	;
    	;
    	break;
    case ps3spu_brsl :  /*  139 , brsl rt,symbol    brsl Branch Relative and Set Link in rt 176  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;immval.ul=extend_sign_I16(select_i16(instr,oplen))<<2;cmd.Op2.value=(cmd.ea+immval.sl)& LSLR & 0xFFFFFFFC;cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_brasl :  /*  140 , brasl rt,symbol    brasl Branch Absolute and Set Link in rt 177  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;immval.ul=extend_sign_I16(select_i16(instr,oplen))<<2;cmd.Op2.value=immval.ul& LSLR & 0xFFFFFFFC;cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_bi :  /*  141 , bi ra    bi Branch Indirect 178  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_ra(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	;
    	;
    	;
    	break;
    case ps3spu_iret :  /*  142 , iret ra    iret Interrupt Return 179  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_ra(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	;
    	;
    	;
    	break;
    case ps3spu_bisled :  /*  143 , bisled rt,ra    bisled Branch Indirect and Set Link if External Data 180  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_bisl :  /*  144 , bisl rt,ra    bisl Branch Indirect and Set Link 181  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_brnz :  /*  145 , brnz rt,symbol    brnz Branch If Not Zero Word 182  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;immval.ul=extend_sign_I16(select_i16(instr,oplen))<<2;cmd.Op2.value=(cmd.ea+immval.sl)& LSLR & 0xFFFFFFFC;cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_brz :  /*  146 , brz rt,symbol    brz Branch If Zero Word 183  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;immval.ul=extend_sign_I16(select_i16(instr,oplen))<<2;cmd.Op2.value=(cmd.ea+immval.sl)& LSLR & 0xFFFFFFFC;cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_brhnz :  /*  147 , brhnz rt,symbol    brhnz Branch If Not Zero Halfword 184  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;immval.ul=extend_sign_I16(select_i16(instr,oplen))<<2;cmd.Op2.value=(cmd.ea+immval.sl)& LSLR & 0xFFFFFFFC;cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_brhz :  /*  148 , brhz rt,symbol    brhz Branch If Zero Halfword 185  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;immval.ul=extend_sign_I16(select_i16(instr,oplen))<<2;cmd.Op2.value=(cmd.ea+immval.sl)& LSLR & 0xFFFFFFFC;cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_biz :  /*  149 , biz rt,ra    biz Branch Indirect If Zero 186  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_binz :  /*  150 , binz rt,ra    binz Branch Indirect If Not Zero 187  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_bihz :  /*  151 , bihz rt,ra    bihz Branch Indirect If Zero Halfword 188  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_bihnz :  /*  152 , bihnz rt,ra    bihnz Branch Indirect If Not Zero Halfword 189  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_hbr :  /*  153 , hbr brinst,brtarg    hbr Hint for Branch (r-form) 192  */ 
    	cmd.Op1.type=o_imm;immval.ul=(extend_sign_ROHL(select_ROH_hbr(instr)<<7|select_rt(instr,oplen))<<2)& LSLR ;
		cmd.Op1.value=cmd.ea+immval.sl;cmd.Op1.dtyp=dt_dword;;//address of branch instruction
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;//ra = target address
    	cmd.Op3.type=o_imm;cmd.Op3.value=select_hbr_p_field(instr);cmd.Op3.dtyp=dt_dword  ;;//p filed, 1 for prefetch no branch, 0 for branch,
    	;
    	break;
    case ps3spu_hbra :  /*  154 , hbra brinst,brtarg    hbra Hint for Branch (a-form) 193  */ 
    	cmd.Op1.type=o_imm;immval.ul=(extend_sign_ROHL(select_ROH(instr)<<7|select_rt(instr,oplen))<<2)& LSLR & 0xFFFFFFFC;
		cmd.Op1.value=cmd.ea+immval.sl;cmd.Op1.dtyp=dt_dword;;//address of branch instruction
    	cmd.Op2.type=o_imm;immval.ul=extend_sign_I16(select_i18(instr,oplen))<<2;cmd.Op2.value=immval.ul& LSLR & 0xFFFFFFFC;cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_hbrr :  /*  155 , hbrr brinst,brtarg    hbrr Hint for Branch Relative 194  */ 
    	cmd.Op1.type=o_imm;immval.ul=(extend_sign_ROHL(select_ROH(instr)<<7|select_rt(instr,oplen))<<2)& LSLR & 0xFFFFFFFC;
		cmd.Op1.value=cmd.ea+immval.sl;cmd.Op1.dtyp=dt_dword;;//address of branch instruction
    	cmd.Op2.type=o_imm;immval.ul=extend_sign_I16(select_i18(instr,oplen))<<2;cmd.Op2.value=(cmd.ea+immval.sl)& LSLR & 0xFFFFFFFC;cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_fa :  /*  156 , fa rt,ra,rb    fa Floating Add 202  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_dfa :  /*  157 , dfa rt,ra,rb    dfa Double Floating Add 203  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_fs :  /*  158 , fs rt,ra,rb    fs Floating Subtract 204  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_dfs :  /*  159 , dfs rt,ra,rb    dfs Double Floating Subtract 205  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_fm :  /*  160 , fm rt,ra,rb    fm Floating Multiply 206  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_dfm :  /*  161 , dfm rt,ra,rb    dfm Double Floating Multiply 207  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_fma :  /*  162 , fma rt,ra,rb,rc    fma Floating Multiply and Add 208  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	cmd.Op4.type=o_reg;cmd.Op4.value=select_rc(instr,oplen);cmd.Op4.dtyp=dt_byte;;
    	break;
    case ps3spu_dfma :  /*  163 , dfma rt,ra,rb    dfma Double Floating Multiply and Add 209  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_fnms :  /*  164 , fnms rt,ra,rb,rc    fnms Floating Negative Multiply and Subtract 210  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	cmd.Op4.type=o_reg;cmd.Op4.value=select_rc(instr,oplen);cmd.Op4.dtyp=dt_byte;;
    	break;
    case ps3spu_dfnms :  /*  165 , dfnms rt,ra,rb    dfnms Double Floating Multiply and Subtract 211  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_fms :  /*  166 , fms rt,ra,rb,rc    fms Floating Multiply and Subtract 212  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	cmd.Op4.type=o_reg;cmd.Op4.value=select_rc(instr,oplen);cmd.Op4.dtyp=dt_byte;;
    	break;
    case ps3spu_dfms :  /*  167 , dfms rt,ra,rb    dfms Double Floating Multiply and Subtract 213  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_dfnma :  /*  168 , dfnma rt,ra,rb    dfnma Double Floating Negative Multiply and Add 214  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_frest :  /*  169 , frest rt,ra    frest Floating Reciprocal Estimate 215  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_frsqest :  /*  170 , frsqest rt,ra    frsqest Floating Reciprocal Absolute Square Root Estimate 217  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_fi :  /*  171 , fi rt,ra,rb    fi Floating Interpolate 219  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_csflt :  /*  172 , csflt rt,ra,scale=u8  csflt Convert Signed Integer to Floating 220  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=select_i8(instr,oplen);cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_cflts :  /*  173 , cflts rt,ra,scale=u8    cflts Convert Floating to Signed Integer 221  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=select_i8(instr,oplen);cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_cuflt :  /*  174 , cuflt rt,ra,scale=u8    cuflt Convert Unsigned Integer to Floating 222  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=select_i8(instr,oplen);cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_cfltu :  /*  175 , cfltu rt,ra,scale=u8    cfltu Convert Floating to Unsigned Integer 223  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=select_i8(instr,oplen);cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_frds :  /*  176 , frds rt,ra    frds Floating Round Double to Single 224  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_fesd :  /*  177 , fesd rt,ra    fesd Floating Extend Single to Double 225  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_dfceq :  /*  178 , dfceq rt,ra,rb    dfceq Double Floating Compare Equal 226  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_dfcmeq :  /*  179 , dfcmeq rt,ra,rb    dfcmeq Double Floating Compare Magnitude Equal 227  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_dfcgt :  /*  180 , dfcgt rt,ra,rb    dfcgt Double Floating Compare Greater Than 228  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_dfcmgt :  /*  181 , dfcmgt rt,ra,rb    dfcmgt Double Floating Compare Magnitude Greater Than 229  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_dftsv :  /*  182 , dftsv rt,ra,value=u7    dftsv Double Floating Test Special Value 230  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_imm;cmd.Op3.value=select_i7(instr,oplen);cmd.Op3.dtyp=dt_dword  ;;
    	;
    	break;
    case ps3spu_fceq :  /*  183 , fceq rt,ra,rb    fceq Floating Compare Equal 231  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_fcmeq :  /*  184 , fcmeq rt,ra,rb    fcmeq Floating Compare Magnitude Equal 232  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_fcgt :  /*  185 , fcgt rt,ra,rb    fcgt Floating Compare Greater Than 233  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_fcmgt :  /*  186 , fcmgt rt,ra,rb    fcmgt Floating Compare Magnitude Greater Than 234  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	cmd.Op3.type=o_reg;cmd.Op3.value=select_rb(instr,oplen);cmd.Op3.dtyp=dt_byte;;
    	;
    	break;
    case ps3spu_fscrrd :  /*  187 , fscrwr ra    fscrrd Floating-Point Status and Control Register Write 235  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_ra(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	;
    	;
    	;
    	break;
    case ps3spu_fscrwr :  /*  188 , fscrrd rt    fscrwr Floating-Point Status and Control Register Read 236  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
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
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_mtspr :  /*  196 , mtspr sa, rt    mtspr Move to Special-Purpose Register 245  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_ra(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_rt(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;
    case ps3spu_rdch :  /*  197 , rdch rt,ca    rdch Read Channel 248  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_rchcnt :  /*  198 , rchcnt rt,ca    rchcnt Read Channel Count 249  */ 
    	cmd.Op1.type=o_reg;cmd.Op1.value=select_rt(instr,oplen);cmd.Op1.dtyp=dt_byte;;
    	cmd.Op2.type=o_imm;cmd.Op2.value=select_ra(instr,oplen);cmd.Op2.dtyp=dt_dword  ;;
    	;
    	;
    	break;
    case ps3spu_wrch :  /*  199 , wrch ca,rt    wrch Write Channel 250  */ 
    	cmd.Op1.type=o_imm;cmd.Op1.value=select_ra(instr,oplen);cmd.Op1.dtyp=dt_dword  ;;
    	cmd.Op2.type=o_reg;cmd.Op2.value=select_rt(instr,oplen);cmd.Op2.dtyp=dt_byte;;
    	;
    	;
    	break;



	

	default:
		warning("%a: bad logic (ana.cpp:getImmValue)", cmd.ea);
		return 0;
	}	
	return cmd.size;
}

/*
static uchar getRegIndex(uchar opcode, uchar how)
{
	// if flag set, high bits = opcode shift count, low 4 bits = first reg no
	if (how & FETCHREG_USESHIFT)
	{
		how &= ~FETCHREG_USESHIFT;
		return ((opcode >> (how >> 4)) & 3) + (how & 0x0F);
	}
	else
		return how;  // regindex as is 
		
}

static long getImmValue(unsigned how)
{
	unsigned offs;
	ea_t     ea;

	offs = (how & _FIMM_OFFS_MASK) >> _FIMM_OFFS_SHIFT;
	if (offs == 0)
		return 0;

	ea = cmd.ea + offs;
	switch (how & _FIMM_TYPE_MASK)
	{
	case dt_byte:                return get_byte(ea);
	case dt_byte | _FIMM_SIGN:   return (signed char)get_byte(ea);
	case dt_word:                return get_word(ea);
	case dt_word | _FIMM_SIGN:   return (signed short)get_word(ea);
	case dt_dword:               return get_long(ea);
	case dt_dword | _FIMM_SIGN:  return (signed long)get_long(ea);
	default:
		warning("%a: bad logic (ana.cpp:getImmValue)", cmd.ea);
		return 0;
	}
}

static void getOpsFromTable(uchar opcode, struct OpGetDef const *def, op_t *op)
{
	op->type = def->op_type;
	switch (op->type)
	{
	case o_void:
		break; //  no operand, ok 

	case o_displ:
		op->addr = getImmValue(def->how2);
		goto GET_BASE_REG;
	case o_imm:
	case o_regs:
		op->value = getImmValue(def->how2);
		break;
	case o_mem:
		op->addr = getImmValue(def->how2);
		switch (cmd.itype)
		{
		case ps3spu_bset:
		case ps3spu_bclr:
		case ps3spu_btst:
		case ps3spu_movbu:
			op->dtyp = dt_byte; break;
		case ps3spu_movhu:
			op->dtyp = dt_word; break;
		case ps3spu_mov:
			op->dtyp = dt_dword; break;
		default:
			warning("%a: bad logic (ana.cpp:getMemMovWidth)", cmd.ea); break;
		}
		break;
	case o_near:
		op->addr = cmd.ea + getImmValue(def->how2);
		break;
	case o_reg:
GET_BASE_REG:
		op->reg = getRegIndex(opcode, def->how1);
		break;
	case o_phrase:
		op->indreg = getRegIndex(opcode, def->how2);
		goto GET_BASE_REG;
	default:
		warning("%a: bad logic (ana.cpp:getOpsFromTable)", cmd.ea);
		break;
	}
}
*/

int is_align_insn(ea_t ea)
{
  decode_insn(ea);
  switch ( cmd.itype ) {
    default:
      return 0;
	case ps3spu_nop:
	case ps3spu_lnop:
      break;
  }
  return cmd.size;
}