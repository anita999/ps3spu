/*
 *      ps3spu module for the Interactive disassembler (IDA).
 *
 *      updates, fixes and bugreports welcomed (you know where i am)
 *
 *      (w)2011 by Jack Chen, aka anita999
 */

#include "ps3spu.hpp"

instruc_t Instructions[] = {
	
{ " ",  0  },  // 0, unknown opcode, dummy

{ "lqd",  CF_CHG1| CF_USE2|CF_USE3  },  // 1 , lqd Load Quadword (d-form) 32
{ "lqx",  CF_CHG1|CF_USE2|CF_USE3  },  // 2 , lqx Load Quadword (x-form) 33
{ "lqa",  CF_CHG1|CF_USE2  },  // 3 , lqa Load Quadword (a-form) 34
{ "lqr",  CF_CHG1|CF_USE2  },  // 4 , lqr Load Quadword Instruction Relative (a-form) 35
{ "stqd",  CF_USE1|CF_USE2|CF_USE3  },  // 5 , stqd Store Quadword (d-form) 36
{ "stqx",  CF_USE1|CF_USE2|CF_USE3  },  // 6 , stqx Store Quadword (x-form) 37
{ "stqa",  CF_USE1|CF_USE2  },  // 7 , stqa Store Quadword (a-form) 38
{ "stqr",  CF_USE1|CF_USE2  },  // 8 , stqr Store Quadword Instruction Relative (a-form) 39
{ "cbd",  CF_CHG1|CF_USE2|CF_USE3  },  // 9 , cbd Generate Controls for Byte Insertion (d-form) 40
{ "cbx",  CF_CHG1|CF_USE2|CF_USE3  },  // 10 , cbx Generate Controls for Byte Insertion (x-form) 41
{ "chd",  CF_CHG1|CF_USE2|CF_USE3  },  // 11 , chd Generate Controls for Halfword Insertion (d-form) 42
{ "chx",  CF_CHG1|CF_USE2|CF_USE3  },  // 12 , chx Generate Controls for Halfword Insertion (x-form) 43
{ "cwd",  CF_CHG1|CF_USE2|CF_USE3  },  // 13 , cwd Generate Controls for Word Insertion (d-form) 44
{ "cwx",  CF_CHG1|CF_USE2|CF_USE3  },  // 14 , cwx Generate Controls for Word Insertion (x-form) 45
{ "cdd",  CF_CHG1|CF_USE2|CF_USE3  },  // 15 , cdd Generate Controls for Doubleword Insertion (d-form) 46
{ "cdx",  CF_CHG1|CF_USE2|CF_USE3  },  // 16 , cdx Generate Controls for Doubleword Insertion (x-form) 47
{ "ilh",  CF_CHG1|CF_USE2  },  // 17 , ilh Immediate Load Halfword 50
{ "ilhu",  CF_CHG1|CF_USE2  },  // 18 , ilhu Immediate Load Halfword Upper 51
{ "il",  CF_CHG1|CF_USE2  },  // 19 , il Immediate Load Word 52
{ "ila",  CF_CHG1|CF_USE2  },  // 20 , ila Immediate Load Address 53
{ "iohl",  CF_CHG1|CF_USE2  },  // 21 , iohl Immediate Or Halfword Lower 54
{ "fsmbi",  CF_CHG1|CF_USE2  },  // 22 , fsmbi Form Select Mask for Bytes Immediate 55
{ "ah",  CF_CHG1|CF_USE2|CF_USE3  },  // 23 , ah Add Halfword 58
{ "ahi",  CF_CHG1|CF_USE2|CF_USE3  },  // 24 , ahi Add Halfword Immediate 59
{ "a",  CF_CHG1|CF_USE2|CF_USE3  },  // 25 , a Add Word 60
{ "ai",  CF_CHG1|CF_USE2|CF_USE3  },  // 26 , ai Add Word Immediate 61
{ "sfh",  CF_CHG1|CF_USE2|CF_USE3  },  // 27 , sfh Subtract from Halfword 62
{ "sfhi",  CF_CHG1|CF_USE2|CF_USE3  },  // 28 , sfhi Subtract from Halfword Immediate 63
{ "sf",  CF_CHG1|CF_USE2|CF_USE3  },  // 29 , sf Subtract from Word 64
{ "sfi",  CF_CHG1|CF_USE2|CF_USE3  },  // 30 , sfi Subtract from Word Immediate 65
{ "addx",  CF_CHG1|CF_USE2|CF_USE3  },  // 31 , addx Add Extended 66
{ "cg",  CF_CHG1|CF_USE2|CF_USE3  },  // 32 , cg Carry Generate 67
{ "cgx",  CF_CHG1|CF_USE2|CF_USE3  },  // 33 , cgx Carry Generate Extended 68
{ "sfx",  CF_CHG1|CF_USE2|CF_USE3  },  // 34 , sfx Subtract from Extended 69
{ "bg",  CF_CHG1|CF_USE2|CF_USE3  },  // 35 , bg Borrow Generate 70
{ "bgx",  CF_CHG1|CF_USE2|CF_USE3  },  // 36 , bgx Borrow Generate Extended 71
{ "mpy",  CF_CHG1|CF_USE2|CF_USE3  },  // 37 , mpy Multiply 72
{ "mpyu",  CF_CHG1|CF_USE2|CF_USE3  },  // 38 , mpyu Multiply Unsigned 73
{ "mpyi",  CF_CHG1|CF_USE2|CF_USE3  },  // 39 , mpyi Multiply Immediate 74
{ "mpyui",  CF_CHG1|CF_USE2|CF_USE3  },  // 40 , mpyui Multiply Unsigned Immediate 75
{ "mpya",  CF_CHG1|CF_USE2|CF_USE3  },  // 41 , mpya Multiply and Add 76
{ "mpyh",  CF_CHG1|CF_USE2|CF_USE3  },  // 42 , mpyh Multiply High 77
{ "mpys",  CF_CHG1|CF_USE2|CF_USE3  },  // 43 , mpys Multiply and Shift Right 78
{ "mpyhh",  CF_CHG1|CF_USE2|CF_USE3  },  // 44 , mpyhh Multiply High High 79
{ "mpyhha",  CF_CHG1|CF_USE2|CF_USE3  },  // 45 , mpyhha Multiply High High and Add 80
{ "mpyhhu",  CF_CHG1|CF_USE2|CF_USE3  },  // 46 , mpyhhu Multiply High High Unsigned 81
{ "mpyhhau",  CF_CHG1|CF_USE2|CF_USE3  },  // 47 , mpyhhau Multiply High High Unsigned and Add 82
{ "clz",  CF_CHG1| CF_USE2  },  // 48 , clz Count Leading Zeros 83
{ "cntb",  CF_CHG1| CF_USE2  },  // 49 , cntb Count Ones in Bytes 84
{ "fsmb",  CF_CHG1| CF_USE2  },  // 50 , fsmb Form Select Mask for Bytes 85
{ "fsmh",  CF_CHG1| CF_USE2  },  // 51 , fsmh Form Select Mask for Halfwords 86
{ "fsm",  CF_CHG1| CF_USE2  },  // 52 , fsm Form Select Mask for Words 87
{ "gbb",  CF_CHG1| CF_USE2  },  // 53 , gbb Gather Bits from Bytes 88
{ "gbh",  CF_CHG1| CF_USE2  },  // 54 , gbh Gather Bits from Halfwords 89
{ "gb",  CF_CHG1| CF_USE2  },  // 55 , gb Gather Bits from Words 90
{ "avgb",  CF_CHG1| CF_USE2|CF_USE3  },  // 56 , avgb Average Bytes 91
{ "absdb",  CF_CHG1| CF_USE2|CF_USE3  },  // 57 , absdb Absolute Differences of Bytes 92
{ "sumb",  CF_CHG1| CF_USE2|CF_USE3  },  // 58 , sumb Sum Bytes into Halfwords 93
{ "xsbh",  CF_CHG1| CF_USE2  },  // 59 , xsbh Extend Sign Byte to Halfword 94
{ "xshw",  CF_CHG1| CF_USE2  },  // 60 , xshw Extend Sign Halfword to Word 95
{ "xswd",  CF_CHG1| CF_USE2  },  // 61 , xswd Extend Sign Word to Doubleword 96
{ "and",  CF_CHG1| CF_USE2|CF_USE3  },  // 62 , and And 97
{ "andc",  CF_CHG1| CF_USE2|CF_USE3  },  // 63 , andc And with Complement 98
{ "andbi",  CF_CHG1| CF_USE2|CF_USE3  },  // 64 , andbi And Byte Immediate 99
{ "andhi",  CF_CHG1| CF_USE2|CF_USE3  },  // 65 , andhi And Halfword Immediate 100
{ "andi",  CF_CHG1| CF_USE2|CF_USE3  },  // 66 , andi And Word Immediate 101
{ "or",  CF_CHG1| CF_USE2|CF_USE3  },  // 67 , or Or 102
{ "orc",  CF_CHG1| CF_USE2|CF_USE3  },  // 68 , orc Or with Complement 103
{ "orbi",  CF_CHG1| CF_USE2|CF_USE3  },  // 69 , orbi Or Byte Immediate 104
{ "orhi",  CF_CHG1| CF_USE2|CF_USE3  },  // 70 , orhi Or Halfword Immediate 105
{ "ori",  CF_CHG1| CF_USE2|CF_USE3  },  // 71 , ori Or Word Immediate 106
{ "orx",  CF_CHG1| CF_USE2  },  // 72 , orx Or Across 107
{ "xor",  CF_CHG1| CF_USE2|CF_USE3  },  // 73 , xor Exclusive Or 108
{ "xorbi",  CF_CHG1| CF_USE2|CF_USE3  },  // 74 , xorbi Exclusive Or Byte Immediate 109
{ "xorhi",  CF_CHG1| CF_USE2|CF_USE3  },  // 75 , xorhi Exclusive Or Halfword Immediate 110
{ "xori",  CF_CHG1| CF_USE2|CF_USE3  },  // 76 , xori Exclusive Or Word Immediate 111
{ "nand",  CF_CHG1| CF_USE2|CF_USE3  },  // 77 , nand Nand 112
{ "nor",  CF_CHG1| CF_USE2|CF_USE3  },  // 78 , nor Nor 113
{ "eqv",  CF_CHG1| CF_USE2|CF_USE3  },  // 79 , eqv Equivalent 114
{ "selb",  CF_CHG1|CF_USE2|CF_USE3|CF_USE4  },  // 80 , selb Select Bits 115
{ "shufb",  CF_CHG1|CF_USE2|CF_USE3|CF_USE4  },  // 81 , shufb Shuffle Bytes 116
{ "shlh",  CF_CHG1| CF_USE2|CF_USE3  },  // 82 , shlh Shift Left Halfword 118
{ "shlhi",  CF_CHG1| CF_USE2|CF_USE3  },  // 83 , shlhi Shift Left Halfword Immediate 119
{ "shl",  CF_CHG1| CF_USE2|CF_USE3  },  // 84 , shl Shift Left Word 120
{ "shli",  CF_CHG1| CF_USE2|CF_USE3  },  // 85 , shli Shift Left Word Immediate 121
{ "shlqbi",  CF_CHG1| CF_USE2|CF_USE3  },  // 86 , shlqbi Shift Left Quadword by Bits 122
{ "shlqbii",  CF_CHG1| CF_USE2|CF_USE3  },  // 87 , shlqbii Shift Left Quadword by Bits Immediate 123
{ "shlqby",  CF_CHG1| CF_USE2|CF_USE3  },  // 88 , shlqby Shift Left Quadword by Bytes 124
{ "shlqbyi",  CF_CHG1| CF_USE2|CF_USE3  },  // 89 , shlqbyi Shift Left Quadword by Bytes Immediate 125
{ "shlqbybi",  CF_CHG1| CF_USE2|CF_USE3  },  // 90 , shlqbybi Shift Left Quadword by Bytes from Bit Shift Count 126
{ "roth",  CF_CHG1| CF_USE2|CF_USE3  },  // 91 , roth Rotate Halfword 127
{ "rothi",  CF_CHG1| CF_USE2|CF_USE3  },  // 92 , rothi Rotate Halfword Immediate 128
{ "rot",  CF_CHG1| CF_USE2|CF_USE3  },  // 93 , rot Rotate Word 129
{ "roti",  CF_CHG1| CF_USE2|CF_USE3  },  // 94 , roti Rotate Word Immediate 130
{ "rotqby",  CF_CHG1| CF_USE2|CF_USE3  },  // 95 , rotqby Rotate Quadword by Bytes 131
{ "rotqbyi",  CF_CHG1| CF_USE2|CF_USE3  },  // 96 , rotqbyi Rotate Quadword by Bytes Immediate 132
{ "rotqbybi",  CF_CHG1| CF_USE2|CF_USE3  },  // 97 , rotqbybi Rotate Quadword by Bytes from Bit Shift Count 133
{ "rotqbi",  CF_CHG1| CF_USE2|CF_USE3  },  // 98 , rotqbi Rotate Quadword by Bits 134
{ "rotqbii",  CF_CHG1| CF_USE2|CF_USE3  },  // 99 , rotqbii Rotate Quadword by Bits Immediate 135
{ "rothm",  CF_CHG1| CF_USE2|CF_USE3  },  // 100 , rothm Rotate and Mask Halfword 136
{ "rothmi",  CF_CHG1| CF_USE2|CF_USE3  },  // 101 , rothmi Rotate and Mask Halfword Immediate 137
{ "rotm",  CF_CHG1| CF_USE2|CF_USE3  },  // 102 , rotm Rotate and Mask Word 138
{ "rotmi",  CF_CHG1| CF_USE2|CF_USE3  },  // 103 , rotmi Rotate and Mask Word Immediate 139
{ "rotqmby",  CF_CHG1| CF_USE2|CF_USE3  },  // 104 , rotqmby Rotate and Mask Quadword by Bytes 140
{ "rotqmbyi",  CF_CHG1| CF_USE2|CF_USE3  },  // 105 , rotqmbyi Rotate and Mask Quadword by Bytes Immediate 141
{ "rotqmbybi",  CF_CHG1| CF_USE2|CF_USE3  },  // 106 , rotqmbybi Rotate and Mask Quadword Bytes from Bit Shift Count 142
{ "rotqmbi",  CF_CHG1| CF_USE2|CF_USE3  },  // 107 , rotqmbi Rotate and Mask Quadword by Bits 143
{ "rotqmbii",  CF_CHG1| CF_USE2|CF_USE3  },  // 108 , rotqmbii Rotate and Mask Quadword by Bits Immediate 144
{ "rotmah",  CF_CHG1| CF_USE2|CF_USE3  },  // 109 , rotmah Rotate and Mask Algebraic Halfword 145
{ "rotmahi",  CF_CHG1| CF_USE2|CF_USE3  },  // 110 , rotmahi Rotate and Mask Algebraic Halfword Immediate 146
{ "rotma",  CF_CHG1| CF_USE2|CF_USE3  },  // 111 , rotma Rotate and Mask Algebraic Word 147
{ "rotmai",  CF_CHG1| CF_USE2|CF_USE3  },  // 112 , rotmai Rotate and Mask Algebraic Word Immediate 148
{ "heq",  CF_USE1|CF_USE2|CF_STOP  },  // 113 , heq Halt If Equal 150
{ "heqi",  CF_USE1|CF_USE2|CF_STOP  },  // 114 , heqi Halt If Equal Immediate 151
{ "hgt",  CF_USE1|CF_USE2|CF_STOP  },  // 115 , hgt Halt If Greater Than 152
{ "hgti",  CF_USE1|CF_USE2|CF_STOP  },  // 116 , hgti Halt If Greater Than Immediate 153
{ "hlgt",  CF_USE1|CF_USE2|CF_STOP  },  // 117 , hlgt Halt If Logically Greater Than 154
{ "hlgti",  CF_USE1|CF_USE2|CF_STOP  },  // 118 , hlgti Halt If Logically Greater Than Immediate 155
{ "ceqb",  CF_CHG1| CF_USE2|CF_USE3  },  // 119 , ceqb Compare Equal Byte 156
{ "ceqbi",  CF_CHG1| CF_USE2|CF_USE3  },  // 120 , ceqbi Compare Equal Byte Immediate 157
{ "ceqh",  CF_CHG1| CF_USE2|CF_USE3  },  // 121 , ceqh Compare Equal Halfword 158
{ "ceqhi",  CF_CHG1| CF_USE2|CF_USE3  },  // 122 , ceqhi Compare Equal Halfword Immediate 159
{ "ceq",  CF_CHG1| CF_USE2|CF_USE3  },  // 123 , ceq Compare Equal Word 160
{ "ceqi",  CF_CHG1| CF_USE2|CF_USE3  },  // 124 , ceqi Compare Equal Word Immediate 161
{ "cgtb",  CF_CHG1| CF_USE2|CF_USE3  },  // 125 , cgtb Compare Greater Than Byte 162
{ "cgtbi",  CF_CHG1| CF_USE2|CF_USE3  },  // 126 , cgtbi Compare Greater Than Byte Immediate 163
{ "cgth",  CF_CHG1| CF_USE2|CF_USE3  },  // 127 , cgth Compare Greater Than Halfword 164
{ "cgthi",  CF_CHG1| CF_USE2|CF_USE3  },  // 128 , cgthi Compare Greater Than Halfword Immediate 165
{ "cgt",  CF_CHG1| CF_USE2|CF_USE3  },  // 129 , cgt Compare Greater Than Word 166
{ "cgti",  CF_CHG1| CF_USE2|CF_USE3  },  // 130 , cgti Compare Greater Than Word Immediate 167
{ "clgtb",  CF_CHG1| CF_USE2|CF_USE3  },  // 131 , clgtb Compare Logical Greater Than Byte 168
{ "clgtbi",  CF_CHG1| CF_USE2|CF_USE3  },  // 132 , clgtbi Compare Logical Greater Than Byte Immediate 169
{ "clgth",  CF_CHG1| CF_USE2|CF_USE3  },  // 133 , clgth Compare Logical Greater Than Halfword 170
{ "clgthi",  CF_CHG1| CF_USE2|CF_USE3  },  // 134 , clgthi Compare Logical Greater Than Halfword Immediate 171
{ "clgt",  CF_CHG1| CF_USE2|CF_USE3  },  // 135 , clgt Compare Logical Greater Than Word 172
{ "clgti",  CF_CHG1| CF_USE2|CF_USE3  },  // 136 , clgti Compare Logical Greater Than Word Immediate 173
{ "br",  CF_USE1|CF_STOP|CF_JUMP  },  // 137 , br Branch Relative 174
{ "bra",  CF_USE1|CF_STOP|CF_JUMP  },  // 138 , bra Branch Absolute 175
{ "brsl",  CF_CHG1|CF_USE2|CF_CALL  },  // 139 , brsl Branch Relative and Set Link in rt 176
{ "brasl",  CF_CHG1|CF_USE2|CF_CALL  },  // 140 , brasl Branch Absolute and Set Link in rt 177
{ "bi",  CF_USE1|CF_STOP|CF_JUMP  },  // 141 , bi Branch Indirect 178
{ "iret",  CF_USE1|CF_STOP  },  // 142 , iret Interrupt Return 179
{ "bisled",  CF_CHG1|CF_USE2|CF_CALL  },  // 143 , bisled Branch Indirect and Set Link if External Data 180
{ "bisl",  CF_CHG1|CF_USE2|CF_CALL  },  // 144 , bisl Branch Indirect and Set Link 181
{ "brnz",  CF_USE1|CF_USE2|CF_JUMP  },  // 145 , brnz Branch If Not Zero Word 182
{ "brz",  CF_USE1|CF_USE2|CF_JUMP  },  // 146 , brz Branch If Zero Word 183
{ "brhnz",  CF_USE1|CF_USE2|CF_JUMP  },  // 147 , brhnz Branch If Not Zero Halfword 184
{ "brhz",  CF_USE1|CF_USE2|CF_JUMP  },  // 148 , brhz Branch If Zero Halfword 185
{ "biz",  CF_USE1|CF_USE2|CF_JUMP  },  // 149 , biz Branch Indirect If Zero 186
{ "binz",  CF_USE1|CF_USE2|CF_JUMP  },  // 150 , binz Branch Indirect If Not Zero 187
{ "bihz",  CF_USE1|CF_USE2|CF_JUMP  },  // 151 , bihz Branch Indirect If Zero Halfword 188
{ "bihnz",  CF_USE1|CF_USE2|CF_JUMP  },  // 152 , bihnz Branch Indirect If Not Zero Halfword 189
{ "hbr",  CF_USE1|CF_USE2  },  // 153 , hbr Hint for Branch (r-form) 192
{ "hbra",  CF_USE1|CF_USE2  },  // 154 , hbra Hint for Branch (a-form) 193
{ "hbrr",  CF_USE1|CF_USE2  },  // 155 , hbrr Hint for Branch Relative 194
{ "fa",  CF_CHG1|CF_USE2|CF_USE3  },  // 156 , fa Floating Add 202
{ "dfa",  CF_CHG1|CF_USE2|CF_USE3  },  // 157 , dfa Double Floating Add 203
{ "fs",  CF_CHG1|CF_USE2|CF_USE3  },  // 158 , fs Floating Subtract 204
{ "dfs",  CF_CHG1|CF_USE2|CF_USE3  },  // 159 , dfs Double Floating Subtract 205
{ "fm",  CF_CHG1|CF_USE2|CF_USE3  },  // 160 , fm Floating Multiply 206
{ "dfm",  CF_CHG1|CF_USE2|CF_USE3  },  // 161 , dfm Double Floating Multiply 207
{ "fma",  CF_CHG1|CF_USE2|CF_USE3|CF_USE4  },  // 162 , fma Floating Multiply and Add 208
{ "dfma",  CF_CHG1|CF_USE2|CF_USE3  },  // 163 , dfma Double Floating Multiply and Add 209
{ "fnms",  CF_CHG1|CF_USE2|CF_USE3|CF_USE4  },  // 164 , fnms Floating Negative Multiply and Subtract 210
{ "dfnms",  CF_CHG1|CF_USE2|CF_USE3  },  // 165 , dfnms Double Floating Multiply and Subtract 211
{ "fms",  CF_CHG1|CF_USE2|CF_USE3|CF_USE4  },  // 166 , fms Floating Multiply and Subtract 212
{ "dfms",  CF_CHG1|CF_USE2|CF_USE3  },  // 167 , dfms Double Floating Multiply and Subtract 213
{ "dfnma",  CF_CHG1|CF_USE2|CF_USE3  },  // 168 , dfnma Double Floating Negative Multiply and Add 214
{ "frest",  CF_CHG1|CF_USE2  },  // 169 , frest Floating Reciprocal Estimate 215
{ "frsqest",  CF_CHG1|CF_USE2  },  // 170 , frsqest Floating Reciprocal Absolute Square Root Estimate 217
{ "fi",  CF_CHG1|CF_USE2  },  // 171 , fi Floating Interpolate 219
{ "csflt",  CF_CHG1|CF_USE2|CF_USE3  },  // 172 , csflt Convert Signed Integer to Floating 220
{ "cflts",  CF_CHG1|CF_USE2|CF_USE3  },  // 173 , cflts Convert Floating to Signed Integer 221
{ "cuflt",  CF_CHG1|CF_USE2|CF_USE3  },  // 174 , cuflt Convert Unsigned Integer to Floating 222
{ "cfltu",  CF_CHG1|CF_USE2|CF_USE3  },  // 175 , cfltu Convert Floating to Unsigned Integer 223
{ "frds",  CF_CHG1|CF_USE2  },  // 176 , frds Floating Round Double to Single 224
{ "fesd",  CF_CHG1|CF_USE2  },  // 177 , fesd Floating Extend Single to Double 225
{ "dfceq",  CF_CHG1|CF_USE2|CF_USE3  },  // 178 , dfceq Double Floating Compare Equal 226
{ "dfcmeq",  CF_CHG1|CF_USE2|CF_USE3  },  // 179 , dfcmeq Double Floating Compare Magnitude Equal 227
{ "dfcgt",  CF_CHG1|CF_USE2|CF_USE3  },  // 180 , dfcgt Double Floating Compare Greater Than 228
{ "dfcmgt",  CF_CHG1|CF_USE2|CF_USE3  },  // 181 , dfcmgt Double Floating Compare Magnitude Greater Than 229
{ "dftsv",  CF_CHG1|CF_USE2|CF_USE3  },  // 182 , dftsv Double Floating Test Special Value 230
{ "fceq",  CF_CHG1|CF_USE2|CF_USE3  },  // 183 , fceq Floating Compare Equal 231
{ "fcmeq",  CF_CHG1|CF_USE2|CF_USE3  },  // 184 , fcmeq Floating Compare Magnitude Equal 232
{ "fcgt",  CF_CHG1|CF_USE2|CF_USE3  },  // 185 , fcgt Floating Compare Greater Than 233
{ "fcmgt",  CF_CHG1|CF_USE2|CF_USE3  },  // 186 , fcmgt Floating Compare Magnitude Greater Than 234
{ "fscrrd",  CF_USE1  },  // 187 , fscrrd Floating-Point Status and Control Register Write 235
{ "fscrwr",  CF_CHG1  },  // 188 , fscrwr Floating-Point Status and Control Register Read 236
{ "stop",  CF_STOP  },  // 189 , stop Stop and Signal 238
{ "stopd",  CF_STOP  },  // 190 , stopd Stop and Signal with Dependencies 239
{ "lnop",  0  },  // 191 , lnop No Operation (Load) 240
{ "nop",  0  },  // 192 , nop No Operation (Execute) 241
{ "sync",  0  },  // 193 , sync Synchronize 242
{ "dsync",  0  },  // 194 , dsync Synchronize Data 243
{ "mfspr",  CF_CHG1|CF_USE2  },  // 195 , mfspr Move from Special-Purpose Register 244
{ "mtspr",  CF_CHG1|CF_USE2  },  // 196 , mtspr Move to Special-Purpose Register 245
{ "rdch",  CF_CHG1|CF_USE2  },  // 197 , rdch Read Channel 248
{ "rchcnt",  CF_CHG1|CF_USE2  },  // 198 , rchcnt Read Channel Count 249
{ "wrch",  CF_CHG1|CF_USE2  },  // 199 , wrch Write Channel 250


}; //total 199 instructions, excclude the two dummy opcode, null, and last.

#ifdef __BORLANDC__
#if sizeof(Instructions)/sizeof(Instructions[0]) != ps3spu_last
#error          No match:  sizeof(InstrNames) !!!
#endif
#endif
