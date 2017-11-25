#include "Objeto3D.h"
#include <sstream>
#include <iostream>

using namespace std;

typedef void (Projecao)(Ponto *);

void Objeto3D::multiplica(double A[4][4], double B[4][4], double out[4][4])
{
    double mat[4][4];
    int i, j, k;

    for(i = 0; i < 4; ++i)
        for(j = 0; j < 4; ++j)
            mat[i][j] = 0;

    for(i = 0; i < 4; ++i)
        for(j = 0; j < 4; ++j)
            for(k = 0; k < 4; ++k)
                mat[i][j] += A[i][k] * B[j][k];

    for(i = 0; i < 4; ++i)
        for(j = 0; j < 4; ++j)
            out[i][j] = mat[i][j];
}

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
    double z = 1 - (ponto->z / 20);
    double x = 1 - (ponto->x / 20);
    double v[2];

    v[0] = (ponto->x) / z;
    v[1] = (ponto->y) / z;

    v[1] += (ponto->y) / x;
    v[0] += (ponto->x) / x;

    ponto->x = v[0];
    ponto->y = v[1];
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

    ponto.x += matrizT[0][3];
    ponto.y += matrizT[1][3];
    ponto.z += matrizT[2][3];

    projeta(&ponto, vetor[iProjecao]);

    ponto.x /= ponto.m;
    ponto.y /= ponto.m;
    ponto.z /= ponto.m;

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
    double rotacao[4][4] = { cos(alfa), 0, -sin(alfa), 0,
                             0, 1,          0, 0,
                             sin(alfa), 0,  cos(alfa), 0,
                             0, 0,          0, 1
                           };

    multiplica(M, rotacao, M);
}

void Objeto3D::setRotacaoEmX(double alfa)
{
    double rotacao[4][4] = { 1,          0,         0, 0,
                             0,  cos(alfa), sin(alfa), 0,
                             0, -sin(alfa), cos(alfa), 0,
                             0,          0,         0, 1
                           };

    multiplica(M, rotacao, M);
}

void Objeto3D::setRotacaoEmZ(double alfa)
{
    double rotacao[4][4] = { cos(alfa), sin(alfa), 0, 0,
                             -sin(alfa), cos(alfa), 0, 0,
                             0,         0, 1, 0,
                             0,         0, 0, 1
                           };
    multiplica(M, rotacao, M);
}

void Objeto3D::setTranslacaoEmX(double jump)
{
    double tranlada[4][4] = { 1, 0, 0, 0,
                              0, 1, 0, 0,
                              0, 0, 1, 0,
                              jump, 0, 0, 1
                            };

    multiplica(matrizT, tranlada, matrizT);
}

void Objeto3D::setTranslacaoEmY(double jump)
{
    double tranlada[4][4] = {1, 0, 0, 0,
                             0, 1, 0, 0,
                             0, 0, 1, 0,
                             0, jump,0, 1
                            };
    multiplica(matrizT, tranlada, matrizT);
}

void Objeto3D::setTranslacaoEmZ(double jump)
{
    double tranlada[4][4] = {1, 0, 0, 0,
                             0, 1, 0, 0,
                             0, 0, 1, 0,
                             0, 0, jump, 1
                            };
    multiplica(matrizT, tranlada, matrizT);
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
