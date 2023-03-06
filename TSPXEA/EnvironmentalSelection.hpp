#include "Individual.hpp"
#include "eax.hpp"


// Yiping Liu, Liting Xu, Yuyan Han, Xiangxiang Zeng, Gary G. Yen, and Hisao Ishibuchi, Evolutionary Multimodal Multiobjective Optimization for Traveling Salesman Problems, IEEE Transactions on Evolutionary Computation, Early Access, 2023, DOI:10.1109/TEVC.2023.3239546
//
// Please contact {yiping0liu@gmail.com} or {litingxu@hnu.edu.cn} if you have any problem.
void pdist2(std::vector<Individual> &lastin_population, std::vector<std::vector<double>> &d_obj,

            std::vector<std::vector<int>> &p_choose) {
	double max_obj = 0;
	double min_obj = INT_MAX;
	for (int i = 0; i < lastin_population.size(); i++) {
		std::vector<double>temp;
		vector<vector<double>> divs(lastin_population.size(), vector<double>(2));
		for (int j = 0; j < lastin_population.size(); j++) {
			double x = abs(lastin_population[i].fitness[0] - lastin_population[j].fitness[0]);
			double y = abs(lastin_population[i].fitness[1] - lastin_population[j].fitness[1]);
			temp.push_back(std::sqrt((x * x) + (y * y)));
			if (max_obj < temp[j])
				max_obj = temp[j];
			if (min_obj > temp[j])
				min_obj = temp[j];
			divs[j][0] = temp[j];
			divs[j][1] = j;
		}
		d_obj.push_back(temp);
		sort(divs.begin(), divs.end());
		vector<int> index;
		for (int j = 0; j < lastin_population.size(); j++) {
			index.push_back(divs[j][1]);
		}
		p_choose.push_back(index);
	}
}

void dec_diversity(std::vector<Individual> &lastin_population, std::vector<std::vector<double>> &d_dec) {
	std::vector<std::vector<int>> parentA(lastin_population.size(), std::vector<int>(lastin_population[0].cycle.size));
	path(parentA, lastin_population);
	std::vector<std::vector<double>> w(lastin_population.size(), std::vector<double>(lastin_population.size()));
	std::vector<std::vector<double>>b(lastin_population.size(), std::vector<double>(lastin_population.size()));
	int size = 0;
	int size1 = lastin_population[0].cycle.size;
	double max_b = 0, max_w = 0;
	double min_b = INT_MAX, min_w = INT_MAX;
	for (int i = 0; i < lastin_population.size(); i++) {
		for (int k = i; k < lastin_population.size(); k++) {
			size = 0;
			for (int j = 0; j < size1; j++) {
				for (int kk = 0; kk < size1; kk++) {
					if (parentA[i][j] == parentA[k][kk]) {
						if (parentA[i][(j + 1) % size1] == parentA[k][(kk + 1) % size1]) {
							b[i][k]++;
						} else if (parentA[i][(j + 1) % size1] == parentA[k][(kk - 1 + size1) % size1]) {
							b[i][k]++;
						}
					}
				}
			}
			b[k][i] = b[i][k];
			if (max_b < b[k][i])
				max_b = b[k][i];
			if (min_b > b[k][i])
				min_b = b[k][i];
		}
	}
	for (int i = 0; i < lastin_population.size(); i++) {
		std::vector<double>temp;
		for (int k = 0; k < lastin_population.size(); k++) {
			temp.push_back(max_b - b[i][k]);
		}
		d_dec.push_back(temp);
	}
}

double conti_divers2(vector<vector<int>> &pop, const int index) {
	int size = pop.size();
	int size1 = pop[0].size();
	vector<vector<int>>sum(size1 + 1, vector<int>(size1 + 1));
	double s = 0;
	int i = 0;
	while (i < size) {
		if (i == index) {
			i++;
			continue;
		}
		for (int j = 0; j < size1; j++) {
			sum[pop[i][j]][pop[i][(j + 1) % size1]]++;
		}
		i++;
	}
	int si = 0;
	if (index >= 0 && index < size) {
		si = size - 1;
	} else
		si = size;
	double t = 0;
	for (int i = 0; i < size1 + 1; i++) {
		for (int j = i + 1; j < size1 + 1; j++) {
			sum[i][j] += sum[j][i];
			if (sum[i][j] != 0)
				s += -(double(sum[i][j]) / (si * size1)) * log(double(sum[i][j]) / (si * size1));
			sum[j][i] = sum[i][j];
			t += sum[i][j];
		}
	}
	if (t != si * size1)
		cout << "VGFCTYDR " << t << ' ' << si *size1 << endl;
	return s;

}

void d_diversity1(std::vector<Individual> &population, vector<double> &index) {
	int n = population.size();
	int size1 = population[0].cycle.size;
	vector<vector<int>>pop(n, vector<int>(size1));
	path(pop, population);
	double s = conti_divers2(pop, -1);
	vector<vector<double>> divs(n, vector<double>(2));
	for (int i = 0; i < n; i++) {
		double m = conti_divers2(pop, i);
		index.push_back(s - m);
	}
}

void compute_mean_var(std::vector<double> &obj) {
	double sumobj = 0, meanobj = 0;
	int N = obj.size();
	for (int i = 0; i < N; i++) {
		sumobj += obj[i];
	}
	meanobj = sumobj / N;
	double var = 0;
	for (int i = 0; i < N; i++) {
		var += (obj[i] - meanobj) * (obj[i] - meanobj);
	}
	var /= (N - 1);
	var = sqrt(var);
	if (var == 0) {
		var = 1;
	}
	for (int i = 0; i < N; i++) {
		obj[i] = (obj[i] - meanobj) / var;
	}
}

void DoulbeSharingSelection(std::vector<Individual> &lastin_population, int K, int nb, int mod,

                            std::vector<int> &Choose, std::vector<double> &fDS) {
	int N = lastin_population.size();
	cout << "NNNN" << N << ' ' << K << endl;
	nb = min(nb, max(N - 1, 0));
	std::vector<std::vector<double>> d_obj;
	std::vector<std::vector<int>> p_choose;
	pdist2(lastin_population, d_obj, p_choose);

	std::vector<std::vector<double>> d_dec;
	vector<double> index;
	d_diversity1(lastin_population, index);
	for (int i = 0; i < N; i++) {
		Choose[i] = 1;
	}
	int sumindx = N - K;
	int N1 = N;
	while (sumindx > 0) {
		std::vector<double>obj;
		std::vector<double>dec;
		std::vector<int>chooseindex;
		nb = min(nb, max(N1 - 1, 0));
		for (int i = 0; i < N; i++) {
			double temp = 0;
			if (Choose[i] == 1) {
				int j = 0, j1 = 0;
				while (j < N) {
					if (Choose[j] == 1) {
						temp += d_obj[i][p_choose[i][j + 1]] / nb;
						j1++;
					}
					if (j1 == nb)
						break;
					j++;
				}
				obj.push_back(temp);
				dec.push_back(index[i]);
				chooseindex.push_back(i);
			}
		}
		compute_mean_var(obj);
		compute_mean_var(dec);
		double del_obj_dec = INT_MAX;
		int del = 0;
		for (int i = 0; i < N1; i++) {
			if (del_obj_dec > obj[i] + dec[i]) {
				del_obj_dec = obj[i] + dec[i];
				del = chooseindex[i];
			}
		}
		Choose[del] = 0;
		sumindx--;
		N1--;
	}
}

void EnvironmentalSelection(std::vector<Individual> &next_population, std::vector<Individual> &population,
                            std::vector<int> &FrontNo, std::vector<double> &FDS, int parPOPULATION) {
	int maxFrontNo = 0;
	int size = next_population.size();
	std::vector<int>Frontno(size);
	NDsort(next_population, parPOPULATION, maxFrontNo, Frontno);
	std::vector<int>next(size);
	std::vector<int>last;
	int sumnext = 0;
	for (int i = 0; i < size; i++) {
		if (Frontno[i] < maxFrontNo) {
			sumnext++;
			next[i] = 1;
		} else if (Frontno[i] == maxFrontNo)
			last.push_back(i);
	}
	if (parPOPULATION - sumnext > 2) {
		next[last[0]] = 1;
		next[last[last.size() - 1]] = 1;
		last.erase(last.begin() + 0);
		last.erase(last.begin() + last.size() - 1);
		sumnext += 2;
	}
	std::vector<Individual> lastin_population(last.size());
	std::vector<int> Choose(last.size());
	std::vector<double> tempfDS(last.size());
	std::vector<double> fDS(size);
	for (int i = 0; i < size; i++) {
		fDS[i] = 0;
	}
	for (int i = 0; i < last.size(); i++) {
		lastin_population[i] = next_population[last[i]];
	}
	cout << "POPULATION-sumnext" << parPOPULATION - sumnext << endl;
	DoulbeSharingSelection(lastin_population, parPOPULATION - sumnext, 3, 0, Choose, tempfDS);
	for (int i = 0; i < last.size(); i++) {
		if (Choose[i] == 1) {
			next[last[i]] = 1;
			fDS[last[i]] = tempfDS[i];
		}
	}
	population.clear();
	FrontNo.clear();
	FDS.clear();
	for (int i = 0; i < next.size(); i++) {
		if (next[i] == 1) {
			population.push_back(next_population[i]);
			FrontNo.push_back(Frontno[i]);
			FDS.push_back(fDS[i]);
		}
	}

	cout << "size" << population.size() << endl;
}


