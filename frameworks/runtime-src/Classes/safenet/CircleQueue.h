/******************************************************************************** 
 * create:cclin
 * date:2014-11-11
 * ver:
 * latest update:
 * function:ѭ������
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
    bool empty() const;//�ж�ѭ�������Ƿ�Ϊ��
    void traverse() const;//����ѭ������
    int length() const;//���ض�����Ԫ�ظ���
        
    bool push(T e);
    bool pop();
    T* front();
	T* rear();
private:
    int m_size; //��������洢�ռ���Ԫ��Ϊ��λ
    int m_front;//��ͷָ��
    int m_rear;//��βָ��
    T *m_base; //�洢�ռ��ַ
};

template <typename T>
CircleQueue<T>::CircleQueue(int init){
    m_size = init; //��ʼ��С
    m_base = new T[m_size];
    if(!m_base)exit(1);//�洢����ʧ��
    m_front = m_rear = 0;   //��ջ��Ԫ�ظ���Ϊ0
}

// ���ض���Q��Ԫ�ظ����������еĳ���
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

//����ǰ���в��������ڵ�ǰ���е�βԪ��֮��
//����Ԫ�� e Ϊ�µĶ���βԪ��,������TRUE,���򷵻�FALSE
template <typename T>
bool CircleQueue<T>::push(T e){
    if((m_rear+1)%m_size  == m_front){//������
        return false;
    }
    m_base[m_rear] = e;
    m_rear = (m_rear+1) % m_size;
    return true;
}

template <typename T>
bool CircleQueue<T>::pop(){
    if(m_front == m_rear){//���п�
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