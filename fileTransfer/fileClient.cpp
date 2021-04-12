#include "../_cplusframe.h"

struct st_fileinfo fileInfo;

CLogFile logFile;

struct st_args
{
  char filepath[301];
  char filematch[301];
  char logpath[301];
  char serverIP[51];
  int port;
}stargs;

void EXIT(int sig)
{ 
  if(sig > 0) 
  {
    signal(sig, SIG_IGN); signal(SIGINT, SIG_IGN); signal(SIGTERM, SIG_IGN);
  }

  logFile.Write("pid(%d) Exit.\n", getpid());
  logFile.Close();
 
  exit(0);
}

int main(int argc, char* argv[])
{

  if(argc != 2)
  {
    printf("Using:  ./fileClient inifile\n");
    printf("Sample: ./fileClient /home/diryl/code/freecplus/fileTransfer/fileClient.xml \n");
    return -1;
  }

  for(int ii = 0; ii < 100; ++ii) signal(ii, SIG_IGN);
  // �����źţ���shell״̬�¿��á�kill ���̺š� ������ֹ����
  // ����Ҫ�� "kill -9 ����" ǿ����ֹ        

  signal(SIGINT, EXIT); signal(SIGTERM, EXIT);
 
  // ���ز���
  CIniFile InitFile;
  if(InitFile.LoadFile(argv[1]) == false) {printf("InitFile.LoadFile(%s) failed.\n", argv[1]); return -1;}
  memset(&stargs, 0, sizeof(struct st_args));
  InitFile.GetValue("filepath", stargs.filepath, 300);
  InitFile.GetValue("filematch", stargs.filematch, 300);
  InitFile.GetValue("logpath", stargs.logpath, 300);
  InitFile.GetValue("serverIP", stargs.serverIP, 50);
  InitFile.GetValue("port", &stargs.port);

  // ����־�ļ�
  logFile.Open(stargs.logpath);
  logFile.Write("logFile start...\n");
  
  // ����socket
  CTcpClient tcpClient;
  
  // ���ӷ����
  if(tcpClient.ConnectToServer(stargs.serverIP, stargs.port) == false){ logFile.Write("TcpClient.ConnectToServer(%s, %d) failed.\n", stargs.serverIP, stargs.port); return -1;}
  logFile.Write("Client connected to ip:(%s) port:(%d)\n", tcpClient.m_ip, tcpClient.m_port);
 
  CDir Dir;

  while(1)
  {
    // ��ȡĿ¼
    if(Dir.OpenDir(stargs.filepath, stargs.filematch) == false) {logFile.Write("Dir.openDir(%s) failed.\n", stargs.filepath); return -1;}
    logFile.Write("Dir.openDir(%s) success.\n", stargs.filepath);
   
    if(Dir.m_vFileName.size() > 0)
    {
      while(Dir.ReadDir() == true)
      {
        // Dir.m_DirName  Dir.m_FileName  Dir.m_FullFileName Dir.m_FileSize Dir.m_ModifyTime Dir.m_createTime Dir.m_AccessTime Dir.m_DateFMT    
        logFile.Write("Deal with file name: %s\n", Dir.m_FullFileName);
   
        // ��д�ļ��ṹ����Ϣ
        memset(&fileInfo, 0, sizeof(fileInfo));
        strcpy(fileInfo.filename, Dir.m_FullFileName);
        fileInfo.filesize = Dir.m_FileSize;
        strcpy(fileInfo.mtime, Dir.m_ModifyTime);
        logFile.Write("Filename: %s, Filesize: %d, Filemtime:%s\n", fileInfo.filename, fileInfo.filesize, fileInfo.mtime);
      
        // �����ļ�
        logFile.Write("File(%s) sending...\n", Dir.m_FullFileName);
    
        if(SendDirFile(tcpClient.m_sockfd, &fileInfo, &logFile) == false) {logFile.Write("File(%s) send failed.\n", Dir.m_FullFileName);}

        logFile.Write("File(%s) send success!\n", Dir.m_FullFileName);  
      }
    }
    else sleep(30);   // ˯��ʱ����ݲɼ������������
  }
  printf("Files send success.\n");  

  return 0;
}

