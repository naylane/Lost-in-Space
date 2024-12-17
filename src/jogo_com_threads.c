#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include "proc_grafico.h"
#include "acelerometro.c"
#include "mouse_thread.c"
#include "colisao.c"
#include "ovni.c"
#include "animacao_portal.c"

extern void fecha_dev_mem();
extern void inicializa_fpga();
extern void escreve_bloco(uint16_t posicao, uint16_t cor);
extern void apaga_bloco(uint16_t posicao);
extern void altera_cor_bg(uint16_t cor, uint8_t registrador);
extern void apaga_cor_bg(uint8_t registrador);
extern void exibe_sprite(uint8_t sp, uint32_t xy, uint16_t offset, uint8_t registrador);
extern int acess_btn();

extern int abre_mouse();
extern int le_mouse_orientacao(int fd);
extern int le_mouse_direcao(int fd, uint16_t *x, uint16_t *y);
extern int teste_leitura(int fd);

#define ALTURA_LAB 60   // Altura do labirinto
#define LARGURA_LAB 80  // Largura do labirinto
#define ESPESSURA 4     // Espessura das paredes e caminhos
#define mascara_10bits 0b1111111111

char labirinto[ALTURA_LAB][LARGURA_LAB];

// Vetores para movimentação (cima, baixo, esquerda, direita)
int dx[] = {0, 0, -1, 1};
int dy[] = {-1, 1, 0, 0};

// Dados dos jogadores p1 e p2
typedef struct {
    uint32_t pos_xy_20b;
    pthread_mutex_t mutex;
    int ativo;
} Dados_jogador;

Dados_jogador p1_acelerometro;
Dados_jogador p2_mouse;

int usleep(useconds_t usec);
void inicializaLabirinto();
void imprimeLabirinto();
int validaPosicao(int x, int y);
void geraLabirinto(int x, int y);
void imprimeLabirintoVGA();
void apagaLabirinto();
int colide(uint16_t prox_pos_x, uint16_t prox_pos_y);
void *move_acelerometro();
void *move_mouse();
void def_saidas_labirinto();
void def_posicao_jogadores();
void def_borda_labirinto();
void posiciona_sprites(uint32_t *pos_xy_20b_p1, uint32_t *pos_xy_20b_p2);
int main();

// Função para inicializar o labirinto com paredes
void inicializaLabirinto() {
    for (int i = 0; i < ALTURA_LAB; i++) {
        for (int j = 0; j < LARGURA_LAB; j++) {
            labirinto[i][j] = '#';
        }
    }
}

// Função para imprimir o labirinto
void imprimeLabirintoTerminal() {
    for (int i = 0; i < ALTURA_LAB; i++) {
        for (int j = 0; j < LARGURA_LAB; j++) {
            printf("%c", labirinto[i][j]);
        }
        printf("\n");
    }
}

// Função para verificar se a posição é válida e se pode ser cavada
int validaPosicao(int x, int y) {
    if (x < ESPESSURA || y < ESPESSURA || x >= ALTURA_LAB - ESPESSURA || y >= LARGURA_LAB - ESPESSURA) {
        return 0; // Fora dos limites
    }
    return labirinto[x][y] == '#'; // Cavar apenas se a posição for parede
}

// Função recursiva para gerar o labirinto
void geraLabirinto(int x, int y) {
    for (int i = 0; i < ESPESSURA; i++) {
        for (int j = 0; j < ESPESSURA; j++) {
            labirinto[x + i][y + j] = ' '; // Marca como espaço
        }
    }

    // Embaralhar as direções
    int direcoes[] = {0, 1, 2, 3};
    for (int i = 0; i < 4; i++) {
        int r = rand() % 4;
        int temp = direcoes[i];
        direcoes[i] = direcoes[r];
        direcoes[r] = temp;
    }

    // Tentar cada direção
    for (int i = 0; i < 4; i++) {
        int nx = x + dx[direcoes[i]] * ESPESSURA * 2;
        int ny = y + dy[direcoes[i]] * ESPESSURA * 2;

        if (validaPosicao(nx, ny)) {
            for (int j = 1; j < ESPESSURA * 2; j++) {
                for (int k = 0; k < ESPESSURA; k++) {
                    labirinto[x + dx[direcoes[i]] * j + k][y + dy[direcoes[i]] * j + k] = ' '; // Remove parede intermediária
                }
            }
            geraLabirinto(nx, ny);
        }
    }
}

void imprimeLabirintoVGA() {
    altera_cor_bg(BRANCO, 0); //pintando fundo
    int i, j;
    for(i=0;i < ALTURA_LAB;i++) { //linhas
        for(j=0; j < LARGURA_LAB; j++) { //colunas
            if(labirinto[i][j] == '#'){
                escreve_bloco( j + (i * 80), AZUL_ESCURO);
            } else if(labirinto[i][j] == 'K'){
                escreve_bloco( j + (i * 80), PRETO);
            }
        }
    }
}

void apagaLabirinto() {
    apaga_cor_bg(0);
    int i, j;
    for(i=0; i < ALTURA_LAB; i++) { //linhas
        for(j=0; j < LARGURA_LAB; j++) { //colunas
                apaga_bloco( j + (i * 80));
        }
    }
}

//Verifica inicio e fim do sprite
int colide(uint16_t prox_pos_x, uint16_t prox_pos_y) { 
    int verifica_x[] = {prox_pos_x, prox_pos_x + 19};
    int verifica_y[] = {prox_pos_y, prox_pos_y + 19};
    int i,j,q,w;
    for( q= 0; q < 2; q++) {
        for( w = 0; w < 2; w++) {
            converte_sprite_para_labirinto(verifica_x[q], verifica_y[w], &i, &j);
            if (labirinto[i][j] == '#') {
                //printf("Tem parede - labirinto %d,%d = %c\n", i,j, labirinto[i][j]);
                return 1; //Tem parede
            } else if (labirinto[i][j] == 'F') {
                return 2; //Saida Fechada
            }
        }
    }
    //printf("Sem parede \n");
    return 0; //Sem colisao 
}

void *move_acelerometro() {
    int direcao_sprite, movimento, colidiu;
    uint16_t pos_x, pos_y, prox_pos_y, prox_pos_x;
    int velocidade = 1;   
    p1_acelerometro.ativo = 1;

    uint32_t pos_xy_20b_ant = p1_acelerometro.pos_xy_20b; //inicia com posicao anterior igual a posicao atual

    while (p1_acelerometro.ativo) {
        pthread_mutex_lock(&p1_acelerometro.mutex);

        pos_y = (p1_acelerometro.pos_xy_20b & mascara_10bits);
        pos_x = ((p1_acelerometro.pos_xy_20b >> 10) & mascara_10bits);

        direcao_sprite = get_movimento(&velocidade); //8 cima, 2 baixo, 6 direita, 4 esquerda, 0 sem movimento

        //apaga o sprite exibido na posicao anterior
        exibe_sprite(0, pos_xy_20b_ant, 1, 1);//sp = 0 - desabilita sprite
        pos_xy_20b_ant = p1_acelerometro.pos_xy_20b;
    
        //exibe o sprite na posicao atual
        exibe_sprite(1, p1_acelerometro.pos_xy_20b, 1, 1);//sp = 1 - habilita sprite

        movimento = 5*velocidade;

        //descendo
        if ( direcao_sprite == 2 ){
            prox_pos_y = pos_y + movimento;
            colidiu = colide(pos_x, prox_pos_y);
            if( !colidiu ){ //sem parede, pode mover
                pos_y = prox_pos_y;
            }
        }
        //subindo
        else if ( direcao_sprite == 8 ){
            prox_pos_y = pos_y - movimento;
            colidiu = colide(pos_x, prox_pos_y);
            if( !colidiu ){ //sem parede, pode mover
                pos_y = prox_pos_y;
            }
        }
        //direita
        else if ( direcao_sprite == 6 ){
            prox_pos_x = pos_x + movimento;
            colidiu = colide(prox_pos_x, pos_y);
            if( !colidiu ){ //sem parede, pode mover
                pos_x = prox_pos_x; 
            }
        }
        //esquerda
        else if ( direcao_sprite == 4 ){
            prox_pos_x = pos_x - movimento;
            colidiu = colide(prox_pos_x, pos_y);
            if( !colidiu ){ //sem parede, pode mover
                pos_x = prox_pos_x; 
            }
        }
        p1_acelerometro.pos_xy_20b = (pos_x << 10 | pos_y);

        pthread_mutex_unlock(&p1_acelerometro.mutex);
        usleep(10000);
    }
}

void *move_mouse() {
    int fd, direcao, orientacao, colidiu;
    uint16_t pos_x, pos_y, prox_pos_y, prox_pos_x;

    fd = abre_mouse();
    p2_mouse.ativo = 1;

    uint32_t pos_xy_20b_ant = p2_mouse.pos_xy_20b;
    
    while (p2_mouse.ativo) {  
        // Trava mutex para acesso seguro aos dados  
        pthread_mutex_lock(&p2_mouse.mutex);

        pos_x = (p2_mouse.pos_xy_20b >> 10) & mascara_10bits;
        pos_y = p2_mouse.pos_xy_20b & mascara_10bits;

        direcao = le_mouse_direcao(fd, &pos_x, &pos_y); // 0 horizontal, 1 vertical
        orientacao = le_mouse_orientacao(fd); //8 cima, 2 baixo, 6 direita, 4 esquerda, 0 sem movimento

        //apaga o sprite exibido na posicao anterior
        exibe_sprite(0, pos_xy_20b_ant, 2, 2);//sp = 0 - desabilita sprite
        pos_xy_20b_ant = p2_mouse.pos_xy_20b;
    
        //exibe o sprite na posicao atual
        exibe_sprite(1, p2_mouse.pos_xy_20b, 2, 2);//sp = 1 - habilita sprite

        if (orientacao == 8) { // cima
            prox_pos_y = pos_y - 10;
            colidiu = colide(pos_x, prox_pos_y);
            if( !colidiu ){ //sem parede, pode mover
                pos_y = prox_pos_y;
            }
        } else if (orientacao == 2) { // baixo
            prox_pos_y = pos_y + 10;
            colidiu = colide(pos_x, prox_pos_y);
            if( !colidiu ){ //sem parede, pode mover
                pos_y = prox_pos_y;
            }            
        } else if (orientacao == 6) { // direita
            prox_pos_x = pos_x + 10;
            colidiu = colide(prox_pos_x, pos_y);
            if( !colidiu ){ //sem parede, pode mover
                pos_x = prox_pos_x; 
            }
        } else if (orientacao == 4) { // esquerda
            prox_pos_x = pos_x - 10;
            colidiu = colide(prox_pos_x, pos_y);
            if( !colidiu ){ //sem parede, pode mover
                pos_x = prox_pos_x; 
            }
        }

        p2_mouse.pos_xy_20b = (pos_x << 10 | pos_y);
        
        pthread_mutex_unlock(&p2_mouse.mutex);
        usleep(10000);
    }
}

void def_saidas_labirinto(){
    // Coloca as saídas
    for (int i = 0; i <= ESPESSURA; i++) {
        labirinto[ESPESSURA][i] = ' ';
        labirinto[ESPESSURA + 1][i] = ' ';
        labirinto[ESPESSURA + 2][i] = ' ';
        labirinto[ESPESSURA + 3][i] = ' ';
        labirinto[ALTURA_LAB - ESPESSURA - 1][LARGURA_LAB - ESPESSURA - i] = ' '; 
        labirinto[ALTURA_LAB - ESPESSURA - 2][LARGURA_LAB - ESPESSURA - i] = ' '; 
        labirinto[ALTURA_LAB - ESPESSURA - 3][LARGURA_LAB - ESPESSURA - i] = ' '; 
        labirinto[ALTURA_LAB - ESPESSURA - 4][LARGURA_LAB - ESPESSURA - i] = ' '; 
    }
    
    labirinto[ESPESSURA][0] = 'F';
    labirinto[ESPESSURA + 1][0] = 'F';
    labirinto[ESPESSURA + 2][0] = 'F';
    labirinto[ESPESSURA + 3][0] = 'F';

    labirinto[ALTURA_LAB - ESPESSURA - 4][LARGURA_LAB - ESPESSURA] = 'F'; 
    labirinto[ALTURA_LAB - ESPESSURA - 3][LARGURA_LAB - ESPESSURA] = 'F'; 
    labirinto[ALTURA_LAB - ESPESSURA - 2][LARGURA_LAB - ESPESSURA] = 'F'; 
    labirinto[ALTURA_LAB - ESPESSURA - 1][LARGURA_LAB - ESPESSURA] = 'F';
}

void def_posicao_jogadores(){
    // Posiciona o p1 e p2
    labirinto[ESPESSURA][1] = '1';
    labirinto[ALTURA_LAB - ESPESSURA - 4][LARGURA_LAB - ESPESSURA - 4] = '2';
}

void def_borda_labirinto(){
    for(int i = 0; i < ALTURA_LAB; i++) { 
        for(int j = 0; j < LARGURA_LAB; j++) {
            labirinto[i][LARGURA_LAB - 1] = 'K';
            labirinto[i][LARGURA_LAB - 2] = 'K';
            labirinto[i][LARGURA_LAB - 3] = 'K';
        }
    }
}

void posiciona_sprites(uint32_t *pos_xy_20b_p1, uint32_t *pos_xy_20b_p2){
    //posicao inicial p1 e p2
    uint16_t pos_x_p1, pos_y_p1, pos_x_p2, pos_y_p2;

    for(int i = 0; i < ALTURA_LAB; i++) { 
        for(int j = 0; j < LARGURA_LAB; j++) {
            if(labirinto[i][j] == '1'){
                //printf("Sprite 1 - x lab: %d y lab: %d\n", i ,j);
                converte_labirinto_para_sprite(i,j, &pos_x_p1, &pos_y_p1);
                pos_x_p1 &= mascara_10bits;
                pos_y_p1 &= mascara_10bits;
                *pos_xy_20b_p1 = (pos_x_p1 << 10 | pos_y_p1);
                exibe_sprite(1, *pos_xy_20b_p1, 1, 1);
            }
            else if(labirinto[i][j] == '2'){
                //printf("Sprite 2 - x lab: %d y lab: %d\n", i ,j);
                converte_labirinto_para_sprite(i,j, &pos_x_p2, &pos_y_p2);
                pos_x_p2 &= mascara_10bits;
                pos_y_p2 &= mascara_10bits;
                *pos_xy_20b_p2 = (pos_x_p2 << 10 | pos_y_p2);
                exibe_sprite(1, *pos_xy_20b_p2, 2, 2);
            }
        }
    }
}

int main(){
    inicializa_fpga();

    // Inicializa mutexes
    pthread_mutex_init(&p1_acelerometro.mutex, NULL);
    pthread_mutex_init(&p2_mouse.mutex, NULL);

    for (int i = 0; i < 50; i++) {
        apagaLabirinto();
    }

    srand(time(NULL)); // Semente para números aleatórios

    inicializaLabirinto();

    geraLabirinto(ESPESSURA, ESPESSURA);

    // Coloca as saidas
    def_saidas_labirinto();

    // Posiciona o p1 e p2
    def_posicao_jogadores();

    def_borda_labirinto();

    //imprimeLabirintoTerminal();

    configurar_acelerometro();
    grava_sprite_ovni();

    for (int i = 0; i < 1200; i++) {
        imprimeLabirintoVGA();
    }

    posiciona_sprites(&p1_acelerometro.pos_xy_20b, &p2_mouse.pos_xy_20b);

    // Cria as threads
    pthread_t thread_acel, thread_mouse;
    
    if (pthread_create(&thread_acel, NULL, move_acelerometro, NULL) != 0) {
        perror("Erro ao criar thread do acelerometro");
        return 1;
    }
    
    if (pthread_create(&thread_mouse, NULL, move_mouse, NULL) != 0) {
        perror("Erro ao criar thread do mouse");
        p1_acelerometro.ativo = 0;
        pthread_join(thread_acel, NULL);
        return 1;
    }
    
    while (1) {
        if (acess_btn() == 1) { // Encerra o jogo
            break;
        }
        usleep(100000);
    }
    
    p1_acelerometro.ativo = 0;
    p2_mouse.ativo = 0;
    
    // Espera as threads terminarem
    pthread_join(thread_acel, NULL);
    pthread_join(thread_mouse, NULL);
    
    pthread_mutex_destroy(&p1_acelerometro.mutex);
    pthread_mutex_destroy(&p2_mouse.mutex);
    
    desmapear_memoria();
    fecha_dev_mem();
    return 0;
}