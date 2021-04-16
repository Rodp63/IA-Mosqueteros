/*****
 Computacion Grafica: CCOMP 7-1
 * Gonzales Navarrete Mateo
 * Nieto Rosas Miguel
 * Palma Ugarte Joaquin
 * Zegarra Armando
 *****/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "buildShaders.h"
#include "vertices_colors.h"
#include "grafo.h"
#include <iostream>

#include <random>
#include <math.h>
#include <iomanip>
#include <thread>

const int SIZE_MAP = 100;// 100
int SIZE_PRINT = 3; // ESPACIADO PARA IMPRESION POR CONSOLA
int NODES_TO_ELIMINATE = 2000;
int MOVER_DIBUJO = 0; //4050 USAR SOLO CUANDO EL TAMANO DESEADO NO ES EL MAXIMO DE PANTALLA
int AGRANDAR_ANCHO = 2; //1 -> 1 CUADRANTE | 2->4 CUADRANTES
int AGRANDAR_ALTO = 402; //201 -> MITAD PANTALLA | 2-> ALTO DE PANTALLA
int MIN_VALUE_COORD = 0;
int MAX_VALUE_COORD = SIZE_MAP - 1;

enum algoritmo { AMPLITUD, PROFUNDIDAD, NODETERMINISTICO, HILLCLIMBING, MEJORPRIMERO, AASTERISCO };
algoritmo ALGORITMO_TYPE = algoritmo::AASTERISCO;

using namespace std;
std::random_device rd;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 1000;

const int N = 500; // number of figures
const int MV = 100000; // maximum number of vertexs in one figure 

unsigned int VBO, VAO, EBO[N];

unsigned int figures[N][MV] = {

	{}, // FIGURE 0 CAMINOS DISPONIBLES
	{},// FIGURE 1 PUNTO DE PARTIDA
	{},// FIGURE 2 PUNTO DE LLEGADA
	{},// FIGURE 3 PAREDES

	{},// FIGURE 4 CAMINO RESULTADO BUSQUEDA PROFUNDIDAD
	{},// FIGURE 5 CAMINO RESULTADO BUSQUEDA AMPLITUD
	{},// FIGURE 6 CAMINO RESULTADO ALEATORIO
	{},// FIGURE 7 CAMINO RESULTADO HILL CLIMBING
	{},// FIGURE 8 CAMINO RESULTADO MEJOR EL PRIMERO
	{},// FIGURE 9 CAMINO RESULTADO A*
	
    {0,100, 100,20200, 20200,20100, 20100,0},// FIGURE 10 BORDE DEL MAPA
	{},// FIGURE 11
	{},// FIGURE 12
	{},// FIGURE 13
	{},// FIGURE 14
	{},// FIGURE 15
	{},// FIGURE 16
	{},// FIGURE 17
	{},// FIGURE 18
	{},// FIGURE 19
	{},// FIGURE 20
};

void drawWithEBO(int EBOindex, int vertexNumber,
	GLenum primitive_type = GL_TRIANGLES) {
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[EBOindex]);
	glDrawElements(primitive_type, vertexNumber, GL_UNSIGNED_INT, 0);
}

void startVertexInput() {
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(N, EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	for (int i = 0; i < N; ++i) {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(figures[i]), figures[i],
			GL_STATIC_DRAW);
	}

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

//FUNCIONES
float distanceBetweenCoordinates(int x1, int y1, int x2, int y2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}
bool isInsideListA(int x1, int y1, list<Node<float, int>> lista)
{
    for (list<Node<float, int>>::iterator it = lista.begin(); it != lista.end(); ++it)
    {
        if (it->x == x1 && it->y == y1)
            return true;
    }
    return false;
}
bool isInsideListB(int x1, int y1, list<Node<float, int>*> lista)
{
    for (list<Node<float, int>*>::iterator it = lista.begin(); it != lista.end(); ++it)
    {
        if ((*it)->x == x1 && (*it)->y == y1)
            return true;
    }
    return false;
}
bool ordenamientoAAsterisco(Node<float, int>* a, Node<float, int>* b)
{
    return (a->distanceToObjective + distanceBetweenCoordinates(a->nodeFather->x, a->nodeFather->y, a->x, a->y)) > (b->distanceToObjective + +distanceBetweenCoordinates(b->nodeFather->x, b->nodeFather->y, b->x, b->y));
}
bool ordenamientoHillClimbing(Node<float, int>* a, Node<float, int>* b)
{
    return a->distanceToObjective > b->distanceToObjective;
}
bool ordenamientoNoDeterministica(list<Node<float, int>*> a, list<Node<float, int>*> b)
{
    uniform_int_distribution<int> distribution(MIN_VALUE_COORD, MAX_VALUE_COORD);
    return ((*a.begin())->id + distribution(rd)) < ((*b.begin())->id + distribution(rd));
}
bool tieneHijosPorAgregarYAgregarAAST(Node<float, int>* buscador, list<Node<float, int>*>& listaTemporalDeBusqueda, list<Node<float, int>*>& listaNodosEliminados, int* idnodosNoRespuesta)
{
    bool r = false;
    list<Node<float, int>*> paraAgregar;
    for (list<Node<float, int>*>::iterator it = buscador->edges.begin(); it != buscador->edges.end(); ++it)
    {
        if (isInsideListB((*it)->x, (*it)->y, listaTemporalDeBusqueda))
            continue;
        if (isInsideListB((*it)->x, (*it)->y, listaNodosEliminados)) //ERROR CANNOT DEREFERENCE END LIST ITERATOR
            continue;
        if (idnodosNoRespuesta[(*it)->id])
            continue;
        (*it)->nodeFather = buscador;
        paraAgregar.push_back((*it));
        r = true;
    }
    if (r)
    {
        paraAgregar.sort(ordenamientoAAsterisco);
        for (list<Node<float, int>*>::iterator it = paraAgregar.begin(); it != paraAgregar.end(); ++it)
        {
            listaTemporalDeBusqueda.push_front((*it));
        }
    }
    return r;
}
bool tieneHijosPorAgregarYAgregarHillClimbing(Node<float, int>* buscador, list<Node<float, int>*>& listaTemporalDeBusqueda, list<Node<float, int>*>& listaNodosEliminados, int* idnodosNoRespuesta)
{
    bool r = false;
    list<Node<float, int>*> paraAgregar;

    for (list<Node<float, int>*>::iterator it = buscador->edges.begin(); it != buscador->edges.end(); ++it)
    {
        if (isInsideListB((*it)->x, (*it)->y, listaTemporalDeBusqueda)) {
            continue;
        }
        if (isInsideListB((*it)->x, (*it)->y, listaNodosEliminados)) {
            continue;
        }
        if (idnodosNoRespuesta[(*it)->id]) {
            continue;
        }
        (*it)->nodeFather = buscador;
        paraAgregar.push_back((*it));
        r = true;
    }
    if (r)
    {
        paraAgregar.sort(ordenamientoHillClimbing);

        for (list<Node<float, int>*>::iterator it = paraAgregar.begin(); it != paraAgregar.end(); ++it)
        {
            listaTemporalDeBusqueda.push_front((*it));
        }
    }
    return r;
}
bool tieneHijosPorAgregarYAgregarBPP(Node<float, int>* buscador, list<Node<float, int>*>& listaTemporalDeBusqueda, list<Node<float, int>*>& listaNodosEliminados, int* idnodosNoRespuesta)
{
    bool r = false;
    list<Node<float, int>*> paraAgregar;
    for (list<Node<float, int>*>::iterator it = buscador->edges.begin(); it != buscador->edges.end(); ++it)
    {
        if (isInsideListB((*it)->x, (*it)->y, listaTemporalDeBusqueda))
            continue;
        if (isInsideListB((*it)->x, (*it)->y, listaNodosEliminados))
            continue;
        if (idnodosNoRespuesta[(*it)->id])
            continue;

        listaTemporalDeBusqueda.push_front((*it));
        r = true;
    }

    return r;
}
void printListAst(list<Node<float, int>*> lista)
{
    for (list<Node<float, int>*>::iterator it = lista.begin(); it != lista.end(); ++it)
    {
        cout << " id:" << (*it)->id << " (" << (*it)->x << "," << (*it)->y << ") " << endl;
    }
}
void printList(list<Node<float, int>*> lista)
{
    for (list<Node<float, int>*>::iterator it = lista.begin(); it != lista.end(); ++it)
    {
        cout << " id:" << (*it)->id << " (" << (*it)->x << "," << (*it)->y << ") " << endl;
    }
}
void printListOnlyId(list<Node<float, int>*> lista)
{
    for (list<Node<float, int>*>::iterator it = lista.begin(); it != lista.end(); ++it)
    {
        cout << " id:" << (*it)->id << " ";
    }
    cout << endl;
}
void printconsola(int sx, int sy, int ex, int ey, list<Node<float, int>*> listaNodosEliminados, list<Node<float, int>*> listaNodosRespuesta)
{
    int matriz[SIZE_MAP][SIZE_MAP]{0};
    for (int x = 0; x < SIZE_MAP; x++)
    {
        for (int y = 0; y < SIZE_MAP; y++)
        {
            matriz[x][y] = 0;
        }
    }
    for (list<Node<float, int>*>::iterator it = listaNodosEliminados.begin(); it != listaNodosEliminados.end(); ++it)
    {
        matriz[(*it)->x][(*it)->y] = -1;
    }
    int a = 1;
    for (list<Node<float, int>*>::iterator it = listaNodosRespuesta.begin(); it != listaNodosRespuesta.end(); ++it, ++a)
    {
        matriz[(*it)->x][(*it)->y] = a;
    }

    //matriz[sx][sy] = 1;
    //matriz[ex][ey] = 'F';
    for (int x = 0; x < SIZE_MAP; x++)
    {
        for (int y = 0; y < SIZE_MAP; y++)
        {
            cout << setw(SIZE_PRINT) << matriz[x][y];
        }
        cout << endl;
    }
}
//FUNCIONES

//ALGORITMOS
void algoritmoAAsterisco(Coordenada<int> coordenadaPartida, Coordenada<int> coordenadaLlegada, list<Node<float, int>*> &listaNodosEliminados, Graph<float, int> grafo)
{
    cout << "START THREAD A*" << endl;
    list<Node<float, int>*> listaTemporalDeBusqueda;
    list<Node<float, int>*> listaNodosRespuesta;
    int idnodosNoRespuesta[40400]{0};

    bool existCamino = false;
    Node<float, int>** nodoInicial;
    if (!grafo.exist(coordenadaPartida.x, coordenadaPartida.y, nodoInicial)) // NODO DE PARTIDA NO EXISTE
    {
        cout << "NODO INICIAL NO EXISTE" << endl;
        return;
    }

    listaTemporalDeBusqueda.push_front((*nodoInicial));
    Node<float, int>* buscador;

    while (listaTemporalDeBusqueda.size() > 0)
    {
        buscador = (*listaTemporalDeBusqueda.begin());
        if (!((listaNodosRespuesta.size() > 0) && ((buscador->x == (*listaNodosRespuesta.rbegin())->x) && (buscador->y == (*listaNodosRespuesta.rbegin())->y))))
        {
            listaNodosRespuesta.push_back(buscador);
        }// EVITAR DOBLE INGRESO DE ULTIMO VALOR
        
        if (buscador->x == coordenadaLlegada.x && buscador->y == coordenadaLlegada.y)
        {
            existCamino = true;
            break;
        }
        else
        {
            if (!tieneHijosPorAgregarYAgregarAAST(buscador, listaTemporalDeBusqueda, listaNodosEliminados, idnodosNoRespuesta))
            {
                listaNodosRespuesta.pop_back();
                listaTemporalDeBusqueda.pop_front();
                idnodosNoRespuesta[buscador->id] = 1;
            }
        }
    }
    //printconsola(coordenadaPartida.x, coordenadaPartida.y, coordenadaLlegada.x, coordenadaLlegada.y, listaNodosEliminados, listaNodosRespuesta);
    if (!existCamino) cout << "A* -> NO HAY SOLUCION" << endl;
    else cout << "A* -> SI HAY SOLUCION" << endl;

    //OPENGL COPIANDO NODOS RESPUESTA
    /*int i = 0;
    for (list<Node<float, int>*>::iterator it = listaNodosRespuesta.begin(); it != listaNodosRespuesta.end(); ++it, i++)
    {
        figures[9][i] = (*it)->id;
    }*/
    //OPENGL COPIANDO NODOS RESPUESTA

    ////OPENGL COPIANDO NODOS RESPUESTA LINEA
    int i = 0;
    list<Node<float, int>*>::iterator it = listaNodosRespuesta.begin();
    figures[9][i] = (*it)->id;
    //cout << "(" << figures[9][0]<<",";
    it++; i++;
    for (; it != listaNodosRespuesta.end(); ++it, i+=2)
    {
        figures[9][i] = (*it)->id;
        //cout << "" << figures[9][i] << ")" << endl;;
        figures[9][i+1] = (*it)->id;
        //cout << "(" << figures[9][i+1] << ",";

    }
    figures[9][i] = figures[9][i-1];
    ////OPENGL COPIANDO NODOS RESPUESTA LINEA
}
void algoritmoMejorHillClimbing(Coordenada<int> coordenadaPartida, Coordenada<int> coordenadaLlegada, list<Node<float, int>*> listaNodosEliminados, Graph<float, int> grafo)
{
    cout << "START THREAD HILL CLIMBING" << endl;
    list<Node<float, int>*> listaTemporalDeBusqueda;
    list<Node<float, int>*> listaNodosRespuesta;
    int idnodosNoRespuesta[40400]{ 0 };
    bool existCamino = false;
    Node<float, int>** nodoInicial;
    if (!grafo.exist(coordenadaPartida.x, coordenadaPartida.y, nodoInicial)) // NODO DE PARTIDA NO EXISTE
    {
        cout << "NODO INICIAL NO EXISTE" << endl;
        return;
    }

    listaTemporalDeBusqueda.push_front((*nodoInicial));
    Node<float, int>* buscador;

    while (listaTemporalDeBusqueda.size() > 0)
    {
        buscador = (*listaTemporalDeBusqueda.begin());

        if (!((listaNodosRespuesta.size() > 0) && ((buscador->x == (*listaNodosRespuesta.rbegin())->x) && (buscador->y == (*listaNodosRespuesta.rbegin())->y))))
        {
            listaNodosRespuesta.push_back(buscador);
        }
        if (buscador->x == coordenadaLlegada.x && buscador->y == coordenadaLlegada.y)
        {
            existCamino = true;
            break;
        }
        else
        {
            if (!tieneHijosPorAgregarYAgregarHillClimbing(buscador, listaTemporalDeBusqueda, listaNodosEliminados, idnodosNoRespuesta))
            {
                listaNodosRespuesta.pop_back();
                listaTemporalDeBusqueda.pop_front();
                idnodosNoRespuesta[buscador->id] = 1;
            }

        }
    }
    //printconsola(coordenadaPartida.x, coordenadaPartida.y, coordenadaLlegada.x, coordenadaLlegada.y, listaNodosEliminados, listaNodosRespuesta);
    if (!existCamino)
        cout << "HILL CLIMBING -> NO HAY SOLUCION" << endl;
    else
    {
        cout << "HILL CLIMBING -> SI HAY SOLUCION" << endl;
    }

    //OPENGL COPIANDO NODOS RESPUESTA
   /* int i = 0;
    for (list<Node<float, int>*>::iterator it = listaNodosRespuesta.begin(); it != listaNodosRespuesta.end(); ++it, i++)
    {
        figures[7][i] = (*it)->id;
    }*/
    //OPENGL COPIANDO NODOS RESPUESTA

    ////OPENGL COPIANDO NODOS RESPUESTA LINEA
    int i = 0;
    list<Node<float, int>*>::iterator it = listaNodosRespuesta.begin();
    figures[7][i] = (*it)->id;
    //cout << "(" << figures[7][0] << ",";
    it++; i++;
    for (; it != listaNodosRespuesta.end(); ++it, i += 2)
    {
        figures[7][i] = (*it)->id;
        //cout << "" << figures[7][i] << ")" << endl;;
        figures[7][i + 1] = (*it)->id;
        //cout << "(" << figures[7][i + 1] << ",";

    }
    figures[7][i] = figures[7][i - 1];
    ////OPENGL COPIANDO NODOS RESPUESTA LINEA
}
void algoritmoPrimeroElMejor(Coordenada<int> coordenadaPartida, Coordenada<int> coordenadaLlegada, list<Node<float, int>*> listaNodosEliminados, Graph<float, int> grafo) {
    cout << "START THREAD MEJOR EL PRIMERO" << endl;
    list<Node<float, int>*> listaNodosRespuesta;
    list<list<Node<float, int>*>> listaTemporalDeBusqueda;
    int idnodosNoRespuesta[40400]{ 0 };
    bool existCamino = false;

    Node<float, int>** nodot;
    list<Node<float, int>*> listPrimeraEnLista;
    Node<float, int>* nodoPrimeroEnLista;

    if (grafo.exist(coordenadaPartida.x, coordenadaPartida.y, nodot))
    {
        listPrimeraEnLista.push_back((*nodot));
    }
    listaTemporalDeBusqueda.push_back(listPrimeraEnLista);

    while (listaTemporalDeBusqueda.size() > 0)
    {
        listPrimeraEnLista = *listaTemporalDeBusqueda.begin();
        if ((*listPrimeraEnLista.rbegin())->x == coordenadaLlegada.x && (*listPrimeraEnLista.rbegin())->y == coordenadaLlegada.y)
        {
            listaNodosRespuesta = listPrimeraEnLista;
            existCamino = true;
            break;
        }
        else
        {
            list<Node<float, int>*> listTemporal = listPrimeraEnLista;
            //printListOnlyId(listTemporal);
            //cout << listaTemporalDeBusqueda.size() << endl;

            nodoPrimeroEnLista = *listTemporal.rbegin();
            listaTemporalDeBusqueda.pop_front();
            for (list<Node<float, int>*>::iterator it = nodoPrimeroEnLista->edges.begin(); it != nodoPrimeroEnLista->edges.end(); ++it)
            {
                if (isInsideListB((*it)->x, (*it)->y, listTemporal) || isInsideListB((*it)->x, (*it)->y, listaNodosEliminados))
                {
                    continue;
                }
                list<Node<float, int>*> listTemporal1 = listTemporal;
                listTemporal1.push_back(*it);
                listaTemporalDeBusqueda.push_back(listTemporal1);
            }
            if (listaTemporalDeBusqueda.size() > 0)
            {
                list<list<Node<float, int>*>>::iterator mejorCamino = listaTemporalDeBusqueda.begin();
                list<list<Node<float, int>*>>::iterator paraEliminar = listaTemporalDeBusqueda.begin();
                for (list<list<Node<float, int>*>>::iterator it = listaTemporalDeBusqueda.begin(); it != listaTemporalDeBusqueda.end(); ++it)
                {
                    if (((*(*it).rbegin())->distanceToObjective) < (*(*mejorCamino).rbegin())->distanceToObjective)
                    {
                        mejorCamino = it;
                        paraEliminar = it;
                    }
                }
                //cout << "mejor camino " << (*(*mejorCamino).rbegin())->distanceToObjective << " " << (*(*mejorCamino).rbegin())->id << endl;
                //cout << "para eliminar " << (*(*paraEliminar).rbegin())->distanceToObjective << " " << (*(*mejorCamino).rbegin())->id << endl;
                listaTemporalDeBusqueda.push_front(*mejorCamino);
                listaTemporalDeBusqueda.erase(paraEliminar);
            }
        }
    }

    //printconsola(coordenadaPartida.x, coordenadaPartida.y, coordenadaLlegada.x, coordenadaLlegada.y, listaNodosEliminados, listaNodosRespuesta);
   
    if (!existCamino)
        cout << "MEJOR EL PRIMERO -> NO HAY SOLUCION" << endl;
    else
    {
        cout << "MEJOR EL PRIMERO -> SI HAY SOLUCION" << endl;
    }

    //OPENGL COPIANDO NODOS RESPUESTA
    /*int i = 0;
    for (list<Node<float, int>*>::iterator it = listaNodosRespuesta.begin(); it != listaNodosRespuesta.end(); ++it, i++)
    {
        figures[8][i] = (*it)->id;
    }*/
    //OPENGL COPIANDO NODOS RESPUESTA

    ////OPENGL COPIANDO NODOS RESPUESTA LINEA
    int i = 0;
    list<Node<float, int>*>::iterator it = listaNodosRespuesta.begin();
    figures[8][i] = (*it)->id;
    //cout << "(" << figures[8][0] << ",";
    it++; i++;
    for (; it != listaNodosRespuesta.end(); ++it, i += 2)
    {
        figures[8][i] = (*it)->id;
        //cout << "" << figures[8][i] << ")" << endl;;
        figures[8][i + 1] = (*it)->id;
        //cout << "(" << figures[8][i + 1] << ",";

    }
    figures[8][i] = figures[8][i - 1];
    ////OPENGL COPIANDO NODOS RESPUESTA LINEA
}

void algoritmoBusquedaProfundidad(Coordenada<int> coordenadaPartida, Coordenada<int> coordenadaLlegada, list<Node<float, int>*> listaNodosEliminados, Graph<float, int> grafo)
{
    cout << "START THREAD POR PROFUNDIDAD" << endl;
    list<Node<float, int>*> listaTemporalDeBusqueda;
    list<Node<float, int>*> listaNodosRespuesta;
    int idnodosNoRespuesta[40400]{ 0 };
    bool existCamino = false;
    Node<float, int>** nodoInicial;
    if (!grafo.exist(coordenadaPartida.x, coordenadaPartida.y, nodoInicial)) // NODO DE PARTIDA NO EXISTE
    {
        cout << "NODO INICIAL NO EXISTE" << endl;
        return;
    }

    listaTemporalDeBusqueda.push_front((*nodoInicial));
    Node<float, int>* buscador;

    while (listaTemporalDeBusqueda.size() != 0)
    {
        buscador = (*listaTemporalDeBusqueda.begin());
        if (!((listaNodosRespuesta.size() != 0) && ((buscador->x == (*listaNodosRespuesta.rbegin())->x) && (buscador->y == (*listaNodosRespuesta.rbegin())->y))))
        {
            listaNodosRespuesta.push_back(buscador);
        }

        if (buscador->x == coordenadaLlegada.x && buscador->y == coordenadaLlegada.y)
        {
            existCamino = true;
            break;
        }
        else
        {
            if (!tieneHijosPorAgregarYAgregarBPP(buscador, listaTemporalDeBusqueda, listaNodosEliminados, idnodosNoRespuesta))
            {
                listaNodosRespuesta.pop_back();
                listaTemporalDeBusqueda.pop_front();
                idnodosNoRespuesta[buscador->id] = 1;
            }
        }
    }
    //printconsola(coordenadaPartida.x, coordenadaPartida.y, coordenadaLlegada.x, coordenadaLlegada.y, listaNodosEliminados, listaNodosRespuesta);
    
    if (!existCamino)
        cout << "PROFUNDIDAD -> NO HAY SOLUCION" << endl;
    else
    {
        cout << "PROFUNDIDAD -> SI HAY SOLUCION" << endl;
    }
    //OPENGL COPIANDO NODOS RESPUESTA PUNTOS
   /* int i = 0;
    for (list<Node<float, int>*>::iterator it = listaNodosRespuesta.begin(); it != listaNodosRespuesta.end(); ++it, i++)
    {
        figures[4][i] = (*it)->id;
    }*/
    //OPENGL COPIANDO NODOS RESPUESTA PUNTOS

    ////OPENGL COPIANDO NODOS RESPUESTA LINEA
    int i = 0;
    list<Node<float, int>*>::iterator it = listaNodosRespuesta.begin();
    figures[4][i] = (*it)->id;
    //cout << "(" << figures[4][0]<<",";
    it++; i++;
    for (; it != listaNodosRespuesta.end(); ++it, i += 2)
    {
        figures[4][i] = (*it)->id;
        //cout << "" << figures[4][i] << ")" << endl;;
        figures[4][i + 1] = (*it)->id;
        //cout << "(" << figures[4][i+1] << ",";

    }
    figures[4][i] = figures[4][i - 1];
    ////OPENGL COPIANDO NODOS RESPUESTA LINEA

}
void algoritmoBusquedaAmplitud(Coordenada<int> coordenadaPartida, Coordenada<int> coordenadaLlegada, list<Node<float, int>*> listaNodosEliminados, Graph<float, int> grafo)
{
    
    cout << "START THREAD BUSQUEDA AMPLITUD" << endl;
    list<Node<float, int>*> listaNodosRespuesta;
    list<list<Node<float, int>*>> listaTemporalDeBusqueda;
    int idnodosNoRespuesta[40400]{0};
    bool existCamino = false;

    Node<float, int>** nodot;
    list<Node<float, int>*> listPrimeraEnLista;
    Node<float, int>* nodoPrimeroEnLista;

    if (grafo.exist(coordenadaPartida.x, coordenadaPartida.y, nodot))
    {
        listPrimeraEnLista.push_back((*nodot));
    }
    listaTemporalDeBusqueda.push_back(listPrimeraEnLista);

    for (list<list<Node<float, int>*>>::iterator it = listaTemporalDeBusqueda.begin(); it != listaTemporalDeBusqueda.end(); ++it)
    {
        listPrimeraEnLista = *it;
        if ((*listPrimeraEnLista.rbegin())->x == coordenadaLlegada.x && (*listPrimeraEnLista.rbegin())->y == coordenadaLlegada.y)
        {
            listaNodosRespuesta = listPrimeraEnLista;
            existCamino = true;
            break;
        }
        else
        {
            nodoPrimeroEnLista = *listPrimeraEnLista.rbegin();
            for (list<Node<float, int>*>::iterator it = nodoPrimeroEnLista->edges.begin(); it != nodoPrimeroEnLista->edges.end(); ++it)
            {
                //if (isInsideListB((*it)->x, (*it)->y, listPrimeraEnLista) || isInsideListB((*it)->x, (*it)->y, listaNodosEliminados)) // TAL CUAL CLASES
                if (isInsideListB((*it)->x, (*it)->y, listPrimeraEnLista) || isInsideListB((*it)->x, (*it)->y, listaNodosEliminados) || idnodosNoRespuesta[(*it)->id]) // MEJORADO
                {
                    continue;
                }
                list<Node<float, int>*> listTemporal = listPrimeraEnLista;
                listTemporal.push_back(*it);
                listaTemporalDeBusqueda.push_back(listTemporal);
                idnodosNoRespuesta[(*it)->id] = 1; //MEJORADO | QUITAR PARA QUE SEA STANDARD
            }
        }
    }

    //printconsola(coordenadaPartida.x, coordenadaPartida.y, coordenadaLlegada.x, coordenadaLlegada.y, listaNodosEliminados, listaNodosRespuesta);
    
    if (!existCamino)
        cout << "AMPLITUD -> NO HAY SOLUCION" << endl;
    else
    {
        cout << "AMPLITUD -> SI HAY SOLUCION" << endl;
    }

    //OPENGL COPIANDO NODOS RESPUESTA PUNTOS
    /*int i = 0;
    for (list<Node<float, int>*>::iterator it = listaNodosRespuesta.begin(); it != listaNodosRespuesta.end(); ++it, i++)
    {
        figures[5][i] = (*it)->id;
    }*/
    //OPENGL COPIANDO NODOS RESPUESTA PUNTOS

    ////OPENGL COPIANDO NODOS RESPUESTA LINEA
    int i = 0;
    list<Node<float, int>*>::iterator it = listaNodosRespuesta.begin();
    figures[5][i] = (*it)->id;
    //cout << "(" << figures[5][0]<<",";
    it++; i++;
    for (; it != listaNodosRespuesta.end(); ++it, i += 2)
    {
        figures[5][i] = (*it)->id;
        //cout << "" << figures[5][i] << ")" << endl;;
        figures[5][i + 1] = (*it)->id;
        //cout << "(" << figures[5][i+1] << ",";

    }
    figures[5][i] = figures[5][i - 1];
    ////OPENGL COPIANDO NODOS RESPUESTA LINEA
}
void algoritmoNoDeterministica(Coordenada<int> coordenadaPartida, Coordenada<int> coordenadaLlegada, list<Node<float, int>*> listaNodosEliminados, Graph<float, int> grafo)
{
    cout << "START THREAD NO DETERMINISTICA" << endl;
    list<Node<float, int>*> listaNodosRespuesta;
    list<list<Node<float, int>*>> listaTemporalDeBusqueda;
    bool existCamino = false;

    Node<float, int>** nodot;
    list<Node<float, int>*> listPrimeraEnLista;
    Node<float, int>* nodoPrimeroEnLista;

    if (grafo.exist(coordenadaPartida.x, coordenadaPartida.y, nodot))
    {
        listPrimeraEnLista.push_back((*nodot));
    }
    listaTemporalDeBusqueda.push_back(listPrimeraEnLista);

    while (listaTemporalDeBusqueda.size() != 0)
    {
        listPrimeraEnLista = *listaTemporalDeBusqueda.begin();
        if ((*listPrimeraEnLista.rbegin())->x == coordenadaLlegada.x && (*listPrimeraEnLista.rbegin())->y == coordenadaLlegada.y)
        {
            listaNodosRespuesta = listPrimeraEnLista;
            existCamino = true;
            break;
        }
        else
        {
            list<Node<float, int>*> listTemporal = listPrimeraEnLista;
            list<list<Node<float, int>*>> listaDeListasTemporal;
            nodoPrimeroEnLista = *listTemporal.rbegin();
            listaTemporalDeBusqueda.pop_front();
            for (list<Node<float, int>*>::iterator it = nodoPrimeroEnLista->edges.begin(); it != nodoPrimeroEnLista->edges.end(); ++it)
            {
                if (isInsideListB((*it)->x, (*it)->y, listTemporal) || isInsideListB((*it)->x, (*it)->y, listaNodosEliminados))
                {
                    continue;
                }
                list<Node<float, int>*> listTemporal1 = listTemporal;
                listTemporal1.push_back(*it);
                listaDeListasTemporal.push_front(listTemporal1);
            }
            /*
            //listaDeListasTemporal.sort(ordenamientoNoDeterministica);
            //random_shuffle(listaDeListasTemporal.begin(), listaDeListasTemporal.end());
            for (list<list<Node<float, int>*>>::iterator it = listaDeListasTemporal.begin(); it != listaDeListasTemporal.end(); ++it)
            {
                listaTemporalDeBusqueda.push_front(*it);
            }*/
            

            list<list<Node<float, int>*>>::iterator it = listaDeListasTemporal.begin();
            while (listaDeListasTemporal.size() > 0) {
                uniform_int_distribution<int> distribution(0, listaDeListasTemporal.size()-1);
                int random = distribution(rd);
                advance(it, random);
                listaTemporalDeBusqueda.push_front(*it);
                listaDeListasTemporal.erase(it);
                it = listaDeListasTemporal.begin();
            }

        }
    }
    
    //printconsola(coordenadaPartida.x, coordenadaPartida.y, coordenadaLlegada.x, coordenadaLlegada.y, listaNodosEliminados, listaNodosRespuesta);
    
    if (!existCamino)
        cout << "NO DETER -> NO HAY SOLUCION" << endl;
    else
    {
        cout << "NO DETER -> SI HAY SOLUCION" << endl;
    }

    //OPENGL COPIANDO NODOS RESPUESTA PUNTOS
    /*int i = 0;
    for (list<Node<float, int>*>::iterator it = listaNodosRespuesta.begin(); it != listaNodosRespuesta.end(); ++it, i++)
    {
        figures[6][i] = (*it)->id;
    }*/
    //OPENGL COPIANDO NODOS RESPUESTA PUNTOS

    ////OPENGL COPIANDO NODOS RESPUESTA LINEA
    int i = 0;
    list<Node<float, int>*>::iterator it = listaNodosRespuesta.begin();
    figures[6][i] = (*it)->id;
    //cout << "(" << figures[6][0]<<",";
    it++; i++;
    for (; it != listaNodosRespuesta.end(); ++it, i += 2)
    {
        figures[6][i] = (*it)->id;
        //cout << "" << figures[6][i] << ")" << endl;;
        figures[6][i + 1] = (*it)->id;
        //cout << "(" << figures[6][i+1] << ",";

    }
    figures[6][i] = figures[6][i - 1];
    ////OPENGL COPIANDO NODOS RESPUESTA LINEA
}
void algoritmoBusquedaProfundidadListasDeListas(Coordenada<int> coordenadaPartida, Coordenada<int> coordenadaLlegada, list<Node<float, int>*> listaNodosEliminados, Graph<float, int> grafo)
{
    //BUSQUEDA POR PROFUNDIDAD CON LISTAS DE LISTAS
    list<Node<float, int>*> listaNodosRespuesta;
    list<list<Node<float, int>*>> listaTemporalDeBusqueda;
    bool existCamino = false;

    Node<float, int>** nodot;
    list<Node<float, int>*> listPrimeraEnLista;
    Node<float, int>* nodoPrimeroEnLista;

    if (grafo.exist(coordenadaPartida.x, coordenadaPartida.y, nodot))
    {
        listPrimeraEnLista.push_back((*nodot));
    }
    listaTemporalDeBusqueda.push_back(listPrimeraEnLista);

    while (listaTemporalDeBusqueda.size() != 0)
    {
        listPrimeraEnLista = *listaTemporalDeBusqueda.begin();
        if ((*listPrimeraEnLista.rbegin())->x == coordenadaLlegada.x && (*listPrimeraEnLista.rbegin())->y == coordenadaLlegada.y)
        {
            listaNodosRespuesta = listPrimeraEnLista;
            existCamino = true;
            break;
        }
        else
        {
            list<Node<float, int>*> listTemporal = listPrimeraEnLista;
            nodoPrimeroEnLista = *listTemporal.rbegin();
            listaTemporalDeBusqueda.pop_front();
            for (list<Node<float, int>*>::iterator it = nodoPrimeroEnLista->edges.begin(); it != nodoPrimeroEnLista->edges.end(); ++it)
            {
                if (isInsideListB((*it)->x, (*it)->y, listTemporal) || isInsideListB((*it)->x, (*it)->y, listaNodosEliminados))
                {
                    continue;
                }
                list<Node<float, int>*> listTemporal1 = listTemporal;
                listTemporal1.push_back(*it);
                listaTemporalDeBusqueda.push_front(listTemporal1);
            }
        }
    }

    //printconsola(coordenadaPartida.x, coordenadaPartida.y, coordenadaLlegada.x, coordenadaLlegada.y, listaNodosEliminados, listaNodosRespuesta);
    
    if (!existCamino)
        cout << "PROFUNDIDAD L D LNO HAY SOLUCION" << endl;
    else
    {
        cout << "PROFUNDIDAD L D L SI HAY SOLUCION" << endl;
    }
    //OPENGL COPIANDO NODOS RESPUESTA
    /*int i = 0;
    for (list<Node<float, int>*>::iterator it = listaNodosRespuesta.begin(); it != listaNodosRespuesta.end(); ++it, i++)
    {
        figures[4][i] = (*it)->id;
    }*/
    //OPENGL COPIANDO NODOS RESPUESTA


}
//ALGORITMOS


int main()
{

	/*
	*************** ALGORITMOS DE AI
	*/

    cout << "***************** ALGORITMOS DE BUSQUEDA *****************" << endl;
    cout << "*****************       CURSO: AI        *****************\n\n" << endl;

	Graph<float, int> grafo;
	list<Node<float, int>*> listaNodosEliminados;
	uniform_int_distribution<int> distribution(MIN_VALUE_COORD, MAX_VALUE_COORD);

	//INGRESAR VALORES A PUNTO PARTIDA Y FINAL
	int coordsPartidaLlegadaAleatorias;
	cout << "[0] encerrar llegada [3] encerrar salida [1] generar aleatorio [2] ingresar manualmente" << endl;
	cin >> coordsPartidaLlegadaAleatorias;
	//INGRESAR VALORES A PUNTO PARTIDA Y FINAL

	//GENERAR PARTIDA Y LLEGADA ALEATORIA
	Coordenada<int> coordenadaPartida(0, 0);
	Coordenada<int> coordenadaLlegada(0, 0);
	if (coordsPartidaLlegadaAleatorias == 1)
	{
		coordenadaPartida.set(distribution(rd), distribution(rd));
		coordenadaLlegada.set(distribution(rd), distribution(rd));
	}
	if (coordsPartidaLlegadaAleatorias == 0)
	{
		coordenadaPartida.set(SIZE_MAP - 1, SIZE_MAP - 1);
		coordenadaLlegada.set(0, 0);
	}
    if (coordsPartidaLlegadaAleatorias == 3)
    {
        coordenadaLlegada.set(SIZE_MAP - 1, SIZE_MAP - 1);
        coordenadaPartida.set(0, 0);
    }
	if (coordsPartidaLlegadaAleatorias == 2)
	{

		int x, y;
		cout << "Coordenada Partida x | y" << endl;
		cin >> x >> y;
		coordenadaPartida.set(x, y);

		cout << "Coordenada Llegada x | y" << endl;
		cin >> x >> y;
		coordenadaLlegada.set(x, y);
	}
	//GENERAR PARTIDA Y LLEGADA ALEATORIA

    // OPENGL COORDENADAS INICIAL Y FINAL
    figures[1][0] = MOVER_DIBUJO + (coordenadaPartida.x * AGRANDAR_ALTO) + (coordenadaPartida.y* AGRANDAR_ANCHO);
    //cout << "partida id: " << MOVER_DIBUJO + (coordenadaPartida.x * AGRANDAR_ALTO) + coordenadaPartida.y<<endl;
    figures[2][0]= MOVER_DIBUJO + (coordenadaLlegada.x * AGRANDAR_ALTO) + (coordenadaLlegada.y * AGRANDAR_ANCHO);
    //cout << "llegada id: " << MOVER_DIBUJO + (coordenadaLlegada.x * AGRANDAR_ALTO) + coordenadaLlegada.y << endl;
    // OPENGL COORDENADAS INICIAL Y FINAL 

	//INSERTING NODES.
	for (int i = 0; i < SIZE_MAP; i++)
	{
		for (int j = 0; j < SIZE_MAP; j++)
		{
            Node<float, int>* tmp = new Node<float, int>(i, j, MOVER_DIBUJO + (i * AGRANDAR_ALTO) + (j* AGRANDAR_ANCHO), distanceBetweenCoordinates(i, j, coordenadaLlegada.x, coordenadaLlegada.y));
			grafo.insertNode(tmp);
		}
	}
	//INSERTING NODES

	//CREATING DELETING NODES
	int ingresoManualParedes;
	cout << "[0] encerrado [1] generar aleatorio [2] ingresar manualmente" << endl;
	cin >> ingresoManualParedes;

	if (ingresoManualParedes == 0)
	{
		Node<float, int>** nodo;
		grafo.exist(0, 1, nodo);
		listaNodosEliminados.push_back((*nodo));
		grafo.exist(1, 0, nodo);
		listaNodosEliminados.push_back((*nodo));
		grafo.exist(1, 1, nodo);
		listaNodosEliminados.push_back((*nodo));
	}
	if (ingresoManualParedes == 2)
	{
		int x, y;
		Node<float, int>** nodo;
		while (ingresoManualParedes)
		{
			cout << "Coordenada pared x | y" << endl;
			cin >> x >> y;
			if (grafo.exist(x, y, nodo))
			{
				listaNodosEliminados.push_back((*nodo));
			}
			else
			{
				cout << "NO DEBERIA IMPRIMIR ESTO" << endl;
			}
			cout << "[2] para ingresar otra pared [0] salir" << endl;
			cin >> ingresoManualParedes;
		}
	}
	int tmpx, tmpy;
	Node<float, int>** nodo;
	while (listaNodosEliminados.size() < NODES_TO_ELIMINATE)
	{
		tmpx = distribution(rd);
		tmpy = distribution(rd);
		if ((isInsideListB(tmpx, tmpy, listaNodosEliminados)) || (tmpx == coordenadaLlegada.x && tmpy == coordenadaLlegada.y) || (tmpx == coordenadaPartida.x && tmpy == coordenadaPartida.y))
			continue;
		if (grafo.exist(tmpx, tmpy, nodo))
		{
			listaNodosEliminados.push_back((*nodo));
		}
		else
		{
			cout << "NO DEBERIA IMPRIMIR ESTO" << endl;
		}
	}
	//CREATING DELETING NODES

    // OPENGL INSERTANDO NODOS ELIMINADOS
    int i = 0;
    for (list<Node<float, int>*>::iterator it = listaNodosEliminados.begin(); it != listaNodosEliminados.end(); ++it, i++) {
        figures[3][i] = (*it)->id;
    }
    // OPENGL INSERTANDO NODOS ELIMINADOS

	//INSERTING EDGES
	for (list<Node<float, int>*>::iterator it = grafo.nodes.begin(); it != grafo.nodes.end(); ++it)
	{
		Node<float, int>** p;
		/*
		A B C
		D X E
		F G H
		*/
		if (grafo.exist(((*it)->x) - 1, ((*it)->y) - 1, p))
			(*it)->edges.push_back(*p); // A
		if (grafo.exist(((*it)->x) - 1, ((*it)->y), p))
			(*it)->edges.push_back(*p); // B
		if (grafo.exist(((*it)->x) - 1, ((*it)->y) + 1, p))
			(*it)->edges.push_back(*p); // C
		if (grafo.exist(((*it)->x), ((*it)->y) - 1, p))
			(*it)->edges.push_back(*p); // D
		if (grafo.exist(((*it)->x), ((*it)->y) + 1, p))
			(*it)->edges.push_back(*p); // E
		if (grafo.exist(((*it)->x) + 1, ((*it)->y) - 1, p))
			(*it)->edges.push_back(*p); // F
		if (grafo.exist(((*it)->x) + 1, ((*it)->y), p))
			(*it)->edges.push_back(*p); // G
		if (grafo.exist(((*it)->x) + 1, ((*it)->y) + 1, p))
			(*it)->edges.push_back(*p); // H
	}
	//INSERTING EDGES

    //MOSTRAR COORDENADA PARTIDA Y LLEGADA
    cout << "NODOS PARTIDA (" << coordenadaPartida.x << " , " << coordenadaPartida.y << ")" << endl;
    cout << "NODOS LLEGADA (" << coordenadaLlegada.x << " , " << coordenadaLlegada.y << ")" << endl;
    //MOSTRAR COORDENADA PARTIDA Y LLEGADA

    cout << "------------------------------------------------------------------" << endl;
    cout << "running threads ... this could be take some minutes... please wait\n" << endl;

    thread threadBusquedaAmplitud(algoritmoBusquedaAmplitud, coordenadaPartida, coordenadaLlegada, listaNodosEliminados, grafo);
    thread threadBusquedaProfundidad(algoritmoBusquedaProfundidad, coordenadaPartida, coordenadaLlegada, listaNodosEliminados, grafo);
    //thread threadNoDeterministica(algoritmoNoDeterministica, coordenadaPartida, coordenadaLlegada, listaNodosEliminados, grafo);
    
    thread threadHillClimbing(algoritmoMejorHillClimbing, coordenadaPartida, coordenadaLlegada, listaNodosEliminados, grafo);
    thread threadMejorElPrimero(algoritmoPrimeroElMejor, coordenadaPartida, coordenadaLlegada, listaNodosEliminados, grafo);
    thread threadAAsterisco(algoritmoAAsterisco, coordenadaPartida, coordenadaLlegada, listaNodosEliminados, grafo);
	
	//algoritmoBusquedaProfundidadListasDeListas(coordenadaPartida, coordenadaLlegada, listaNodosEliminados, grafo);
	
    /*algoritmoBusquedaProfundidad(coordenadaPartida, coordenadaLlegada, listaNodosEliminados, grafo);
	algoritmoBusquedaAmplitud(coordenadaPartida, coordenadaLlegada, listaNodosEliminados, grafo);
	algoritmoNoDeterministica(coordenadaPartida, coordenadaLlegada, listaNodosEliminados, grafo);
	algoritmoMejorHillClimbing(coordenadaPartida,coordenadaLlegada,listaNodosEliminados,grafo);
	algoritmoPrimeroElMejor(coordenadaPartida, coordenadaLlegada, listaNodosEliminados, grafo);
    algoritmoAAsterisco(coordenadaPartida, coordenadaLlegada, listaNodosEliminados, grafo);*/

    threadBusquedaProfundidad.join();
    threadBusquedaAmplitud.join();
    //threadNoDeterministica.join();
    
    threadHillClimbing.join();
    threadMejorElPrimero.join();
    threadAAsterisco.join();
	/*
	*************** ALGORITMOS DE AI
	*/
	
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Artificial Intelligence Search Algorithms", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	buildingShaders();

	startVertexInput();

    glfwSetKeyCallback(window, key_callback);

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(VAO);

        // -------------CODE FROM HERE

		//CAMINOS DISPONIBLES
		/*glUseProgram(colorblack4);
		glEnable(GL_LINE_SMOOTH);
		glEnable(GL_LINE_WIDTH);
		glLineWidth(1);
		drawWithEBO(0, MV, GL_LINES);*/
		//CAMINOS DISPONIBLES

        //PAREDES
        glUseProgram(colorblack1);
        glEnable(GL_POINT_SMOOTH);
        glEnable(GL_POINT_SIZE);
        glPointSize(4);
        drawWithEBO(3, MV, GL_POINTS);
        //PAREDES

		


        switch (ALGORITMO_TYPE)
        {
        case algoritmo::AMPLITUD:
            //RESPUESTA BUSQUEDA AMPLITUD
            glUseProgram(colorblue1);
            glEnable(GL_LINE_SMOOTH);
            glEnable(GL_LINE_WIDTH);
            glLineWidth(3);
            drawWithEBO(5, MV, GL_LINES);
            //RESPUESTA BUSQUEDA AMPLITUD
            break;
        case algoritmo::PROFUNDIDAD:
            //RESPUESTA BUSQUEDA PROFUNDIDAD
            glUseProgram(coloryellow1);
            glEnable(GL_LINE_SMOOTH);
            glEnable(GL_LINE_WIDTH);
            glLineWidth(3);
            drawWithEBO(4, MV, GL_LINES);
            //RESPUESTA BUSQUEDA PROFUNDIDAD

            break;
        case algoritmo::NODETERMINISTICO:
            //RESPUESTA NO DETERMINISTICO
            glUseProgram(colorred1);
            glEnable(GL_LINE_SMOOTH);
            glEnable(GL_LINE_WIDTH);
            glLineWidth(3);
            drawWithEBO(6, MV, GL_LINES);
            //RESPUESTA NO DETERMINISTICO
            break;
        case algoritmo::HILLCLIMBING:
            //RESPUESTA HILL CLIMBING
            glUseProgram(colorwater1);
            glEnable(GL_LINE_SMOOTH);
            glEnable(GL_LINE_WIDTH);
            glLineWidth(3);
            drawWithEBO(7, MV, GL_LINES);
            //RESPUESTA HILL CLIMBING
            break;
        case algoritmo::MEJORPRIMERO:
            //RESPUESTA MEJOR EL PRIMERO
            glUseProgram(colorwater1);
            glEnable(GL_LINE_SMOOTH);
            glEnable(GL_LINE_WIDTH);
            glLineWidth(3);
            drawWithEBO(8, MV, GL_LINES);
            //RESPUESTA MEJOR EL PRIMERO
            break;
        case algoritmo::AASTERISCO:
            glUseProgram(colorpurple1);
            glEnable(GL_LINE_SMOOTH);
            glEnable(GL_LINE_WIDTH);
            glLineWidth(3);
            drawWithEBO(9, MV, GL_LINES);
            //RESPUESTA A ASTERISCO
            break;
        default:
            break;
        }
		

        //PUNTO PARTIDA
        glUseProgram(colorred1);
        glEnable(GL_POINT_SMOOTH);
        glEnable(GL_POINT_SIZE);
        glPointSize(6);
        drawWithEBO(1, 1, GL_POINTS);
        //PUNTO PARTIDA

        //PUNTO LLEGADA
        glUseProgram(colorgreen1);
        glEnable(GL_POINT_SMOOTH);
        glEnable(GL_POINT_SIZE);
        glPointSize(6);
        drawWithEBO(2, 1, GL_POINTS);
        //PUNTO LLEGADA
        //BORDE DEL MAPA
       /* glUseProgram(colorolive1);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_LINE_WIDTH);
        glLineWidth(1);
        drawWithEBO(10, MV, GL_LINES);*/
        //BORDE DEL MAPA

		
		// -------------CODE UNTIL HERE

		glBindVertexArray(0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(N, EBO);

	glDeleteProgram(colorred1);
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action,
    int mods) {
    if (key == GLFW_KEY_Z && action == GLFW_PRESS)
        ALGORITMO_TYPE = algoritmo::PROFUNDIDAD;

    if (key == GLFW_KEY_X && action == GLFW_PRESS)
        ALGORITMO_TYPE = algoritmo::AMPLITUD;

    if (key == GLFW_KEY_C && action == GLFW_PRESS)
        ALGORITMO_TYPE = algoritmo::NODETERMINISTICO;

    if (key == GLFW_KEY_V && action == GLFW_PRESS)
        ALGORITMO_TYPE = algoritmo::HILLCLIMBING;

    if (key == GLFW_KEY_B && action == GLFW_PRESS)
        ALGORITMO_TYPE = algoritmo::MEJORPRIMERO;

    if (key == GLFW_KEY_N && action == GLFW_PRESS)
        ALGORITMO_TYPE = algoritmo::AASTERISCO;
}

