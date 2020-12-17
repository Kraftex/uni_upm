Estructura de Computadores
==========================

Esa gran asignatura del DATSI que nos da pesadillas en binario, complemento A1 y A2 y sobre todo en ensamblador.

## Contenidos
- [own_emu.c](#own_emuc)
- [tester.ens](#testerens)


### own_emu.c
Implementación propia del mc88110.\
La implementación contempla todas las funciones que emula el original pero no están implementadas aún las referentes a la coma flotante.

   ***
### tester.ens
Dentro están declaradas las etiquetas de las subrutinas donde se deberá de escribir el código a testear.\
El tester funciona de esta manera, se llama en forma de subrutina a un test (dentro de este se realizaran 8 pruebas a la subrutina especifica). Y en **r29** debería dejar un valor que contenga varias **F** o **A** y pueden estar intercaladas. Para que luego al llamar a `Check` lo compruebe. Cuando `Check` lo revise, usará casi todos los registros **[r1-r28]** para escribir una palabra de cuatro letras.

Aunque ese es el comportamiento habitual, se puede especificar que solo se hagan unos tests y no otros. Para ello se dispone de dos *macros*:
```
- TESTEAR(subrutina)
- TESTWB(subrutina)
```

Donde en *subrutina* se especifica el nombre de la subrutina a testear, la gran diferencia de ambas subrutinas es que `TESTEAR` va siempre a hacer un `stop` al final de realizar el test especificado, si se desea agrupar varios tests se debe usar `TESTWB` otro añadido que incluyen es que si la subrutina está vacía, es decir, solo hay un **jmp (r1)** no se realizará el test a la subrutina.

**Interpretación de los registros del 1 al 28**\
Después de la llamada a un test siempre se llama o se debería de llamar a la subrutina `Check` y procederá a escribir en todos los registros mencionados una serie de valores (1, A, E, F) para componer una palabra de 4 carateres de las siguientes: **PASS** y **FAIL**. Otra palabra que puede aparecer es **WTF** en relacion al pasarle a `TESTEAR` o `TESTWB` una etiqueta que no corresponda a una subrutina que disponga de test, también puede aparecer **SNI** debido a que la subrutina está vacía y solo posee el retorno (**jmp (r1)**).\
**- Ejemplo**: 
```
 R01 = AAAAAA11 h R02 = 111AA111 h R03 = 11AAAA11 h R04 = 11AAAA11 h
 R05 = AA1111AA h R06 = 1AA11AA1 h R07 = AA1111AA h R08 = AA1111AA h
 R09 = AA1111AA h R10 = 1AA11AA1 h R11 = AA111111 h R12 = AA111111 h
 R13 = AAAAAA11 h R14 = AA1111AA h R15 = 11AAAA11 h R16 = 11AAAA11 h
 R17 = AA111111 h R18 = AAAAAAAA h R19 = 111111AA h R20 = 111111AA h
 R21 = AA111111 h R22 = AA1111AA h R23 = AA1111AA h R24 = AA1111AA h
 R25 = AA111111 h R26 = AA1111AA h R27 = 11AAAA11 h R28 = 11AAAA11 h
 R29 = AAAAAAAA h R30 = 00020000 h R31 = 000004A8 h
```
El test ha salido perfecto y usando A se escribe **PASS**(Pista: Ctrl+F AA).

**Interpretación del resultado de r29**\
Como un registro de 32b puede representarse usando 8 caracteres hexadecimales, es el motivo por el que se hacen solo 8 pruebas. La posición donde se encuentre el carácter indica el número interno de la prueba. El carácter en cuestión será siempre **A** cuando sea correcto pero cualquier otro carácter indicará que se produjo un error en alguna verificación de la prueba, normalmente se usará la **F**. Otro posible valor que puede tomar este registro es `01C000F4` al usar `TESTWB` o `TESTEAR`.\
**- Ejemplo**:\
`R29 = AAAAAFAF h`\
Indica que dos pruebas no fueron bien y corresponden a la prueba 6 y 8.

**Interpretación del resultado de r31**\
Aunque este registro no debería de tocarse debido a que se usa como **FP**(Marco de pila), `Check` solo se llama o se debería de llamar durante la ejecución del `main` ya que no se han reservado variables en la pila y por tanto se puede usar. Su único uso dentro de este contexto es para marcar "donde se quedó ejecutando la subrutina" que falló he hizo que el procesador se parara. Para averiguar que test/subrutina falló basta con usar el comando `d 0xValorR31 1` donde en `ValorR31` se escribe el contenido de dicho registro, se pueden ignorar los 0 iniciales.\
Los posibles valores pueden ser: `TX`(donde **X** es el Nº del test), `T{name}`(donde **{name}** es el nombre de la subrutina), `t{name}`(donde **{name}** es el nombre de la subrutina) y `FIN_TESTS`(Indica la finalización de todos los tests o fallo del Test8). Adicionalmente puede aparecer **WAYD** o **wayd** cuando no se encuentra la subrutina, producido únicamente porque no es una subrutina que disponga de test.\
**- Ejemplo**:\
`R31 = 000004A8 h`\
Ejecutar `d 0x4a8 1` y al inicio de la linea que escribe aparece la subrutina/test que falló.

**Interpretación de las etiquetas**\
A lo largo del todo el test existen un montón de etiquetas, no todas son tan importantes. La más importante en relación al resultado de las pruebas es `Get_wellTX`(donde **X** es el Nº del test y se reservó memoría para guardar el valor de dicho test). Otras de gran interés son: `bcl_X`(**X**=Nº Test, etiqueta para el bucle), `bsr_X`(**X**=Nº Test, etiqueta localizada justo cuando se va a llamar a la subrutina a testear), `actgw_X`(**X**=Nº Test, etiqueta que está colocada antes de las comprobaciones para saber si la prueba fue bien o mal), `Data_numL`(**L**=Letra, variables globales que usa cada test para almacenar datos de entrada o resultados), `Data_picL`(**L**=Letra, variables globales que usa cada test para almacenar imagenes) y `ConfigTX`(**X**=Nº Test, subrutina que tiene el único fin de configurar todas las varibles globales mencionadas anteriormente).\
Para revisar el resultado de `Get_wellTX` ejecutar `v Get_wellTX 1`.\
Para revisar los valores de `Data_numL` ejecutar `v Data_numL 8`, hay más de un valor ya que realmente es un array de numeros.

**Cambios**\
**- V2.3**
Arreglado el conflicto de las macros de testeo (`TESTWB`, `TESTEAR`) al estar declarados solo como macros las etiquetas se duplicaban e impedía el uso de usarlas varias veces sobre todo `TESTWB`.\
Las palabras/letras que se escriban usando **E** son cosideradas errores no relacionados directamente con lo que devuelve de resultado la subrutina, así `WTF` ha pasado a usar **E** y `SNI` también las usa.\
Se ha añadido el nuevo acrónimo visual `SNI` cuyo significado es **S**ubrutine **N**ot **I**mplemented.\