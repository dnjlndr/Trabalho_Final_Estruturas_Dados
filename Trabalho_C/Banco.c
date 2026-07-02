#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 10

// --- Estruturas ---
typedef struct Cliente {
    int senha;
    char tipo; // 'N' ou 'P'
    char horario[20];
} Cliente;

// Para o modo extra
typedef struct No {
    Cliente dado;
    struct No* proximo;
} No;

// --- Filas Estáticas ---
Cliente fila_normal[MAX];
int n_inicio = 0, n_fim = 0, n_total = 0;

Cliente fila_pref[MAX];
int p_inicio = 0, p_fim = 0, p_total = 0;

int ultima_senha = 0;
int cont_normal = 0;

// --- Funções Auxiliares ---
void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void obter_horario(char* buffer) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buffer, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
}

// --- Operações de Fila Circular Estática (Aritmética de Módulo) ---
int enfileirar_normal(Cliente c) {
    if (n_total >= MAX) return 0;
    fila_normal[n_fim] = c;
    n_fim = (n_fim + 1) % MAX;
    n_total++;
    return 1;
}

int desenfileirar_normal(Cliente* c) {
    if (n_total == 0) return 0;
    *c = fila_normal[n_inicio];
    n_inicio = (n_inicio + 1) % MAX;
    n_total--;
    return 1;
}

int enfileirar_pref(Cliente c) {
    if (p_total >= MAX) return 0;
    fila_pref[p_fim] = c;
    p_fim = (p_fim + 1) % MAX;
    p_total++;
    return 1;
}

int desenfileirar_pref(Cliente* c) {
    if (p_total == 0) return 0;
    *c = fila_pref[p_inicio];
    p_inicio = (p_inicio + 1) % MAX;
    p_total--;
    return 1;
}

// --- Persistência CSV ---
void salvar_csv() {
    FILE* f = fopen("atendimento.csv", "w");
    if (!f) return;
    fprintf(f, "senha;tipo;horario\n");
    
    // Salva Normal
    int i = n_inicio;
    for (int count = 0; count < n_total; count++) {
        fprintf(f, "%d;%c;%s\n", fila_normal[i].senha, fila_normal[i].tipo, fila_normal[i].horario);
        i = (i + 1) % MAX;
    }
    // Salva Preferencial
    i = p_inicio;
    for (int count = 0; count < p_total; count++) {
        fprintf(f, "%d;%c;%s\n", fila_pref[i].senha, fila_pref[i].tipo, fila_pref[i].horario);
        i = (i + 1) % MAX;
    }
    fclose(f);
}

void carregar_csv() {
    FILE* f = fopen("atendimento.csv", "r");
    if (!f) return;
    char linha[256];
    fgets(linha, sizeof(linha), f);
    while (fgets(linha, sizeof(linha), f)) {
        Cliente c;
        char* token = strtok(linha, ";");
        if (!token) continue;
        c.senha = atoi(token);
        if (c.senha > ultima_senha) ultima_senha = c.senha;
        
        token = strtok(NULL, ";");
        if (!token) continue;
        c.tipo = token[0];
        
        token = strtok(NULL, "\n");
        if (!token) continue;
        strncpy(c.horario, token, 19); c.horario[19] = '\0';
        
        if (c.tipo == 'N') enfileirar_normal(c);
        else enfileirar_pref(c);
    }
    fclose(f);
}

// --- Modo Extra: Fila Circular Dinâmica ---
void modo_dinamico() {
    printf("\n[MODO EXTRA] Demonstracao de Fila Circular Dinamica\n");
    if (p_total == 0) { printf("Fila preferencial vazia.\n"); return; }
    
    No *inicio = NULL, *fim = NULL;
    int i = p_inicio;
    for (int count = 0; count < p_total; count++) {
        No* novo = (No*)malloc(sizeof(No));
        novo->dado = fila_pref[i];
        if (!inicio) {
            inicio = fim = novo;
            novo->proximo = inicio;
        } else {
            fim->proximo = novo;
            fim = novo;
            fim->proximo = inicio;
        }
        i = (i + 1) % MAX;
    }
    printf("Fila circular encadeada criada com %d elementos.\n", p_total);
    // Libera
    No* atual = inicio;
    for (int count = 0; count < p_total; count++) {
        No* temp = atual;
        atual = atual->proximo;
        free(temp);
    }
    printf("Memoria dinamica liberada.\n");
}

int main() {
    carregar_csv();
    int opcao;
    Cliente c;

    do {
        printf("\n--- BANCO (FILA CIRCULAR ESTATICA) ---\n");
        printf("1. Senha Normal\n");
        printf("2. Senha Preferencial\n");
        printf("3. Chamar Proximo (2N:1P)\n");
        printf("4. Listar Filas\n");
        printf("8. MODO DINAMICO (EXTRA)\n");
        printf("0. Sair\n");
        printf("Escolha: ");
        
        if (scanf("%d", &opcao) != 1) { limpar_buffer(); continue; }

        switch (opcao) {
            case 1:
                c.senha = ++ultima_senha; c.tipo = 'N'; obter_horario(c.horario);
                if (enfileirar_normal(c)) printf("Gerada: %dN\n", c.senha);
                else printf("Fila cheia!\n");
                break;
            case 2:
                c.senha = ++ultima_senha; c.tipo = 'P'; obter_horario(c.horario);
                if (enfileirar_pref(c)) printf("Gerada: %dP\n", c.senha);
                else printf("Fila cheia!\n");
                break;
            case 3: {
                int atendido = 0;
                if (cont_normal < 2 && n_total > 0) {
                    desenfileirar_normal(&c); cont_normal++; atendido = 1;
                } else if (p_total > 0) {
                    desenfileirar_pref(&c); cont_normal = 0; atendido = 1;
                } else if (n_total > 0) {
                    desenfileirar_normal(&c); atendido = 1;
                }
                if (atendido) printf("Atendendo: %d%c\n", c.senha, c.tipo);
                else printf("Ninguem na fila.\n");
                break;
            }
            case 4:
                printf("\n--- Filas ---\nNormal: %d | Preferencial: %d\n", n_total, p_total);
                break;
            case 8: modo_dinamico(); break;
            case 0: salvar_csv(); break;
        }
    } while (opcao != 0);

    return 0;
}
