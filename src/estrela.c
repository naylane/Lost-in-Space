#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include "sprite.c"

int usleep(useconds_t usec);
void gera_sprite_estrela_offset8();
void gera_sprite_estrela_offset9();
void gera_sprite_estrela_offset10();
void grava_sprite_estrela();
void animacao_estrela(uint32_t pos_xy_20b, int sp);

//Dados da imagem para formar um sprite (20x20) em formato RRRGGGBB - 1 byte por pixel
uint16_t dados_da_imagem_1[altura_sprite][largura_sprite] = {
{ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0xf4,0xfc,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0xf4,0xf4,0xfc,0xfc,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0x00,0x00,0xfe,0xf4,0xf8,0xf8,0xfc,0xfc,0xf8,0xfe,0x00,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0x00,0xfe,0xf4,0xf4,0xf8,0xf8,0xfc,0xfc,0xf8,0xf8,0xfe,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0x00,0xfe,0xf8,0xf8,0xfc,0xfc,0xfe,0xfe,0xfc,0xfc,0xfe,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0xfe,0xfe,0xf8,0xf8,0xfc,0xfc,0xfe,0xfe,0xfc,0xfc,0xfe,0xfe,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0xfe,0xfe,0xf8,0xf8,0xfc,0xfc,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xfc,0xfc,0xfe,0xfe,0x00,0x00},
{ 0x00,0xfe,0xf4,0xf4,0xf8,0xf8,0xfc,0xfc,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xfc,0xfc,0xfc,0xfc,0xfe,0x00},
{ 0xfe,0xf4,0xf8,0xf8,0xf8,0xf8,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xfd,0xfd,0xfc,0xfe},
{ 0xf4,0xf4,0xf8,0xf8,0xf8,0xf8,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xfd,0xfd,0xfc,0xfc},
{ 0xf8,0xf8,0xf8,0xf8,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfd,0xfd,0xfd,0xfd,0xfd,0xfd},
{ 0xfe,0xf8,0xf8,0xf8,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfd,0xfd,0xfd,0xfd,0xfd,0xfe},
{ 0x00,0xfe,0xf4,0xf4,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfe,0x00},
{ 0x00,0x00,0xfe,0xf4,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfe,0x00,0x00},
{ 0x00,0x00,0x00,0xfe,0xf8,0xf8,0xfc,0xfc,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xfc,0xfc,0xfe,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0xfe,0xf8,0xf8,0xfc,0xfc,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xfc,0xfc,0xfe,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0xfe,0xf8,0xf8,0xfc,0xfc,0xf4,0xf4,0xf8,0xf8,0xfd,0xfd,0xfc,0xfc,0xfe,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0xfe,0xf8,0xf8,0xfc,0xfc,0xf4,0xf4,0xf8,0xf8,0xfd,0xfd,0xfc,0xfc,0xfe,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0xfe,0xf8,0xf8,0xf4,0xf4,0xfe,0xfe,0xfe,0xfe,0xf8,0xf8,0xfc,0xfc,0xfe,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0xfe,0xf8,0xf4,0xfe,0x00,0x00,0x00,0x00,0xfe,0xf8,0xfc,0xfe,0x00,0x00,0x00,0x00}
};

//Dados da imagem para formar um sprite (20x20) em formato RRRGGGBB - 1 byte por pixel
uint16_t dados_da_imagem_2[altura_sprite][largura_sprite] = {
{ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf4,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf4,0xf4,0xfc,0xfc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xf4,0xf8,0xf8,0xfc,0xfc,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0x00,0x00,0xf4,0xf4,0xf8,0xf8,0xfc,0xfc,0xf8,0xf8,0x00,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0x00,0x00,0xf8,0xf8,0xfc,0xfc,0xfe,0xfe,0xfc,0xfc,0x00,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0x00,0x00,0xf8,0xf8,0xfc,0xfc,0xfe,0xfe,0xfc,0xfc,0x00,0x00,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0xf8,0xf8,0xfc,0xfc,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xfc,0xfc,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0xf4,0xf4,0xf8,0xf8,0xfc,0xfc,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xfc,0xfc,0xfc,0xfc,0x00,0x00},
{ 0x00,0xf4,0xf8,0xf8,0xf8,0xf8,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xfd,0xfd,0xfc,0x00},
{ 0xf4,0xf4,0xf8,0xf8,0xf8,0xf8,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xfd,0xfd,0xfc,0xfc},
{ 0xf8,0xf8,0xf8,0xf8,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfd,0xfd,0xfd,0xfd,0xfd,0xfd},
{ 0x00,0xf8,0xf8,0xf8,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfd,0xfd,0xfd,0xfd,0xfd,0x00},
{ 0x00,0x00,0xf4,0xf4,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0x00,0x00},
{ 0x00,0x00,0x00,0xf4,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0xf8,0xf8,0xfc,0xfc,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xfc,0xfc,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0xf8,0xf8,0xfc,0xfc,0xfc,0xfc,0xfd,0xfd,0xfe,0xfe,0xfc,0xfc,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0xf8,0xf8,0xfc,0xfc,0xf4,0xf4,0xf8,0xf8,0xfd,0xfd,0xfc,0xfc,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0xf8,0xf8,0xfc,0xfc,0xf4,0xf4,0xf8,0xf8,0xfd,0xfd,0xfc,0xfc,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0xf8,0xf8,0xf4,0xf4,0x00,0x00,0x00,0x00,0xf8,0xf8,0xfc,0xfc,0x00,0x00,0x00,0x00},
{ 0x00,0x00,0x00,0x00,0x00,0xf8,0xf4,0x00,0x00,0x00,0x00,0x00,0x00,0xf8,0xfc,0x00,0x00,0x00,0x00,0x00}
};

//Dados da imagem para formar um sprite (20x20) em formato RRRGGGBB - 1 byte por pixel
uint16_t dados_da_imagem_3[altura_sprite][largura_sprite] = {
{ 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff },
{ 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0xfc,0xfc,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff },
{ 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff },
{ 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff },
{ 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff },
{ 0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xff,0xff,0xff,0xff,0xff,0xff },
{ 0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xff,0xff,0xff,0xff,0xff,0xff },
{ 0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc },
{ 0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0x00,0xfc,0xfc,0xfc,0xfc,0x00,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc },
{ 0xff,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0x00,0xfc,0xfc,0xfc,0xfc,0x00,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xff },
{ 0xff,0xff,0xfc,0xfc,0xfc,0xfc,0xfc,0x00,0xfc,0xfc,0xfc,0xfc,0x00,0xfc,0xfc,0xfc,0xfc,0xfc,0xff,0xff },
{ 0xff,0xff,0xff,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xff,0xff,0xff },
{ 0xff,0xff,0xff,0xff,0xfc,0xfc,0x00,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0x00,0xfc,0xfc,0xff,0xff,0xff,0xff },
{ 0xff,0xff,0xff,0xff,0xfc,0xfc,0xfc,0x00,0xfc,0xfc,0xfc,0xfc,0x00,0xfc,0xfc,0xfc,0xff,0xff,0xff,0xff },
{ 0xff,0xff,0xff,0xfc,0xfc,0xfc,0xfc,0xfc,0x00,0x00,0x00,0x00,0xfc,0xfc,0xfc,0xfc,0xfc,0xff,0xff,0xff },
{ 0xff,0xff,0xff,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xff,0xff,0xff },
{ 0xff,0xff,0xff,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xff,0xff,0xff },
{ 0xff,0xff,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xff,0xff,0xff,0xff,0xfc,0xfc,0xfc,0xfc,0xfc,0xfc,0xff,0xff },
{ 0xff,0xff,0xfc,0xfc,0xfc,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0xfc,0xfc,0xfc,0xff,0xff },
{ 0xff,0xff,0xfc,0xfc,0xfc,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xfc,0xfc,0xfc,0xff,0xff },
};


void gera_sprite_estrela_offset8(){
    //Dados para formar um sprite 20x20 em formato RRR GGG BBB - 9 bits
    uint16_t dados_do_sprite[altura_sprite][largura_sprite];
    uint16_t cor_temp;
    int y, x;
    for ( y = 0; y < altura_sprite; y++) {
        for ( x = 0; x < largura_sprite; x++) {
            cor_temp = dados_da_imagem_1[y][x];
            if (cor_temp == 0x00){
                dados_do_sprite[y][x] = APAGA;
            } else {
                dados_do_sprite[y][x] = converte_em_bgr(dados_da_imagem_1[y][x]);//Converte pixel por pixel do formato RGB para o BGR
            }
        }
    }
    //Escreve os dados de cada pixel na memoria de sprites. [end_base = offset * 400]
    cria_sprite(3200, dados_do_sprite);
}

void gera_sprite_estrela_offset9(){
    //Dados para formar um sprite 20x20 em formato RRR GGG BBB - 9 bits
    uint16_t dados_do_sprite[altura_sprite][largura_sprite];
    uint16_t cor_temp;
    int y, x;
    for ( y = 0; y < altura_sprite; y++) {
        for ( x = 0; x < largura_sprite; x++) {
            cor_temp = dados_da_imagem_1[y][x];
            if (cor_temp == 0x00){
                dados_do_sprite[y][x] = APAGA;
            } else {
                dados_do_sprite[y][x] = converte_em_bgr(dados_da_imagem_2[y][x]);//Converte pixel por pixel do formato RGB para o BGR
            }
        }
    }
    //Escreve os dados de cada pixel na memoria de sprites. [end_base = offset * 400]
    cria_sprite(3600, dados_do_sprite);
}

void gera_sprite_estrela_offset10(){
    //Dados para formar um sprite 20x20 em formato RRR GGG BBB - 9 bits
    uint16_t dados_do_sprite[altura_sprite][largura_sprite];
    uint16_t cor_temp;
    int y, x;
    for ( y = 0; y < altura_sprite; y++) {
        for ( x = 0; x < largura_sprite; x++) {
            cor_temp = dados_da_imagem_1[y][x];
            if (cor_temp == 0xff){
                dados_do_sprite[y][x] = APAGA;
            } else {
                dados_do_sprite[y][x] = converte_em_bgr(dados_da_imagem_3[y][x]);//Converte pixel por pixel do formato RGB para o BGR
            }
        }
    }
    //Escreve os dados de cada pixel na memoria de sprites. [end_base = offset * 400]
    cria_sprite(4000, dados_do_sprite);
}

void grava_sprite_estrela(){
    gera_sprite_estrela_offset8();
    gera_sprite_estrela_offset9();
    gera_sprite_estrela_offset10();
}

void animacao_estrela(uint32_t pos_xy_20b, int sp){    
    while (1) {    
        int i;
        for (i=8; i<10; i++){
            exibe_sprite(sp, pos_xy_20b, i, 11);
            usleep(100000);
        }
    }
}