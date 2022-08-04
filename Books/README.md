# ASERE polyglot book by Ernesto Torres

**Version:** 22.05  
**Release date:** 01-05-2022  
**License:** GPL  
**email:** NawitoChessEngine@yandex.com  


# DESCRIPCION GENERAL
Asere es el libro de apertura utilizado por el motor cubano Nawito.  
Ha sido confeccionado teniendo en cuenta la ganancia de ELO y no el  
resultado de las partidas, segir leyendo para mas detalles.  


# CARACTERISTICAS
1. Confeccionado a partir de partidas de CCRL.  
2. Donde al menos aparece un motor con ELO >= 3000.  
3. Alcanzando una maxima profundidad de 16 movimientos.  
4. Las posiciones ha sido jugadas un minimo de 8 veces.  
5. Con un maximo de 4 movimientos por posicion.  
6. Los movimientos con baja probabilidad de seleccion _(menor al 8%)_ han sido removidos.  
7. El valor de los movimientos es tomado como la suma de las ganancias del ELO + 1 _(solo cuando es >= 1)_.  
8. Otras modificaciones para garantizar la compatibilidad con el formato polyglot.  

