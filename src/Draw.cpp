#include "Draw.h"

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
void desenhaPoligono(SDL_Renderer *render, Objeto3D* obj, int h)
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
            Ponto P1 = obj->getPonto((*it).at(0));
            Ponto P2 = obj->getPonto((*it).at(1));
            Ponto P3 = obj->getPonto((*it).at(2));

            Ponto d1, d2;

            d1.x = P3.x - P1.x;
            d1.y = P3.y - P1.y;
            d2.x = P3.x - P2.x;
            d2.y = P3.y - P2.y;

            if ((d1.x * d2.y) - (d1.y * d2.x) < (double) 0)
                continue;

            /*-----------------Le a cor da face E trata luz-------------------*/

            Ponto pn;

            pn.x = (P3.y - P2.y) * (P1.z - P2.z) - (P1.y - P2.y) * (P3.z - P2.z);
            pn.y = (P3.z - P2.z) * (P1.x - P2.x) - (P1.z - P2.z) * (P3.x - P2.x);
            pn.z = (P3.x - P2.x) * (P1.y - P2.y) - (P1.x - P2.x) * (P3.y - P2.y);

            Ponto pLuz;
            pLuz.x = 0;
            pLuz.y = 0;
            pLuz.z = 100;
          //  SDL_SetRenderDrawColor(render, 255, 255, 255, 255);
         //   SDL_RenderDrawPoint(render, pLuz.x, pLuz.y);
            double normal = pn.x * (pLuz.x - P2.x) + pn.y * (pLuz.y - P2.y) + pn.z * (pLuz.z - P2.z);
            /*-----------------------Le a cor da face-------------------------*/
            Cor cor = obj->cores[c];

            RGB rgb;
            rgb.r = cor.r;
            rgb.b = cor.b;
            rgb.g = cor.g;

            HSV hsv = rgb2hsv(rgb);

            hsv.v  = 255;
            if(normal < 0)
                hsv.v = 25;

            rgb = hsv2rgb(hsv);
            cor.r = rgb.r;
            cor.g = rgb.g;
            cor.b = rgb.b;

            /* Configura a cor do poligno.                                    */
            SDL_SetRenderDrawColor(render, cor.r, cor.g, cor.b, 0);

            /*-----------------------desenha a face---------------------------*/
            int L[(*it).size()][2];
            double P[obj->pontos.size()][2];

            for(unsigned int i = 0; i < obj->pontos.size(); ++i)
            {
                P[i][0] = obj->getPonto(i).x;
                P[i][1] = obj->getPonto(i).y;
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
            for(yVarredura = 0; yVarredura < h; ++yVarredura)
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
