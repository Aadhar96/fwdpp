#ifndef __FWDPP_INTERNAL_MUTATION_HPP__
#define __FWDPP_INTERNAL_MUTATION_HPP__

#include <algorithm>
//#include <fwdpp/tags/mutation_tags.hpp>
#include <fwdpp/type_traits.hpp>

namespace KTfwd {
  namespace fwdpp_internal
  {
    /*!
      Mechanics of adding a new mutation.
      We do this odd choosing between
      find_if and upper_bound 
      b/c of the way the GCC STL is implemented.
      
      Objectively, upper_bound should always  be preferred,
      but that is not the case in practice because the GNU STL
      implements find_if in a very clever way for
      small containers.
    */
    template< typename gamete_type,
	      typename mutation_iterator>
    void add_new_mutation( mutation_iterator & mitr,
			   gamete_type & new_gamete )
    {
      if(mitr->neutral)
	{
	  new_gamete.mutations.insert( (new_gamete.mutations.size()<=250) ? 
				       std::find_if(new_gamete.mutations.begin(),
						    new_gamete.mutations.end(),
						    [&](const mutation_iterator & mut_itr)
						    {
						      return mut_itr->pos > mitr->pos;
						    }) :
				       std::upper_bound(new_gamete.mutations.begin(),
							new_gamete.mutations.end(),mitr->pos,
							[](const double & __value,const mutation_iterator & __mut){ return __value < __mut->pos;}),
				       mitr );
	}
      else
	{
	  new_gamete.smutations.insert( (new_gamete.smutations.size() <= 250) ? 
					std::find_if(new_gamete.smutations.begin(),
						     new_gamete.smutations.end(),
						     [&](const mutation_iterator & mut_itr)
						     {
						       return mut_itr->pos > mitr->pos;
						     }) :
					std::upper_bound(new_gamete.smutations.begin(),
							 new_gamete.smutations.end(),mitr->pos,
							 [](const double & __value,const mutation_iterator & __mut){ return __value < __mut->pos;}),
					mitr );
	}
    }

    template<typename mmodel,
	     typename gamete_type,
	     typename mlist_type>
    inline
    typename std::enable_if< KTfwd::traits::is_nullary_t<mmodel>::value,
			     typename std::result_of<mmodel()>::type >::type
    mmodel_dispatcher( const mmodel & m, gamete_type &, mlist_type * ) 
    {
      return m();
    }
    
    template<typename mmodel,
	     typename gamete_type,
	     typename mlist_type>
    inline
    typename std::enable_if< KTfwd::traits::is_unary_t<mmodel,gamete_type&>::value,
			     typename std::result_of<mmodel(gamete_type &)>::type >::type
    mmodel_dispatcher( mmodel & m, gamete_type & g, mlist_type * ) 
    {
      return m(g);
    }
    
    template<typename mmodel,
	     typename gamete_type,
	     typename mlist_type>
    inline
    typename std::enable_if< KTfwd::traits::is_unary_t<mmodel,mlist_type *>::value,
			     typename std::result_of<mmodel(mlist_type *)>::type >::type
    mmodel_dispatcher( mmodel & m, gamete_type & , mlist_type * mutations) 
    {
      return m(mutations);
    }
    
    template<typename mmodel,
    	     typename gamete_type,
    	     typename mlist_type>
    inline
    typename std::enable_if< (KTfwd::traits::is_binary_t<mmodel,gamete_type &, mlist_type *>::value &&
     			      !(KTfwd::traits::is_unary_t<mmodel,gamete_type &>::value || KTfwd::traits::is_unary_t<mmodel,mlist_type *>::value)),
			     typename std::result_of<mmodel(gamete_type&,mlist_type*)>::type >::type
    mmodel_dispatcher( const mmodel & m, gamete_type & g, mlist_type * mutations)
    {
      return m(g,mutations);
    }
    
    
    /*!
      Apply mutation model N times to a new gamete.
      Updates mutation list
    */
    template<typename mutation_model,
    	     typename mutation_insertion_policy,
    	     typename mlist_type,
    	     typename gamete_type>
    void add_N_mutations( const mutation_model & mmodel,
    			  const mutation_insertion_policy & mpolicy,
    			  const unsigned & n,
    			  mlist_type * mutations,
    			  gamete_type & g)
    {
      for( unsigned i = 0 ; i < n ; ++i )
    	{
	  auto m = mmodel_dispatcher(mmodel,g,mutations);
    	  auto mitr = mpolicy(std::move(m),mutations);
    	  add_new_mutation(mitr,g);
    	}
    }
  }
}

#endif
