#ifndef tour_hpp
#define tour_hpp

#include <bits/stdc++.h>

#include "parameter.hpp"
#include "node.hpp"




// Yiping Liu, Liting Xu, Yuyan Han, Xiangxiang Zeng, Gary G. Yen, and Hisao Ishibuchi, Evolutionary Multimodal Multiobjective Optimization for Traveling Salesman Problems, IEEE Transactions on Evolutionary Computation, Early Access, 2023, DOI:10.1109/TEVC.2023.3239546
//
// Please contact {yiping0liu@gmail.com} or {litingxu@hnu.edu.cn} if you have any problem.
struct S {
	int rank;
	double x;  //f1
	double y;  //f2
	int id;
};

bool cmp1(S a, S b) {
	if (a.x != b.x)
		return a.x < b.x;
	else
		return a.y < b.y;
}

bool cmp2(S a, S b) {
	if (a.rank != b.rank)
		return a.rank < b.rank;
	else if (a.x != b.x)
		return a.x < b.x;
	else
		return a.y < b.y;
}

void Ndsort(std::list<S> &pop) {
	int nSort = pop.size();
	int MaxFNo = 0;
	std::vector<std::vector<int>> mpop(nSort, std::vector<int>(2));
	int M = 2;
	std::vector<int> FrontNo(nSort);
	auto itr = pop.begin();
	for (int i = 0; i < nSort; i++) {
		mpop[i][0] = itr->x;
		mpop[i][1] = itr->y;
		FrontNo[i] = INT_MAX;
		itr++;
	}
	sort(mpop.begin(), mpop.end());
	pop.sort(cmp1);
	MaxFNo = 0;
	int sumcount = 0;
	while (sumcount < nSort) {
		MaxFNo = MaxFNo + 1;
		for (int i = 0; i < FrontNo.size(); i++) {
			if (FrontNo[i] == INT_MAX) {
				bool Dominated = false;
				for (int j = i - 1; j >= 0; j--) {
					if (FrontNo[j] == MaxFNo) {
						int m = 1;
						int s = 1;
						while (m <= M && mpop[i][m - 1] >= mpop[j][m - 1]) {
							if (mpop[i][m - 1] == mpop[j][m - 1])
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
	itr = pop.begin();
	for (int i = 0; i < nSort; i++) {
		itr->rank = FrontNo[i];
		itr++;
	}
}


class Tour {

	public:
		int size;
		std::vector<Node> tour;

	public:
		void setvalue() {
			for (int i = 0; i < size; i++) {
				tour.push_back(Node(i + 1));
			}
		}
		void setvalue(std::vector<int> parentA) {
			for (int i = 0; i < size; i++) {
				tour.push_back(Node(parentA[i]));
			}
		}
		Tour &operator=(const Tour &t) {
			size = t.size;
			tour = t.tour;
			return *this;
		}

		bool operator==(const Tour &t)const {
			return equal(tour.begin(), tour.end(), t.tour.begin());
		}

		bool operator!=(const Tour &t)const {
			return !equal(tour.begin(), tour.end(), t.tour.begin());
		}

	public:
		void calc_citycost_nnlist(std::vector<std::vector<int> > &cost,
		                          std::vector<std::vector<int> > &cost1, std::vector < std::list<S> > &NNlist) {

			int id, dist, dist1;
			for (int i = 1; i <= size; i++) {
				for (int j = i + 1; j <= size; j++) {
					dist = cost[i][j];
					dist1 = cost1[i][j];
					id = j;
					S temp;
					temp.x = dist;
					temp.y = dist1;
					temp.id = id;
					NNlist[i].push_back(temp);

					temp.x = dist;
					temp.y = dist1;
					temp.id = i;
					NNlist[j].push_back(temp);
				}
				Ndsort(NNlist[i]);
				NNlist[i].sort(cmp2);
			}
		}


		void calc_citycost_nnlist(std::vector<std::vector<int> > &cost, std::vector<std::list<std::pair<int, int> > > &NNlist) {

			int id, dist;
			for (int i = 1; i <= size; i++) {
				for (int j = i + 1; j <= size; j++) {
					dist = cost[i][j];
					id = j;
					NNlist[i].push_back(std::make_pair(dist, id));
					NNlist[j].push_back(std::make_pair(dist, i));
				}
				NNlist[i].sort();
			}
		}

		bool fill_id() {
			for (int i = 0; i < size; i++) {
				if (tour[i] != tour[(i + 1) % size])
					return false;
			}
			return true;
		}
};

#endif
