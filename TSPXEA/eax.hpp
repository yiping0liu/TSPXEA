#ifndef eax_hpp
#define eax_hpp

#include "Individual.hpp"

// Yiping Liu, Liting Xu, Yuyan Han, Xiangxiang Zeng, Gary G. Yen, and Hisao Ishibuchi, Evolutionary Multimodal Multiobjective Optimization for Traveling Salesman Problems, IEEE Transactions on Evolutionary Computation, Early Access, 2023, DOI:10.1109/TEVC.2023.3239546
//
// Please contact {yiping0liu@gmail.com} or {litingxu@hnu.edu.cn} if you have any problem.

void judge_ABcycle(Tour &trace, std::vector<Tour> &Eset, int group) {

	int end = trace.size - 1;
	Tour ABcycle;

	for (int i = 0; i < end; i++) {
		if (trace.tour[i].id == trace.tour[end].id && (end - i) % 2 == 0) {

			ABcycle.tour.insert(ABcycle.tour.begin(), trace.tour.begin() + i + 1, trace.tour.end());

			if ((end - i) != 2) {
				if (group == 0) {
					ABcycle.tour.push_back(ABcycle.tour[0]);
					ABcycle.tour.erase(ABcycle.tour.begin());
				}
				ABcycle.size = end - i;
				Eset.push_back(ABcycle);
			}

			trace.tour.erase(trace.tour.begin() + i + 1, trace.tour.end());
			trace.size = i + 1;
			break;
		}
	}
}

void select_edge(Tour &trace, Tour &stock, int &current, int &next) {

	std::uniform_int_distribution<> bit(0, 1);
	int side = bit(mt);

	if (side == 0) {
		if (stock.tour[current - 1].left != 0) {
			next = stock.tour[current - 1].left;
		} else {
			next = stock.tour[current - 1].right;
		}
	} else {
		if (stock.tour[current - 1].right != 0) {
			next = stock.tour[current - 1].right;
		} else {
			next = stock.tour[current - 1].left;
		}
	}
	if (next == 0)
		std::cout << next - 1 << ' ' << stock.tour[current - 1].right << ' ' << stock.tour[current - 1].left << endl;
	trace.tour.push_back(stock.tour[next - 1]);

	if (stock.tour[current - 1].left == next) {
		stock.tour[current - 1].left = 0;
	} else {
		stock.tour[current - 1].right = 0;
	}
	if (stock.tour[next - 1].left == current) {
		stock.tour[next - 1].left = 0;
	} else {
		stock.tour[next - 1].right = 0;
	}

	if (stock.tour[current - 1].left == stock.tour[current - 1].right)
		stock.tour[current - 1].id = 0;
	if (stock.tour[next - 1].left == stock.tour[next - 1].right)
		stock.tour[next - 1].id = 0;
}

void ABcycle(const Individual &parentA, const Individual &parentB, std::vector<Tour> &Eset) {

	Tour stockA, stockB, trace;
	int group, current, next;   // prev, current, next

	if (parentA == parentB)
		return;

	stockA = parentA.cycle;
	stockB = parentB.cycle;

	while (!stockA.fill_id()) {
		std::uniform_int_distribution<> rand(1, stockA.size - 1);
		do {
			current = rand(mt);
		} while (stockA.tour[current - 1].id == 0);

		trace.tour.clear();
		trace.size = 0;
		trace.tour.push_back(stockA.tour[current - 1]);
		trace.size++;
		group = 0;

		do {

			if (group == 0) {
				select_edge(trace, stockA, current, next);
				group = 1;
			} else {
				select_edge(trace, stockB, current, next);
				group = 0;
			}

			trace.size++;

			judge_ABcycle(trace, Eset, group);

			current = next;
		} while (trace.size != 1);
	}
}

void NDsort(std::vector<Individual> &pop, int nSort, int &MaxFNo, std::vector<int> &FrontNo) {
	int M = 2;
	for (int i = 0; i < FrontNo.size(); i++) {
		FrontNo[i] = INT_MAX;
	}
	sort(pop.begin(), pop.end());

	MaxFNo = 0;
	int sumcount = 0;
	int n = FrontNo.size();
	nSort = min(nSort, n);
	while (sumcount < nSort) {
		MaxFNo = MaxFNo + 1;
		for (int i = 0; i < FrontNo.size(); i++) {
			if (FrontNo[i] == INT_MAX) {
				bool Dominated = false;
				for (int j = i - 1; j >= 0; j--) {
					if (FrontNo[j] == MaxFNo) {
						int m = 1;
						int s = 1;
						while (m <= M && pop[i].fitness[m - 1] >= pop[j].fitness[m - 1]) {
							if (pop[i].fitness[m - 1] == pop[j].fitness[m - 1])
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
}

bool isequal(Individual a, Individual b) {
	vector<int>a1;
	vector<int>b1;
	int prev = 0, current = 1;
	for (int i = 0; i < a.cycle.size; i++) {
		if (prev != a.cycle.tour[current - 1].left) {
			prev = current;
			current = a.cycle.tour[current - 1].left;
		} else {
			prev = current;
			current = a.cycle.tour[current - 1].right;
		}
		a1.push_back(prev);
	}
	prev = 0, current = 1;
	for (int i = 0; i < b.cycle.size; i++) {
		if (prev != b.cycle.tour[current - 1].left) {
			prev = current;
			current = b.cycle.tour[current - 1].left;
		} else {
			prev = current;
			current = b.cycle.tour[current - 1].right;
		}
		b1.push_back(prev);
	}
	for (int i = 1; i < a1.size(); i++) {
		if (a1[i] != b1[i] && a1[i] != b1[a1.size() - i])
			return false;
	}

	return true;
}
//eax-W
void EAX(const Individual &parentA, const Individual &parentB, const std::vector<std::vector<int> > &costm,

         const std::vector<std::list<std::pair<int, int>>> &NNlistm, std::vector<Individual> &people) {

	std::vector<Tour> Eset;
	int CHILDREN = 2;
	std::vector<Individual> children( CHILDREN, parentA);
	ABcycle(parentA, parentB, Eset);
	for (int i = 0; i < CHILDREN; i++) {
		children[i].divide_tour(Eset);
		children[i].conect_subtour(costm, NNlistm);
		children[i].fitness[0] = 0;
	}
	int size = children.size();
	for (int i = 0; i < size; i++) {
		bool f = true;
		int sum = 0;
		for (int j = 0; j < people.size(); j++) {
			if (isequal(children[i], people[j])) {
				f = false;
				break;
			}
		}
		if (f == true) {
			people.push_back(children[i]);
		}
	}
}

void compute_edge(std::vector<std::vector<int>> &edgesum, std::vector<std::vector<int>>parentall, int parentallsize) {
	for (int i = 0; i < parentallsize; i++) {
		for (int j = 0; j < parentall[i].size() - 1; j++) {
			edgesum[parentall[i][j]][parentall[i][j + 1]]++;
			edgesum[parentall[i][j + 1]][parentall[i][j]]++;
		}
	}
}

void path(std::vector<std::vector<int>> &parentA, std::vector<Individual> &lastin_population) {
	for (int i = 0; i < lastin_population.size(); i++) {
		int current = 1;
		int prev = 0;
		for (int j = 0; j < lastin_population[i].cycle.size; j++) {
			if (prev != lastin_population[i].cycle.tour[current - 1].left) {
				prev = current;
				current = lastin_population[i].cycle.tour[current - 1].left;
			} else {
				prev = current;
				current = lastin_population[i].cycle.tour[current - 1].right;
			}
			parentA[i][j] = prev;
		}
	}

}
// eax-ND
void EAX(const Individual &parentA, const Individual &parentB, const std::vector<std::vector<int> > &cost,
         const std::vector<std::vector<int> > &cost1, const std::vector<std::list<S > > &NNlist,

         std::vector<Individual> &people, std::vector<std::vector<int>>edgesum) {

	std::vector<Tour> Eset;
	int CHILDREN = 2;
	std::vector<Individual> children(CHILDREN, parentA);
	ABcycle(parentA, parentB, Eset);
	for (int i = 0; i < CHILDREN; i++) {
		children[i].divide_tour(Eset);
		children[i].conect_subtour(cost, cost1, NNlist, edgesum);
		children[i].fitness[0] = 0;
	}
	int size = children.size();
	for (int i = 0; i < size; i++) {
		bool f = true;
		int sum = 0;
		for (int j = 0; j < people.size(); j++) {
			if (isequal(children[i], people[j])) {
				f = false;
				break;
			}
		}
		if (f == true)
			people.push_back(children[i]);
	}
}


#endif
