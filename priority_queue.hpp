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
	// template<typename U, class Comppare = std::less<U>>
	class node {
	public:
		T data;
		int dist;
		node *ch[2];
		node() //: ch[0](NULL), ch[1](NULL), data(T()), dist(0)
		{
			ch[0] = ch[1] = NULL;
			data = T();
			dist = 0;
		}
		node(const node &r) : data(r.data){
			// data = r.data;
			dist = r.dist;
			for (int i = 0; i < 2; ++i) {
				if (r.ch[i] == NULL) ch[i] = r.ch[i];
				else {
					ch[i] = new node(*r.ch[i]);
				}
			}
		}
		node(const T &d) : data(d){
			// data = d;
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
			if(r == NULL) return this;
			node *now = this;
			// std::cerr << "   " << now << " " << r << std::endl;
			if (Compare()(data, r -> data)) {
				// node *tmp = this;
				// this = r;
				// r = tmp;
				std::swap(now, r);
			}
			// std::cerr << "   " << now << " " << r << std::endl;
			if (now -> ch[1] == NULL) now -> ch[1] = r;
			else {
				now -> ch[1] = now -> ch[1] -> merge(r);
			}
			// std::cerr << "   " << now << " " << r << std::endl;
			if (now -> ch[1] && (now -> ch[0] == NULL || now -> ch[0] -> dist < now -> ch[1] -> dist)) {
				std::swap(now -> ch[0], now -> ch[1]);
			}
			 // std::cerr << "   " << now << " " << r << std::endl;
			now -> dist = now -> ch[1] == NULL ? 1 : now -> ch[1] -> dist + 1;
			return now;
		}
	};
	node *root;
	size_t siz;
public:
	/**
	 * TODO constructors
	 */
	priority_queue() : root(NULL), siz(0) {}
	priority_queue(const priority_queue &other) {
		root = new node(*other.root);
		siz = other.siz;
	}
	/**
	 * TODO deconstructor
	 */
	~priority_queue() {
		if (root == NULL) return;
		root -> empty();
		delete root;
	}
	/**
	 * TODO Assignment operator
	 */
	priority_queue &operator=(const priority_queue &other) {
		if (&other == this) return *this;
		siz = other.siz;
		if (root == NULL) {
			root = new node(*other.root);
		}
		else {
			root -> empty();
			*root = node(*other.root);
		}
		return *this;
	}
	/**
	 * get the top of the queue.
	 * @return a reference of the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	const T & top() const {
		// std::cerr << root << std::endl;
		if (root == NULL) {
			throw container_is_empty() ;
		}
		return root -> data;
	}
	/**
	 * TODO
	 * push new element to the priority queue.
	 */
	void push(const T &e) {
		++siz;
		// std::cerr << e << std::endl;
		if (root == NULL) root = new node(e);
		else {
			node *tmp = new node(e);
			// std::cerr << root << std::endl;
			root = root -> merge(tmp);
		}
	}
	/**
	 * TODO
	 * delete the top element.
	 * throw container_is_empty if empty() returns true;
	 */
	void pop() {
		if (root == NULL) throw container_is_empty() ;
		siz--;
		node *tmp;
		if(root -> ch[0] == NULL) {
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
		return siz;
	}
	/**
	 * check if the container has at least an element.
	 * @return true if it is empty, false if it has at least an element.
	 */
	bool empty() const {
		return root == NULL ? true : false;
		// root -> empty();
		// delete root;
		// root = NULL;
	}
	/**
	 * return a merged priority_queue with at least O(logn) complexity.
	 */
	void merge(priority_queue &other) {
		if (root == NULL) root = other.root;
		else {
			root = root -> merge(other.root);
		}
		other.root = NULL;
		siz = siz + other.siz;
	}
};

}

#endif
