#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP

#include "exceptions.hpp"

#include <iostream>
#include <cstddef>

namespace sjtu {

template<class T>
class deque {
	friend class iterator;
	friend class const_iterator;
	const static int MAXSIZE = 100;
public:
	class iterator;
	class LLNode {
	public:
		T data;
		LLNode *next[2];
		LLNode(T _data, LLNode *pre, LLNode *nex) : data(_data), next{pre, nex} {}
		LLNode(const LLNode &other) : data(other.data), next{nullptr, nullptr} {}
		LLNode(const LLNode &other, LLNode *prev) : data(other.data), next{prev, nullptr} {}
	};

	class Node {
	public:
		LLNode *head, *tail;
		size_t size;
		Node *next[2];
		Node() : head(nullptr), tail(nullptr), size(0), next{nullptr, nullptr} {}
		Node(LLNode *_head, LLNode *_tail, size_t _size, Node *n0, Node *n1) : head(_head), tail(_tail), size(_size), next{n0, n1} {}
		Node(const Node &other) : head(nullptr), tail(nullptr), size(other.size), next{nullptr, nullptr} {
			if (other.head != nullptr) {
				head = new LLNode(*other.head);
				LLNode *p = head;
				LLNode *q = other.head -> next[1];
				while(q) {
					p -> next[1] = new LLNode(*q, p);
					p = p -> next[1];
					q = q -> next[1];
				}
				tail = p;
			}
		}
		Node(const Node &other, Node *prev) : Node(other) {
			next[0] = prev;
		}
		~Node() {
			if (head) {
				LLNode *p = head -> next[1];
				while (p) {
					delete p -> next[0];
					p = p -> next[1];
				}
				delete tail;
			}
		}
		iterator push_back(const T &value) {
			LLNode *np = new LLNode(value, tail, nullptr);
			if (tail) tail -> next[1] = np;
			else head = np;
			tail = np;
			size++;
			return {np, size, this, nullptr};
		}
		Node *push_front(const T &value) {
			LLNode *np = new LLNode(value, nullptr, head);
			if (head) head -> next[0] = np;
			else tail = np;
			head = np;
			size++;
			return this;
		}
		Node *pop_front() {
			--size;
			LLNode *p = head -> next[1];
			if (p) {
				p -> next[0] = nullptr;
			}
			else tail = nullptr;
			delete head;
			head = p;
			return this;
		}
		Node* pop_back() {
			--size;
			LLNode *p = tail -> next[0];
			if (p) {
				p -> next[1] = nullptr;
			}
			else head = nullptr;
			delete tail;
			tail = p;
			return this;
		}
	};
private:
	size_t siz;
	Node *head, *tail;
	Node *merge(Node *p, Node *q) {
		static int c = 0;
		++c;
		p -> next[1] = q -> next[1];
		if (q -> next[1]) q -> next[1] -> next[0] = p;
		p -> size += q -> size;

		if (p -> tail) p -> tail -> next[1] = q -> head;
		else p -> head = q -> head;
		if (q -> head) {
			q -> head -> next[0] = p -> tail;
			p -> tail = q -> tail;
		}

		if (tail == q) tail = p;
		q -> head = q -> tail = nullptr;
		delete q;
		return p;

	}
	Node *split(Node *p, LLNode *node, int pos) { //split before "node"
		Node *np = new Node;
		if (p -> next[1]) p -> next[1] -> next[0] = np;
		np -> next[1] = p -> next[1];
		np -> next[0] = p;
		p -> next[1] = np;
		np ->size = p -> size - pos + 1;
		p -> size = pos - 1;

		np -> tail = p -> tail;
		p -> tail = node -> next[0];
		np -> head = node;
		if (p -> tail) p -> tail -> next[1] = nullptr;
		else p -> head = nullptr;
		node -> next[0] = nullptr;

		if (p == tail) tail = np;
		return p;
	}

	void copy(const deque &other) {    		//container is empty before call
		if (other.head != nullptr) {
			head = new Node(*other.head);
			Node *p = head;
			Node *q = other.head -> next[1];
			while(q) {
				p -> next[1] = new Node(*q, p);
				p = p -> next[1];
				q = q -> next[1];
			}
			tail = p;
		}
	}
	Node *arrange(Node *x) {
		while (x ->next[1] && x -> next[1] -> size + x -> size <= MAXSIZE)
			x = merge(x, x -> next[1]);
		return x;
	}
public:
	class const_iterator;
	class iterator {
		friend class deque;
	private:
		LLNode *now;
		size_t pos;
		Node *blo;
		deque<T> *ori;
	public:
		/**
		 * return a new iterator which pointer n-next elements
		 *   even if there are not enough elements, the behaviour is **undefined**.
		 * as well as operator-
		 */
		iterator(LLNode *_now, size_t _pos, Node *_blo, deque<T> *_ori) : now(_now), pos(_pos), blo(_blo), ori(_ori) {}
		iterator() = default;
		iterator(const iterator &other) = default;
		iterator operator-(const int&);
		iterator operator+(const int &n) const {
			if (n < 0) return operator-(-n);
			iterator np(*this);
			if (n + np.pos <= blo -> size) {
				np.pos += n;
				for (int i = 1; i <= n; ++i) {
					np.now = np.now -> next[1];
				}
			}
			else {
				int c = n - (blo -> size - pos + 1);
				np.blo = blo -> next[1];
				while (np.blo && np.blo -> size <= c) {
					c -= np.blo -> size;
					np.blo = np.blo -> next[1];
				}
				if (!np.blo) {
					return {nullptr, 0, nullptr, ori};
				}
				np.now = np.blo -> head;
				np.pos = c + 1;
				while (c--) {
					np.now = np.now -> next[1];
				}
			}
			return np;
		}
		iterator operator-(const int &n) const {   //end() hasn't been processed yet
			if (n < 0) return operator+(-n);
			iterator np(*this);
			int nn = n;
			if (np == ori -> end()) {
				--nn;
				np = ori -> last();
			}
			if (np.pos - nn) {
				np.pos -= nn;
				for (int i = 1; i <= nn; ++i) {
					np.now = np.now -> next[0];
				}
			}
			else {
				int c = nn - pos;
				np.blo = blo -> next[0];
				while (np.blo && np.blo -> size <= c) {
					c -= np.blo -> size;
					np.blo = np.blo -> next[0];
				}
				if (!np.blo) {
					return {nullptr, 0, nullptr, ori};
				}
				np.now = np.blo -> tail;
				np.pos = np.blo -> size - c;
				while (c--) {
					np.now = np.now -> next[0];
				}
			}
			return np;
		}
		// return th distance between two iterator,
		// if these two iterators points to different vectors, throw invaild_iterator.
		int operator-(const iterator &rhs) const {
			if (ori != rhs.ori) throw invalid_iterator();
			int ans = 0;
			for(Node *i = rhs.blo; i != blo; i = i -> next[1]) {
				if (i == nullptr) {
					ans = 0;
					break;
				}
				ans += i -> size;
			}
			if (ans >= 0) {
				return ans + pos - rhs.pos;
			}
			else {
				ans = 0;
				for(Node *i = blo; i != rhs.blo; i = i -> next[1]) {
					ans += i -> size;
				}
				return -(ans + rhs.pos - pos);
			}
		}
		iterator &operator+=(const int &n) {
			return (*this) = this -> operator+(n);
		}
		iterator &operator-=(const int &n) {
			return (*this) = this -> operator-(n);
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
			iterator tmp(*this);
			(*this) += 1;
			return tmp;
		}
		/**
		 * TODO ++iter
		 */
		iterator& operator++() {
			return (*this) += 1;
		}
		/**
		 * TODO iter--
		 */
		iterator operator--(int) {
			iterator tmp(*this);
			(*this) -= 1;
			return tmp;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() {
			return (*this) -= 1;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const {
			return now -> data;
		}
		/**
		 * TODO it->field
		 */
		T* operator->() const noexcept {
			return &(now -> data);
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
			return now == rhs.now;
		}
		bool operator==(const const_iterator &rhs) const {
			return now == rhs.now;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
			return now != rhs.now;
		}
		bool operator!=(const const_iterator &rhs) const {
			return now != rhs.now;
		}
	};
	class const_iterator {
		friend class deque;
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
		private:
			LLNode *now;
			size_t pos;
			Node *blo;
			deque<T> *ori;
		public:

   			const_iterator(LLNode *_now, size_t _pos, Node *_blo, deque<T> *_ori) : now(_now), pos(_pos), blo(_blo), ori(_ori) {}
			const_iterator() = default;
			const_iterator(const iterator &other) : now(other.now), pos(other.pos), blo(other.blo), ori(other.ori) {}
			const_iterator(const const_iterator &other) = default;
			const_iterator operator-(const int&);
			const_iterator operator+(const int &n) const {
				if (n < 0) return operator-(-n);
				const_iterator np(*this);
				if (n + np.pos <= blo -> size) {
					np.pos += n;
					for (int i = 1; i <= n; ++i) {
						np.now = np.now -> next[1];
					}
				}
				else {
					int c = n - (blo -> size - pos + 1);
					np.blo = blo -> next[1];
					while (np.blo -> size <= c) {
						c -= np.blo -> size;
						np.blo = np.blo -> next[1];
					}
					np.now = np.blo -> head;
					np.pos = c + 1;
					while (c--) {
						np.now = np.now -> next[1];
					}
				}
				return np;
			}
			const_iterator operator-(const int &n) const {
				if (n < 0) return operator+(-n);
				const_iterator np(*this);
				int nn = n;
				if (np == cend()) {
					--nn;
					np = clast();
				}
				if (np.pos - nn) {
					np.pos -= nn;
					for (int i = 1; i <= nn; ++i) {
						np.now = np.now -> next[0];
					}
				}
				else {
					int c = nn - pos;
					np.blo = blo -> next[0];
					while (np.blo -> size <= c) {
						c -= np -> blo -> size;
						np.blo = np.blo -> next[0];
					}
					np.now = np.blo -> tail;
					np.pos = np.blo -> size - c;
					while (c--) {
						np.now = np.now -> next[0];
					}
				}
				return np;
			}
			// return th distance between two const_iterator,
			// if these two iterators points to different vectors, throw invaild_iterator.
			int operator-(const const_iterator &rhs) const {
				if (ori != rhs.ori) throw invalid_iterator();
				int ans = 0;
				for(Node *i = rhs.blo; i != blo; i = i -> next[1]) {
					if (i == nullptr) {
						ans = 0;
						break;
					}
					ans += i -> size;
				}
				if (ans >= 0) {
					return ans + pos - rhs.pos;
				}
				else {
					ans = 0;
					for(Node *i = blo; i != rhs.blo; i = i -> next[1]) {
						ans += i -> size;
					}
					return -(ans + rhs.pos - pos);
				}
			}
			const_iterator &operator+=(const int &n) {
				return (*this) = this -> operator+(n);
			}
			const_iterator &operator-=(const int &n) {
				return (*this) = this -> operator-(n);
			}
			/**
			 * TODO iter++
			 */
			const_iterator operator++(int) {
				const_iterator tmp(*this);
				(*this) += 1;
				return tmp;
			}
			/**
			 * TODO ++iter
			 */
			const_iterator& operator++() {
				return (*this) += 1;
			}
			/**
			 * TODO iter--
			 */
			const_iterator operator--(int) {
				const_iterator tmp(*this);
				(*this) -= 1;
				return tmp;
			}
			/**
			 * TODO --iter
			 */
			const_iterator& operator--() {
				return (*this) -= 1;
			}
			/**
			 * TODO *it
			 */
			const T& operator*() const {
				return now -> data;
			}
			/**
			 * TODO it->field
			 */
			const T* operator->() const noexcept {
				return &(now -> data);
			}
			/**
			 * a operator to check whether two iterators are same (pointing to the same memory).
			 */
			bool operator==(const iterator &rhs) const {
				return now == rhs.now;
			}
			bool operator==(const const_iterator &rhs) const {
				return now == rhs.now;
			}
			/**
			 * some other operator for const_iterator.
			 */
			bool operator!=(const iterator &rhs) const {
				return now != rhs.now;
			}
			bool operator!=(const const_iterator &rhs) const {
				return now != rhs.now;
			}
	};
private:
	iterator last() {
		return siz ? iterator(tail -> tail, tail -> size, tail, this) : end();
	}
	const_iterator clast() {
		return siz ? iterator(tail -> tail, tail -> size, tail, this) : end();
	}
public:
	/**
	 * TODO Constructors
	 */
	deque() : siz(0), head(nullptr), tail(nullptr) {}
	deque(const deque &other) : siz(other.siz), head(nullptr), tail(nullptr) {
		copy(other);
	}
	/**
	 * TODO Deconstructor
	 */
	~deque() {
		clear();
	}
	/**
	 * TODO assignment operator
	 */
	deque &operator=(const deque &other) {
		if (&other == this) return (*this);
		clear();
		siz = other.siz;
		copy(other);
		return (*this);
	}
	/**
	 * access specified element with bounds checking
	 * throw index_out_of_bound if out of bound.
	 */
	T & at(const size_t &pos) {
		if (pos >= siz) throw index_out_of_bound();
		return *(begin() + (pos));
	}
	const T & at(const size_t &pos) const {
		if (pos >= siz) throw index_out_of_bound();
		return *(begin() + (pos));
	}
	T & operator[](const size_t &pos) {
		return at(pos);
	}
	const T & operator[](const size_t &pos) const {
		return at(pos);
	}
	/**
	 * access the first element
	 * throw container_is_empty when the container is empty.
	 */
	const T & front() const {
		return head -> head -> data;
	}
	/**
	 * access the last element
	 * throw container_is_empty when the container is empty.
	 */
	const T & back() const {
		return tail -> tail -> data;
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {
		if(empty()) return end();
		return iterator(head -> head, 1, head, this);
	}
	const_iterator cbegin() const {
		if(empty()) return cend();
		return {head -> head, 1, head, this};
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {
		return {nullptr, 0, nullptr, this};
	}
	const_iterator cend() const {
		return {nullptr, 0, nullptr, this};
	}
	/**
	 * checks whether the container is empty.
	 */
	bool empty() const {
		return !siz;
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const {
		return siz;
	}
	/**
	 * clears the contents
	 */
	void clear() {
		siz = 0;
		if (head) {
			Node *p = head -> next[1];
			while (p) {
				delete p -> next[0];
				p = p -> next[1];
			}
			delete tail;
		}
	}
	/**
	 * inserts elements at the specified locat on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 *     throw if the iterator is invalid or it point to a wrong place.
	 */
	iterator insert(iterator pos, const T &value) {
		++siz;
		if (pos == end()) {
			return push_back(value);
		}
		else {
			iterator tmp = split(pos.blo, pos.now, pos.pos) -> push_back(value);
			arrange(tmp.blo);
			tmp.ori = this;
			return tmp;
		}
	}
	/**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
	 */
	iterator erase(iterator pos) {
		--siz;
		if (pos.now == nullptr) throw invalid_iterator();
		if(pos.pos == pos.blo -> size) {
			pos.blo -> pop_back();
			if (pos.blo -> next[0]) {
				if (pos.blo -> next[0] -> size + pos.blo -> size <= MAXSIZE) {
					pos.blo = pos.blo -> next[0];
					merge(pos.blo, pos.blo -> next[1]);
				}
			}
			iterator tmp;
			if (pos.blo -> next[1] == nullptr) {
				tmp = end();
			}
			else iterator(pos.blo -> next[1] -> head, 1, pos.blo -> next[1], this);
			if (head -> size == 0) {
				Node *np = head;
				head = head -> next[1];
				delete np;
			}
			return tmp;
		}
		iterator ni = pos + 1;
		Node *p = split(pos.blo, pos.now, pos.pos);
		p -> next[1] -> pop_front();
		arrange(p);
		if (ni.blo -> head == ni.now) ni.pos = 1;
		else ni.pos--;
		return ni;
	}
	/**
	 * adds an element to the end
	 */
	iterator push_back(const T &value) {
		++siz;
		if (tail == nullptr) {
			head = tail = new Node;
		}
		if (tail -> size == MAXSIZE) {
			LLNode *np = new LLNode(value, nullptr, nullptr);
			Node *q = new Node(np, np, 1, tail, nullptr);
			tail -> next[1] = q;
			tail = q;
			if (!head) head = tail;
			return {np, 1, q, this};
		}
		else {
			iterator tmp = tail -> push_back(value);
			tmp.ori = this;
			return tmp;
		}
	}
	/**
	 * removes the last element
	 *     throw when the container is empty.
	 */
	void pop_back() {
		// --siz;
		// tail -> pop_back();
		erase(last());

	}
	/**
	 * inserts an element to the beginning.
	 */
	void push_front(const T &value) {
		// ++siz;
		insert(begin(), value);
	}
	/**
	 * removes the first element.
	 *     throw when the container is empty.
	 */
	void pop_front() {
		// --siz;
		erase(begin());
		// head -> pop_front();
		// head = arrange(head);
	}
};

}

#endif
