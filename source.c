#include <stdio.h>
#include <string.h>
#include "source.h"

#define MAX_LENGTH 50  /* Tamanho máximo dos nomes de ficheiro e de cliente */

FILE *fptr;  /* Pointer para o início de um ficheiro */

/* Printa os dados de uma reserva ou pré-reserva */
void printData(Booking *element) {
    printf("\nNome do cliente: %s", element->name);
    printf("\nServiço [1- Lavagem ou 2- Manutenção]: %d\n", element->service);
    printf("Data [DD/MM/AAAA]: %d/%d/%d\n", element->date.day, element->date.month, element->date.year);
    printf("Horário [HH:MM]: %d:%d\n", element->time.hour, element->time.minutes);
    return;
}

/* Compara duas datas */
int dateCompare(Date first, Date second) {
    int firstForm, secondForm;
    firstForm = first.year * 10000 + first.month * 100 + first.day;  /* Formatação yyyymmdd */
    secondForm = second.year * 10000 + second.month * 100 + second.day;
    if (firstForm == secondForm) return 0;  /* Se as datas forem iguais devolve 0 */
    if (firstForm > secondForm) return -1;  /* Se o segundo for menor devolve -1 */
    return 1;  /* Se o segundo for maior devolve 1 */
}

 /* Verifica se o tempo de fim de uma reserva é compatível com o início de outra */
int timeCompatibility(Time first, Time second, int duration) {
    int firstMin, secondMin;
    firstMin = first.hour * 60 + first.minutes;
    secondMin = second.hour * 60 + second.minutes + duration;
    if (firstMin >= secondMin) return 1;  /* Devolve 1 se a hora de fim da anterior é compativel com a seguinte */
    return 0;
}

/* Compara dois horários */
int timeCompare(Time first, Time second){
    int firstMin, secondMin;
    firstMin = first.hour * 60 + first.minutes;
    secondMin = second.hour * 60 + second.minutes;
    if (firstMin == secondMin) return 0;  /* Se os horários forem iguais devolve 0 */
    if (firstMin > secondMin) return -1;  /* Se o segundo for menor devolve -1 */
    return 1;  /* Se o segundo for maior devolve 1 */
}

/* Reservar um serviço */
void reserve(Booking element, ptr_list list, typeQueue *queue) {
    char answer;
    ptr_list current, previous;
    
    if (!searchItem(list, element, &previous, &current, 0)) {  /* Verifica se o elemento já existe, tendo em conta o nome */
        if (insertItemOrder(list, element)) {  /* Verifica a compatibilidade de horário */
            printf("\n\e[1;32mReserva efetuada com sucesso!\e[0m\n");
        } else {
            printf("\n\e[1;33mDe momento, o horário desejado está ocupado.\e[0m\n");
            do {
                printf("Pretende fazer uma pré-reserva [S/N]? ");
                scanf("%c", &answer);
                while(getchar() != '\n');  /* Limpeza do buffer */
            } while (answer != 's' && answer != 'S' && answer != 'n' && answer != 'N');
            if (answer == 's' || answer == 'S') preReserve(element, queue);
        }
    } else {
        printf("\n\e[1;33mJá existe uma reserva nessa data e horário!\e[0m\n");
    }
    return;
}

/* Pré-reservar um serviço */
void preReserve(Booking element, typeQueue *queue) {
    addItem(queue, element);  /* Adicionar pré-reserva à queue */
    printf("\n\e[1;32mPré-reserva efetuada com sucesso!\e[0m\n");
    return;
}

/* Cancelar uma reserva */
void cancelRes(ptr_list list, typeQueue *queue, Booking key) {
    if(removeItem1(list, key, 0)) {
        printf("\n\e[1;32mElemento removido com sucesso!\e[0m\n"); /* Remover a pré-reserva da queue */
        if(toRes(list, queue, key)) printf("\n\e[1;32mElemento movido para reserva com sucesso!\e[0m\n");  /* Tenta inserir uma pré-reserva no lugar da reserva cancelada */
        else printf("Não foi movido nenhum elemento para reserva.\n");
    }
    else printf("\n\e[1;33mElemento não encontrado!\e[0m\n");
    return;
}

/* Cancelar uma pré-reserva */
void cancelPreRes(typeQueue *queue, Booking key) {
    if(removeItem2(queue, key)) printf("\n\e[1;32mElemento removido com sucesso!\e[0m\n"); /* Remover a pré-reserva da queue */
    else printf("\n\e[1;33mElemento não encontrado!\e[0m\n");
    return;
}

/* Listar todas as reservas e pré-reservas (+ antigas primeiro) */
void listingRes(ptr_list list, typeQueue *queue, int check) {
    int n;
    printf("\n\e[1;32mReservas »»\e[0m\n");
    if (!emptyList(list)) printList(list);
    else printf("\nNão existem reservas.\e[0m\n");

    printf("\n\e[1;32mPré-reservas »»\e[0m\n");
    if(!emptyQueue(queue)) printQueue(queue, "", &n, check);
    else printf("\nNão existem pré-reservas.\e[0m\n");
    return;
}

/* Listar as reservas e pré-reservas de um cliente (+ recentes primeiro) */
void listingClient(ptr_list list, typeQueue *queue, int check) {
    /* Pedir o nome do cliente */
    char name[MAX_LENGTH];
    int ctrl;
    do {
        ctrl = 0;
        int i = 0;
        printf("Nome do cliente: ");
        scanf("%49[^\n]s", name);
        while(getchar() != '\n');  /* Limpeza do buffer */
        while (name[i] != '\0') {
            if ( !((name[i] >= 'a' && name[i] <= 'z') || (name[i] >= 'A' && name[i] <= 'Z') || name[i] == ' ') ) {
                ctrl = 1;  /* Inserido caracter não suportado */
            }
            i++;
        }
    } while (ctrl == 1);

    int n = 0;
    printf("\n\e[1;32mReservas »»\e[0m\n");
    searchClient(list, name, &n);
    if (n == 0) printf("\nNão existem reservas com esse nome.\e[0m\n");

    printf("\n\e[1;32mPré-reservas »»\e[0m\n");
    n = 0;
    printQueue(queue, name, &n, check);
    if (n == 0) printf("\nNão existem pré-reservas com esse nome.\e[0m\n");
    return;
}

/* Dar update aos serviços já realizados (realizar uma lavagem ou manutenção) */
void updateServices(typeQueue *queue, ptr_list list, Booking key) {
    ptr_list previous, current;
    if (searchItem(list, key, &previous, &current, 0)) {
        removeItem1(list, key, 1); /* Remoção de todas as reservas anteriores à data e hora do serviço realizado (inclusivé) */
        removePastItems(queue, key);  /* Remoção de todas as pré-reservas anteriores à data e hora do serviço realizado (inclusivé) */
        printf("\n\e[1;32mServiço realizado com sucesso!\e[0m\n");
    } else {
        printf("\n\e[1;33mElemento não encontrado!\e[0m\n");
    }
    return;
}

/* Verificar o início do nome de um ficheiro */
void checkFile(char fileName[]) {
    char aux[5] = {}; /* Armazena os 4 caracteres iniciais do nome inserido pelo utilizador (para posterior comparação) */
    if (strlen(fileName) > 4) {
        for (int i = 0; i < 4; i++) {
            aux[i] = fileName[i];
        }
        /* Remoção dos prefixos "res_" ou "pre_", caso existam */
        if (!strcmp(aux,"res_") || !strcmp(aux, "pre_")) {
            printf("\n%s\n", aux);
            char *p = fileName + 4;
            char *temp = fileName;  /* Usada para armazenar o valor de *p antes de incrementar p */
            while (*p != '\0') {
                *temp++ = *p++;  /* Copia cada caracter da posição atual de p para a posição atual de temp e avança ambos os ponteiros para a próxima posição */
            }
            *temp = '\0';
        }
    }
    return;
}

/* Carregar a informação de um ficheiro .txt */
void loadInfo(char fileName[], ptr_list list, typeQueue *queue) {
    char prefix[50] = "data/res_";  /* Armazena um dos prefixos de ficheiro definidos */
    Booking element;
    int sizes;
    checkFile(fileName);  /* Verifica o nome do ficheiro dado */
    strcat(prefix, fileName);  /* Concatena o prefixo dado com o nome (já devidamente verificado) e guarda a alteração feita em 'prefix' */

    if ((fptr = fopen(prefix, "r")) == NULL) {  /* fopen("..\\res_t1.txt", "a") */
        printf("Error opening file!\n");
        return;
    }

    fscanf(fptr, "%d\n", &sizes);  /* Primeira coisa guardada foi o tamanho da lista de reservas, lê-o para saber quantas reservas tem que ler */
    for (int i = 0; i < sizes; ++i) {  /* Lê cada uma das reservas guardadas no ficheiro e insere-as na lista */
        fgets(element.name, 50, fptr);
        element.name[strcspn(element.name, "\n")] = 0;  /* fgets guarda na string o char \n, tem que ser removido */
        fscanf(fptr, "%d\n", &element.service);
        fscanf(fptr, "%d/%d/%d\n", &element.date.day, &element.date.month, &element.date.year);
        fscanf(fptr, "%d:%d\n\n", &element.time.hour, &element.time.minutes);
        insertItemOrder(list, element);
    }

    fclose(fptr);

    /* Mudança do prefixo */
    prefix[5]='p';
    prefix[6]='r';
    prefix[7]='e';

    if ((fptr = fopen(prefix, "r")) == NULL) {  /* fopen("..\\pre_t1.txt", "a") */
        printf("Error opening file!\n");
        return;
    }

    fscanf(fptr, "%d\n", &sizes);  /* Primeira coisa guardada foi o tamanho da fila de pré-reservas, lê-o para saber quantas pré-reservas tem que ler */
    for (int i = 0; i < sizes; ++i) {  /* Lê cada uma das pré-reservas guardadas no ficheiro e insere-as na fila */
        fgets(element.name, 50, fptr);
        element.name[strcspn(element.name, "\n")] = 0;  /* fgets guarda na string o char \n, tem que ser removido */
        fscanf(fptr, "%d\n", &element.service);
        fscanf(fptr, "%d/%d/%d\n", &element.date.day, &element.date.month, &element.date.year);
        fscanf(fptr, "%d:%d\n\n", &element.time.hour, &element.time.minutes);
        addItem(queue, element);
    }

    fclose(fptr);
}

/* Gravar o estado atual das reservas num ficheiro .txt */
void saveInfo(char fileName[], ptr_list list, typeQueue *queue) {
    char prefix[50] = "data/res_";  /* Armazena um dos prefixos de ficheiro definidos */
    ptr_list aux_list = list->next;
    noQueue *aux_queue = queue->first;
    checkFile(fileName);  /* Verifica o nome do ficheiro dado */
    strcat(prefix, fileName);  /* Concatena o prefixo dado com o nome (já devidamente verificado) e guarda a alteração feita em 'prefix' */
    
    if ((fptr = fopen(prefix, "w")) == NULL) {  /* fopen("..\\res_t1.txt", "a") */
        printf("Error opening file!\n");
        return;
    }

    fprintf(fptr,"%d\n",getListSize(list));
    while (aux_list) {
        fprintf(fptr, "%s\n%d\n", aux_list->itemList.name, aux_list->itemList.service);
        fprintf(fptr, "%d/%d/%d\n", aux_list->itemList.date.day, aux_list->itemList.date.month, aux_list->itemList.date.year);
        fprintf(fptr, "%d:%d\n\n", aux_list->itemList.time.hour, aux_list->itemList.time.minutes);
        aux_list = aux_list->next;
    }

    fclose(fptr);

    /* Mudança do prefixo */
    prefix[5]='p';
    prefix[6]='r';
    prefix[7]='e';

    if ((fptr = fopen(prefix, "w")) == NULL) {  /* fopen("..\\pre_t1.txt", "a") */
        printf("Error opening file!\n");
        return;
    }

    fprintf(fptr,"%d\n",queue->size);
    while (aux_queue) {
        fprintf(fptr, "%s\n%d\n", aux_queue->itemQueue.name, aux_queue->itemQueue.service);
        fprintf(fptr, "%d/%d/%d\n", aux_queue->itemQueue.date.day, aux_queue->itemQueue.date.month, aux_queue->itemQueue.date.year);
        fprintf(fptr, "%d:%d\n\n", aux_queue->itemQueue.time.hour, aux_queue->itemQueue.time.minutes);
        aux_queue = aux_queue->next;
    }
    fclose(fptr);
}
