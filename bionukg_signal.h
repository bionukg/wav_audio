#pragma once
#signal.h


#include<stdint.h>
const double PI = acos(-1.0);

class Complex {
private:
	
public:
	double r, i;//实部和虚部


	Complex() { r = 0; i = 0; }
	Complex(double r, double i) { this->r = r; this->i = i; }
	Complex(double direction) { this->r = cos(direction); this->i = sin(direction); }

	void real(const double& x) { r = x; }
	void image(const double& x) { i = x; }
	void re_im(const double& x, const double& y) { r = x; i = y; }
	double real() { return r; }
	double image() { return i; }
	void show_ri(void)
	{
		printf("%3.3e + %3.3e i\n", r, i);
		//std::cout << r << '+' << i <<" i" << '\n';
	}
	Complex operator + (const Complex& x)  const {
		return Complex(r + x.r, i + x.i);
	}
	Complex operator - (const Complex& x)  const {
		return Complex(r - x.r, i - x.i);
	}
	Complex operator * (const Complex& x)  const {
		return Complex(r * x.r - i * x.i, r * x.i + i * x.r);
	}
	void operator /= (const double& x) {
		r /= x, i /= x;
	}
	void operator *= (const Complex& x) {
		*this = Complex(r * x.r - i * x.i, r * x.i + i * x.r);
	}
	void operator += (const Complex& x) {
		r += x.r, i += x.i;
	}
	void operator =(const int& x)
	{
		r = x, i = 0;
	}

	inline Complex conj() {
		return Complex(r, -i);
	}
};
Complex* ft(Complex* t, Complex* f, const int& N)
{
	if (f == 0)
		return 0;
	Complex omega;
	for (int k = 0; k < N; k++)
	{
		f[k] = Complex(0, 0);
		for (int n = 0; n < N; n++)
		{
			omega = Complex(-2 * PI * k * n / N);
			f[k] += t[n] * omega;//以方向表示的模长为1的复数
		}
	}
	return f;
}
Complex* ft2(Complex* t, Complex* f, const int& N2)
{
	if (f == 0)
		return 0;
	Complex omega;
	for (int k = 0; k < N2; k++)
	{
		f[k] = Complex(0, 0);
		f[k + N2] = Complex(0, 0);
		Complex f1, f2;
		for (int n = 0; n < N2; n++)
		{
			omega = Complex(-2 * PI * k * n / N2);
			f1 += t[2 * n] * omega;
			f2 += t[2 * n + 1] * omega;
		}
		f2 = f2 * Complex(-1 * PI * k / N2);
		f[k] = f1 + f2;
		f[k + N2] = f1 - f2;
	}
	return f;
}
Complex* fft_r(Complex* t, Complex* f, int N)
{
	if (f == 0)
		return 0;
	if (N == 1)
	{
		*f = *t;
		return f;
	}
	N /= 2;
	Complex* t1 = new Complex[N], * t2 = new Complex[N],
		* f1 = new Complex[N], * f2 = new Complex[N];

	for (int i = 0; i < N; i++)
	{
		t1[i] = t[2 * i];
		t2[i] = t[2 * i + 1];
	}
	fft_r(t1, f1, N);
	fft_r(t2, f2, N);

	Complex omega;

	for (int k = 0; k < N; k++)
	{
		omega = Complex(-1 * PI * k / N);;
		f[k] = f1[k] + f2[k] * omega;
		f[k + N] = f1[k] - f2[k] * omega;
	}
	delete[]t1, t2, f1, f2;
	return f;
}
Complex* fft_rs(_Inout_ Complex* io,_In_ uint32_t N)
{
	if (io == 0)
		return 0;
	if (N == 1)
	{		
		return io;
	}
	N /= 2;
	Complex* io0 = new Complex[N], * io1 = new Complex[N];

	for (int i = 0; i < N; i++)
	{
		io0[i] = io[2 * i];
		io1[i] = io[2 * i + 1];
	}
	fft_rs(io0, N);
	fft_rs(io1, N);

	Complex omega;

	for (int k = 0; k < N; k++)
	{
		omega = Complex(-1 * PI * k / N);;
		io[k] = io0[k] + io1[k] * omega;
		io[k + N] = io0[k] - io1[k] * omega;
	}
	delete[]io0, io1;
	return io;
}
Complex* fft_bs(_Inout_ Complex* io, _In_ uint32_t N);
