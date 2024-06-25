#include <iostream>  //permite la entrada y salida estandar cout cin
#include <vector>    // permite la implementacion de arreglo dinamico que puede crecer y reducir su tamanho automaticamente
#include <stack>     //implementa una estructura de datos tipo pila Lifo(ultimo en entrar primero en salir)
#include <algorithm> // contiene varias funciones estandar
#include <random>    // ofrece herramientas para la generacion de numeros aleatorios

using namespace std; // es una forma conveniente de acceder a todos los elementos de la biblioteca estándar sin tener que especificar std:: repetidamente

enum Celda // define un enumerador  para representar diferentes estados o tipos de celdas que se necesite distinguir entre varios tipos de celdas.
{
    MURO,     // Representa una celda que es un muro o una barrera.
    CAMINO,   // Representa una celda que es transitable (un camino libre).
    VISITADO, // Representa una celda que ha sido visitada durante algún proceso (por ejemplo, búsqueda).
    SOLUCION  // Representa una celda que forma parte de la solución (por ejemplo, un camino encontrado).
};

struct Posicion // La estructura Posicion definida representa simplemente las coordenadas (x, y)
{
    int x, y;                              // Son variables enteras que representan las coordenadas de la posición en el plano. x representa la coordenada horizontal y y la coordenada vertical.
    Posicion() : x(-1), y(-1) {}           // Este constructor inicializa las coordenadas x y y con el valor -1 cuando se necesita inicializar una posición sin asignar valores específicos a las coordenadas.
    Posicion(int x, int y) : x(x), y(y) {} // Este constructor permite crear un objeto Posicion especificando las coordenadas x e y deseadas. proporciona flexibilidad al crear objetos Posicion con valores específicos de coordenadas desde el momento de la creación.
};

class Laberinto // define una nueva clase
{
private: // No global, solo se pueden acceder a estos dentro de la clase laberinto
    // Caracteres que representan los distintos tipos de celdas en el laberinto
    static const char CARACTER_MURO = '#';     // Define un carácter estático (static) constante (const) que representa un muro en el laberinto.
    static const char CARACTER_CAMINO = ' ';   // Define un carácter estático constante que representa un camino libre en el laberinto.
    static const char CARACTER_ENTRADA = 'E';  // Define un carácter estático constante que indlica la entrada al laberinto.
    static const char CARACTER_SALIDA = 'S';   // Define un carácter estático constante que indica la salida del laberinto.
    static const char CARACTER_VISITADO = '?'; // Define un carácter estático constante que se usa para marcar una celda como visitada durante algún proceso en el laberinto.
    static const char CARACTER_SOLUCION = '*'; // Define un carácter estático constante que se usa para marcar una celda como parte de la solución encontrada en el laberinto.

    // Vectores para almacenar la representación lógica y visual del laberinto
    vector<vector<Celda>> laberinto;      // Almacena el estado lógico del laberinto  declara un vector 2d que almacenará el estado lógico del laberinto, utilizando el tipo Celda que probablemente está definido como el enumerador mencionado anteriormente.
    vector<vector<char>> laberintoVisual; // Almacena la representación visual del laberinto Declara un vector 2D que almacenará la representación visual del laberinto, utilizando caracteres (char).
    vector<vector<bool>> visitado;        // Almacena si una celda ha sido visitada durante el procesamiento  Declara un vector 2D que almacenará información booleana sobre si una celda ha sido visitada durante algún proceso.
    int ancho, alto;                      // Dimensiones del laberinto (ancho x alto)  Declara dos enteros (int) para almacenar el ancho y alto del laberinto.
    Posicion entrada, salida;             // Posiciones de entrada y salida del laberinto  Declara dos objetos de tipo Posicion que representan las coordenadas de la entrada y la salida del laberinto.

    // Arreglos para facilitar el movimiento en las cuatro direcciones (arriba, abajo, izquierda, derecha)
    const int dx[4] = {-1, 1, 0, 0}; // Desplazamiento en x para cada dirección Declara un arreglo constante de enteros (int) que contiene los desplazamientos en el eje x para cada una de las cuatro direcciones posibles (arriba, abajo, izquierda, derecha).
    const int dy[4] = {0, 0, -1, 1}; // Desplazamiento en y para cada dirección Declara un arreglo constante de enteros que contiene los desplazamientos en el eje y para cada una de las cuatro direcciones posibles.

    // Función privada para verificar si una posición (x, y) está dentro de los límites del laberinto
    bool esValida(int x, int y) // Define una función privada que verifica si una posición (x, y) está dentro de los límites del laberinto (es decir, si x está entre 0 y alto-1 y y está entre 0 y ancho-1).
    {
        return x >= 0 && x < alto && y >= 0 && y < ancho; // evalúa si las coordenadas (x, y) están dentro de los límites válidos del laberinto, utilizando las dimensiones alto (filas) y ancho (columnas) como referencia.
    }

public:                                         // define que se puede pueden acceder a desde afuera //global
    Laberinto(int w, int h) : ancho(w), alto(h) // Constructor de la clase Laberinto que recibe dos parámetros enteros (w y h) para especificar el ancho y alto del laberinto.
    {
        laberinto.resize(alto, vector<Celda>(ancho, MURO));     // Redimensiona el vector laberinto para que tenga alto filas y ancho columnas, inicializándolo con celdas del tipo MURO.
        laberintoVisual.resize(alto, vector<char>(ancho, ' ')); // Redimensiona el vector laberintoVisual para que tenga las mismas dimensiones que laberinto, inicializándolo con espacios en blanco (' ').
        visitado.resize(alto, vector<bool>(ancho, false));      // Redimensiona el vector visitado para que tenga las mismas dimensiones que laberinto, inicializándolo con valores booleanos false.
    }

    void generar() // esta funcion se encarga de generar un laberinto inicializando con muros
    {
        random_device rd;  // se utiliza para obtener numeros aleatorios de alta calidad// una semilla aleatoria
        mt19937 rng(rd()); // asegura que cada ejecucion tenga unsa secuencia de numeros diferentes

        // Inicializar el laberinto con todos los muros
        for (int i = 0; i < alto; ++i) // recorrecmos todas las filas o el alto del laberinto
        {
            for (int j = 0; j < ancho; ++j) // iniciamos otro bucle que recorre las columnas del laberinto para cada fila i
            {
                laberinto[i][j] = MURO; // inicializa todas las celdas del laberinto con muros
            }
        }

        stack<Posicion> pila;                   // almacena objetos del tipo posicion el ultimo en insertar el primero en eliminarse
        Posicion inicio(1, 1);                  //// Crea un objeto Posicion llamado inicio con coordenadas (1, 1)
        laberinto[inicio.x][inicio.y] = CAMINO; //// Marca la posición inicio en el laberinto como CAMINO
        pila.push(inicio);                      //// Agrega la posición inicio a la pila

        while (!pila.empty()) // inicia un bucle que se ejecuta mientras pila no este vacia y hasta que todas las posiciones se hayan explorado completamente
        {
            // Obtener la posición actual de la cima de la pila
            Posicion actual = pila.top();
            // Eliminar la posición actual de la pila
            pila.pop();

            // Definir las cuatro direcciones posibles y barajarlas aleatoriamente
            vector<int> direcciones = {0, 1, 2, 3};               // Crea un vector direcciones que contiene las cuatro direcciones posibles (arriba, abajo, izquierda, derecha).
            shuffle(direcciones.begin(), direcciones.end(), rng); // Baraja aleatoriamente las direcciones en el vector direcciones utilizando el generador de números pseudoaleatorios rng.

            // Iterar sobre las direcciones barajadas
            for (int dir : direcciones)
            {
                // Calcular las posiciones vecinas y la posición después de la vecina
                int nx = actual.x + dx[dir];      // Calcula la coordenada x de la posición vecina en la dirección dir.
                int ny = actual.y + dy[dir];      // Calcula la coordenada y de la posición vecina en la dirección dir.
                int nnx = actual.x + 2 * dx[dir]; // Calcula la coordenada x de la posición después de la vecina en la dirección dir
                int nny = actual.y + 2 * dy[dir]; // Calcula la coordenada y de la posición después de la vecina en la dirección dir.

                // Verificar si la posición después de la vecina es válida y es un muro
                if (esValida(nnx, nny) && laberinto[nnx][nny] == MURO) // Verifica si la posición después de la vecina (nnx, nny) es válida dentro de los límites del laberinto y si es un muro (MURO)
                {
                    // Marcar como CAMINO las posiciones vecina y la posición después de la vecina
                    laberinto[nx][ny] = CAMINO;   // Marca la posición vecina (nx, ny) como parte del camino (CAMINO).
                    laberinto[nnx][nny] = CAMINO; // Marca la posición después de la vecina (nnx, nny) como parte del camino (CAMINO). Esto simula excavar un pasillo en el laberinto.
                    // Agregar la posición después de la vecina a la pila
                    pila.push(Posicion(nnx, nny)); // Esto asegura que la nueva posición se explore en la siguiente iteración del bucle while.
                }
            }
        }

        entrada = {0, 1};                                         // Establecer la posición de entrada en las coordenadas (0, 1)
        salida = {alto - 2, ancho - 2};                           // Asegurar que la posición de salida esté en la esquina inferior derecha del laberinto
        laberinto[entrada.x][entrada.y] = CAMINO;                 // Marcar la posición de entrada como parte del camino en el laberinto lógico
        laberinto[salida.x][salida.y] = CAMINO;                   // Marcar la posición de salida como parte del camino en el laberinto lógico
        laberintoVisual[entrada.x][entrada.y] = CARACTER_ENTRADA; // Mostrar la posición de entrada en el laberinto visual con el caracter de entrada
        laberintoVisual[salida.x][salida.y] = CARACTER_SALIDA;    // Mostrar la posición de salida en el laberinto visual con el caracter de salida
    }

    void imprimir() // Esta función se encarga de imprimir el laberinto en la consola.
    {
        for (int i = 0; i < alto; i++) // Itera sobre cada fila del laberinto, desde 0 hasta alto - 1.
        {
            for (int j = 0; j < ancho; j++) // Itera sobre cada columna del laberinto para la fila actual, desde 0 hasta ancho - 1
            {
                switch (laberinto[i][j]) // Evalúa el valor de laberinto[i][j] para determinar qué caracter imprimir en función del tipo de celda.
                {
                case MURO: // En caso de que laberinto[i][j] sea MURO
                    // Imprimir el caracter correspondiente al MURO
                    cout << CARACTER_MURO;
                    break;
                case CAMINO: // case CAMINO:: En caso de que laberinto[i][j] sea CAMINO
                    // Verificar si la posición en laberintoVisual contiene CARACTER_VISITADO
                    if (laberintoVisual[i][j] == CARACTER_VISITADO) //  Si laberintoVisual[i][j] es igual a CARACTER_VISITADO, imprime CARACTER_VISITADO
                    {
                        // Imprimir CARACTER_VISITADO en rojo
                        cout << "\x1b[31m" << CARACTER_VISITADO << "\x1b[0m";
                    }
                    // Verificar si la posición en laberintoVisual contiene CARACTER_SOLUCION
                    else if (laberintoVisual[i][j] == CARACTER_SOLUCION) // Si laberintoVisual[i][j] es igual a CARACTER_SOLUCION, imprime CARACTER_SOLUCION
                    {
                        // Imprimir CARACTER_SOLUCION en verde
                        cout << "\x1b[32m" << CARACTER_SOLUCION << "\x1b[0m";
                    }
                    else // En cualquier otro caso (cuando laberintoVisual[i][j] no es ni CARACTER_VISITADO ni CARACTER_SOLUCION), imprime el caracter almacenado en laberintoVisual[i][j], que representa el estado actual de esa posición en el laberinto.
                    {
                        // Imprimir el caracter normal de laberintoVisual
                        cout << laberintoVisual[i][j];
                    }
                    break;
                default:
                    // Imprimir el caracter normal de laberintoVisual en caso de otro valor
                    cout << laberintoVisual[i][j];
                    break;
                }
            }
            // Saltar a una nueva línea al terminar cada fila del laberinto
            cout << endl;
        }
    }

    bool resolver() // se encarga de buscar una solucion para salir del laberinto desde la E hasta la S utilizando dfs
    {
        visitado.assign(alto, vector<bool>(ancho, false)); // Reiniciar celdas visitadas

        stack<Posicion> pila; // Crea una pila de tipo Posicion para almacenar las posiciones que se están explorando.
        pila.push(entrada);   // Agregar la posición de entrada a la pila

        vector<vector<Posicion>> padre(alto, vector<Posicion>(ancho, Posicion())); // Matriz para almacenar los padres de cada posición

        bool encontrado = false;       // Bandera para verificar si se encontró alguna solución
        bool salidaEncontrada = false; // Bandera para verificar si se ha encontrado la salida

        while (!pila.empty()) // Inicia un bucle while que continúa mientras la pila no esté vacía, lo que significa que aún hay posiciones por explorar.
        {
            Posicion actual = pila.top(); // Obtener la posición actual de la pila
            pila.pop();                   // Eliminar la posición actual de la pila

            if (laberintoVisual[actual.x][actual.y] == CARACTER_SALIDA) // Verifica si la posición actual es la salida del laberinto en el laberinto visual.
            {
                encontrado = true;       // Indicar que se encontró una solución
                salidaEncontrada = true; // Indicar que se encontró la salida
                break;                   // Salir del bucle una vez encontrada la salida
            }

            bool tieneMovimientoValido = false; // Bandera para verificar si la posición actual tiene movimientos válidos

            // Iterar sobre las cuatro direcciones posibles (arriba, abajo, izquierda, derecha)
            for (int i = 0; i < 4; i++)
            {
                int nx = actual.x + dx[i], ny = actual.y + dy[i]; // Calcular las coordenadas de la posición vecina

                // Verificar si la posición vecina es válida y si es un camino no visitado
                if (esValida(nx, ny) && laberinto[nx][ny] == CAMINO && !visitado[nx][ny]) // Verifica si la posición vecina es válida, es un camino (CAMINO) y no ha sido visitada.
                {
                    pila.push(Posicion(nx, ny));  // Agregar la posición vecina a la pila
                    visitado[nx][ny] = true;      // Marcar la posición vecina como visitada
                    padre[nx][ny] = actual;       // Establecer el padre de la posición vecina como la posición actual
                    tieneMovimientoValido = true; // Indicar que la posición actual tiene al menos un movimiento válido
                }
            }

            // Marcar la posición actual como parte del camino de solución con '*'
            if (tieneMovimientoValido && (actual.x != entrada.x || actual.y != entrada.y) && (actual.x != salida.x || actual.y != salida.y)) // Marca la posición actual como parte del camino de solución (CARACTER_SOLUCION) si tiene movimientos válidos y no es ni la entrada ni la salida.
            {
                laberintoVisual[actual.x][actual.y] = CARACTER_SOLUCION; // Marca la posición actual como parte del camino de solución (CARACTER_SOLUCION) si tiene movimientos válidos y no es ni la entrada ni la salida.
            }
        }

        // Marcar el camino de solución completo con '*'
        if (encontrado)
        {
            marcarCaminoSolucion(padre);                              // Marcar el camino de solución en el laberinto visual
            laberintoVisual[entrada.x][entrada.y] = CARACTER_ENTRADA; // Marcar la posición de entrada con el caracter de entrada
            laberintoVisual[salida.x][salida.y] = CARACTER_SALIDA;    // Marcar la posición de salida con el caracter de salida
        }

        // Marcar las celdas exploradas pero que no son parte del camino de solución con '?'
        for (int i = 0; i < alto; i++) // marcar el camino de solución completo
        {
            for (int j = 0; j < ancho; j++) // Itera sobre cada columna en la fila actual del laberinto.
            {
                if (laberintoVisual[i][j] == ' ' && visitado[i][j]) // Verifica si la posición en laberintoVisual es un espacio vacío (' ') y ha sido visitada (visitado[i][j]).
                {
                    laberintoVisual[i][j] = CARACTER_VISITADO; // Marcar la celda como visitada pero no parte de la solución
                }
            }
        }

        return salidaEncontrada; // Devolver si se encontró la salida
    }

private:
    // Método privado para marcar el camino de solución en el laberinto visual
    void marcarCaminoSolucion(vector<vector<Posicion>> &padre) // contiene las posiciones padres para cada celda del laberinto, lo que permite rastrear el camino desde la salida hasta la entrada.
    {
        Posicion paso = salida;                               // Comenzar desde la salida
        while (!(paso.x == entrada.x && paso.y == entrada.y)) // Continuar hasta llegar a la entrada
        {
            laberintoVisual[paso.x][paso.y] = CARACTER_SOLUCION; // Marcar la posición actual como parte de la solución
            paso = padre[paso.x][paso.y];                        // Moverse a la posición padre
        }
    }
};

int main() // La función main es el punto de entrada del programa.
{
    int ancho, alto;                            // Declaración de variables para el ancho y alto del laberinto
    cout << "Ingrese el ancho del laberinto: "; // Pide al usuario ingresar el ancho del laberinto
    cin >> ancho;                               // Lee el ancho ingresado por el usuario
    cout << "Ingrese el alto del laberinto: ";  // Pide al usuario ingresar el alto del laberinto
    cin >> alto;                                // Lee el alto ingresado por el usuario

    Laberinto laberinto(ancho, alto); // Crea una instancia de la clase Laberinto con las dimensiones dadas
    bool resuelto = false;            // Inicializa una bandera para indicar si el laberinto ha sido resuelto

    while (!resuelto) // Bucle que continúa hasta que el laberinto sea resuelto
    {
        laberinto.generar();      // Genera un nuevo laberinto
        if (laberinto.resolver()) // Intenta resolver el laberinto
        {
            cout << "Este es su laberinto" << endl; // Imprime un mensaje indicando que el laberinto está listo
            laberinto.imprimir();                   // Imprime el laberinto generado y resuelto
            resuelto = true;                        // Marca que el laberinto ha sido resuelto, lo que termina el bucle
        }
    }

    return 0; // Indica que el programa terminó exitosamente
}
