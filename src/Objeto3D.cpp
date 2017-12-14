#include "Objeto3D.h"
#include <sstream>
#include <iostream>

using namespace std;

typedef void (Projecao)(Ponto *);

void multMatriz(double A[][4], int ma, double B[][4], int mb, double C[][4])
{
    int i, j, k;
    double tmp[ma][4];

    for(i = 0; i < ma; ++i)
    {
        for(j = 0; j < mb; ++j)
        {
            tmp[i][j] = 0;
            for(k = 0;  k < 4; ++k)
            {
                tmp[i][j] += A[i][k] * B[k][j];
            }
        }
    }
    for(i = 0; i < ma; ++i)
    {
        for(j = 0; j < 4; ++j)
        {
            C[i][j] = tmp[i][j];
        }
    }
}

/**
 * \brief Realiza a projecao Cavaleira.
 * \param ponto Ponteiro para uma estrutura Ponto que representa um ponto.
 */
void cavaleira(Ponto *ponto)
{
    double x = cos((45 * M_PI / 180.0));
    double y = sin((45 * M_PI / 180.0));

    double cav[4][4] =
    {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {x, y, 0, 0},
        {0, 0, 0, 1}
    };

    double v[1][4];
    v[0][0] = ponto->x;
    v[0][1] = ponto->y;
    v[0][2] = ponto->z;
    v[0][3] = ponto->m;

    multMatriz(v, 1, cav, 4, v);

    ponto->x = v[0][0];
    ponto->y = v[0][1];
    ponto->z = v[0][2];
    ponto->m = v[0][3];
}

/**
 * \brief Realiza a projecao Cabinet.
 * \param ponto Ponteiro para uma estrutura Ponto que representa um ponto.
 */
void cabinet(Ponto *ponto)
{
    double x = cos((30 * M_PI / 180.0));
    double y = sin((30 * M_PI / 180.0));

    double cab[4][4] =
    {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {x, y, 0, 0},
        {0, 0, 0, 1}
    };

    double v[1][4];
    v[0][0] = ponto->x;
    v[0][1] = ponto->y;
    v[0][2] = ponto->z;
    v[0][3] = ponto->m;

    multMatriz(v, 1, cab, 4, v);

    ponto->x = v[0][0];
    ponto->y = v[0][1];
    ponto->z = v[0][2];
    ponto->m = v[0][3];
}

/**
 * \brief Realiza a projecao Isometrica.
 * \param ponto Ponteiro para uma estrutura Ponto que representa um ponto.
 */
void isometrica(Ponto *ponto)
{
    double v[1][4];
    double matriz[4][4] =
    {
        {0.707,  0.408, 0.000, 0.000},
        {0.000,  0.816, 0.000, 0.000},
        {0.707, -0.408, 0.000, 0.000},
        {0.000,  0.000, 0.000, 1.000}
    };
    v[0][0] = ponto->x;
    v[0][1] = ponto->y;
    v[0][2] = ponto->z;
    v[0][3] = ponto->m;

    multMatriz(v, 1, matriz, 4, v);

    ponto->x = v[0][0];
    ponto->y = v[0][1];
    ponto->z = v[0][2];
    ponto->m = v[0][3];
}

/**
 * \brief Realiza a projecao em Fuga em Z.
 * \param ponto Ponteiro para uma estrutura Ponto que representa um ponto.
 */
void fugaZ(Ponto *ponto)
{

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
    double v[1][4];

    v[0][0] = pontos.at(indice).x;
    v[0][1] = pontos.at(indice).y;
    v[0][2] = pontos.at(indice).z;
    v[0][3] = pontos.at(indice).m;

    multMatriz(v, 1, mTransformacoes, 4, v);

    ponto.x = v[0][0];
    ponto.y = v[0][1];
    ponto.z = v[0][2];
    ponto.m = v[0][3];

    ponto.x /= ponto.m;
    ponto.y /= ponto.m;
    ponto.z /= ponto.m;

    ponto.x += Tx;
    ponto.y += Ty;
    ponto.z += Tz;

    projeta(&ponto, vetor[iProjecao]);
    return ponto;
}

void Objeto3D::setEscalaEmX(double fator)
{
    mTransformacoes[0][0] += fator;
}

void Objeto3D::setEscalaEmY(double fator)
{
    mTransformacoes[1][1] += fator;
}

void Objeto3D::setEscalaEmZ(double fator)
{
    mTransformacoes[2][2] += fator;
}

void Objeto3D::setRotacaoEmY(double alfa)
{

    double mat[4][4] =
    {
        {cos(alfa), 0, -sin(alfa), 0},
        {        0, 1,          0, 0},
        {sin(alfa), 0,  cos(alfa), 0},
        {        0, 0,          0, 1}
    };

    multMatriz(mTransformacoes, 4, mat, 4, mTransformacoes);
}

void Objeto3D::setRotacaoEmX(double alfa)
{
    double mat[4][4] =
    {
        {1,          0,         0, 0},
        {0,  cos(alfa), sin(alfa), 0},
        {0, -sin(alfa), cos(alfa), 0},
        {0,          0,         0, 1}
    };

    multMatriz(mTransformacoes, 4, mat, 4, mTransformacoes);
}

void Objeto3D::setRotacaoEmZ(double alfa)
{

    double mat[4][4] =
    {
        {cos(alfa), -sin(alfa), 0, 0},
        {sin(alfa),  cos(alfa), 0, 0},
        {        0,          0, 1, 0},
        {        0,          0, 0, 1}
    };

    multMatriz(mTransformacoes, 4, mat, 4, mTransformacoes);
}

void Objeto3D::setTranslacaoEmX(double jump)
{
    Tx += jump;
}

void Objeto3D::setTranslacaoEmY(double jump)
{
    Ty += jump;
}

void Objeto3D::setTranslacaoEmZ(double jump)
{
    Tz += jump;
}

/**
 * \brief Carrega o arquivo que contem a relacao de pontos (x, y ,z) e linhas.
 * \param path String com o diretorio e nome do arquivo.
 */
void Objeto3D::carregaObj(string path)
{
    ifstream file;
    Ponto novoPonto; // Variavel temporaria para ler os pontos no arquivo.
    Cor novaCor;

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
        else if(letra == 'c')
        {
            iss >> novaCor.r >> novaCor.g >> novaCor.b;
            cores.push_back(novaCor);
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
