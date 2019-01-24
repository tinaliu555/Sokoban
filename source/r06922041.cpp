#include <cstdio>
#include <iostream>
#include <queue>
#include <set>
#include <utility>
#include <cmath>
#include <cstddef>
#include <vector>
#include <thread>
#include <bitset>
#include"sokoban.h"
Board Brd;
int BoxCnt=0;
std::vector<Move> History;
#define BOXMAXSIZE 11
#define maxLoop 5000
using namespace std;
#define HEIGHT_MAX 15
#define WIDTH_MAX 15
bool NotFindGoal = true;
int canAchieveBrd[HEIGHT_MAX][WIDTH_MAX+2];
bool canAchieveBrdAll[BOXMAXSIZE][HEIGHT_MAX][WIDTH_MAX+2];

unsigned long long int goalLocated[BOXMAXSIZE];
bool isPush;

class HistoryID{
public:
	unsigned long long int id;
	std::vector<Move> mv;
	HistoryID(unsigned long long int newId, std::vector<Move>& newMv){
		id = newId;
		mv = newMv;
	}
	bool operator ==(const HistoryID & obj) const
	  {
	    if (id == obj.id)
	      return true;
	    else
	      return false;
	  }
	  bool operator<(const HistoryID& other) const{
	  	return id < other.id;
	  }
};


namespace std
{
  template<>
    struct hash<HistoryID>
    {
      size_t
      operator()(const HistoryID & obj) const
      {
        return hash<int>()(obj.id);
      }
    };
}

std::set<HistoryID> backward;

class Point {
	private:
	public:
		int xval, yval;
		// Constructor uses default arguments to allow calling with zero, one,
		// or two values.
		Point(int x = 0, int y = 0) {
			xval = x;
			yval = y;
		}

		Point(const Point & p){
		//	cout<<"P-Copy constructor"<<endl;
			xval = p.xval;
			yval = p.yval;
		}

		// Extractors.
		int x() const { return xval; }
		int y() const { return yval; }

		// Distance to another point.  Pythagorean thm.
		double dist(Point other) {
			return abs(xval - other.xval) + abs(yval - other.yval);
		}

		// Print the point on the stream.  The class ostream is a base class
		// for output streams of various types.
		void print(std::ostream &strm)
		{
			strm << "(" << xval << "," << yval << ")";
		}
		bool operator==(const Point& p){
			return (p.xval==xval)&&(p.yval==yval);
		}
		bool operator<(const Point & p) const{
			return ((xval < p.xval)||(yval<p.yval));
		}
		friend ostream& operator<<(ostream& o, const Point& p);
};

std::vector<Point> goalPos;

void printVM(const vector<Move>& v){
	printf("%lu\n", v.size());
	for (auto& x: v){
		putchar("udlr"[x.dir]);
		
	}
	printf("\n");
	return;
}

void printVMBoth(const vector<Move>& v,const vector<Move>& v2){
	printf("%lu\n", v.size()+v2.size());
	for (auto& x: v){
		putchar("udlr"[x.dir]);
		
	}
	for (auto& x: v2){
		putchar("udlr"[x.dir]);
		
	}
	printf("\n");
	return;
}
int calculate(int i, int j, Board& b){
    int sumHeuristic = 0;
    if(b.board[i][j]=='*')
        return 0;
    for(vector<Point>::iterator it=goalPos.begin(); it != goalPos.end(); it++){
        if(b.board[it->xval][it->yval]=='*')
            continue;
        sumHeuristic += abs(i-it->xval) + abs(j-it->yval);
    }
    return sumHeuristic;
}
class NodeCost{
	private:
	public:
		int g;//depth
		int cost=0;//distance  + depth
		Board B;
		vector<Move> mv;
		NodeCost(){}
		NodeCost(int g2, Board& b, vector<Move>& vectorMV){
			// cout<<"NCost constructor"<<endl;
			g = g2;
			B = b;
			mv = vectorMV;
			cost = Manhattan() + g2;
			// cost = Manhattan();
			// if(BoxCnt<=8)
			// 	cost = g2;
			// else
				// cost = Manhattan();
		}
		NodeCost(int g2, Board& b, vector<Move>& vectorMV, bool isBack){
			// cout<<"NCost constructor"<<endl;
			g = g2;
			B = b;
			mv = vectorMV;
			cost = g2;
		}
		NodeCost(const NodeCost& n){
			// cout<<"NCost-Copy constructor"<<endl;
			g = n.g;
			B = n.B;
			mv = n.mv;
			cost = n.cost;
		}
		bool operator<(const NodeCost & n) const{
			return cost > n.cost;
		}
		friend ostream& operator<<(ostream& o, const NodeCost& n);
		int Manhattan(){
			int min=50;
			int minPlayer=100;
			int dis = 0;
			int temp;
			int w = B.width;
			int x,y;
			// int sumHeuristic =0;
			for(int i=0;i<50;i++){
				if((B.id>>i) & 1){
					x = i / w;
					y = i - (x * w);
					min = abs(x-goalPos[0].xval) + abs(y-goalPos[0].yval);
					temp = abs(x-B.p_row) + abs(y-B.p_col);
					if(minPlayer>temp)
						minPlayer = temp;
					for(int j=1; j<BoxCnt; ++j){
						temp = abs(x-goalPos[j].xval) + abs(y-goalPos[j].yval);
						if(min>temp)
							min = temp;
					}
					dis += min;
				}
			}
			return dis + minPlayer;
		}
};

std::ostream& operator<<(std::ostream& o, const Point & p){
	o << "(" << p.x() << "," << p.y() << ") ";
	return o;
}

std::ostream& operator<<(std::ostream& o, const NodeCost& n){
	o << "[NodeCost: cost:"<<n.cost<<" ]"<<endl;
	o << "m_cnt: "<< n.B.m_cnt << ", height: " << n.B.height << " , weight : " << n.B.width << ", Player: ("<< n.B.p_row << "," << n.B.p_col << ")" <<endl;
	for(int i=0;i<=n.B.height-1;++i){
		for(int j=0;j<=n.B.width-1;++j){
			o << n.B.board[i][j];
		}
		o << endl;
	}
	printVM(n.mv);
	return o;
}

int const Di[]={-1,1,0,0},Dj[]={0,0,-1,1};

int notOnGoal;
bool isAxisGetDeadlock(int x, int y, Board& b, bool checkVertical){
	int h = b.height, w = b.width;
	if(checkVertical){
		if(x==0 || b.board[x-1][y] == '#')
			return true;
		else if(x==h-1 || b.board[x+1][y] == '#')
			return true;
		else if(b.board[x-1][y] == '$'){
			b.board[x-1][y] = '#';
			if(isAxisGetDeadlock(x-1,y,b,false)){
				notOnGoal++;
				return true;
			}
			else if(b.board[x+1][y] == '$'){
				b.board[x+1][y] = '#';
				if(isAxisGetDeadlock(x+1,y,b,false)){
					notOnGoal++;
					return true;
				}
				return false;
			}
			else if(b.board[x+1][y] == '*'){
				b.board[x+1][y] = '#';
				return isAxisGetDeadlock(x+1,y,b,false);
			}
		}
		else if(b.board[x-1][y] == '*'){
			b.board[x-1][y] = '#';
			if(isAxisGetDeadlock(x-1,y,b,false))
				return true;
			else if(b.board[x+1][y] == '$'){
				b.board[x+1][y] = '#';
				if(isAxisGetDeadlock(x+1,y,b,false)){
					notOnGoal++;
					return true;
				}
				return false;
			}
			else if(b.board[x+1][y] == '*'){
				b.board[x+1][y] = '#';
				return isAxisGetDeadlock(x+1,y,b,false);
			}
		}
		else if(b.board[x+1][y] == '$'){
				b.board[x+1][y] = '#';
				if(isAxisGetDeadlock(x+1,y,b,false)){
					notOnGoal++;
					return true;
				}
				return false;
			}
			else if(b.board[x+1][y] == '*'){
				b.board[x+1][y] = '#';
				return isAxisGetDeadlock(x+1,y,b,false);
		}
		else
			return false;

	}else{
		if(y==0 || b.board[x][y-1] == '#')
			return true;
		else if(y==w-1 || b.board[x][y+1] == '#')
			return true;
		else if(b.board[x][y-1] == '$'){
			b.board[x][y-1] = '#';
			if(isAxisGetDeadlock(x,y-1,b,true)){
				notOnGoal++;
				return true;
			}
			else if(b.board[x][y+1] == '$'){
				b.board[x][y+1] = '#';
				if(isAxisGetDeadlock(x,y+1,b,true)){
					notOnGoal++;
					return true;
				}
				return false;
			}
			else if(b.board[x][y+1] == '*'){
				b.board[x][y+1] = '#';
				return isAxisGetDeadlock(x,y+1,b,true);
			}
		}
		else if(b.board[x][y-1] == '*'){
			b.board[x][y-1] = '#';
			if(isAxisGetDeadlock(x,y-1,b,true))
				return true;
			else if(b.board[x][y+1] == '$'){
				b.board[x][y+1] = '#';
				if(isAxisGetDeadlock(x,y+1,b,true)){
					notOnGoal++;
					return true;
				}
				return false;
			}
			else if(b.board[x][y+1] == '*'){
				b.board[x][y+1] = '#';
				return isAxisGetDeadlock(x,y+1,b,true);
			}
		}
		else if(b.board[x][y+1] == '$'){
				b.board[x][y+1] = '#';
				if(isAxisGetDeadlock(x,y+1,b,true)){
					notOnGoal++;
					return true;
				}
				return false;
			}
			else if(b.board[x][y+1] == '*'){
				b.board[x][y+1] = '#';
				return isAxisGetDeadlock(x,y+1,b,true);
		}
		else
			return false;
	}
	return false;
}

bool isRecursiveDeadLock(int& x, int& y, Board *const b_ptr){
	Board tempB = *b_ptr;
	notOnGoal = 0;
	if((isAxisGetDeadlock(x,y,tempB,true) && isAxisGetDeadlock(x,y,tempB,false))){
		if(b_ptr->board[x][y] == '$')
			notOnGoal++;
		// cout<<"notOnGoal="<<notOnGoal;
		if(notOnGoal!=0){
			return true;
		}
	}
	return false;
}

bool isCornerDeadLock(int x, int y, Board *const b_ptr){
	int cntWall=0;
	bool isWall[4];
	for(int j=0;j<4;++j){
		isWall[j] = false;
	}
	if( x<1 || b_ptr->board[x-1][y] == '#'){
		cntWall++;
		isWall[0] = true;
	}
	if( y<1 || b_ptr->board[x][y-1] == '#'){
		cntWall++;
		isWall[1] = true;
	}
	if(cntWall>1)
		return true;
	if( x+1>=b_ptr->height || b_ptr->board[x+1][y] == '#'){
		cntWall++;
		isWall[2] = true;
	}
	if(isWall[1] && isWall[2])
		return true;
	if( y+1>=b_ptr->width || b_ptr->board[x][y+1] == '#'){
		cntWall++;
		isWall[3] = true;
	}
	if(cntWall>=3)
		return true;
	if(isWall[2] && isWall[3])
		return true;
	if(isWall[3] && isWall[0])
		return true;
	return false;
}

bool isNeighborBlockWithWall(int x, int y, Board& b, bool isvertical, bool isBigger){
	int h = b.height, w = b.width;
	if(isvertical){
		if(isBigger){
	/**   
	# $     $ #      $ #    # $
	#($)    #($)    ($)#     ($)#
	**/
			if(y<1 || y+1>=w)
				return true;
			if(b.board[x][y-1] == '#'){
				if(b.board[x-1][y-1] == '#')
					return true;
				if(b.board[x-1][y+1] == '#')
					return true;
			}
			if(b.board[x][y+1] == '#'){
				if(b.board[x-1][y+1] == '#')
					return true;
				if(b.board[x-1][y-1] == '#')
					return true;
			}
		}else{
	/**   
	#($)    #($)     ($)#    ($)#
	# $       $ #     $ #   # $  
	**/
			if(y<1 || y+1>=w)
				return true;
			if(b.board[x][y-1] == '#'){
				if(b.board[x+1][y-1] == '#')
					return true;
				if(b.board[x+1][y+1] == '#')
					return true;
			}
			if(b.board[x][y+1] == '#'){
				if(b.board[x+1][y+1] == '#')
					return true;
				if(b.board[x+1][y-1] == '#')
					return true;
			}
		}
	}else{
		if(isBigger){
	/**
	#  #       #             #    
	$ ($)    $($)    $($)    $($)   
	         #       # #       #...
	**/
			if(x<1 || x+1>=h)
				return true;
			if(b.board[x-1][y] == '#'){
				if(b.board[x-1][y-1] == '#')
					return true;
				if(b.board[x+1][y-1] == '#')
					return true;
			}
			if(b.board[x+1][y] == '#'){
				if(b.board[x+1][y-1] == '#')
					return true;
				if(b.board[x-1][y-1] == '#')
					return true;
			}
		}else{
	/**
	 # #     #                # 
	($)$    ($)$   ($)$    ($)$ 
	           #.  .# #     #....
	**/
			if(x<1 || x+1>=h)
				return true;
			if(b.board[x-1][y] == '#'){
				if(b.board[x-1][y+1] == '#')
					return true;
				if(b.board[x+1][y+1] == '#')
					return true;
			}
			if(b.board[x+1][y] == '#'){
				if(b.board[x+1][y+1] == '#')
					return true;
				if(b.board[x-1][y+1] == '#')
					return true;
			}
		}
	}
	return false;
}

bool isMdeadloack(int& x, int& y, Board *const b_ptr){
	/*
	##
	#.
	#$
	#.
	##
	*/
	if((!Inside(b_ptr,x-1,y-1) && !Inside(b_ptr,x,y-1) && !Inside(b_ptr,x+1,y-1)) || 
		(!Inside(b_ptr,x-1,y+1) && !Inside(b_ptr,x,y+1) && !Inside(b_ptr,x+1,y+1))){
			if(b_ptr->board[x-1][y] != '.' && b_ptr->board[x+1][y] != '.'){
				if(!Inside(b_ptr,x-2,y) && !Inside(b_ptr,x+2,y)){
					return true;
				}
			}
		}
	/*
	#####
	#.$.#
	*/
	else 
		if((!Inside(b_ptr,x-1,y-1) && !Inside(b_ptr,x-1,y) && !Inside(b_ptr,x-1,y+1)) || 
		(!Inside(b_ptr,x+1,y-1) && !Inside(b_ptr,x+1,y) && !Inside(b_ptr,x+1,y+1))){
			if(b_ptr->board[x][y-1] != '.' && b_ptr->board[x][y+1] != '.'){
				if(!Inside(b_ptr,x,y-2) && !Inside(b_ptr,x,y+2)){
					return true;
				}
			}
		}
	return false;
}

bool IsBox(Board const *const b_ptr, int const i, int const j){
    return b_ptr->board[i][j]=='$' || b_ptr->board[i][j]=='*';
}
bool IsBoxWall(Board const *const b_ptr, int const i, int const j){
    return !Inside(b_ptr, i, j) || IsBox(b_ptr, i, j);
}

bool is2X2Deadlock(int const x, int const y, Board *const b_ptr){

	if(canAchieveBrd[x][y]==0){
		return true;
	}
			// if(isRecursiveDeadLock(x,y,b_ptr)){
			// 	// cout<<"X:"<<x<<" , Y:"<<y<<endl;
			// 	return true;
			// }
	if(b_ptr->board[x][y]=='*')
		return false;
	if(IsBoxWall(b_ptr, x+1, y) && IsBoxWall(b_ptr, x, y+1) && IsBoxWall(b_ptr, x+1, y+1))
        return true;
    if(IsBoxWall(b_ptr, x+1, y) && IsBoxWall(b_ptr, x, y-1) && IsBoxWall(b_ptr, x+1, y-1))
        return true;
    if(IsBoxWall(b_ptr, x-1, y) && IsBoxWall(b_ptr, x, y+1) && IsBoxWall(b_ptr, x-1, y+1))
        return true;
    if(IsBoxWall(b_ptr, x-1, y) && IsBoxWall(b_ptr, x, y-1) && IsBoxWall(b_ptr, x-1, y-1))
        return true;
    
    return false;
}
/**
	 * simple check to see if any of the boxes are in a deadlock state:
	 *  #  or  #####  or  ##
	 * #$      #X$X#      $$
**/
bool isDeadlock(Board *const b_ptr){
	int x,y;
	int w = b_ptr->width;
	unsigned long long int boxLocated = 0;
	for(int i=0;i<50;i++){
		if((b_ptr->id>>i) & 1){
			x = i / w;
			y = i - (w * x);
			if(canAchieveBrd[x][y]==0){
				return true;
			}
			if(b_ptr->board[x][y]=='$'){
				if(isMdeadloack(x,y,b_ptr)){
					return true;
				}
			}
			if(isRecursiveDeadLock(x,y,b_ptr)){
				// cout<<endl<<"isRecursiveDeadLock:true "<<x<<","<<y<<endl;
				return true;
			}
			if(b_ptr->board[x][y]=='$' || b_ptr->board[x][y]=='*')
				boxLocated |= 1ULL << i;
		}
	}

	for(int c=0;c<BoxCnt;++c){
		if((goalLocated[c] & boxLocated)==0){
			// cout<<endl<<"goalLocated:true "<<x<<","<<y<<endl;
			return true;
		}
	}
	return false;
}

bool isBoxDeadlock(int dir, Board *const b_ptr){
	int x = b_ptr->p_row + Di[dir], y = b_ptr->p_col + Dj[dir];
	if(canAchieveBrd[x][y]==0){
		return true;
	}
	

	if(b_ptr->board[x][y]=='$'){
		if(isMdeadloack(x,y,b_ptr)){
			return true;
		}
	}
	if(isRecursiveDeadLock(x,y,b_ptr)){
		// cout<<endl<<"isRecursiveDeadLock:true "<<x<<","<<y<<endl;
		return true;
	}
	// cout<<endl<<"isRecursiveDeadLock:false "<<x<<","<<y<<endl;
	return false;
}

bool isDeadlock(int x,int y, Board *const b_ptr){
	if(canAchieveBrd[x][y]==0){
		return true;
	}
	
	if(b_ptr->board[x][y]=='$'){
		if(isMdeadloack(x,y,b_ptr)){
			return true;
		}
	}
	if(isRecursiveDeadLock(x,y,b_ptr)){
		return true;
	}
	return false;
}
int fCost(const NodeCost& n){
	return n.cost;
}
bool DFS_2(int* threshold){	
	std::priority_queue<NodeCost> pq;
	std::set<unsigned long long int> myset;
	NodeCost firstNode(0,Brd,History);
	pq.push(firstNode);
	myset.insert(firstNode.B.id);
	vector<Move> VM;
	while (!pq.empty()) {
		for(int i=0; i<=3; ++i){

			NodeCost temp = pq.top();
			// for(int i2=0;i2<temp.B.height;i2++){
			// 				for(int j=0;j<temp.B.width;j++){
			// 					cout<<pq.top().B.board[i2][j];
			// 				}
			// 				cout<<endl;
			// 			}cout<<endl;
			if(Do_move(&temp.B, (Direction) i, &temp.mv)){
				if(myset.insert(temp.B.id).second){//Not exist in myset
					if(temp.B.m_cnt==0){
						printVM(temp.mv);
						return true;//goal is found!
					}
					auto search = backward.find(HistoryID(temp.B.id,VM));
					if (search != backward.end()) {
						printVMBoth(temp.mv,(*search).mv);
						return true;
					}
					// if(isPush && isDeadlock(&temp.B)!=isBoxDeadlock(i,&temp.B)){
					// 	cout<<"Pop:("<<pq.top().B.p_row<<","<<pq.top().B.p_col<<")"<<endl;
					// 	for(int i2=0;i2<temp.B.height;i2++){
					// 		for(int j=0;j<temp.B.width;j++){
					// 			cout<<pq.top().B.board[i2][j];
					// 		}
					// 		cout<<endl;
					// 	}cout<<endl;
					// 	for(int i2=0;i2<temp.B.height;i2++){
					// 		for(int j=0;j<temp.B.width;j++){
					// 			cout<<temp.B.board[i2][j];
					// 		}
					// 		cout<<endl;
					// 	}cout<<endl;
					// 	cout<<(temp.B.p_row+Di[i])<<","<<(temp.B.p_col+Dj[i])<<" i="<<i<<endl;
					// 	cout<<"* "<<isDeadlock(&temp.B)<<", "<<isBoxDeadlock(i,&temp.B)<<"("<<temp.B.p_row<<","<<temp.B.p_col<<")"<<endl;
					// }
					if(!isPush || !isDeadlock(&temp.B)){
						pq.push((NodeCost(temp.g+1,temp.B,temp.mv)));
					}
				}
				
			}

		}
		pq.pop();
	}
	return false;
}

bool IdIDA(){
	int threshold = 100;
	bool findGoal = false;
	// while( threshold<maxLoop ){
		// printf("%d ",threshold);
		// if(DFS_2(&threshold, boxPos, pq, myset)){
		if(DFS_2(&threshold)){
			findGoal = true;
			// break;
		}
	// 	threshold+=20;
	// 	cout<<"Threshold:"<<threshold<<endl;
	// }
	if(!findGoal){
		cout<<"Threshold:"<<threshold<<endl;
		printf("Can't find solution in loop index %d\n",maxLoop);
	}
	NotFindGoal = false;
	return findGoal;
}

void ReverseBFS(Board & goalBrd){
	std::priority_queue<NodeCost> pq;
	Board btemp;
	vector<Move> VM;
	int h = Brd.height, w = Brd.width;
	for(int i=0;i<h;++i){
		for(int j=0;j<w;++j){
			if(goalBrd.board[i][j]=='-'){
				// if((Inside(&goalBrd,i-1,j) && goalBrd.board[i-1][j]=='*') || 
				// 	(Inside(&goalBrd,i+1,j) && goalBrd.board[i+1][j]=='*') ||
				// 	(Inside(&goalBrd,i,j-1) && goalBrd.board[i][j-1]=='*') ||
				// 	(Inside(&goalBrd,i,j+1) && goalBrd.board[i][j+1]=='*'))
				{
					btemp = goalBrd;
					btemp.p_row = i;
					btemp.p_col = j;
					btemp.board[i][j] = '@';
					btemp.id |= ( (unsigned long long int) (i * w + j)) << 50;
					backward.insert(HistoryID(btemp.id,VM));
					pq.push(NodeCost(0,btemp,VM,true));
				}
				
			}
		}
	}
	while(!pq.empty() && NotFindGoal){
		for(int i=0; i<=3; ++i){
			NodeCost temp = pq.top();
			if(Do_backward_move(&temp.B, (Direction) i, &temp.mv, true)){
				if(backward.insert(HistoryID(temp.B.id,temp.mv)).second){
					pq.push((NodeCost(temp.g+1,temp.B,temp.mv,true)));
				}
			}
			temp = pq.top();
			if(Do_backward_move(&temp.B, (Direction) i, &temp.mv, false)){
				if(backward.insert(HistoryID(temp.B.id,temp.mv)).second){
					pq.push((NodeCost(temp.g+1,temp.B,temp.mv,true)));
				}
			}
		}
		pq.pop();
	}
	return;
}

void getBoxPos(){
	for(int i=0; i<=Brd.height-1; ++i){
		for(int j=0; j<=Brd.width-1; ++j){
			canAchieveBrd[i][j] = 0;
			for(int k=0;k<BoxCnt;++k)
				canAchieveBrdAll[k][i][j] = false;
			switch(Brd.board[i][j]){
				case '*':
					goalPos.emplace_back(Point(i,j));
					break;
				case '+':
					goalPos.emplace_back(Point(i,j));
				case '.':
					goalPos.emplace_back(Point(i,j));
			}
		}
	}
	return;
}

bool Do_pull(int & x, int & y,  int & dir, int & goalNum){
	int bX = x+Di[dir], bY = y+Dj[dir];
	int nX = bX+Di[dir], nY = bY+Dj[dir];
	
	
	
	if(!canAchieveBrdAll[goalNum][bX][bY]){
		int h = Brd.height, w = Brd.width;
		if(nX<0 || nX>=h)
			return false;
		if(nY<0 || nY>=w)
			return false;
		if(Brd.board[nX][nY]=='#')
			return false;
		if(bX<0 || bX>=h)
			return false;
		if(bX<0 || nY>=w)
			return false;
		if(Brd.board[bX][bY]=='#')
			return false;
		canAchieveBrd[bX][bY] ++;
		canAchieveBrdAll[goalNum][bX][bY] = true;
		for(int i=0;i<=3;++i)
			Do_pull(bX,bY,i,goalNum);
	}
	
	return true;
}

void findSimpleDeadLock(){
	int h = Brd.height, w = Brd.width;
	int x,y,cnt=0;
	int index;
	for (std::vector<Point>::iterator it = goalPos.begin() ; it != goalPos.end(); ++it)
	{
		x = it->xval;
		y = it->yval;
		canAchieveBrd[x][y] ++;
		canAchieveBrdAll[cnt][x][y] = true;
		for(int i=0;i<=3;++i)
			Do_pull(x,y,i,cnt);
		

		goalLocated[cnt] = 0;
		index=0;
		for(int i=0;i<h;++i){
			for(int j=0;j<w;++j){
				if(canAchieveBrdAll[cnt][i][j]){
					// cout<<"X:"<<i<<","<<j<<endl;
					goalLocated[cnt] |= 1ULL << index;
				}
				index++;
			}
			
		}
		cnt++;
	}
	return;
}

void findGoalBoard(Board & gBrd){
	gBrd = Brd;
	gBrd.id = 0;
	int h = Brd.height, w = Brd.width;
	for(int i=0;i<h;++i){
		for(int j=0;j<w;++j){
			switch (gBrd.board[i][j]){
				case '@':
				case '$':
					gBrd.board[i][j] = '-';
					break;
				case '.':
				case '+':
					gBrd.board[i][j] = '*';
					gBrd.id |= (1ULL << ((i * w) + j));
					break;
				case '*':
					gBrd.id |= (1ULL << ((i * w) + j));
					break;
			}

		}
	}
	gBrd.m_cnt = 0;
	return;
}
int main(){
	while(Get_Board(stdin, &Brd)){
		BoxCnt = Brd.boxCnt;
		NotFindGoal = true;

		Board GoalBoard;
		findGoalBoard(GoalBoard);
		std::thread backWardThread(ReverseBFS,std::ref(GoalBoard));
		
		getBoxPos();
		findSimpleDeadLock();
		IdIDA();
		backWardThread.join();
		goalPos.clear();
		History.clear();
		backward.clear();

	}
	return 0;
}
