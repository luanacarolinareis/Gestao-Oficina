#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "source.h"

#define MAX_LENGTH 50  /* Tamanho máximo dos nomes de ficheiro e de cliente */

void printMenu();  /* Menu principal */
Booking askData(int check);  /* Pedir os dados ao utilizador */

int is_leap_year(int year);  /* Verifica se o ano é bissexto */
int is_valid_date(int day, int month, int year);  /* Verifica a validade do formato da data */
int check_date_validity(Booking element);  /* Verifica se a data é posterior à data atual de sistema */

int main() {
    printf("\n\e[0;34m»»»» GESTÃO DE RESERVAS DE OFICINA ««««\e[0m\n");

    /* Declaração de variáveis */
    char choice, extra;
    char *fileName = (char*) malloc (MAX_LENGTH * sizeof(char));  /* Alocação de memória para o pointer do nome de ficheiro */
    typeQueue *ptr_queue = (typeQueue *) malloc (sizeof(typeQueue));  /* Alocação de memória para o pointer da queue */
    ptr_list list = createList();  /* Alocação de memória para o pointer da linked list, juntamente com a sua criação */

    if (fileName == NULL || ptr_queue == NULL || list == NULL) {
        printf("Memory not allocated.\n");
        exit(1);
    }
    createQueue(ptr_queue);  /* Criação da queue */

    /* Menu e seleção */
    do {
        printMenu();
        choice = getchar();
        if(choice != '\n') extra = getchar();  /* Auxilia a deteção da inserção de + do que 1 caracter */
        if(extra != '\n' || choice == '\n') {  /* Se + do que 1 caracter foi inserido */
            printf("\e[1;32m>$ O programa recebe 1 caracter! Tente novamente.\e[0m\n");
            if (choice != '\n') while (getchar() != '\n');  /* Limpeza do buffer */
            choice = 'e';  /* Sair do ciclo */
        } else {  /* Senão... */
            switch (choice) {
                case '0':
                    printf("\e[1;32m>$ A encerrar o programa...\e[0m\n");
                    printf("\n");
                    break;
                case '1':
                    printf("\e[1;32m>$ Nome do ficheiro [ex: t1.txt]:\e[0m ");
                    scanf("%49[^\n]s", fileName);
                    while(getchar() != '\n');  /* Limpeza do buffer */
                    loadInfo(fileName, list, ptr_queue);
                    printf(">$ Informação carregada com sucesso.\n");
                    break;
                case '2':
                    printf("\e[1;32m>$ Nome do ficheiro [ex: t1.txt]:\e[0m ");
                    scanf("%49[^\n]s", fileName);
                    while(getchar() != '\n');  /* Limpeza do buffer */
                    saveInfo(fileName, list, ptr_queue);
                    printf(">$ Informação gravada com sucesso.\n");
                    break;
                case '3':
                    reserve(askData(1), list, ptr_queue); /* O parâmetro 1/0 apenas auxilia no pedido (ou não) do serviço ao utilizador */
                    break;
                case '4':
                    cancelRes(list, ptr_queue, askData(0));
                    break;
                case '5':
                    cancelPreRes(ptr_queue, askData(0));
                    break;
                case '6':
                    listingRes(list, ptr_queue, 6);
                    break;
                case '7':
                    listingClient(list, ptr_queue, 7);
                    break;
                case '8':
                    updateServices(ptr_queue, list, askData(0));
                    break;
                default:
                    printf("\e[1;32m>$ Opção inválida! Tente novamente.\e[0m\n");
                    break;
            }
        }
    } while (choice != '0');

    free(fileName);
    destroyList(list);
    destroyQueue(ptr_queue);
    return 0;
}

/* Menu principal */
void printMenu() {
    printf("\e[1;32m\n[ 0 ]\e[0m Encerrar o programa\n");
    printf("\e[1;32m[ 1 ]\e[0m Carregar informação\n");
    printf("\e[1;32m[ 2 ]\e[0m Gravar o estado atual das reservas\n");
    printf("\e[1;32m[ 3 ]\e[0m Reservar lavagem ou manutenção\n");
    printf("\e[1;32m[ 4 ]\e[0m Cancelar reserva\n");
    printf("\e[1;32m[ 5 ]\e[0m Cancelar pré-reserva\n");
    printf("\e[1;32m[ 6 ]\e[0m Listar todas as reservas e pré-reservas\n");
    printf("\e[1;32m[ 7 ]\e[0m Listar as reservas e pré-reservas de um cliente\n");
    printf("\e[1;32m[ 8 ]\e[0m Realizar uma lavagem ou manutenção\n");
    printf("\n\e[1;32m>$ OPÇÃO: \e[0m");
}

/* Pedir os dados ao utilizador */
Booking askData(int check) {
    Booking element;
    
    /* Pedir o nome do cliente */
    int ctrl;
    do {
        ctrl = 0;
        int i = 0;
        printf("Nome do cliente: ");
        scanf("%49[^\n]s", element.name);
        while(getchar() != '\n');  /* Limpeza do buffer */

        while (element.name[i] != '\0') {
            if ( !((element.name[i] >= 'a' && element.name[i] <= 'z') || (element.name[i] >= 'A' && element.name[i] <= 'Z') || element.name[i] == ' ') ) {
                ctrl = 1;  /* Inserido caracter não suportado */
            }
            i++;
        }
    } while (ctrl == 1);

    if (check == 1) {
        /* Pedir o tipo de serviço desejado */
        char opt[MAX_LENGTH];
        do {
            printf("Serviço [1- Lavagem ou 2- Manutenção]: ");
            scanf("%s", opt);
            element.service = atoi(opt);
        } while (element.service != 1 && element.service != 2);
    }

    /* Pedir a data da reserva ou pré-reserva */
    do {
        printf("Data [DD/MM/AAAA]: ");
        scanf("%d/%d/%d", &element.date.day, &element.date.month, &element.date.year);
    } while (!check_date_validity(element));
    
    /* Pedir a hora da reserva ou pré-reserva */
    do {
        printf("Horário [ex: 08:00 ou 08:30] (das 8h às 18h): ");
        scanf("%d:%d", &element.time.hour, &element.time.minutes);
    } while (element.time.hour < 8 || element.time.hour > 17 || (element.time.minutes != 0 && element.time.minutes != 30) || (element.time.hour == 17 && element.time.minutes == 30 && element.service == 2));

    while(getchar() != '\n');  /* Limpeza do buffer */
    return element;  /* Devolução da struct já com os dados */
}

/* Verifica se o ano é bissexto */
int is_leap_year(int year) {
    if (year % 4 == 0) {
        if (year % 100 == 0) {
            if (year % 400 == 0) return 1;
            return 0;
        }
        return 1;
    }
    return 0;
}

/* Verifica a validade do formato da data */
int is_valid_date(int day, int month, int year) {
    int max_day;  
    if (month < 1 || month > 12) return 0;
    switch (month) {
        case 2:
            if (is_leap_year(year)) max_day = 29;
            else max_day = 28;
            break;
        case 4:
        case 6:
        case 9:
        case 11:
            max_day = 30;
            break;
        default:
            max_day = 31;
            break;
    }
    if (day < 1 || day > max_day) return 0;
    return 1;
}

/* Verifica se a data é posterior à data atual de sistema */
int check_date_validity(Booking element) {
    time_t current_time;
    struct tm *local_time;

    /* Obtém a data atual de sistema */
    current_time = time(NULL);
    local_time = localtime(&current_time);

    /* Extrai o dia, mês e ano da data atual de sistema */
    int current_day = local_time->tm_mday;
    int current_month = local_time->tm_mon + 1;
    int current_year = local_time->tm_year + 1900;

    /* A data deve ser posterior ou igual à data atual de sistema */
    if (is_valid_date(element.date.day, element.date.month, element.date.year) && (element.date.year > current_year || (element.date.year == current_year && (element.date.month > current_month || (element.date.month == current_month && element.date.day >= current_day))))) {
        return 1;  /* Data válida */
    } else {
        return 0;  /* Data inválida */
    }
}
