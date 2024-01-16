
#ifndef MASUTILS_TRIPLET_H_
#define MASUTILS_TRIPLET_H_

namespace masutils {

template <typename _T1, typename _T2, typename _T3>
struct triplet
{
	typedef _T1 first_type;
	typedef _T2 second_type;
    typedef _T3 third_type;
	triplet()
	: first (_T1()), 
      second(_T2()),
      third (_T3())
    {}
	triplet( const _T1& _V1, const _T2& _V2, const _T3& _V3)
	: first (_V1), 
      second(_V2),
      third (_V3)
    {}

	~triplet() {}

    _T1  first;
    _T2  second;
    _T3  third;
};

template <typename _T1, typename _T2, typename _T3>
triplet<_T1, _T2, _T3> make_triplet( const _T1& _V1, const _T2& _V2, const _T3& _V3 )
{ return triplet<_T1, _T2, _T3>( _V1, _V2, _V3 ); }

}

#endif // MASUTILS_TRIPLET_H_

