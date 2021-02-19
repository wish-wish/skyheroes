/******************************************************************************** 
 * create:cclin
 * date:2014-11-11
 * ver:
 * latest update:
 * function:循环队列
 * update log:
 * <author>  <time>           <version >   <desc>
 *********************************************************************************/
#pragma once

#include "cocos2d.h"
#include "Singleton.h"
#include "CocosGUI.h"

USING_NS_CC;
using namespace std;
using namespace ui;

template <typename T>
class CircleQueue{
public:
    CircleQueue(int init=50);
    bool empty() const;//判断循环队列是否为空
    void traverse() const;//遍历循环队列
    int length() const;//返回队列中元素个数
        
    bool push(T e);
    bool pop();
    T* front();
	T* rear();
private:
    int m_size; //允许的最大存储空间以元素为单位
    int m_front;//队头指针
    int m_rear;//队尾指针
    T *m_base; //存储空间基址
};

template <typename T>
CircleQueue<T>::CircleQueue(int init){
    m_size = init; //初始大小
    m_base = new T[m_size];
    if(!m_base)exit(1);//存储分配失败
    m_front = m_rear = 0;   //空栈中元素个数为0
}

// 返回队列Q中元素个数，即队列的长度
template <typename T>
int CircleQueue<T>::length() const {
    return ((m_rear-m_front+m_size) % m_size);
}

template <typename T>
void CircleQueue<T>::traverse() const {
    int length = (m_rear-m_front+m_size) % m_size;
    CCLOG("rear to front: ");
    while(length--){
        CCLOG("queue %d",m_base[m_front+length]);
    }
    CCLOG("list end");
}

//若当前队列不满，这在当前队列的尾元素之后，
//插入元素 e 为新的队列尾元素,并返回TRUE,否则返回FALSE
template <typename T>
bool CircleQueue<T>::push(T e){
    if((m_rear+1)%m_size  == m_front){//队列满
        return false;
    }
    m_base[m_rear] = e;
    m_rear = (m_rear+1) % m_size;
    return true;
}

template <typename T>
bool CircleQueue<T>::pop(){
    if(m_front == m_rear){//队列空
        return false;
    }
    m_front = (m_front+1) % m_size;
    return true;
}

template <typename T>
T* CircleQueue<T>::front(){
	if(m_front!=m_rear)
		return m_base[m_front];
	else
		return nullptr;
}

template <typename T>
T* CircleQueue<T>::rear(){
	if(m_front!=m_rear)
		return m_base[m_rear];
	else
		return nullptr;
}