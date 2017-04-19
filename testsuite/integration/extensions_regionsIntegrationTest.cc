#include <config.h>
#include <boost/test/unit_test.hpp>
#include <fwdpp/sugar/GSLrng_t.hpp>
#include <fwdpp/extensions/regions.hpp>
#include <fwdpp/sample_diploid.hpp>
#include <fwdpp/util.hpp>
#include <fwdpp/type_traits.hpp>
#include <limits>
#include "../fixtures/sugar_fixtures.hpp"

using namespace KTfwd;

BOOST_FIXTURE_TEST_SUITE(test_regions, singlepop_popgenmut_fixture)

// Check that extensions::discrete_mut_model::make_mut can be bound
// with placeholders, that the resulting type is a valid
// mutation model, and can be passed to KTfwd::sample_diploid
BOOST_AUTO_TEST_CASE(discrete_mut_model_test_4)
{
    // attempt
    extensions::discrete_mut_model dm({ 0, 1 }, { 1, 2 }, { 1, 0.5 }, {}, {},
                                      {}, {});
    KTfwd::GSLrng_t<KTfwd::GSL_RNG_TAUS2> rng(0u);
    auto mmodel = std::bind(
        &extensions::discrete_mut_model::
            make_mut<KTfwd::traits::recycling_bin_t<decltype(pop.mutations)>,
                     decltype(pop.mut_lookup), decltype(pop.mutations)>,
        &dm, std::placeholders::_1, std::placeholders::_2, rng.get(), 0.001,
        0., 0u, std::ref(pop.mut_lookup));
    static_assert(traits::is_mutation_model<decltype(mmodel), poptype::mcont_t,
                                            poptype::gcont_t>::value,
                  "error: type mutation_model is not a dispatchable mutation "
                  "model type!");

    auto wbar = KTfwd::sample_diploid(
        rng.get(), pop.gametes, pop.diploids, pop.mutations, pop.mcounts, 1000,
        0.001, mmodel, std::bind(KTfwd::poisson_xover(), rng.get(), 0.001, 0.,
                                 2., std::placeholders::_1,
                                 std::placeholders::_2, std::placeholders::_3),
        std::bind(KTfwd::multiplicative_diploid(), std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3, 2.),
        pop.neutral, pop.selected);
}

// Test the convenience fxn
BOOST_AUTO_TEST_CASE(discrete_mut_model_test_5)
{
    // attempt
    extensions::discrete_mut_model dm({ 0, 1 }, { 1, 2 }, { 1, 0.5 }, {}, {},
                                      {}, {});
    KTfwd::GSLrng_t<KTfwd::GSL_RNG_TAUS2> rng(0u);

    auto wbar = KTfwd::sample_diploid(
        rng.get(), pop.gametes, pop.diploids, pop.mutations, pop.mcounts, 1000,
        0.001, extensions::bind_dmm(dm, pop.mutations, pop.mut_lookup,
                                    rng.get(), 0.001, 0., generation),
        std::bind(KTfwd::poisson_xover(), rng.get(), 0.001, 0., 2.,
                  std::placeholders::_1, std::placeholders::_2,
                  std::placeholders::_3),
        std::bind(KTfwd::multiplicative_diploid(), std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3, 2.),
        pop.neutral, pop.selected);
}

/*
  Now, test discrete_mut_model's constructor that takes labels,
  a feature introduced in 0.4.9.  The purpose of this is to
  assign to mutation_base::xtra, which allows mutations to be integer-labelled.
*/
BOOST_AUTO_TEST_CASE(discrete_mut_model_test_6)
// This is an 'integration' test, I guess...
{
    // attempt
    extensions::discrete_mut_model dm(
        { 0, 1 },   // starts of 'neutral' regions
        { 1, 2 },   // ends of 'neutral' regions
        { 1, 0.5 }, // weights on 'neutral' regions
        {},         // starts of 'selected' regions
        {},         // stops of 'selected' regions
        {},         // weights on 'selected' regions
        { 0,
          1 }, // labels to put on mutations from each of the 'neutral' regions
        {}, // labels to put on mutations from each of the 'selected' regions
        {}  // vector of shmodels
        );

    // now, evolve the population
    KTfwd::GSLrng_t<KTfwd::GSL_RNG_TAUS2> rng(0u);
    auto mmodel = std::bind(
        &extensions::discrete_mut_model::
            make_mut<KTfwd::traits::recycling_bin_t<decltype(pop.mutations)>,
                     decltype(pop.mut_lookup), decltype(pop.mutations)>,
        &dm, std::placeholders::_1, std::placeholders::_2, rng.get(), 0.001,
        0., 0u, std::ref(pop.mut_lookup));
    static_assert(traits::is_mutation_model<decltype(mmodel), poptype::mcont_t,
                                            poptype::gcont_t>::value,
                  "error: type mutation_model is not a dispatchable mutation "
                  "model type!");
    auto wbar = KTfwd::sample_diploid(
        rng.get(), pop.gametes, pop.diploids, pop.mutations, pop.mcounts, 1000,
        0.01, // mutation rate--high so that there are lots of mutations to
              // test below...
        mmodel, std::bind(KTfwd::poisson_xover(), rng.get(), 0.001, 0., 2.,
                          std::placeholders::_1, std::placeholders::_2,
                          std::placeholders::_3),
        std::bind(KTfwd::multiplicative_diploid(), std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3, 2.),
        pop.neutral, pop.selected);
    // Check that mutations in certain position intervals have the correct
    // label
    for (const auto& m : pop.mutations)
        {
            if (m.pos < 1)
                {
                    BOOST_REQUIRE_EQUAL(m.xtra, 0);
                }
            else
                {
                    BOOST_REQUIRE_EQUAL(m.xtra, 1);
                }
        }
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_FIXTURE_TEST_SUITE(test_multilocus_regions, multiloc_popgenmut_fixture)
BOOST_AUTO_TEST_CASE(test_bind_vec_drm)
{
    double length = 10.;
    std::vector<extensions::discrete_rec_model> vdrm;
    for (unsigned i = 0; i < 4; ++i)
        {
            double begin = static_cast<double>(i) * length;
            extensions::discrete_rec_model drm(
                { begin, begin + 3., begin + 7. },
                { begin + 3., begin + 7., begin + length }, { 1., 10., 1. });
            vdrm.emplace_back(std::move(drm));
        }
    auto bound = extensions::bind_vec_drm(vdrm, pop.gametes, pop.mutations,
                                          rng.get(), mu);

    double wbar = sample_diploid(
        rng.get(), pop.gametes, pop.diploids, pop.mutations, pop.mcounts,
        pop.N, &mu[0], mutmodels, bound, &rbw[0],
        [](const gsl_rng* __r, const double __d) {
            return gsl_ran_binomial(__r, __d, 1);
        },
        std::bind(multilocus_additive(), std::placeholders::_1,
                  std::placeholders::_2, std::placeholders::_3),
        pop.neutral, pop.selected);
}

BOOST_AUTO_TEST_SUITE_END()
