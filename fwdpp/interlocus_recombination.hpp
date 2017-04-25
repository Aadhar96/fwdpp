//
// Copyright (C) 2017 Kevin Thornton <krthornt@uci.edu>
//
// This file is part of fwdpp.
//
// fwdpp is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// fwdpp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with fwdpp.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef FWDPP_INTERLOCUS_RECOMBINATION_HPP__
#define FWDPP_INTERLOCUS_RECOMBINATION_HPP__

#include <vector>
#include <functional>
#include <fwdpp/fwd_functional.hpp>

namespace KTfwd
{
    inline std::vector<std::function<unsigned(void)>>
    make_poisson_interlocus_rec(const gsl_rng* r, const double* means,
                                const std::size_t n)
    /// Create a vector of KTfwd::poisson_interlocus_rec
    {
        std::vector<std::function<unsigned(void)>> rv;
        for (std::size_t i = 0; i < n; ++i)
            {
                rv.emplace_back(
                    std::bind(poisson_interlocus_rec(means[i]), r));
            }
        return rv;
    }

    inline std::vector<std::function<unsigned(void)>>
    make_binomial_interlocus_rec(const gsl_rng* r, const double* cM,
                                 const std::size_t n)
    /// Create a vector of KTfwd::binomial_interlocus_rec
    {
        std::vector<std::function<unsigned(void)>> rv;
        for (std::size_t i = 0; i < n; ++i)
            {
                rv.emplace_back(std::bind(binomial_interlocus_rec(cM[i]), r));
            }
        return rv;
    }
}

#endif
