#include "UnivariatePolynomial.hpp"
#include <utility>
#include <stdexcept>
#include <iostream>

UnivariatePolynomial::UnivariatePolynomial() {}

UnivariatePolynomial::UnivariatePolynomial(const std::vector<double>& coeffs) : coefficients(coeffs) {}

UnivariatePolynomial::UnivariatePolynomial(const int degree) 
{
	this->coefficients.resize(degree + 1);
}

UnivariatePolynomial::UnivariatePolynomial( int degree, std::vector<double>& coeffs)
{
	coefficients.resize(degree + 1);
	coefficients = coeffs;
}

UnivariatePolynomial::~UnivariatePolynomial() {}

void UnivariatePolynomial::print() const
{
	size_t degree = coefficients.size() -1;
	bool printed = false;
	for (size_t i = degree; i > 0; --i) {
		double coef = coefficients[i];
		if (coef != 0.0) {
			if (printed && coef > 0.0)
				std::cout << " + ";
			else if (coef < 0.0) {
				std::cout << " - ";
				coef = -coef;
			}
			std::cout << coef;
			if (i > 1)
				std::cout << "x^" << i;
			else if (i == 1)
				std::cout << "x";
			printed = true;
		}
	}
	double coef = coefficients[0];
	if (coef != 0.0) {
		if (printed && coef > 0.0)
			std::cout << " + ";
		else if (coef < 0.0) {
			std::cout << " - ";
			coef = -coef;
		}
		std::cout << coef;
	}
	else if (!printed)
		std::cout << "0";
	std::cout << std::endl;
}

double UnivariatePolynomial::evaluate_horner(double x) const
{
	double result = coefficients.back(); // �������� � ��������� �������
	for (int i = coefficients.size() - 1; i >= 0; i--) {
		result = coefficients[i] + x * result; // ��������� �������� �� ����� �������
	}
	return result;
}

int UnivariatePolynomial::getDegree() const
{
	return coefficients.size() - 1;
}

double UnivariatePolynomial::getCoefficient(int power) const
{
	if (power >= 0 && power < coefficients.size() - 1) {
		return coefficients[power];
	}
	return 0.0;
}

void UnivariatePolynomial::setCoefficient(int power, double value)
{
	if (power >= 0) {
		if (power >= coefficients.size() - 1) {
			coefficients.resize(power + 1, 0.0);
		}
		coefficients[power] = value;
	}
}

bool UnivariatePolynomial::isZero() const
{
	return coefficients.empty() || (coefficients.size() == 1 && coefficients[0] == 0);
}

double& UnivariatePolynomial::operator[](int index)
{
	if (index < 0 || index > coefficients.size()) {
		throw std::out_of_range("Index out of range");
	}
	return coefficients[index];
}

UnivariatePolynomial UnivariatePolynomial::operator+(const UnivariatePolynomial& other) const
{
	int degree = coefficients.size() - 1,
		other_degree = other.coefficients.size() - 1,
		new_degree = std::max(degree, other_degree);
	std::vector<double> result_coeffs(new_degree + 1, 0.0);
	for (int i = 0; i <= new_degree; i++)
		result_coeffs[i] = (i <= coefficients.size() - 1 ? coefficients[i] : 0.0) + (i <= other.coefficients.size() - 1 ? other.coefficients[i] : 0.0);
	return UnivariatePolynomial(result_coeffs);
}
UnivariatePolynomial UnivariatePolynomial::operator-(const UnivariatePolynomial& other) const
{
	int degree = coefficients.size() - 1,
		other_degree = other.coefficients.size() - 1,
		new_degree = std::max(degree, other_degree);
	std::vector<double> result_coeffs(new_degree + 1, 0.0);
	for (int i = 0; i <= new_degree; i++)
		result_coeffs[i] = (i <= degree ? coefficients[i] : 0.0) - (i <= other_degree ? other.coefficients[i] : 0.0);
	return UnivariatePolynomial(result_coeffs);
}

std::vector<double> UnivariatePolynomial::findRoots() const //���������� ����������� �����������
{
	std::vector<double> roots;
	int n = coefficients.size() - 1;

	// Find the root of the highest degree coefficient
	double a = coefficients[n];
	double root = sqrt(abs(a));
	if (a < 0) {
		roots.push_back(-root);
		roots.push_back(root);
	}
	else {
		roots.push_back(root);
		roots.push_back(-root);
	}

	// Find the remaining roots
	for (int i = 1; i <= n / 2; i++) {
		double a = coefficients[n - 2 * i + 1];
		double b = coefficients[n - 2 * i];
		double c = roots[0];
		double d = 0;
		for (int j = 1; j < i; j++) {
			d += 2 * j * roots[j] * coefficients[n - 2 * i + 2 * j];
		}
		double root = (b - c * d) / a;
		roots.insert(roots.begin(), root);
		roots.push_back(-root);
	}

	return roots;
}

UnivariatePolynomial UnivariatePolynomial::operator*(const UnivariatePolynomial& other) const
{
	int degree = coefficients.size() - 1,
		other_degree = other.coefficients.size() - 1,
		new_degree = degree + other_degree;
	std::vector<double> result_coeffs((new_degree + 1), 0.0);

	for (int i = 0; i <= new_degree; i++) {
		result_coeffs[i] = 0;
		for (int j = 0; j <= degree; j++) {
			if (i - j >= 0 && i - j <= other_degree) {
				result_coeffs[i] += coefficients[j] * other.coefficients[i - j];
			}
		}
	}
	return UnivariatePolynomial(result_coeffs);
}

std::pair<UnivariatePolynomial, UnivariatePolynomial> UnivariatePolynomial::operator/(const UnivariatePolynomial& other) const
{
	if (other.isZero()) {
		throw std::invalid_argument("Division by zero!");
	}
	int degree = coefficients.size() - 1,
		other_degree = other.coefficients.size() - 1;
	std::vector<double> coefficients_quotient(degree - other_degree + 1);
	std::vector<double> coefficients_remainder(coefficients.begin(), coefficients.end());
	for (int i = degree - other_degree; i >= 0; i--) {

		double quotientCoefficient = coefficients_remainder[other_degree + i] / other.coefficients[other_degree];
		coefficients_quotient[i] = quotientCoefficient;
	
		for (int j = 0; j <= other.getDegree(); j++) {
			int index = i + j;
			coefficients_remainder[index] -= quotientCoefficient * other.coefficients[j];
		}
	}

	UnivariatePolynomial quotient(coefficients_quotient);
	UnivariatePolynomial remainder(coefficients_remainder);
	std::pair<UnivariatePolynomial, UnivariatePolynomial> result(quotient, remainder);
	return result;
}
