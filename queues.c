#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "source.h"

/* A queue é criada */
void createQueue(typeQueue *queue) {
    queue->first = NULL;
    queue->last = NULL;
    queue->size = 0;
    return;
}

/* Verifica se a queue está vazia */
int emptyQueue(typeQueue *queue) {
    if (queue->first == NULL) return 1;
    else return 0;
}

/* A queue é destruída */
void destroyQueue(typeQueue *queue, int check) {
    noQueue *temp_ptr;
    while (!emptyQueue(queue)) {
        temp_ptr = queue->first;
        queue->first = queue->first->next;
        free(temp_ptr);  /* Libertação da memória alocada */
    }
    queue->size = 0;
    queue->last = NULL;
    if (check) free(queue);  /* Libertação da memória alocada */
    return;
}

/* Remoção de um elemento em qualquer local da queue */
int removeItemQueue(typeQueue *queue, Booking key) {
    noQueue *current = queue->first, *previous = queue->first;

    while (current) {  /* Percorre a queue */
        /* Verificação da igualdade das datas, dos horários e dos nomes */
        if ((dateCompare(current->itemQueue.date, key.date) == 0 && timeCompare(current->itemQueue.time, key.time) == 0 && !strcmp(current->itemQueue.name, key.name))) {
            /* Remoção da pré-reserva da queue*/
            if (current == previous) {  /* Remoção do 1º elemento da queue */
                queue->first = current->next;
                if (current->next == NULL) queue->last = current;
            } else {
                previous->next = current->next;                
                if (previous->next == NULL) queue->last = previous;
            } 
            free(current);  /* Libertação da memória alocada */
            (queue->size)--;
            return 1;         
        }
        /* Ainda não foi encontrado o elemento a remover */
        if (previous != current) previous = previous->next;
        current = current->next;
    }
    return 0;
}

/* Remoção de todos os elementos anteriores a uma data e hora */
void removePastItems(typeQueue *queue, Booking key) {
    noQueue *current = queue->first, *aux = NULL;

    while (current) {
        if ((dateCompare(key.date, current->itemQueue.date) == -1) || ((dateCompare(key.date, current->itemQueue.date) == 0) && (timeCompare(key.time, current->itemQueue.time) <= 0))) {
            aux = current->next;
            removeItemQueue(queue, current->itemQueue);  /* Chamada da função de remoção */
            current = aux;
        } else {
            current = current->next;
        }
    }
    return;
}

/* Adição de um elemento ao final da queue */
void addItem(typeQueue *queue, Booking element) {
    noQueue *temp_ptr;
    temp_ptr = (noQueue*) malloc (sizeof(noQueue));

    if (temp_ptr != NULL) {  /* Memória alocada */
        temp_ptr->itemQueue = element;
        temp_ptr->next = NULL;
        (queue->size)++;

        if (emptyQueue(queue)) queue->first = temp_ptr;
        else queue->last->next = temp_ptr;

        queue->last = temp_ptr;
    } 
    return; 
}

/* Devolve inteiro positivo, negativo ou nulo, conforme o primeiro argumento é posterior, anterior ou igual ao segundo */
int comparator(const void *first, const void *second) {
    noQueue **a = (noQueue**) first;
    noQueue **b = (noQueue**) second;
    int aux = dateCompare((*b)->itemQueue.date, (*a)->itemQueue.date);
    if (aux == 0) return timeCompare((*b)->itemQueue.time, (*a)->itemQueue.time);  /* Se a data for a mesma, devolve a diferença de tempo */
    return aux;  /* Caso contrário, devolve a diferença de datas */
}

/* Printa todas as pré-reservas ou as pré-reservas de um cliente */
void printQueue(typeQueue *queue, char *name, int *n, int check) {
    noQueue *aux = queue->first;
    noQueue **sortPtr = (noQueue**) malloc (sizeof(noQueue*) * queue->size);
    int i;

    for (i = 0; i < queue->size; i++) {
        sortPtr[i] = aux;  /* Cada elemento da array é um pointer para cada um dos elementos da queue */
        aux = aux->next;
    }

    qsort(sortPtr, queue->size, sizeof(noQueue*), comparator);  /* Organiza os pointers da array por ordem, com base na função de comparação "comparator" */

    if (check == 6) {  /* Selecionada a opção de listar todas */
        for (i = 0; i < queue->size; i++) {  /* Da + antiga para a + recente */
            printData(&(sortPtr[i]->itemQueue));
        }
    } else {  /* Selecionada a opção de listar as de um cliente específico */
        for (i = queue->size - 1; i >= 0; i--) {  /* Da + recente para a + antiga */
            if (!strcmp(name, sortPtr[i]->itemQueue.name)) {
                printData(&(sortPtr[i]->itemQueue));
                (*n)++;
            }
        }
    }
    free(sortPtr);  /* Libertação da memória alocada */
    return;
}

/* Quando uma reserva é cancelada, envia uma pré-reserva da queue para a linked list (passa a reserva) */
int toRes(ptr_list list, typeQueue *queue, Booking key) {
    if (emptyQueue(queue))  return 0;  /* Não há pré-reservas */
    
    char choice, extra;
    noQueue *aux = queue->first;
    Booking element;

    while (aux) {  /* Percorre a queue */
        if (dateCompare(aux->itemQueue.date, key.date) == 0 && timeCompare(aux->itemQueue.time, key.time) == 0) {  /* Se for uma correspondência exata de data e hora */
            if (insertItemOrder(list, aux->itemQueue)) {  /* Tenta inserir e, se inserir com sucesso, pode sair da função, removendo a pré-reserva */
                printf("\n\e[1;32mPré-reserva com correspondência exata »»\e[0m\n");
                printData(&aux->itemQueue);
                removeItemQueue(queue, aux->itemQueue);  /* Remoção do elemento da queue, após adição na linked list */
                return 1;
            }
        }
        aux = aux->next;
    }
    /* Não foi encontrada nenhuma correspondência exata (primeiro da queue tem prioridade) */
    aux = queue->first;
    printf("Não há nenhuma correspondência exata com o horário cancelado ou não foi possível inseri-la.\n");

    while (aux) {  /* Se não há nenhuma correspondência exata ou se a correspondência exata não foi possível de ser inserida, volta a percorrer as pré-reservas, tendo prioridade as primeiras da queue */
        if (dateCompare(aux->itemQueue.date, key.date) != 0 || timeCompare(aux->itemQueue.time, key.time) != 0) {  /* Este processo apenas é feito para as correspondências não exatas */
            printf("\n\e[1;32mPré-reserva prioritária »»\e[0m\n");
            printData(&aux->itemQueue);
            printf("\n");
            do {  /* Pergunta se quer inserir a pré-reserva em que está atualmente */
                printf("Pretende inserir esta pré-reserva nas reservas? ");
                choice = getchar();
                if (choice != '\n') extra = getchar();  /* Auxilia a deteção da inserção de + do que 1 caracter */
                if (extra != '\n' || choice == '\n') {  /* Se + do que 1 caracter foi inserido (ou nenhum) */
                    if (choice != '\n') while (getchar() != '\n');  /* Limpeza do buffer */
                    choice = 'r';  /* Sair do ciclo (reset) */
                }
            } while (tolower(choice) != 's' && tolower(choice) != 'n');

            if (choice == 's' || choice == 'S') {  /* Se sim, tenta inseri-la */
                element = aux->itemQueue;  /* Copia para um elemento auxiliar */
                
                if (insertItemOrder(list, element)) {  /* Tenta inserir e, se inserir com sucesso, pode sair da função, removendo a pré-reserva */
                    removeItemQueue(queue, aux->itemQueue);  /* Remoção do elemento da queue, após adição na linked list */
                    return 1;
                }
                element.date = key.date;  /* Altera a data */
                if (insertItemOrder(list, element)) {  /* Tenta inserir e, se inserir com sucesso, pode sair da função, removendo a pré-reserva */
                    removeItemQueue(queue, aux->itemQueue);  /* Remoção do elemento da queue, após adição na linked list */
                    return 1;
                }
                element.time = key.time;  /* Altera o horário */
                if (insertItemOrder(list, element)) {  /* Tenta inserir e, se inserir com sucesso, pode sair da função, removendo a pré-reserva */
                    removeItemQueue(queue, aux->itemQueue);  /* Remoção do elemento da queue, após adição na linked list */
                    return 1;
                }
            }
        }
        aux = aux->next;
    }
    return 0;
}
