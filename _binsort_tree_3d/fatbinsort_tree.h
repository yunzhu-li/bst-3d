



#ifndef __FATBST_H__
#define __FATBST_H__

//Types
struct bstNodeData{
	int iKey;
	void *iDataArray;
};

typedef struct bstNode{
	bstNode *lChild;
	bstNode *rChild;
	bstNode *Parent;
	bstNodeData nData;
}*bstNodePtr;

//Functions
void CreateBST(bstNode *&T, int *_seq);
//int CreateTree(bstNode *&T, bstNode *_parent);
int InsertBST(bstNode *&T,bstNodeData &e);
int SearchBST(bstNode *T,int key,bstNode *f,bstNode *&p);
void DeleteBST(bstNode *&T,int _key);
void DeleteBST3(bstNode *&p);
void PreOrderTraverse(bstNode *T, void (*_callback)(bstNode *_node));
void PostOrderTraverse(bstNode *T, void (*_callback)(bstNode *_node));
void InOrderTraverse(bstNode *T, unsigned char *_depth, void (*_callback)(bstNode *_node));
void LevelOrderTraverse(bstNode *T, void (*_callback)(bstNode *_node));
void LevelOrderTraverseWithNullNodes(bstNode *T, void (*_callback)(bstNode *_node, unsigned int _curNum, void *_param), void *_param);




void _vis(bstNode *_node)
{
	int _tmp;
	if(_node){
		_tmp = _node->nData.iKey;
	}else{
		_tmp = -1;
	}
}


void _testBST(bstNode *T)
{
	PostOrderTraverse(T, &_vis);

}






int SearchBST(bstNode *T,int key,bstNode *f,bstNode *&p)
{
	if(!T)							{  p = f;	return 0; }
	else if (key == T->nData.iKey)	{  p = T;	return 1; }
	else if (key <  T->nData.iKey)	return  SearchBST(T->lChild, key, T, p);  
	else							return  SearchBST(T->rChild, key, T, p);
} 


int InsertBST(bstNode *&T,bstNodeData &e)
{
	bstNode *s;
	bstNode *p;
	if(!SearchBST (T, e.iKey, NULL, p))
	{
		s = (bstNode *) malloc(sizeof(bstNode));
		s->nData = e;
		s->lChild = NULL; 
		s->rChild = NULL;
		s->Parent = p;
		if (!p){
			T = s;
		}
		else if(e.iKey < p->nData.iKey){
			p->lChild = s;
		}else{
			p->rChild = s;
		}
		return 1;
	} 
	else return 0; //Already exists
}



void CreateBST(bstNode *&T, int *_seq)
{
	int _i = 1;
	bstNodeData _nData;
	T->nData.iKey = _seq[0];
	while(_seq[_i]){
		_nData.iKey = _seq[_i];
		InsertBST(T, _nData);
		_i++;
	}
}


/*
int CreateTree(bstNode *&T, bstNode *_parent)
{

	//char _node_seq[] = {45, 24, 12, 0x23, 13, 0x23, 0x23, 37, 0x23, 0x23, 53, 0x23, 93, 61, 0x23, 0x23, 0x23, 0x23};
	//char _node_seq[] = {45, 13, 3, 1, 0x23, 0x23, 0x23, 37, 4, 0x23, 0x23, 38, 0x23, 0x23, 53, 47, 0x23, 0x23, 99, 61, 0x23, 0x23, 0x23, 0x23};
	char ch = _node_seq[_seqPos++];

	if (ch == 0x23){T = NULL;}
	else{
		T = (bstNode *)malloc(sizeof(bstNode));
		T->nData.iKey = ch;
		T->Parent = _parent;
		CreateTree(T->lChild, T);
		CreateTree(T->rChild, T);
	}
	return 1; //OK
}
*/



void DeleteBST(bstNode *&T,int _key)
{
	if(!T) return;
	else{
		if (_key == T->nData.iKey)  return DeleteBST3(T);
		else if (_key < T->nData.iKey)  return DeleteBST(T->lChild, _key);
		else return DeleteBST(T->rChild, _key);
	}
}//DeleteBST



void DeleteBST3(bstNode *&p)
{
	//TODO
	bstNode *q, *s;
	if(!p->rChild){
		q = p; 
		if(p->lChild) {p->lChild->Parent = p->Parent;}
		p = p->lChild; 
		free(q);
	}
	else if (!p->lChild){
		q = p; 
		if(p->rChild) {p->rChild->Parent = p->Parent; }
		p = p->rChild; 
		free(q);
	}else{
		q = p; s = p->lChild;
		while(s->rChild){
			q = s; 
			s = s->rChild;
		}
		s->Parent = p->Parent;
		p->nData = s->nData;
		if(q != p){
			if(s->lChild){
				s->lChild->Parent = q;
			}
			q->rChild = s->lChild;
		}else{
			if(s->lChild){
				s->lChild->Parent = p;
			}
			q->lChild = s->lChild;
		}
		delete s;
	}
}//DeleteBST3

void PreOrderTraverse(bstNode *T, void (*_callback)(bstNode *_node))
{
	bstNode *_pT = T;
	bstNode * _stack[4096];
	unsigned int _stackItems = 0;

	while(_pT || _stackItems)
	{
		while(_pT){
			if (_callback) {_callback(_pT);}
			_stack[_stackItems++] = _pT;
			_pT = _pT->lChild;
		}
		_pT = _stack[--_stackItems];
		if(_pT){_pT = _pT->rChild;}
	}
}//PreOrderTraverse


void InOrderTraverse(bstNode *T, unsigned char *_depth, void (*_callback)(bstNode *_node))
{
	bstNode *_pT = T;
	int _iKeysVisited[4096];
	int _iVisitedNum = 0;
	unsigned char _iDepth = 0;
	int _iCurLevel = 0;

	BOOL _fromLowLv = 0;	//From lower level
	BOOL _nodeVisited = 0;

	memset(_iKeysVisited, 0, 4096 * 4);

	while(1)
	{

		if (_iCurLevel > _iDepth){_iDepth = _iCurLevel;}

		_nodeVisited = 0;
		for (UINT8 _i = 0; _i < 4096; _i++){
			if(_iKeysVisited[_i] == 0){break;}
			if(_iKeysVisited[_i] == _pT->nData.iKey){
				if(!_pT->Parent){
					if(_depth){*_depth = _iDepth + 1;}
					return;
				}
				_pT = _pT->Parent;
				_iCurLevel--;
				_fromLowLv = 1;
				_nodeVisited = 1;
				break;
			}
		}
		if (!_nodeVisited)
		{
			if(!_fromLowLv)
			{
				if(_pT->lChild){
					_pT = _pT->lChild;
					_iCurLevel++;
					_fromLowLv = 0;
					continue;
				}
			}
			if (_callback) {_callback(_pT);}
			_iKeysVisited[_iVisitedNum] = _pT->nData.iKey;
			_iVisitedNum++;

			if(_pT->rChild){
				_pT = _pT->rChild;
				_iCurLevel++;
				_fromLowLv = 0;
				continue;
			}
		}
	}
}//InOrderTraverse




void PostOrderTraverse(bstNode *T, void (*_callback)(bstNode *_node))
{
	bstNode *_pT = T;
	bstNode * _stack[4096];
	unsigned int _stackItems = 0;
	unsigned int flag;     // Key of visited node 
	bstNode *q = (bstNode *)malloc(sizeof(bstNode));
	_pT = T;
	while (_pT || _stackItems)
	{
		if(_pT != q)
		{
			while(_pT)                                               
			{
				//push _pT
				_stack[_stackItems++] = _pT; 
				if(_pT->lChild) {_pT = _pT->lChild;}
				else _pT = _pT->rChild;
			}
		}
		if (!_stackItems)   break;                       
		//Gettop
		q = _stack[_stackItems - 1];

		if(q->rChild != NULL)
		{
			if(q->rChild == _pT)      
			{
				_pT = (bstNode *)malloc(sizeof(bstNode));
				//Pop
				_pT = _stack[--_stackItems];
				if (_callback) {_callback(_pT);}
				_pT = q;
				flag = _pT->nData.iKey;
			}
			else
			{
				_pT = q->rChild;
				if (flag == _pT->nData.iKey)   
				{
					_pT = (bstNode *)malloc(sizeof(bstNode));
					//Pop
					_pT = _stack[--_stackItems];
					if (_callback) {_callback(_pT);}
					_pT= q;
					flag = _pT->nData.iKey;
				}
			}
		}
		else {
			_pT = (bstNode *)malloc(sizeof(bstNode));
			//Pop
			_pT = _stack[--_stackItems];
			if (_callback) {_callback(_pT);}
			_pT= q;
			flag = _pT->nData.iKey;    
		}
	}
}//PostOrderTraverse


void LevelOrderTraverse(bstNode *T, void (*_callback)(bstNode *_node))
{
    bstNode *_pT;
    bstNode *_quene[256];
    int _front, _rear;
    _front = _rear = -1;
    _rear++;
    _quene[_rear] = T;
    while(_front != _rear)
    {
        _front++;
        _pT = _quene[_front];
		if (_callback) {_callback(_pT);}
        if(_pT->lChild != NULL)
        {
            _rear++;
            _quene[_rear]=_pT->lChild;
        }
        if(_pT->rChild!=NULL)
        {
            _rear++;
            _quene[_rear]=_pT->rChild;
        }
    }
}//PostOrderTraverse

void LevelOrderTraverseWithNullNodes(bstNode *T, void (*_callback)(bstNode *_node, unsigned int _curNum, void *_param), void *_param)
{
	bstNode *_pT;
	bstNode *_quene[4096];
	int _front, _rear;
	unsigned char _depth;
	unsigned int _curNum = 0;
	unsigned int _nullCounter = 0;

	//Initialize Variables
	_front = -1; _rear = -1;
	//Get Depth
	InOrderTraverse(T, &_depth, NULL);

	_rear++;
	_quene[_rear] = T;
	while(_front != _rear)
	{
		_front++;
		_pT = _quene[_front];
		//Callback
		if (_callback) {_callback(_pT, _curNum, _param);}
		_curNum ++;
		if (!_pT) {
			_rear += 2;
			_quene[_rear - 1] = NULL;
			_quene[_rear] = NULL;
			_nullCounter += 2;
			if(_nullCounter >= pow(2, (double)_depth)){return;}
			continue;
		}
		_nullCounter = 0;
		//Add to quene
		_rear++;
		_quene[_rear] = _pT->lChild;
		_rear++;
		_quene[_rear] = _pT->rChild;

	}
}//LevelOrderTraverseWithNullNodes

















#endif //__FATBST_H__





