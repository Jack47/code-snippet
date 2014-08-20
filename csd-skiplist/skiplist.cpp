#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include<assert.h>
void Insert(int key, int value)
{
	int value;
	Node* curPtr = NULL;
	Node* curNode = NULL;	
	//prev node in every height
	Node prev[kMaxHeight];
	int curHeight;

	bool ret = Find(key, value, true, curPtr, curHeight, prev);
	if(ret==true){
		curPtr->value = value;
		printf("found key[%d] when insert, replace it with value[%d]\n", key, value);
	}
	else {
	  assert(curPtr!=NULL);
		assert(curHeight==curPtr->height);
		assert(curPtr, (prev[curHeight]->ptr)+curHeight);
		int height = getHeight(kMaxHeight);
		printf("insert key[%d] with height[%d]\n", key, height);
		Node* nodePtr = new Node(height);
		nodePtr->key = key;
		nodePtr->value = value;
	  
		while(curHeight>=1){
			nodePtr->ptr[curHeight] = prev[curHeight]->ptr[curHeight];
			prev[curHeight]->ptr[curHeight] = nodePtr;
			curHeight--;
		}
	}
}
void SkipList::Delete(int key)
{
	int value;
	Node* curPtr = NULL;
	Node* curNode = NULL;	
	//prev node in every height
	Node prev[kMaxHeight];
	int curHeight;

	memset(prev, 0, sizeof(Node*)*kMaxHeight);
	bool ret = Find(key, value, false, curPtr, curHeight, prev);
	if(ret==true){
	  assert(curPtr!=NULL);
		assert(curHeight==curPtr->height);
		assert(curPtr, (prev[curHeight]->ptr)+curHeight);
		curNode = curPtr;

		while(curHeight>=1){
			prev[curHeight]->ptr[curHeight] = curPtr->ptr[curHeight];
			curHeight--;
		}
	}
}

bool SkipList::Find(int key, int& value){
	Node* ptr = NULL;
	int curHeight;
	//prev node in every height
	Node prev[kMaxHeight];

	memset(prev, 0, sizeof(Node*)*kMaxHeight);
	bool ret = Find(key, value, true, this.maxHeight, ptr, curHeight, prev); 
	return ret;
}
void SkipList::downForward(Node* prev, int& curHeight, Node*& curPtr)
{
	assert(curHeight>=0);
	assert(curPtr!=NULL);
	prev[curHeight] = curPtr;
	curHeight--;
	curPtr--;
}
void SkipList::rightForward(Node* prev, int curHeight, Node*& curPtr)
{
	assert(curPtr!=NULL);
	prev[curHeight] = curPtr;
	curPtr = curPtr->ptr[curHeight];
}
bool SkipList::Find(int key, int& value, bool stopWhenFound, Node*& currentPtr, int& currentHeight, Node* prev)
{
	int curHeight = this.maxHeight_;
	Node* curPtr = this.head_+curHeight;
	bool found = false;

	while(curPtr!=NULL && curHeight>=1){
		if(curPtr->key<key){//right forward 
			if(curPtr->ptr[curHeight]!=NULL)
				rightForward(prev, curHeight, curPtr);
			else {//NULL means maximum; can't right forward
				downForward(prev, curHeight, curPtr);	
			}
		}
		else if(curPtr->key>key){//down forward 
			downForward(prev, curHeight, curPtr);	
		}
		else {
			value = curPtr->value;
			currentPtr = curPtr;
			currentHeight = curHeight;
			found = true;

			if(stopWhenFound || curHeight==1){
				break;
			}//start from the next lower level
			else {
				downForward(prev, curHeight, curPtr);	
			}
		}
	}
	//currentHeight,  currentPtr is point to the ptr if found key, else is not assigned 
	//= curHeight;
	return found;
}
void testInsert(){
	
}
int main()
{
	srand(time(NULL));
	//testGetHeight();	
	
	
}
