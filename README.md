# Trabalho Prático - AV3

Este repositório contém a implementação dos três trabalhos práticos independentes desenvolvidos para a disciplina de Estruturas de Dados, utilizando a linguagem C.

## 👥 Integrantes
* **Nome do Aluno:** Danielle de Jesus Leandro, Jorge Luís da Silva Brito, João Pedro Vidal Massaranduba, Luan Macedo Santos
* **Turma:** Ciências da Computação, Turma B

---

## 📁 Organização do Repositório
O projeto está estruturado da seguinte forma:
* `Trabalho_A/`: Código e base de dados do Controle de Estoque.
* `Trabalho_B/`: Código e base de dados do Histórico Web.
* `Trabalho_C/`: Código e base de dados do Atendimento Bancário.
* `relatorio.md`: Documentação técnica contendo decisões de projeto e tratamentos.

---

## 🛠️ Resumo dos Trabalhos Implementados

### Trabalho A: Controle de Estoque
Lista Linear armazenada em vetor estático de `structs` (`estoque[MAX]`). O programa realiza a inserção fisicamente ordenada através do algoritmo de deslocamento (*shifting*) baseado na chave única (`codigo`), impedindo duplicatas. Conta com funções de busca, edição, exclusão lógica e persistência. Como funcionalidade extra, adicionamos a realização do espelhamento completo dos dados para uma Lista Simplesmente Encadeada Dinâmica na memória com o uso de `malloc` e `free`.

### Trabalho B: Histórico de Navegação Web
Duas Pilhas Estáticas baseadas em vetor (`historico` e `futuro`), controladas pelos índices de topo (`topo_hist` e `topo_fut`). O programa simula as ações reais de "Voltar" e "Avançar" de um navegador web, empilhando e desempilhando páginas contendo metadados de data/hora e títulos. O programa também transfere e gerencia temporariamente o fluxo do histórico dentro de uma Pilha Dinâmica Encadeada.

### Trabalho C: Sistema de Atendimento Bancário
Fila Normal Linear e Fila Preferencial **Circular** Estática baseadas em arrays. O programa gerencia a emissão de senhas e executa uma política de prioridade intercalada (chama até 2 senhas Normais para cada 1 Preferencial). A Fila Preferencial utiliza aritmética modular (`% MAX`) para reaproveitamento cíclico de memória. Também converte a estrutura estática em uma Fila Circular Encadeada Dinâmica com nós autorreferenciados.

---

## 🚀 Como Compilar e Executar

Abra o terminal na pasta raiz do projeto e execute os seguintes comandos para compilar e rodar cada sistema de forma independente:

```bash
cd Trabalho_A
gcc estoque.c -o estoque
./estoque
```
```bash
cd Trabalho_B
gcc historico.c -o historico
./historico
```
```bash
cd Trabalho_C
gcc banco.c -o banco
./banco
```
