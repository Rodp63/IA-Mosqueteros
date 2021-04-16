#include <iostream>
#include <list>
#include <vector>

using namespace std;

template <class N, class E>
struct Edge;

template <class S>
struct Coordenada
{
    S x;
    S y;
    Coordenada(S _x, S _y)
    {
        x = _x;
        y = _y;
    }
    Coordenada()
    {
        x = 0;
        y = 0;
    }
    void set(S _x, S _y) {
        x = _x;
        y = _y;
    }
};

template <class N, class E>
struct Node
{
    int id = 0;
    E x = 0;
    E y = 0;
    N distanceToObjective = 0;
    Node<N, E>* nodeFather = NULL;
    //list<Edge<N, E> *> edges; // para que los edges tengan valores propios - mejor implementacion
    list<Node<N, E>*> edges; // implementacion simple, apunta directo al nodo posible siguiente.

    Node(E x1, E y1, E _id, N _distance)
    {
        x = x1;
        y = y1;
        id = _id;
        distanceToObjective = _distance;
    }
    Node(E x1, E y1)
    {
        x = x1;
        y = y1;
        id = 0;
        distanceToObjective = 0;
    }
};

template <class N, class E>
struct Edge
{
    E value;
    bool dir = 0; // 0 <-> | [1] [0]->[1] |
    Node<N, E>* nodes[2];
};

template <class N, class E>
struct Graph
{
    list<Node<N, E>*> nodes;

    bool insertNode(Node<N, E>* newNode)
    {
        nodes.push_back(newNode);
        return 1;
    }

    bool exist(E _x, E _y, Node<float, int>**& p)
    {
        for (list<Node<float, int>*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
        {
            if ((*it)->x == _x && (*it)->y == _y)
            {
                p = &(*it);
                return true;
            }
        }
        return false;
    }

    bool insertEdge(Node<N, E>* a, Node<N, E>* b, E value, bool distance)
    {
        return 1;
    }

    bool removeEdge(Node<N, E>* a, Node<N, E>* b, E value)
    {
        return 1;
    }

    bool removeNode(Node<N, E>* a)
    {
        return 1;
    }

    void printNodesWithChildren() // print: id:101 (1,1) ->[id:0 (0,0)] ->[id:1 (0,1)] ->[id:2 (0,2)] ->[id:100 (1,0)] ->[id:102 (1,2)] ->[id:200 (2,0)] ->[id:201 (2,1)] ->[id:202 (2,2)]
    {
        for (list<Node<float, int>*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
        {
            cout << " id:" << (*it)->id << " (" << (*it)->x << "," << (*it)->y << ") ";
            for (list<Node<float, int>*>::iterator itt = (*it)->edges.begin(); itt != (*it)->edges.end(); ++itt)
            {
                cout << "-> [id:" << (*itt)->id << " (" << (*itt)->x << "," << (*itt)->y << ") ] ";
            }
            cout << endl;
        }
    }

    void printNodesWithDetails() // print: id: 90 x:0 y:4 distance 1.65
    {
        for (list<Node<float, int>*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
        {
            cout << "id: " << (*it)->id << " x:" << (*it)->x << " y:" << (*it)->y << " distance:" << (*it)->distanceToObjective << endl;
        }
    }
};