#ifndef OBJETO3D_H
#define OBJETO3D_H

#include <string>
#include <stdlib.h>
#include <fstream>
#include <math.h>
#include <vector>

typedef struct Ponto
{
    double x;
    double y;
    double z;
    double m;
} Ponto;

typedef struct Linha
{
    unsigned int P1;
    unsigned int P2;
} Linha;

typedef struct Cor
{
    unsigned int r;
    unsigned int g;
    unsigned int b;
}Cor;

class Objeto3D
{
public:
    void multiplica(double A[4][4], double B[4][4], double out[4][4]);
    Objeto3D(std::string, int, int);
    std::vector<Ponto> pontos;
    std::vector<Linha> linhas;
    std::vector<std::vector<int>> faces;
    std::vector<Cor> cores;
    void setTranslacaoEmX(double);
    void setTranslacaoEmY(double);
    void setTranslacaoEmZ(double);
    void setRotacaoEmX(double);
    void setRotacaoEmY(double);
    void setRotacaoEmZ(double);
    void setEscalaEmX(double);
    void setEscalaEmY(double);
    void setEscalaEmZ(double);
    void fugaZ(Ponto *);
    Ponto getPonto(int);
    virtual ~Objeto3D();
    double mTransformacoes[4][4] = {{1, 0, 0, 0},
                                    {0, 1, 0, 0},
                                    {0, 0, 1, 0},
                                    {0, 0, 0, 1}};

    double Tx = 0;
    double Ty = 0;
    double Tz = 0;
    int W;
    int H;
protected:

private:
    void carregaObj(std::string);
};

#endif // OBJETO3D_H
