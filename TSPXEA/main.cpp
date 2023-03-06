#include "eax.hpp"
#include "EnvironmentalSelection.hpp"
#include "unistd.h"
#include <bits/stdc++.h>
using namespace std;



// Yiping Liu, Liting Xu, Yuyan Han, Xiangxiang Zeng, Gary G. Yen, and Hisao Ishibuchi, Evolutionary Multimodal Multiobjective Optimization for Traveling Salesman Problems, IEEE Transactions on Evolutionary Computation, Early Access, 2023, DOI:10.1109/TEVC.2023.3239546
//
// Please contact {yiping0liu@gmail.com} or {litingxu@hnu.edu.cn} if you have any problem.

int main() {
	int size;
	int a[] = {
		11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 115, 115, 115, 115, 115, 115, 115, 115, 115, 115
	};
//		'3ndr140', '3ndr196','3ndr128', '3ndr240', '3ndr244','3ndr109', '3ndr232', '3ndr219', '3ndr236', '3ndr225'(D1-D10)
	string strs[] = {"3ndr140", "3ndr196", "3ndr128", "3ndr240", "3ndr244", "3ndr109", "3ndr232", "3ndr219", "3ndr236", "3ndr225",
	                 "bays29+r3ndr140", "bays29+r3ndr196", "bays29+r3ndr128", "bays29+r3ndr240", "bays29+r3ndr244", "bays29+r3ndr109",
	                 "bays29+r3ndr232", "bays29+r3ndr219", "bays29+r3ndr236", "bays29+r3ndr225",
	                 "att48+r3ndr140", "att48+r3ndr196", "att48+r3ndr128", "att48+r3ndr240", "att48+r3ndr244", "att48+r3ndr109",
	                 "att48+r3ndr232", "att48+r3ndr219", "att48+r3ndr236", "att48+r3ndr225",
	                 "lin105+r3ndr140", "lin105+r3ndr196", "lin105+r3ndr128", "lin105+r3ndr240", "lin105+r3ndr244", "lin105+r3ndr109",
	                 "lin105+r3ndr232", "lin105+r3ndr219", "lin105+r3ndr236", "lin105+r3ndr225",
	                };
	for (int ks = 0; ks < 40; ks ++) {
		size = a[ks];
		string argv = strs[ks];
		string filename;
		filename = "./problem/" + argv + "tsp1.txt";
		std::vector<std::vector<int> > cost(size + 1, std::vector<int>(size + 1, 0));//f1
		std::vector<std::vector<int> > cost1(size + 1, std::vector<int>(size + 1, 0));//f2
		std::vector<std::vector<int> > costm(size + 1, std::vector<int>(size + 1, 0));//f1+f2
		ifstream infile;
		infile.open(filename);
		// input cost cost1 costm
		for (int i = 1; i <= size; i++) {
			for (int j = 1; j <= size; j++) {
				infile >> cost[i][j];
			}
			cout << i << endl;
		}
		for (int i = 1; i <= size; i++) {
			for (int j = 1; j <= size; j++) {
				infile >> cost1[i][j];
				costm[i][j] = cost1[i][j] + cost[i][j];
			}
			cout << i << endl;
		}
		infile.close();
		Tour data;
		data.size = size;
		data.setvalue();
		std::vector < std::list<S> > NNlist3(data.size + 1);
		data.calc_citycost_nnlist(cost, cost1, NNlist3);
		std::vector<std::list<std::pair<int, int> > > NNlist(data.size + 1);
		std::vector<std::list<std::pair<int, int> > > NNlist1(data.size + 1);
		std::vector<std::list<std::pair<int, int> > > NNlistm(data.size + 1);
		data.calc_citycost_nnlist(cost, NNlist);
		data.calc_citycost_nnlist(cost1, NNlist1);
		data.calc_citycost_nnlist(costm, NNlistm);

		double opt = 0, opt_ave = 0, conv_gene = 0;
		int TRIAL = 30;


		for (int i = 0; i < TRIAL; i++) {

			std::cout << "trial..." << i + 1 << std::endl;
			int MAXFES ;
			MAXFES = 50000;
			int maxFEs = 0;

			std::vector<Individual> population(POPULATION);
			std::vector<int>FrontNo(POPULATION);
			std::vector<double>fDS(POPULATION);

			for (int j = 0; j < POPULATION; j++) {
				population[j].init(data);
				population[j].evaluate(cost, cost1, maxFEs);
			}
			int maxFrontNo = 0;
			NDsort(population, POPULATION, maxFrontNo, FrontNo);
			for (int kk = 0; kk < POPULATION; kk++) {
				fDS[kk] = 0;
			}
			double min, max, ave;
			std::cout << "gene" << '\t' << "min" << '\t' << "max" << '\t' << "ave" << std::endl;
			stringstream ss;
			ss << i ;
			string Srcpath;

			Srcpath = ".\\result\\probleme\\" + argv + ss.str();
			if ((_access(Srcpath.c_str(), 0)) != 0) {
				string dd = "md " + Srcpath;
				system(dd.c_str());
			}
			bool ff = true;
			int ts = 0;
			ofstream myout(Srcpath + "\\" + argv + "out.txt");
			ofstream myout1(Srcpath + "\\" + argv + "allout.txt");
			ofstream timeout(Srcpath + "\\" + argv + "timeout.txt");
			clock_t start = clock();
			clock_t end = clock();
			double time1 = 0;
			while (maxFEs < MAXFES) {
				start = clock();
				std::vector<Individual> next_population;
				for (int k = 0; k < POPULATION; k++) {
					next_population.push_back(population[k]);
				}
				double Eta = 0;
				std::vector<std::vector<int>> parentALL(population.size(), vector<int>(population[0].cycle.size));
				std::vector<std::vector<int>>edgesum(population[0].cycle.size + 1, vector<int>(population[0].cycle.size + 1));
				path(parentALL, population);
				compute_edge(edgesum, parentALL, population.size());
				for (int k = 0; k < POPULATION; k ++) {
					Individual parentA = population[k];
					Individual parentB = population[(k + 1) % POPULATION];
					if (maxFEs < Eta * MAXFES) {
						EAX(parentA, parentB, costm, NNlistm, next_population);//eax-W
					} else {
						EAX(parentA, parentB, cost, cost1, NNlist3, next_population, edgesum); //eax-ND
					}
				}
				for (int i = 0; i < next_population.size(); i++) {
					if (next_population[i].fitness[0] == 0) {
						next_population[i].evaluate(cost, cost1, maxFEs);
					}
				}
				cout << "next_population.size " << next_population.size() << endl;
				cout << "population.size" << population.size() << endl;
				EnvironmentalSelection(next_population, population, FrontNo, fDS, POPULATION);

				cout << "population.size:" << population.size() << endl;
				for (int i = 0; i < next_population.size(); i++) {
					next_population.clear();
				}
				cout << "population.fitness" << maxFEs << ' ' << population[0].fitness[0] << ' ' << population[0].fitness[1] << endl;
				std::shuffle(population.begin(), population.end(), mt);
				end = clock();
				time1 += end - start;
				for (int t = 0; t < population.size(); t++) {
					int prev = 0, current = 1;
					for (int i = 0; i < population[t].cycle.size; i++) {
						if (prev != population[t].cycle.tour[current - 1].left) {
							prev = current;
							current = population[t].cycle.tour[current - 1].left;
						} else {
							prev = current;
							current = population[t].cycle.tour[current - 1].right;
						}
						myout1 << prev << ' ';
					}
					myout1 << population[t].fitness[0] << ' ' << population[t].fitness[1] << ' ';
					myout1 << maxFEs << endl;
				}
			}
			timeout << time1 / 1000 << endl;
			timeout.close();
			myout1.close();
			for (int t = 0; t < population.size(); t++) {
				int prev = 0, current = 1;
				for (int i = 0; i < population[t].cycle.size; i++) {
					if (prev != population[t].cycle.tour[current - 1].left) {
						prev = current;
						current = population[t].cycle.tour[current - 1].left;
					} else {
						prev = current;
						current = population[t].cycle.tour[current - 1].right;
					}
					myout << prev << ' ';
				}
				myout << population[t].fitness[0] << ' ' << population[t].fitness[1] << ' ';
				myout << maxFEs << endl;
			}
			myout.close();
		}
		printf("over\n");
	}
	return 0;
}
