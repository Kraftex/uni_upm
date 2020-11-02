/******************************************************\
 *  Autor:    Julian C.                               *
 *  Version:  v0.9.8                                  *
\******************************************************/

#define AUTHOR "Julian C."
#define VERSION "v0.9.8"
#define URL_GITHUB "https://github.com/Kraftex/uni_upm"

#include <stdio.h>
#include <stdint.h> //-> is_pc_little_endian
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#if _WIN32
#define clear_screen() system("cls")
//Other way -> printf("\033[2K")  //Solo posible si la consola soporta VT100 / VT52 (La mayoría los soporta)
//                          Mas informacion: http://ascii-table.com/ansi-escape-sequences-vt-100.php
#else
#define clear_screen() system("clear")
#endif

#ifndef DEBUG
#define DEBUG 0
#endif
#ifndef HIDE
#define HIDE 0
#endif


//Hacer que el emulador pueda decir información más útil de la intrucción que se va a ejecutar.

//Tras varios intentos fallidos intentando solo ejecutar por aparte el programa original para añadirle cosas
//He aquí mi propia implementación con herramientas extras.
typedef unsigned char uchar;    //It's like a byte...
typedef enum { r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16,
               r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31,
               pc, sr, ir, aux } Reg_type;
typedef enum { double_word, word, half_word, byte } Size_dir;
// 'check_state' -> tambien revise si se ha producido alguna excepcion
typedef enum { halt, working, by_steps, add_err, bus_err } States;    //Se podrían añadir excepciones
typedef enum { no_valid, n_deci, n_hex, n_float, str, chr, math_symb, symbols, symbol, t_num } Data_type;
typedef struct {
    uchar data[4];          //En hexadecimal
    int deci;              //Conversion rapida en 'int'
} reg32;
typedef struct {
    uchar data[8];          //En hexadecimal
    long deci;             //Conversion rapida en 'long'
} reg64;
typedef struct {            //Cosas genericas de un microprocesador
    reg32 pc;
    reg32 sr;               //No tiene porque tener 32b pero da igual...
    reg32 ir;
    char lbe;               //Denotar si usa little-endian o big-endian
    //reg32* sp;              //No se usan pero se podrían cablear a r30 y r31, respectivamente.
    //reg32* fp;
} mc;
typedef struct {
    char* symb_name;        //Nemotecnic name
    reg32 value;
} etiq;
typedef struct {
    reg32 oc;               //Operation Code
    char* format;           //Formato de visualización
    unsigned int cycles;
    void (*execute)( Reg_type, Reg_type, reg32, struct mc88110* );
} ins;
typedef struct mc88110 {            //Cosas propias del 88110
    mc micro;
    reg32 gen_regs[32];
    reg32 rg32aux;
    ins* insts;             //Ordenar usando un orden alfabetico y de longitud (alphalenght)
    int n_insts;
    etiq* etiqs;
    int n_etiqs;
    etiq* breakpoints;
    int n_brkpt;
    //char regstr[9];//DEPRECATE//Su uso es para la conversion de 'reg32' a su representacion en hexadecimal
    uchar* mem;             //256KB
    unsigned int size_mem;  //En Bytes
    States state;
    States before_state;
    int mod_pc;             //Si una instruccion lo modifica, el fetch solo se encarga de actualizar el IR
    void (*who_ovf)( Reg_type, Reg_type, reg32, struct mc88110* );
    reg32 pc_ovf;
} mc88110;

typedef struct {            //Agregar las pequeñas cosas del propio emulador
    mc88110* to_emu;
    reg32 pc_init;
    unsigned long tot_inst;
    unsigned long ciclos;
    short enabl_interup;
    int inst_execute;
    int clear;
    char cmd[1024];
    int mode;  //= 1(auto_mode)
    int sp_init;
    int fp_init;
} emulator;

typedef struct {
    int count;
    int* where_data;
    Data_type* dtype;
    char* cmdptr;
    //char* (*chrptrdata)( args, int );
} args;

//Funciones
void conf_screen ( );  //DEPRECATE...

//Como en batman usan Big-Endian al leer del archivo los numeros y el codigo están en esa forma.
int is_pc_little_endian ( )
{
    int16_t i = 1;
    int8_t *p = (int8_t*)&i;
    return *p;
}
char* int2str ( int num );                  //Reserva memoria, aún sin uso...
mc88110* init_mc88110 ( );
void free_mc88110 ( mc88110* mc );
ins* init_insts ( int* n_insts );
void free_insts ( ins* insts );
ins* lookfor_inst ( mc88110* mc, unsigned int oc_deci );// Implemented
etiq* lookfor_etiqsn ( mc88110* mc, char* symb_name );  // Implemented
etiq* lookfor_etiqn ( mc88110* mc, int num );           // Implemented
etiq* lookfor_brkpt ( mc88110* mc, unsigned int dir );  // Implemented
int decode_oc ( unsigned int inst_deci );               // Implemented
void execute_inst ( mc88110* mc );                      // Implemented
void execute_inst_woir ( mc88110* mc, reg32 pc );       // Implemented, but thinking...
void check_state ( mc88110* mc );                       // Implemented
void draw_emu ( emulator* emu );                        // Implemented, call all 'draw_' functions
void draw_state ( emulator* emu );                      // Implemented
void draw_regsflags ( mc88110* mc );                    // Implemented
void draw_promt ( char* cmd, int size );                // Implemented
void fetch ( mc88110* mc );                             // Implemented
void readfile ( mc88110* mc, FILE* bin );               // Implemented
void ins2str ( ins* inst, char* str, int deci_oc );     // Implemented
int check_breakpoint ( );
void add_breakpoint ( mc88110* mc, char* symb_name, int deci ); // Implemented
void add_brkpt_ptr ( mc88110* mc, etiq* brkpt );        // Implemented
int del_brkpt_num ( mc88110* mc, int deci );            // Implemented
void show_mem ( mc88110* mc, unsigned int dir, int size_words ); //Revisar si 'dir' está dentro
void show_mem_insts ( mc88110* mc, unsigned int dir, int size_words );

int make_something ( emulator* emu, args* arguments );   // Implemented
int basic_cmds ( emulator* emu, args* arguments );       // Implemented, in progress
int extend_cmds ( emulator* emu, args* arguments );      // Implemented, in progress

args create_args ( char* cmd, int size );                // Implemented
char* where_arg ( args* arguments, int i );
int is_dt ( args* arguments, int i, Data_type dtype );
int is_dts ( args* arguments, int size, Data_type dtypes[] );
void free_args ( args* arguments );                     // Implemented
void getaline ( char* str, int size );  //Obtener una linea.
int str2int ( char* str, int* err );                    // Implemented
int str2intg ( char* str, int* err, int base );         // Implemented
//float str2float ( char* str, int* err );  //Otro día hacer dichas intrucciones

reg32* regt2regp ( Reg_type reg, mc88110* mc );
//Deprecate
void set_reg32 ( reg32* reg, unsigned int value );

reg32 create_reg32i ( unsigned int value );
reg32 create_reg32s ( uchar* value4 );
reg32 create_reg32sh ( char* val_fhex );     //Valor en formato hexadecimal
reg64 create_reg64i ( unsigned long value );
reg64 create_reg64s ( uchar* value8 );
reg64 create_reg64sh ( char* val_fhex );     //Valor en formato hexadecimal
reg64 create_reg64freg32 ( reg32 reg );
reg64 create_reg64freg32reg32 ( reg32 reg0, reg32 reg1 );

void set_mem ( Size_dir size, mc88110* mc, reg32 dir, reg64 data );
void get_mem ( Size_dir size, mc88110* mc, reg32 dir, Reg_type dest );
//Intrucciones del procesador
//Como pueden tener mas de un formato: f1-Inmediato, f2-Regitro. <- Mentira cochina...
void stf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void stf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void ldf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void ldf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void ldf3 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void ldf4 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void andf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void andf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void addf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void addf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void adduf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void adduf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void subf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void subf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void subuf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void subuf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void xorf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void xorf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void maskf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void orf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void orf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void cmpf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void cmpf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void mulsf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void muluf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void muluf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void divsf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void divsf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void divuf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void divuf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void clrf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void clrf3 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void setf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void setf3 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void extf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void extf3 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void extuf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void extuf3 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void makf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void makf3 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void rotf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void rotf3 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void faddf4 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void fcmpf4 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void fcvtf4 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void fdivf4 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void fltf4 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void fmulf4 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void fsubf4 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void intf4 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void bb0f1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void bb1f1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void brf5 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void bsrf5 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void jmpf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void jsrf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void ldcrf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void stcrf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void xmemf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );
void stop ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc );

//Funciones para trabajar con coma flotante, más adelante incluirlas de momento tirar por lo rapido...
int chg2int ( float n )
{
    int result = 0;
    memcpy( &result, &n, sizeof(float) );
    return result;
}
long chg2long ( double n )
{
    long result = 0;
    memcpy( &result, &n, sizeof(double) );
    return result;
}
float chg2float ( int n )
{
    float result = 0;
    memcpy( &result, &n, sizeof(int) );
    return result;
}
double chg2double ( int n0, int n1 )
{
    int ns[2] = { n0, n1 };
    double result = 0;
    memcpy( &result, ns, sizeof(long) );
    return result;
}
/*int buildfloat ( int sign, int exp, int mantisa )
{
    int result = 0;
    if ( sign < 0 ) result |= 0x80000000;
    return result;
}*/
void show_reg32 ( reg32 r )
{
    printf( "reg.deci: 0x%x(%d)\n", r.deci, r.deci );
    printf( "reg.char: %c'%c'%c'%c\n", r.data[0], r.data[1], r.data[2], r.data[3] );
    printf( "reg.char-deci: %d'%d'%d'%d\n", r.data[0], r.data[1], r.data[2], r.data[3] );
}
int sign_ieee754_32 ( int int_float );
int exp_ieee754_32 ( int int_float );
int mant_ieee754_32 ( int int_float );
int sign_ieee754_64 ( long long_double );
int exp_ieee754_64 ( long long_double );
int mant_ieee754_64 ( long long_double );

/*   Esquema del emulador
 * check_state
 * draw_emu
 * -> input de args
 * --> Lo que corresponda..
 * */
/*
 * BUGS del Emulador Original
 * > <Ejct. divuf2>{v1.6-1.7} Si se está ejecutando y hay una instruccion 'divu rX, rY, rZ',
 * el emulador se queda parado en dicha instruccion "ejecutandola..." eternamente, se puede usar
 * como un 'breakpoint', esta instruccion está muy mal hecha, los registros pueden ser iguales...
 * Si hay dos instrucciones juntas puede bloquearse y el programa no avanzará más...
 * En la v1.9 ya está arreglado.
 *
 * > <Err Address>{v1.6-1.9} Si está ejecutando instrucciones y se produce un reporte de 
 * 'alineamiento a palabra', este reporte apuntará a la siguiente intruccion y no a la que lo provocó.
 *
 * > <Info. emulador> En el PDF sobre el emulador y el proyecto, contiene erratas en los codigos
 * de algunas intrucciones.
 * */
int main ( int argc, char* argv[] )
{
    if ( 2 == argc )
    {
        int ms = 0;
        emulator emu;
        memset( &emu, 0, sizeof(emulator) );
        args argmts = { 0, NULL, NULL, NULL };
        emu.to_emu = init_mc88110( );
        memset( emu.cmd, 0, 1024 );
        readfile( emu.to_emu, fopen( argv[1], "r" ) );
        //emu.ciclos = emu.tot_inst = 0;
        //emu.auto_mode = emu.sp_init = emu.fp_init = 0;
        //emu.enabl_interup = 0;
        //emu.inst_execute = 0;
        //emu.clear = 0;
        emu.pc_init = emu.to_emu->micro.pc;
        while(1)
        {
            //Pensar muy bien que ocurrirá con 'clear_screen'
            draw_emu(&emu); //-> pedir info al usuario
            if (*emu.cmd)
            {
                if ( ms == 2 ) free_args(&argmts); //-> liberar 'args'
                argmts = create_args( emu.cmd, 1024 ); //-> generar 'args'
            }
            //while(/* Something about the input */0);
            //'make_something' devolverá un numero, si ese numero es 2 corresponderá a que se emulará algo
            // o refrescar el emulador (los regitros y esas cosas...)
            while( (ms = make_something( &emu, &argmts )) != 2 )
            {
                memset( emu.cmd, 0, 1024 );
                draw_promt( emu.cmd, 1024 ); //-> pedir info al usuario
                if (*emu.cmd)
                {
                    free_args(&argmts); //-> liberar 'args'
                    argmts = create_args( emu.cmd, 1024 ); //-> generar 'args'
                }
            }
            if (emu.clear) clear_screen( );
            //Que ocurre con 'halt'...
            if ( emu.to_emu->state == halt ) printf("\n  <>>El procesador se ha parado.<<>\n\n");
            while( emu.inst_execute && emu.to_emu->state == by_steps )
            {
                emu.tot_inst++;
                //Replantear donde va 'execute_inst'...
                execute_inst(emu.to_emu);//, 0 );//Use 'decode_inst'
                if ( emu.enabl_interup && ( emu.to_emu->state == add_err || emu.to_emu->state == bus_err ) )
                    emu.inst_execute = 1;
                check_state(emu.to_emu);
                fetch(emu.to_emu);
                emu.inst_execute--;
                if (lookfor_brkpt( emu.to_emu, emu.to_emu->micro.pc.deci ))
                {
                    printf("      Punto de ruptura alcanzado.\n");
                    emu.inst_execute = 0;
                    //break;
                }
            }
            while( emu.to_emu->state == working )
            {
                //-> si hay o se reconoce un punto de ruptura, cambiar a 'by_steps' y break o continue
                emu.tot_inst++;
                execute_inst(emu.to_emu);
                if ( emu.enabl_interup && ( emu.to_emu->state == add_err || emu.to_emu->state == bus_err ) )
                {
                    check_state(emu.to_emu);
                    emu.to_emu->state = by_steps;
                    //break; //Se quita para que el PC avance a la siguiente instruccion y no se repita
                }
                check_state(emu.to_emu);
                fetch(emu.to_emu);
                if (lookfor_brkpt( emu.to_emu, emu.to_emu->micro.pc.deci ))
                {
                    //Formalizar en una funcion la informacion al usuario en el emulador
                    printf("      Punto de ruptura alcanzado.\n");
                    emu.to_emu->state = by_steps;
                    break;
                }
            }
        }
        //free_mc88110(emu.to_emu);
        //free_args(&argmts); //-> liberar 'args'
        //No es necesario ya que con 'q' se sale y se encarga de liberarlo
    }
    else
    {
        fprintf( stderr, "Formato: %s <archivo.bin>\n", argv[0] );
        return 1;
    }
    return 0;
}

//Aux
void setbit_reg32 ( reg32* reg, int kbit )
{
    reg->data[3 - kbit/8] |= 1 << (kbit % 8);
    reg->deci |= 1 << kbit;
}

//Aux
void clearbit_reg32 ( reg32* reg, int kbit )
{
    reg->data[3 - kbit/8] &= ~(1 << (kbit % 8));
    reg->deci &= ~(1 << kbit);
}

//Aux
int getbit_reg32 ( reg32* reg , int kbit )
{
#if DEBUG
    printf("DEBUG: getbit_reg32\n");
    show_reg32(*reg);
    printf( "Bloq: %u - ", kbit/8 );
    printf( "Bit: %u\n", kbit%8 );
#endif
    return !!(reg->data[3 - kbit/8] & (1 << (kbit % 8)));
    //return ((reg->data[kbit/8] & (1 << (kbit % 8)))) >> (kbit % 8);
}

//Aux, me parece mejor getbit_reg32
int testbit_reg32 ( reg32* reg, int kbit )
{
    return (reg->data[3 - kbit/8] & (1 << (kbit % 8))) == 0;
    //return !(data[kbit/8] & (1 << (kbit % 8)));
}

//Aux
int createfmt ( int o5, int w5 )
{
    int result = 0;
    int bitoffset = 1 << o5;
    while(w5--)
    {
        result |= bitoffset;
        bitoffset = bitoffset << 1;
    }
    return result;
}

//Aux
int add_ovf ( int* res, int a, int b )
{
    *res = a + b;
    if ( a > 0 && b > 0 && *res < 0 )
        return 1;
    if ( a < 0 && b < 0 && *res > 0 )
        return 1;
    return 0;
}

//Aux
int addu_ovf ( unsigned int* res, unsigned int a, unsigned int b )
{
    *res = a + b;
    if ( *res < a || *res < b )
        return 1;
    return 0;
}

//Aux
int sub_ovf ( int* res, int a, int b )
{
    *res = a - b;
    if ( a < 0 && b > 0 && *res > 0 )
        return 1;
    if ( a > 0 && b < 0 && *res < 0 )
        return 1;
    return 0;
}

//Aux
int subu_ovf ( unsigned int* res, unsigned int a, unsigned int b )
{
    *res = a - b;
    if ( (signed)*res < 0 )
        return 1;
    return 0;
}

//Aux
int muls_ovf ( int* res, int a, int b )
{
    *res = a * b;
    if ( b != 0 && *res/b != a )
        return 1;
    return 0;
}

int divs_ovf ( int* res, int a, int b )
{
    if (!b)
        return 1;
    *res = a / b;
    return 0;
}

//Aux
int ext_int ( int data, int o5, int w5 )//Puede tener errores
{
    w5 = w5 == 0? 32 : w5;
    unsigned int result = data & createfmt( o5, w5 );
    result = result >> o5;
    //
    //return !!(reg->data[kbit/8] & (1 << (kbit % 8)));
    if ( (data & (1 << (w5 - 1)) ) )
    {
        result |= createfmt( w5, 32 - w5 );
    }
    return result;
}

//Aux
unsigned int extu_int ( int data, int o5, int w5 )//Puede tener errores
{
    w5 = w5 == 0? 32 : w5;
    unsigned int result = data & createfmt( o5, w5 );
    result = result >> o5;
    return result;
}

//Aux
unsigned int rot_right ( int data, int times )
{
    while(times--)
    {
        if ( data & 0x00000001 ) 
        {
            data >>= 1;
            data += 0x80000000;
        }
        else
        {
            data >>= 1;
        }
    }
    return data;
}

//Aux
int is_little_endian ( mc88110* mc )
{
    return mc->micro.lbe == 'l' || mc->micro.lbe == 'L';
}

//Aux
int is_valid_dir ( mc88110* mc, unsigned int dir )
{
    return dir < mc->size_mem;
}

//Aux
int sizedir_bits ( Size_dir size )
{
    switch(size)
    {
        case double_word: return 64;
        case word: return 32;
        case half_word: return 16;
        case byte: return 8;
        default: return 0;
    }
}

//Aux
int sizedir_bytes ( Size_dir size )
{
    return sizedir_bits(size) / 8;
}

//Aux
void extsign_reg32 ( int size, reg32* reg )
{
    //size = (size == double_word)? half_word : size;
    if ( size == 16 && getbit_reg32( reg, size - 1 ) )
    {
        reg->data[3] = 0xff;
        reg->data[2] = 0xff;
        reg->deci = 0xffff0000 | (reg->deci & 0x0000ffff);
    }
    else if ( size == 8 && getbit_reg32( reg, size - 1 ) )
    {
        reg->data[3] = 0xff;
        reg->data[2] = 0xff;
        reg->data[1] = 0xff;
        reg->deci = 0xffffff00 | (reg->deci & 0x000000ff);
    }
    else//TODO Revisar
    {
        //reg->data[3] |= 0xfc;
        *reg = create_reg32i(ext_int( reg->deci, 0, size ));
    }
}

//Aux, deprecate
reg32 regt2reg ( Reg_type reg, mc88110* mc )
{
    if ( reg == pc )
    {
        return mc->micro.pc;
    }
    else if ( reg == sr )
    {
        return mc->micro.sr;
    }
    else if ( reg == ir )
    {
        return mc->micro.ir;
    }
    else //Registros genericos
    {
        return mc->gen_regs[reg];
    }
}

//Aux
char num2hexs ( int num ) //num to hexadecimal en string
{
    if ( num < 0 )
    {
        return '0';
    }
    else if ( 15 < num )
    {
        return 'F';
    }
    if ( -1 < num && num < 10 )
    {
        return num + '0';
    }
    return num + 'A' - 10;
}

//Aux
int hexs2num ( char hexs )
{
    if ( hexs < '0' )
    {
        return 0;
    }
    else if ( ('9' < hexs && hexs < 'A') || ('F' < hexs && hexs < 'a') || 'f' < hexs )
    {
        return 15;
    }
    if ( 'A' <= hexs && hexs <= 'F' )
    {
        return hexs - 'A' + 10;
    }
    else if ( 'a' <= hexs && hexs <= 'f' )
    {
        return hexs - 'a' + 10;
    }
    return hexs - '0';
}

//Aux
reg32 reverse_reg32 ( reg32 reg )
{
#if DEBUG
    printf( "Before: %02x %02x %02x %02x\n", reg.data[0], reg.data[1], reg.data[2], reg.data[3] );
#endif
    uchar tmp = reg.data[0];
    reg.data[0] = reg.data[3];
    reg.data[3] = tmp;
    tmp = reg.data[1];
    reg.data[1] = reg.data[2];
    reg.data[2] = tmp;
#if DEBUG
    printf( "After: %02x %02x %02x %02x\n", reg.data[0], reg.data[1], reg.data[2], reg.data[3] );
#endif
    return create_reg32s(reg.data);
}

//Aux
void check_ptr ( void* ptr )
{
    if (!ptr)
    {
        perror( "No se pudo reservar memoria" );
        exit(2);
    }
}

reg32* regt2regp ( Reg_type reg, mc88110* mc )
{
    if ( reg == pc )
    {
        return &(mc->micro.pc);
    }
    else if ( reg == sr )
    {
        return &(mc->micro.sr);
    }
    else if ( reg == aux )
    {
        return &(mc->rg32aux);
    }
    else if ( reg == ir )
    {
        return &(mc->micro.ir);
    }
    else //Registros genericos
    {
        return mc->gen_regs + reg;
    }
}

reg64 create_reg64sh ( char* val_fhex )
{
    reg64 result;
    memset( &result, 0, sizeof(reg64) );
    int offset = val_fhex[1] == 'x'? 2 : 0;
    int i = strlen(val_fhex) - offset;
    int j = 7;
    int two_steps_from_the_hell = 2;    //Good music
    int pow16 = 1;
    while( i-- && (j || two_steps_from_the_hell) )
    {
        if (!two_steps_from_the_hell)
        {
            two_steps_from_the_hell = 2;
            j--;
        }
        if ( two_steps_from_the_hell == 2 )
        {
            result.data[j] += hexs2num(val_fhex[offset + i]);
        }
        else // two_steps_from_the_hell == 1
        {
            result.data[j] += hexs2num(val_fhex[offset + i]) * 16;
        }
        result.deci +=  hexs2num(val_fhex[offset + i]) * pow16;
        two_steps_from_the_hell--;
        pow16 *= 16;
    }
    return result;
}

reg64 create_reg64s ( uchar* value8 )
{
    reg64 result;
    memcpy( result.data, value8, 8 );
    result.deci = 0;
    int i = 8;
    int pow256 = 1;
    while(i--)
    {
        result.deci += value8[i] * pow256;
        pow256 *= 256;
    }
    return result;
}

reg64 create_reg64i ( unsigned long value )
{
    reg64 result;
    result.deci = value;
    memset( result.data, 0, 8 );
    int i = 8;
    while(i--)
    {
        result.data[i] = value % 256;
        value /= 256;
    }
    return result;
}

reg64 create_reg64freg32 ( reg32 reg )
{
    reg64 result;
    memset( &result, 0, sizeof(reg64) );
    result.deci = reg.deci;
    result.data[4] = reg.data[0];
    result.data[5] = reg.data[1];
    result.data[6] = reg.data[2];
    result.data[7] = reg.data[3];
    return result;
}

reg64 create_reg64freg32reg32 ( reg32 reg0, reg32 reg1 )
{
    reg64 result;
    memset( &result, 0, sizeof(reg64) );
    result.deci |= reg0.deci * 0x100000000 + reg1.deci;
    result.data[0] = reg0.data[0];
    result.data[1] = reg0.data[1];
    result.data[2] = reg0.data[2];
    result.data[3] = reg0.data[3];
    result.data[4] = reg1.data[0];
    result.data[5] = reg1.data[1];
    result.data[6] = reg1.data[2];
    result.data[7] = reg1.data[3];
    return result;
}

reg32 create_reg32sh ( char* val_fhex )     // value está en formato hexadecimal
{
    reg32 result;
    memset( &result, 0, sizeof(reg32) );
    int offset = val_fhex[1] == 'x'? 2 : 0;
    int i = strlen(val_fhex) - offset;
    int j = 3;
    int two_steps_from_the_hell = 2;    //Good music
    int pow16 = 1;
    while( i-- && (j || two_steps_from_the_hell) )
    {
        if (!two_steps_from_the_hell)
        {
            two_steps_from_the_hell = 2;
            j--;
        }
        if ( two_steps_from_the_hell == 2 )
        {
            result.data[j] += hexs2num(val_fhex[offset + i]);
        }
        else // two_steps_from_the_hell == 1
        {
            result.data[j] += hexs2num(val_fhex[offset + i]) * 16;
        }
        result.deci +=  hexs2num(val_fhex[offset + i]) * pow16;
        two_steps_from_the_hell--;
        pow16 *= 16;
    }
    return result;
}

reg32 create_reg32s ( uchar* value4 )
{
    reg32 result;
    memcpy( result.data, value4, 4 );
    result.deci = 0;
    int i = 4;
    int pow256 = 1;
    while(i--)
    {
        result.deci += value4[i] * (unsigned)pow256;
        pow256 *= 256;
    }
    return result;
}

reg32 create_reg32i ( unsigned int value )
{
    reg32 result;
    result.deci = value;
    memset( result.data, 0, sizeof(result.data) );
    int i = 4;
    while(i--)
    {
        result.data[i] = value % 256;
        value /= 256;
    }
    return result;
}

//Aux
void write_mem ( mc88110* mc, reg32 dir, reg64 data, int size )
{
    int i = 0;
    int offset = 8 - size;
#if DEBUG
    show_reg32(dir);
#endif
    if (is_little_endian(mc))
    {
        while( i < size )
        {
            //mc->mem[dir.deci + i] = data.data[8 - i - 1];  //GOOD!!
            //mc->mem[dir.deci + 3 - i] = data.data[8 - i - 1];  //GOOD
            //mc->mem[dir.deci + size - i - 1] = data.data[offset + i];
            if ( 4 < size )
            {
                mc->mem[dir.deci + size - i - 1] = data.data[7 - i];
            }
            else  //La forma del 'else' es de las mejores, intentar ver si es plausible generalizarla...
                //mc->mem[dir.deci + 3 - i] = data.data[7 - i];
                mc->mem[(dir.deci & ~3) + 3 - i - dir.deci % 4] = data.data[7 - i];
            i++;
        }
    }
    else
    {
        while( i < size )
        {
            mc->mem[dir.deci + i] = data.data[offset + i];
            i++;
        }
    }
}

//dir y data, siempre en big-endian
void set_mem ( Size_dir size, mc88110* mc, reg32 dir, reg64 data )  //st
{
    if ( mc->size_mem <= (unsigned)dir.deci )
    {
        mc->before_state = mc->state;
        mc->state = bus_err;        //Error de dirección
        return;
    }
    switch(size)
    {
        case double_word:
            if ( dir.deci % 2 )     //Error a alineacion a palabra
            {
                mc->before_state = mc->state;
                mc->state = add_err;
                break;
            }
            write_mem( mc, dir, data, 8 );
            break;
        case word:
            if ( dir.deci % 2 )     //Error a alineacion a palabra
            {
                mc->before_state = mc->state;
                mc->state = add_err;
                break;
            }
            write_mem( mc, dir, data, 4 );
            break;
        case half_word:
            if ( dir.deci % 2 )     //Error a alineacion a palabra
            {
                mc->before_state = mc->state;
                mc->state = add_err;
                break;
            }
            write_mem( mc, dir, data, 2 );
            break;
        case byte:
            write_mem( mc, dir, data, 1 );
            break;
        default:
            break;
    }
}

//Aux, posible uso para hacer 'fetch'
void read_mem ( mc88110* mc, reg32 dir, int size, reg32* reg_dest )
{
    int i = 0;
    int offset = 8 - size;
#if DEBUG
    show_reg32(dir);
#endif
    if (is_little_endian(mc))
    {
        while( i < size )
        {
            //reg_dest->data[(offset - i + size - 1)%4] = mc->mem[dir.deci + i];
            //reg_dest->data[(offset + i)%4] = mc->mem[dir.deci + (offset + size - i - 1)%4];
            //reg_dest->data[(offset - i + size - 1)%4] = mc->mem[dir.deci + (4 - size) - i];
            //Esto se deja en forma de chapuza, buscar un forma mas lógica de hacerlo...
            if ( 4 < size )
                reg_dest->data[(offset - i + size - 1)%4] = mc->mem[dir.deci + size - i - 1];
            else  //La forma del 'else' es de las mejores, intentar ver si es plausible generalizarla...
                reg_dest->data[(offset - i + size - 1)%4] = mc->mem[(dir.deci & ~3) + 3 - i - dir.deci % 4];
            i++;
            if ( i == 4 ) //Revisar
            {
                *reg_dest = create_reg32s(reg_dest->data);
                reg_dest++;
            }
        }
    }
    else
    {
        while( i < size )
        {
            reg_dest->data[offset + i] = mc->mem[dir.deci + i];
            i++;
            if ( i == 4 )
            {
                *reg_dest = create_reg32s(reg_dest->data);
                reg_dest++;
            }
        }
    }
    *reg_dest = create_reg32s(reg_dest->data);
#if DEBUG
    show_reg32( *reg_dest );
#endif
}

void get_mem ( Size_dir size, mc88110* mc, reg32 dir, Reg_type dest )  //ld
{
    reg32* reg_dest = regt2regp( dest, mc );
    if ( mc->size_mem <= (unsigned)dir.deci )
    {
        mc->before_state = mc->state;
        mc->state = bus_err;        //Error de dirección
        return;
    }
    switch(size)
    {
        case double_word:
            if ( dir.deci % 2 )     //Error a alineacion a palabra
            {
                mc->before_state = mc->state;
                mc->state = add_err;
                break;
            }
            read_mem( mc, dir, 8, reg_dest );
            break;
        case word:
            if ( dir.deci % 2 )     //Error a alineacion a palabra
            {
                mc->before_state = mc->state;
                mc->state = add_err;
                break;
            }
            read_mem( mc, dir, 4, reg_dest );
            break;
        case half_word:
            if ( dir.deci % 2 )     //Error a alineacion a palabra
            {
                mc->before_state = mc->state;
                mc->state = add_err;
                break;
            }
            read_mem( mc, dir, 2, reg_dest );
            break;
        case byte:
            read_mem( mc, dir, 1, reg_dest );
            break;
        default:
            perror("No deberías de haber llegado aqui.");
    }
}

void show_mem ( mc88110* mc, unsigned int dir, int size_words )
{
    if ( size_words <= 0 && is_valid_dir( mc, dir ) ) return;
    dir &= ~3;
    int i = 0;
    unsigned int counter = dir & ~15;
    char* blank_space = "        ";
    if ( counter < dir )
        printf( " 0x%08x | %8d|", counter, counter );
    while( counter < dir )
    {
        printf( "    %s", blank_space );
        counter += 4;
    }
    //if (is_pc_little_endian( ))
    //{
    while( i < size_words && is_valid_dir( mc, dir ) )
    {
        if ( counter % 16 == 0 )
        {
            printf( "\n 0x%08x | %8d|", counter, counter );
        }
        printf("    ");
        //Comprobar si es 'little-endian'???
        printf( "%02X%02X%02X%02X", mc->mem[dir + 3], mc->mem[dir + 2], mc->mem[dir + 1], mc->mem[dir + 0] );
        i++;
        counter += 4;
        dir += 4;
        }
    //}
    /*else
    {
        while( i < size_words && is_valid_dir( mc, dir ) )
        {
            if ( counter % 16 == 0 )
            {
                printf( "\n 0x%08x | %8d|", counter, counter );
            }
            printf("    ");
            //Comprobar si es 'little-endian'???
            printf( "%02X%02X%02X%02X", mc->mem[dir + 0], mc->mem[dir + 1], mc->mem[dir + 2], mc->mem[dir + 3] );
            i++;
            counter += 4;
            dir += 4;
        }
    }*/
    printf("\n");
}

void show_mem_insts ( mc88110* mc, unsigned int dir, int size_words )
{
    dir &= ~3;
    int i = 0;
    char blank = '\0';
    char str[30];
    //if (is_pc_little_endian( ))
    //{
    while( i < size_words && is_valid_dir( mc, dir ) )
    {
        int oc = reverse_reg32(create_reg32s( mc->mem + dir )).deci;
        ins* inst = lookfor_inst( mc, decode_oc(oc) );
        etiq* etiquette = lookfor_etiqn( mc, dir );
        ins2str( inst, str, oc );
        if (etiquette)
            printf( "%-14.14s: 0x%08x | %8d|   %s\n", etiquette->symb_name, dir, dir, str );
        else
            printf( "%-14.14s  0x%08x | %8d|   %s\n", &blank, dir, dir, str );
        dir += 4;
        i++;
    }
    //}
    /*else
    {
        while( i < size_words && is_valid_dir( mc, dir ) )
        {
            int oc = create_reg32s( mc->mem + dir ).deci;
            ins* inst = lookfor_inst( mc, decode_oc(oc) );
            etiq* etiquette = lookfor_etiqn( mc, dir );
            ins2str( inst, str, oc );
            if (etiquette)
                printf( "%-14.14s: 0x%08x | %8d|   %s\n", etiquette->symb_name, dir, dir, str );
            else
                printf( "%-14.14s  0x%08x | %8d|   %s\n", &blank, dir, dir, str );
            dir += 4;
            i++;
        }
    }*/
}

//Deprecate
void set_reg32 ( reg32* reg, unsigned int value )
{
    *reg = create_reg32i(value);
}

char* int2str ( int num ) //Inservivble, si y bastante F
{
    char* result = NULL;
    int lenght = 1;// < num? 0 : 1;
    int pow10 = 10;
    int num_cp = num;
    while( num_cp / pow10 )
    {
        lenght++;
        pow10 *= 10;
    }
#if DEBUG
    printf( "Length: %d\n", lenght );
#endif
    lenght++;                       //Reservar espacio para '\0'
    lenght += lenght < 0? 1 : 0;    //Reservar espacio para '-'
    result = (char*)malloc(lenght);
    lenght--;
    *result = '-';
    *(result + lenght) = '\0';
    while(lenght)
    {
        lenght--;
        *(result + lenght) = '0' + num%10;
        num /= 10;
    }
    return result;
}

mc88110* init_mc88110 ( )//Introducir FILE*???
{
    mc88110* result = (mc88110*)malloc(sizeof(mc88110));
    check_ptr(result);
    memset( result, 0, sizeof(mc88110) );
    result->micro.lbe = 'l';
    result->insts = init_insts(&(result->n_insts));
    result->etiqs = NULL;
    result->n_etiqs = 0;
    result->breakpoints = NULL;
    result->n_brkpt = 0;
    result->mem = (uchar*)malloc(sizeof(uchar) * 256 * 1024);
    check_ptr(result->mem);
    memset( result->mem, 0, sizeof(uchar) * 256 * 1024 );           //Reset memory
    result->state = result->before_state = by_steps;
    result->size_mem = 256 * 1024;
    result->mod_pc = 0;
    result->who_ovf = NULL;
    return result;
}

/*
 * Los primeros 4B estan en blanco...
 * Los siguientes 4B son el punto de entrada, '-e <ETIQ>'
 * Los siguientes 4B son el numero de bytes que pertenecen al codigo
 * Los siguientes 4B son el nº de bytes que pertenecen a las etiquetas
 * Los siguientes 16B no se sabe nada...
 * Y apartir de hay aparecen las etiquetas y luego el codigo
 * */

//Rectificar y en vez de tocar todas las funciones, lo mejor es hacer que cuando se guarde la informacion
//darle la vuelta
void readfile ( mc88110* mc, FILE* bin )
{
    if (!bin)
    {
        perror("No se puede abrir el archivo");
        exit(-1);
    }
    int is_little_endian = is_pc_little_endian( );
    unsigned int i = 0, j = 0;
    int f_charnull = 0;
    unsigned int bytes_etiqs = 0;
    unsigned int bytes_code = 0;
    uchar front_info[32];
    uchar etiqstr[256];
    memset( etiqstr, 0, 256 );
    mc->micro.sr = create_reg32i(0x40000001);
    mc->micro.lbe = 'l';
    while( !feof(bin) && i < 32 )
    {
        front_info[i] = getc(bin);
#if DEBUG
        printf( "%02x'", front_info[i] );
        if ( (i + 1) % 8 == 0 ) printf("\n");
        else if ( (i + 1) % 4 == 0 ) printf(" ");
#endif
        i++;
    }
    i = 0;
    if (is_little_endian)
    {
        mc->micro.pc = reverse_reg32(create_reg32s(front_info + 4));
        bytes_code = reverse_reg32(create_reg32s(front_info + 8)).deci;
        bytes_etiqs = reverse_reg32(create_reg32s(front_info + 12)).deci;
    }
    else
    {
        mc->micro.pc = create_reg32s(front_info + 4);
        bytes_code = create_reg32s(front_info + 8).deci;
        bytes_etiqs = create_reg32s(front_info + 12).deci;
    }
#if DEBUG
    unsigned int tot_bytes = 0;
    printf( "Code: %dB - %#x\nEtiqs: %dB - %#x\nTotal: %dB - %#x\n", bytes_code, bytes_code, bytes_etiqs, bytes_etiqs, tot_bytes, tot_bytes );
#endif
    uchar* ptr_uc = etiqstr;
    while( i < bytes_etiqs )
    {
        *ptr_uc = getc(bin);
        if ( *ptr_uc == '\0' && !f_charnull )
        {
            mc->etiqs = (etiq*)realloc( mc->etiqs, sizeof(etiq) * (++(mc->n_etiqs)) );
            j = 5;
            f_charnull = ptr_uc - etiqstr + 1;
        }
        if ( j && f_charnull )
        {
            j--;
        }
        ptr_uc++;
        i++;
        if ( !j && f_charnull )
        {
            //Hacer un 'add_etiq' y buscar en ella, de momento no
            mc->etiqs[mc->n_etiqs - 1].symb_name = strdup((char*)etiqstr);
            if (is_little_endian)
                mc->etiqs[mc->n_etiqs - 1].value = reverse_reg32(create_reg32s( etiqstr + f_charnull ));
            else
                mc->etiqs[mc->n_etiqs - 1].value = create_reg32s( etiqstr + f_charnull );
#if DEBUG
            printf( "%s - %#x\n", mc->etiqs[mc->n_etiqs - 1].symb_name, mc->etiqs[mc->n_etiqs - 1].value.deci);
#endif
            ptr_uc = etiqstr;
            memset( etiqstr, 0, 256 );
            f_charnull = 0;
        }
    }
    i = 0;
    while( i < bytes_code )
    {
        if ( mc->size_mem <= i )
        {
            fprintf( stderr, "Se ha superado la capacidad de memoria(%.02fKB).\n", (double)mc->size_mem / 1024 );
            exit(-2);
        }
        if (is_little_endian)
            mc->mem[i] = getc(bin);
        else
            mc->mem[(i & ~3) + 3 - (i%4)] = getc(bin);
        i++;
    }
#if DEBUG
    printf( "Etiqs: %d\n", mc->n_etiqs );
    i = 0;
    while( i < bytes_code )     //show raw...
    {
        if ( i % 16 == 0 ) printf("| ");
        printf( "%02x ", mc->mem[i] );
        if ( (i + 1) % 16 == 0 ) printf("|\n");
        else if ( (i + 1) % 8 == 0 ) printf(" ");
        i++;
    }
#endif
    mc->mod_pc = 1;
    fetch(mc);
    //mc->micro.ir = reverse_reg32(create_reg32s( mc->mem + mc->micro.pc.deci ));
    fclose(bin);
}

void check_state ( mc88110* mc )
{
    if ( mc->state == add_err )
    {
        int dir = 0;
        char type_word[30] = "";
        int oc = mc->micro.ir.deci;
        if ( extu_int( oc, 28, 4 ) == 0x1 ) // ldf1
        {
            Reg_type reg_src1 = (Reg_type)extu_int( oc, 16, 5 );
            dir = regt2reg( reg_src1, mc ).deci + ext_int( oc, 0, 16 );
            if ( !getbit_reg32( &mc->micro.ir, 27 ) && !getbit_reg32( &mc->micro.ir, 26 ) )
                strcpy( type_word, "doble " );
            if (getbit_reg32( &mc->micro.ir, 27 ))
                strcpy( type_word, "media " );
        }
        else if ( extu_int( oc, 27, 5 ) == 0x1 ) // ldf2
        {
            Reg_type reg_src1 = (Reg_type)extu_int( oc, 16, 5 );
            dir = regt2reg( reg_src1, mc ).deci + ext_int( oc, 0, 16 );
            if (!getbit_reg32( &mc->micro.ir, 26 ))
                strcpy( type_word, "media " );
        }
        else if ( extu_int( oc, 26, 6 ) == 0x3c ) // ldf3
        {
            Reg_type reg_src1 = (Reg_type)extu_int( oc, 16, 5 );
            Reg_type reg_src2 = (Reg_type)extu_int( oc, 0, 5 );
            dir = regt2reg( reg_src1, mc ).deci + regt2reg( reg_src2, mc ).deci;
            if ( !getbit_reg32( &mc->micro.ir, 11 ) && !getbit_reg32( &mc->micro.ir, 10 ) )
                strcpy( type_word, "doble " );
            if (getbit_reg32( &mc->micro.ir, 10 ))
                strcpy( type_word, "media " );
        }
        else if ( extu_int( oc, 26, 6 ) == 0x3d ) //ldf4
        {
            Reg_type reg_src2 = (Reg_type)extu_int( oc, 0, 5 );
            dir = regt2reg( reg_src2, mc ).deci + ext_int( oc, 0, 16 );
            if (!getbit_reg32( &mc->micro.ir, 26 ))
                strcpy( type_word, "media " );
        }
        else
        {
            printf( "Aqui no deberías de haber podido llegar.\nReportalo. PC: 0x%x\n", mc->micro.pc.deci );
            return;
        }
        printf( "ERROR: Alineamiento a %spalabra erroneo. Direccion: 0x%x. PC: 0x%x\n", type_word, dir, mc->micro.pc.deci );
        mc->state = mc->before_state;
    }
    else if ( mc->state == bus_err ) //TODO revisar
    {
        int dir = 0;
        int oc = mc->micro.ir.deci;
        if ( extu_int( oc, 28, 4 ) == 0x1 || extu_int( oc, 28, 4 ) == 0x2 ) // ldf1, stf1
        {
            Reg_type reg_src2 = (Reg_type)extu_int( oc, 0, 5 );
            dir = regt2reg( reg_src2, mc ).deci + ext_int( oc, 0, 16 );
        }
        else if ( extu_int( oc, 27, 5 ) == 0x1 ) // ldf2
        {
            Reg_type reg_src1 = (Reg_type)extu_int( oc, 16, 5 );
            dir = regt2reg( reg_src1, mc ).deci + ext_int( oc, 0, 16 );
        }
        else if ( extu_int( oc, 26, 6 ) == 0x3c ) // ldf3, stf2
        {
            Reg_type reg_src1 = (Reg_type)extu_int( oc, 16, 5 );
            Reg_type reg_src2 = (Reg_type)extu_int( oc, 0, 5 );
            dir = regt2reg( reg_src1, mc ).deci + regt2reg( reg_src2, mc ).deci;
        }
        else if ( extu_int( oc, 26, 6 ) == 0x3d ) //ldf4
        {
            Reg_type reg_src2 = (Reg_type)extu_int( oc, 0, 5 );
            dir = regt2reg( reg_src2, mc ).deci + ext_int( oc, 0, 16 );
        }
        else
        {
            printf( "Aqui no deberías de haber podido llegar.\nReportalo. PC: 0x%x\n", mc->micro.pc.deci );
            return;
        }
        printf( "ERROR: Direccion no implementada. Direccion: 0x%x. PC: 0x%x\n", dir, mc->micro.pc.deci );
        mc->state = mc->before_state;
    }
    else if ( mc->state == halt ) //Habrá que ver
    {
        if ( mc->size_mem <= (unsigned)mc->micro.pc.deci )
            printf( "ERROR: Direccion no implementada. PC: 0x%x\n", mc->micro.pc.deci );
        else if ( mc->micro.pc.deci % 4 )
            printf( "ERROR: Alineamiento a palabra erroneo. PC: 0x%x\n", mc->micro.pc.deci );
        else
            printf("\n  <>>El procesador se ha parado.<<>\n\n");
    }
}

void fetch ( mc88110* mc )
{
    if (!mc->mod_pc)
        mc->micro.pc = create_reg32i( mc->micro.pc.deci + 4 );
    else
        mc->mod_pc = 0;
    if ( mc->size_mem <= (unsigned)mc->micro.pc.deci || mc->micro.pc.deci % 4 )
    {
        mc->before_state = mc->state;
        mc->state = halt;
        return;
    }
    //if (is_pc_little_endian( ))
    mc->micro.ir = reverse_reg32(create_reg32s( mc->mem + mc->micro.pc.deci ));
    //else
      //  mc->micro.ir = create_reg32s( mc->mem + mc->micro.pc.deci );
}

//https://www.geeksforgeeks.org/function-pointer-in-c/
ins create_ins ( reg32 oc, char* fmt, unsigned int cycles, void exec( Reg_type, Reg_type, reg32, mc88110* ) )
{
    ins result;
    result.oc = oc;
    result.format = fmt == NULL? NULL : strdup(fmt);        //Se ha reservado memoria
    result.cycles = cycles;
    result.execute = exec;
    return result;
}

ins* init_insts ( int* n_insts )
{
    *n_insts = 62;      //= 63, incluyendo 'break'
    ins* result = (ins*)malloc(sizeof(ins) * *n_insts);
    check_ptr(result);
#if !HIDE
    result[0] = create_ins( create_reg32i(0x08000000), "ld%-4s    r%02d,r%02d,%d", 0, ldf2 );//ld.bu, ld.hu
    result[1] = create_ins( create_reg32i(0x10000000), "ld%-4s    r%02d,r%02d,%d", 0, ldf1 );
    result[2] = create_ins( create_reg32i(0x20000000), "st%-4s    r%02d,r%02d,%d", 0, stf1 );
    result[3] = create_ins( create_reg32i(0x40000000), "and%-4s   r%02d,r%02d,0x%.4x", 0, andf1 );
    result[4] = create_ins( create_reg32i(0x48000000), "mask%-4s  r%02d,r%02d,0x%.4x", 0, maskf1 );
    result[5] = create_ins( create_reg32i(0x50000000), "xor%-4s   r%02d,r%02d,0x%.4x", 0, xorf1 );
    result[6] = create_ins( create_reg32i(0x58000000), "or%-4s    r%02d,r%02d,0x%.4x", 0, orf1 );
    result[7] = create_ins( create_reg32i(0x60000000), "addu      r%02d,r%02d,%u", 0, adduf1 );
    result[8] = create_ins( create_reg32i(0x64000000), "subu      r%02d,r%02d,%u", 0, subuf1 );
    result[9] = create_ins( create_reg32i(0x68000000), "divu      r%02d,r%02d,%u", 0, divuf1 );
    result[10] = create_ins( create_reg32i(0x68008000), "clr       r%02d,r%02d,%u<%u>", 0, clrf3 );
    result[11] = create_ins( create_reg32i(0x6c000000), "mulu      r%02d,r%02d,%u", 0, muluf1 );
    result[12] = create_ins( create_reg32i(0x70000000), "add       r%02d,r%02d,%d", 0, addf1 );
    result[13] = create_ins( create_reg32i(0x74000000), "sub       r%02d,r%02d,%d", 0, subf1 );
    result[14] = create_ins( create_reg32i(0x78000000), "divs      r%02d,r%02d,%d", 0, divsf1 );
    result[15] = create_ins( create_reg32i(0x7c000000), "cmp       r%02d,r%02d,%d", 0, cmpf1 );
    result[16] = create_ins( create_reg32i(0x80000000), "ldcr      r%02d", 0, ldcrf2 );
    result[17] = create_ins( create_reg32i(0x80008000), "stcr      r%02d", 0, stcrf2 );
    result[18] = create_ins( create_reg32i(0x84000000), "fmul%-4s  r%02d,r%02d,r%02d", 0, fmulf4 );
    result[19] = create_ins( create_reg32i(0x84000800), "fcvt%-4s  r%02d,r%02d", 0, fcvtf4 );
    result[20] = create_ins( create_reg32i(0x84002800), "fadd%-4s  r%02d,r%02d,r%02d", 0, faddf4 );
    result[21] = create_ins( create_reg32i(0x84003000), "fsub%-4s  r%02d,r%02d,r%02d", 0, fsubf4 );
    result[22] = create_ins( create_reg32i(0x84003800), "fcmp%-4s  r%02d,r%02d,r%02d", 0, fcmpf4 );
    result[23] = create_ins( create_reg32i(0x84004000), "flt%-4s   r%02d,r%02d", 0, fltf4 );
    result[24] = create_ins( create_reg32i(0x84004800), "int%-4s   r%02d,r%02d", 0, intf4 );
    result[25] = create_ins( create_reg32i(0x84007000), "fdiv%-4s  r%02d,r%02d,r%02d", 0, fdivf4 );
    result[26] = create_ins( create_reg32i(0xc0000000), "br%-4s    %d", 0, brf5 );//Signed
    result[27] = create_ins( create_reg32i(0xc8000000), "bsr%-4s   %d", 0, bsrf5 );//Signed
    result[28] = create_ins( create_reg32i(0xd0000000), "bb0%-4s   %s,r%02d,%d", 0, bb0f1 );//Signed
    result[29] = create_ins( create_reg32i(0xd8000000), "bb1%-4s   %s,r%02d,%d", 0, bb1f1 );//Signed
    result[30] = create_ins( create_reg32i(0xf0001000), "ld%-4s    r%02d,r%02d,r%02d", 0, ldf3 );
    //Not implement st.b'.wt', wt. Ni si quiera hay un sistema por el tema de las caches...
    result[31] = create_ins( create_reg32i(0xf0002000), "st%-4s    r%02d,r%02d,r%02d", 0, stf2 );
    result[32] = create_ins( create_reg32i(0xf0008800), "set       r%02d,r%02d,%u<%u>", 0, setf3 );
    result[33] = create_ins( create_reg32i(0xf0009000), "ext       r%02d,r%02d,%u<%u>", 0, extf3 );
    result[34] = create_ins( create_reg32i(0xf0009800), "extu      r%02d,r%02d,%u<%u>", 0, extuf3 );
    result[35] = create_ins( create_reg32i(0xf000a000), "mak       r%02d,r%02d,%u<%u>", 0, makf3 );
    result[36] = create_ins( create_reg32i(0xf000a800), "rot       r%02d,r%02d,<%u>", 0, rotf3 );
    result[37] = create_ins( create_reg32i(0xf4000000), "xmem      r%02d,r%02d,r%02d", 0, xmemf2 );
    result[38] = create_ins( create_reg32i(0xf4000800), "ld%-4s    r%02d,r%02d,r%02d", 0, ldf4 );//ld.bu, ld.hu
    result[39] = create_ins( create_reg32i(0xf4004000), "and%-4s   r%02d,r%02d,r%02d", 0, andf2 );
    result[40] = create_ins( create_reg32i(0xf4005000), "xor%-4s   r%02d,r%02d,r%02d", 0, xorf2 );
    result[41] = create_ins( create_reg32i(0xf4005800), "or%-4s    r%02d,r%02d,r%02d", 0, orf2 );
    result[42] = create_ins( create_reg32i(0xf4006000), "addu%-4s  r%02d,r%02d,r%02d", 0, adduf2 );
    result[43] = create_ins( create_reg32i(0xf4006400), "subu%-4s  r%02d,r%02d,r%02d", 0, subuf2 );
    result[44] = create_ins( create_reg32i(0xf4006800), "divu%-4s  r%02d,r%02d,r%02d", 0, divuf2 );
    result[45] = create_ins( create_reg32i(0xf4006c00), "mulu%-4s  r%02d,r%02d,r%02d", 0, muluf2 );
    result[46] = create_ins( create_reg32i(0xf4006e00), "muls      r%02d,r%02d,r%02d", 0, mulsf2 );//Limitado
    result[47] = create_ins( create_reg32i(0xf4007000), "add%-4s   r%02d,r%02d,r%02d", 0, addf2 );
    result[48] = create_ins( create_reg32i(0xf4007400), "sub%-4s   r%02d,r%02d,r%02d", 0, subf2 );
    result[49] = create_ins( create_reg32i(0xf4007800), "divs      r%02d,r%02d,r%02d", 0, divsf2 );
    result[50] = create_ins( create_reg32i(0xf4007c00), "cmp       r%02d,r%02d,r%02d", 0, cmpf2 );
    result[51] = create_ins( create_reg32i(0xf4008000), "clr       r%02d,r%02d,r%02d", 0, clrf2 );
    result[52] = create_ins( create_reg32i(0xf4008800), "set       r%02d,r%02d,r%02d", 0, setf2 );
    result[53] = create_ins( create_reg32i(0xf4009000), "ext       r%02d,r%02d,r%02d", 0, extf2 );
    result[54] = create_ins( create_reg32i(0xf4009800), "extu      r%02d,r%02d,r%02d", 0, extuf2 );
    result[55] = create_ins( create_reg32i(0xf400a000), "mak       r%02d,r%02d,r%02d", 0, makf2 );
    result[56] = create_ins( create_reg32i(0xf400a800), "rot       r%02d,r%02d,r%02d", 0, rotf2 );
    //La instrucción jmp provoca un salto incondicional a la dirección contenida en rS2.
    //Los dos bits menos significativos de esa dirección son puestos a cero para obtener
    //  una dirección alineada a palabra.
    result[57] = create_ins( create_reg32i(0xf400c000), "jmp%-4s   r%02d", 0, jmpf2 );
    result[58] = create_ins( create_reg32i(0xf400c800), "jsr%-4s   r%02d", 0, jsrf2 );
    result[59] = create_ins( create_reg32i(0xffffffff), "stop", 0, stop );
    result[60] = create_ins( create_reg32i(0x0), "instruccion incorrecta", 0, NULL );
    result[61] = create_ins( create_reg32i(0x0), NULL, 0, NULL );  //centinela
#endif
    return result;
}

ins* lookfor_inst ( mc88110* mc, unsigned int oc_deci )
{
    int left = 0;
    int right = mc->n_insts - 3;
    while( left <= right )
    {
        int medium = (left + right) / 2;
        if ( (unsigned)mc->insts[medium].oc.deci < oc_deci )
        {
            left = medium + 1;
        }
        else if ( (unsigned)mc->insts[medium].oc.deci > oc_deci )
        {
            right = medium - 1;
        }
        else
        {
            return mc->insts + medium;
        }
    }
    return mc->insts + (mc->n_insts - 2);
}

etiq* lookfor_etiqsn ( mc88110* mc, char* symb_name )
{
    etiq* result = NULL;
    int i = 0;
    while( i < mc->n_etiqs && !result )
    {
        if ( !strcmp( mc->etiqs[i].symb_name, symb_name ) ) result = mc->etiqs + i;
        i++;
    }
    return result;
}

etiq* lookfor_etiqn ( mc88110* mc, int num )
{
    int left = 0;
    int right = mc->n_etiqs - 1;
    while( left <= right )
    {
        int medium = (left + right) / 2;
        if ( (unsigned)mc->etiqs[medium].value.deci < (unsigned)num )
        {
            left = medium + 1;
        }
        else if ( (unsigned)mc->etiqs[medium].value.deci > (unsigned)num )
        {
            right = medium - 1;
        }
        else
        {
            return mc->etiqs + medium;
        }
    }
    return NULL;
}

etiq* lookfor_brkpt ( mc88110* mc, unsigned int dir )
{
    if (!mc->breakpoints) return NULL;
    etiq* result = NULL;
    int i = 0;
    while( i < mc->n_brkpt && !result )
    {
        if ( mc->breakpoints[i].value.deci == dir ) result = mc->breakpoints + i;
        i++;
    }
    return result;
}

//Make a 'pointerer_manager'
void add_breakpoint ( mc88110* mc, char* symb_name, int deci )
{
    int i = 0;
    deci &= ~3;
    while( i < mc->n_brkpt && mc->breakpoints[i].value.deci != deci ) i++;
    if ( i != mc->n_brkpt )
    {
        printf("      Punto de ruptura ya existente.\n");
        return;
    }
    mc->n_brkpt++;
    mc->breakpoints = (etiq*)realloc( mc->breakpoints, sizeof(etiq) * mc->n_brkpt );
    mc->breakpoints[mc->n_brkpt - 1].symb_name = strdup(symb_name);
    mc->breakpoints[mc->n_brkpt - 1].value = create_reg32i(deci);
}

void add_brkpt_ptr ( mc88110* mc, etiq* brkpt )
{
    int i = 0;
    while( i < mc->n_brkpt && mc->breakpoints[i].value.deci != brkpt->value.deci ) i++;
    if ( i != mc->n_brkpt )
    {
        printf("      Punto de ruptura ya existente.\n");
        return;
    }
    mc->n_brkpt++;
    mc->breakpoints = (etiq*)realloc( mc->breakpoints, sizeof(etiq) * mc->n_brkpt );
    //mc->breakpoints[mc->n_brkpt - 1] = *brkpt;    //No por motivos de que cada uno debe tener sus propios punteros reservando memoria
    mc->breakpoints[mc->n_brkpt - 1].symb_name = strdup(brkpt->symb_name);
    mc->breakpoints[mc->n_brkpt - 1].value = brkpt->value;
}

// del -> 1, si lo elimina
int del_brkpt_num ( mc88110* mc, int deci )
{
    int pos = 0;
    deci &= ~3;
    if (!mc->breakpoints)
    {
        printf("      No hay puntos de ruptura.\n");
        return 0;
    }
    while( pos < mc->n_brkpt && mc->breakpoints[pos].value.deci != deci ) pos++;
    if ( pos == mc->n_brkpt )
    {
        //if (!mc->n_brkpt) printf("    No hay puntos de ruptura.\n"); //Provisional
        printf( "      Punto de ruptura inexistente en '0x%x'.\n", deci );
        return 0;
    }
    free(mc->breakpoints[pos].symb_name);
    mc->n_brkpt--;
    while( pos < mc->n_brkpt )
    {
        mc->breakpoints[pos] = mc->breakpoints[pos + 1];
        pos++;
    }
    mc->breakpoints = (etiq*)realloc( mc->breakpoints, sizeof(etiq) * mc->n_brkpt );
    if (!mc->n_brkpt) mc->breakpoints = NULL;
    return 1;
}

int decode_oc ( unsigned int deci_oc )
{
    int ahead = extu_int( deci_oc, 28, 4 );
    if ( ahead == 1 || ahead == 2 ) return 0xf0000000 & deci_oc;
    ahead = extu_int( deci_oc, 27, 5 );
    if ( ahead == 8 || ahead == 10 || ahead == 9 || ahead == 11 || ahead == 26 || ahead == 27 || ahead == 25 || ahead == 24 || ahead == 1 ) return 0xf8000000 & deci_oc;
    ahead = extu_int( deci_oc, 26, 6 );
    if ( ahead == 61 )
    {
        int midhead = extu_int( deci_oc, 11, 5 );
        if ( midhead == 8 || midhead == 10 || midhead == 11 ||
             midhead == 24 || midhead == 25 || midhead == 1 ) return 0xfc00fbe0 & deci_oc;
        midhead = extu_int( deci_oc, 10, 6 );
        if ( midhead == 28 || midhead == 24 || midhead == 29 || midhead == 25 ) return 0xfc00fce0 & deci_oc;
        midhead = extu_int( deci_oc, 9, 7 );
        if ( midhead == 52 || midhead == 54 ) return 0xfc00fee0 & deci_oc;
        return 0xfc00ffe0 & deci_oc;
    }
    if ( ahead == 60 && extu_int( deci_oc, 12, 4 ) == 1 ) return 0xfc00f3e0 & deci_oc;
    if ( ahead == 60 && extu_int( deci_oc, 12, 4 ) == 2 ) return 0xfc00f1e0 & deci_oc;
    if ( ahead == 60 || ahead == 26 ) return 0xfc00fc00 & deci_oc;
    if ( ahead == 28 || ahead == 24 || ahead == 29 || ahead == 25 || ahead == 31 ||
         ahead == 27 || ahead == 30 || ahead == 26 ) return 0xfc000000 & deci_oc;
    if ( ahead == 33 ) return 0xfc00f800 & deci_oc; //TODO Revisar
    if ( ahead == 32 )
    {
        if ( extu_int( deci_oc, 15, 1 ) ) return 0xfc00ffff & deci_oc;
        else return 0xfc1fffff & deci_oc;
    }
    return deci_oc; //Si ninguna se le adapta
}

void execute_inst ( mc88110* mc )//Lo que está en el 'ir'
{
    ins* to_execute = lookfor_inst( mc, decode_oc(mc->micro.ir.deci) );
#if DEBUG
    printf( "to_execute: %s\n", to_execute->format );
#endif
    Reg_type reg_dest = (Reg_type)extu_int( mc->micro.ir.deci, 21, 5 );
    Reg_type reg_src1 = (Reg_type)extu_int( mc->micro.ir.deci, 16, 5 );
    Reg_type reg_src2 = (Reg_type)extu_int( mc->micro.ir.deci, 0, 5 );
    if (to_execute->execute)
        to_execute->execute( reg_dest, reg_src1, *regt2regp( reg_src2, mc ), mc );
    else//Darle un vistazo mejor y seguramente habría que ponerlo en 'check_state'
        printf( "    Instruccion no implementada: %s\n", to_execute->format );
    if (!to_execute->oc.deci) //Caso particular, 'Instruccion incorrecta'
        mc->state = halt;
}

void execute_inst_woir ( mc88110* mc, reg32 pc )
{
    reg32 pc_bak = mc->micro.pc;
    mc->micro.pc = pc;
    mc->micro.ir = reverse_reg32(create_reg32s( mc->mem + mc->micro.pc.deci ));
    execute_inst(mc);
    mc->micro.pc = pc_bak;
    mc->micro.ir = reverse_reg32(create_reg32s( mc->mem + mc->micro.pc.deci ));
}

void free_insts ( ins* insts )
{
    ins* cpy = insts;
    while( cpy->format )
    {
        free(cpy->format);
        cpy->format = NULL;
        cpy++;
    }
    free(insts);
}

void free_etiqs ( etiq* etiqs, int n_etiqs )
{
    while(n_etiqs--)
    {
#if DEBUG
        printf( "Etiq[%d]: %s\n", n_etiqs, etiqs[n_etiqs].symb_name );
#endif
        free(etiqs[n_etiqs].symb_name);
    }
    free(etiqs);
}

void free_mc88110 ( mc88110* mc )
{
    free_etiqs( mc->etiqs, mc->n_etiqs );
    free_insts(mc->insts);
    if (mc->breakpoints)
        free_etiqs( mc->breakpoints, mc->n_brkpt );
    free(mc->mem);
    free(mc);
    //mc = NULL;              //Para que esto funcione se necesitaría pasar un puntero de dos indirecciones
}

//Good function...
/*void freenull ( void** ptr )
{
    free(*ptr);
    *ptr = NULL;
}*/

void ins2str ( ins* inst, char* str, int deci_oc )
{
    char opt[5] = { 0, 0, 0, 0, 0 };
    int ahead = extu_int( deci_oc, 28, 4 );
    if ( ahead == 1 || ahead == 2 )
    {
        //return 0xf0000000;
        int TY = extu_int( deci_oc, 26, 2 );
        int D = extu_int( deci_oc, 21, 5 );
        int S1 = extu_int( deci_oc, 16, 5 );
        int SI16 = ext_int( deci_oc, 0, 16 );
        if ( TY != 1 ) opt[0] = '.';
        if ( TY == 0 ) opt[1] = 'd';
        if ( TY == 2 ) opt[1] = 'h';
        if ( TY == 3 ) opt[1] = 'b';
        if ( sprintf( str, inst->format, opt, D, S1, SI16 ) < 0 )
            printf( "ERROR: Al formatear la instruccion 0x%x", deci_oc );
        return;
    }
    ahead = extu_int( deci_oc, 27, 5 );
    if ( ahead == 8 || ahead == 10 || ahead == 9 || ahead == 11 )
    {
        //return 0xf8000000;
        int u = extu_int( deci_oc, 26, 1 );
        int D = extu_int( deci_oc, 21, 5 );
        int S1 = extu_int( deci_oc, 16, 5 );
        int UI16 = extu_int( deci_oc, 0, 16 );
        if (u)
        {
            opt[0] = '.';
            opt[1] = 'u';
        }
        if ( sprintf( str, inst->format, opt, D, S1, UI16 ) < 0 )
            printf( "ERROR: Al formatear la instruccion 0x%x", deci_oc );
        return;
    }
    if ( ahead == 26 || ahead == 27 )
    {
        //return 0xf8000000;
        char cnd[3] = { 0, 0, 0 };
        int n = extu_int( deci_oc, 26, 1 );
        if (n)
        {
            opt[0] = '.';
            opt[1] = 'n';
        }
        int B5 = extu_int( deci_oc, 21, 5 ); //TESTEAR
        int S1 = extu_int( deci_oc, 16, 5 );
        int SI16 = ext_int( deci_oc, 0, 16 );
        switch(B5)
        {
            case 2: strcpy( cnd, "eq" ); break;
            case 3: strcpy( cnd, "ne" ); break;
            case 4: strcpy( cnd, "gt" ); break;
            case 5: strcpy( cnd, "le" ); break;
            case 6: strcpy( cnd, "lt" ); break;
            case 7: strcpy( cnd, "ge" ); break;
            case 8: strcpy( cnd, "hi" ); break;
            case 9: strcpy( cnd, "ls" ); break;
            case 10: strcpy( cnd, "lo" ); break;
            case 11: strcpy( cnd, "hs" ); break;
            case 12: strcpy( cnd, "be" ); break;
            case 13: strcpy( cnd, "nb" ); break;
            case 14: strcpy( cnd, "he" ); break;
            case 15: strcpy( cnd, "nh" ); break;
            default: perror("No deberías haber llegado aqui.\n");
        }
        if ( sprintf( str, inst->format, opt, cnd, S1, SI16 ) < 0 )
            printf( "ERROR: Al formatear la instruccion 0x%x", deci_oc );
        return;
    }
    if ( ahead == 24 || ahead == 25 )
    {
        //return 0xf8000000;
        int n = extu_int( deci_oc, 26, 1 );
        if (n)
        {
            opt[0] = '.';
            opt[1] = 'n';
        }
        int SI26 = ext_int( deci_oc, 0, 26 );
        if ( sprintf( str, inst->format, opt, SI26 ) < 0 )
            printf( "ERROR: Al formatear la instruccion 0x%x", deci_oc );
        return;
    }
    if ( ahead == 1 )
    {
        //return 0xf8000000;
        opt[0] = '.';
        opt[2] = 'u';
        if ( extu_int( deci_oc, 26, 1 ) ) opt[1] = 'b';
        else opt[1] = 'h';
        int D = extu_int( deci_oc, 21, 5 );
        int S1 = extu_int( deci_oc, 16, 5 );
        int SI16 = ext_int( deci_oc, 0, 16 );
        if ( sprintf( str, inst->format, opt, D, S1, SI16 ) < 0 )
            printf( "ERROR: Al formatear la instruccion 0x%x", deci_oc );
        return;
    }
    ahead = extu_int( deci_oc, 26, 6 );
    if ( ahead == 61 )
    {
        int midhead = extu_int( deci_oc, 11, 5 );
        if ( midhead == 8 || midhead == 10 || midhead == 11 ) //TODO revisar...
        {
            //return 0xfc00fbe0;
            int c = extu_int( deci_oc, 10, 1 );
            if (c)
            {
                opt[0] = '.';
                opt[1] = 'c';
            }
            int D = extu_int( deci_oc, 21, 5 );
            int S1 = extu_int( deci_oc, 16, 5 );
            int S2 = extu_int( deci_oc, 0, 5 );
            if ( sprintf( str, inst->format, opt, D, S1, S2 ) < 0 )
                printf( "ERROR: Al formatear la instruccion 0x%x", deci_oc );
            return;
        }
        if ( midhead == 24 || midhead == 25 )
        {
            //return 0xfc00fbe0;
            int n = extu_int( deci_oc, 10, 1 );
            if (n)
            {
                opt[0] = '.';
                opt[1] = 'n';
            }
            int S2 = extu_int( deci_oc, 0, 5 );
            if ( sprintf( str, inst->format, opt, S2 ) < 0 )
                printf( "ERROR: Al formatear la instruccion 0x%x", deci_oc );
            return;
        }
        if ( midhead == 1 )
        {
            //return 0xfc00fbe0;
            opt[0] = '.';
            opt[2] = 'u';
            if ( extu_int( deci_oc, 26, 1 ) ) opt[1] = 'b';
            else opt[1] = 'h';
            int D = extu_int( deci_oc, 21, 5 );
            int S1 = extu_int( deci_oc, 16, 5 );
            int S2 = extu_int( deci_oc, 0, 5 );
            if ( sprintf( str, inst->format, opt, D, S1, S2 ) < 0 )
                printf( "ERROR: Al formatear la instruccion 0x%x", deci_oc );
            return;
        }
        midhead = extu_int( deci_oc, 10, 6 );
        if ( midhead == 28 || midhead == 24 || midhead == 29 || midhead == 25 )
        {
            //return 0xfc00fce0;
            int i = extu_int( deci_oc, 9, 1 );
            int o = extu_int( deci_oc, 8, 1 );
            if ( o || i )
            {
                opt[0] = '.';
                opt[1] = 'c';
                if (i) opt[2] = 'i';
                if ( i && o ) opt[3] = 'o';
                else if (o) opt[2] = 'o';
            }
            int D = extu_int( deci_oc, 21, 5 );
            int S1 = extu_int( deci_oc, 16, 5 );
            int S2 = extu_int( deci_oc, 0, 5 );
            if ( sprintf( str, inst->format, opt, D, S1, S2 ) < 0 )
                printf( "ERROR: Al formatear la instruccion 0x%x", deci_oc );
            return;
        }
        midhead = extu_int( deci_oc, 9, 7 );
        if ( midhead == 52 || midhead == 54 )
        {
            //return 0xfc00fee0;
            int d = extu_int( deci_oc, 8, 1 );
            if (d)
            {
                opt[0] = '.';
                opt[1] = 'd';
            }
            int D = extu_int( deci_oc, 21, 5 );
            int S1 = extu_int( deci_oc, 16, 5 );
            int S2 = extu_int( deci_oc, 0, 5 );
            if ( sprintf( str, inst->format, opt, D, S1, S2 ) < 0 )
                printf( "ERROR: Al formatear la instruccion 0x%x", deci_oc );
            return;
        }
        //return 0xfc00ffe0;
        int D = extu_int( deci_oc, 21, 5 );
        int S1 = extu_int( deci_oc, 16, 5 );
        int S2 = extu_int( deci_oc, 0, 5 );
        if ( sprintf( str, inst->format, D, S1, S2 ) < 0 )
            printf( "ERROR: Al formatear la instruccion 0x%x", deci_oc );
        return;
    }
    if ( ahead == 60 && extu_int( deci_oc, 12, 4 ) == 1 )
    {
        //return 0xfc00f3e0;
        int TY = extu_int( deci_oc, 10, 2 );
        int D = extu_int( deci_oc, 21, 5 );
        int S1 = extu_int( deci_oc, 16, 5 );
        int S2 = extu_int( deci_oc, 0, 5 );
        if ( TY != 1 ) opt[0] = '.';
        if ( TY == 0 ) opt[1] = 'd';
        if ( TY == 2 ) opt[1] = 'h';
        if ( TY == 3 ) opt[1] = 'b';
        if ( sprintf( str, inst->format, opt, D, S1, S2 ) < 0 )
            printf( "ERROR: Al formatear la instruccion 0x%x", deci_oc );
        return;
    }
    if ( ahead == 60 && extu_int( deci_oc, 12, 4 ) == 2 )
    {
        //return 0xfc00f1e0;
        int TY = extu_int( deci_oc, 10, 2 );
        //int T = extu_int( deci_oc, 9, 1 ); //Not Implemented
        int D = extu_int( deci_oc, 21, 5 );
        int S1 = extu_int( deci_oc, 16, 5 );
        int S2 = extu_int( deci_oc, 0, 5 );
        if ( TY != 1 ) opt[0] = '.';
        if ( TY == 0 ) opt[1] = 'd';
        if ( TY == 2 ) opt[1] = 'h';
        if ( TY == 3 ) opt[1] = 'b';
        if ( sprintf( str, inst->format, opt, D, S1, S2 ) < 0 )
            printf( "ERROR: Al formatear la instruccion 0x%x", deci_oc );
        return;
    }
    if ( ahead == 60 )//|| ahead == 26 )
    {
        //return 0xfc00fc00;
        int D = extu_int( deci_oc, 21, 5 );
        int S1 = extu_int( deci_oc, 16, 5 );
        int W5 = extu_int( deci_oc, 5, 5 );
        int O5 = extu_int( deci_oc, 0, 5 );
        if ( sprintf( str, inst->format, D, S1, W5, O5 ) < 0 )
            printf( "ERROR: Al formatear la instruccion 0x%x", deci_oc );
        return;
    }
    if ( ahead == 28 || ahead == 29 || ahead == 31 || ahead == 30 )
    {
        //return 0xfc000000;
        int D = extu_int( deci_oc, 21, 5 );
        int S1 = extu_int( deci_oc, 16, 5 );
        int SI16 = ext_int( deci_oc, 0, 16 );
        if ( sprintf( str, inst->format, D, S1, SI16 ) < 0 )
            printf( "ERROR: Al formatear la instruccion 0x%x", deci_oc );
        return;
    }
    if ( ahead == 24 || ahead == 25 || ahead == 27 || ahead == 26 )
    {
        //return 0xfc000000;
        int D = extu_int( deci_oc, 21, 5 );
        int S1 = extu_int( deci_oc, 16, 5 );
        int UI16 = extu_int( deci_oc, 0, 16 );
        if ( sprintf( str, inst->format, D, S1, UI16 ) < 0 )
            printf( "ERROR: Al formatear la instruccion 0x%x", deci_oc );
        return;
    }
    if ( ahead == 33 )
    {
        //return 0xfc00f800;
        int D = extu_int( deci_oc, 21, 5 );
        int S1 = extu_int( deci_oc, 16, 5 );
        int S2 = extu_int( deci_oc, 0, 5 );
        int T1 = extu_int( deci_oc, 9, 2 );
        int T2 = extu_int( deci_oc, 7, 2 );
        int TD = extu_int( deci_oc, 5, 2 );
        opt[0] = '.';
        if (TD) opt[1] = 'd';
        else opt[1] = 's';
        if (T1) opt[2] = 'd';
        else opt[2] = 's';
        if (T2) opt[3] = 'd';
        else opt[3] = 's';
        if ( sprintf( str, inst->format, opt, D, S1, S2 ) < 0 )
            printf( "ERROR: Al formatear la instruccion 0x%x", deci_oc );
        return;
    }
    if ( ahead == 32 )
    {
        if ( extu_int( deci_oc, 15, 1 ) )
        {
            //return 0xfc00ffff;
            int D = extu_int( deci_oc, 21, 5 );
            if ( sprintf( str, inst->format, D ) < 0 )
                printf( "ERROR: Al formatear la instruccion 0x%x", deci_oc );
            return;
        }
        else
        {
            //return 0xfc1fffff;
            int S1 = extu_int( deci_oc, 16, 5 );
            if ( sprintf( str, inst->format, S1 ) < 0 )
                printf( "ERROR: Al formatear la instruccion 0x%x", deci_oc );
            return;
        }
    }
    //return -1; //Si ninguna se le adapta
    strcpy( str, inst->format );
}

void draw_state ( emulator* emu )
{//PC = 6 digits  //Notas...
    // fmul.sss  r00,r00,r00    -> 21 caracteres
    // addu  r00,r00,65535 | add  r00,r00,-1
    // Comando: info -> r00 + 65535 (0x0000ffff) -> r00
    // Para dar más informacion acerca de la intruccion que se va a ejecutar.
    // 30 characteres para escribir
    char str[31];
    ins* inst = lookfor_inst( emu->to_emu, decode_oc(emu->to_emu->micro.ir.deci) );
    ins2str( inst, str, emu->to_emu->micro.ir.deci );
    printf( "  PC=%-6d    %s    Tot. Inst: %6lu Ciclo: %6lu  \n", emu->to_emu->micro.pc.deci, str, emu->tot_inst, emu->ciclos );
}

void draw_emu ( emulator* emu )
{
    draw_state( emu );
    draw_regsflags( emu->to_emu );
    draw_promt( emu->cmd, 1024 );
}

void draw_promt ( char* cmd, int size )
{
    printf( "88110> " );
    getaline( cmd, size );
}

void draw_regsflags ( mc88110* mc )
{
    int FL = getbit_reg32( &mc->micro.sr, 30 ); //Little-Endian o Big-Endian //Use getbit
    int FE = getbit_reg32( &mc->micro.sr, 0 );  //Excepciones
    int FC = getbit_reg32( &mc->micro.sr, 28 ); //Carrie
    int FV = getbit_reg32( &mc->micro.sr, 12 ); //Overflow
    int FR = getbit_reg32( &mc->micro.sr, 11 ) * 2 + getbit_reg32( &mc->micro.sr, 10 ); //Modo de redondeo
    printf( "  FL=%d FE=%d FC=%d FV=%d FR=%d \n", FL, FE, FC, FV, FR );
    printf( "  R01 = %08X h  R02 = %08X h  R03 = %08X h  R04 = %08X h\n",
            mc->gen_regs[1].deci, mc->gen_regs[2].deci, mc->gen_regs[3].deci, mc->gen_regs[4].deci );
    printf( "  R05 = %08X h  R06 = %08X h  R07 = %08X h  R08 = %08X h\n",
            mc->gen_regs[5].deci, mc->gen_regs[6].deci, mc->gen_regs[7].deci, mc->gen_regs[8].deci );
    printf( "  R09 = %08X h  R10 = %08X h  R11 = %08X h  R12 = %08X h\n",
            mc->gen_regs[9].deci, mc->gen_regs[10].deci, mc->gen_regs[11].deci, mc->gen_regs[12].deci );
    printf( "  R13 = %08X h  R14 = %08X h  R15 = %08X h  R16 = %08X h\n",
            mc->gen_regs[13].deci, mc->gen_regs[14].deci, mc->gen_regs[15].deci, mc->gen_regs[16].deci );
    printf( "  R17 = %08X h  R18 = %08X h  R19 = %08X h  R20 = %08X h\n",
            mc->gen_regs[17].deci, mc->gen_regs[18].deci, mc->gen_regs[19].deci, mc->gen_regs[20].deci );
    printf( "  R21 = %08X h  R22 = %08X h  R23 = %08X h  R24 = %08X h\n",
            mc->gen_regs[21].deci, mc->gen_regs[22].deci, mc->gen_regs[23].deci, mc->gen_regs[24].deci );
    printf( "  R25 = %08X h  R26 = %08X h  R27 = %08X h  R28 = %08X h\n",
            mc->gen_regs[25].deci, mc->gen_regs[26].deci, mc->gen_regs[27].deci, mc->gen_regs[28].deci );
    printf( "  R29 = %08X h  R30 = %08X h  R31 = %08X h\n",
            mc->gen_regs[29].deci, mc->gen_regs[30].deci, mc->gen_regs[31].deci );
    printf("\n");
}

int make_something ( emulator* emu, args* arguments )
{
    int result = 0;
    if ( (result = basic_cmds( emu, arguments )) ) return result;
    if ( (result = extend_cmds( emu, arguments )) ) return result;
    printf( "       Error de sintaxis.\n" );
    return result;
}

int basic_cmds ( emulator* emu, args* arguments )
{
    args argmts = *arguments;
    //if ( argmts.count == 1 && *argmts.dtype == chr )
    if (is_dts( arguments, 1, (Data_type[]){ chr } ))
    {
        switch(*where_arg( arguments, 0 ))
        {
            case 'q':
            case 'Q':
                free_mc88110(emu->to_emu);
                free_args(arguments);
                exit(0);
            case 'e':
            case 'E':
                if ( emu->to_emu->state != halt )
                    emu->to_emu->state = working;
                return 2;
                break;
            case 'p':
            case 'P': ; //<- This(;) is for the compilator don't throw an error....
                        //https://www.educative.io/edpresso/resolving-the-a-label-can-only-be-part-of-a-statement-error
                int i = 0;
                char str[40];
                reg32 oc;
                while( i < emu->to_emu->n_brkpt )
                {
                    read_mem( emu->to_emu, emu->to_emu->breakpoints[i].value, 4, &oc );
                    oc = reverse_reg32(oc);//Posiblemente haya que revisar algo de esto...
                    int deci_oc = oc.deci;
                    ins2str( lookfor_inst( emu->to_emu, decode_oc(deci_oc) ), str, deci_oc );
                    printf( "      %-8d   %-25s - <%s>\n", emu->to_emu->breakpoints[i].value.deci, str, emu->to_emu->breakpoints[i].symb_name );
                    i++;
                }
                break;
            case 't':
            case 'T':
                emu->inst_execute = 1;
                return 2;
                break;
            case 'r':
            case 'R':
                return 2;
                break;
            case 'h':
            case 'H':
                //Pensar muy bien como hacer esto y que no rompa el programa...
                printf(" Q|q -> Termina la simulacion.\n");
                printf(" H|h -> Ayuda de comandos.\n");
                printf(" E|e -> Ejecuta el programa.\n");
                printf(" R|r [<num> [valor]] -> Presenta/Modifica los registros de maquina.\n");
                printf(" D|d <dir> [N] -> Desemsambla N instrucciones a partir de direccion.\n");
                printf(" P|p [(+|-) <dir>] -> Puntos de ruptura, + añade, - suprime..\n");
                printf(" V|v <dir> [N] -> Presenta el contenido de N palabras de memoria..\n");
                printf(" T|t [num] -> Ejecuta una o varias instrucciones.\n");
                printf(" I|i <dir> <valor> -> Modifica el contenido de una palabra de memoria.\n");
                printf("\n");
                printf("    'dir'   puede ser escrito como un numero en hexadecimal o decimal\n");
                printf("    'valor' puede ser escrito como un numero en hexadecimal o decimal\n");
                printf("    'num'   solo puede ser escrito como un numero en decimal\n");
                printf("    'N'     solo puede ser escrito como un numero en decimal\n");
                printf("    <x> el parametro 'x' es obligatorio.\n");
                printf("    [x] el parametro 'x' es no es obligatorio.\n");
                printf("    (x|y) es obligatorio escoger entre el parametro 'x' o 'y'.\n");
                printf("\n");
                printf(" Para más comandos, hay que escribir 'extra'\n");
                break;
            default:
                return 0;
        }
        return 1;
    }
    //else if ( argmts.count == 2 && argmts.dtype[0] == chr && argmts.dtype[1] == n_hex )
    else if (is_dts( arguments, 2, (Data_type[]){ chr, n_hex } ))
    {
        switch(*where_arg( arguments, 0 ))
        {
            case 'd':
            case 'D':
                show_mem_insts( emu->to_emu, str2intg( where_arg( arguments, 1 ), NULL, 16 ), 15 );
                break;
            case 't':
            case 'T':
                emu->inst_execute = str2intg( argmts.cmdptr + argmts.where_data[1] + 2, NULL, 16 );
                return 2;
                break;
            case 'v':
            case 'V':
                show_mem( emu->to_emu, str2intg( where_arg( arguments, 1 ), NULL, 16 ), 20 );
                break;
            default:
                return 0;
        }
        return 1;
    }
    else if (is_dts( arguments, 2, (Data_type[]){ chr, n_deci } ))
    {
        switch(*where_arg( arguments, 0 ))
        {
            case 'd':
            case 'D':
                show_mem_insts( emu->to_emu, str2intg( where_arg( arguments, 1 ), NULL, 10 ), 15 );
                break;
            case 't':
            case 'T':
                emu->inst_execute = str2intg( argmts.cmdptr + argmts.where_data[1], NULL, 10 );
                return 2;
                break;
            case 'v':
            case 'V':
                show_mem( emu->to_emu, str2intg( where_arg( arguments, 1 ), NULL, 10 ), 20 );
                break;
            case 'r':
            case 'R': ;
                int reg_num = str2intg( where_arg( arguments, 1 ), NULL, 10 );
                reg_num = reg_num < 0 || 31 < reg_num? 31 : reg_num;
                int reg_value = emu->to_emu->gen_regs[reg_num].deci;
                printf( "      r%d -> 0x%x (%d)\n", reg_num, reg_value, reg_value );
                break;
            default:
                return 0;
        }
        return 1;
    }
    else if (is_dts( arguments, 2, (Data_type[]){ chr, str } ))
    {
        int init = argmts.cmdptr[argmts.where_data[1]] == '0' && tolower(argmts.cmdptr[argmts.where_data[1] + 1]) == 'x' ? 2: 0;
        etiq* etiquette = lookfor_etiqsn( emu->to_emu, where_arg( arguments, 1 ) );
        switch(*where_arg( arguments, 0 ))
        {
            case 'd':
            case 'D':
                if (etiquette)
                    show_mem_insts( emu->to_emu, etiquette->value.deci, 15 );
                else
                    printf( "No existe la etiqueta '%s'\n", where_arg( arguments, 1 ) );
                break;
            case 't':
            case 'T':
                //emu->inst_execute = str2intg( where_arg( arguments, 1 ), NULL, 16 );
                emu->inst_execute = str2intg( where_arg( arguments, 1 ) + init, NULL, init? 16 : 10 );
                return 2;
                break;
            case 'v':
            case 'V':
                if (etiquette)
                    show_mem( emu->to_emu, etiquette->value.deci, 20 );
                else
                    printf( "No existe la etiqueta '%s'\n", where_arg( arguments, 1 ) );
                break;
            default:
                return 0;
        }
        return 1;
    }
    else if (is_dts( arguments, 3, (Data_type[]){ chr, math_symb, n_deci } ))
    {
        unsigned int dir = str2intg( where_arg( arguments, 2 ), NULL, 10 ) & ~3;
        if ( emu->to_emu->size_mem < dir )//Para manejar a los numeros muy grandes
        {
            printf( "      Esta direccion '0x%x' no pertenece a la memoria.\n", dir );
            return 1;
        }
        if ( tolower(*where_arg( arguments, 0 )) == 'p' && *where_arg( arguments, 1 ) == '+' )
        {
            etiq* brkpt = lookfor_etiqn( emu->to_emu, dir );
            if (brkpt)
                add_brkpt_ptr( emu->to_emu, brkpt );
            else
                add_breakpoint( emu->to_emu, "", dir );
            return 1;
        }
        if ( tolower(*where_arg( arguments, 0 )) == 'p' && *where_arg( arguments, 1 ) == '-' )
        {
            del_brkpt_num( emu->to_emu, dir );
            return 1;
        }
        return 0;
    }
    else if (is_dts( arguments, 3, (Data_type[]){ chr, math_symb, n_hex } ))
    {
        unsigned int dir = str2intg( where_arg( arguments, 2 ), NULL, 16 ) & ~3;
        if (!is_valid_dir( emu->to_emu, dir ))
        {
            printf( "      Esta direccion '0x%x' no pertenece a la memoria.\n", dir );
            return 1;
        }
        if ( tolower(*where_arg( arguments, 0 )) == 'p' && *where_arg( arguments, 1 ) == '+' )
        {
            etiq* brkpt = lookfor_etiqn( emu->to_emu, dir );
            if (brkpt)
                add_brkpt_ptr( emu->to_emu, brkpt );
            else
                add_breakpoint( emu->to_emu, "", dir );
            return 1;
        }
        if ( tolower(*where_arg( arguments, 0 )) == 'p' && *where_arg( arguments, 1 ) == '-' )
        {
            del_brkpt_num( emu->to_emu, dir );
            return 1;
        }
        return 0;
    }
    else if (is_dts( arguments, 3, (Data_type[]){ chr, math_symb, str } ))
    {
        etiq* brkpt = lookfor_etiqsn( emu->to_emu, where_arg( arguments, 2 ) );
        if ( tolower(*where_arg( arguments, 0 )) == 'p' && *where_arg( arguments, 1 ) == '+' )
        {
            if (brkpt)
                add_brkpt_ptr( emu->to_emu, brkpt );
            else
                printf( "      No existe la etiqueta '%s'.\n", where_arg( arguments, 2 ) );
            return 1;
        }
        if ( tolower(*where_arg( arguments, 0 )) == 'p' && *where_arg( arguments, 1 ) == '-' )
        {
            if (brkpt)
                del_brkpt_num( emu->to_emu, brkpt->value.deci );
            else
                printf( "      No existe el punto de ruptura '%s'.\n", where_arg( arguments, 2 ) );
            return 1;
        }
        return 0;
    }
    else if (is_dts( arguments, 3, (Data_type[]){ chr, n_deci, n_deci } ))
    {
        unsigned int dir = str2intg( where_arg( arguments, 1 ), NULL, 10 );
        switch(*where_arg( arguments, 0 ))
        {
            case 'd':
            case 'D':
                show_mem_insts( emu->to_emu, dir, str2intg( where_arg( arguments, 2 ), NULL, 10 ) );
                break;
            case 'v':
            case 'V':
                show_mem( emu->to_emu, dir, str2intg( where_arg( arguments, 2 ), NULL, 10 ) );
                break;
            case 'i':
            case 'I': ;
                reg32 data = create_reg32i(str2intg( where_arg( arguments, 2 ), NULL, 10 ));
                if (!is_valid_dir( emu->to_emu, dir ))
                {
                    printf( "      Esta direccion '0x%x' no pertenece a la memoria.\n", dir );
                    return 1;
                }
                //write_mem( emu->to_emu, create_reg32i(dir), create_reg64freg32(reverse_reg32(data)), 4 );
                write_mem( emu->to_emu, create_reg32i(dir), create_reg64freg32(data), 4 );
                break;
            case 'r':
            case 'R': ;
                int reg_num = dir;
                if (reg_num)
                {
                    reg_num = reg_num < 0 || 31 < reg_num? 31 : reg_num;
                    emu->to_emu->gen_regs[reg_num] = create_reg32i(str2intg( where_arg( arguments, 2 ), NULL, 10 ));
                }
                break;
            default:
                return 0;
        }
        return 1;
    }
    else if (is_dts( arguments, 3, (Data_type[]){ chr, n_deci, n_hex } ))
    {
        unsigned int dir = str2intg( where_arg( arguments, 1 ), NULL, 10 );
        switch(*where_arg( arguments, 0 ))
        {
            case 'd':
            case 'D':
                show_mem_insts( emu->to_emu, dir, str2intg( where_arg( arguments, 2 ), NULL, 16 ) );
                break;
            case 'v':
            case 'V':
                show_mem( emu->to_emu, dir, str2intg( where_arg( arguments, 2 ), NULL, 16 ) );
                break;
            case 'i':
            case 'I': ;
                reg32 data = create_reg32i(str2intg( where_arg( arguments, 2 ), NULL, 16 ));
                if (!is_valid_dir( emu->to_emu, dir ))
                {
                    printf( "      Esta direccion '0x%x' no pertenece a la memoria.\n", dir );
                    return 1;
                }
                //write_mem( emu->to_emu, create_reg32i(dir), create_reg64freg32(reverse_reg32(data)), 4 );
                write_mem( emu->to_emu, create_reg32i(dir), create_reg64freg32(data), 4 );
                break;
            case 'r':
            case 'R': ;
                int reg_num = dir;
                if (reg_num)
                {
                    reg_num = reg_num < 0 || 31 < reg_num? 31 : reg_num;
                    emu->to_emu->gen_regs[reg_num] = create_reg32i(str2intg( where_arg( arguments, 2 ), NULL, 16 ));
                }
                break;
            default:
                return 0;
        }
        return 1;
    }
    else if (is_dts( arguments, 3, (Data_type[]){ chr, n_hex, n_hex } ))
    {
        unsigned int dir = str2intg( where_arg( arguments, 1 ), NULL, 16 );
        switch(*where_arg( arguments, 0 ))
        {
            case 'd':
            case 'D':
                show_mem_insts( emu->to_emu, dir, str2intg( where_arg( arguments, 2 ), NULL, 16 ) );
                break;
            case 'v':
            case 'V':
                show_mem( emu->to_emu, dir, str2intg( where_arg( arguments, 2 ), NULL, 16 ) );
                break;
            case 'i':
            case 'I': ;
                reg32 data = create_reg32i(str2intg( where_arg( arguments, 2 ), NULL, 16 ));
                if (!is_valid_dir( emu->to_emu, dir ))
                {
                    printf( "      Esta direccion '0x%x' no pertenece a la memoria.\n", dir );
                    return 1;
                }
                //write_mem( emu->to_emu, create_reg32i(dir), create_reg64freg32(reverse_reg32(data)), 4 );
                write_mem( emu->to_emu, create_reg32i(dir), create_reg64freg32(data), 4 );
                break;
            case 'r':
            case 'R': ;
                int reg_num = dir;
                if (reg_num)
                {
                    reg_num = reg_num < 0 || 31 < reg_num? 31 : reg_num;
                    emu->to_emu->gen_regs[reg_num] = create_reg32i(str2intg( where_arg( arguments, 2 ), NULL, 16 ));
                }
                break;
            default:
                return 0;
        }
        return 1;
    }
    else if (is_dts( arguments, 3, (Data_type[]){ chr, n_hex, n_deci } ))
    {
        unsigned int dir = str2intg( where_arg( arguments, 1 ), NULL, 16 );
        switch(*where_arg( arguments, 0 ))
        {
            case 'd':
            case 'D':
                show_mem_insts( emu->to_emu, dir, str2intg( where_arg( arguments, 2 ), NULL, 10 ) );
                break;
            case 'v':
            case 'V':
                show_mem( emu->to_emu, dir, str2intg( where_arg( arguments, 2 ), NULL, 10 ) );
                break;
            case 'i':
            case 'I': ;
                reg32 data = create_reg32i(str2intg( where_arg( arguments, 2 ), NULL, 10 ));
                if (!is_valid_dir( emu->to_emu, dir ))
                {
                    printf( "      Esta direccion '0x%x' no pertenece a la memoria.\n", dir );
                    return 1;
                }
                //write_mem( emu->to_emu, create_reg32i(dir), create_reg64freg32(reverse_reg32(data)), 4 );
                write_mem( emu->to_emu, create_reg32i(dir), create_reg64freg32(data), 4 );
                break;
            case 'r':
            case 'R': ;
                int reg_num = str2intg( where_arg( arguments, 1 ), NULL, 10 );
                if (reg_num)
                {
                    reg_num = reg_num < 0 || 31 < reg_num? 31 : reg_num;
                    emu->to_emu->gen_regs[reg_num] = create_reg32i(str2intg( where_arg( arguments, 2 ), NULL, 10 ));
                }
                break;
            default:
                return 0;
        }
        return 1;
    }
    return 0;
}

int extend_cmds ( emulator* emu, args* arguments )
{
    //pc, nombrar los registros...
    //SP, revisarlo de forma generica o mediante lo que deberían de guardar las subrutinas
    //auto, para el tema del SP y FP
    //config <STR> para cambiar cosas del emulador...
    //  > inter: Para que el emulador se pare si ocurre cualquier error en el procesador...
    //show_img <str|dir>, img <str|dir>
    //calc, para hacer operaciones simples...
    //raw, revisar la memoria como está almacenada de verdad
    //Hacer que el emulador pueda decir información más útil de la intrucción que se va a ejecutar.
    //Mediante comando 'info'
    //reload -> recargar el archivo
    //reset -> solo restaura el PC e IR
    //load <STR> -> cargar otro archivo
    //comp <STR> -> compilar un archivo
    //emu <STR> -> compila y carga el archivo compilado
    //look <DIR> [N] -> mirar alrededor de una instruccion
    //stack [STR] -> interpreta la informacion de la pila, de una manera más comprensible.
    //etiqs -> Muestra las etiquetas y sus valores
    if (is_dts( arguments, 1, (Data_type[]){ str } ))
    {
        if (!strcmp( where_arg( arguments, 0 ), "reset" ))
        {
            emu->to_emu->state = by_steps;
            emu->inst_execute = 0; //Por si acaso
            emu->to_emu->micro.pc = emu->pc_init;
            emu->to_emu->mod_pc = 1;
            emu->tot_inst = 0;
            emu->ciclos = 0;
            fetch(emu->to_emu);
            return 2;
        }
        else if (!strcmp( where_arg( arguments, 0 ), "auto" ))
        {
            emu->mode = 1;
            printf("    Auto modo activado.\n"\
                   "Si el registro 29 y el 30 se ven modificados, se detectará y se guardarán los valores.\n"\
                   "Para monitorearlos y usarlos en 'stack'.\n");
            return 1;
        }
        else if (!strcmp( where_arg( arguments, 0 ), "about" ))
        {
            printf("  Emulador creado por "AUTHOR"\n");
            printf("    Version: "VERSION"\n");
            printf("      Mas informacion en GitHub: "URL_GITHUB"\n");
            return 1;
        }
        else if (!strcmp( where_arg( arguments, 0 ), "look" ))
        {
            show_mem_insts( emu->to_emu, emu->to_emu->micro.pc.deci - 5*4, 5 );
            printf(">>> ");
            show_mem_insts( emu->to_emu, emu->to_emu->micro.pc.deci, 1 );
            show_mem_insts( emu->to_emu, emu->to_emu->micro.pc.deci + 4, 5 );
            return 1;
        }
        else if (!strcmp( where_arg( arguments, 0 ), "etiqs" )) //help de los nuevos comandos...
        {
            int i = 0;
            while( i < emu->to_emu->n_etiqs )
            {
                printf( "      %25s - 0x%x(%d)\n", emu->to_emu->etiqs[i].symb_name,
                        emu->to_emu->etiqs[i].value.deci, emu->to_emu->etiqs[i].value.deci );
                i++;
            }
            return 1;
        }
        else if (!strcmp( where_arg( arguments, 0 ), "extra" )) //help de los nuevos comandos...
        {
            printf(" about -> Muestra informacion acerca del emulador.\n");
            printf(" auto -> Configuracion automatica sobre el SP y el FP para mirar la pila.\n");
            printf(" about -> Muestra informacion acerca del emulador.\n");
            printf(" about -> Muestra informacion acerca del emulador.\n");
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else if (is_dts( arguments, 2, (Data_type[]){ str, n_deci } ))
    {
        if (!strcmp( where_arg( arguments, 0 ), "look" ))  //manejar 'look 0'??
        {//
            //int show = 1 + 2*5;//*str2intg( where_arg( arguments, 1 ), NULL, 10 );
            //show_mem_insts( emu->to_emu, str2intg( where_arg( arguments, 1 ), NULL, 10 ) - 5*4, show );
            show_mem_insts( emu->to_emu, str2intg( where_arg( arguments, 1 ), NULL, 10 ) - 5*4, 5 );
            printf(">>> ");
            show_mem_insts( emu->to_emu, str2intg( where_arg( arguments, 1 ), NULL, 10 ), 1 );
            show_mem_insts( emu->to_emu, str2intg( where_arg( arguments, 1 ), NULL, 10 ) + 4, 5 );
            return 1;
        }
    }
    else if (is_dts( arguments, 2, (Data_type[]){ str, n_hex } ))
    {
        if (!strcmp( where_arg( arguments, 0 ), "look" ))  //manejar 'look 0'??
        {//
            //int show = 1 + 2*5;//*str2intg( where_arg( arguments, 1 ), NULL, 10 );
            show_mem_insts( emu->to_emu, str2intg( where_arg( arguments, 1 ), NULL, 16 ) - 5*4, 5 );
            printf(">>> ");
            show_mem_insts( emu->to_emu, str2intg( where_arg( arguments, 1 ), NULL, 16 ), 1 );
            show_mem_insts( emu->to_emu, str2intg( where_arg( arguments, 1 ), NULL, 16 ) + 4, 5 );
            return 1;
        }
    }
    else if (is_dts( arguments, 2, (Data_type[]){ str, str } ))
    {
        if ( !strcmp( where_arg( arguments, 0 ), "config" ) && !strcmp( where_arg( arguments, 1 ), "inter" ) )
        {
            emu->enabl_interup = !emu->enabl_interup;
            printf( "      Habilitada la parada del emulador al producirse un error cualquiera.\n" );
            return 1;
        }
    }
    return 0;
}

void stf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    Size_dir size = byte;
    int TY = extu_int( mc->micro.ir.deci, 26, 2 );
    if (!TY) size = double_word;
    else if ( TY == 1 ) size = word;
    else if ( TY == 2 ) size = half_word;
    reg_inm = create_reg32i(ext_int( mc->micro.ir.deci, 0, 16 ));  //TODO Replantearse esto...
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    reg64 data;
    if ( size == double_word )
    {
        data = create_reg64freg32reg32( regp_dest[1], *regp_dest );
    }
    else
    {
        data = create_reg64freg32(*regp_dest);
    }
    set_mem( size, mc, create_reg32i( regp_org->deci + reg_inm.deci ), data );
}

void stf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    Size_dir size = byte;
    int TY = extu_int( mc->micro.ir.deci, 10, 2 );
    //int T = extu_int( mc->micro.ir.deci, 9, 1 );  //ignore
    if (!TY) size = double_word;
    else if ( TY == 1 ) size = word;
    else if ( TY == 2 ) size = half_word;
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    reg64 data;
    if ( size == double_word )
    {
        data = create_reg64freg32reg32( regp_dest[1], *regp_dest );
    }
    else
    {
        data = create_reg64freg32(*regp_dest);
    }
    set_mem( size, mc, create_reg32i( regp_org->deci + reg_inm.deci ), data );
}

void ldf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    Size_dir size = byte;
    int TY = extu_int( mc->micro.ir.deci, 26, 2 );
    if (!TY) size = double_word;
    else if ( TY == 1 ) size = word;
    else if ( TY == 2 ) size = half_word;
    reg_inm = create_reg32i(ext_int( mc->micro.ir.deci, 0, 16 ));  //TODO Replantearse esto...
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    get_mem( size, mc, create_reg32i( regp_org->deci + reg_inm.deci ), reg_dest );
    if ( size == byte ) extsign_reg32( sizedir_bits(byte), regp_dest );
    else if ( size == half_word ) extsign_reg32( sizedir_bits(half_word), regp_dest );
    if ( mc->gen_regs[0].deci ) mc->gen_regs[0] = create_reg32i(0);
}

void ldf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    Size_dir size = byte;
    int B = getbit_reg32( &(mc->micro.ir), 26 );
    if (!B) size = half_word;
    reg_inm = create_reg32i(ext_int( mc->micro.ir.deci, 0, 16 ));  //TODO Replantearse esto...
    extsign_reg32( sizedir_bits(half_word), &reg_inm );
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    get_mem( size, mc, create_reg32i( regp_org->deci + reg_inm.deci ), reg_dest );
    *regp_dest = create_reg32i(extu_int( regp_dest->deci, 0, sizedir_bits(size) ));
    if ( mc->gen_regs[0].deci ) mc->gen_regs[0] = create_reg32i(0);
}

void ldf3 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    Size_dir size = byte;
    int TY = extu_int( mc->micro.ir.deci, 10, 2 );
    if (!TY) size = double_word;
    else if ( TY == 1 ) size = word;
    else if ( TY == 2 ) size = half_word;
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    get_mem( size, mc, create_reg32i( regp_org->deci + reg_inm.deci ), reg_dest );
    if ( size == byte ) extsign_reg32( sizedir_bits(byte), regp_dest );
    else if ( size == half_word ) extsign_reg32( sizedir_bits(half_word), regp_dest );
    if ( mc->gen_regs[0].deci ) mc->gen_regs[0] = create_reg32i(0);
}

void ldf4 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    Size_dir size = byte;
    int B = getbit_reg32( &(mc->micro.ir), 10 );
    if (!B) size = half_word;
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    get_mem( size, mc, create_reg32i( regp_org->deci + reg_inm.deci ), reg_dest );
    *regp_dest = create_reg32i(extu_int( regp_dest->deci, 0, sizedir_bits(size) ));
    if ( mc->gen_regs[0].deci ) mc->gen_regs[0] = create_reg32i(0);
}

void andf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    if ( reg_dest == r0 ) return;
    int result = 0;
    reg_inm = create_reg32i(extu_int( mc->micro.ir.deci, 0, 16 ));  //TODO Replantearse esto...
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    if (getbit_reg32( &mc->micro.ir, 26 )) // and.u
    {
        reg_inm.deci <<= 16;
        result = regp_org->deci & reg_inm.deci;
        result &= 0xffff0000;
        result += regp_org->deci & 0x0000ffff;
    }
    else
    {
        result = regp_org->deci & reg_inm.deci;
        result &= 0x0000ffff;
        result += regp_org->deci & 0xffff0000;
    }
    *regp_dest = create_reg32i(result);
}

void andf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    if ( reg_dest == r0 ) return;
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    if (getbit_reg32( &mc->micro.ir, 10 )) // and.c
        *regp_dest = create_reg32i( regp_org->deci & ~reg_inm.deci );
    else
        *regp_dest = create_reg32i( regp_org->deci & reg_inm.deci );
}

void xorf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    if ( reg_dest == r0 ) return;
    int result = 0;
    reg_inm = create_reg32i(extu_int( mc->micro.ir.deci, 0, 16 ));  //TODO Replantearse esto...
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    if (getbit_reg32( &mc->micro.ir, 26 )) // xor.u
    {
        reg_inm.deci <<= 16;
        result = regp_org->deci ^ reg_inm.deci;
        result &= 0xffff0000;
        result += regp_org->deci & 0x0000ffff;
    }
    else
    {
        result = regp_org->deci ^ reg_inm.deci;
        result &= 0x0000ffff;
        result += regp_org->deci & 0xffff0000;
    }
    *regp_dest = create_reg32i(result);
}

void xorf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    if ( reg_dest == r0 ) return;
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    if (getbit_reg32( &mc->micro.ir, 10 )) // xor.c
        *regp_dest = create_reg32i( regp_org->deci ^ ~reg_inm.deci );
    else
        *regp_dest = create_reg32i( regp_org->deci ^ reg_inm.deci );
}

void maskf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    andf1( reg_dest, reg_org, reg_inm, mc );
}

void orf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    if ( reg_dest == r0 ) return;
    int result = 0;
    reg_inm = create_reg32i(extu_int( mc->micro.ir.deci, 0, 16 ));  //TODO Replantearse esto...
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    if (getbit_reg32( &mc->micro.ir, 26 )) // or.u
    {
        reg_inm.deci <<= 16;
        result = regp_org->deci | reg_inm.deci;
        result &= 0xffff0000;
        result += regp_org->deci & 0x0000ffff;
    }
    else
    {
        result = regp_org->deci | reg_inm.deci;
        result &= 0x0000ffff;
        result += regp_org->deci & 0xffff0000;
    }
    *regp_dest = create_reg32i(result);
}

void orf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    if ( reg_dest == r0 ) return;  //En las 'add' y 'sub' no se debería usar porque si puede afectar al SR.
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    if (getbit_reg32( &mc->micro.ir, 10 )) // or.c
        *regp_dest = create_reg32i( regp_org->deci | ~reg_inm.deci );
    else
        *regp_dest = create_reg32i( regp_org->deci | reg_inm.deci );
}

void addf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    //TODO revisar...
    //if ( reg_dest == r0 ) return;
    reg_inm = create_reg32i(ext_int( mc->micro.ir.deci, 0, 16 ));  //TODO Replantearse esto...
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    if ( reg_dest != r0 )
        *regp_dest = create_reg32i( regp_org->deci + reg_inm.deci );
}

void addf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )  //TODO
{
    //Revisar en mc->micro.ir, si es add.ci, add.co, addcio
    //if ( reg_dest == r0 ) return;
    int result = 0, c;
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    if (getbit_reg32( &mc->micro.ir, 9 ))           //add.ci
    {
        reg_inm.deci += getbit_reg32( &mc->micro.sr, 28 );
    }
    if ((c = add_ovf( &result, regp_org->deci, reg_inm.deci )))
    {
        //Do something, OVF
        setbit_reg32( &mc->micro.sr, 12 );      //Bit de OVF(FV)
        //return;
    }
    else
    {
        clearbit_reg32( &mc->micro.sr, 12 );    //Bit de OVF(FV)
    }
    if ( getbit_reg32( &mc->micro.ir, 8 ) && c )    //add.co
    {
        setbit_reg32( &mc->micro.sr, 28 );
    }
    if ( reg_dest != r0 )
        *regp_dest = create_reg32i(result);
}

void adduf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    //TODO revisar...
    //if ( reg_dest == r0 ) return;
    reg_inm = create_reg32i(extu_int( mc->micro.ir.deci, 0, 16 ));  //TODO Replantearse esto...
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    if ( reg_dest != r0 )
        *regp_dest = create_reg32i( (unsigned)regp_org->deci + (unsigned)reg_inm.deci );
}

//Pensar si hay diferencia con 'add', los 'XXXu'. Los ultimos hacen que no se produzca OVF
void adduf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )  //TODO
{
    //TODO revisar, fuertemente pero no es tan necesario...
    //Revisar en mc->micro.ir, si es add.ci, add.co, add.cio
    //if ( reg_dest == r0 ) return;
    unsigned int result = 0, c;
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    if (getbit_reg32( &mc->micro.ir, 9 ))           //add.ci
    {
        reg_inm.deci += getbit_reg32( &mc->micro.sr, 28 );
    }
    c = addu_ovf( &result, regp_org->deci, reg_inm.deci );
    if ( getbit_reg32( &mc->micro.ir, 8 ) && c )           //add.co
    {
        setbit_reg32( &mc->micro.sr, 28 );
    }
    if ( reg_dest != r0 )
        *regp_dest = create_reg32i(result);
}

void subf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    //TODO revisar...
    //if ( reg_dest == r0 ) return;
    reg_inm = create_reg32i(ext_int( mc->micro.ir.deci, 0, 16 ));  //TODO Replantearse esto...
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    if ( reg_dest != r0 )
        *regp_dest = create_reg32i( regp_org->deci - reg_inm.deci );
}

void subf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )  //TODO
{
    //Revisar en mc->micro.ir, si es add.ci, add.co, addcio
    //if ( reg_dest == r0 ) return;
    int result = 0, c;
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    if (getbit_reg32( &mc->micro.ir, 9 ))           //add.ci    //TODO: REVISAR
    {
        reg_inm.deci += getbit_reg32( &mc->micro.sr, 28 );
    }
    if ((c = sub_ovf( &result, regp_org->deci, reg_inm.deci )))
    {
        //Do something, OVF
        setbit_reg32( &mc->micro.sr, 12 );      //Bit de OVF(FV)
        //return;  // Es 'muls' quien no permite escribir el resultado
    }
    else
    {
        clearbit_reg32( &mc->micro.sr, 12 );    //Bit de OVF(FV)
    }
    if ( getbit_reg32( &mc->micro.ir, 8 ) && c )           //add.co
    {
        setbit_reg32( &mc->micro.sr, 28 );
    }
    if ( reg_dest != r0 )
        *regp_dest = create_reg32i(result);
}

void subuf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    //TODO revisar...
    //if ( reg_dest == r0 ) return;
    reg_inm = create_reg32i(extu_int( mc->micro.ir.deci, 0, 16 ));  //TODO Replantearse esto...
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    if ( reg_dest != r0 )
        *regp_dest = create_reg32i( (unsigned)regp_org->deci - (unsigned)reg_inm.deci );
}

//Pensar si hay diferencia con 'add', los 'XXXu' hacen que no se produzca OVF
void subuf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )  //TODO
{
    //TODO revisar, fuertemente pero no es tan necesario...
    //Revisar en mc->micro.ir, si es add.ci, add.co, add.cio
    //if ( reg_dest == r0 ) return; //Lo de abajo es una mejor aproximacion
    unsigned int result = 0, c;
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    if (getbit_reg32( &mc->micro.ir, 9 ))           //add.ci
    {
        reg_inm.deci += getbit_reg32( &mc->micro.sr, 28 );
    }
    c = subu_ovf( &result, regp_org->deci, reg_inm.deci );
    if ( getbit_reg32( &mc->micro.ir, 8 ) && c )           //add.co
    {
        setbit_reg32( &mc->micro.sr, 28 );
    }
    if ( reg_dest != r0 )
        *regp_dest = create_reg32i(result);
}

//Aux
unsigned int cmp_gen ( int a, int b )
{
    unsigned int result = 0;
    unsigned a_byte0 = extu_int( a, 0, 8 );
    unsigned b_byte0 = extu_int( b, 0, 8 );
    unsigned a_byte1 = extu_int( a, 8, 8 );
    unsigned b_byte1 = extu_int( b, 8, 8 );
    unsigned a_byte2 = extu_int( a, 16, 8 );
    unsigned b_byte2 = extu_int( b, 16, 8 );
    unsigned a_byte3 = extu_int( a, 24, 8 );
    unsigned b_byte3 = extu_int( b, 24, 8 );
    unsigned a_hw0 = extu_int( a, 0, 16 );
    unsigned b_hw0 = extu_int( b, 0, 16 );
    unsigned a_hw1 = extu_int( a, 16, 16 );
    unsigned b_hw1 = extu_int( b, 16, 16 );
    if ( a == b ) result |= 0x4;
    else result |= 0x8;
    if ( a > b ) result |= 0x10;
    else result |= 0x20;
    if ( a < b ) result |= 0x40;
    else result |= 0x80;
    if ( (unsigned)a > (unsigned)b ) result |= 0x100;
    else result |= 0x200;
    if ( (unsigned)a < (unsigned)b ) result |= 0x400;
    else result |= 0x800;
    if ( a_byte0 == b_byte0 || a_byte0 == b_byte1 || a_byte0 == b_byte2 || a_byte0 == b_byte3
        || a_byte1 == b_byte0 || a_byte1 == b_byte1 || a_byte1 == b_byte2 || a_byte1 == b_byte3
        || a_byte2 == b_byte0 || a_byte2 == b_byte1 || a_byte2 == b_byte2 || a_byte2 == b_byte3
        || a_byte3 == b_byte0 || a_byte3 == b_byte1 || a_byte3 == b_byte2 || a_byte3 == b_byte3 )
        result |= 0x1000;
    else result |= 0x2000;
    if ( a_hw0 == b_hw0 || a_hw0 == b_hw1 || a_hw1 == b_hw0 || a_hw1 == b_hw1  ) result |= 0x4000;
    else result |= 0x8000;
    return result;
}

void cmpf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    if ( reg_dest == r0 ) return;
    reg_inm = create_reg32i(ext_int( mc->micro.ir.deci, 0, 16 ));  //TODO Replantearse esto...
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    *regp_dest = create_reg32i( cmp_gen( regp_org->deci, reg_inm.deci ) );
}

void cmpf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    if ( reg_dest == r0 ) return;
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    *regp_dest = create_reg32i( cmp_gen( regp_org->deci, reg_inm.deci ) );
}

//Revisar el tema del OVF
//Y reformarlo para que este acorde al emulador original
/*
 7.5.2.5 INTEGER OVERFLOW EXCEPTION (VECTOR OFFSET $48).
   The integer overflow exception occurs when the result of a signed integer arithmetic
    instruction cannot be represented as a 32-bit signed number. The EXIP points to the
    instruction that caused the exception. The destination register and carry bit are
    unchanged by an instruction that causes an integer overflow exception.
 * */
void mulsf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    int result = 0, c;
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    if ((c = muls_ovf( &result, regp_org->deci, reg_inm.deci )))
    {
        //Do something, OVF
        setbit_reg32( &mc->micro.sr, 12 );      //Bit de OVF(FV)
        return;
    }
    else
    {
        clearbit_reg32( &mc->micro.sr, 12 );    //Bit de OVF(FV)
    }
    if ( reg_dest != r0 )
        *regp_dest = create_reg32i(result);
}

void muluf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    if ( reg_dest == r0 ) return;
    reg_inm = create_reg32i(extu_int( mc->micro.ir.deci, 0, 16 ));  //TODO Replantearse esto...
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    *regp_dest = create_reg32i( (unsigned)regp_org->deci * (unsigned)reg_inm.deci );
}

// m = a*0x10000 + b, n = c*0x10000 + d; m * n = a*c*0x100000000 + c*b*0x10000 + a*d*0x10000 + b*d
// reg32_0 = a*c + c*b/0x10000 + a*d/0x10000
// reg32_1 = b*d + c*b << 16 + a*d << 16
void muluf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    if (getbit_reg32( &mc->micro.ir, 8 )) // Destino de double palabra
    {
        unsigned int a = extu_int( regp_dest->deci, 16, 16 );
        unsigned int b = extu_int( regp_dest->deci, 0, 16 );
        unsigned int c = extu_int( reg_inm.deci, 16, 16 );
        unsigned int d = extu_int( reg_inm.deci, 0, 16 );
        int result = a*c + c*b/0x10000 + a*d/0x10000;
        mc->rg32aux = create_reg32i(result);
        result = b*d + ((c*b) << 16) + ((a*d) << 16);
        *(regp_dest++) = mc->rg32aux;
        *regp_dest = create_reg32i(result);
    }
    else
    {
        *regp_dest = create_reg32i( (unsigned)regp_org->deci * (unsigned)reg_inm.deci );
    }
    // El destino puede ser 'double_word'
    if ( mc->gen_regs[0].deci ) mc->gen_regs[0] = create_reg32i(0);
}

void divsf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    reg_inm = create_reg32i(ext_int( mc->micro.ir.deci, 0, 16 ));  //TODO Replantearse esto...
    divsf2( reg_dest, reg_org, reg_inm, mc );
}

void divsf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    int result = 0, c;
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    if ((c = divs_ovf( &result, regp_org->deci, reg_inm.deci )))
    {
        //Do something, OVF
        setbit_reg32( &mc->micro.sr, 12 );      //Bit de OVF(FV)
        return;
    }
    else
    {
        clearbit_reg32( &mc->micro.sr, 12 );    //Bit de OVF(FV)
    }
    if ( reg_dest != r0 )
        *regp_dest = create_reg32i(result);
}

void divuf1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    if ( reg_dest == r0 ) return;
    reg_inm = create_reg32i(extu_int( mc->micro.ir.deci, 0, 16 ));  //TODO Replantearse esto...
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    if ( reg_inm.deci != 0 )
        *regp_dest = create_reg32i( (unsigned)regp_org->deci / reg_inm.deci );
}

void divuf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    if ( reg_inm.deci == 0 ) return;
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    if (getbit_reg32( &mc->micro.ir, 8 )) // Destino de double palabra
    {
        unsigned int res_fdiv = 0x10000000 / reg_inm.deci * 0x10;
        *(regp_dest++) = create_reg32i( (unsigned)(*(regp_org++)).deci / reg_inm.deci );
        *regp_dest = create_reg32i( (unsigned)regp_org->deci / reg_inm.deci + res_fdiv );
    }
    else
    {
        *regp_dest = create_reg32i( (unsigned)regp_org->deci * (unsigned)reg_inm.deci );
    }
    // El destino puede ser 'double_word'
    if ( mc->gen_regs[0].deci ) mc->gen_regs[0] = create_reg32i(0);
}

void clrf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    if ( reg_dest == r0 ) return;
    int o5 = extu_int( reg_inm.deci, 0, 5 );
    int w5 = extu_int( reg_inm.deci, 5, 5 );
    if (!w5) w5 = 32;
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    *regp_dest = create_reg32i( regp_org->deci & ~createfmt( o5, w5 ) );
}

void clrf3 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    if ( reg_dest == r0 ) return;
    int o5 = extu_int( mc->micro.ir.deci, 0, 5 );
    int w5 = extu_int( mc->micro.ir.deci, 5, 5 );
    if (!w5) w5 = 32;
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    *regp_dest = create_reg32i( regp_org->deci & ~createfmt( o5, w5 ) );
}

void setf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    if ( reg_dest == r0 ) return;
    int o5 = extu_int( reg_inm.deci, 0, 5 );
    int w5 = extu_int( reg_inm.deci, 5, 5 );
    if (!w5) w5 = 32;
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    *regp_dest = create_reg32i( regp_org->deci | createfmt( o5, w5 ) );
}

void setf3 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    if ( reg_dest == r0 ) return;
    int o5 = extu_int( mc->micro.ir.deci, 0, 5 );
    int w5 = extu_int( mc->micro.ir.deci, 5, 5 );
    if (!w5) w5 = 32;
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    *regp_dest = create_reg32i( regp_org->deci | createfmt( o5, w5 ) );
}

void extf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    if ( reg_dest == r0 ) return;
    int o5 = extu_int( reg_inm.deci, 0, 5 );
    int w5 = extu_int( reg_inm.deci, 5, 5 );
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    *regp_dest = create_reg32i(ext_int( regp_org->deci, o5, w5 ));
}

void extf3 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    if ( reg_dest == r0 ) return;
    int o5 = extu_int( mc->micro.ir.deci, 0, 5 );
    int w5 = extu_int( mc->micro.ir.deci, 5, 5 );
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    *regp_dest = create_reg32i(ext_int( regp_org->deci, o5, w5 ));
}

void extuf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    if ( reg_dest == r0 ) return;
    int o5 = extu_int( reg_inm.deci, 0, 5 );
    int w5 = extu_int( reg_inm.deci, 5, 5 );
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    *regp_dest = create_reg32i(extu_int( regp_org->deci, o5, w5 ));
}

void extuf3 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    if ( reg_dest == r0 ) return;
    int o5 = extu_int( mc->micro.ir.deci, 0, 5 );
    int w5 = extu_int( mc->micro.ir.deci, 5, 5 );
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    *regp_dest = create_reg32i(extu_int( regp_org->deci, o5, w5 ));
}

void makf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    //Si w5 == 0? Solo se desplaza a la izquierda, porque coge el registro entero
    if ( reg_dest == r0 ) return;
    int o5 = extu_int( reg_inm.deci, 0, 5 );
    int w5 = extu_int( reg_inm.deci, 5, 5 );
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    *regp_dest = create_reg32i( extu_int( regp_org->deci, 0, w5 ) << o5 );
}

void makf3 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    //Si w5 == 0? Solo se desplaza a la izquierda, porque coge el registro entero
    if ( reg_dest == r0 ) return;
    int o5 = extu_int( mc->micro.ir.deci, 0, 5 );
    int w5 = extu_int( mc->micro.ir.deci, 5, 5 );
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    *regp_dest = create_reg32i( extu_int( regp_org->deci, 0, w5 ) << o5 );
}

void rotf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    //Si w5 == 0? Solo se desplaza a la izquierda, porque coge el registro entero
    if ( reg_dest == r0 ) return;
    int o5 = extu_int( reg_inm.deci, 0, 5 );
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    *regp_dest = create_reg32i(rot_right( regp_org->deci, o5 ));
}

void rotf3 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    //Si w5 == 0? Solo se desplaza a la izquierda, porque coge el registro entero
    if ( reg_dest == r0 ) return;
    int o5 = extu_int( mc->micro.ir.deci, 0, 5 );
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    *regp_dest = create_reg32i(rot_right( regp_org->deci, o5 ));
}

//TODO
void faddf4 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    reg32* regp_org2 = regt2regp( (Reg_type)extu_int( mc->micro.ir.deci, 0, 5 ), mc );
    int TD = extu_int( mc->micro.ir.deci, 5, 2 );
    int T1 = extu_int( mc->micro.ir.deci, 9, 2 );
    int T2 = extu_int( mc->micro.ir.deci, 7, 2 );
    float fS1 = chg2float(regp_org->deci);
    float fS2 = chg2float(reg_inm.deci);
    double dS1 = chg2double( regp_org->deci, regp_org[1].deci );
    double dS2 = chg2double( regp_org2->deci, regp_org2[1].deci );
    printf("No implementado del todo.\n");
    if ( !TD && !T1 && !T2 )
    {
        //Hacer funcion para detectar 'OVF', 'UNF' y 'operando reservado'
        *regp_dest = create_reg32i(chg2int( fS1 + fS2 ));
    }
    else if ( !TD && !T1 && T2 )
    {
    }
    else if ( !TD && T1 && !T2 )
    {
    }
    else if ( !TD && T1 && T2 )
    {
    }
    else if ( TD && !T1 && !T2 )
    {
    }
    else if ( TD && !T1 && T2 )
    {
    }
    else if ( TD && T1 && !T2 )
    {
    }
    else
    {
    }
}

void fcmpf4 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    printf("No implementado..\n");
    mc->before_state = mc->state;
    mc->state = halt;
}
void fcvtf4 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    printf("No implementado..\n");
    mc->before_state = mc->state;
    mc->state = halt;
}
void fdivf4 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    printf("No implementado..\n");
    mc->before_state = mc->state;
    mc->state = halt;
}
void fltf4 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    printf("No implementado..\n");
    mc->before_state = mc->state;
    mc->state = halt;
}
void fmulf4 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    printf("No implementado..\n");
    mc->before_state = mc->state;
    mc->state = halt;
}
void fsubf4 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    printf("No implementado..\n");
    mc->before_state = mc->state;
    mc->state = halt;
}
void intf4 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    printf("No implementado..\n");
    mc->before_state = mc->state;
    mc->state = halt;
}

void bb0f1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    int B5 = extu_int( mc->micro.ir.deci, 21, 5 );
    int D16 = ext_int( mc->micro.ir.deci, 0, 16 );
    reg32* regp_org = regt2regp( reg_org, mc );
    if (testbit_reg32( regp_org, B5 ))
    {
        //Aunque debería ser antes, nada dice que no se modifique el pc y luego ejecutarla para saltar...
        //Quien sabe si es así pero el sentido del '.n' podría ser para hacer los 'switch'
        //Lo anterior no tiene sentido...
        reg32 pc_next = create_reg32i( mc->micro.pc.deci + 4 );
        mc->micro.pc = create_reg32i( mc->micro.pc.deci + 4*D16 );
        if (getbit_reg32( &mc->micro.ir, 26 )) // bb0.n
        {
            //Mejorar la implementacion
            execute_inst_woir( mc, pc_next );
        }
        mc->mod_pc = 1;
    }
}

void bb1f1 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    int B5 = extu_int( mc->micro.ir.deci, 21, 5 );
    int D16 = ext_int( mc->micro.ir.deci, 0, 16 );
    reg32* regp_org = regt2regp( reg_org, mc );
    if (getbit_reg32( regp_org, B5 ))
    {
        //Aunque debería ser antes, nada dice que no se modifique el pc y luego ejecutarla para saltar...
        //Quien sabe si es así pero el sentido del '.n' podría ser para hacer los 'switch'
        //Lo anterior no tiene sentido...
        reg32 pc_next = create_reg32i( mc->micro.pc.deci + 4 );
        mc->micro.pc = create_reg32i( mc->micro.pc.deci + 4*D16 );
        if (getbit_reg32( &mc->micro.ir, 26 )) // bb0.n
        {
            //Mejorar la implementacion
            execute_inst_woir( mc, pc_next );
        }
        mc->mod_pc = 1;
    }
}

void brf5 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    int D26 = ext_int( mc->micro.ir.deci, 0, 26 );
    reg32 pc_next = create_reg32i( mc->micro.pc.deci + 4 );
    mc->micro.pc = create_reg32i( mc->micro.pc.deci + 4*D26 );
    if (getbit_reg32( &mc->micro.ir, 26 )) // br.n
    {
        //printf("Not implemented or bad implementation...\n");
        //Mejorar la implementacion
        execute_inst_woir( mc, pc_next );
    }
    mc->mod_pc = 1;
}

void bsrf5 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    int D26 = ext_int( mc->micro.ir.deci, 0, 26 );
    int n = getbit_reg32( &mc->micro.ir, 26 );
    mc->gen_regs[1] = n? create_reg32i( mc->micro.pc.deci + 8 ) : create_reg32i( mc->micro.pc.deci + 4 );
    reg32 pc_next = create_reg32i( mc->micro.pc.deci + 4 );
    mc->micro.pc = create_reg32i( mc->micro.pc.deci + 4*D26 );
    if (n) // bsr.n
    {
        //printf("Not implemented or bad implementation...\n");
        //Mejorar la implementacion
        execute_inst_woir( mc, pc_next );
    }
    mc->mod_pc = 1;
}

void jmpf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    int n = getbit_reg32( &mc->micro.ir, 10 );
    reg32 pc_next = create_reg32i( mc->micro.pc.deci + 4 );
    mc->micro.pc = create_reg32i( reg_inm.deci & ~3 );
    if (n) // jmp.n
    {
        //printf("Not implemented or bad implementation...\n");
        //Mejorar la implementacion
        execute_inst_woir( mc, pc_next );
    }
    mc->mod_pc = 1;
}

void jsrf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    int n = getbit_reg32( &mc->micro.ir, 10 );
    mc->gen_regs[1] = n? create_reg32i( mc->micro.pc.deci + 8 ) : create_reg32i( mc->micro.pc.deci + 4 );
    reg32 pc_next = create_reg32i( mc->micro.pc.deci + 4 );
    mc->micro.pc = create_reg32i( reg_inm.deci & ~3 );
    if (n) // jsr.n
    {
        //printf("Not implemented or bad implementation...\n");
        //Mejorar la implementacion
        execute_inst_woir( mc, pc_next );
    }
    mc->mod_pc = 1;
}

void ldcrf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    *regt2regp( reg_dest, mc ) = mc->micro.sr;
}

void stcrf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    mc->micro.sr = *regt2regp( reg_org, mc );
}

void xmemf2 ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    //Funcion atomica
    reg32* regp_dest = regt2regp( reg_dest, mc );
    reg32* regp_org = regt2regp( reg_org, mc );
    reg32 dir = create_reg32i( (unsigned)regp_org->deci + reg_inm.deci );
    mc->rg32aux = *regp_dest;
    get_mem( word, mc, dir, reg_dest );
    if ( mc->state == bus_err || mc->state == add_err ) return;
    //Se puede usar 'write_mem' ya que 'get_mem' habrá dado por valido la direccion
    //Al usar 'write_mem' o 'set_mem' hay que darle la vuelta al dato por como está
    //    todo implementado y por como se lee directamente del '.bin'
    //    ya que el 'show_mem' debe hacer más de los mismo.
    write_mem( mc, dir, create_reg64freg32(reverse_reg32(mc->rg32aux)), 4 );
    if ( mc->gen_regs[0].deci ) mc->gen_regs[0] = create_reg32i(0);
}

void stop ( Reg_type reg_dest, Reg_type reg_org, reg32 reg_inm, mc88110* mc )
{
    mc->before_state = mc->state;
    mc->state = halt;
}

//Deprecate???
void conf_screen ( )
{
    char str[5] = "";
    do {
        clear_screen( );
        printf( "****************************************************************************\n" );
        printf( "* PC=XXXXXX|      Instruccion...          |Tot. Inst: XXXXXX Ciclo: XXXXXX *\n" );
        printf( "* FL=X FE=X FC=X FV=X FR=X                                                 *\n" );
        printf( "* R01 = XXXXXXXX h  R02 = XXXXXXXX h  R03 = XXXXXXXX h  R04 = XXXXXXXX h   *\n" );
        printf( "* R05 = XXXXXXXX h  R06 = XXXXXXXX h  R07 = XXXXXXXX h  R08 = XXXXXXXX h   *\n" );
        printf( "* R09 = XXXXXXXX h  R10 = XXXXXXXX h  R11 = XXXXXXXX h  R12 = XXXXXXXX h   *\n" );
        printf( "* R13 = XXXXXXXX h  R12 = XXXXXXXX h  R03 = XXXXXXXX h  R14 = XXXXXXXX h   *\n" );
        printf( "* R17 = XXXXXXXX h  R12 = XXXXXXXX h  R03 = XXXXXXXX h  R20 = XXXXXXXX h   *\n" );
        printf( "* R21 = XXXXXXXX h  R22 = XXXXXXXX h  R23 = XXXXXXXX h  R24 = XXXXXXXX h   *\n" );
        printf( "* R25 = XXXXXXXX h  R22 = XXXXXXXX h  R23 = XXXXXXXX h  R24 = XXXXXXXX h   *\n" );
        printf( "* R29 = XXXXXXXX h  R30 = XXXXXXXX h  R31 = XXXXXXXX h                     *\n" );
        printf( "*--------------------------------------------------------------------------*\n" );
        printf( "*88110>                                                                    *\n" );
        printf( "*                                                                          *\n" );
        printf( "*                                                                          *\n" );
        printf( "*                                                                          *\n" );
        printf( "*               Configuracion de la pantalla                               *\n" );
        printf( "*                        <ENTER> Para confirmar                            *\n" );
        printf( "*                 <SPACE><ENTER> Para volver a escribir la pantalla        *\n" );
        printf( "*                                                                          *\n" );
        printf( "*                                                                          *\n" );
        printf( "*                                                                          *\n" );
        printf( "*                                                                          *\n" );
        printf( "*                                                                          *\n" );
        printf( "*                                                                          *\n" );
        printf( "****************************************************************************\n" );
        fgets( str, 5, stdin );
#if DEBUG
        printf( "Linea obtenida: '%s'\n", str );
#endif
    } while ( strcmp( str, "\n" ) );
}

#define SKIP 0
#define CHAR 1
#define STR 2
#define N_DECI 3
#define N_HEX 4
#define NO_FMT 5
#define N_FLOAT 6
#define MATH_SYBM 7
#define SYMBOL 8
#define SYMBOLS 9
//Añadir configuraciones, que distinga sin delimiador o con uno, que distinga con menor precision o mayor, 
//que sea mas permisivo o menos
args create_args ( char* cmd, int size )    //size = 1024
{
    args result = { 0, NULL, NULL, strdup(cmd) };
    int i = 0;
    int state = SKIP;
    int count = 0;
    while( i < size && cmd[i] == ' ' ) i++;
    while( i < size && cmd[i] )
    {
        //if ( state != SKIP && (cmd[i] == ' ' || cmd[i + 1] == '\0') )
        if ( state != SKIP && cmd[i] == ' ' )//Revisar por el tema de \n, aqui se podría meter un delimitador
        {
            if (cmd[i] == ' ') result.cmdptr[i] = '\0';
            result.count++;
            result.dtype = (Data_type*)realloc( result.dtype, sizeof(Data_type) * result.count );
            switch(state)
            {
                case CHAR: result.dtype[result.count - 1] = chr; break;
                case STR: result.dtype[result.count - 1] = str; break;
                case N_DECI: result.dtype[result.count - 1] = n_deci; break;
                case N_FLOAT: result.dtype[result.count - 1] = n_float; break;
                case N_HEX: result.dtype[result.count - 1] = n_hex; break;
                case MATH_SYBM: result.dtype[result.count - 1] = math_symb; break;
                case SYMBOL: result.dtype[result.count - 1] = symbol; break;
                case SYMBOLS: result.dtype[result.count - 1] = symbols; break;
                case NO_FMT: result.dtype[result.count - 1] = no_valid; break;
                default:
                    perror("[create_args] No deberías acceder aquí.");
            }
            result.where_data = (int*)realloc( result.where_data, sizeof(int) * result.count );
            result.where_data[result.count - 1] = i - count;
            state = SKIP;
            count = 0;
        }
        if ( state == STR && isgraph(cmd[i]) ) count++;//Replantearse si fuese necesario solo 'state == STR'
        if ( state == N_DECI && isdigit(cmd[i]) ) count++;
        if ( state == N_FLOAT && isdigit(cmd[i]) ) count++;
        if ( state == N_HEX && isxdigit(cmd[i]) ) count++;
        if ( state == SYMBOLS && !isalnum(cmd[i]) && !iscntrl(cmd[i]) && cmd[i] != ' ' && cmd[i] != 0x7f )
            count++;
        if ( state == NO_FMT ) count++;
        if ( state == N_HEX && !isxdigit(cmd[i]) )
        {
            state = STR;
            count++;
        }
        if ( state == N_DECI && tolower(cmd[i]) == 'x' )
        {
            count++;
            state = N_HEX;
            if ( cmd[i - 1] != '0' ) state = NO_FMT;
            //if ( cmd[i - 2] == '-' ) state = NO_FMT;  //A esto me refiero con ser permisivo o no...
        }
        if ( state == N_FLOAT && (cmd[i] == ',' || cmd[i] == '.') )
        {
            state = NO_FMT;
            count++;
        }
        if ( state == N_DECI && (cmd[i] == ',' || cmd[i] == '.') )
        {
            state = N_FLOAT;
            count++;
        }
        if ( state == N_DECI && isalpha(cmd[i]) )
        {
            state = STR;
            count++;
        }
        if ( state == CHAR && isalnum(cmd[i]) )
        {
            state = STR;
            count++;
        }
        if ( state == MATH_SYBM && isdigit(cmd[i]) )
        {
            if ( cmd[i - 1] == '-' || cmd[i - 1] == '+' )
                state = N_DECI;
            else
                state = NO_FMT;
            count++;
        }
        if ( state == SKIP && isalpha(cmd[i]) )
        {
            state = CHAR;
            count++;
        }
        if ( state == SKIP && isdigit(cmd[i]) )
        {
            state = N_DECI;
            count++;
        }
        if ( (state == MATH_SYBM || state == SYMBOL) &&
             !isalnum(cmd[i]) && !iscntrl(cmd[i]) && cmd[i] != ' ' && cmd[i] != 0x7f )
        {
            state = SYMBOLS;
            count++;
        }
        if ( state == SKIP
             && (cmd[i] == '+' || cmd[i] == '*' || cmd[i] == '/'
             || cmd[i] == '%' || cmd[i] == '-' || cmd[i] == '=' || cmd[i] == '^' ) )
        {
            state = MATH_SYBM;
            count++;
        }
        if ( state == SKIP && !isalnum(cmd[i]) && !iscntrl(cmd[i]) && cmd[i] != ' ' && cmd[i] != 0x7f )
        {
            state = SYMBOL;
            count++;
        }
        i++;
    }
    if ( state != SKIP )
    {
        result.count++;
        result.dtype = (Data_type*)realloc( result.dtype, sizeof(Data_type) * result.count );
        switch(state)
        {
            case CHAR: result.dtype[result.count - 1] = chr; break;
            case STR: result.dtype[result.count - 1] = str; break;
            case N_DECI: result.dtype[result.count - 1] = n_deci; break;
            case N_FLOAT: result.dtype[result.count - 1] = n_float; break;
            case N_HEX: result.dtype[result.count - 1] = n_hex; break;
            case MATH_SYBM: result.dtype[result.count - 1] = math_symb; break;
            case SYMBOL: result.dtype[result.count - 1] = symbol; break;
            case SYMBOLS: result.dtype[result.count - 1] = symbols; break;
            case NO_FMT: result.dtype[result.count - 1] = no_valid; break;
            default:
                perror("[create_args] No deberías acceder aquí.");
        }
        result.where_data = (int*)realloc( result.where_data, sizeof(int) * result.count );
        result.where_data[result.count - 1] = i - count;
        state = SKIP;
        count = 0;
    }
    return result;
}

char* where_arg ( args* arguments, int i )
{
    return arguments->cmdptr + arguments->where_data[i];
}

int is_dt ( args* arguments, int i, Data_type dtype )
{
    //return arguments->dtype[i] == dtype;
    if ( arguments->dtype[i] == t_num )
        return dtype == n_deci || dtype == n_hex || dtype == n_float;
    else
        return arguments->dtype[i] == dtype;
}

int is_dts ( args* arguments, int size, Data_type dtypes[] )
{
    int result = arguments->count == size;
    int i = 0;
    while( result && i < size )
    {
        if ( arguments->dtype[i] == t_num )
            result = dtypes[i] == n_deci || dtypes[i] == n_hex || dtypes[i] == n_float;
        else
            result = arguments->dtype[i] == dtypes[i];
        i++;
    }
    return result;
}

void free_args ( args* arguments )
{
    free(arguments->where_data);
    arguments->where_data = NULL;
    free(arguments->dtype);
    arguments->dtype = NULL;
    free(arguments->cmdptr);
    arguments->cmdptr = NULL;
}

//Añadir que devuelva un valor por si no se ha conseguido una linea con el tamaño indicado
void getaline ( char* str, int size )
{
    if (!str) return;
    int i = 0;
    char c;
    while( i < size - 1 && (c = getc(stdin)) != '\n' )
    {
        str[i] = c;
        i++;
    }
    if ( 0 < size )
        str[i] = '\0';
}

//err = 1 (err_formato), err = 2 (null), err = 3 (overflow)
int str2int ( char* str, int* err )
{
    int result = 0;
    char* strc = str && *str == '-'? str + 1: str;
    if (err)
    {
        *err = str? 0: 2;
        while( !*err && *strc )
        {
            result *= 10;
            if (isdigit(*strc))
            {
                result += *strc - '0';
            }
            else
            {
                *err = 1;
            }
            if ( result < 0 && result != INT_MIN ) *err = 3;
            strc++;
        }
        if ( str && *str == '-' ) result *= -1;
    }
    else
    {
        while( str && isdigit(*strc) && 0 <= result )
        {
            result *= 10;
            result += *strc - '0';
            strc++;
        }
        if ( str && *str == '-' ) result *= -1;
    }
    return result;
}

//err = 0 (no_err), err = 1 (err_formato), err = 2 (null), err = 3 (overflow), err = 4 (err_base)
//err_formato si todos no son minusculas o mayusculas...
//base 32 (dhexa) = 0123456789 abcdefghij klmnopqrts uv
int str2intg ( char* str, int* err, int base )
{
    int result = 0;
    char* strc = str && (*str == '-' || *str == '+')? str + 1: str;
    strc = str && *strc == '0' && tolower(strc[1]) == 'x' && base == 16? strc + 2 : strc;
    //strc = str && *strc == '0' && tolower(strc[1]) == 'b' && base == 2? strc + 2 : strc;
    if (err)
    {
        if ( base < 2 || 32 < base )
        {
            *err = 4;
            return result;
        }
        *err = str? 0: 2;
        while( !*err && *strc )
        {
            result *= base;
            if (isdigit(*strc))
            {
                result += *strc - '0';
            }
            else if ( 'a' <= *strc && *strc <= 'a' + base - 11 )
            {
                result += *strc - 'a' + 10;
            }
            else if ( 'A' <= *strc && *strc <= 'A' + base - 11 )
            {
                result += *strc - 'A' + 10;
            }
            else
            {
                *err = 1;
            }
            if ( result < 0 && result != INT_MIN ) *err = 3;
            strc++;
        }
        if ( str && *str == '-' ) result *= -1;
    }
    else
    {
        int err_fmt = 0;
        while( str && *strc && !err_fmt && 0 <= result )
        {
            if ( base < 2 || 32 < base ) err_fmt = 1;
            result *= base;
            if (isdigit(*strc))
            {
                result += *strc - '0';
            }
            else if ( 'a' <= *strc && *strc <= 'a' + base - 11 )
            {
                result += *strc - 'a' + 10;
            }
            else if ( 'A' <= *strc && *strc <= 'A' + base - 11 )
            {
                result += *strc - 'A' + 10;
            }
            else
            {
                err_fmt = 1;
            }
            strc++;
        }
        if ( str && *str == '-' ) result *= -1;
    }
    return result;
}
