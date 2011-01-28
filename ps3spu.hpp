/*
 *      PS3SPU module for the Interactive disassembler (IDA).
 *
 *      updates, fixes and bugreports welcomed (you know where i am)
 *
 *      (w)2011 by Jack Chen, aka anita999
 */

#ifndef _PS3SPU_HPP
#define _PS3SPU_HPP

#define LSLR 0x0003FFFF //LS 256 KB
//#define LSLR 0x0001FFFF //LS 128 KB
//#define LSLR 0x0000FFFF //LS 64 KB
//#define LSLR 0x00007FFF //LS 32 KB

// uncomment this for ida v4.8 support
//#define IDA_48
// uncomment this for ida v4.9 support
#define IDA_55
// comment out both lines above for v4.7 support

// ensure that for IDA_49 all changes for IDA_48 are taken also into consideration


#include "../idaidp.hpp"
#include "ins.hpp"
#include <diskio.hpp>

//------------------------------------------------------------------------
// customization of the 'cmd' structure:

//#define o_bit           o_idpspec0
//#define o_bitnot        o_idpspec1
#define o_regs           o_idpspec1

// fRi indirect register number (for o_phrase):
#define indreg          specflag1
//#define o_bit251        o_idpspec2
//#define b251_bit        specflag1               // bit number
//#define b251_bitneg     specflag2               // negate?

// cmd.auxpref bits:
//#define aux_0ext      0x0001  // high bit 0-extension immediate value
//#define aux_1ext      0x0002  // high bit 1-extension immediate value

// ash.uflag bit meanings:

#define UAS_PSAM        0x0001          // PseudoSam: use funny form of equ for intmem
#define UAS_SECT        0x0002          // Segments are named .SECTION
#define UAS_NOSEG       0x0004          // No 'segment' directives
#define UAS_NOBIT       0x0008          // No bit.# names, use bit_#
#define UAS_SELSG       0x0010          // Segment should be selected by its name
#define UAS_EQCLN       0x0020          // ':' in EQU directives
#define UAS_AUBIT       0x0040          // Don't use BIT directives - assembler generates bit names itself
#define UAS_CDSEG       0x0080          // Only DSEG,CSEG,XSEG
#define UAS_NODS        0x0100          // No .DS directives in Code segment
#define UAS_NOENS       0x0200          // don't specify start addr in the .end directive
#define UAS_PBIT        0x0400          // assembler knows about predefined bits
#define UAS_PBYTNODEF   0x0800          // do not define predefined byte names

enum processor_subtype_t
{
                // odd types are binary mode
                // even types are source modes
  prc_ps3spu = 0,                      // plain ps3spu
};

extern processor_subtype_t ptype;
extern char device[];
extern char deviceparams[];
 
extern ea_t intmem;               // address of the internal memory
//extern ea_t sfrmem;               // address of SFR memory

ea_t map_addr(ulong off, int opnum, bool isdata);

//------------------------------------------------------------------------
// Registers

enum ps3spu_registers
{

GPR00 = 0 ,
GPR01 = 1 ,
GPR02 = 2 ,
GPR03 = 3 ,
GPR04 = 4 ,
GPR05 = 5 ,
GPR06 = 6 ,
GPR07 = 7 ,
GPR08 = 8 ,
GPR09 = 9 ,
GPR0A = 10 ,
GPR0B = 11 ,
GPR0C = 12 ,
GPR0D = 13 ,
GPR0E = 14 ,
GPR0F = 15 ,
GPR10 = 16 ,
GPR11 = 17 ,
GPR12 = 18 ,
GPR13 = 19 ,
GPR14 = 20 ,
GPR15 = 21 ,
GPR16 = 22 ,
GPR17 = 23 ,
GPR18 = 24 ,
GPR19 = 25 ,
GPR1A = 26 ,
GPR1B = 27 ,
GPR1C = 28 ,
GPR1D = 29 ,
GPR1E = 30 ,
GPR1F = 31 ,
GPR20 = 32 ,
GPR21 = 33 ,
GPR22 = 34 ,
GPR23 = 35 ,
GPR24 = 36 ,
GPR25 = 37 ,
GPR26 = 38 ,
GPR27 = 39 ,
GPR28 = 40 ,
GPR29 = 41 ,
GPR2A = 42 ,
GPR2B = 43 ,
GPR2C = 44 ,
GPR2D = 45 ,
GPR2E = 46 ,
GPR2F = 47 ,
GPR30 = 48 ,
GPR31 = 49 ,
GPR32 = 50 ,
GPR33 = 51 ,
GPR34 = 52 ,
GPR35 = 53 ,
GPR36 = 54 ,
GPR37 = 55 ,
GPR38 = 56 ,
GPR39 = 57 ,
GPR3A = 58 ,
GPR3B = 59 ,
GPR3C = 60 ,
GPR3D = 61 ,
GPR3E = 62 ,
GPR3F = 63 ,
GPR40 = 64 ,
GPR41 = 65 ,
GPR42 = 66 ,
GPR43 = 67 ,
GPR44 = 68 ,
GPR45 = 69 ,
GPR46 = 70 ,
GPR47 = 71 ,
GPR48 = 72 ,
GPR49 = 73 ,
GPR4A = 74 ,
GPR4B = 75 ,
GPR4C = 76 ,
GPR4D = 77 ,
GPR4E = 78 ,
GPR4F = 79 ,
GPR50 = 80 ,
GPR51 = 81 ,
GPR52 = 82 ,
GPR53 = 83 ,
GPR54 = 84 ,
GPR55 = 85 ,
GPR56 = 86 ,
GPR57 = 87 ,
GPR58 = 88 ,
GPR59 = 89 ,
GPR5A = 90 ,
GPR5B = 91 ,
GPR5C = 92 ,
GPR5D = 93 ,
GPR5E = 94 ,
GPR5F = 95 ,
GPR60 = 96 ,
GPR61 = 97 ,
GPR62 = 98 ,
GPR63 = 99 ,
GPR64 = 100 ,
GPR65 = 101 ,
GPR66 = 102 ,
GPR67 = 103 ,
GPR68 = 104 ,
GPR69 = 105 ,
GPR6A = 106 ,
GPR6B = 107 ,
GPR6C = 108 ,
GPR6D = 109 ,
GPR6E = 110 ,
GPR6F = 111 ,
GPR70 = 112 ,
GPR71 = 113 ,
GPR72 = 114 ,
GPR73 = 115 ,
GPR74 = 116 ,
GPR75 = 117 ,
GPR76 = 118 ,
GPR77 = 119 ,
GPR78 = 120 ,
GPR79 = 121 ,
GPR7A = 122 ,
GPR7B = 123 ,
GPR7C = 124 ,
GPR7D = 125 ,
GPR7E = 126 ,
GPR7F = 127 ,

SPR00 = 128 ,
SPR01 = 129 ,
SPR02 = 130 ,
SPR03 = 131 ,
SPR04 = 132 ,
SPR05 = 133 ,
SPR06 = 134 ,
SPR07 = 135 ,
SPR08 = 136 ,
SPR09 = 137 ,
SPR0A = 138 ,
SPR0B = 139 ,
SPR0C = 140 ,
SPR0D = 141 ,
SPR0E = 142 ,
SPR0F = 143 ,
SPR10 = 144 ,
SPR11 = 145 ,
SPR12 = 146 ,
SPR13 = 147 ,
SPR14 = 148 ,
SPR15 = 149 ,
SPR16 = 150 ,
SPR17 = 151 ,
SPR18 = 152 ,
SPR19 = 153 ,
SPR1A = 154 ,
SPR1B = 155 ,
SPR1C = 156 ,
SPR1D = 157 ,
SPR1E = 158 ,
SPR1F = 159 ,
SPR20 = 160 ,
SPR21 = 161 ,
SPR22 = 162 ,
SPR23 = 163 ,
SPR24 = 164 ,
SPR25 = 165 ,
SPR26 = 166 ,
SPR27 = 167 ,
SPR28 = 168 ,
SPR29 = 169 ,
SPR2A = 170 ,
SPR2B = 171 ,
SPR2C = 172 ,
SPR2D = 173 ,
SPR2E = 174 ,
SPR2F = 175 ,
SPR30 = 176 ,
SPR31 = 177 ,
SPR32 = 178 ,
SPR33 = 179 ,
SPR34 = 180 ,
SPR35 = 181 ,
SPR36 = 182 ,
SPR37 = 183 ,
SPR38 = 184 ,
SPR39 = 185 ,
SPR3A = 186 ,
SPR3B = 187 ,
SPR3C = 188 ,
SPR3D = 189 ,
SPR3E = 190 ,
SPR3F = 191 ,
SPR40 = 192 ,
SPR41 = 193 ,
SPR42 = 194 ,
SPR43 = 195 ,
SPR44 = 196 ,
SPR45 = 197 ,
SPR46 = 198 ,
SPR47 = 199 ,
SPR48 = 200 ,
SPR49 = 201 ,
SPR4A = 202 ,
SPR4B = 203 ,
SPR4C = 204 ,
SPR4D = 205 ,
SPR4E = 206 ,
SPR4F = 207 ,
SPR50 = 208 ,
SPR51 = 209 ,
SPR52 = 210 ,
SPR53 = 211 ,
SPR54 = 212 ,
SPR55 = 213 ,
SPR56 = 214 ,
SPR57 = 215 ,
SPR58 = 216 ,
SPR59 = 217 ,
SPR5A = 218 ,
SPR5B = 219 ,
SPR5C = 220 ,
SPR5D = 221 ,
SPR5E = 222 ,
SPR5F = 223 ,
SPR60 = 224 ,
SPR61 = 225 ,
SPR62 = 226 ,
SPR63 = 227 ,
SPR64 = 228 ,
SPR65 = 229 ,
SPR66 = 230 ,
SPR67 = 231 ,
SPR68 = 232 ,
SPR69 = 233 ,
SPR6A = 234 ,
SPR6B = 235 ,
SPR6C = 236 ,
SPR6D = 237 ,
SPR6E = 238 ,
SPR6F = 239 ,
SPR70 = 240 ,
SPR71 = 241 ,
SPR72 = 242 ,
SPR73 = 243 ,
SPR74 = 244 ,
SPR75 = 245 ,
SPR76 = 246 ,
SPR77 = 247 ,
SPR78 = 248 ,
SPR79 = 249 ,
SPR7A = 250 ,
SPR7B = 251 ,
SPR7C = 252 ,
SPR7D = 253 ,
SPR7E = 254 ,
SPR7F = 255 ,


		//others
		lr=256,			//link register
//		spr=129,		//special purpose register
		fspscr=257,		//floating point status and control register

  rVcs, rVds            // these 2 registers are required by the IDA kernel
};

const ioport_t *find_sym(int address);
const ioport_bit_t *find_bit(int address, int bit);
bool IsPredefined(const char *name);

//------------------------------------------------------------------------
void header(void);
void footer(void);

void segstart(ea_t ea);
void segend(ea_t ea);
int is_align_insn(ea_t ea);
int     ana(void);
int     emu(void);
void    out(void);
bool    outop(op_t &op);

void    ps3spu_data(ea_t ea);

bool    idaapi ps3spu_is_switch(switch_info_t *si);
 
#endif
