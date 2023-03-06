#ifndef individual_hpp
#define individual_hpp

#include <climits>
#include <algorithm>
#include <iostream>
using namespace std;

#include "Tour.hpp"

// Yiping Liu, Liting Xu, Yuyan Han, Xiangxiang Zeng, Gary G. Yen, and Hisao Ishibuchi, Evolutionary Multimodal Multiobjective Optimization for Traveling Salesman Problems, IEEE Transactions on Evolutionary Computation, Early Access, 2023, DOI:10.1109/TEVC.2023.3239546
//
// Please contact {yiping0liu@gmail.com} or {litingxu@hnu.edu.cn} if you have any problem.
int DEPTH = 20;

class Individual {

	public:
		Tour cycle;
		int fitness[2] = {0, 0};
	public:
		Individual &operator=(const Individual &indiv) {
			cycle = indiv.cycle;
			fitness [0] = indiv.fitness[0];
			fitness [1] = indiv.fitness[1];
			return *this;
		}

		bool operator==(const Individual &indiv)const {
			return cycle == indiv.cycle;
		}

		bool operator!=(const Individual &indiv)const {
			return cycle != indiv.cycle;
		}

		bool operator<(const Individual &indiv)const {
			if (fitness[0] != indiv.fitness[0])
				return fitness[0] < indiv.fitness[0];
			else
				return fitness[1] < indiv.fitness[1];
		}
//
		bool operator>(const Individual &indiv)const {
			if (fitness[0] != indiv.fitness[0])
				return fitness[0] > indiv.fitness[0];
			else
				return fitness[1] > indiv.fitness[1];
		}

	public:
		void evaluate(const std::vector<std::vector<int> > &cost, const std::vector<std::vector<int> > &cost1, int &maxFEs) {
			fitness[0] = 0;
			fitness[1] = 0;
			for (int i = 0; i < cycle.size; i++) {
				fitness[0] += cost[cycle.tour[i].id ][cycle.tour[i].left ];
				fitness[0] += cost[cycle.tour[i].id ][cycle.tour[i].right];
				fitness[1] += cost1[cycle.tour[i].id ][cycle.tour[i].left ];
				fitness[1] += cost1[cycle.tour[i].id ][cycle.tour[i].right];
			}
			fitness[0] /= 2;
			fitness[1] /= 2;
			maxFEs++;
		}

		void init(const Tour &data) {

			cycle = data;
			std::shuffle(cycle.tour.begin(), cycle.tour.end(), mt);
			for (int i = 0; i < cycle.size; i++) {
				cycle.tour[i].left = cycle.tour[(i + cycle.size - 1) % cycle.size].id;
				cycle.tour[i].right = cycle.tour[(i + 1) % cycle.size].id;
			}

			std::sort(cycle.tour.begin(), cycle.tour.end());
		}

		void init(const Tour &data, int m) {

			cycle = data;
			for (int i = 0; i < cycle.size; i++) {
				cycle.tour[i].left = cycle.tour[(i + cycle.size - 1) % cycle.size].id;
				cycle.tour[i].right = cycle.tour[(i + 1) % cycle.size].id;
			}
			std::sort(cycle.tour.begin(), cycle.tour.end());
		}

		void divide_tour(std::vector<Tour> Eset) {

			if (Eset.size() == 0)
				return;

			std::uniform_int_distribution<> rand(1, Eset.size());
			std::shuffle(Eset.begin(), Eset.end(), mt);
			int num = rand(mt);
			Eset.resize(num);

			for (int i = 0; i < num; i++) {

				int prev, current, next;

				for (int j = 0; j < Eset[i].size; j += 2) {
					prev = Eset[i].tour[(j - 1 + Eset[i].size) % Eset[i].size].id;
					current = Eset[i].tour[j].id;
					next = Eset[i].tour[j + 1].id;

					if (cycle.tour[current - 1].left == next) {
						cycle.tour[current - 1].left = prev;
					} else {
						cycle.tour[current - 1].right = prev;
					}
				}

				for (int j = 1; j < Eset[i].size; j += 2) {
					prev = Eset[i].tour[j - 1].id;
					current = Eset[i].tour[j].id;
					next = Eset[i].tour[(j + 1) % Eset[i].size].id;

					if (cycle.tour[current - 1].left == prev) {
						cycle.tour[current - 1].left = next;
					} else {
						cycle.tour[current - 1].right = next;
					}
				}
			}
		}

		void switch_edge_evaluate(int t1, int t2, int t3, int t4, int &p1, int &p2, int &p3, int &p4, int &min_distance,
		                          const std::vector<std::vector<int> > &cost) {

			int e1, e2, e3, e4;

			e1 = cost[t1][t2], e2 = cost[t3][t4];
			e3 = cost[t1][t3], e4 = cost[t2][t4];

			if (min_distance > (- e1 - e2 + e3 + e4)) {
				p1 = t1;
				p2 = t2;
				p3 = t3;
				p4 = t4;
				min_distance = (- e1 - e2 + e3 + e4);
			} else if (min_distance == (- e1 - e2 + e3 + e4)) {
				std::uniform_int_distribution<> randi(0, 1);
				if (randi(mt) == 0 ) {
					p1 = t1;
					p2 = t2;
					p3 = t3;
					p4 = t4;
				}
			}

			e3 = cost[t1][t4];
			e4 = cost[t2][t3];

			if (min_distance > (- e1 - e2 + e3 + e4)) {
				p1 = t1;
				p2 = t2;
				p3 = t4;
				p4 = t3;
				min_distance = (- e1 - e2 + e3 + e4);
			} else if (min_distance == (- e1 - e2 + e3 + e4)) {
				std::uniform_int_distribution<> randi(0, 1);
				if (randi(mt) == 0 ) {
					p1 = t1;
					p2 = t2;
					p3 = t4;
					p4 = t3;
				}
			}
		}

		void conect_subtour(const std::vector<std::vector<int> > &cost,
		                    const std::vector<std::list<std::pair<int, int> > > &NNlist) {

			std::vector<int> numbering(cycle.size, 0);
			std::vector<std::pair<int, std::pair<int, int> > >
			mapping;

			int sum = 0, size = 0, num = 1;
			int prev = 0, current = 1, target = current;
			if (cycle.size <= 20)
				DEPTH = 5;
			while (true) {
				if (numbering[current - 1] == 0) {
					numbering[current - 1] = num;
					size++;
				} else {
					mapping.push_back(std::make_pair(size, std::make_pair(num, target)));
					num++;
					sum += size;
					if (sum == cycle.size)
						break;
					size = 0;
					prev = 0;
					for (int i = 0; i < cycle.size; i++) {
						if (numbering[i] == 0) {
							current = i + 1;
							target = current;
							break;
						}
					}
				}

				if (prev != cycle.tour[current - 1].left) {
					prev = current;
					current = cycle.tour[current - 1].left;
				} else {
					prev = current;
					current = cycle.tour[current - 1].right;
				}
			}

			// Finding the edge of the smallest island and the most evaluable edge
			while (mapping.size() != 1) {

				int t1, t2, t3, t4;
				int p1, p2, p3, p4;
				int min_distance;

				std::sort(mapping.begin(), mapping.end());

				min_distance = INT_MAX;
				prev = 0;
				current = mapping[0].second.second;

				for (int i = 0; i < mapping[0].first; i++) {
					int depth = 0;
					auto list_itr = NNlist[current].begin();
					while (depth < DEPTH) {

						// If the neighbor node is an island
						if (numbering[current - 1] == numbering[list_itr->second - 1]) {
							depth++;
							list_itr++;
							continue;
						}

						// min->left, near->left
						t1 = current, t2 = cycle.tour[t1 - 1].left;
						t3 = list_itr->second, t4 = cycle.tour[t3 - 1].left;
						switch_edge_evaluate(t1, t2, t3, t4, p1, p2, p3, p4, min_distance, cost);

						// min->left, near->right
						t1 = current, t2 = cycle.tour[t1 - 1].left;
						t3 = list_itr->second, t4 = cycle.tour[t3 - 1].right;
						switch_edge_evaluate(t1, t2, t3, t4, p1, p2, p3, p4, min_distance, cost);

						// min->right, near->left
						t1 = current, t2 = cycle.tour[t1 - 1].right;
						t3 = list_itr->second, t4 = cycle.tour[t3 - 1].left;
						switch_edge_evaluate(t1, t2, t3, t4, p1, p2, p3, p4, min_distance, cost);

						// min->right, near->right
						t1 = current, t2 = cycle.tour[t1 - 1].right;
						t3 = list_itr->second, t4 = cycle.tour[t3 - 1].right;
						switch_edge_evaluate(t1, t2, t3, t4, p1, p2, p3, p4, min_distance, cost);

						depth++;
						list_itr++;
					}

					if (prev != cycle.tour[current - 1].left) {
						prev = current;
						current = cycle.tour[current - 1].left;
					} else {
						prev = current;
						current = cycle.tour[current - 1].right;
					}
				}

				//Change
				if (cycle.tour[p1 - 1].left == p2) {
					cycle.tour[p1 - 1].left = p3;
				} else {
					cycle.tour[p1 - 1].right = p3;
				}

				if (cycle.tour[p2 - 1].left == p1) {
					cycle.tour[p2 - 1].left = p4;
				} else {
					cycle.tour[p2 - 1].right = p4;
				}

				if (cycle.tour[p3 - 1].left == p4) {
					cycle.tour[p3 - 1].left = p1;
				} else {
					cycle.tour[p3 - 1].right = p1;
				}

				if (cycle.tour[p4 - 1].left == p3) {
					cycle.tour[p4 - 1].left = p2;
				} else {
					cycle.tour[p4 - 1].right = p2;
				}

				for (int i = 1; i < mapping.size(); i++) {
					if (numbering[p3 - 1] == mapping[i].second.first) {
						mapping[i].first += mapping[0].first;
					}
				}

				int island_num = numbering[p1 - 1];
				for (int i = 0; i < cycle.size; i++) {
					if (numbering[i] == island_num) {
						numbering[i] = numbering[p3 - 1];
					}
				}
				mapping.erase(mapping.begin());
			}
		}
		void switch_edge_evaluate(int t1, int t2, int t3, int t4, std::vector<int> &p1, std::vector<int> &p2,
		                          std::vector<int> &p3, std::vector<int> &p4,
		                          std::vector < std::vector<int>> &min_distance,
		                          const std::vector<std::vector<int> > &cost,
		                          const std::vector<std::vector<int> > &cost1, std::vector<std::vector<int>> &edgesum, std::vector<int> &min_edge) {

			int e1, e2, e3, e4;

			e1 = cost[t1][t2], e2 = cost[t3][t4];
			e3 = cost[t1][t3], e4 = cost[t2][t4];
			double sum1 = e3 + e4 - e1 - e2;

			e1 = cost1[t1][t2], e2 = cost1[t3][t4];
			e3 = cost1[t1][t3], e4 = cost1[t2][t4];
			double sum2 = e3 + e4 - e1 - e2;
			std::vector<int>sum;
			sum.push_back(sum1);
			sum.push_back(sum2);
			min_edge.push_back(edgesum[t1][t3] + edgesum[t2][t4] - edgesum[t1][t2] - edgesum[t3][t4]);
			min_distance.push_back(sum);
			sum.clear();
			p1.push_back(t1);
			p2.push_back(t2);
			p3.push_back(t3);
			p4.push_back(t4);

			e1 = cost[t1][t2], e2 = cost[t3][t4];
			e3 = cost[t1][t4];
			e4 = cost[t2][t3];
			sum1 = e3 + e4 - e1 - e2;

			e1 = cost1[t1][t2], e2 = cost1[t3][t4];
			e3 = cost1[t1][t4];
			e4 = cost1[t2][t3];
			sum2 = e3 + e4 - e1 - e2;
			sum.push_back(sum1);
			sum.push_back(sum2);
			min_edge.push_back(edgesum[t1][t4] + edgesum[t2][t3] - edgesum[t1][t2] - edgesum[t3][t4]);
			min_distance.push_back(sum);
			sum.clear();
			p1.push_back(t1);
			p2.push_back(t2);
			p3.push_back(t4);
			p4.push_back(t3);
		}

		void Ndsort(std::vector<vector<int>> &pop, int nSort, int &MaxFNo, std::vector<int> &FrontNo1) {
			int M = 2;
			vector<int>FrontNo(FrontNo1.size());
			for (int i = 0; i < FrontNo1.size(); i++) {
				FrontNo1[i] = INT_MAX;
				FrontNo[i] = INT_MAX;
			}
			std::vector<vector<int>> pop1;
			for (int i = 0; i < pop.size(); i++) {
				pop1.push_back(pop[i]);
				pop1[i].push_back(i);
			}
			sort(pop1.begin(), pop1.end());
			for (int i = 0; i < pop1.size(); i++) {
				pop[i][0] = pop1[i][0];
				pop[i][1] = pop1[i][1];
			}

			MaxFNo = 0;
			int sumcount = 0;
			int n = FrontNo.size();
			nSort = min(nSort, n);
//	while(maxfronto<MaxFNo){
			while (sumcount < nSort) {
				MaxFNo = MaxFNo + 1;
				for (int i = 0; i < FrontNo.size(); i++) {
					if (FrontNo[i] == INT_MAX) {
						bool Dominated = false;
						for (int j = i - 1; j >= 0; j--) {
							if (FrontNo[j] == MaxFNo) {
								int m = 1;
								int s = 1;
								while (m <= M && pop[i][m - 1] >= pop[j][m - 1]) {
									if (pop[i][m - 1] == pop[j][m - 1])
										s++;
									m = m + 1;
								}
								Dominated = m > M;
								if (s == M + 1)
									Dominated = false;
								if ( Dominated || M == 2)
									break;
							}
						}
						if (!Dominated)
							FrontNo[i] = MaxFNo;
					}
				}
				sumcount = 0;
				for (int i = 0; i < FrontNo.size(); i++) {
					if (FrontNo[i] < INT_MAX)
						sumcount++;
				}
			}
			for (int i = 0; i < pop1.size(); i++) {
				FrontNo1[pop1[i][2]] = FrontNo[i];
			}
		}

		void conect_subtour(const std::vector<std::vector<int> > &cost, const std::vector<std::vector<int> > &cost1,
		                    const std::vector<std::list<S> > &NNlist, std::vector<std::vector<int>> &edgesum) {

			std::vector<int> numbering(cycle.size, 0);
			std::vector<std::pair<int, std::pair<int, int> > >
			mapping;

			int sum = 0, size = 0, num = 1;
			int prev = 0, current = 1, target = current;
			if (cycle.size <= 20)
				DEPTH = 5;
			while (true) {
				if (numbering[current - 1] == 0) {
					numbering[current - 1] = num;
					size++;
				} else {
					mapping.push_back(std::make_pair(size, std::make_pair(num, target)));
					num++;
					sum += size;
					if (sum == cycle.size)
						break;
					size = 0;
					prev = 0;
					for (int i = 0; i < cycle.size; i++) {
						if (numbering[i] == 0) {
							current = i + 1;
							target = current;
							break;
						}
					}
				}

				if (prev != cycle.tour[current - 1].left) {
					prev = current;
					current = cycle.tour[current - 1].left;
				} else {
					prev = current;
					current = cycle.tour[current - 1].right;
				}
			}

			while (mapping.size() != 1) {

				int t1, t2, t3, t4;
				vector<int> p1, p2, p3, p4;
				vector<vector<int>> min_distance;
				vector<int> min_edge;

				std::sort(mapping.begin(), mapping.end());

				prev = 0;
				current = mapping[0].second.second;

				for (int i = 0; i < mapping[0].first; i++) {
					int depth = 0;
					auto list_itr = NNlist[current].begin();
					while (depth < DEPTH) {

						if (numbering[current - 1] == numbering[list_itr->id - 1]) {
							depth++;
							list_itr++;
							continue;
						}

						// min->left, near->left
						t1 = current, t2 = cycle.tour[t1 - 1].left;
						t3 = list_itr->id, t4 = cycle.tour[t3 - 1].left;
						switch_edge_evaluate(t1, t2, t3, t4, p1, p2, p3, p4, min_distance, cost, cost1, edgesum, min_edge);

						// min->left, near->right
						t1 = current, t2 = cycle.tour[t1 - 1].left;
						t3 = list_itr->id, t4 = cycle.tour[t3 - 1].right;
						switch_edge_evaluate(t1, t2, t3, t4, p1, p2, p3, p4, min_distance, cost, cost1, edgesum, min_edge);

						// min->right, near->left
						t1 = current, t2 = cycle.tour[t1 - 1].right;
						t3 = list_itr->id, t4 = cycle.tour[t3 - 1].left;
						switch_edge_evaluate(t1, t2, t3, t4, p1, p2, p3, p4, min_distance, cost, cost1, edgesum, min_edge);

						// min->right, near->right
						t1 = current, t2 = cycle.tour[t1 - 1].right;
						t3 = list_itr->id, t4 = cycle.tour[t3 - 1].right;
						switch_edge_evaluate(t1, t2, t3, t4, p1, p2, p3, p4, min_distance, cost, cost1, edgesum, min_edge);

						depth++;
						list_itr++;
					}

					if (prev != cycle.tour[current - 1].left) {
						prev = current;
						current = cycle.tour[current - 1].left;
					} else {
						prev = current;
						current = cycle.tour[current - 1].right;
					}
				}
				std::vector<int> FrontNo(min_distance.size());
				int MaxFNo;
				Ndsort(min_distance, min_distance.size(), MaxFNo, FrontNo);
				std::vector<int>Choose;
				std::vector<int>Choose1;
				int  minedge = INT_MAX;
				for (int i = 0; i < min_distance.size(); i++) {
					if (FrontNo[i] == 1 && minedge > min_edge[i]) {
						minedge = min_edge[i];
					}
				}
				for (int i = 0; i < min_distance.size(); i++) {
					if (FrontNo[i] == 1 && min_edge[i] == minedge) {
						Choose1.push_back(i);
					}
				}
				std::uniform_int_distribution<>randi(0, Choose1.size() - 1);
				int t, choose;
				t = randi(mt);
				choose = Choose1[t];
				int p11 = p1[choose];
				int p21 = p2[choose];
				int p31 = p3[choose];
				int p41 = p4[choose];
				p1.clear();
				p2.clear();
				p3.clear();
				p4.clear();

				if (cycle.tour[p11 - 1].left == p21) {
					cycle.tour[p11 - 1]
					.left = p31;
				} else {
					cycle.tour[p11 - 1].right = p31;
				}

				if (cycle.tour[p21 - 1].left == p11) {
					cycle.tour[p21 - 1]
					.left = p41;
				} else {
					cycle.tour[p21 - 1].right = p41;
				}

				if (cycle.tour[p31 - 1].left == p41) {
					cycle.tour[p31 - 1]
					.left = p11;
				} else {
					cycle.tour[p31 - 1].right = p11;
				}

				if (cycle.tour[p41 - 1].left == p31) {
					cycle.tour[p41 - 1]
					.left = p21;
				} else {
					cycle.tour[p41 - 1].right = p21;
				}

				for (int i = 1; i < mapping.size(); i++) {
					if (numbering[p31 - 1] == mapping[i].second.first) {
						mapping[i].first += mapping[0].first;
					}
				}

				int island_num = numbering[p11 - 1];
				for (int i = 0; i < cycle.size; i++) {
					if (numbering[i] == island_num) {
						numbering[i] = numbering[p31 - 1];
					}
				}
				mapping.erase(mapping.begin());
			}
		}



		void output() {
			for (int i = 0; i < cycle.size; i++)
				std::cout << cycle.tour[i].left << " ";
			std::cout << std::endl;
			for (int i = 0; i < cycle.size; i++)
				std::cout << cycle.tour[i].id << " ";
			std::cout << std::endl;
			for (int i = 0; i < cycle.size; i++)
				std::cout << cycle.tour[i].right << " ";
			std::cout << std::endl;
		}
};

#endif
