﻿#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include "methods.h"
#include "funcs.h"
#include "visualize/visualize.h"

void makeFirstTable(const Optimizator& o, const ArgMinFunction& argmin, const Function& f, const Vector& x0, const std::string& file) {
	std::ofstream fout(file + ".txt");
	fout << std::setprecision(10);
	fout << "x0 = " << x0 << std::endl;
	fout << "10^i\titer\tfCount\tgrad.norm()\tvalue" << std::endl;
	for (int i = 3; i < 7; ++i) {
		double eps = pow(10.0, -double(i));
		auto result = o(f, argmin, x0, eps);
		fout << -i << "\t" << result.iterations << "\t" << result.fCount << "\t" << result.steps.back().grad.norm() << "\t" << result.steps.back().value << std::endl;
	}
	fout.close();
}

void makeSecondTable(const Optimizator& o, const ArgMinFunction& argmin, const Function& f, const Vector& x0, const double& eps, const std::string& file) {
	auto result = o(f, argmin, x0, eps);
	std::ofstream fout(file + ".txt");
	fout << std::setprecision(10);

	fout << "x0 = " << x0 << ", eps=" << eps << std::endl;

	fout << "answer: " << result.answer << ", fCount: " << result.fCount << ", exit type: " << result.exit << ", iterations: " << result.iterations << std::endl;

	fout << "i\tpoint\tvalue\tdir\tlambda\tdiff_point\tdiff_value\tgrad\thessian" << std::endl;
	auto lastPoint = result.steps.front().point;
	auto lastValue = result.steps.front().value;
	int counter = 0;
	for (auto& i : result.steps) {
		Vector diff_point = Vector(i.point - lastPoint);
		double diff_value = std::fabs(i.value - lastValue);
		fout << counter << "\t" << i.point << "\t" << i.value << "\t" << i.dir << "\t" << i.lambda << "\t" << diff_point << "\t" << diff_value << "\t" << i.grad << "\t\\scalebox{.5}{" << i.hessian << "}" << std::endl;
		lastPoint = i.point;
		lastValue = i.value;
		counter++;
		if (counter % 25 == 0 && counter != 0) {
			fout.close();
			fout.open(file + "_" + std::to_string(counter / 25) + ".txt");
			fout << "next table part" << std::endl << "empty line for compability" << std::endl;
			fout << "i\tpoint\tvalue\tdir\tlambda\tdiff_point\tdiff_value\tgrad\thessian" << std::endl;
		}
	}

	fout.close();
}

void makeTableForF2(const Vector& x0, const ArgMinFunction& argmin, const std::string& file) {
	std::ofstream fout(file + ".txt");
	fout << "Table for function t*(y-x^2)^2 + (1-x)^2: " << std::endl;
	fout << "t\tfCount" << std::endl;

	for (int i = 0; i < 1000; i++) {
		double t = i / 10.0;
		auto f = [t](const Vector& v) -> double {
			const double& x = v(0);
			const double& y = v(1);
			return t * (y - x * x)*(y - x * x) + (1 - x)*(1 - x);
		};
		auto result = optimizeConjugateGradient(f, argmin, x0, 1e-3);
		fout << t << "\t" << result.fCount << std::endl;
	}

	fout.close();
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
int main() {
	Vector x0(2); x0 << 0, 0;
	double eps = 1e-3;

	auto argmin = bindArgmin(optimizeGoldenRatio);
	auto argminParabola = bindArgmin(optimizeParabola);

	std::vector<std::pair<Function, std::string>> funcs = {
		{f1, "f1"}, 
		{f2, "f2"}, 
		{f3, "f3"}, 
	};

	for (auto& i : funcs) {
		std::cout << "Draw " << i.second << std::endl;
		// Строим рисунки зависимости числа вычислений функции от положения
		visualize(optimizeBroyden, optimizeConjugateGradient, argmin, i.first, x0, 0.001, 500, L"Метод Бройдена", L"Метод Сопряженных Градиентов", "image_" + i.second);

		// Первая и вторая таблица для метода Бройдена
		makeFirstTable(optimizeBroyden, argmin, i.first, x0, "table1_" + i.second + "_broyden");
		makeSecondTable(optimizeBroyden, argmin, i.first, x0, eps, "table2_" + i.second +"_broyden");

		// Первая и вторая таблица для метода сопряженных градиентов
		makeFirstTable(optimizeConjugateGradient, argmin, i.first, x0, "table1_" + i.second + "_gradient");
		makeSecondTable(optimizeConjugateGradient, argmin, i.first, x0, eps, "table2_" + i.second +"_gradient");
	}

	std::cout << "Draw parabola" << std::endl;
	auto f = funcs.back().first;
	visualize(optimizeBroyden, optimizeConjugateGradient, argminParabola, f, x0, 0.001, 500, L"Метод Бройдена", L"Метод Сопряженных Градиентов", "image_parabola");
	makeFirstTable(optimizeBroyden, argminParabola, f, x0, "table1_parabola_broyden");
	makeFirstTable(optimizeConjugateGradient, argminParabola, f, x0, "table1_parabola_gradient");

	makeTableForF2(x0, argmin, "table_f2");
}