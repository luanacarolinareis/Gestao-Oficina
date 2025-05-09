# Workshop Management

Sistema de Gestão de Reservas e Pré-Reservas para Oficinas.

O objetivo do **Workshop Management** é auxiliar os funcionários de uma oficina na administração eficiente das reservas e pré-reservas (fila de espera) para dois serviços principais: lavagem e manutenção de veículos. O sistema foi totalmente implementado em linguagem C, utilizando listas ligadas e filas para garantir flexibilidade, eficiência e organização das operações.

---

## ✨ Funcionalidades Principais

- **Gestão de Reservas**
  - Criação, listagem e cancelamento de reservas para lavagem ou manutenção.
  - Inserção ordenada por data e hora, evitando sobreposição de horários.

- **Gestão de Pré-Reservas (Fila de Espera)**
  - Inserção automática de pedidos em fila de espera caso não haja vaga disponível.
  - Promoção automática de pré-reservas para reservas quando surgem vagas.

- **Listagens Detalhadas**
  - Listagem de todas as reservas e pré-reservas (ordem cronológica).
  - Listagem das reservas e pré-reservas de um cliente específico (ordem inversa).

- **Gestão de Serviços Realizados**
  - Atualização do sistema após a realização de serviços, removendo reservas e pré-reservas antigas.

- **Pesquisa de Disponibilidade**
  - Visualização de todos os horários disponíveis para marcação num determinado dia.

- **Persistência de Dados**
  - Carregamento e gravação do estado atual das reservas e pré-reservas em ficheiros `.txt`.

- **Validação e Proteção de Dados**
  - Verificação rigorosa de nomes, datas e horários inseridos pelo utilizador.
  - Proteção contra introdução de informação inválida.

---

## 🧩 Estruturas de Dados

- **Listas Ligadas:**  
  Utilizadas para armazenar reservas, permitindo inserção e remoção eficientes e ordenação cronológica.

- **Filas (Queues):**  
  Utilizadas para gerir pré-reservas (fila de espera), garantindo atendimento por ordem de chegada (FIFO).

- **Struct Principal:**
typedef struct {
  char name;
  int service; // 0: Lavagem, 1: Manutenção
  Date date;
  Time time;
} Booking;

---

## 🛠️ Como Usar

1. **Clone o repositório:**

- git clone https://github.com/luanacarolinareis/Gestao-Oficina.git
- cd Gestao-Oficina

2. **Compile o projeto:**

- gcc main.c source.c linkedLists.c queues.c -o gestao_oficina

3. **Execute o programa:**

- ./gestao_oficina

---

## 📋 Menu de Funcionalidades

- [0] Encerrar o programa
- [1] Carregar informação
- [2] Gravar o estado atual das reservas
- [3] Reservar lavagem ou manutenção
- [4] Cancelar reserva
- [5] Cancelar pré-reserva
- [6] Listar todas as reservas e pré-reservas
- [7] Listar as reservas e pré-reservas de um cliente
- [8] Realizar uma lavagem ou manutenção
- [9] Listar as reservas disponíveis por dia

---

## 👩‍💻 Equipa

- [Carolina Reis](https://github.com/luanacarolinareis)
- Tiago Silva

---

## 📄 Documentação

- [Manual do Programador (PDF)](manual-do-programador.pdf)

---

## 📢 Contribuição

Sugestões e melhorias são bem-vindas!  
Abra uma issue ou envie um pull request para contribuir para este projeto.

---

Gestão de Oficina – Organização, eficiência e flexibilidade na gestão de serviços de oficina!

