/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include <iostream>
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
		// return 0;
		return seed_now = seed_now * seed_a + seed_b;
	}
	typedef pair<const Key, T> value_type;
private:
	Compare cmp;
	size_t siz;
	struct node {
		node *ch[2], *father;
		value_type value;
		unsigned w;
		node(const Key &k, const T &d) : value(value_type(k, d)), father(nullptr){
			w = randU();
			ch[0] = ch[1] = NULL;
		}
		node *next(int t) {
			if (ch[t]) {
				node *p = ch[t];
				while (p -> ch[t ^ 1]) p = p -> ch[t ^ 1];
				return p;
			}
			else {
				node *p = this;
				while (p -> father && p == p -> father -> ch[t]) p = p -> father;
				return p -> father;
			}
		}
	};

	node *root;

	void rotate(node *&x, int d) {
		node *tmp = x -> ch[d ^ 1];
		x -> ch[d ^ 1] = x -> ch[d ^ 1] -> ch[d];
		if (x -> ch[d ^ 1]) x -> ch[d ^ 1] -> father = x;
		tmp -> ch[d] = x;
		// if (x -> father) x -> father -> ch[x -> father -> ch[0] == x ? 0 : 1] = tmp;
		tmp -> father = x -> father;
		x -> father = tmp;
		x = tmp;
	}

	pair<node *, bool> insert(node *&x, const Key &k, const T &d) {
		if (x == NULL) {
			x = new node(k, d);
			return pair<node *, bool>(x, true);
		}
		node *tmp1 = NULL;
		bool tmp2 = true;
		if (cmp(k, x -> value.first)) {
			pair<node *, bool> tmp = insert(x -> ch[0], k, d);
			x -> ch[0] -> father = x;
			tmp1 = tmp.first;
			tmp2 = tmp.second;
			if (tmp.second && x -> w < x -> ch[0] -> w) {
				rotate(x, 1);
			}
		}
		else if (cmp(x -> value.first, k)) {
			pair<node *, bool> tmp = insert(x -> ch[1], k, d);
			x -> ch[1] -> father = x;
			tmp1 = tmp.first;
			tmp2 = tmp.second;
			if (tmp.second && x -> w < x -> ch[1] -> w) {
				rotate(x, 0);
			}
		}
		else {
			return pair<node *, bool>(x, false);
		}
		return pair<node *, bool>(tmp1, tmp2);
	}

	node *query(node * const &x, const Key &k) const{
		if (x == NULL) {
			throw index_out_of_bound();
		}
		if (cmp(k, x -> value.first)) {
			return query(x -> ch[0], k);
		}
		else if (cmp(x -> value.first, k)) {
			return query(x -> ch[1], k);
		}
		else {
			return x;
		}
	}

	pair<node *, bool> locate(node *&x, const Key &k) {
		if (x == NULL) {
			x = new node(k, T());
			return pair<node *, bool>(x, true);
		}
		node *tmp1 = NULL;
		bool tmp2 = true;
		if (cmp(k, x -> value.first)) {
			pair<node *, bool> tmp = locate(x -> ch[0], k);
			x -> ch[0] -> father = x;
			tmp1 = tmp.first;
			tmp2 = tmp.second;
			if (tmp.second) {
				if (x -> w < x -> ch[0] -> w) {
					rotate(x, 1);
				}
			}
		}
		else if (cmp(x -> value.first, k)) {
			pair<node *, bool> tmp = locate(x -> ch[1], k);
			x -> ch[1] -> father = x;
			tmp1 = tmp.first;
			tmp2 = tmp.second;
			if (tmp.second) {
				if (x -> w < x -> ch[1] -> w) {
					rotate(x, 0);
				}
			}
		}
		else {
			return pair<node *, bool>(x, false);
		}
		return pair<node *, bool>(tmp1, tmp2);
	}


	void del(node *&x) {
		if(x == NULL) return;
		del(x -> ch[0]);
		del(x -> ch[1]);
		delete x;
		x = NULL;
	}

public:
	class iterator;
	class const_iterator;

private:
	node *last() const {
		node *p = root;
		if (p == NULL)  return p;
		while(p -> ch[1]) p = p -> ch[1];
		return p;
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
	class iterator {
		friend class const_iterator;
		friend void map::erase(iterator);
	private:
		node *pos;
		const map<Key, T, Compare> *from;
		/**
		 * TODO add data members
		 *   just add whatever you want.
		 */
	public:
		iterator() {
			// TODO
		}

		iterator(node *_pos, const map<Key, T, Compare> *_from) : pos(_pos), from(_from){}

		iterator(const iterator &other) = default;
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, just return the answer.
		 * as well as operator-
		 */
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
			if (pos == NULL) throw invalid_iterator();
			node *op = pos;
			pos = pos -> next(1);
			return iterator(op, from);
		}
		/**
		 * TODO ++iter
		 */
		iterator & operator++() {
			if (pos == NULL) throw invalid_iterator();
			pos = pos -> next(1);
			return (*this);
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
			node *op = pos;
			pos = pos ? pos -> next(0) : from -> last();
			if (pos == NULL) throw invalid_iterator();
			return iterator(op, from);
		}
		/**
		 * TODO --iter
		 */
		iterator & operator--() {
			pos = pos ? pos -> next(0) : from -> last();
			if (pos == NULL) throw invalid_iterator();
			return (*this);
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		value_type & operator*() const { //why can this function be const
			return pos -> value;
		}
		bool operator==(const iterator &rhs) const {
			return pos == rhs.pos && from == rhs.from;
		}
		bool operator==(const const_iterator &rhs) const {
			return pos == rhs.pos && from == rhs.from;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return pos != rhs.pos || from != rhs.from;
		}
		bool operator!=(const const_iterator &rhs) const {
			return pos != rhs.pos || from != rhs.from;
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
		friend class iterator;
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
	private:
			node *pos;
			const map<Key, T, Compare> *from;
		public:
			const_iterator() {
				// TODO
			}
			const_iterator(node *_pos, const map<Key, T, Compare> *_from) : pos(_pos), from(_from){}
			const_iterator(const const_iterator &other) = default;
			const_iterator(const iterator &other) : pos(other.pos), from(other.from) {
				// TODO
			}
			const_iterator operator++(int) {
				if (pos == NULL) throw invalid_iterator();
				node *op = pos;
				pos = pos -> next(1);
				return const_iterator(op, from);
			}
			/**
			 * TODO ++iter
			 */
			const_iterator & operator++() {
				if (pos == NULL) throw invalid_iterator();
				pos = pos -> next(1);
				return (*this);
			}
			/**
			 * TODO iter--
			 */
			const_iterator operator--(int) {
				node *op = pos;
				pos = pos ? pos -> next(0) : from -> last();
				if (pos == NULL) throw invalid_iterator();
				return const_iterator(op, from);
			}
			/**
			 * TODO --iter
			 */
			const_iterator & operator--() {
				pos = pos ? pos -> next(0) : from -> last();
				if (pos == NULL) throw invalid_iterator();
				return (*this);
			}
			/**
			 * a operator to check whether two iterators are same (pointing to the same memory).
			 */
			const value_type & operator*() const {
				return pos -> value;
			}
			bool operator==(const iterator &rhs) const {
				return pos == rhs.pos && from == rhs.from;
			}
			bool operator==(const const_iterator &rhs) const {
				return pos == rhs.pos && from == rhs.from;
			}
			/**
			 * some other operator for iterator.
			 */
			bool operator!=(const iterator &rhs) const {
				return pos != rhs.pos || from != rhs.from;
			}
			bool operator!=(const const_iterator &rhs) const {
				return pos != rhs.pos || from != rhs.from;
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
		if (p == NULL)  return iterator(p, this);
		while(p -> ch[0]) p = p -> ch[0];
		return iterator(p, this);
	}
	const_iterator cbegin() const {
		node *p = root;
		if (p == NULL)  return const_iterator(p, this);
		while(p -> ch[0]) p = p -> ch[0];
		return const_iterator(p, this);
	}
	/**
	 * return a iterator to the end
	 * in fact, it returns past-the-end.
	 */
	iterator end() {
		return iterator(NULL, this);
	}
	const_iterator cend() const {
		return const_iterator(NULL, this);
	}

	/**
	 * TODO two constructors
	 */
	void copy(const map &o) {
		if (this == &o) return;
		del(root);
		for (const_iterator i = o.cbegin(); i != o.cend(); ++i) {
			insert(root, i -> first, i -> second);
		}
	}
	map() : root(NULL), siz(0) {}
	map(const map &other) : root(NULL), siz(other.siz) {
		copy(other);
	}
	/**
	 * TODO assignment operator
	 */
	map & operator=(const map &other) {
		siz = other.siz;
		copy(other);
		return (*this);
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
		pair<node *, bool> &&tmp = locate(root, key);
		siz += tmp.second ? 1 : 0;
		return tmp.first -> value.second;
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
		return siz;
	}
	/**
	 * clears the contents
	 */
	void clear() {
		siz = 0;
		del(root);
	}
	/**
	 * insert an element.
	 * return a pair, the first of the pair is
	 *   the iterator to the new element (or the element that prevented the insertion),
	 *   the second one is true if insert successfully, or false.
	 */
	pair<iterator, bool> insert(const value_type &value) {
		pair<node *, bool> &&tmp = insert(root, value.first, value.second);
		siz += tmp.second ? 1 : 0;
		return pair<iterator, bool>(iterator(tmp.first, this), tmp.second);
	}
	/**
	 * erase the element at pos.
	 *
	 * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
	 */

	void find_erase(node *&x, const Key &key) {
		if (cmp(key, x -> value.first)) {
			find_erase(x -> ch[0], key);
		}
		else if (cmp(x -> value.first, key)) {
			find_erase(x -> ch[1], key);
		}
		else {
			if (x -> ch[0] == NULL) {
				node *tmp = x;
				if (x -> ch[1]) x -> ch[1] -> father = x -> father;
				x = x -> ch[1];
				delete tmp;
			}
			else if (x -> ch[1] == NULL) {
				node *tmp = x;
			    x -> ch[0] -> father = x -> father;
				x = x -> ch[0];
				delete tmp;
			}
			else {
				if (x -> ch[0] -> w > x -> ch[1] -> w) {
					rotate(x, 1);
					find_erase(x -> ch[1], key);
				}
				else {
					rotate(x, 0);
					find_erase(x -> ch[0], key);
				}
			}
		}
	}
	void erase(iterator pos) {
		if (pos.pos == NULL || pos.from != this) throw invalid_iterator();
		--siz;
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
			return iterator(query(root, key), this);
		}
		catch(...) {
			return end();
		}
	}
	const_iterator find(const Key &key) const {
		try {
			return const_iterator(query(root, key), this);
		}
		catch(...) {
			return cend();
		}
	}
};
template<class Key, class T, class Compare>
unsigned map<Key, T, Compare>::seed_a = 134775813;
template<class Key, class T, class Compare>
unsigned map<Key, T, Compare>::seed_b = 1;
template<class Key, class T, class Compare>
unsigned map<Key, T, Compare>::seed_now = 1;

}

#endif
