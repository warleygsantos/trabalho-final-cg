#include "Objeto3D.h"
#include <sstream>
#include <iostream>

using namespace std;

typedef void (Projecao)(Ponto *);

/**
 * \brief Realiza a projecao Cavaleira.
 * \param ponto Ponteiro para uma estrutura Ponto que representa um ponto.
 */
void cavaleira(Ponto *ponto)
{
    double cav = 0.707106781 * ponto->z;

    ponto->x += cav;
    ponto->y += cav;
}

/**
 * \brief Realiza a projecao Cabinet.
 * \param ponto Ponteiro para uma estrutura Ponto que representa um ponto.
 */
void cabinet(Ponto *ponto)
{
    double cab = 0.353553391 * ponto->z;

    ponto->x += cab;
    ponto->y += cab;
}

/**
 * \brief Realiza a projecao Isometrica.
 * \param ponto Ponteiro para uma estrutura Ponto que representa um ponto.
 */
void isometrica(Ponto *ponto)
{
    double v[4];
    v[0] = ponto->x;
    v[1] = ponto->y;
    v[2] = ponto->z;
    v[3] = ponto->m;

    v[0] = (ponto->x + ponto->z) * 0.707;
    v[1] = (ponto->x * 0.408) + (ponto->y * 0.816) + (ponto->z * -0.408);

    ponto->x = v[0];
    ponto->y = v[1];
}

/**
 * \brief Realiza a projecao em Fuga em Z.
 * \param ponto Ponteiro para uma estrutura Ponto que representa um ponto.
 */
void fugaZ(Ponto *ponto)
{
    ponto->x = (20 / ponto->z) * ponto->x;
    ponto->y = (20 / ponto->z) * ponto->y;
}

/**
 * \brief Realiza a projecao Fuga em X e Z.
 * \param ponto Ponteiro para uma estrutura Ponto que representa um ponto.
 */
void fugaXZ(Ponto *ponto)
{

}

/* Vetor de ponteira para as funcoes de projecaol.                            */
Projecao (*vetor[5]) = {cavaleira, cabinet, isometrica, fugaZ, fugaXZ};

/**
 * \brief Responsavel por chamar a projecao de forma generica.
 * \param ponto Ponteiro para o ponto que se deseja projetar.
 * \param projecao Ponteiro para a funcao de projecao.
 */
void projeta(Ponto *ponto, Projecao projecao)
{
    projecao(ponto);
}

/**
 * \brief Retorna Ponto (x, y, z).
 * \param indice Indice correspondente ao ponto.
 * \param projecao Indice correspondente a projecao que deve ser usada.
 * \return Ponto com operacoes de translacao, escala, rotacao e projecao.
 */
Ponto Objeto3D::getPonto(int indice, int iProjecao)
{
    Ponto ponto;
    double v[4];

    v[0] = pontos.at(indice).x;
    v[1] = pontos.at(indice).y;
    v[2] = pontos.at(indice).z;
    v[3] = pontos.at(indice).m;

    ponto.x = (v[0] * M[0][0]) + (v[1] * M[0][1]) + (v[2] * M[0][2]) + (v[3] * M[0][3]);
    ponto.y = (v[0] * M[1][0]) + (v[1] * M[1][1]) + (v[2] * M[1][2]) + (v[3] * M[1][3]);
    ponto.z = (v[0] * M[2][0]) + (v[1] * M[2][1]) + (v[2] * M[2][2]) + (v[3] * M[2][3]);
    ponto.m = (v[0] * M[3][0]) + (v[1] * M[3][1]) + (v[2] * M[3][2]) + (v[3] * M[3][3]);

    ponto.x /= ponto.m;
    ponto.y /= ponto.m;
    ponto.z /= ponto.m;

    ponto.x += transladaX;
    ponto.y += transladaY;
    ponto.z += transladaZ;

    ponto.y = ponto.y;
    projeta(&ponto, vetor[iProjecao]);
    return ponto;
}

void Objeto3D::setEscalaEmX(double fator)
{
    M[0][0] += fator;
}

void Objeto3D::setEscalaEmY(double fator)
{
    M[1][1] += fator;
}

void Objeto3D::setEscalaEmZ(double fator)
{
    M[2][2] += fator;
}

void Objeto3D::setRotacaoEmY(double alfa)
{
    double v[3];

    for(unsigned int i = 0; i < pontos.size(); ++i)
    {
        v[0] = (pontos[i].x * cos(alfa)) + (pontos[i].z * sin(alfa));
        v[2] = ((pontos[i].x * sin(alfa)) * -1) + (pontos[i].z * cos(alfa));

        pontos[i].x = v[0];
        pontos[i].z = v[2];
    }
}

void Objeto3D::setRotacaoEmX(double alfa)
{
    double v[3];

    for(unsigned int i = 0; i < pontos.size(); ++i)
    {
        v[1] = pontos[i].y * cos(alfa) + (pontos[i].z * -sin(alfa));
        v[2] = (pontos[i].y * sin(alfa)) + (pontos[i].z * cos(alfa));

        pontos[i].y = v[1];
        pontos[i].z = v[2];
    }
}

void Objeto3D::setRotacaoEmZ(double alfa)
{

    double v[3];

    for(unsigned int i = 0; i < pontos.size(); ++i)
    {
        v[0] = pontos[i].x * cos(alfa) + (pontos[i].y * -sin(alfa));
        v[1] = (pontos[i].x * sin(alfa)) + (pontos[i].y * cos(alfa));

        pontos[i].x = v[0];
        pontos[i].y = v[1];
    }
}

void Objeto3D::setTranslacaoEmX(double jump)
{
    transladaX += jump;
}

void Objeto3D::setTranslacaoEmY(double jump)
{
    transladaY += jump;
}

void Objeto3D::setTranslacaoEmZ(double jump)
{
    transladaZ += jump;
}

/**
 * \brief Carrega o arquivo que contem a relacao de pontos (x, y ,z) e linhas.
 * \param path String com o diretorio e nome do arquivo.
 */
void Objeto3D::carregaObj(string path)
{
    ifstream file;
    Ponto novoPonto; // Variavel temporaria para ler os pontos no arquivo.

    file.open(path.c_str());
    if(!file.is_open())
    {
        printf("Arquivo %s nao foi aberto!\n", path.c_str());
        exit(-1);
    }

    std::string entrada;
    char letra;
    int indiceVertice;

    while (std::getline(file, entrada))
    {
        std::istringstream iss(entrada.c_str());
        iss >> letra;

        if(letra == 'v')
        {
            iss >> novoPonto.x >> novoPonto.y >> novoPonto.z;
            novoPonto.m = 1;
            pontos.push_back(novoPonto);
        }
        else if(letra  == 'f')
        {
            vector<int> novaFace;
            while(iss >> indiceVertice)
            {
                novaFace.push_back(indiceVertice);
            }
            faces.push_back(novaFace);
        }
    }
    file.close();
}

/**
 * \brief Construtor para o objeto 3D.
 * \param path Diretorio\nome do arquivo de pontos e linhas.
 */
Objeto3D::Objeto3D(string path)
{
    /* Carrega o arquivo com os pontos e linhas.                              */
    carregaObj(path);
}

Objeto3D::~Objeto3D()
{

}
