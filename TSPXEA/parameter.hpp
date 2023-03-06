#ifndef parameter_hpp
#define parameter_hpp

#include <string>
#include <random>

// Yiping Liu, Liting Xu, Yuyan Han, Xiangxiang Zeng, Gary G. Yen, and Hisao Ishibuchi, Evolutionary Multimodal Multiobjective Optimization for Traveling Salesman Problems, IEEE Transactions on Evolutionary Computation, Early Access, 2023, DOI:10.1109/TEVC.2023.3239546
//
// Please contact {yiping0liu@gmail.com} or {litingxu@hnu.edu.cn} if you have any problem.
static const int TRIAL = 30;
//static const int GENERATION = 300;
//static const
static int POPULATION = 100;
//static const int CHILDREN = 2;
static const int OPT_DIST = 6773;
//static const int DEPTH = 20;
static std::string NAME = "";
static std::string COMMENT = "";
static std::string TYPE = "";
static std::string DIMENSION = "";
static std::string EDGE_WEIGHT_TYPE = "";
std::mt19937 mt(0);


#endif
