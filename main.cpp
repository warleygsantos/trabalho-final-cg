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

#include <iostream>
#include <Objeto3D.h>
#include <ColorSystem.h>
#include <array>
#include <string>

#ifdef _WIN32
#include <SDL.h>
#include <SDL_ttf.h>
#endif

#ifdef __linux
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#endif

typedef struct No
{
    int x;
    int y;
} No;

#define FATOR 0.1 /**< Salto do valor de escala a cada distorcao.             */
#define ALFA 0.05 /**< Salto do valor do raio a cada rotacao.                 */
#define JUMP 5    /**< Salto do valor de posicao a cada deslocamento.         */

SDL_DisplayMode DM; /**< Usado para consultar a dimensao da tela.             */
const char *fonteName = "fonts/verdana.ttf"; /**< Nome/local da fonte TTF.    */

using namespace std;

/** \brief Desenha linha usando o metodo DDA.
 *
 * \param render Render correspondente a janela utilizada.
 * \param xi Coordenada x do ponto inicial.
 * \param yi Coordenada y do ponto inicial.
 * \param xf Coordenada x do ponto final.
 * \param yf Coordenada y do ponto final.
 * \param calor Cor utilizada para pintar a linha.
 * \return void
 */
void linhaDDA(SDL_Renderer *render,
              double xi, double yi, double xf, double yf)
{
    double dx = xf - xi;
    double dy = yf - yi;
    int steps;
    int k;
    double incWIDTH, incHEIGHT;
    double x = xi;
    double y = yi;

    if(fabs(dx) > fabs(dy))
        steps = fabs(dx);
    else steps = fabs(dy);

    incWIDTH = dx / (double) steps;
    incHEIGHT = dy / (double) steps;

    SDL_RenderDrawPoint(render, x, y);
    for(k = 0; k < steps; ++k)
    {
        x += incWIDTH;
        y += incHEIGHT;
        SDL_RenderDrawPoint(render, x, y);
    }
}

void mostraNumero(SDL_Renderer *render, SDL_Color cor, string num, Ponto ponto)
{
    TTF_Font *fonte = TTF_OpenFont(fonteName, 15);
    if(!fonte)
    {
        printf("Nao foi possivel abrir a fonte: %s\n", fonteName);
        exit(-1);
    }
    cor.r = 150;
    cor.g = 150;
    cor.b = 150;

    /* Escreve o nome da projecao em uma surface s.
     * Cria uma textura apartir da surface s.
     * Copia a textura para o render no canto superior direito da tela.
     */
    SDL_Surface *s = TTF_RenderText_Solid(fonte, num.c_str(), cor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(render, s);
    SDL_Rect rect = {(int) ponto.x, (int) ponto.y, s->w, s->h};
    SDL_RenderCopy(render, texture, NULL, &rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(s);
    TTF_CloseFont(fonte);
}


double calcula1porM(double P[][2], int L[][2], int indice)
{
    double deltaX = (double) (P[(L[indice][0])][0] - P[(L[indice][1])][0]);
    double deltaY = (double) (P[(L[indice][0])][1] - P[(L[indice][1])][1]);

    if(deltaY)
        return deltaX / deltaY;
    return 0;
}

/**
 * \brief Limpa a tela e desenha o poligno.
 * \param render Render correspondente a janela utilizada.
 * \param obj Instancia do objeto a ser desenhado.
 * \param corBorda Cor da borda do poligono.
 * \param projecao Indice da projecao.
 *  (0 = cavaleira, 1 = cabinet, 2 = isometrica, 3 = fugaZ, 4 = fugaXZ).
 * \see linhaDDA
 * \return void
 */
void desenhaPoligono(SDL_Renderer *render, Objeto3D* obj, int projecao)
{
    bool preenche = true;
    vector<No> nodes;

    if(preenche)
    {
        int c = 0;
        std::vector<vector<int>>::iterator it;
        for (it = obj->faces.begin(); it != obj->faces.end(); ++it, ++c)
        {

            /*-----------------------Esconde face-----------------------------*/
            Ponto P1 = obj->getPonto((*it).at(0), projecao);
            Ponto P2 = obj->getPonto((*it).at(1), projecao);
            Ponto P3 = obj->getPonto((*it).at(2), projecao);

            Ponto d1, d2;

            d1.x = P3.x - P1.x;
            d1.y = P3.y - P1.y;
            d2.x = P3.x - P2.x;
            d2.y = P3.y - P2.y;

            if ((d1.x * d2.y) - (d1.y * d2.x) < (double) 0)
                continue;

            /*-----------------------Le a cor da face-------------------------*/
            Cor cor= obj->cores[c];

            RGB nrgb;
            nrgb.r = cor.r;
            nrgb.b = cor.b;
            nrgb.g = cor.g;

            HSV nhsv = rgb2hsv(nrgb);

            //nhsv.v  = 255;

            nrgb = hsv2rgb(nhsv);
            cor.r = nrgb.r;
            cor.g = nrgb.g;
            cor.b = nrgb.b;

            /* Configura a cor do poligno.                                    */
            SDL_SetRenderDrawColor(render, cor.r, cor.g, cor.b, 0);

            /*-----------------------desenha a face---------------------------*/
            int L[(*it).size()][2];
            double P[obj->pontos.size()][2];

            for(unsigned int i = 0; i < obj->pontos.size(); ++i)
            {
                P[i][0] = obj->getPonto(i, projecao).x;
                P[i][1] = obj->getPonto(i, projecao).y;
            }
            unsigned int i;
            for(i = 0; i <  (*it).size() -1; ++i)
            {
                L[i][0] = (*it).at(i);
                L[i][1] = (*it).at(i + 1);
            }
            L[i][0] = (*it).at(i);
            L[i][1] = (*it).at(0);
            double tLados[(*it).size()][4];
            for(unsigned int i = 0; i < (*it).size(); ++i)
            {
                //   y1 < y2
                if(P[(L[i][0])][1] < P[(L[i][1])][1])
                {
                    tLados[i][0] = P[(L[i][0])][1]; //** y minimo.
                    tLados[i][1] = P[(L[i][1])][1]; // /** y maximo.
                    tLados[i][2] = P[(L[i][0])][0]; ///** x para y min.
                    tLados[i][3] = calcula1porM(P, L, i);// /**  1/m
                }
                else
                {
                    tLados[i][0] = P[(L[i][1])][1]; // /** y minimo.
                    tLados[i][1] = P[(L[i][0])][1]; ///** y maximo.
                    tLados[i][2] = P[(L[i][1])][0]; //** x para y min.
                    tLados[i][3] = calcula1porM(P, L, i); ///** 1/m
                }
            }

            int yVarredura;
            for(yVarredura = 0; yVarredura < DM.h; ++yVarredura)
            {
                for(int unsigned i = 0; i < (*it).size(); ++i)
                {
                    /* Para eliminar os lados do polígono, os quais a linha de
                    varredura não intercepta, são verificadas as seguintes condições
                        Yvarredura > Ymax
                        Yvarredura < Ymi */
                    if(yVarredura < tLados[i][0] || yVarredura > tLados[i][1])
                    {
                        continue;
                    }
                    No novo;
                    novo.x = ((tLados[i][3] * (yVarredura - tLados[i][0]))) + tLados[i][2];
                    novo.y = yVarredura;
                    nodes.push_back(novo);
                    if(tLados[i][1] == yVarredura)
                    {
                        novo.x = ((tLados[i][3] * (yVarredura - tLados[i][0])) + tLados[i][2]);
                        novo.y = yVarredura;
                        nodes.push_back(novo);
                    }
                }

                for(unsigned int i = 1; i < nodes.size(); ++i)
                {
                    linhaDDA(render, nodes.at(i - 1).x, nodes.at(i - 1).y, nodes.at(i).x, nodes.at(i).y);
                }
                nodes.clear();
            }
        }
    }
}
/**
 * \brief Retorna uma surface com a string.
 * \param fonte Fonte do texto a ser escrito.
 * \param str String a ser escrita.
 * \param cor Cor do texto.
 * \see criAjuda
 * \return SDL_Surface *
 */
SDL_Surface *imprimeTexto(TTF_Font *fonte, string str, SDL_Color cor)
{
    /* Distancia recomendada para escrever as linhas.                         */
    unsigned int distanciaLinha = TTF_FontLineSkip(fonte);

    /* Quebra a string ao encontrar '\n' e insere cada pedaço em um vetor.    */
    std::vector<string> vLines;
    int pos = 0;
    while(pos != -1)
    {
        string strSub;
        pos = str.find('\n', 0);
        strSub = str.substr(0, pos);
        if(pos != -1)
        {
            str = str.substr(pos + 1);
        }
        vLines.push_back(strSub);
    }

    /* Surface de saida para o texto.                                          */
    SDL_Surface *sText = SDL_CreateRGBSurface(0, DM.w, DM.h, 32,
                         0, 0, 0, 0);
    SDL_SetSurfaceBlendMode(sText, SDL_BLENDMODE_ADD);
    /* Surface temporaria.                                                    */
    SDL_Surface *sTemp = NULL;

    /* Para cada linha:
     *      Escreve a linha em uma surface temporaria.
     *      Copia a surface temporaria p/ surface de saida na posicao correta.
     */
    for(unsigned int i = 0; i < vLines.size(); i++)
    {
        sTemp = TTF_RenderText_Solid(fonte, vLines[i].c_str(), cor);
        SDL_Rect newSDL_Rect = {0, (signed int) (i * distanciaLinha), 0, 0};
        SDL_BlitSurface(sTemp, NULL, sText, &newSDL_Rect);
        SDL_FreeSurface(sTemp);
    }
    return sText;
}

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
 * \brief Escreve o nome da projecao utilizada na parte superior direta da tela.
 * \param render Ponteiro para o render usado na tela.
 * \param i Indice da projecao: (0 = Cavaleira, 1 = Cabinet, 2 = Isometrica,
 *                                3 =Fuga em Z, 4 = Fuga em X e Z).
 * \return void
 */
void SProjecao(SDL_Renderer *render, int i)
{
    SDL_Color cor = {0, 255, 255, 0};
    std::array<string, 5> cProjecao = {"Cavaleira", "Cabinet", "Isometrica",
                                       "Fuga em Z", "Fuga em X e Z"
                                      };
    TTF_Font *fonte = TTF_OpenFont(fonteName, 30);
    if(!fonte)
    {
        printf("Nao foi possivel abrir a fonte: %s\n", fonteName);
        exit(-1);
    }

    /* Escreve o nome da projecao em uma surface s.
     * Cria uma textura apartir da surface s.
     * Copia a textura para o render no canto superior direito da tela.
     */
    SDL_Surface *s = TTF_RenderText_Solid(fonte, cProjecao.at(i).c_str(), cor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(render, s);
    SDL_Rect rect = {DM.w - s->w, 0, s->w, s->h};
    SDL_RenderCopy(render, texture, NULL, &rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(s);
    TTF_CloseFont(fonte);
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
    desenhaPoligono(render, *obj, iProjecao);

    /* Mostra a ajuda por cima do desenho.                                    */
    if(mostraAjuda)
    {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(render, ajuda);
        SDL_RenderCopy(render, texture, NULL, NULL);
        SDL_DestroyTexture(texture);
    }

    /* Mostra a projecao utilizada.                                           */
    SProjecao(render, iProjecao);

    /* Renderiza.                                                             */
    SDL_RenderPresent(render);
    return true; // Continua em execucao.
}

/**
 * \brief Gera surface com as informaçoes de ajuda.
 * \return SDL_Surface* Ponteiro para surface com mapeamento das teclas.
 * \see imprimeTexto
 */
SDL_Surface *criaAjuda()
{
    SDL_Color color = {255, 255, 255, 0}; // Branco.

    /* Abre a o formato da fonte escolhida, e especifica o tamanho.           */
    TTF_Font *fonte = TTF_OpenFont(fonteName, 25);
    if(!fonte)
    {
        printf("Nao foi possivel abrir a fonte: %s\n", fonteName);
        exit(-1);
    }

    SDL_Surface *ajuda = imprimeTexto(fonte,
                                      " Translacao:\n"
                                      "     Em torno de X: [A] [D]\n"
                                      "     Em torno de Y: [W] [S]\n"
                                      "     Em torno de z: [R] [F]\n"
                                      "\n Rotacao:\n"
                                      "     Em torno de X: [J] [L]\n"
                                      "     Em torno de Y: [I] [K]\n"
                                      "     Em torno de Z: [U] [H]\n"
                                      "\n Escala:\n"
                                      "    Em torno de X: [Ctrl+J] [Ctrl+L]\n"
                                      "    Em torno de Y: [Ctrl+I] [Ctrl+K]\n"
                                      "    Em torno de Z: [Ctrl+U] [Ctrl+H]\n"
                                      "\n Novo objeto:\n"
                                      "    [ENTER]\n"
                                      "\n Troca de projecao:\n"
                                      "    [P]\n"
                                      "\n Sair:\n"
                                      "    [ESC]", color);
    TTF_CloseFont(fonte);
    return ajuda;
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
    desenhaPoligono(render, obj, 0);
    /* Escreve qual projecao esta sendo usada. Defaul: Cavaleira.             */
    SProjecao(render, 0);

    /* Renderiza.                                                             */
    SDL_RenderPresent(render);

    /* Cria a mensagem de ajuda.                                              */
    SDL_Surface *ajuda = criaAjuda();

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
