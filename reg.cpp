/*
 * 	ps3spu processor module for IDA Pro.
 *	Jack Chen, aka anita999
 *	Freeware.
 */

#include "ps3spu.hpp"
#include <diskio.hpp>
#include <entry.hpp>
#include <srarea.hpp>
#include <loader.hpp>
#include <fpro.h>
#include <ctype.h>
//--------------------------------------------------------------------------
// 
static char *RegNames[] =
{
	"$LR",  //  0
"$SP",  //  1
"$2",  //  2
"$3",  //  3
"$4",  //  4
"$5",  //  5
"$6",  //  6
"$7",  //  7
"$8",  //  8
"$9",  //  9
"$10",  //  10
"$11",  //  11
"$12",  //  12
"$13",  //  13
"$14",  //  14
"$15",  //  15
"$16",  //  16
"$17",  //  17
"$18",  //  18
"$19",  //  19
"$20",  //  20
"$21",  //  21
"$22",  //  22
"$23",  //  23
"$24",  //  24
"$25",  //  25
"$26",  //  26
"$27",  //  27
"$28",  //  28
"$29",  //  29
"$30",  //  30
"$31",  //  31
"$32",  //  32
"$33",  //  33
"$34",  //  34
"$35",  //  35
"$36",  //  36
"$37",  //  37
"$38",  //  38
"$39",  //  39
"$40",  //  40
"$41",  //  41
"$42",  //  42
"$43",  //  43
"$44",  //  44
"$45",  //  45
"$46",  //  46
"$47",  //  47
"$48",  //  48
"$49",  //  49
"$50",  //  50
"$51",  //  51
"$52",  //  52
"$53",  //  53
"$54",  //  54
"$55",  //  55
"$56",  //  56
"$57",  //  57
"$58",  //  58
"$59",  //  59
"$60",  //  60
"$61",  //  61
"$62",  //  62
"$63",  //  63
"$64",  //  64
"$65",  //  65
"$66",  //  66
"$67",  //  67
"$68",  //  68
"$69",  //  69
"$70",  //  70
"$71",  //  71
"$72",  //  72
"$73",  //  73
"$74",  //  74
"$75",  //  75
"$76",  //  76
"$77",  //  77
"$78",  //  78
"$79",  //  79
"$80",  //  80
"$81",  //  81
"$82",  //  82
"$83",  //  83
"$84",  //  84
"$85",  //  85
"$86",  //  86
"$87",  //  87
"$88",  //  88
"$89",  //  89
"$90",  //  90
"$91",  //  91
"$92",  //  92
"$93",  //  93
"$94",  //  94
"$95",  //  95
"$96",  //  96
"$97",  //  97
"$98",  //  98
"$99",  //  99
"$100",  //  100
"$101",  //  101
"$102",  //  102
"$103",  //  103
"$104",  //  104
"$105",  //  105
"$106",  //  106
"$107",  //  107
"$108",  //  108
"$109",  //  109
"$110",  //  110
"$111",  //  111
"$112",  //  112
"$113",  //  113
"$114",  //  114
"$115",  //  115
"$116",  //  116
"$117",  //  117
"$118",  //  118
"$119",  //  119
"$120",  //  120
"$121",  //  121
"$122",  //  122
"$123",  //  123
"$124",  //  124
"$125",  //  125
"$126",  //  126
"$127",  //  127

"$spr0",  //  128
"$spr1",  //  129
"$spr2",  //  130
"$spr3",  //  131
"$spr4",  //  132
"$spr5",  //  133
"$spr6",  //  134
"$spr7",  //  135
"$spr8",  //  136
"$spr9",  //  137
"$spr10",  //  138
"$spr11",  //  139
"$spr12",  //  140
"$spr13",  //  141
"$spr14",  //  142
"$spr15",  //  143
"$spr16",  //  144
"$spr17",  //  145
"$spr18",  //  146
"$spr19",  //  147
"$spr20",  //  148
"$spr21",  //  149
"$spr22",  //  150
"$spr23",  //  151
"$spr24",  //  152
"$spr25",  //  153
"$spr26",  //  154
"$spr27",  //  155
"$spr28",  //  156
"$spr29",  //  157
"$spr30",  //  158
"$spr31",  //  159
"$spr32",  //  160
"$spr33",  //  161
"$spr34",  //  162
"$spr35",  //  163
"$spr36",  //  164
"$spr37",  //  165
"$spr38",  //  166
"$spr39",  //  167
"$spr40",  //  168
"$spr41",  //  169
"$spr42",  //  170
"$spr43",  //  171
"$spr44",  //  172
"$spr45",  //  173
"$spr46",  //  174
"$spr47",  //  175
"$spr48",  //  176
"$spr49",  //  177
"$spr50",  //  178
"$spr51",  //  179
"$spr52",  //  180
"$spr53",  //  181
"$spr54",  //  182
"$spr55",  //  183
"$spr56",  //  184
"$spr57",  //  185
"$spr58",  //  186
"$spr59",  //  187
"$spr60",  //  188
"$spr61",  //  189
"$spr62",  //  190
"$spr63",  //  191
"$spr64",  //  192
"$spr65",  //  193
"$spr66",  //  194
"$spr67",  //  195
"$spr68",  //  196
"$spr69",  //  197
"$spr70",  //  198
"$spr71",  //  199
"$spr72",  //  200
"$spr73",  //  201
"$spr74",  //  202
"$spr75",  //  203
"$spr76",  //  204
"$spr77",  //  205
"$spr78",  //  206
"$spr79",  //  207
"$spr80",  //  208
"$spr81",  //  209
"$spr82",  //  210
"$spr83",  //  211
"$spr84",  //  212
"$spr85",  //  213
"$spr86",  //  214
"$spr87",  //  215
"$spr88",  //  216
"$spr89",  //  217
"$spr90",  //  218
"$spr91",  //  219
"$spr92",  //  220
"$spr93",  //  221
"$spr94",  //  222
"$spr95",  //  223
"$spr96",  //  224
"$spr97",  //  225
"$spr98",  //  226
"$spr99",  //  227
"$spr100",  //  228
"$spr101",  //  229
"$spr102",  //  230
"$spr103",  //  231
"$spr104",  //  232
"$spr105",  //  233
"$spr106",  //  234
"$spr107",  //  235
"$spr108",  //  236
"$spr109",  //  237
"$spr110",  //  238
"$spr111",  //  239
"$spr112",  //  240
"$spr113",  //  241
"$spr114",  //  242
"$spr115",  //  243
"$spr116",  //  244
"$spr117",  //  245
"$spr118",  //  246
"$spr119",  //  247
"$spr120",  //  248
"$spr121",  //  249
"$spr122",  //  250
"$spr123",  //  251
"$spr124",  //  252
"$spr125",  //  253
"$spr126",  //  254
"$spr127",  //  255

"$SPU_RdEventStat" , // 256
"$SPU_WrEventMask" , // 257
"$SPU_WrEventAck" , // 258
"$SPU_RdSigNotify1" , // 259
"$SPU_RdSigNotify2" , // 260
"$ch5" , // 261
"$ch6" , // 262
"$SPU_WrDec" , // 263
"$SPU_RdDec" , // 264
"$MFC_WrMSSyncReq" , // 265
"$ch10" , // 266
"$SPU_RdEventMask" , // 267
"$MFC_RdTagMask" , // 268
"$SPU_RdMachStat" , // 269
"$SPU_WrSRR0" , // 270
"$SPU_RdSRR0" , // 271
"$MFC_LSA" , // 272
"$MFC_EAH" , // 273
"$MFC_EAL" , // 274
"$MFC_Size" , // 275
"$MFC_TagID" , // 276
"$MFC_Cmd" , // 277
"$MFC_WrTagMask" , // 278
"$MFC_WrTagUpdate" , // 279
"$MFC_RdTagStat" , // 280
"$MFC_RdListStallStat" , // 281
"$MFC_WrListStallAck" , // 282
"$MFC_RdAtomicStat" , // 283
"$SPU_WrOutMbox" , // 284
"$SPU_RdInMbox" , // 285
"$SPU_WrOutIntrMbox" , // 286
"$ch31" , // 287
"$ch32" , // 288
"$ch33" , // 289
"$ch34" , // 290
"$ch35" , // 291
"$ch36" , // 292
"$ch37" , // 293
"$ch38" , // 294
"$ch39" , // 295
"$ch40" , // 296
"$ch41" , // 297
"$ch42" , // 298
"$ch43" , // 299
"$ch44" , // 300
"$ch45" , // 301
"$ch46" , // 302
"$ch47" , // 303
"$ch48" , // 304
"$ch49" , // 305
"$ch50" , // 306
"$ch51" , // 307
"$ch52" , // 308
"$ch53" , // 309
"$ch54" , // 310
"$ch55" , // 311
"$ch56" , // 312
"$ch57" , // 313
"$ch58" , // 314
"$ch59" , // 315
"$ch60" , // 316
"$ch61" , // 317
"$ch62" , // 318
"$ch63" , // 319
"$ch64" , // 320
"$ch65" , // 321
"$ch66" , // 322
"$ch67" , // 323
"$ch68" , // 324
"$ch69" , // 325
"$ch70" , // 326
"$ch71" , // 327
"$ch72" , // 328
"$ch73" , // 329
"$ch74" , // 330
"$ch75" , // 331
"$ch76" , // 332
"$ch77" , // 333
"$ch78" , // 334
"$ch79" , // 335
"$ch80" , // 336
"$ch81" , // 337
"$ch82" , // 338
"$ch83" , // 339
"$ch84" , // 340
"$ch85" , // 341
"$ch86" , // 342
"$ch87" , // 343
"$ch88" , // 344
"$ch89" , // 345
"$ch90" , // 346
"$ch91" , // 347
"$ch92" , // 348
"$ch93" , // 349
"$ch94" , // 350
"$ch95" , // 351
"$ch96" , // 352
"$ch97" , // 353
"$ch98" , // 354
"$ch99" , // 355
"$ch100" , // 356
"$ch101" , // 357
"$ch102" , // 358
"$ch103" , // 359
"$ch104" , // 360
"$ch105" , // 361
"$ch106" , // 362
"$ch107" , // 363
"$ch108" , // 364
"$ch109" , // 365
"$ch110" , // 366
"$ch111" , // 367
"$ch112" , // 368
"$ch113" , // 369
"$ch114" , // 370
"$ch115" , // 371
"$ch116" , // 372
"$ch117" , // 373
"$ch118" , // 374
"$ch119" , // 375
"$ch120" , // 376
"$ch121" , // 377
"$ch122" , // 378
"$ch123" , // 379
"$ch124" , // 380
"$ch125" , // 381
"$ch126" , // 382
"$ch127" , // 383


"fspscr",  //  384


	"cs","ds" //?
};


//-----------------------------------------------------------------------
//           SPU assembler
//-----------------------------------------------------------------------
static asm_t spuasm =
{
  AS_COLON|AS_N2CHR|ASH_HEXF3|ASD_DECF0|ASB_BINF3|ASO_OCTF0|AS_ONEDUP,
  0,
  "GNU Assembler",
  0,
  NULL,         // header lines
  NULL,         // no bad instructions
  ".org",       // org
  ".exit",      // end

  ";",          // comment string
  '"',          // string delimiter
  '\'',         // char delimiter
  "\"'",        // special symbols in char and string constants

  ".ascii",       	 // ascii string directive
  ".byte",        	// byte directive
  ".word",       	 // word directive
  ".dword",  		// double words
  ".qword",  		// qwords
  ".octa",         // oword  (16 bytes)
  NULL,         // float  (4 bytes)
  NULL,         // double (8 bytes)
  NULL,         // tbyte  (10/12 bytes)
  NULL,         // packed decimal real
  NULL,         // arrays (#h,#d,#v,#s(...)
  ".long %s",   // uninited arrays
  ".equ",       // equ
  NULL,         // 'seg' prefix (example: push seg seg001)
  NULL,         // Pointer to checkarg_preline() function.
  NULL,         // char *(*checkarg_atomprefix)(char *operand,void *res); // if !NULL, is called before each atom
  NULL,         // const char **checkarg_operations;
  NULL,         // translation to use in character and string constants.
  NULL,         // current IP (instruction pointer)
  NULL,         // func_header
  NULL,         // func_footer
  NULL,         // "public" name keyword
  NULL,         // "weak"   name keyword
  NULL,         // "extrn"  name keyword
  NULL,         // "comm" (communal variable)
  NULL,         // get_type_name
  NULL,         // "align" keyword
  '(', ')',	// lbrace, rbrace
  NULL,         // mod
  "&",          // and
  "|",          // or
  "^",          // xor
  "~",          // not
  "<<",         // shl
  ">>",         // shr
  NULL,         // sizeof
};

static asm_t *asms[] = { &spuasm, NULL };

//--------------------------------------------------------------------------
static ioport_t *ports = NULL;
static size_t numports = 0;
char device[MAXSTR] = "";
netnode helper;


ea_t ram = BADADDR;

static bool entry_processing(ea_t &ea1)
{
  helper.altset(ea1, 1);
  create_insn(ea1);
  ea_t ea = get_first_fcref_from(ea1);
  if ( ea != BADADDR ) ea1 = ea;
  return false; // continue processing
}

#define ENTRY_PROCESSING(ea, name, cmt)  entry_processing(ea)
#include "../module/iocommon.cpp"

//--------------------------------------------------------------------------
const char *set_idp_options(const char *keyword,int /*value_type*/,const void * /*value*/)
{
  if ( keyword != NULL ) return IDPOPT_BADKEY;
  return IDPOPT_OK;
}

//#include "opcode\spu.h"
//extern const struct spu_opcode spu_opcodes[1<<11];
//--------------------------------------------------------------------------
static int notify(processor_t::idp_notify msgid, ...)
{
  va_list va;
  va_start(va, msgid);

// A well behaving processor module should call invoke_callbacks()
// in his notify() function. If this function returns 0, then
// the processor module should process the notification itself
// Otherwise the code should be returned to the caller:

  int code = invoke_callbacks(HT_IDP, msgid, va);
  if ( code ) return code;

  switch(msgid)
  {
    case processor_t::init:
		{
		helper.create("$ spu");
		inf.mf = 1;
		}
		break;

    default:
      break;

    case processor_t::term:
      free_ioports(ports, numports);
      break;

    case processor_t::newfile:   // new file loaded
      {
        
      }
      break;

    case processor_t::oldfile:   // old file loaded
      {
      }
      break;

    case processor_t::newprc:    // new processor type
      break;

    case processor_t::newasm:    // new assembler type
      break;

    case processor_t::newseg:    // new segment
      {
        
      }
      break;

    case processor_t::outlabel: // The kernel is going to generate an instruction
                                // label line or a function header
      {
        
      }
      break;

    case processor_t::move_segm:// A segment is moved
      break;
	case processor_t::is_ret_insn:
		{
/*			ea_t ea		= va_arg(va, ea_t);
			bool strict	= va_arg(va, bool);
			switch ((enum spu_insns)(&spu_opcodes[cmd.itype] - spu_opcodes))
			{
				case M_IRET:
				case M_STOP:
				case M_STOPD:
					return 2;
				default:
					return 1;
			}
*/			break;
		}
  }
  va_end(va);
  return 1;
}
#define PLFM_PS3SPU 0x8201
//-----------------------------------------------------------------------
static char *shnames[] = { "ps3spu", NULL };
static char *lnames[] = {
  "PS3 CellBE SPE",
  NULL
};

//-----------------------------------------------------------------------
//      Processor Definition
//-----------------------------------------------------------------------
processor_t LPH =
{
  IDP_INTERFACE_VERSION,        // version
  PLFM_PS3SPU,                     // id
  PRN_HEX|PR_RNAMESOK,
  8,							// 8 bits in a byte for code segments
  8,							// 8 bits in a byte for other segments

  shnames,
  lnames,

  asms,

  notify,

  header,
  footer,

  segstart,
  segend,

  NULL,                 // generate "assume" directives

  ana,                  // analyze instruction
  emu,                  // emulate instruction

  out,                  // generate text representation of instruction
  outop,                // generate ...                    operand
  intel_data,           // generate ...                    data directive
  NULL,                 // compare operands
  NULL,                 // can have type

  qnumber(RegNames), // Number of registers
  RegNames,       // Register names
  NULL,                 // get abstract register

  0,                    // Number of register files
  NULL,                 // Register file names
  NULL,                 // Register descriptions
  NULL,                 // Pointer to CPU registers

  rVcs,                 // first
  rVds,                 // last
  0,                    // size of a segment register
  rVcs, rVds,

  NULL,                 // No known code start sequences
  NULL,					// retcodes handled by is_ret_insn notification

  0,
  1<<11,
  Instructions,

  NULL,                 // int  (*is_far_jump)(int icode);
  NULL,                 // Translation function for offsets
  0,                    // int tbyte_size;  -- doesn't exist
  NULL,                 // int (*realcvt)(void *m, ushort *e, ushort swt);
  { 0, },               // char real_width[4];
                        // number of symbols after decimal point
                        // 2byte float (0-does not exist)
                        // normal float
                        // normal double
                        // long double
  NULL,                 // int (*is_switch)(switch_info_t *si);
  NULL,                 // int32 (*gen_map_file)(FILE *fp);
  NULL,                 // ea_t (*extract_address)(ea_t ea,const char *string,int x);
  NULL,                 // int (*is_sp_based)(op_t &x); -- always, so leave it NULL
  NULL,                 // int (*create_func_frame)(func_t *pfn);
  NULL,                 // int (*get_frame_retsize(func_t *pfn)
  NULL,                 // void (*gen_stkvar_def)(char *buf,const member_t *mptr,int32 v);
  gen_spcdef,           // Generate text representation of an item in a special segment
  ps3spu_iret,              // Icode of return instruction. It is ok to give any of possible return instructions
  set_idp_options,		// const char *(*set_idp_options)(const char *keyword,int value_type,const void *value);
  is_align_insn,		// int (*is_align_insn)(ea_t ea);
  NULL,					// mvm_t *mvm;
};
