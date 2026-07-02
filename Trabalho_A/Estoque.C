#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100

// --- Estruturas ---
typedef struct Produto {
    int codigo;
    char nome[50];
    int quantidade;
    float preco;
} Produto;

// Estrutura para o modo extra (alocação dinâmica)
typedef struct No {
    Produto dado;
    struct No* proximo;
} No;

// --- Variáveis Globais (Estrutura Estática) ---
Produto estoque[MAX];
int total_produtos = 0;

// --- Funções Auxiliares ---
void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void remover_newline(char* str) {
    size_t len = strlen(str);
    if (len > 0 && str[len - 1] == '\n') str[len - 1] = '\0';
}

// --- Funções do Modo Estático (Obrigatório) ---
int buscar_estatico(int codigo) {
    for (int i = 0; i < total_produtos; i++) {
        if (estoque[i].codigo == codigo) return i;
    }
    return -1;
}

void inserir_estatico(Produto p) {
    if (total_produtos >= MAX) {
        printf("Erro: Estoque cheio!\n");
        return;
    }
    if (buscar_estatico(p.codigo) != -1) {
        printf("Erro: Codigo %d ja existe!\n", p.codigo);
        return;
    }

    // Inserção ordenada com shifting (deslocamento)
    int i = total_produtos - 1;
    while (i >= 0 && estoque[i].codigo > p.codigo) {
        estoque[i + 1] = estoque[i];
        i--;
    }
    estoque[i + 1] = p;
    total_produtos++;
    printf("Produto inserido com sucesso (Estatico).\n");
}

void listar_estatico() {
    printf("\n--- Estoque (Modo Estatico) ---\n");
    if (total_produtos == 0) {
        printf("Estoque vazio.\n");
        return;
    }
    printf("ID     | Nome                | Qtd | Preco\n");
    for (int i = 0; i < total_produtos; i++) {
        printf("%-6d | %-19s | %-3d | R$ %.2f\n", 
               estoque[i].codigo, estoque[i].nome, estoque[i].quantidade, estoque[i].preco);
    }
}

void excluir_estatico(int codigo) {
    int pos = buscar_estatico(codigo);
    if (pos == -1) {
        printf("Erro: Produto nao encontrado.\n");
        return;
    }
    for (int i = pos; i < total_produtos - 1; i++) {
        estoque[i] = estoque[i + 1];
    }
    total_produtos--;
    printf("Produto removido com sucesso.\n");
}

// --- Persistência CSV ---
void salvar_csv() {
    FILE* f = fopen("estoque.csv", "w");
    if (!f) return;
    fprintf(f, "codigo;nome;quantidade;preco\n");
    for (int i = 0; i < total_produtos; i++) {
        fprintf(f, "%d;%s;%d;%.2f\n", 
                estoque[i].codigo, estoque[i].nome, estoque[i].quantidade, estoque[i].preco);
    }
    fclose(f);
}

void carregar_csv() {
    FILE* f = fopen("estoque.csv", "r");
    if (!f) return;
    char linha[256];
    fgets(linha, sizeof(linha), f); // Ignora cabeçalho
    while (fgets(linha, sizeof(linha), f)) {
        Produto p;
        char* token = strtok(linha, ";");
        if (!token) continue;
        p.codigo = atoi(token);
        
        token = strtok(NULL, ";");
        if (!token) continue;
        strncpy(p.nome, token, 49);
        p.nome[49] = '\0';
        
        token = strtok(NULL, ";");
        if (!token) continue;
        p.quantidade = atoi(token);
        
        token = strtok(NULL, "\n");
        if (!token) continue;
        p.preco = atof(token);
        
        inserir_estatico(p);
    }
    fclose(f);
}

// --- Modo Extra: Alocação Dinâmica (Opcional) ---
void modo_dinamico() {
    printf("\n[MODO EXTRA] Demonstracao de Alocacao Dinamica\n");
    No* lista = NULL;
    
    // Converte vetor para lista encadeada para demonstrar
    for (int i = 0; i < total_produtos; i++) {
        No* novo = (No*)malloc(sizeof(No));
        novo->dado = estoque[i];
        novo->proximo = lista;
        lista = novo;
    }
    
    printf("Lista encadeada criada em memoria com %d elementos.\n", total_produtos);
    
    // Libera memoria
    while (lista != NULL) {
        No* temp = lista;
        lista = lista->proximo;
        free(temp);
    }
    printf("Memoria dinamica liberada com sucesso.\n");
}

int main() {
    carregar_csv();
    int opcao, cod;

    do {
        printf("\n--- MENU ESTOQUE (ESTATICO) ---\n");
        printf("1. Inserir Produto\n");
        printf("2. Buscar Produto\n");
        printf("3. Listar Todos\n");
        printf("4. Excluir Produto\n");
        printf("5. Salvar CSV\n");
        printf("8. ATIVAR MODO DINAMICO (EXTRA)\n");
        printf("0. Sair\n");
        printf("Escolha: ");
        
        if (scanf("%d", &opcao) != 1) {
            limpar_buffer();
            continue;
        }

        switch (opcao) {
            case 1: {
                Produto p;
                printf("Codigo: "); scanf("%d", &p.codigo);
                printf("Nome: "); limpar_buffer(); fgets(p.nome, 50, stdin); remover_newline(p.nome);
                printf("Quantidade: "); scanf("%d", &p.quantidade);
                printf("Preco: "); scanf("%f", &p.preco);
                inserir_estatico(p);
                break;
            }
            case 2:
                printf("Codigo: "); scanf("%d", &cod);
                int pos = buscar_estatico(cod);
                if (pos != -1) printf("Encontrado: %s | Qtd: %d | R$ %.2f\n", estoque[pos].nome, estoque[pos].quantidade, estoque[pos].preco);
                else printf("Nao encontrado.\n");
                break;
            case 3: listar_estatico(); break;
            case 4: printf("Codigo: "); scanf("%d", &cod); excluir_estatico(cod); break;
            case 5: salvar_csv(); printf("Salvo!\n"); break;
            case 8: modo_dinamico(); break;
            case 0: salvar_csv(); break;
        }
    } while (opcao != 0);

    return 0;
}
