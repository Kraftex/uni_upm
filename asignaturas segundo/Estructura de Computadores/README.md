Estructura de Computadores
==========================

### own_emu.c
Implementación propia del mc88110.

   ***
### tester.ens
El tester funciona de esta manera, se llama en forma de subrutina a un test (dentro de este se realizaran 8 pruebas a la subrutina especifica). Y en r29 debería dejar un valor que contenga varias F o A y pueden estar intercaladas. Para que luego al llamar a Check lo compruebe. Cuando Check lo revise, usará casi todos los registros [r1-r28] para escribir una palabra de cuatro letras.

Aunque ese es el comportamiento habitual, se puede especificar que solo se hagan unos tests y no otros. Para ello se dispone de dos *macros*:
```
- TESTEAR(subrutina)
- TESTWB(subrutina)
```