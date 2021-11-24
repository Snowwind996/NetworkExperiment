/*
	实现通过tcp/ip协议实现当服务器接收到浏览器请求后，发送一段HTML网页给浏览器
*/
#include <stdio.h>

#include <sys/types.h>
#include <string.h>
#include<winsock.h>

//初始化地址端口及开启服务器等待连接
int init_net()
{
	//创建套接字
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0)
	{
		perror("socket failed");
		return -1;
	}

	//======================设置地址复用====================================
	int on = 1;
	int rret = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	//=====================================================================


	//=======================绑定套接字=====================================
	struct sockaddr_in my_addr;
	memset(&my_addr, 0, sizeof(my_addr));

	my_addr.sin_family = AF_INET;//使用的协议：ipv4
	my_addr.sin_port = htons(9988);//绑定的端口号
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);//绑定任意地址

	int ret = bind(socketfd, (struct sockaddr*)&my_addr, sizeof(my_addr));
	if (ret < 0)
	{
		perror("bind failed");
		close(socketfd);
		return -1;
	}
	//=======================================================================


	//=======================监听套接字=======================================
	ret = listen(socketfd, 5);
	if (ret == -1)
	{
		perror("listen failed");
		close(socketfd);
		return -1;
	}
	//========================================================================


	//初始化完毕，返回套接字
	return socketfd;
}


//线程函数，用于每个客户端连接后创建一个线程进行调用
void* func(void* arg)
{
	//================先发送头部信息，再打开本地文件读取发送=======================
	int acceptfd = *(int*)arg;//把传递的参数强转为int类型

	//
	//	tcp服务器响应头部信息：
	//		HTTP:使用http 1.1协议
	//		Accept-Ranges：范围请求的单位是 bytes （字节）
	//		Content-Length：内容长度
	//		Connection:短连接
	//		最后必须加上\r\n(格式要求)
	//
	char head_buf[] = "HTTP/1.1 200 OK\r\nAccept-Ranges: bytes\r\n\
					 Content-Length: 1024\r\n\
					 Content-Type: text/html;charset=UTF-8\r\n\
					 Connection: close\r\n\r\n";

	int ret = send(acceptfd, head_buf, strlen(head_buf), 0);
	if (ret == -1)
	{
		perror("send failed");
		close(acceptfd);
		return NULL;
	}
	//============================================================================


	//网页正文代码,在网页打印一个“text”
	/*char text_buf[] = "<!DOCTYPE html>\n\
<html>\n\
<head></head>\n\
<body>test</body>\n\
</html>";*/


///html网页代码编码格式
///		http-equiv:设置指定页面使用的编码集
///	<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n\
		

	char text_buf[] = "<!DOCTYPE html>\n\
<html>\r\n\
<head>\r\n\
<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\r\n\
<script>\r\n\
function checkForm()\r\n\
{\r\n\
alert(\"表单已提交！\");\r\n\
}\r\n\
</script>\r\n\
</head>\r\n\
<body>\r\n\
<form action=\"open\" οnsubmit=\"checkForm()\">\r\n\
<label type=\"text\" value=\"开灯\">\r\n\
<input type=\"submit\" value=\"开灯\">\r\n\
</form>\r\n\
<form action=\"close\" οnsubmit=\"checkForm()\">\r\n\
<label type=\"text\" value=\"关灯\">\r\n\
<input type=\"submit\" value=\"关灯\">\r\n\
</form>\r\n\
</body>\r\n\
</html>";
	ret = send(acceptfd, text_buf, strlen(text_buf), 0);
	if (ret < 0)
	{
		perror("text write failed");
	}

	//如果关闭套接字则会导致浏览器发送多次命令后无法再次发送数据
	//close(acceptfd);

	//线程退出
	pthread_exit(NULL);
}


//主函数入口
int main(int argc, char** argv)
{

	//初始化
	int socketfd = init_net();

	while (1)
	{
		struct sockaddr_in addr;//保存客户端ip，端口等信息的结构体
		socklen_t len = sizeof(addr);//保持多少个字节

		//阻塞等待客户端开始连接
		int acceptfd = accept(socketfd, (struct sockaddr*)&addr, &len);
		if (acceptfd == -1)
		{
			perror("acceptfd failed");
			close(acceptfd);
			return -1;
		}

		//当有连接时，打印连接的客户端ip号
		char* ip = inet_ntoa(addr.sin_addr);
		printf("\n\n\n有客户端连接:%s\n", ip);


		//创建线程让其工作
		pthread_t pid = -1;
		int ret = pthread_create(&pid, NULL, func, &acceptfd);
		if (ret != 0)
		{
			printf("pthread_create failed\n");
			continue;
		}

		//线程分离
		pthread_detach(pid);

	}

	return 0;
