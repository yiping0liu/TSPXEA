#ifndef node_hpp
#define node_hpp



// Yiping Liu, Liting Xu, Yuyan Han, Xiangxiang Zeng, Gary G. Yen, and Hisao Ishibuchi, Evolutionary Multimodal Multiobjective Optimization for Traveling Salesman Problems, IEEE Transactions on Evolutionary Computation, Early Access, 2023, DOI:10.1109/TEVC.2023.3239546
//
// Please contact {yiping0liu@gmail.com} or {litingxu@hnu.edu.cn} if you have any problem.
class Node {

	public:
		int id;
		int left;
		int right;

	public:
		Node(int n) {
			this->id = n;
		}
		Node &operator=(const Node &n) {
			id = n.id;
			left = n.left;
			right = n.right;
			return *this;
		}
		bool operator==(const Node &n)const {
			return ((id == n.id && left == n.left && right == n.right) || (id == n.id && left == n.right && right == n.left));
		}

		bool operator!=(const Node &n)const {
			return !((id == n.id && left == n.left && right == n.right) || (id == n.id && left == n.right && right == n.left));
		}

		bool operator<(const Node &n)const {
			return id < n.id;
		}

		bool operator>(const Node &n)const {
			return id > n.id;
		}
};

#endif
