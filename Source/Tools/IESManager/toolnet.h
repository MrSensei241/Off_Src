#pragma once



namespace net
{

	void	Init();
	void	Exit();
	
	BOOL	ReqLogin(const char * id, const char * passWord, const char * ip, unsigned short port);
	void	ChangeIESProp(const char * idSpace, int classID, const char * propName, const char * propValue, const char * log);
	void	DeleteIESHis(int delDB, int delHistory, int rollbackValue, const INT_LIST& revisionList);


	void	Recv();
	BOOL	IsConnected();


};


