#include "ftp_test.h"

// NOTE: Please make sure that you fully understand the description before starting the coding. It will save your time!

int main(int argc, char *argv[]){

  /*####################################################################################*/
  /*###################### DO NOT MODIFY THE CODE BELOW THIS LINE ######################*/
  int serv_sock; // server socket
  int clnt_sock; // client socket
  if(argc != 3 || strcmp(argv[1], "-p")){
    printf("invalid command line\n");
    printf("./server -p <port>\n");
    exit(1);
  }
  /*###################### DO NOT MODIFY THE CODE ABOVE THIS LINE ######################*/
  /*####################################################################################*/
  
  // Initialize a server socket.
  // To ensure the server socket connect with client socket, 
  // you should initialize the server socket with the proper settings.
  // Note that you should use "INADDR_ANY" option when you setup the server address.
  struct sockaddr_in serv_addr;
  struct sockaddr_in clnt_addr;
  serv_sock = socket(AF_INET, SOCK_STREAM, 0);
  /* Your Code */
  if(serv_sock == -1){
    printf("socket error\n");
    exit(1);
  }
  
  memset(&serv_addr, 0x00, sizeof(serv_addr));
  // Bind the server socket.
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addr.sin_port = htons(atoi(argv[2]));
  /* Your Code */
  if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1){
    printf("bind error\n");
    exit(1);
  }

  // Set the server socket to listening mode.
  /* Your Code */
  printf("Server start: listen on port %s\n", argv[2]);
  if(listen(serv_sock, 1) == -1){
    printf("listen error\n");
    exit(1);
  }

  // Accept client sockets if they request to accept.
  unsigned int clnt_addrsize = sizeof(clnt_addr);
  clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addrsize);
  /* Your Code */
  if(clnt_sock == -1){
    printf("accept error\n");
    exit(1);
  }
  
  
  // By now, we have initialized the server socket and made the socket accept the client sockets.
  // We will then implement the auxiliary loop to let the server socket dealing with the requests from the clients.
  // Funtions that you should implement are as follows: "Upload", "Download", and "Terminate".
  char buff[256] = {};
  int recv_l = 0;
  int send_l = 0;
  while(True){
      memset(buff, 0x00, sizeof(buff));
      if((recv_l = recv(clnt_sock, buff, 1, 0)) == -1) return False;
  
    // upload
    if(!strcmp(buff, "u")){
      /* Your Code */
      memset(buff, 0x00, sizeof(buff));
      if((recv_l = recv(clnt_sock, buff, 100, 0)) == -1) 
        return False;
      char f_name [100] = {};
      int name_l;
      for(int i = 0; buff[i] != '\0'; i++){
          f_name[i] = buff[i];
          name_l = i;
      }
      //memset(buff, 0x00, sizeof(buff));
      //recv_l = recv(clnt_sock, buff, 4, 0);
      //파일의 전체 크기 받기
      int f_l = buff[name_l+5]&0xff;
      f_l = ((f_l<<8) + (buff[name_l+4]&0xff));
      f_l = ((f_l<<8) + (buff[name_l+3]&0xff));
      f_l = ((f_l<<8) + (buff[name_l+2]&0xff));

      if(f_l>0){
      char d [] = "/elice/project_file/server_files";
      char pathFile[260] = {};
      sprintf(pathFile, "%s/%s", d, f_name);
      FILE* fp = fopen(pathFile, "wb");
      int i = 1;
      if ((fp==NULL)) return False;
      int f_ll = f_l;
      for(int r = 0; r < (f_ll/256); r++){
        memset(buff, 0x00, sizeof(buff));
        recv_l = recv(clnt_sock, buff, 256, 0);
        for(int k = 0 ; k < sizeof(buff); k++){
            fputc(buff[k], fp);
        }
        f_l = f_l - 256;
        i++;
      }
      memset(buff, 0x00, sizeof(buff));
      recv_l = recv(clnt_sock, buff, f_l, 0);
      if(f_l > 0){
        for (int j = 0; j<f_l; j++){
          fputc(buff[j], fp);
        }          
      }

      fseek(fp, 0, SEEK_END);
      size_t fileLen=ftell(fp);
      fseek(fp, 0, SEEK_SET);
      fclose(fp);
      }
    }

    // download
    else if(!strcmp(buff, "d")){
      /* Your Code */
      //파일 이름 저장하기 f_name
      memset(buff, 0x00, sizeof(buff));
      if((recv_l = recv(clnt_sock, buff, 100, 0)) == -1) 
        return False;
      char f_name [100] = {};
      for(int i = 0; buff[i] != '\0'; i++){
          f_name[i] = buff[i];
      }
      char d [] = "/elice/project_file/server_files";
      char pathFile[260] = {};
      sprintf(pathFile, "%s/%s", d, f_name);
      FILE* fp = fopen(pathFile, "rb");
      if ((fp == NULL)) return False;
      int send_l = 0;
      //파일 크기 구하기
      int fileLen = 0;
      fseek(fp, 0, SEEK_END);
      fileLen = ftell(fp);
      fseek(fp, 0, SEEK_SET);
    
      //파일 크기 보내기
      char cmd[4] = {};
      int a = fileLen;
      cmd[0] = (0xff & a);
      cmd[1] = (0xff & (a>>8));
      cmd[2] = (0xff & (a>>16));
      cmd[3] = (0xff & (a>>24));
      if(write(clnt_sock, cmd, 4) == -1) return False;

      int read_num = 1;
      int i = 1;
      if(fp){
            while(read_num > 0){
                memset(buff, 0x00, sizeof(buff));
                read_num = fread(buff, sizeof(char), 256, fp);
                if((send_l = send(clnt_sock, buff, read_num, 0))==-1) return False;
                i++;
            }
    fclose(fp);
    }
    remove(pathFile);
    }

    // Terminate
    else if(!strcmp(buff, "t")){
      /* Your Code */
      close(serv_sock);
      close(clnt_sock);
    }
}
}
