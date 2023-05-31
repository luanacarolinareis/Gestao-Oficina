#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "source.h"

#define MAX_LENGTH 50  /* Tamanho máximo dos nomes de ficheiro e de cliente */
#define DATE_LENGTH 11  /* Tamanho máximo de um formato de data */
#define TIME_LENGTH 6  /* Tamanho máximo de um formato de horário */

void printMenu();  /* Menu principal */
Booking askData(int check, int *validate, int isCancel);  /* Pedir os dados ao utilizador */

int is_leap_year(int year);  /* Verifica se o ano é bissexto */
int is_valid_date(int day, int month, int year);  /* Verifica a validade do formato da data */
struct tm systemTime();  /* Guarda a data e hora de sistema atual */
int check_date_validity(Booking *element, int isCancel);  /* Verifica se a data é posterior à data de sistema */
int check_time_validity(Booking *element, int validate, int isCancel);  /* Se a data de sistema coincidir com a dada, verifica se o horário é posterior ao horário de sistema */

int main() {
    printf("\n\e[0;34m»»»» GESTÃO DE RESERVAS DE OFICINA ««««\e[0m\n");

    /* Declaração de variáveis e pointers */
    int validate;
    char choice, extra;
    char fileName[MAX_LENGTH];
    char str_available_data[DATE_LENGTH];
    Booking available; 

    typeQueue *ptr_queue = (typeQueue *) malloc (sizeof(typeQueue));  /* Alocação de memória para o pointer da queue */
    ptr_list list = createList();  /* Alocação de memória para o pointer da linked list, juntamente com a sua criação */
    if (ptr_queue == NULL || list == NULL) {
        printf("Memória não alocada.\n");
        exit(1);
    }
    createQueue(ptr_queue);  /* Criação da queue */

    /* Menu e seleção */
    do {
        printMenu();
        choice = getchar();
        if (choice != '\n') extra = getchar();  /* Auxilia a deteção da inserção de + do que 1 caracter */
        if (extra != '\n' || choice == '\n') {  /* Se + do que 1 caracter foi inserido (ou nenhum) */
            printf(">$ O programa recebe 1 caracter! Tente novamente.\n");
            if (choice != '\n') while (getchar() != '\n');  /* Limpeza do buffer */
            choice = 'r';  /* Sair do ciclo (reset) */
        } else {  /* Senão... */
            switch (choice) {
                case '0':  /* Encerrar o programa */
                    printf("\e[1;32m>$ A encerrar o programa...\e[0m\n\n");
                    break;
                case '1':  /* Carregar informação */
                    printf("\e[1;32m>$ Nome do ficheiro [ex: t1.txt]: \e[0m ");
                    scanf("%45[^\n]s", fileName);
                    while (getchar() != '\n');  /* Limpeza do buffer */
                    loadInfo(fileName, list, ptr_queue);
                    break;
                case '2':  /* Gravar o estado atual das reservas */
                    printf("\e[1;32m>$ Nome do ficheiro [ex: t1.txt]: \e[0m");
                    scanf("%45[^\n]s", fileName);
                    while (getchar() != '\n');  /* Limpeza do buffer */
                    saveInfo(fileName, list, ptr_queue);
                    break;
                case '3':  /* Reservar lavagem ou manutenção */
                    reserve(askData(1, &validate, 0), list, ptr_queue, &validate);  /* O parâmetro 1 apenas auxilia no pedido do serviço ao utilizador */
                    break;
                case '4': /* Cancelar reserva */
                    cancelRes(list, ptr_queue, askData(0, &validate, 1));
                    break;
                case '5':  /* Cancelar pré-reserva */
                    cancelPreRes(ptr_queue, askData(0, &validate, 1));
                    break;
                case '6':  /* Listar todas as reservas e pré-reservas (+ antigas primeiro) */
                    listingRes(list, ptr_queue, 6);  /* O parâmetro 6 auxilia o print das reservas da + antiga para a mais recente */
                    break;
                case '7':  /* Listar as reservas e pré-reservas de um cliente (+ recentes primeiro) */
                    listingClient(list, ptr_queue, 7);  /* O parâmetro 7 auxilia o print das reservas da + recente para a mais antiga */
                    break;
                case '8':  /* Realizar uma lavagem ou manutenção */
                    updateServices(ptr_queue, list, askData(0, &validate, 1));
                    break;
                case '9':  /* Listar as reservas disponíveis num determinado dia */
                    do {
                        printf("Data [DD/MM/AAAA]: ");
                        scanf("%10[^\n]s", str_available_data);  /* Lê a entrada como uma string de até 10 caracteres */
                        while (getchar() != '\n');  /* Limpeza do buffer */
                    } while (sscanf(str_available_data, "%d/%d/%d", &available.date.day, &available.date.month, &available.date.year) != 3 || !(check_date_validity(&available, 0)));
                    printf("\n");
                    if (!printAvailableTime(list, &available.date)) printf("Não há horários disponíveis nesse dia.\n");
                    break;
                default:  /* Opção inválida */
                    printf("\e[1;32m>$ Opção inválida! Tente novamente.\e[0m\n");
                    break;
            }
        }
    } while (choice != '0');

    /* Libertação da memória alocada */
    destroyList(list, 1);
    destroyQueue(ptr_queue, 1);

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
    printf("\e[1;32m[ 9 ]\e[0m Listar as reservas disponíveis por dia\n");
    printf("\n\e[1;32m>$ OPÇÃO: \e[0m");
}

/* Pedir os dados ao utilizador */
Booking askData(int check, int *validate, int isCancel) {
    Booking element;
    char choice, extra;
    char *input = (char*) malloc (DATE_LENGTH * sizeof(char));

    checkName(element.name);  /* Pedir o nome do cliente */
    
    /* Só é necessário pedir o serviço desejado ao tentar reservar */
    if (check == 1) {
        /* Pedir o tipo de serviço desejado (lavagem: 30 minutos || manutenção: 1 hora) */
        do {
            printf("Serviço [1- Lavagem ou 2- Manutenção]: ");
            choice = getchar();
            if (choice != '\n') extra = getchar();  /* Auxilia a deteção da inserção de + do que 1 caracter */
            if (extra != '\n' || choice == '\n') {  /* Se + do que 1 caracter foi inserido (ou nenhum) */
                if (choice != '\n') while (getchar() != '\n');  /* Limpeza do buffer */
                choice = 'r';  /* Sair do ciclo (reset) */
            }
        } while (choice != '1' && choice != '2');
        element.service = choice - 48; /* '1' = 49 || '2' = 50 */
    }

    /* Pedir a data da reserva ou pré-reserva */
    do {
        printf("Data [DD/MM/AAAA]: ");
        scanf("%10[^\n]s", input);  /* Lê a entrada como uma string de até 10 caracteres */
        while (getchar() != '\n');  /* Limpeza do buffer */   
    } while (sscanf(input, "%d/%d/%d", &element.date.day, &element.date.month, &element.date.year) != 3 || !(*validate = check_date_validity(&element, isCancel)));

    /* Pedir a hora da reserva ou pré-reserva */
    input = realloc(input, TIME_LENGTH * sizeof(char));
    if (*validate != 3 || isCancel) {  /* Se a data corresponde à data de sistema e a oficina já fechou */
        do {
            printf("Horário [ex: 08:00 ou 08:30] (das 8h às 18h): ");
            scanf("%5[^\n]s", input);  /* Lê a entrada como uma string de até 5 caracteres */
            while (getchar() != '\n');  /* Limpeza do buffer */
        } while (sscanf(input, "%d:%d", &element.time.hour, &element.time.minutes) != 2 || element.time.hour < 8 || element.time.hour > 17 || (element.time.minutes != 0 && element.time.minutes != 30) || (element.time.hour == 17 && element.time.minutes == 30 && element.service == 2) || !check_time_validity(&element, *validate, isCancel)); 
    }
    free(input);
    return element;  /* Devolução da struct, já com os dados */
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

/* Guarda a data e hora de sistema atual */
struct tm systemTime() {
    time_t current_time;
    struct tm *local_time;

    /* Obtém a data atual de sistema */
    current_time = time(NULL);
    local_time = localtime(&current_time);

    local_time->tm_mon = local_time->tm_mon + 1;  /* Meses desde Janeiro - [0, 11] */
    local_time->tm_year = local_time->tm_year + 1900;  /* Anos desde 1900 */

    return *local_time;
}

/* Verifica se a data é posterior à data atual de sistema */
int check_date_validity(Booking *element, int isCancel) {
    struct tm system = systemTime();
    /* Se a data inserida for exatamente igual à de sistema */
    if (element->date.year == system.tm_year && element->date.month == system.tm_mon && element->date.day == system.tm_mday) {
        if (system.tm_hour >= 18 || (system.tm_hour == 17 && system.tm_min > 30)) return 3; /* No dia de sistema atual já não são aceites reservas (oficina fechada) */
        return 2;  /* Data igual à de sistema e a oficina está aberta */
    }
    if (is_valid_date(element->date.day, element->date.month, element->date.year) && (isCancel || (element->date.year > system.tm_year || (element->date.year == system.tm_year && (element->date.month > system.tm_mon || (element->date.month == system.tm_mon && element->date.day >= system.tm_mday))))))
        return 1;  /* Data válida */ 
    return 0;  /* Data inválida */
}

/* Se a data de sistema coincidir com a dada, verifica se o horário é posterior ao horário de sistema */
int check_time_validity(Booking *element, int validate, int isCancel) {
    struct tm system = systemTime();
    if (validate != 2) return 1;  /* Hora válida */

    /* Verifica se a hora fornecida é posterior ou igual à hora atual de sistema */
    if (isCancel || element->time.hour > system.tm_hour || (element->time.hour == system.tm_hour && element->time.minutes >= system.tm_min))
        return 1;  /* Hora válida */
    return 0;  /* Hora inválida */
}
