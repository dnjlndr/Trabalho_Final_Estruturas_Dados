# Relatório Técnico de Implementação

Este relatório descreve os aspectos teóricos, as decisões de modelagem e as soluções de contorno adotadas no desenvolvimento da suíte de programas em C. 
Visando cumprir rigorosamente as normas obrigatórias da avaliação, os três sistemas operam primariamente sobre estruturas estáticas simples. 

Os temas aplicados foram:
* **Trabalho A:** Controle de Estoque (Lista Linear Ordenada por Vetor).
* **Trabalho B:** Histórico de Navegação Web (Duas Pilhas Estáticas por Vetor).
* **Trabalho C:** Sistema de Atendimento Bancário (Fila Linear e Fila Circular Estática).

---

### Modelagem das Estruturas e Decisões de Projeto

Os programas foram arquitetados em um formato híbrido:

| Contexto | Implementação Estática | Abordagem Dinâmica |
| :--- | :--- | :--- |
| **Trabalho A** | Array fixo `estoque[100]` com busca sequencial e ordenação física por *shifting* no ato da inserção. | Geração sob demanda de uma Lista Dinamicamente Encadeada Ordenada. |
| **Trabalho B** | Arrays `historico[100]` e `futuro[100]` controlados por contadores inteiros de topo. | Cópia temporária da coleção estruturada para uma Pilha Encadeada Dinâmica. |
| **Trabalho C** | Fila Normal sequencial e Fila Preferencial mapeada com lógica circular através do operador de resto (`%`). | Mapeamento transitório para uma Fila Circular Encadeada Dinâmica. |

---

### Dificuldades Superadas e Soluções Aplicadas

#### Tratamento de Strings e Falha de Segmentação no CSV (Trabalho B)
* **Problema:** A leitura de strings compostas por espaços via terminal capturava caracteres invisíveis de nova linha (`\n`). Ao salvar no arquivo CSV, as linhas se rompiam, fazendo com que a função de fatiamento `strtok()` retornasse ponteiros nulos (`NULL`) na inicialização seguinte. Tentar ler um ponteiro nulo com `strcpy()` causava erro fatal de *Segmentation Fault*.
* **Solução:** Desenvolvimento da função de tratamento `remover_caracteres_invisiveis()`, que limpa as strings recebidas mantendo apenas os caracteres úteis da tabela ASCII imprimível (intervalo 32 a 126).

#### Sincronização dos Índices Cíclicos (Trabalho C)
* **Problema:** Controlar de forma estática o movimento circular da fila de prioridade sem permitir sobreposição de dados quando o ponteiro final atinge o limite do array (`MAX`).
* **Solução:** Aplicação matemática rigorosa da congruência modular: `fim = (fim + 1) % MAX`. Essa abordagem reutiliza as posições liberadas do início do vetor perfeitamente.

---

### Validação e Interface com o Usuário
* **Proteção contra Overflows:** Todas as operações que modificam coleções realizam testes prévios de integridade (ex: `total_produtos >= MAX`, `topo_hist < MAX - 1`), disparando alertas amigáveis caso as estruturas fiquem cheias ou vazias.
* **Menus Blindados:** Os inputs numéricos validam o retorno de leitura. Entradas espúrias limpam o fluxo de entrada através da função `limpar_buffer()`, garantindo que o programa continue em execução estável.
