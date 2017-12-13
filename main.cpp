/*******************************************************************************
 * \file main.cpp
 * \author Warley Goncalves dos Santos - 201300779
 * \date 29 Sep 2017
 * \brief Trabalho de Implementacao 4
 *
 * Universidade Federal de Goias(UFG)
 * Instituto de Informatica(INF)
 * Curso: Ciencia da Computacao
 *
 * Evolua o seu programa SDL, desenvolvido até o momento, para:
 *
 *   1. ter uma estrutura de dados representando objetos 3D no formato de
 * “arames”;
 *   2. possuir funções para criar, fazer um cópia e destruir objetos 3D (ou
 * seja, instâncias dessa estrutura de dados);
 *   3. ter funções para corretamente aplicar transformações de translação,
 * rotação e de escala sobre objetos 3D;
 *   4. possuir uma instância de um objeto 3D simples, como um cubo ou uma
 * pirâmide;
 *   5. desenhar esse objeto na tela utilizando um projeção cavaleira; e
 *   6. ter teclas do teclado associadas a comandos para transladar, rotacionar
 * e escalonar o objeto nos três eixos (X, Y e Z) - isso significa a definição
 * de, pelo menos, 18 teclas se consideramos incrementos e decrementos de
 * valores em cada eixo e para cada tipo de transformação. Configure também a
 * tecla F1 para mostrar uma mensagem de ajuda na tela descrevendo as teclas de
 * movimentação.
 *
 * Recomendação para a estrutura de dados dos objetos 3D:
 *
 * - Campos principais:
 *
 *  n (inteiro): número de pontos
 *  m (inteiro): número de linhas ligando os pontos
 *  pontos: matriz n x 3 para guardar as coordenadas (X,Y,Z) de cada ponto
 *  linhas: matriz m x 2 para guardar as indicações de quais pontos compõem cada
 * linha do objeto.
 *
 * - Campos secundários (eles podem estar na estrutura de dados ou serem
 * informados no momento de realizar processamentos e a projeção de um objeto):
 *
 *  Tx, Ty, Tz : posição do centro de referência do objeto no sistema de
 * referência do universo.
 *  Rx, Ry, Rz: ângulos de do objeto no sistema de referência do
 * universo.
 *  Sx, Sy, Sz: fator de escala do objeto no sistema de referência do universo.
 ******************************************************************************/

#include "Objeto3D.h"
#include "ColorSystem.h"
#include "Draw.h"
#include "Ajuda.h"

#ifdef _WIN32
#include <SDL.h>
#include <SDL_ttf.h>
#endif

#ifdef __linux
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#endif

#define FATOR 0.1 /**< Salto do valor de escala a cada distorcao.             */
#define ALFA 0.05 /**< Salto do valor do raio a cada rotacao.                 */
#define JUMP 5    /**< Salto do valor de posicao a cada deslocamento.         */

SDL_DisplayMode DM; /**< Usado para consultar a dimensao da tela.             */

using namespace std;

/**
 * \brief Instancia um novo objeto 3D.
 *  APENAS O QUADRADO ABILITADO ATE MODIFICAR O FORMATO DOS OUTROS ARQUIBOS.
 */
void changeObjeto3D(Objeto3D **obj)
{
    static int i = 0;
    const char *escolha[] = { "objetos3D/quadrado.war"
                              //       "objetos3D/coracao.war",
                              //     "objetos3D/letraA.war"
                            };
    delete *obj;
    *obj = new Objeto3D(escolha[i]);

    /* Posiciona o novo objeto perto do centro da tela.                       */
    (*obj)->setTranslacaoEmX(DM.w/2);
    (*obj)->setTranslacaoEmY(DM.h/2);

    ++i;
    if(i >= 1)
    {
        i = 0;
    }
}

/**
 * \brief Tratamento de açoes ao se precionar uma tecla.
 * \param event Ponteiro para o evento que causou a chamada.
 * \param obj Ponteiro duplo para o objeto 3D.
 * \param render Ponteiro para o render da janela.
 * \param cor Cor do poligno.
 * \param ajuda Ponteiro para o surface com a tela de ajuda.
 * \return False caso o usuario deseja fechar o programa. True continua.
 */
bool handleEventKey(
    SDL_Event *event, Objeto3D **obj, SDL_Renderer *render, SDL_Surface *ajuda)
{
    /* Permite combinaçao de teclas. True: SPACE precionado.                  */
    static bool spaceIsDown = false;

    /* True para tecla de ajuda precionada.                                   */
    static bool mostraAjuda = false;

    /* Indeice da projeçao escolhida. Default: Cavaleira.
     *   0 = Cavaleira, 1 = Cabinet, 2 = Isometrica, 3 = Fuga em Z
     *   4 = Fuga em X e Z).
     */
    static int iProjecao = 0;

    /* Eventos ao precionar tecla.                                            */
    if(event->type == SDL_KEYDOWN)
    {
        if(event->key.keysym.sym == SDLK_a)
        {
            (*obj)->setTranslacaoEmX(-JUMP); // Vai para esquerda.
        }
        else if(event->key.keysym.sym == SDLK_d)
        {
            (*obj)->setTranslacaoEmX(JUMP); // Vai para a diretira.
        }
        else if(event->key.keysym.sym == SDLK_w)
        {
            (*obj)->setTranslacaoEmY(-JUMP); // Vai para cima.
        }
        else if(event->key.keysym.sym == SDLK_s)
        {
            (*obj)->setTranslacaoEmY(JUMP); // Vai para baixo.
        }
        else if(event->key.keysym.sym == SDLK_r)
        {
            (*obj)->setTranslacaoEmZ(-JUMP); // Vai para frente.
        }
        else if(event->key.keysym.sym == SDLK_f)
        {
            (*obj)->setTranslacaoEmZ(JUMP); // Vai para tras.
        }
        else if(event->key.keysym.sym == SDLK_j && !spaceIsDown)
        {
            (*obj)->setRotacaoEmY(-ALFA); // Roda eixo y sentido horario.
        }
        else if(event->key.keysym.sym == SDLK_l && !spaceIsDown)
        {
            (*obj)->setRotacaoEmY(ALFA); // Roda eixo y sentido anti horario.
        }
        else if(event->key.keysym.sym == SDLK_k && !spaceIsDown)
        {
            (*obj)->setRotacaoEmX(-ALFA); // Roda entorno eixo x.
        }
        else if(event->key.keysym.sym == SDLK_i && !spaceIsDown)
        {
            (*obj)->setRotacaoEmX(ALFA); // Roda entorno eixo x.
        }
        else if(event->key.keysym.sym == SDLK_u && !spaceIsDown)
        {
            (*obj)->setRotacaoEmZ(-ALFA); // Roda entorno eixo Z.
        }
        else if(event->key.keysym.sym == SDLK_h && !spaceIsDown)
        {
            (*obj)->setRotacaoEmZ(ALFA); // Roda entorno eixo Z.
        }
        else if(event->key.keysym.sym == SDLK_j && spaceIsDown)
        {
            (*obj)->setEscalaEmX(-FATOR); // Achata no eixo X.
        }
        else if(event->key.keysym.sym == SDLK_l && spaceIsDown)
        {
            (*obj)->setEscalaEmX(FATOR); // Estica pelo eixo X.
        }
        else if(event->key.keysym.sym == SDLK_k && spaceIsDown)
        {
            (*obj)->setEscalaEmY(-FATOR); // Achata no eixo Y.
        }
        else if(event->key.keysym.sym == SDLK_i && spaceIsDown)
        {
            (*obj)->setEscalaEmY(FATOR); // Estica pelo eixo Y.
        }
        else if(event->key.keysym.sym == SDLK_u && spaceIsDown)
        {
            (*obj)->setEscalaEmZ(-FATOR); // Achata pelo eixo X.
        }
        else if(event->key.keysym.sym == SDLK_h && spaceIsDown)
        {
            (*obj)->setEscalaEmZ(FATOR); // Estica pelo eixo Z.
        }
        else if(event->key.keysym.sym == SDLK_SPACE)
        {
            spaceIsDown = true; // Tecla SPACE esta precionada.
        }
        else if(event->key.keysym.sym == SDLK_ESCAPE)
        {
            return false; // Usuario clicou em ESC. Programa termina.
        }
        else if(event->key.keysym.sym == SDLK_RETURN)
        {
            changeObjeto3D(obj); // Muda de objeto 3D.
        }
        /* Quando o evento for causado por precionar F1:
         * (Apenas manipula a variaval de controle)
         *      Tela de ajuda some se ja estava aparecendo.
         *      Mostra ajuda se F1 for precionado.
         */
        else if(event->key.keysym.sym == SDLK_F1)
        {
            if(mostraAjuda)
            {
                mostraAjuda = false; // Faz ajuda sumir.
            }
            else
            {
                mostraAjuda = true; // Mostra ajuda na tela.
            }
        }
        /* Percorre pelas projecoes de forma circurlar.                       */
        else if(event->key.keysym.sym == SDLK_p)
        {
            ++iProjecao;
            if(iProjecao >= 5)
            {
                iProjecao = 0;
            }
        }
    }
    /* Eventos ao soltar tecla.                                               */
    else if(event->type == SDL_KEYUP)
    {
        if(event->key.keysym.sym == SDLK_SPACE)
            spaceIsDown = false; // A tecla SPACE nao esta precionada.
        else return true; /* Nao atualiza a tela se caiu em uma das condicoes.*/
    }
    else return true; /* Nao atualiza a tela se caiu em uma das condicoes     */
    /* Escolhe preto como cor de fundo e limpa a tela.                        */
    SDL_SetRenderDrawColor(render, 0x0, 0x0, 0x0, 0x0);
    SDL_RenderClear(render);
    /* Desenha o objeto 3D na tela.                                           */
    desenhaPoligono(render, *obj, iProjecao, DM.h);
    /* Mostra a ajuda por cima do desenho.                                    */
    if(mostraAjuda)
    {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(render, ajuda);
        SDL_RenderCopy(render, texture, NULL, NULL);
        SDL_DestroyTexture(texture);
    }
    /* Mostra a projecao utilizada.                                           */
    SProjecao(render, iProjecao, DM.w);
    /* Renderiza.                                                             */
    SDL_RenderPresent(render);
    return true; // Continua em execucao.
}

int main(int argc, char *argv[])
{
    /* Inicializa as bibliotecas de SDL Video e TTF.                          */
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        SDL_Log("Can't init %s", SDL_GetError());
    TTF_Init();

    /* Obtem as medidas de tamanho da tela.                                   */
    SDL_GetCurrentDisplayMode(0, &DM);

    /* Instancia um objeto 3D e o posiciona perto do centro da tela.          */
    Objeto3D *obj = new Objeto3D("objetos3D/quadrado.war");
    obj->setTranslacaoEmX(DM.w/2);
    obj->setTranslacaoEmY(DM.h/2);

    /* Cria uma janela e associa um render a ela.                             */
    SDL_Window *window = SDL_CreateWindow("Trabalho Pratico 5",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          DM.w, DM.h, 1);
    SDL_Renderer* render = SDL_CreateRenderer(window, -1,
                           SDL_RENDERER_ACCELERATED);

    /* Desenha o poligno e renderiza.                                         */
    desenhaPoligono(render, obj, 0, DM.h);
    /* Escreve qual projecao esta sendo usada. Defaul: Cavaleira.             */
    SProjecao(render, 0, DM.w);

    /* Renderiza.                                                             */
    SDL_RenderPresent(render);

    /* Cria a mensagem de ajuda.                                              */
    SDL_Surface *ajuda = criaAjuda(DM.w, DM.h);

    bool running = true; // Controle para continuar executando.
    SDL_Event event;

    /* Mantem o programa em executacao ate usuario escolher opcao de sair.    */
    while(running)
    {
        while(SDL_PollEvent(&event)) // Pode ser muitos eventos.
        {
            switch(event.type)
            {
            case SDL_QUIT: // A janela foi fechada.
                running = false;
                break;
            case SDL_KEYDOWN: // Eventos de precionar tecla.
                running = handleEventKey(&event, &obj, render, ajuda);
                break;
            case SDL_KEYUP: // Eventosde soltar tecla.
                running = handleEventKey(&event, &obj, render, ajuda);
                break;
            }
        }
        SDL_Delay(35); // Delay para popular processamento.
    }
    delete obj;
    SDL_FreeSurface(ajuda);
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    return 0;
}
