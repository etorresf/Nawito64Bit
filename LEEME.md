Nawito2012 - Motor de ajedrez por Ernesto Torres. Cuba.
===============================================================================================
Versión del motor:			2012
Fecha de publicación:		?-12-2020
Versiones disponibles:		Windows/Linux de 64 bits
Protocolo de comunicación:	Xboard
ELO estimado:				± 2650
Licencia:					GPL
Autor email:				ernesto2@nauta.cu


TABLA DE CONTENIDO
===============================================================================================
1) Descripción general.
2) Contenido.
3) Rendimiento.
4) Características.
5) Licencia.
6) Créditos.


1)	DESCRIPCIÓN GENERAL
===============================================================================================

Nawito2012 es un motor de ajedrez cubano libre derivado de Danasah5.07 compatible con el protocolo 
Winboard. Para sacarle el máximo provecho debería usarse bajo una GUI(interfaz de usuario gráfica) 
como Arena(Linux/Windows), Winboard(Windows), Pychess(Linux/Windows), SCID(Linux/Windows).

Su autor, se ha basado en la función de búsqueda del motor Danasah5.07 e inspirado en otros como TSCP, 
CPW, Rebel, etc. para desarrollar ideas propias y dotarlo de un estilo particular.

Esta escrito en C y se utiliza CodeBlocks con GNU GCC, tanto para Windows como para Linux. 

Este motor usa su propio esquema de libros de aperturas.

Por favor reporte cualquier idea, sugerencia o error acerca del motor.


2)	CONTENIDO
===============================================================================================

- Nawito2012				->	Motor para Linux.
- Nawito2012.exe			->	Motor para Windows.
- LEEME.txt					->	Este archivo.
- README.txt				->	Traducción al inglés de este archivo.
- Nawito2012.ini			->	Configuración del motor.
- Nawito1812book.nbk, 
  Nawito1812MainBook.nbk	->	Libros de aperturas del motor Nawito.
- logo.jpg					->	Logo of program.


3)	RENDIMIENTO
===============================================================================================

--perft <n>				->	Comando para descubrir fallos y velocidad del generador
							de movimientos desde la posición:
							"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - "
							hasta la profundidad <n> (por defecto 5).
--bench					->	Comando para comprobar la velocidad relativa del ordenador 
							respecto a otro. Resuelve mate desde la posición:
							"r3rk2/ppq2pbQ/2p1b1p1/4p1B1/2P3P1/3P1B2/P3PPK1/1R5R w - - 0 1 "


4)	CARACTERÍSTICAS
===============================================================================================

- Protocolo						->	Winboard.
- Representación del tablero	->	Bitboard
- Generador de movimientos		->	Bitboard.
- Búsqueda						->	Aspiration window
								->	Profundidad iterativa.
								->	PVS/Alpha-Beta.
								->	Quiescence.
- Reducciones/Podas				->	Distancia mate.
								->	Movimiento nulo.
								->	Tabla de transposición.
								->	Futility.
								->	Razoring.
								->	Captures.
								->	Late Move Reductions (LMR).
- Extensiones					->	Jaque.
								->	Amenaza de mate.
								->	Entrando a final de peones.
- Ordenación de movimientos		->	PV.
								->	Tabla de transposición.
								->	Profundización iterativa Interna. (IID)
								->	MVV/LVA.
								->	Killer heurístico.
								->	Historico heurístico.
- Evaluación					->	Material.
								->	Posición de las piezas en el tablero.
								->	Material tables.
								->	Tapered eval.
								->	Estructura de peones.
									->	Peón avanzado.
									->	Peón retrasado.
									->	Peón candidato.
									->	Peón doblados.
									->	Peón aislado.
									->	Peón pasado.
								->	Pareja de alfiles.
								->	Alfil malo.
								->	Alfil atrapado.
								->	Caballo atrapado.
								->	Caballo apostado.
								->	Caballo valor decreciente según desaparecen los peones.
								->	Torre valor creciente según desaparecen los peones.
								->	Torre en fila abierta y semi abierta.
								->	Torre en octava.
								->	Torre atrapada.
								->	Torre bloqueada.
								->	Torre en 7ma.
								->	Seguridad del rey.
								->	Escudo del rey.
								->	Tropismo al Rey.
								->	Finales (KP vs k, KBN vs k, KR vs k, KBB vs K, KRP vs KR, otros).
- Libro de Aperturas			->	Esquema propio de libro de aperturas.
								->	El aprendizaje del libro aun esta bajo pruebas y perfeccionamiento.
- Otros							->	Detección del final del juego con las reglas de la FIDE, incluyendo triple repetición y la regla de 50 movimientos..
									Tablas por material insuficiente con (KNN vs k, K vs knn, (KR vs k, K vs kr)...
- Modo de juego					->	Control por tiempo (convencional, incremental y tiempo fijo por movimiento) o profundidad fija.


5)	LICENCIA
===============================================================================================

Este programa es software libre: puedes redistribuirlo y/o modificarlo. Bajo los términos de la 
Licencia Pública General GNU tal como fue publicada por la Free Software Foundation, ya sea la 
versión 3 de la Licencia, o (a su elección) cualquier versión posterior.

Este programa se distribuye con la esperanza de que sea útil, pero SIN NINGUNA GARANTÍA; sin 
siquiera la garantía implícita de COMERCIABILIDAD o APTITUD PARA UN PROPÓSITO PARTICULAR. Ver el 
GNU General Public License para más detalles.

Debería haber recibido una copia de la Licencia Pública General de GNU junto con este programa. 
Si no, vea <http://www.gnu.org/licenses/>.


6)	CRÉDITOS
===============================================================================================

Estoy muy agradecido a las siguientes personas:

- A mi familia y amigos, que siempre están ahí para ayudar.
- A todos aquellos que han decidido compartir el código fuente de sus motores (Me han aclarado funciones y entendimiento de ellos).
- A Pedro Castro por la funcion de busqueda de su motor DanaSah. Por su cooperación y por aclararme asuntos legales en cuanto a motores de ajedrez.
- A la web CPW <a href="http://chessprogramming.wikispaces.com">(http://chessprogramming.wikispaces.com)</a> por todo el contenido sobre las técnicas aplicables a un motor ajedrez.
- A la web ajedrezmagno <a href="http://ajedrezmagno.cubava.cu"> por divulgar y brindar información acerca de Nawito.
- A Dusan Stamenkovic por hacer un logo para Nawito.
- A Pedro Moreno por crear los libros: Nawito1812book.nbk y Nawito1812MainBook.nbk
- A todos aquellos que han jugado con el motor y han publicado su experiencia y opiniones.

