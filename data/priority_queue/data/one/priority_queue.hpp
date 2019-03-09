#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {

/**
 * a container like std::priority_queue which is a heap internal.
 * it should be based on the vector written by yourself.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
	template<typename T, class Compare = std::less<T>>
	class node {
		T data;
		int dist;
		node *ch[2];
		node() : ch[0](NULL), ch[1](NULL), data(T()), dist(0){}
		node(const node &r) {
			data = r.data;
			dist = r.dist;
			for (int i = 0; i < 2; ++i) {
				if (r.ch[i] == NULL) ch[i] = r.ch[i];
				else {
					ch[i] = new node(*r.ch[i]);
				}
			}
		}
		node(const T &d) {
			data = d;
			dist = 1;
			ch[0] = ch[1] = NULL;
		}
		void empty() {
			for (int i = 0; i < 2; ++i) {
				if (ch[i] != NULL) {
					ch[i] -> empty();
					delete ch[i];
					ch[i] = NULL;
				}
			}
		}
		node *merge(node *r) {
			if (data < r.data) {
				std::swap(this, r);
			}
			if (ch[1] == NULL) ch[1] = r;
			else {
				ch[1] = ch[1] -> merge(r);
			}
			if (ch[0] -> dist < ch[1] -> dist) {
				std::swap(ch[0], ch[1]);
			}
			dist = ch[1] -> dist + 1;
		}
	}
	node *root;
	size_t size;
public:
	/**
	 * TODO constructors
	 */
	priority_queue() : root(NULL), size(0) {}
	priority_queue(const priority_queue &other) {
		root = new root(*other.root);
		size = 0;
	}
	/**
	 * TODO deconstructor
	 */
	~priority_queue() {
		root -> empty();
		delete root;
	}
	/**
	 * TODO Assignment operator
	 */
	priority_queue &operator=(const priority_queue &other) {
		if (other == *this) return *this;
		root -> empty();
		*root = node(other.root);
		return *this;
	}
	/**
	 * get the top of the queue.
	 * @return a reference of the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	const T & top() const {
		return root -> data;
	}
	/**
	 * TODO
	 * push new element to the priority queue.
	 */
	void push(const T &e) {
		if (root == NULL) root = node(e);
		else {
			root = root -> merge(node(e));
		}
	}
	/**
	 * TODO
	 * delete the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	void pop() {
		if (root == NULL) return;
		node *tmp;
		if(ch[0] == NULL) {
			tmp = root -> ch[1];
		}
		else tmp = root -> ch[0] -> merge(root -> ch[1]);
		delete root;
		root = tmp;
	}
	/**
	 * return the number of the elements.
	 */
	size_t size() const {
		return size;
	}
	/**
	 * check if the container has at least an element.
	 * @return true if it is empty, false if it has at least an element.
	 */
	bool empty() const {
		root -> empty();
		delete root;
		root = NULL;
	}
	/**
	 * return a merged priority_queue with at least O(logn) complexity.
	 */
	void merge(priority_queue &other) {
		if (root == NULL) root = other.root;
		else {
			root = root -> merge(other.root);
		}
		size = size + other.size;
	}
};

}

#endif
