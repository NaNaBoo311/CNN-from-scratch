/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt
 * to change this license Click
 * nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this
 * template
 */

/*
 * File:   Softmax.cpp
 * Author: ltsach
 *
 * Created on August 25, 2024, 2:46 PM
 */

#include "layer/Softmax.h"

#include <filesystem>  //require C++17

#include "ann/functions.h"
#include "sformat/fmt_lib.h"
namespace fs = std::filesystem;

Softmax::Softmax(int axis, string name) : m_nAxis(axis) {
  if (trim(name).size() != 0)
    m_sName = name;
  else
    m_sName = "Softmax_" + to_string(++m_unLayer_idx);
}

Softmax::Softmax(const Softmax& orig) {}

Softmax::~Softmax() {}

xt::xarray<double> Softmax::forward(xt::xarray<double> X) {
  // Todo CODE YOUR
  // if (X.dimension() == 1) {
  //   xt::xarray<double> exp_arr = xt::exp(X);
  //   xt::xarray<double> sum_exp = xt::sum(exp_arr);
  //   this->m_aCached_Y = exp_arr / sum_exp;
  // }

  // else if (X.dimension() > 1) {
  //   xt::xarray<double> Y = xt::zeros_like(X);
  //   for (int i = 0; i < X.shape()[0]; i++) {
  //     xt::xarray<double> X_i = xt::view(X, i);
  //     xt::xarray<double> exp_arr_i = xt::exp(X_i);
  //     xt::xarray<double> sum_exp_i = xt::sum(exp_arr_i);

  //     xt::view(Y, i) = exp_arr_i / sum_exp_i;
  //   }
  //   this->m_aCached_Y = Y;
  // }
  this->m_aCached_Y = softmax(X, m_nAxis);
  return this->m_aCached_Y;
}

xt::xarray<double> Softmax::backward(xt::xarray<double> DY) {
  // Todo CODE YOUR
  //∆z = (DIAG(y) − y ⊗ yT ) × ∆y
  xt::xarray<double> DZ = xt::zeros_like(DY);
  if (DY.dimension() == 1) {
    xt::xarray<double> outer_product = xt::linalg::outer(this->m_aCached_Y, xt::transpose(this->m_aCached_Y));
    DZ = xt::linalg::dot(xt::diag(this->m_aCached_Y) - outer_product, DY);
  }

  else if (DY.dimension() > 1) {
    for (int i = 0; i < DY.shape()[0]; i++) {
      xt::xarray<double> DY_i = xt::view(DY, i);
      xt::xarray<double> Y_i = xt::view(this->m_aCached_Y, i);
      xt::xarray<double> outer_product_i = xt::linalg::outer(Y_i, xt::transpose(Y_i));

      xt::view(DZ, i) = xt::linalg::dot(xt::diag(Y_i) - outer_product_i, DY_i);
    }
  }
  return DZ;
}

string Softmax::get_desc() {
  string desc = fmt::format("{:<10s}, {:<15s}: {:4d}", "Softmax",
                            this->getname(), m_nAxis);
  return desc;
}
