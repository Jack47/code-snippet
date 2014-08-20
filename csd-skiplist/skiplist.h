#include<stdio.h>
#include<time.h>
#include<stdlib.h>

//maxLevel is <=16
int getHeight(int maxLevel)
{
	assert(maxLevel>=1 && maxLevel<=16);

	int height = 1;
	int i;
	int rand_number = rand();
  rand_number = (rand_number>>8)&0xffff;

	printf("%d\t", rand_number);

	for(i=2;i<=maxLevel;i++)
	{
		if( (rand_number&0x01) == 1){
			height++;
			rand_number = (rand_number>>1);
		}
		else break;
	}
	assert(height>=1&&height<=16);
	return height;
}
void testGetHeight()
{
	int i = 0;
	int height = 0;
	for(i=0;i<=20;i++){
		height = getHeight(5);
		printf("%d\n", height);
	}
}
typedef strut_node Node;

struct struct_node 
{
	struct_node(int theight){
		assert(theight>=1 && <= kMaxHeight);
		ptr = new Node[theight+1];
		height = theight; 
	}
	int key;
	int value;
	Node* ptr;
	int height;
};

class SkipList
{
	static const int kMaxHeight = 5;
	public:
	//return true if find key, value is the key's value; else return false;
	bool Find(int key, int& value);
	void Insert(int key, int value);
	//if key exists, delete;
	void Delete(int key);
	SkipList(){
		memset(head_,0x00, sizeof(Node*)*(kMaxHeight+1));	
		maxHeight=1;
	};
	private:
	//return true,if find key, currentHeight is the height that first find this key, prev means the previous ptr in the levels we traversed
	//return false, if not find key, currentHeight is the height that we don't find the key, prev means the previous ptr in the levels we traversed
	//maxLevel points which level to begin to traverse
	//stopWhenFound=false, will find the previous ptr for every level if we find the key.
	bool Find(int key, int& value, bool stopWhenFound, Node*& ptr, int& currentHeight, Node* prev);
	private:
	Node head_[kMaxHeight+1];
	int maxHeight_;
 	
};
