#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOME 50
#define TAM_HASH 10

// ----------------------------
// ESTRUTURA 1: ÁRVORE BINÁRIA (MAPA DA MANSÃO)
// ----------------------------
typedef struct Sala {
    char nome[MAX_NOME];
    struct Sala *esquerda;
    struct Sala *direita;
    char pista[MAX_NOME];      // Pista encontrada na sala (se houver)
    char suspeito[MAX_NOME];   // Suspeito associado à pista (se houver)
} Sala;

// Função para criar uma nova sala
Sala* criarSala(char nome[], char pista[], char suspeito[]) {
    Sala* nova = (Sala*) malloc(sizeof(Sala));
    strcpy(nova->nome, nome);
    strcpy(nova->pista, pista);
    strcpy(nova->suspeito, suspeito);
    nova->esquerda = NULL;
    nova->direita = NULL;
    return nova;
}

// ----------------------------
// ESTRUTURA 2: ÁRVORE DE BUSCA (BST) PARA PISTAS
// ----------------------------
typedef struct NoBST {
    char pista[MAX_NOME];
    struct NoBST *esq, *dir;
} NoBST;

// Cria novo nó de pista
NoBST* novoNoBST(char pista[]) {
    NoBST* novo = (NoBST*) malloc(sizeof(NoBST));
    strcpy(novo->pista, pista);
    novo->esq = novo->dir = NULL;
    return novo;
}

// Inserção em árvore de busca (ordem alfabética)
NoBST* inserirPista(NoBST* raiz, char pista[]) {
    if (raiz == NULL) return novoNoBST(pista);
    if (strcmp(pista, raiz->pista) < 0)
        raiz->esq = inserirPista(raiz->esq, pista);
    else if (strcmp(pista, raiz->pista) > 0)
        raiz->dir = inserirPista(raiz->dir, pista);
    return raiz;
}

// Impressão em ordem (ordem alfabética)
void imprimirPistas(NoBST* raiz) {
    if (raiz != NULL) {
        imprimirPistas(raiz->esq);
        printf("- %s\n", raiz->pista);
        imprimirPistas(raiz->dir);
    }
}

// ----------------------------
// ESTRUTURA 3: TABELA HASH (PISTA → SUSPEITO)
// ----------------------------
typedef struct HashNode {
    char pista[MAX_NOME];
    char suspeito[MAX_NOME];
    struct HashNode* prox;
} HashNode;

HashNode* tabelaHash[TAM_HASH];

// Função hash simples: soma dos caracteres mod TAM_HASH
int funcaoHash(char* chave) {
    int soma = 0;
    for (int i = 0; chave[i] != '\0'; i++)
        soma += chave[i];
    return soma % TAM_HASH;
}

// Insere relação pista → suspeito
void inserirNaHash(char* pista, char* suspeito) {
    int indice = funcaoHash(pista);
    HashNode* novo = (HashNode*) malloc(sizeof(HashNode));
    strcpy(novo->pista, pista);
    strcpy(novo->suspeito, suspeito);
    novo->prox = tabelaHash[indice];
    tabelaHash[indice] = novo;
}

// Exibe todas as relações pista → suspeito
void exibirHash() {
    printf("\n=== Relações de Pistas e Suspeitos ===\n");
    for (int i = 0; i < TAM_HASH; i++) {
        HashNode* atual = tabelaHash[i];
        while (atual) {
            printf("%s → %s\n", atual->pista, atual->suspeito);
            atual = atual->prox;
        }
    }
}

// Encontra o suspeito mais citado
void suspeitoMaisCitado() {
    char suspeitos[20][MAX_NOME];
    int contadores[20] = {0};
    int total = 0;

    // Conta repetições
    for (int i = 0; i < TAM_HASH; i++) {
        HashNode* atual = tabelaHash[i];
        while (atual) {
            int encontrado = 0;
            for (int j = 0; j < total; j++) {
                if (strcmp(suspeitos[j], atual->suspeito) == 0) {
                    contadores[j]++;
                    encontrado = 1;
                    break;
                }
            }
            if (!encontrado) {
                strcpy(suspeitos[total], atual->suspeito);
                contadores[total++] = 1;
            }
            atual = atual->prox;
        }
    }

    // Determina o mais citado
    int max = 0, indiceMax = -1;
    for (int i = 0; i < total; i++) {
        if (contadores[i] > max) {
            max = contadores[i];
            indiceMax = i;
        }
    }

    if (indiceMax != -1)
        printf("\nSuspeito mais citado: %s (%d pistas)\n", suspeitos[indiceMax], max);
    else
        printf("\nNenhum suspeito encontrado!\n");
}

// ----------------------------
// FUNÇÃO PRINCIPAL DE EXPLORAÇÃO
// ----------------------------
void explorarSala(Sala* atual, NoBST** arvorePistas) {
    char opcao;
    while (atual != NULL) {
        printf("\nVocê está em: %s\n", atual->nome);

        // Se houver pista na sala
        if (strlen(atual->pista) > 0) {
            printf("Pista encontrada: %s\n", atual->pista);
            printf("Suspeito associado: %s\n", atual->suspeito);
            *arvorePistas = inserirPista(*arvorePistas, atual->pista);
            inserirNaHash(atual->pista, atual->suspeito);
        }

        printf("\nEscolha o caminho: (e) esquerda | (d) direita | (s) sair: ");
        scanf(" %c", &opcao);

        if (opcao == 'e')
            atual = atual->esquerda;
        else if (opcao == 'd')
            atual = atual->direita;
        else if (opcao == 's')
            break;
        else
            printf("Opção inválida!\n");
    }
}

// ----------------------------
// CONSTRUÇÃO DO MAPA DA MANSÃO
// ----------------------------
Sala* montarMansao() {
    Sala* hall = criarSala("Hall de Entrada", "", "");
    Sala* biblioteca = criarSala("Biblioteca", "Livro rasgado", "Sr. Black");
    Sala* cozinha = criarSala("Cozinha", "Faca suja", "Sra. White");
    Sala* jardim = criarSala("Jardim", "Pegadas", "Srta. Green");
    Sala* porao = criarSala("Porão", "Chave dourada", "Coronel Mustard");

    // Conexões da mansão
    hall->esquerda = biblioteca;
    hall->direita = cozinha;
    biblioteca->esquerda = jardim;
    cozinha->direita = porao;

    return hall;
}

// ----------------------------
// FUNÇÃO PRINCIPAL (MAIN)
// ----------------------------
int main() {
    // Inicializa a tabela hash
    for (int i = 0; i < TAM_HASH; i++)
        tabelaHash[i] = NULL;

    // Monta a mansão
    Sala* mansao = montarMansao();
    NoBST* arvorePistas = NULL;

    printf("Detective Quest - Nível Mestre\n");
    printf("Explore a mansão e colete pistas.\n");

    explorarSala(mansao, &arvorePistas);

    printf("\n=== Pistas Coletadas (Ordem Alfabética) ===\n");
    imprimirPistas(arvorePistas);

    exibirHash();
    suspeitoMaisCitado();

    printf("\nFim da investigação.\n");
    return 0;
}
