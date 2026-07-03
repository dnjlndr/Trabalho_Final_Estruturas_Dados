#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX 100
#define FILENAME "atendimento.csv"

// Estrutura do Registro
typedef struct {
    int senha;
    char tipo; // 'N' para Normal, 'P' para Preferencial
    char horario[20];
} Registro;

// Estrutura da Fila Normal (Simples)
typedef struct {
    Registro dados[MAX];
    int inicio;
    int fim;
    int quantidade;
} FilaNormal;

// Estrutura da Fila Preferencial (Circular)
typedef struct {
    Registro dados[MAX];
    int inicio;
    int fim;
    int quantidade;
} FilaCircular;

// Estrutura para Estatísticas
typedef struct {
    int totalAtendidos;
    int totalNormais;
    int totalPreferenciais;
} Estatisticas;

// Variáveis Globais
FilaNormal filaN;
FilaCircular filaP;
Estatisticas stats = {0, 0, 0};
int ratioNormal = 2;
int contadorAtendimento = 0;
int ultimaSenha = 0;

// Inicialização
void inicializarFilas() {
    filaN.inicio = 0;
    filaN.fim = 0;
    filaN.quantidade = 0;

    filaP.inicio = 0;
    filaP.fim = 0;
    filaP.quantidade = 0;
}

// Limpar tela
void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Obter horário atual formatado
void getHorarioAtual(char *buffer) {
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, 20, "%H:%M:%S", timeinfo);
}

// --- Operações Fila Normal ---
int enfileirarNormal(Registro r) {
    if (filaN.quantidade >= MAX) return 0;
    filaN.dados[filaN.fim] = r;
    filaN.fim++;
    filaN.quantidade++;
    return 1;
}

Registro desenfileirarNormal() {
    Registro r = filaN.dados[filaN.inicio];
    for (int i = 0; i < filaN.fim - 1; i++) {
        filaN.dados[i] = filaN.dados[i + 1];
    }
    filaN.fim--;
    filaN.quantidade--;
    return r;
}

// --- Operações Fila Circular ---
int enfileirarCircular(Registro r) {
    if (filaP.quantidade >= MAX) return 0;
    filaP.dados[filaP.fim] = r;
    filaP.fim = (filaP.fim + 1) % MAX;
    filaP.quantidade++;
    return 1;
}

Registro desenfileirarCircular() {
    Registro r = filaP.dados[filaP.inicio];
    filaP.inicio = (filaP.inicio + 1) % MAX;
    filaP.quantidade--;
    return r;
}

// --- Funcionalidades Principais ---

void gerarSenha(char tipo) {
    if (tipo == 'N' && filaN.quantidade >= MAX) {
        printf("\n[ERRO] Fila Normal atingiu o limite maximo de %d senhas!\n", MAX);
        return;
    }
    if (tipo == 'P' && filaP.quantidade >= MAX) {
        printf("\n[ERRO] Fila Preferencial atingiu o limite maximo de %d senhas!\n", MAX);
        return;
    }

    Registro novo;
    novo.senha = ++ultimaSenha;
    novo.tipo = tipo;
    getHorarioAtual(novo.horario);

    if (tipo == 'N') enfileirarNormal(novo);
    else enfileirarCircular(novo);

    printf("\n[SUCESSO] Senha %c%d gerada as %s\n", tipo, novo.senha, novo.horario);
}

void chamarProximo() {
    if (filaN.quantidade == 0 && filaP.quantidade == 0) {
        printf("\n[AVISO] Nenhuma senha aguardando atendimento.\n");
        return;
    }

    Registro atendido;
    if (filaP.quantidade > 0 && (contadorAtendimento >= ratioNormal || filaN.quantidade == 0)) {
        atendido = desenfileirarCircular();
        contadorAtendimento = 0;
        stats.totalPreferenciais++;
    } else if (filaN.quantidade > 0) {
        atendido = desenfileirarNormal();
        contadorAtendimento++;
        stats.totalNormais++;
    } else {
        atendido = desenfileirarCircular();
        stats.totalPreferenciais++;
    }

    stats.totalAtendidos++;
    printf("\n==================================\n");
    printf("  CHAMANDO: %s%d\n", (atendido.tipo == 'N' ? "Normal " : "Preferencial "), atendido.senha);
    printf("  Horario de Chegada: %s\n", atendido.horario);
    printf("==================================\n");
}

void listarFilas() {
    printf("\n--- FILA NORMAL (%d/%d) ---\n", filaN.quantidade, MAX);
    if (filaN.quantidade == 0) printf("Vazia\n");
    for (int i = 0; i < filaN.quantidade; i++) {
        printf("[%d] %s%s", filaN.dados[i].senha, filaN.dados[i].horario, (i == filaN.quantidade - 1 ? "" : " | "));
    }

    printf("\n\n--- FILA PREFERENCIAL (%d/%d) ---\n", filaP.quantidade, MAX);
    if (filaP.quantidade == 0) printf("Vazia\n");
    int idx = filaP.inicio;
    for (int i = 0; i < filaP.quantidade; i++) {
        printf("[%d] %s%s", filaP.dados[idx].senha, filaP.dados[idx].horario, (i == filaP.quantidade - 1 ? "" : " | "));
        idx = (idx + 1) % MAX;
    }
    printf("\n");
}

void buscarSenha(int s) {
    for (int i = 0; i < filaN.quantidade; i++) {
        if (filaN.dados[i].senha == s) {
            printf("\nSenha %d encontrada na Fila Normal (Posicao %d)\n", s, i + 1);
            return;
        }
    }
    int idx = filaP.inicio;
    for (int i = 0; i < filaP.quantidade; i++) {
        if (filaP.dados[idx].senha == s) {
            printf("\nSenha %d encontrada na Fila Preferencial (Posicao %d)\n", s, i + 1);
            return;
        }
        idx = (idx + 1) % MAX;
    }
    printf("\nSenha %d nao encontrada.\n", s);
}

void cancelarSenha(int s) {
    char confirm;
    printf("Tem certeza que deseja cancelar a senha %d? (s/n): ", s);
    scanf(" %c", &confirm);
    if (confirm != 's' && confirm != 'S') return;

    for (int i = 0; i < filaN.quantidade; i++) {
        if (filaN.dados[i].senha == s) {
            for (int j = i; j < filaN.quantidade - 1; j++) {
                filaN.dados[j] = filaN.dados[j + 1];
            }
            filaN.fim--;
            filaN.quantidade--;
            printf("Senha %d cancelada com sucesso da Fila Normal.\n", s);
            return;
        }
    }

    int idx = filaP.inicio;
    for (int i = 0; i < filaP.quantidade; i++) {
        if (filaP.dados[idx].senha == s) {
            int atual = idx;
            int proximo = (idx + 1) % MAX;
            for (int j = 0; j < (filaP.quantidade - i - 1); j++) {
                filaP.dados[atual] = filaP.dados[proximo];
                atual = proximo;
                proximo = (proximo + 1) % MAX;
            }
            filaP.fim = (filaP.fim - 1 + MAX) % MAX;
            filaP.quantidade--;
            printf("Senha %d cancelada com sucesso da Fila Preferencial.\n", s);
            return;
        }
        idx = (idx + 1) % MAX;
    }
    printf("Senha %d nao encontrada.\n", s);
}

// --- Persistência CSV ---

void salvarCSV() {
    FILE *f = fopen(FILENAME, "w");
    if (!f) {
        printf("[ERRO] Nao foi possivel criar o arquivo.\n");
        return;
    }

    fprintf(f, "senha;tipo;horario\n");
    for (int i = 0; i < filaN.quantidade; i++) {
        fprintf(f, "%d;%c;%s\n", filaN.dados[i].senha, filaN.dados[i].tipo, filaN.dados[i].horario);
    }
    int idx = filaP.inicio;
    for (int i = 0; i < filaP.quantidade; i++) {
        fprintf(f, "%d;%c;%s\n", filaP.dados[idx].senha, filaP.dados[idx].tipo, filaP.dados[idx].horario);
        idx = (idx + 1) % MAX;
    }
    fclose(f);
    printf("\n[INFO] Dados salvos com sucesso em %s\n", FILENAME);
}

void carregarCSV() {
    FILE *f = fopen(FILENAME, "r");
    if (!f) {
        printf("\n[INFO] Nenhum arquivo previo encontrado.\n");
        return;
    }

    inicializarFilas();
    ultimaSenha = 0;
    char linha[100];
    if (!fgets(linha, 100, f)) { fclose(f); return; }

    while (fgets(linha, 100, f)) {
        Registro r;
        char *token = strtok(linha, ";");
        if (!token) continue;
        r.senha = atoi(token);
        if (r.senha > ultimaSenha) ultimaSenha = r.senha;

        token = strtok(NULL, ";");
        if (!token) continue;
        r.tipo = token[0];

        token = strtok(NULL, "\n");
        if (!token) continue;
        strcpy(r.horario, token);

        if (r.tipo == 'N') enfileirarNormal(r);
        else enfileirarCircular(r);
    }

    fclose(f);
    printf("\n[INFO] %d senhas carregadas de %s\n", filaN.quantidade + filaP.quantidade, FILENAME);
}

// --- Funcionalidades Extras ---

void exibirEstatisticas() {
    printf("\n--- RELATORIO DE ATENDIMENTO ---\n");
    printf("Total Atendidos: %d\n", stats.totalAtendidos);
    printf("Senhas Normais: %d\n", stats.totalNormais);
    printf("Senhas Preferenciais: %d\n", stats.totalPreferenciais);
    printf("--------------------------------\n");
}

void configurarPrioridade() {
    printf("\nConfiguracao Atual: %d Normais para 1 Preferencial\n", ratioNormal);
    printf("Nova quantidade de Normais antes de 1 Preferencial: ");
    if (scanf("%d", &ratioNormal) != 1) {
        printf("Entrada invalida.\n");
        while(getchar() != '\n');
    } else {
        printf("Configuracao atualizada para %d:1!\n", ratioNormal);
    }
}

void menu() {
    int opcao, s;
    do {
        printf("\n================================");
        printf("\n     SISTEMA BANCARIO ");
        printf("\n================================");
        printf("\n1. Gerar Senha Normal");
        printf("\n2. Gerar Senha Preferencial");
        printf("\n3. Chamar Proximo (Regra %d:1)", ratioNormal);
        printf("\n4. Listar Filas");
        printf("\n5. Buscar Senha");
        printf("\n6. Cancelar Senha");
        printf("\n7. Salvar ");
        printf("\n8. Carregar ");
        printf("\n9. Estatisticas / Relatorio");
        printf("\n10. Configurar Proporcao");
        printf("\n0. Sair e Limpar Dados");
        printf("\nEscolha: ");

        if (scanf("%d", &opcao) != 1) {
            printf("Por favor, digite um numero.\n");
            while(getchar() != '\n');
            continue;
        }

        switch(opcao) {
            case 1: gerarSenha('N'); break;
            case 2: gerarSenha('P'); break;
            case 3: chamarProximo(); break;
            case 4: listarFilas(); break;
            case 5:
                printf("Senha a buscar: ");
                scanf("%d", &s);
                buscarSenha(s);
                break;
            case 6:
                printf("Senha a cancelar: ");
                scanf("%d", &s);
                cancelarSenha(s);
                break;
            case 7: salvarCSV(); break;
            case 8: carregarCSV(); break;
            case 9: exibirEstatisticas(); break;
            case 10: configurarPrioridade(); break;
            case 0:
                printf("Limpando arquivo CSV e encerrando...\n");
                remove(FILENAME);
                break;
            default: printf("Opcao invalida!\n");
        }
    } while(opcao != 0);
}

int main() {
    inicializarFilas();
    menu();
    return 0;
}
