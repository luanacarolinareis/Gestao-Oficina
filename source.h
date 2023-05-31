#ifndef SOURCE_H
#define SOURCE_H

/* Struct de data */
typedef struct {
    int day, month, year;
} Date;

/* Struct de horário */
typedef struct {
    int hour, minutes;
} Time;

/* Struct de uma reserva ou pré-reserva */
typedef struct {
    char name[50];
    int service;
    Date date;
    Time time;
} Booking;

/* Protótipos de funções relacionadas com o print de reservas e pré-reservas (gerais e de cliente) */

void checkName(char *name);  /* Verifica se o nome inserido é válido */
void capitalizeWords(char *name);  /* Capitaliza as letras de um nome (NOME PRÓPRIO + APELIDO) */
void printData(Booking *element);  /* Printa os dados de uma reserva ou pré-reserva */

/* Protótipos das funções relacionadas com a comparação de structs */

int dateCompare(Date first, Date second);  /* Compara duas datas */
int timeCompatibility(Time first, Time second, int duration);  /* Verifica se o tempo de fim de uma reserva é compatível com o início de outra */
int timeCompare(Time first, Time second);  /* Compara dois horários */

// -----------------------------------------------------------------------------------------------------

/* Estruturas relacionadas com as linked lists */
typedef struct noList {
    Booking itemList;
    struct noList *next;
} noList;

typedef noList *ptr_list;

/* Protótipos das funções relacionadas com as linked lists */

ptr_list createList();  /* A linked list é criada */
int emptyList(ptr_list list);  /* Verifica se a linked list está vazia */
ptr_list destroyList(ptr_list list, int check);  /* A linked list é destruída */
int removeItemList(ptr_list list, Booking key, int check);  /* Remoção de um elemento da linked list */
int searchItem(ptr_list list, Booking key, ptr_list *previous, ptr_list *current, int check);  /* Procurar um item na linked list (encontrar uma reserva no mesmo dia e horário) */
void searchClient(ptr_list list, char *name, int *n);  /* Procura e imprime todas as reservas de um cliente (por recursividade)*/
void printList(ptr_list list);  /* Printar a linked list */
int insertItemOrder(ptr_list list, Booking element);  /* Insere a nova reserva, por ordem, e retorna um inteiro que diz se foi ou não possível inserir */
int getListSize(ptr_list list);  /* Devolve o tamanho da linked list */

// -----------------------------------------------------------------------------------------------------

/* Estruturas relacionadas com as queues */
typedef struct noQueue {
    Booking itemQueue;
    struct noQueue *next;
} noQueue;

typedef struct typeQueue {
    noQueue *first;
    noQueue *last;
    int size;
} typeQueue;

/* Protótipos das funções relacionadas com as queues */

void createQueue (typeQueue *queue);  /* A queue é criada */
int emptyQueue(typeQueue *queue);  /* Verifica se a queue está vazia */
void destroyQueue(typeQueue *queue, int check);  /* A queue é destruída */
int removeItemQueue(typeQueue *queue, Booking key);  /* Remoção de um elemento em qualquer local da queue */
void removePastItems(typeQueue *queue, Booking key);  /* Remoção de todos os elementos anteriores a uma data e hora */
void addItem(typeQueue *queue, Booking element);  /* Adição de um elemento ao final da queue */
int comparator(const void *first, const void *second); /* Devolve inteiro positivo, negativo ou nulo, conforme o primeiro argumento é posterior, anterior ou igual ao segundo */
void printQueue(typeQueue *queue, char *name, int *n, int check);  /* Printa todas as pré-reservas ou as pré-reservas de um cliente */
int toRes(ptr_list list, typeQueue *queue, Booking key); /* Quando uma reserva é cancelada, envia uma pré-reserva das queues para as linked lists (passa a reserva) */

// -----------------------------------------------------------------------------------------------------

/* Protótipos das funções relacionadas com as funcionalidades a implementar */

void reserve(Booking element, ptr_list list, typeQueue *queue, int *validate);  /* Reservar um serviço */
void preReserve(Booking element, typeQueue *queue);  /* Pré-reservar um serviço */
void cancelRes(ptr_list list, typeQueue *queue, Booking key);  /* Cancelar uma reserva */
void cancelPreRes(typeQueue *queue, Booking key);  /* Cancelar uma pré-reserva */
void listingRes(ptr_list list, typeQueue *queue, int check);  /* Listar todas as reservas e pré-reservas (+ antigas primeiro) */
void listingClient(ptr_list list, typeQueue *queue, int check);  /* Listar as reservas e pré-reservas de um cliente (+ recentes primeiro) */
void updateServices(typeQueue *queue, ptr_list list, Booking key);  /* Dar update aos serviços já realizados (realizar uma lavagem ou manutenção) */

/* Protótipos de funções relacionadas com o armazenamento e carregamento de dados */

void checkFile(char fileName[]);  /* Verificar o início do nome de um ficheiro */
void loadInfo(char fileName[], ptr_list list, typeQueue *queue);  /* Carregar a informação de um ficheiro .txt */
void saveInfo(char fileName[], ptr_list list, typeQueue *queue);  /* Gravar o estado atual das reservas num ficheiro .txt */

#endif // SOURCE_H
