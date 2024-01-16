// Copyright 2024 Mark Solinski
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// buckets.h : Define the bucket collection class.
//

#ifndef MASUTILS_BUCKETS_H_
#define MASUTILS_BUCKETS_H_

#include <functional>
#include <list>
#include "triplet.h"

namespace masutils {

template<class _E>
struct bucket_traits {

    typedef _E index_type;

    static bool eq(const _E& _X, const _E& _Y) { return (_X == _Y); }
    static bool lt(const _E& _X, const _E& _Y) { return (_X <  _Y); }
    static void assign  (_E& _X, const _E& _Y) {         _X =  _Y;  }
protected:
	~bucket_traits() {}
};

template<class _E>
struct bucket_traits_descending : public bucket_traits<_E> {

    static bool lt(const _E& _X, const _E& _Y) { return (_Y < _X); }
protected:
	~bucket_traits_descending() {}
};

template<class _E, class _C = std::list<_E> >
struct bucket_value_traits {

    typedef _E value_type;
    typedef _C value_container;

    static void add   (value_container& _X,  const value_type& _Y)
                        { _X.push_back(_Y); }
    static void append(value_container& _X,  const value_container& _Y)
                        { _X.insert(_X.end(), _Y.begin(), _Y.end()); }
protected:
	~bucket_value_traits() {}
};

template <class indices,
          class values,
          class traits = bucket_traits<indices>,
          class container = bucket_value_traits<values> >
class buckets
{
public:
    typedef buckets<indices,
                    values,
                    traits,
                    container> mytype;

    typedef indices            index_type;
    typedef values             value_type;

    typedef       typename container::value_container value_container;
    typedef const typename container::value_container const_value_container;
    typedef triplet<index_type,
                    index_type,
                    value_container>                  triplet_type;
    typedef std::list<triplet_type>                   triplet_list;

    typedef typename triplet_list::iterator               iterator;
    typedef typename triplet_list::const_iterator         const_iterator;
    typedef typename triplet_list::reverse_iterator       reverse_iterator;
    typedef typename triplet_list::const_reverse_iterator const_reverse_iterator;

    iterator begin()                { return _buckets.begin(); }
    iterator end()                  { return _buckets.end(); }
    const_iterator begin() const    { return const_iterator(_buckets.begin()); }
    const_iterator end() const      { return const_iterator(_buckets.end()); }

    reverse_iterator rbegin()       { return _buckets.rbegin(); }
    reverse_iterator rend()         { return _buckets.rend(); }
    const_reverse_iterator rbegin() const
                                    { return const_reverse_iterator(_buckets.rbegin()); }
    const_reverse_iterator rend() const
                                    { return const_reverse_iterator(_buckets.rend()); }

    int size() const                { return _buckets.size(); }

private:
    buckets(const mytype&);
    mytype& operator=(const mytype&);

    triplet_list    _buckets;
    index_type      _low;
    index_type      _high;
    bool            _constrained;

public:
    explicit buckets(index_type low, index_type high) : _low(low), _high(high), _constrained(true)
    {
        if (traits::lt(_high, _low))
            throw std::invalid_argument("Arguments not in correct order.");
    }

    explicit buckets() : _low(0), _high(0), _constrained(false) {}

    ~buckets() {}

    bool splice(index_type low, index_type high, iterator& begin, iterator& end)
    {
        index_type _l, _h;
        traits::assign(_l, low);
        traits::assign(_h, high);

        if (_constrained)
        {
            if (traits::lt(_h, _low) || traits::lt(_high, _l))
              return false;

            // They now must overlap somewhere and their range can now be
            // constricted to the bounds
            if (traits::lt(_l,  _low)) traits::assign(_l, _low);
            if (traits::lt(_high, _h)) traits::assign(_h, _high);
        }

        index_type _lowest, _highest;
        traits::assign(_lowest, _l);
        traits::assign(_highest, _h);

        // check for overlaps to slice buckets
        for (iterator p = _buckets.begin(); p != _buckets.end(); ++p)
        {
            if (traits::lt(_l, _h) != true)
                break; // all done since range is null

            triplet_type& triplet = *p;

            // step 1: first isolate the part of the new range which occurs
            // before the current bucket
            if (traits::lt(_l, triplet.first))
            {
                value_container _container;
                if (traits::lt(triplet.first, _h)) // overlap
                {
                    triplet_type _triplet(_l, triplet.first, _container);
                    _buckets.insert(p, _triplet);
                    traits::assign(_l, triplet.first);
                }
                else // no overlap
                {
                    triplet_type _triplet(_l, _h, _container);
                    _buckets.insert(p, _triplet);
                    traits::assign(_l, triplet.first);
                    continue; // it all ends before the current bucket
                              // so we're done
                }
            }

            // step 2: now isolate the part remaining which starts at
            // the current bucket but ends before the the end of the
            // current bucket
            if (traits::eq(_l, triplet.first))
            {
                if (traits::lt(_h, triplet.second))
                {
                    triplet_type _triplet(triplet);
                    traits::assign(_triplet.second, _h);
                    _buckets.insert(p, _triplet);
                    traits::assign(triplet.first, _h);
                    traits::assign(_l, _h);
                    continue;
                }
                else
                {   // already have a bucket from _l to triplet.second
                    // (the current bucket) so adjust _l
                    traits::assign(_l, triplet.second);
                }
            }

            // step 3: now isolate the part remaining which starts after
            // the current bucket and create the two buckets which split
            // the current bucket with _l
            if (traits::lt(_l, triplet.second))
            {
                {   // first create the "start of current bucket to
                    // _l" bucket
                    triplet_type _triplet(triplet);
                    traits::assign(_triplet.second, _l);
                    _buckets.insert(p, _triplet);
                    // next change current bucket to be "_l to end of
                    // current bucket"
                    traits::assign(triplet.first, _l);
                }

                // step 4: now isolate the part remaining which starts at
                // _l, the new start of the current bucket and check if _h
                // is less than the end of the current bucket
                if (traits::lt(_h, triplet.second))
                {
                    triplet_type _triplet(triplet);
                    traits::assign(_triplet.second, _h);
                    _buckets.insert(p, _triplet);
                    traits::assign(triplet.first, _h);
                }

                // already have a bucket from _l to triplet.second, adjust
                // _l to after the current bucket -- triplet.second
                traits::assign(_l, triplet.second);
            }

            // at most, only _l (which should be after current bucket) to
            // _h is remaining, so loop
        }

        if (traits::lt(_l, _h)) // either first bucket or after last bucket
        {
            value_container _container;
            triplet_type _triplet(_l, _h, _container);
            _buckets.push_back(_triplet);
        }

        bool bBegin = false, bEnd = false;

        {
            for (iterator p = _buckets.begin(); p != _buckets.end(); ++p)
            {
                triplet_type& triplet = *p;
                if (traits::eq(_lowest, triplet.first))
                {
                    begin = p;
                    bBegin = true;
                }
                if (traits::eq(_highest, triplet.second))
                {
                    end = p;
                    ++end;
                    bEnd = true;
                    break;
                }
            }
        }

        return (bBegin && bEnd);
    }

    int spread(const triplet_type& _triplet)
    {
        int added_to_bucket = 0;

        iterator begin, end;
        bool bSpliced = splice(_triplet.first, _triplet.second, begin, end);

        if (!bSpliced)
            return added_to_bucket;

        index_type _l, _h;
        traits::assign(_l, _triplet.first);
        traits::assign(_h, _triplet.second);

        if (_constrained)
        {
            // The call to splice above catches the condition below so it
            // is not needed here

            //if (traits::lt(_h, _low) || traits::lt(_high, _l))
            //  return false;

            // They now must overlap somewhere and their range can now be
            // constricted to the bounds
            if (traits::lt(_l,  _low)) traits::assign(_l, _low);
            if (traits::lt(_high, _h)) traits::assign(_h, _high);
        }

        for (iterator p = begin; p != end; ++p)
        {
            triplet_type& triplet = *p;
            if (traits::lt(triplet.second, _l)) continue; // not yet...
            if (traits::lt(_h,  triplet.first)) break;    // already done...
            value_container& _ocontainer = triplet.third;
            const value_container& _icontainer = _triplet.third;
            container::append(_ocontainer, _icontainer);
            added_to_bucket++;
        }

        return added_to_bucket;
    }

    int cover(const triplet_type& _triplet)
    {
        int added_to_bucket = 0;

        iterator begin, end;
        bool bSpliced = splice(_triplet.first, _triplet.second, begin, end);

        if (!bSpliced)
            return added_to_bucket;

        index_type _l, _h;
        traits::assign(_l, _triplet.first);
        traits::assign(_h, _triplet.second);

        if (_constrained)
        {
            // The call to splice above catches the condition below so it
            // is not needed here

            //if (traits::lt(_h, _low) || traits::lt(_high, _l))
            //  return false;

            // They now must overlap somewhere and their range can now be
            // constricted to the bounds
            if (traits::lt(_l,  _low)) traits::assign(_l, _low);
            if (traits::lt(_high, _h)) traits::assign(_h, _high);
        }

        iterator next = _buckets.erase(begin, end);

        triplet_type _triplet2(_l, _h, _triplet.third);

        _buckets.insert(next, _triplet2);

        added_to_bucket++;

        return added_to_bucket;
    }

    int spread(index_type low, index_type high, value_type value)
    {
        value_container _container;
        container::add(_container, value);
        triplet_type _triplet(low, high, _container);

        return spread(_triplet);
    }

    int cover(index_type low, index_type high, value_type value)
    {
        value_container _container;
        container::add(_container, value);
        triplet_type _triplet(low, high, _container);

        return cover(_triplet);
    }

    int spread(const buckets& aBucket)
    {
        int added_to_bucket = 0;

        for (iterator p = aBucket.begin(); p != aBucket.end(); ++p)
        {
            triplet_type& triplet = *p;
            added_to_bucket += spread(triplet.first,
                                      triplet.second,
                                      triplet.third);
        }

        return added_to_bucket;
    }

    int cover(const buckets& aBucket)
    {
        int added_to_bucket = 0;

        for (iterator p = aBucket.begin(); p != aBucket.end(); ++p)
        {
            triplet_type& triplet = *p;
            added_to_bucket += cover(triplet.first,
                                     triplet.second,
                                     triplet.third);
        }

        return added_to_bucket;
    }
};

}

#endif // MASUTILS_BUCKETS_H_
