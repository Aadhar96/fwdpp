//  -*- C++ -*- 
#ifndef _MUTATION_TCC_
#define _MUTATION_TCC_

#include <type_traits>
#include <algorithm>
#include <numeric>

#include <gsl/gsl_randist.h>

#if defined(HAVE_BOOST_VECTOR) && !defined(USE_STANDARD_CONTAINERS)
#include <boost/container/vector.hpp>
#endif

#include <fwdpp/internal/mutation_internal.hpp>
#include <fwdpp/tags/mutation_tags.hpp>
#include <vector>

namespace KTfwd
{
  template< typename iterator_type,
	    typename mutation_model,
	    typename mutation_insertion_policy,
	    typename gamete_insertion_policy,
	    typename list_type_allocator,
	    typename list_type_allocator2,
	    template<typename,typename> class list_type,
	    template<typename,typename> class list_type2>
  iterator_type mutate_gamete( gsl_rng * r,
			       const double & mu, list_type< typename iterator_type::value_type,list_type_allocator > * gametes,
			       list_type2<typename iterator_type::value_type::mutation_type,list_type_allocator2 > * mutations, 
			       iterator_type &g,
			       const mutation_model &mmodel,
			       const mutation_insertion_policy & mpolicy,
			       const gamete_insertion_policy & gpolicy)
  {
    assert( g != gametes->end() );
    unsigned nm = gsl_ran_poisson(r,mu);
    if ( nm )
      {
	assert( g->n > 0 );
	g->n--;
	typename iterator_type::value_type ng( 1, g->mutations,g->smutations);
	fwdpp_internal::add_N_mutations(mmodel,mpolicy,nm,mutations,ng,
					typename std::is_base_of<tags::gamete_dependent,mutation_model>::type());
	return gpolicy(std::move(ng),gametes);
      }
    return g;
  }
}
#endif /* _MUTATION_TCC_ */
