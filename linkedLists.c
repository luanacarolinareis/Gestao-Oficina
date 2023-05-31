#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "source.h"

/* A linked list é criada */
ptr_list createList() {
    ptr_list aux;
    Booking p1 =  {"", 0, {0, 0, 0}, {0, 0}};
    aux = (ptr_list) malloc (sizeof(noList));
    if (aux != NULL) {
        aux->itemList = p1;
        aux->next = NULL;
    }
    return aux;  /* Devolução do pointer da linked list */
}

/* Verifica se a linked list está vazia */
int emptyList(ptr_list list) {
    if (list->next == NULL) return 1;
    else return 0;
}

/* A linked list é destruída */
ptr_list destroyList(ptr_list list, int check) {
    ptr_list temp_ptr, aux = list->next;
    while (aux) {
        temp_ptr = aux;
        aux = aux->next;
        free(temp_ptr);  /* Libertação da memória alocada */
    }
    list->next = NULL;
    if (check) free(list);  /* Libertação da memória alocada */
    return NULL;
}

/* Remoção de um elemento da linked list */
int removeItemList(ptr_list list, Booking key, int check) {
    ptr_list previous, current;  /* Current é um pointer para o elemento a remover */ 
    if (searchItem(list, key, &previous, &current, check)) { /* Procura o item que se quer remover */
        previous->next = current->next;  /* Remoção do elemento */
        free(current); /* Libertação da memória alocada */
        return 1;
    }
    return 0;
}

/* Procurar um item na linked list (encontrar uma reserva no mesmo dia e horário) */
int searchItem(ptr_list list, Booking key, ptr_list *previous, ptr_list *current, int check) {
    *previous = list;
    *current = list->next;

    while ((*current) != NULL && (dateCompare(key.date, (*current)->itemList.date) == -1) ) {  /* Percorre até data ser igual (ou fim) */
        if (check == 1) {  /* Encontrar e remover os elementos (updateServices) */
            (*previous)->next = (*current)->next;  /* Remoção do elemento */
            free(*current); /* Libertação da memória alocada */
            *current = (*previous)->next;
        } else {  /* Encontrar os elementos */
            *previous = *current;
            *current = (*current)->next;
        }
    }
    if ((*current) != NULL && dateCompare(key.date, (*current)->itemList.date) == 1) return 0;
    while ((*current) != NULL && (dateCompare(key.date, (*current)->itemList.date) == 0) && (timeCompare(key.time, (*current)->itemList.time) == -1) ) {  /* Percorre até hora ser igual e estiver na mesma data (ou fim) */
        if (check == 1) {  /* Encontrar e remover os elementos (updateServices) */
            (*previous)->next = (*current)->next;  /* Remoção do elemento */
            free(*current); /* Libertação da memória alocada */
            *current = (*previous)->next;
        } else {
            *previous = *current;
            *current = (*current)->next;
        }
    }
    if ((*current) != NULL && (dateCompare((*current)->itemList.date, key.date) == 0) && (timeCompare((*current)->itemList.time, key.time) == 0) && !strcmp((*current)->itemList.name, key.name)) return 1;  /* Item encontrado */
    return 0;  /* Não foi encontrado um item igual */
}

/* Procura e imprime todas as reservas de um cliente (por recursividade) */
void searchClient(ptr_list list, char *name, int *n) {
    ptr_list aux = list->next;  /* Salta o header */
    if (aux == NULL) return;
    searchClient(aux, name, n);
    if (!strcmp(name, aux->itemList.name)) {  /* Devolve 0 quando igual (strcmp) */
        printData(&(aux->itemList));
        (*n)++;
    }
    return;
}

/* Printar a linked list */
void printList(ptr_list list) {
    ptr_list aux = list->next; /* Salta o header */
    while (aux) {
        printData(&(aux->itemList));
        aux = aux->next;
    }
    return;
}

/* Insere a nova reserva, por ordem, e devolve um inteiro que diz se foi ou não possível inserir */
int insertItemOrder(ptr_list list, Booking element) {
    ptr_list new = (ptr_list) malloc (sizeof(noList));  /* Alocação de memória para o novo nó */
    ptr_list current, last;

    if (new == NULL) {
        printf("Memory not allocated.\n");
        exit(1);
    } 
    new->itemList = element;  /* Inicializa os dados do novo nó */
    new->next = NULL;

    if (list->next == NULL) {  /* Se a lista está vazia, então este é o primeiro nó */
        list->next = new;
        return 1;
    }
    current = list->next;
    last = list;

    while (current != NULL && (dateCompare(new->itemList.date,current->itemList.date) == -1)) {  /* Procura a posição de inserção */
        last = current;
        current = current->next;
    }
    while (current != NULL && (timeCompare(new->itemList.time,current->itemList.time) == -1) && (dateCompare(new->itemList.date,current->itemList.date) == 0)) {  /* Procura a posição de inserção */
        last = current;
        current = current->next;
    }
    if (last == list) {  /* Se o ponteiro para o elemento last for a cabeça da lista, insere no princípio */
        /* Verificar se tem espaço para inserir */

        /* Se a data do seguinte (se houver) é igual à nova */
        /* E o fim do novo não é compatível com o início do seguinte */
        if (current != NULL && (dateCompare(current->itemList.date,new->itemList.date) == 0) && !timeCompatibility(current->itemList.time, new->itemList.time, new->itemList.service * 30)) {  
            free(new);
            return 0;
        }
        list->next = new;
    } else {
        /* Verificar se tem espaço para inserir */

        /* Se a data do anterior é igual à nova */
        /* E o fim do anterior não é compatível com o início do novo */
        if ((dateCompare(new->itemList.date,last->itemList.date) == 0) && !timeCompatibility(new->itemList.time, last->itemList.time, last->itemList.service * 30)) {
            free(new);
            return 0;
        }

        /* Se a data do seguinte (se houver) é igual à nova */
        /* E o fim do novo não é compatível com o inicio do seguinte */
        if (current != NULL && (dateCompare(current->itemList.date,new->itemList.date) == 0) && !timeCompatibility(current->itemList.time, new->itemList.time, new->itemList.service * 30)) {
            free(new);
            return 0;
        }
        last->next = new;  /* Caso contrário, insere entre dois nós */
    }
    new->next = current; /* Em qualquer dos casos, atualiza o ponteiro */
    return 1;
}

/* Listar as reservas disponíveis num determinado dia */
int printAvailableTime(ptr_list list, Date *date) {
    ptr_list aux = list->next;  /* Salta o header */
    Time ini = {8, 0}, end = {18, 0};  /* 'ini' começa com o horário de início de funcionamento da oficina, 'end' tem o horário de fim */
    int check = 0, count = 0;  /* 'count' verifica se houve algum horário disponível */

    while (aux) {  /* Percorre a linked list */
        if (dateCompare(aux->itemList.date,*date) == 0) {  /* Ao encontrar uma reserva com a data igual à dada */
            if (timeCompare(aux->itemList.time, ini) == 0)  /* Se a reserva tiver tempo de início igual ao 'ini' atual */
                check = 1;  /* Não será printada, ou seja, vai ser saltado o intervalo de tempo dessa reserva */
            
            if (!check) {  /* Se for uma reserva com tempo de início depois do guardado em 'ini', vai ser printado o intervalo entre 'ini' e o início da reserva */
                printf("%d:%d -- %d:%d\n", ini.hour, ini.minutes, aux->itemList.time.hour, aux->itemList.time.minutes);
                count = 1;
            }
            check=0;

            /* 'ini' vai agora ser o tempo de fim da reserva atual */
            ini.hour = aux->itemList.time.hour;
            ini.minutes = aux->itemList.time.minutes + 30 * aux->itemList.service;

            timeFix(&ini);  /* Correção dos minutos e das horas de 'ini' */
        }
        if (dateCompare(aux->itemList.date, *date) == -1) /* Reserva com data superior, vai parar imediatamente de percorrer */
            break;
        aux = aux->next;
    }
    if (timeCompare(ini, end) != 0) {  /* Se o tempo de fim da última reserva daquele dia não for 18:00, é printado o último intervalo de tempo, desde 'ini' até às 18:00 */
        printf("%d:%d -- 18:00\n", ini.hour, ini.minutes);
        count = 1;
    }
    return count;
}


/* Devolve o tamanho da linked list */
int getListSize(ptr_list list) {
    ptr_list aux = list->next;
    int count = 0;
    while (aux) {
        count++;
        aux = aux->next;
    }
    return count;
}
