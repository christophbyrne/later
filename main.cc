
#include "util.h"
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <thread>
#include <fstream>
#include <streambuf>
char buffer[256];
int client_sockfd;
int port;
int content;

string getlatex( string id)
{
	FILE *pi, *def;
	char buff[128];
	string stt;
	struct stat bufe;
	if(stat("test/test.html", &bufe) == 0)
	{
		cout << "Exists" << endl;
	}
	else
	{
		cout << "Doesn't Exists" << endl;
		if (( pi = popen("latex2html -no_navigation -info 0 -split 0 test.tex", "r")) == NULL)
		{
			perror("popen");
			exit(1);
		}

		while(fgets(buff, sizeof(buff), pi) != NULL){
			cout << buff;
		}

		pclose(pi);
	}
	/*if ((def = fopen("test/index.html", "r")) == NULL)
	{
		perror("fopen");
		exit(1);
	}
	do{
		fgets(buff, sizeof(buff), def);
	} while(!feof(def));

	fclose(def);*/

	if(id.compare("css") == 0)
	{
		ifstream t("test/test.css");
		string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
		return str;
	}
	else
	{ 
		ifstream t("test/test.html");
		string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
		return str;
	}

	msg_q("Latex failure...", 4);

}

void spfunc()
{
	msg_q("Hit",3);
	char dev[3];
	chrono::time_point<chrono::system_clock> now = chrono::system_clock::now();
	time_t tnow = chrono::system_clock::to_time_t(now);
	string stime(ctime(&tnow));
	string htmlst = "HTTP/1.1 200 OK\r\nDate: Mon\r\nServer: Later\r\nAccept-Ranges: bytes\r\n";
	content = read(client_sockfd, buffer, 256);
	strncpy(dev, buffer, 3);
	string devel(dev);
	if(devel.compare("GET") == 0)
	{
		string html = "Content-type:text/html\r\n\r\n";
		string css = "Content-type:text/css\r\n\r\n";
		string latex = getlatex("html");
		string latexcss = getlatex("css");
		string buf(buffer);
		if(buf.find("html") !=string::npos)
		{
			string constru = htmlst + html + latex;
			int size = latex.length() + html.length() + htmlst.length();
			cout << size << endl;
			write(client_sockfd, constru.c_str() , size );
			cout << constru<<endl;
		}
		else if(buf.find("css") !=string::npos)
		{
			cout << "HIT" << endl;
			string constru = htmlst + css + latexcss;
			cout << latexcss << endl;
			int size = latexcss.length() + css.length() + htmlst.length();
			cout << size << endl;
			write(client_sockfd, constru.c_str() , size );
			cout << constru<<endl;
		}
		//cout << strchr( buffer,'\n') << endl;

		close(client_sockfd);
		
	}
	else if(devel.compare("POST") == 0) 
	{

		msg_q("POST", 3);
		char params[40];
		char addbuffer[130];
		sprintf(addbuffer, "%d: %s", client_sockfd, buffer);
		msg_q(addbuffer, 0);
		if(content < 0)
			msg_q("Error reading content.", 4);
		string clr(buffer);
		if(clr.find("www-form-urlencoded") != string::npos)
		{
			if(clr.find("\n\n") != string::npos)
				cout << "Data found at: " << clr.find("\n\n") << endl;
		}
		close(client_sockfd);
	}
	else
	{
		msg_q("Unknown", 4);
	}


	/*
	char addbuffer[130];
	sprintf(addbuffer, "%d: %s", client_sockfd, buffer);
	msg_q(addbuffer, 0);
	if(content < 0)
		msg_q("Error reading content.", 4);

	string htmlst = "HTTP/1.1 200 OK\r\nDate:Today\r\nServer: UNKNOWN\r\nAccept-Ranges: bytes\r\nContent-type:text/html\r\n\r\n";
	string conts = htmlst + latexhtml;
	int size = latexhtml.length() + htmlst.length();
	cout << size << endl;
	write(client_sockfd, conts.c_str() , size );
	*/
}

class sock_init
{
	private:
		int sockfd;
		struct sockaddr_in server_addr, client_addr;
		socklen_t client_zise;
		int pid;
	public:
	sock_init(int port)
	{
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		
		if (sockfd < 0)
		{
			msg_q("Failure to open Socket.",4);
		}

		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = INADDR_ANY;
		server_addr.sin_port = htons(port);

		if(bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
			msg_q("Failure to open Socket.",4);

		msg_q("Socket created successfully. Waiting to listen...", 0);

	}
	void forkr(int port)
	{
		/*if(pid > 1)
		pid = fork();
		if(pid < 0)
			msg_q("No PID found", 4);*/
		
		thread comms(spfunc);
		comms.join();
	}

	void listen()
	{
		if(sockfd < 0)
			msg_q("No client socket detected.",4);

		::listen(sockfd, 5);
		int client_size = sizeof(client_addr);

		while(1){
			client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_zise);
			if(client_sockfd < 0)
				msg_q("Failure to connect to client. Returning...", 3);
			this->forkr(client_sockfd);
		}
		
	}

};

int main(int argc, char** argv)
{	
	msg_q("Starting environment", 0);

	if (argv[1] == NULL)
		port = 8089;
	else
		port = atoi(argv[1]);

	cout << "Port: " << port << endl;
	sock_init si = sock_init((int)port);
	si.listen();
	return 0;	
}
