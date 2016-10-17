#pragma once

namespace algs
{
	// Splits string s at delimiter placing resultant strings into output iterator dest
	template<typename InputIterator, typename OutputIterator>
	void Split( InputIterator begin
			  , InputIterator end
			  , typename std::iterator_traits<InputIterator>::value_type delimiter
			  , OutputIterator dest
			  , bool compress=true
			  )
	{
		typedef typename std::iterator_traits<InputIterator>::value_type value_type;
		typedef typename std::iterator_traits<OutputIterator>::value_type out_type;

		InputIterator buffer_start = begin;
		InputIterator buffer_end = begin;
		for (InputIterator i = begin; i != end; ++i)
		{
			if (*i != delimiter)
			{
				++buffer_end;
			}
			else if (buffer_start != buffer_end || !compress)
			{
				*dest = out_type(buffer_start, buffer_end);
				++dest;
				buffer_start = i;
				buffer_end = i;
			}
		}

		if (buffer_start != buffer_end || !compress)
		{
			*dest = out_type(buffer_start, buffer_end);
		}
	};

	// faster than remove_if at the cost of not preserving order
	template <typename ForwardItor, typename Pred>
	ForwardItor unstable_remove_if(ForwardItor first, ForwardItor last, Pred pred)
	{
	  while(first != last)
	  {
		if(pred(*first))
		  swap(*first, *--last);
		else
		  ++first;
	  }
	  return last;
	}

	// faster than remove at the cost of not preserving order
	template <typename ForwardItor, typename T>
	ForwardItor unstable_remove(ForwardItor first, ForwardItor last, const T &value)
	{
	  return unstable_remove_if(first, last, std::bind2nd(std::equal_to<T>(), value));
	}

};
