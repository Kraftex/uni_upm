Estructura de Computadores
==========================

Esa gran asignatura del DATSI que nos da pesadillas en binario, complemento A1 y A2 y sobre todo en ensamblador.

## Contenidos
- [own_emu.c](#own_emuc)
- [tester.ens](#testerens)


### own_emu.c
Implementación propia del mc88110.

   ***
### tester.ens
El tester funciona de esta manera, se llama en forma de subrutina a un test (dentro de este se realizaran 8 pruebas a la subrutina especifica). Y en **r29** debería dejar un valor que contenga varias **F** o **A** y pueden estar intercaladas. Para que luego al llamar a `Check` lo compruebe. Cuando `Check` lo revise, usará casi todos los registros **[r1-r28]** para escribir una palabra de cuatro letras.

Aunque ese es el comportamiento habitual, se puede especificar que solo se hagan unos tests y no otros. Para ello se dispone de dos *macros*:
```
- TESTEAR(subrutina)
- TESTWB(subrutina)
```

Donde en *subrutina* se especifica el nombre de la subrutina a testear, la gran diferencia de ambas subrutinas es que `TESTEAR` va siempre a hacer un `stop` al final de realizar el test especificado, si se desea agrupar varios tests se debe usar `TESTWB`.

**Interpretación de los registros del 1 al 28**\
Después de la llamada a un test siempre se llama o se debería de llamar a la subrutina `Check` y procederá a escribir en todos los registros mencionados una serie de valores (1, A, F) para componer una palabra de 4 carateres de las siguientes: **PASS** y **FAIL**. Otra palabra que puede aparecer es **WTF** en relacion al pasarle a `TESTEAR` o `TESTWB` una etiqueta que no corresponda a una subrutina que disponga de test.\
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
El test ha salido perfecto y usando A se escribe **PASS**.

**Interpretación del resultado de r29**\
Como un registro de 32b puede representarse usando 8 caracteres hexadecimales, es el motivo por el que se hacen solo 8 pruebas. La posición donde se encuentre el carácter indica el número interno de la prueba. El carácter en cuentión será siempre **A** cuando sea correcto pero cualquier otro carácter indicará que se produjo un error en alguna verificación de la prueba, normalmente se usará la **F**.\
**- Ejemplo**:\
`R29 = AAAAAFAF h`\
Indica que dos pruebas no fueron bien y corresponden a la prueba 6 y 8.

**Interpretación del resultado de r31**\
Aunque este registro no debería de tocarse debido a que se usa como **FP**(Marco de pila), `Check` solo se llama o se debería de llamar durante la ejecución del `main` ya que no se han reservado variables en la pila y por tanto se puede usar. Su único uso dentro de este contexto es para marcar "donde se quedó ejecutando la subrutina" que falló he hizo que el procesador se parara. Para averiguar que test/subrutina falló basta con usar el comando `d 0xValorR31 1` donde en `ValorR31` se escribe el contenido de dicho registro, se pueden ignorar los 0 iniciales.\
Los posibles valores pueden ser: `TX`(donde **X** es el Nº del test), `T{name}`(donde **{name}** es el nombre de la subrutina) y `t{name}`(donde **{name}** es el nombre de la subrutina). Adicionalmente puede aparecer **WAYD** o **wayd** cuando no se encuentra la subrutina, producido únicamente porque no es una subrutina que disponga de test.\
**- Ejemplo**:\
`R31 = 000004A8 h`\
Ejecutar `d 0x4a8 1` y al inicio de la linea que escribe aparece la subrutina/test que falló.
