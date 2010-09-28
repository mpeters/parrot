/* ex: set ro ft=c:
 * !!!!!!!   DO NOT EDIT THIS FILE   !!!!!!!
 *
 * This file is generated automatically from 'compilers/imcc/imcc.y'
 * by tools/build/fixup_gen_file.pl.
 *
 * Any changes made here will be lost!
 *
 */
/* HEADERIZER HFILE: none */
/* HEADERIZER STOP */
/* A Bison parser, made by GNU Bison 2.4.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006,
   2009, 2010 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     LOW_PREC = 258,
     PARAM = 259,
     SOL = 260,
     HLL = 261,
     TK_LINE = 262,
     TK_FILE = 263,
     GOTO = 264,
     ARG = 265,
     IF = 266,
     UNLESS = 267,
     PNULL = 268,
     SET_RETURN = 269,
     SET_YIELD = 270,
     ADV_FLAT = 271,
     ADV_SLURPY = 272,
     ADV_OPTIONAL = 273,
     ADV_OPT_FLAG = 274,
     ADV_NAMED = 275,
     ADV_ARROW = 276,
     ADV_INVOCANT = 277,
     ADV_CALL_SIG = 278,
     NAMESPACE = 279,
     DOT_METHOD = 280,
     SUB = 281,
     SYM = 282,
     LOCAL = 283,
     LEXICAL = 284,
     CONST = 285,
     ANNOTATE = 286,
     GLOBAL_CONST = 287,
     PLUS_ASSIGN = 288,
     MINUS_ASSIGN = 289,
     MUL_ASSIGN = 290,
     DIV_ASSIGN = 291,
     CONCAT_ASSIGN = 292,
     BAND_ASSIGN = 293,
     BOR_ASSIGN = 294,
     BXOR_ASSIGN = 295,
     FDIV = 296,
     FDIV_ASSIGN = 297,
     MOD_ASSIGN = 298,
     SHR_ASSIGN = 299,
     SHL_ASSIGN = 300,
     SHR_U_ASSIGN = 301,
     SHIFT_LEFT = 302,
     SHIFT_RIGHT = 303,
     INTV = 304,
     FLOATV = 305,
     STRINGV = 306,
     PMCV = 307,
     LOG_XOR = 308,
     RELOP_EQ = 309,
     RELOP_NE = 310,
     RELOP_GT = 311,
     RELOP_GTE = 312,
     RELOP_LT = 313,
     RELOP_LTE = 314,
     RESULT = 315,
     RETURN = 316,
     TAILCALL = 317,
     YIELDT = 318,
     GET_RESULTS = 319,
     POW = 320,
     SHIFT_RIGHT_U = 321,
     LOG_AND = 322,
     LOG_OR = 323,
     COMMA = 324,
     ESUB = 325,
     DOTDOT = 326,
     PCC_BEGIN = 327,
     PCC_END = 328,
     PCC_CALL = 329,
     PCC_SUB = 330,
     PCC_BEGIN_RETURN = 331,
     PCC_END_RETURN = 332,
     PCC_BEGIN_YIELD = 333,
     PCC_END_YIELD = 334,
     NCI_CALL = 335,
     METH_CALL = 336,
     INVOCANT = 337,
     MAIN = 338,
     LOAD = 339,
     INIT = 340,
     IMMEDIATE = 341,
     POSTCOMP = 342,
     METHOD = 343,
     ANON = 344,
     OUTER = 345,
     NEED_LEX = 346,
     MULTI = 347,
     VTABLE_METHOD = 348,
     LOADLIB = 349,
     SUB_INSTANCE_OF = 350,
     SUBID = 351,
     NS_ENTRY = 352,
     UNIQUE_REG = 353,
     LABEL = 354,
     EMIT = 355,
     EOM = 356,
     IREG = 357,
     NREG = 358,
     SREG = 359,
     PREG = 360,
     IDENTIFIER = 361,
     REG = 362,
     MACRO = 363,
     ENDM = 364,
     STRINGC = 365,
     INTC = 366,
     FLOATC = 367,
     USTRINGC = 368,
     PARROT_OP = 369,
     VAR = 370,
     LINECOMMENT = 371,
     FILECOMMENT = 372,
     DOT = 373,
     CONCAT = 374
   };
#endif
/* Tokens.  */
#define LOW_PREC 258
#define PARAM 259
#define SOL 260
#define HLL 261
#define TK_LINE 262
#define TK_FILE 263
#define GOTO 264
#define ARG 265
#define IF 266
#define UNLESS 267
#define PNULL 268
#define SET_RETURN 269
#define SET_YIELD 270
#define ADV_FLAT 271
#define ADV_SLURPY 272
#define ADV_OPTIONAL 273
#define ADV_OPT_FLAG 274
#define ADV_NAMED 275
#define ADV_ARROW 276
#define ADV_INVOCANT 277
#define ADV_CALL_SIG 278
#define NAMESPACE 279
#define DOT_METHOD 280
#define SUB 281
#define SYM 282
#define LOCAL 283
#define LEXICAL 284
#define CONST 285
#define ANNOTATE 286
#define GLOBAL_CONST 287
#define PLUS_ASSIGN 288
#define MINUS_ASSIGN 289
#define MUL_ASSIGN 290
#define DIV_ASSIGN 291
#define CONCAT_ASSIGN 292
#define BAND_ASSIGN 293
#define BOR_ASSIGN 294
#define BXOR_ASSIGN 295
#define FDIV 296
#define FDIV_ASSIGN 297
#define MOD_ASSIGN 298
#define SHR_ASSIGN 299
#define SHL_ASSIGN 300
#define SHR_U_ASSIGN 301
#define SHIFT_LEFT 302
#define SHIFT_RIGHT 303
#define INTV 304
#define FLOATV 305
#define STRINGV 306
#define PMCV 307
#define LOG_XOR 308
#define RELOP_EQ 309
#define RELOP_NE 310
#define RELOP_GT 311
#define RELOP_GTE 312
#define RELOP_LT 313
#define RELOP_LTE 314
#define RESULT 315
#define RETURN 316
#define TAILCALL 317
#define YIELDT 318
#define GET_RESULTS 319
#define POW 320
#define SHIFT_RIGHT_U 321
#define LOG_AND 322
#define LOG_OR 323
#define COMMA 324
#define ESUB 325
#define DOTDOT 326
#define PCC_BEGIN 327
#define PCC_END 328
#define PCC_CALL 329
#define PCC_SUB 330
#define PCC_BEGIN_RETURN 331
#define PCC_END_RETURN 332
#define PCC_BEGIN_YIELD 333
#define PCC_END_YIELD 334
#define NCI_CALL 335
#define METH_CALL 336
#define INVOCANT 337
#define MAIN 338
#define LOAD 339
#define INIT 340
#define IMMEDIATE 341
#define POSTCOMP 342
#define METHOD 343
#define ANON 344
#define OUTER 345
#define NEED_LEX 346
#define MULTI 347
#define VTABLE_METHOD 348
#define LOADLIB 349
#define SUB_INSTANCE_OF 350
#define SUBID 351
#define NS_ENTRY 352
#define UNIQUE_REG 353
#define LABEL 354
#define EMIT 355
#define EOM 356
#define IREG 357
#define NREG 358
#define SREG 359
#define PREG 360
#define IDENTIFIER 361
#define REG 362
#define MACRO 363
#define ENDM 364
#define STRINGC 365
#define INTC 366
#define FLOATC 367
#define USTRINGC 368
#define PARROT_OP 369
#define VAR 370
#define LINECOMMENT 371
#define FILECOMMENT 372
#define DOT 373
#define CONCAT 374




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1685 of yacc.c  */
#line 986 "compilers/imcc/imcc.y"

    IdList * idlist;
    int t;
    char * s;
    SymReg * sr;
    Instruction *i;



/* Line 1685 of yacc.c  */
#line 299 "compilers/imcc/imcparser.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




