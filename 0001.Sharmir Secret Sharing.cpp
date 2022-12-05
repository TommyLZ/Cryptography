//shamir’s secret sharing algorithm C++ 实现

#include <bits/stdc++.h>
using namespace std;

// 计算y的值，y = poly[0] + x*poly[1] + x^2*poly[2] + ...
int calculate_Y(int x, vector<int>& poly)
{
	// 初始化 y
	int y = 0;
	int temp = 1;

	// 数组迭代
	for (auto coeff : poly) {

		// 计算y的值
		y = (y + (coeff * temp));
		temp = (temp * x);
	}
	return y;
}

// 进行秘密共享然后编码
void secret_sharing(int S, vector<pair<int, int> >& points,
					int N, int K)
{
	vector<int> poly(K);	// 存储次数为（k-1)的多项式系数
	
	// 随机选择K - 1非零数，poly[0]是秘密值
	// 创建一个这样的多项式
	poly[0] = S;	// 常数项为秘密值

	for (int j = 1; j < K; ++j) {
		int p = 0;
		while (p == 0) // 多项式系数必须是非0值，如果产生0值，则继续尝试
			p = (rand() % 997);	// 随机选择其他系数值，进行模操作
		poly[j] = p;
	}

	// 从创建的多项式中生成N个点
	for (int j = 1; j <= N; ++j) {
		int x = j;
		int y = calculate_Y(x, poly);

		// 存储计算过程中产生的点
		points[j - 1] = { x, y };
	}
}

// This structure is used for fraction part handling multiplication and addition of fraction
struct fraction {
	int num, den; // 分子numerator; 分母denominator

	fraction(int n, int d)
	{
		num = n, den = d;
	}

	// 如果分数不是最简，分子分母同除以其最大公因数
	void reduce_fraction(fraction& f)
	{
		int gcd = __gcd(f.num, f.den);	// 内置函数
		f.num /= gcd, f.den /= gcd;
	}

	// 重载乘法运算符
	fraction operator*(fraction f)
	{
		fraction temp(num * f.num, den * f.den); // 分子*分子, 分母*分母
		reduce_fraction(temp);
		return temp;
	}

	// 重载加法运算符
	fraction operator+(fraction f)
	{
		fraction temp(num * f.den + den * f.num, den * f.den);

		reduce_fraction(temp); // 转换为最简分数
		return temp;
	}
};

// 对秘密值进行恢复
// 使用拉格朗日插值法
// 当x=0时，常数项就是多项式的值，所以在进行拉格朗日插值计算的时候，x可以直接取0
// Instead of finding the complete Polynomial We only required the poly[0] as our secret code, thus we can get rid of x terms
int Generate_Secret(int x[], int y[], int M)
{

	fraction ans(0, 1);

	for (int i = 0; i < M; ++i) {

		// 对分数进行初始化
		fraction l(y[i], 1);
		for (int j = 0; j < M; ++j) {

			// 计算拉格朗日的项
			if (i != j) {
				fraction temp(-x[j], x[i] - x[j]); // x直接取0，就能够得到秘密值
				l = l * temp; // 使用重载的函数运算符
			}
		}
		ans = ans + l; // 累加
	}

	// 返回秘密值
	return ans.num;
}

// 编码和解码给定秘密值
void operation(int S, int N, int K)
{

	// 用来存储点的向量
	vector<pair<int, int> > points(N);

	// 执行秘密共享
	secret_sharing(S, points, N, K);

	cout << "Secret is divided to " << N << " Parts - " << endl;

	for (int i = 0; i < N; ++i) {
		cout << points[i].first << " " << points[i].second << endl;
	}

	cout << "We can generate Secret from any of " << K << " Parts" << endl;

	int M = 2;

	// 门限检测，M要求大于等于门限值才能将原始秘密值恢复
	if (M < K) {
		cout << "Points are less than threshold " << K << " Points Required" << endl;
	}

	int* x = new int[M];
	int* y = new int[M];

	// 输入M个点，就能够得到秘密值
	// 本实验中，直接采用前M的点
	// 事实上，我们可以随机选择M个点
	for (int i = 0; i < M; ++i) {
		x[i] = points[i].first;
		y[i] = points[i].second;
	}

	// 对秘密值进行恢复并输出
	cout << "Our Secret Code is : " << Generate_Secret(x, y, M) << endl;
}

int main()
{
	int S = 65;	// 原始秘密值
	int N = 4;  // 分割成4个秘密值
	int K = 2;  // 门限为2

	operation(S, N, K);
	return 0;
}