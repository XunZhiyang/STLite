/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
	class Key,
	class T,
	class Compare = std::less<Key>
> class map {
public:
	static unsigned seed_a, seed_b, seed_now;
public:
	static unsigned randU() {
		return seed_now = seed_now * seed_a + seed_b;
	}
private:
	typedef pair<const Key, T> value_type;
	class node {
	public:
		node *ch[2], *next[2];
		value_type value;
		size_t siz;
		unsigned w;
		// node() : data(T()), siz(1) {
		// 	// data = T();
		// 	w = randU();
		// 	ch[0] = ch[1] = NULL;
		// }
		node(const Key &k, const T &d) : value(value_type(k, d)), siz(1) {
			w = randU();
			ch[0] = ch[1] = NULL;
		}
		// node(const node &o) : value.first(o.value.first), value.second(o.value.second), siz(o.siz), w(o.w) {
		// 	for (int i = 0; i < 2; ++i) {
		// 		next[i] = o.next[i];
		// 		if
		// 	}
		// }
	};

	node *root;

	void update(node *&x) {
		x -> siz = 1;
		x -> siz += x -> ch[0] ? x -> ch[0] -> siz : 0;
		x -> siz += x -> ch[1] ? x -> ch[1] -> siz : 0;
	}

	void rotate(node *&x, int d) {
		node *tmp = x -> ch[d ^ 1];
		x -> ch[d ^ 1] = x ->ch[d ^ 1] -> ch[d];
		tmp -> ch[d] = x;
		update(x);
		x = tmp;
		update(x);
	}

	pair<node *, bool> insert(node *&x, const Key &k, const T &t) {
		bool flag = true;
		if (x == NULL) {
			x = new node(k, t);
			return value_type(x, true);
		}
		pair<node *, bool> tmp = value_type(NULL, true);
		if (Compare()(k, x -> value.first)) {
			// node *p = x -> ch[0];
			tmp = insert(x -> ch[0], k);
			if (tmp.second) {
				update(x);
				if (tmp.first == x -> ch[0]) {
					x -> ch[0] -> next[0] = x -> next[0];
					if (x -> next[0]) x -> next[0] -> next[1] = x -> ch[0];
					x -> ch[0] -> next[1] = x;
					x -> next[0] = x -> ch[0];
				}
				if (flag && Compare()(x -> w, x -> ch[0] -> w)) {
					rotate(x, 1);
				}
			}
		}
		else if (Compare()(x -> value.first, k)) {
			// node *p = x -> ch[1];
			tmp = insert(x -> ch[1], k);
			if (tmp.second) {
				update(x);
				if (tmp.first == x -> ch[1]) {
					x -> ch[1] -> next[1] = x -> next[1];
					if (x -> next[1]) x -> next[1] -> next[0] = x -> ch[1];
					x -> ch[1] -> next[0] = x;
					x -> next[1] = x -> ch[1];
				}
				if (flag && Compare() (x -> w, x -> ch[1] -> w)) {
					rotate(x, 0);
				}
			}
		}
		else {
			return value_type(x, false);
		}
		return tmp;
	}

	node *query(node *&x, const Key &k) {
		if (x == NULL) {
			throw index_out_of_bound();
		}
		if (Compare()(k, x -> value.first)) {
			return query(x -> ch[0], k);
		}
		else if (Compare()(x -> value.first, k)) {
			return query(x -> ch[1], k);
		}
		else {
			return x;
		}
	}
	//
	// const T &query(node *&x, const Key &k) {
	// 	if (x == NULL) {
	// 		throw index_out_of_bound();
	// 	}
	// 	if (Compare()(k, x -> value.first)) {
	// 		return query(x -> ch[0], k);
	// 	}
	// 	else if (Compare()(x -> value.first, k)) {
	// 		return query(x -> ch[1], k);
	// 	}
	// 	else {
	// 		return x -> value.second;
	// 	}
	// }
	void del(node *&x) {
		if(x == NULL) return;
		del(x -> ch[0]);
		del(x -> ch[1]);
		delete x;
		x = NULL;
	}

public:
	/**
	 * the internal type of data.
	 * it should have a default constructor, a copy constructor.
	 * You can use sjtu::map as value_type by typedef.
	 */
	/**
	 * see BidirectionalIterator at CppReference for help.
	 *
	 * if there is anything wrong throw invalid_iterator.
	 *     like it = map.begin(); --it;
	 *       or it = map.end(); ++end();
	 */
	class const_iterator;
	class iterator {
	private:
		node *pos;
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
	public:
		iterator() {
			// TODO
		}

		iterator(node *_pos) : pos(_pos) {}

		iterator(const iterator &other) : pos(other.pos) {
			// TODO
		}
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
			node *op = pos;
			pos = pos -> next[1];
			return op;
		}
		/**
		 * TODO ++iter
		 */
		iterator & operator++() {
			return pos = pos -> next[1];
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
			node *op = pos;
			pos = pos -> next[0];
			return op;
		}
		/**
		 * TODO --iter
		 */
		iterator & operator--() {
			return pos = pos -> next[0];
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const { //why can this function be const
			return value_type(pos -> value.first, pos -> value.second);
		}
		bool operator==(const iterator &rhs) const {
			return pos == rhs.pos;
		}
		bool operator==(const const_iterator &rhs) const {
			return pos == rhs.pos;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return pos != rhs.pos;
		}
		bool operator!=(const const_iterator &rhs) const {
			return pos != rhs.pos;
		}

		/**
		 * for the support of it->first.
		 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
		 */
		value_type* operator->() const noexcept {
			return &(pos -> value);
		}
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
			node *pos;
		public:
			const_iterator() {
				// TODO
			}
			const_iterator(node *_pos) : pos(_pos) {}
			const_iterator(const const_iterator &other) : pos(other.pos) {
				// TODO
			}
			const_iterator(const iterator &other) : pos(other.pos) {
				// TODO
			}
			const_iterator operator++(int) {
				node *op = pos;
				pos = pos -> next[1];
				return op;
			}
			/**
			 * TODO ++iter
			 */
			const_iterator & operator++() {
				return pos = pos -> next[1];
			}
			/**
			 * TODO iter--
			 */
			const_iterator operator--(int) {
				node *op = pos;
				pos = pos -> next[0];
				return op;
			}
			/**
			 * TODO --iter
			 */
			const_iterator & operator--() {
				return pos = pos -> next[0];
			}
			/**
			 * a operator to check whether two iterators are same (pointing to the same memory).
			 */
			const value_type & operator*() const {
				return value_type(pos -> value.first, pos -> value.second);
			}
			bool operator==(const iterator &rhs) const {
				return pos == rhs.pos;
			}
			bool operator==(const const_iterator &rhs) const {
				return pos == rhs.pos;
			}
			/**
			 * some other operator for iterator.
			 */
			bool operator!=(const iterator &rhs) const {
				return pos != rhs.pos;
			}
			bool operator!=(const const_iterator &rhs) const {
				return pos != rhs.pos;
			}

			/**
			 * for the support of it->first.
			 * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
			 */
			const value_type* operator->() const noexcept {
				return &(pos -> value);
			}
	};

	/**
	 * return a iterator to the beginning
	 */
	iterator begin() {
		node *p = root;
		if (p == NULL)  return iterator(p);
		while(p -> ch[0]) p = p -> ch[0];
		return iterator(p);
	}
	const_iterator cbegin() const {
		node *p = root;
		if (p == NULL)  return const_iterator(p);
		while(p -> ch[0]) p = p -> ch[0];
		return const_iterator(p);
	}
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() {
		return iterator(NULL);
	}
	const_iterator cend() const {
		return const_iterator(NULL);
	}

	/**
	 * TODO two constructors
	 */
	void copy(const map &o) {
		if (this == &o) return;
		del(root);
		for (auto i = o.cbegin(); i != o.cend(); ++i) {
			insert(root, o -> first, o ->second);
		}
	}
	map() : root(NULL) {}
	map(const map &other) {
		other.copy(other);
	}
	/**
	 * TODO assignment operator
	 */
	map & operator=(const map &other) {
		other.copy(other);
	}
	/**
	 * TODO Destructors
	 */
	~map() {
		del(root);
	}
	/**
	 * TODO
	 * access specified element with bounds checking
	 * Returns a reference to the mapped value of the element with key equivalent to key.
	 * If no such element exists, an exception of type `index_out_of_bound'
	 */
	T & at(const Key &key) {
		return query(root, key) -> value.second;
	}
	const T & at(const Key &key) const {
		return query(root, key) -> value.second;
	}
	/**
	 * TODO
	 * access specified element
	 * Returns a reference to the value that is mapped to a key equivalent to key,
	 *   performing an insertion if such key does not already exist.
	 */
	T & operator[](const Key &key) {
		return at(key);
	}
	/**
	 * behave like at() throw index_out_of_bound if such key does not exist.
	 */
	const T & operator[](const Key &key) const {
		return at(key);
	}

	/**
	 * checks whether the container is empty
	 * return true if empty, otherwise false.
	 */
	bool empty() const {
		return root == NULL;
	}
	/**
	 * returns the number of elements.
	 */
	size_t size() const {
		return root == NULL ? 0 : root -> siz;
	}
	/**
	 * clears the contents
	 */
	void clear() {
		del(root);
	}
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion),
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {
		insert(root, value.first, value.second);
	}
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */
	void find_erase(node *&x, const Key &key) {
		if (Compare()(key, x -> value.first)) {
			find_erase(x -> ch[0], key);
			update(x);
		}
		else if (Compare()(x -> value.first, key)) {
			find_erase(x -> ch[1], key);
			update(x);
		}
		else {
			if (x -> ch[0] == NULL) {
				node *tmp = x;
				x = x -> ch[1];
				delete tmp;
			}
			else if (x -> ch[1] == NULL) {
				node *tmp = x;
				x = x -> ch[0];
				delete tmp;
			}
			else {
				if (x -> ch[0] -> w > x -> ch[1] -> w) {
					rotate(x, 1);
					del(x -> ch[1], key);
				}
				else {
					rotate(x, 0);
					del(x -> ch[0], key);
				}
			}
		}
	}
	void erase(iterator pos) {
		try {
			pos.pos -> ch[0];
		}
		catch(...) {
			throw invalid_iterator();
		}
		// node *p = pos.pos;
		find_erase(root, pos -> first);
	}
	/**
	 * Returns the number of elements with key
	 *   that compares equivalent to the specified argument,
	 *   which is either 1 or 0
	 *     since this container does not allow duplicates.
	 * The default method of check the equivalence is !(a < b || b > a)
	 */
	size_t count(const Key &key) const {
		try {
			query(root, key);
		}
		catch(...) {
			return 0;
		}
		return 1;
	}
	/**
	 * Finds an element with key equivalent to key.
	 * key value of the element to search for.
	 * Iterator to an element with key equivalent to key.
	 *   If no such element is found, past-the-end (see end()) iterator is returned.
	 */
	iterator find(const Key &key) {
		try {
			return iterator(query(root, key));
		}
		catch(...) {
			return end();
		}
	}
	const_iterator find(const Key &key) const {
		try {
			return const_iterator(query(root, key));
		}
		catch(...) {
			return cend();
		}
	}
};
template<class Key, class T, class Compare>
unsigned map<Key, T, Compare>::seed_a = 69069;
template<class Key, class T, class Compare>
unsigned map<Key, T, Compare>::seed_b = 1;
template<class Key, class T, class Compare>
unsigned map<Key, T, Compare>::seed_now = 1;

}

#endif