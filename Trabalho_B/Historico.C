#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 100

// --- Estruturas ---
typedef struct Pagina {
    char url[100];
    char titulo[50];
    char dataHora[20];
} Pagina;

// Para o modo extra
typedef struct No {
    Pagina dado;
    struct No* proximo;
} No;

// --- Variáveis Globais (Pilha Estática) ---
Pagina historico[MAX];
int topo_hist = -1;

Pagina futuro[MAX];
int topo_fut = -1;

// --- Funções Auxiliares ---
void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void remover_caracteres_invisiveis(char* str) {
    int i, j = 0;
    char temp[256];
    for (i = 0; str[i] != '\0'; i++) {
        if (str[i] >= 32 && str[i] <= 126) {
            temp[j++] = str[i];
        }
    }
    temp[j] = '\0';
    strcpy(str, temp);
}

void obter_data_hora(char* buffer) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(buffer, "%02d/%02d/%d %02d:%02d", tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min);
}

// --- Operações de Pilha Estática ---
void push_hist(Pagina p) {
    if (topo_hist < MAX - 1) {
        historico[++topo_hist] = p;
    }
}

int pop_hist(Pagina* p) {
    if (topo_hist >= 0) {
        *p = historico[topo_hist--];
        return 1;
    }
    return 0;
}

void push_fut(Pagina p) {
    if (topo_fut < MAX - 1) {
        futuro[++topo_fut] = p;
    }
}

int pop_fut(Pagina* p) {
    if (topo_fut >= 0) {
        *p = futuro[topo_fut--];
        return 1;
    }
    return 0;
}

// --- Persistência CSV ---
void salvar_csv() {
    FILE* f = fopen("historico.csv", "w");
    if (!f) return;
    fprintf(f, "url;titulo;dataHora\n");
    for (int i = 0; i <= topo_hist; i++) {
        fprintf(f, "%s;%s;%s\n", historico[i].url, historico[i].titulo, historico[i].dataHora);
    }
    fclose(f);
}

void carregar_csv() {
    FILE* f = fopen("historico.csv", "r");
    if (!f) return;
    char linha[256];
    if (!fgets(linha, sizeof(linha), f)) { fclose(f); return; }
    
    while (fgets(linha, sizeof(linha), f)) {
        Pagina p;
        char* token = strtok(linha, ";");
        if (!token) continue;
        strncpy(p.url, token, 99); p.url[99] = '\0';
        remover_caracteres_invisiveis(p.url);
        
        token = strtok(NULL, ";");
        if (!token) continue;
        strncpy(p.titulo, token, 49); p.titulo[49] = '\0';
        remover_caracteres_invisiveis(p.titulo);
        
        token = strtok(NULL, "\n");
        if (!token) continue;
        strncpy(p.dataHora, token, 19); p.dataHora[19] = '\0';
        remover_caracteres_invisiveis(p.dataHora);
        
        push_hist(p);
    }
    fclose(f);
}

// --- Modo Extra: Pilha Dinâmica ---
void modo_dinamico() {
    printf("\n[MODO EXTRA] Demonstracao de Pilha Dinamica Encadeada\n");
    No* topo = NULL;
    for (int i = 0; i <= topo_hist; i++) {
        No* novo = (No*)malloc(sizeof(No));
        novo->dado = historico[i];
        novo->proximo = topo;
        topo = novo;
    }
    printf("Pilha dinamica sincronizada com %d paginas.\n", topo_hist + 1);
    while (topo) {
        No* t = topo;
        topo = topo->proximo;
        free(t);
    }
    printf("Memoria dinamica liberada.\n");
}

int main() {
    carregar_csv();
    int opcao;
    Pagina p;

    do {
        printf("\n--- NAVEGADOR (PILHA ESTATICA) ---\n");
        if (topo_hist >= 0) printf("ATUAL: %s\n", historico[topo_hist].titulo);
        else printf("ATUAL: Nenhuma\n");
        
        printf("1. Visitar Pagina\n");
        printf("2. Voltar (Back)\n");
        printf("3. Avancar (Forward)\n");
        printf("4. Ver Historico\n");
        printf("8. MODO DINAMICO (EXTRA)\n");
        printf("0. Sair\n");
        printf("Escolha: ");
        
        if (scanf("%d", &opcao) != 1) { limpar_buffer(); continue; }

        switch (opcao) {
            case 1:
                printf("URL: "); limpar_buffer(); fgets(p.url, 100, stdin); remover_caracteres_invisiveis(p.url);
                printf("Titulo: "); fgets(p.titulo, 50, stdin); remover_caracteres_invisiveis(p.titulo);
                obter_data_hora(p.dataHora);
                push_hist(p);
                topo_fut = -1; // Limpa futuro ao visitar nova
                break;
            case 2:
                if (pop_hist(&p)) {
                    push_fut(p);
                    printf("Voltou de: %s\n", p.titulo);
                } else printf("Nao ha para onde voltar.\n");
                break;
            case 3:
                if (pop_fut(&p)) {
                    push_hist(p);
                    printf("Avancou para: %s\n", p.titulo);
                } else printf("Nao ha para onde avancar.\n");
                break;
            case 4:
                printf("\n--- Historico ---\n");
                for (int i = topo_hist; i >= 0; i--) printf("[%s] %s\n", historico[i].dataHora, historico[i].titulo);
                break;
            case 8: modo_dinamico(); break;
            case 0: salvar_csv(); break;
        }
    } while (opcao != 0);

    return 0;
}
