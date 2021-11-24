/*
	ʵ��ͨ��tcp/ipЭ��ʵ�ֵ����������յ����������󣬷���һ��HTML��ҳ�������
*/
#include <stdio.h>

#include <sys/types.h>
#include <string.h>
#include<winsock.h>

//��ʼ����ַ�˿ڼ������������ȴ�����
int init_net()
{
	//�����׽���
	int socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketfd < 0)
	{
		perror("socket failed");
		return -1;
	}

	//======================���õ�ַ����====================================
	int on = 1;
	int rret = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	//=====================================================================


	//=======================���׽���=====================================
	struct sockaddr_in my_addr;
	memset(&my_addr, 0, sizeof(my_addr));

	my_addr.sin_family = AF_INET;//ʹ�õ�Э�飺ipv4
	my_addr.sin_port = htons(9988);//�󶨵Ķ˿ں�
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);//�������ַ

	int ret = bind(socketfd, (struct sockaddr*)&my_addr, sizeof(my_addr));
	if (ret < 0)
	{
		perror("bind failed");
		close(socketfd);
		return -1;
	}
	//=======================================================================


	//=======================�����׽���=======================================
	ret = listen(socketfd, 5);
	if (ret == -1)
	{
		perror("listen failed");
		close(socketfd);
		return -1;
	}
	//========================================================================


	//��ʼ����ϣ������׽���
	return socketfd;
}


//�̺߳���������ÿ���ͻ������Ӻ󴴽�һ���߳̽��е���
void* func(void* arg)
{
	//================�ȷ���ͷ����Ϣ���ٴ򿪱����ļ���ȡ����=======================
	int acceptfd = *(int*)arg;//�Ѵ��ݵĲ���ǿתΪint����

	//
	//	tcp��������Ӧͷ����Ϣ��
	//		HTTP:ʹ��http 1.1Э��
	//		Accept-Ranges����Χ����ĵ�λ�� bytes ���ֽڣ�
	//		Content-Length�����ݳ���
	//		Connection:������
	//		���������\r\n(��ʽҪ��)
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


	//��ҳ���Ĵ���,����ҳ��ӡһ����text��
	/*char text_buf[] = "<!DOCTYPE html>\n\
<html>\n\
<head></head>\n\
<body>test</body>\n\
</html>";*/


///html��ҳ��������ʽ
///		http-equiv:����ָ��ҳ��ʹ�õı��뼯
///	<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\n\
		

	char text_buf[] = "<!DOCTYPE html>\n\
<html>\r\n\
<head>\r\n\
<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\" />\r\n\
<script>\r\n\
function checkForm()\r\n\
{\r\n\
alert(\"�����ύ��\");\r\n\
}\r\n\
</script>\r\n\
</head>\r\n\
<body>\r\n\
<form action=\"open\" ��nsubmit=\"checkForm()\">\r\n\
<label type=\"text\" value=\"����\">\r\n\
<input type=\"submit\" value=\"����\">\r\n\
</form>\r\n\
<form action=\"close\" ��nsubmit=\"checkForm()\">\r\n\
<label type=\"text\" value=\"�ص�\">\r\n\
<input type=\"submit\" value=\"�ص�\">\r\n\
</form>\r\n\
</body>\r\n\
</html>";
	ret = send(acceptfd, text_buf, strlen(text_buf), 0);
	if (ret < 0)
	{
		perror("text write failed");
	}

	//����ر��׽�����ᵼ����������Ͷ��������޷��ٴη�������
	//close(acceptfd);

	//�߳��˳�
	pthread_exit(NULL);
}


//���������
int main(int argc, char** argv)
{

	//��ʼ��
	int socketfd = init_net();

	while (1)
	{
		struct sockaddr_in addr;//����ͻ���ip���˿ڵ���Ϣ�Ľṹ��
		socklen_t len = sizeof(addr);//���ֶ��ٸ��ֽ�

		//�����ȴ��ͻ��˿�ʼ����
		int acceptfd = accept(socketfd, (struct sockaddr*)&addr, &len);
		if (acceptfd == -1)
		{
			perror("acceptfd failed");
			close(acceptfd);
			return -1;
		}

		//��������ʱ����ӡ���ӵĿͻ���ip��
		char* ip = inet_ntoa(addr.sin_addr);
		printf("\n\n\n�пͻ�������:%s\n", ip);


		//�����߳����乤��
		pthread_t pid = -1;
		int ret = pthread_create(&pid, NULL, func, &acceptfd);
		if (ret != 0)
		{
			printf("pthread_create failed\n");
			continue;
		}

		//�̷߳���
		pthread_detach(pid);

	}

	return 0;
