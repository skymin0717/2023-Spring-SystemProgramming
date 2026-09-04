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
      char f_name [100] = {};
      int i = 0;
      while (1) {
          if (i >= (int)sizeof(f_name) - 1) { printf("FAIL: name too long\n"); return False; } // 경계 검사
          recv_l = recv(clnt_sock, &f_name[i], 1, 0);
          if (recv_l <= 0) { printf("FAIL: name recv=%d\n", recv_l); return False; } // 0 = 상대가 끊음
          if (f_name[i] == '\0') break;
          i++;
      } // 널문자 앞 부분까지가 파일 이름
      memset(buff, 0x00, sizeof(buff));
      if((recv_l = recv(clnt_sock, buff, 4, MSG_WAITALL)) != 4) 
          return False;
      //파일의 전체 크기 받기
      int f_l = buff[3]&0xff;
      f_l = ((f_l<<8) + (buff[2]&0xff));
      f_l = ((f_l<<8) + (buff[1]&0xff));
      f_l = ((f_l<<8) + (buff[0]&0xff));

      if(f_l>0){
      char d [] = "server_files";
      char pathFile[260] = {};
      sprintf(pathFile, "%s/%s", d, f_name);
      printf("OK: name=[%s] size=%d\n", f_name, f_l);
      FILE* fp = fopen(pathFile, "wb");
      if ((fp==NULL)) { printf("FAIL: fopen failed\n"); return False; }
      int remaining = f_l;
      while (remaining > 0) {
          int chunk = remaining < 256 ? remaining : 256;
          int n = recv(clnt_sock, buff, chunk, MSG_WAITALL);
          if (n != chunk) { printf("FAIL: file recv=%d\n", n); fclose(fp); return False; }
          fwrite(buff, 1, chunk, fp);
          remaining -= chunk;
      }
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
