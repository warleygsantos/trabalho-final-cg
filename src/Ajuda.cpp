#include "Ajuda.h"

const char *fonteTTF = "fonts/verdana.ttf"; /**< Nome/local da fonte TTF.    */

/**
 * \brief Gera surface com as informaçoes de ajuda.
 * \return SDL_Surface* Ponteiro para surface com mapeamento das teclas.
 * \see imprimeTexto
 */
SDL_Surface *criaAjuda(int w, int h)
{
    SDL_Color color = {255, 255, 255, 0}; // Branco.

    /* Abre a o formato da fonte escolhida, e especifica o tamanho.           */
    TTF_Font *fonte = TTF_OpenFont(fonteTTF, 25);
    if(!fonte)
    {
        printf("Nao foi possivel abrir a fonte: %s\n", fonteTTF);
        exit(-1);
    }

    SDL_Surface *ajuda = imprimeTexto(fonte,
                                      " Translacao:\n"
                                      "     Em X: [A] [D]\n"
                                      "     Em Y: [W] [S]\n"
                                      "     Em Z: [R] [F]\n"
                                      "\n Rotacao:\n"
                                      "     Em torno de X: [K] [I]\n"
                                      "     Em torno de Y: [J] [L]\n"
                                      "     Em torno de Z: [U] [H]\n"
                                      "\n Escala:\n"
                                      "     Em X: [Ctrl+J] [Ctrl+L]\n"
                                      "     Em Y: [Ctrl+I] [Ctrl+K]\n"
                                      "     Em Z: [Ctrl+U] [Ctrl+H]\n"
                                      "\n Seleciona objeto: [TAB]\n"
                                      "\n Insere novo objeto: [INSERT]\n"
                                      "\n Reseta objeto: [ENTER]\n"
                                      "\n Apaga objeto: [DELETE]\n"
                                      "\n Sair: [ESC]", color, w, h);
    TTF_CloseFont(fonte);
    return ajuda;
}

/**
 * \brief Escreve o nome da projecao utilizada na parte superior direta da tela.
 * \param render Ponteiro para o render usado na tela.
 * \param i Indice da projecao: (0 = Cavaleira, 1 = Cabinet, 2 = Isometrica,
 *                                3 =Fuga em Z, 4 = Fuga em X e Z).
 * \return void
 */
void SProjecao(SDL_Renderer *render, int w)
{
    SDL_Color cor = {0, 255, 255, 0};

    TTF_Font *fonte = TTF_OpenFont(fonteTTF, 30);
    if(!fonte)
    {
        printf("Nao foi possivel abrir a fonte: %s\n", fonteTTF);
        exit(-1);
    }

    /* Escreve o nome da projecao em uma surface s.
     * Cria uma textura apartir da surface s.
     * Copia a textura para o render no canto superior direito da tela.
     */
    SDL_Surface *s = TTF_RenderText_Solid(fonte, "Ponto de fuga em Z", cor);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(render, s);
    SDL_Rect rect = {w - s->w, 0, s->w, s->h};
    SDL_RenderCopy(render, texture, NULL, &rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(s);
    TTF_CloseFont(fonte);
}

SDL_Surface *imprimeTexto(TTF_Font *fonte, std::string str, SDL_Color cor, int w, int h)
{
    /* Distancia recomendada para escrever as linhas.                         */
    unsigned int distanciaLinha = TTF_FontLineSkip(fonte);

    /* Quebra a string ao encontrar '\n' e insere cada pedaço em um vetor.    */
    std::vector<std::string> vLines;
    int pos = 0;
    while(pos != -1)
    {
        std::string strSub;
        pos = str.find('\n', 0);
        strSub = str.substr(0, pos);
        if(pos != -1)
        {
            str = str.substr(pos + 1);
        }
        vLines.push_back(strSub);
    }

    /* Surface de saida para o texto.                                          */
    SDL_Surface *sText = SDL_CreateRGBSurface(0, w, h, 32,
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
