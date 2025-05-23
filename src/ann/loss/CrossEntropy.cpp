/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt
 * to change this license Click
 * nbfs://nbhost/SystemFileSystem/Templates/cppFiles/class.cc to edit this
 * template
 */

/*
 * File:   CrossEntropy.cpp
 * Author: ltsach
 *
 * Created on August 25, 2024, 2:47 PM
 */

#include "loss/CrossEntropy.h"

#include "ann/functions.h"

CrossEntropy::CrossEntropy(LossReduction reduction) : ILossLayer(reduction) {}

CrossEntropy::CrossEntropy(const CrossEntropy &orig) : ILossLayer(orig) {}

CrossEntropy::~CrossEntropy() {}

double CrossEntropy::forward(xt::xarray<double> X, xt::xarray<double> t)
{
  // Todo CODE YOUR
  m_aYtarget = t;
  m_aCached_Ypred = X;

  // int N = (m_eReduction == REDUCE_MEAN) ? X.shape()[0] : 1;
  // xt::xarray<double> res;

  // // Formula 26
  // if (X.size() == t.size())
  // {
  //   res = (-1.0 / N) * xt::sum(t * xt::log(X))();
  // }
  // //Formula 27
  // else
  // {
  //   xt::xarray<double> sum = {0.0}; 
  //   for (int row = 0; row < N; row++)
  //   {
  //     int col = t(row);
  //     xt::xarray<double> X_value = {X(row, col)};
  //     xt::xarray<double> log_value = xt::log(X_value);
  //     sum += log_value;
  //   }
  //   res = (-1.0 / N) * sum();
  // }


  int nsamples = X.shape()[0];
  xt::xarray<double> logYpred = xt::log(X + 1e-7);
  xt::xarray<double> R = -t * logYpred;
  R = xt::sum(R, -1);

  xt::xarray<double> sum = xt::sum(R);
  if (m_eReduction == REDUCE_MEAN)
    return (sum / nsamples)[0];
  else
    return sum[0];

  return 0.0;
}
xt::xarray<double> CrossEntropy::backward()
{
  // Todo CODE YOUR
  xt::xarray<double> T;
  int N = m_aCached_Ypred.shape()[0];

  //Reconstruct T in case hard-label t = {0,1,2...N-1}
  if (m_aYtarget.size() != m_aCached_Ypred.size()) {
    T = xt::zeros_like(m_aCached_Ypred);
    for (int i = 0; i < N; i++) {
        T(i, int(m_aYtarget(i))) = 1;
    }
  }

  else {
    T = m_aYtarget;
  }

  xt::xarray<double> res =  (-1.0/N) * (T / (m_aCached_Ypred + 1e-7));
  return res;
  // int N_norm = this->m_aCached_Ypred.shape(0);
  // xt::xarray<double> DY = (-1.0/N_norm) * (this->m_aYtarget/(this->m_aCached_Ypred + 1e-7));
  // return DY;
}