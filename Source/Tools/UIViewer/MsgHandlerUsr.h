#ifndef __MSGHANDLERUSR
#define __MSGHANDLERUSR

class CMsgHandlerUsr : public ui::IMessageHandler
{
public:
	int Handle(const char* pAppMessage, void * arg1 = 0, void * arg2 = 0);
};

#endif