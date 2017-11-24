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
    int P1;
    int P2;
} Linha;

class Objeto3D
{
public:
    void multiplica(double A[4][4], double B[4][4], double out[4][4]);
    Objeto3D(std::string);
    int m;
    int n;
    std::vector<Ponto> pontos;
    std::vector<Linha> linhas;
    std::vector<std::vector<int>> faces;
    void setTranslacaoEmX(double);
    void setTranslacaoEmY(double);
    void setTranslacaoEmZ(double);
    void setRotacaoEmX(double);
    void setRotacaoEmY(double);
    void setRotacaoEmZ(double);
    void setEscalaEmX(double);
    void setEscalaEmY(double);
    void setEscalaEmZ(double);
    Ponto getPonto(int, int);
    virtual ~Objeto3D();
    double M[4][4] = {1, 0, 0, 0,
                           0, 1, 0, 0,
                           0, 0, 1, 0,
                           0, 0, 0, 1};

    double matrizT[4][4] = {1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1};
protected:

private:
    void carregaObj(std::string);
};

#endif // OBJETO3D_H
