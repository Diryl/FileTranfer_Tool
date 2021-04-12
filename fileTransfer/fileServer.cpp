#include "../_cplusframe.h"

void FathEXIT(int sig);
void ChldEXIT(int sig);

struct st_fileinfo fileInfo;

CLogFile logFile;

CTcpServer tcpServer;  

struct st_args{
  char filepath[301];
  char logpath[301];
  int  port;
}stargs;


void FathEXIT(int sig)
{
  if( sig > 0){
    signal(sig, SIG_IGN); signal(SIGINT, SIG_IGN); signal(SIGTERM, SIG_IGN);
    logFile.Write("catching the signal(%d).\n", sig);
  }
  kill(0, 15);
  logFile.Write("Parent process(%d) exit.\n", getpid());
  logFile.Write("logFile exit.\n");
  // ��д�ƺ���룬���ͷ���Դ���ύ��ع�����
  tcpServer.CloseListen();
  logFile.Close();
 
  exit(0);
}

void ChldEXIT(int sig){
  
  if(sig > 0) {
    signal(sig, SIG_IGN); signal(SIGINT, SIG_IGN); signal(SIGTERM, SIG_IGN);
  }

  // logFile.Write("chld(%d) Exit.\n", getpid());
  // ��д�ƺ����
  tcpServer.CloseClient();
  
  exit(0);
}

int main(int argc, char* argv[]){

  if(argc != 2){
    printf("Using:  ./fileServer logfile_path_name\n");
    printf("Sample: ./fileServer /home/diryl/code/freecplus/fileTransfer/fileServer.xml\n");
  }
  
  // CloseIOAndSignal();
  for(int ii = 0; ii < 100; ++ii) signal(ii, SIG_IGN);
  // �����źţ���shell״̬�¿��á�kill ���̺š� ������ֹ����
  // ����Ҫ�� "kill - ����" ǿ����ֹ

  signal(SIGINT, FathEXIT); signal(SIGTERM, FathEXIT);
  signal(SIGCHLD, SIG_IGN);  

  // ���ز����ļ�
  CIniFile InitFile;
  if(InitFile.LoadFile(argv[1]) == false) {printf("InitFile.LoadFile(%s) failed.\n", argv[1]); return -1;}
  // ��ȡ�����������stargs�ṹ��
  memset(&stargs, 0, sizeof(struct st_args));
  InitFile.GetValue("filepath", stargs.filepath, 300);
  InitFile.GetValue("logpath", stargs.logpath, 300);
  InitFile.GetValue("port", &stargs.port);

  logFile.Open(stargs.logpath);
  logFile.Write("logFile start...\n");

  if(tcpServer.InitServer(stargs.port) == false){
    logFile.Write("tcpServer init failed.\n");
    return -1;
  }
  logFile.Write("tcpServer.InitServer(%d)\n", stargs.port); 

  while(1){
    if(tcpServer.Accept() == false) continue;
    if(fork() > 0) {
      tcpServer.CloseClient(); continue;
    }

    signal(SIGINT, ChldEXIT); signal(SIGTERM, ChldEXIT);
    tcpServer.CloseListen();

    logFile.Write("Client(%s) had connected.\n", tcpServer.GetIP());

    logFile.Write("File recving ...\n");
    
    while(1){
      memset(&fileInfo, 0, sizeof(struct st_fileinfo));

      if( RecvDirFile(tcpServer.m_connfd, &fileInfo, stargs.filepath, &logFile) == false) { logFile.Write("RecvFile() failed.\n"); ChldEXIT(0);}

      logFile.Write("File(%s) recv success!\n", fileInfo.filename);    

    }

    printf("File recv success!.\n");
 
    ChldEXIT(0);
  }

}
