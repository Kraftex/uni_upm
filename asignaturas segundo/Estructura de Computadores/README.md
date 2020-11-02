Estructura de Computadores
==========================

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
Después de la llamada a un test siempre se llama o se debería de llamar a la subrutina `Check` y procederá a escribir en todos los registros mencionados una serie de valores (1, A, F) para componer una palabra de 4 carateres de las siguientes: **PASS** y **FAIL**. Otra palabra que puede aparecer es **WTF** en relacion al pasarle a `TESTEAR` o `TESTWB` una etiqueta que no corresponda a una subrutina que disponga de test.
**-- Ejemplo**: 
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

**Interpretación del resultado de r29**\
Como un registro de 32b puede representarse usando 8 caracteres hexadecimales, es el motivo por el que se hacen solo 8 pruebas. La posición donde se encuentre el carácter indica el número interno de la prueba. El carácter en cuentión será siempre **A** cuando sea correcto pero cualquier otro carácter indicará que se produjo un error en alguna verificación de la prueba, normalmente se usará la **F**.\
**-- Ejemplo**:\
R29 = AAAAAFAF
Indica que dos pruebas no fueron bien y corresponden a la prueba 6 y 8.