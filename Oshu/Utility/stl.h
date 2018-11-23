#pragma once

// Modify from https://stackoverflow.com/questions/2333728/stdmap-default-value
template <template<class, class, class...> class C, typename K, typename V, typename Ki, typename Vi, typename... Args>
V GetWithDef(const C<K, V, Args...>& m, Ki const& key, const Vi & defval) {
	typename C<K, V, Args...>::const_iterator it = m.find((K)key);
	if (it == m.end())
		return (V)defval;
	return it->second;
}
