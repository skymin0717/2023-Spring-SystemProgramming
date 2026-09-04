#include "ftp_test.h"

// NOTE: Please make sure that you fully understand the description before starting the coding. It will save your time!

// Initialize a client socket.
// To ensure the client socket connect with the target server,
// you should pay attention to set the parameters "ip address" and "port" correctly.
// Remind that we assumed "./client -h 127.0.0.1 -p 8080".
int init(int argc, char *argv[]){

  /*####################################################################################*/
  /*###################### DO NOT MODIFY THE CODE BELOW THIS LINE ######################*/
  int sock;
  if(argc != 5 || strcmp(argv[1], "-h") || strcmp(argv[3], "-p")){
    printf("unvalid command line\n");
    printf("./client -h <address> -p <port>\n");
    exit(1);
  }
  /*###################### DO NOT MODIFY THE CODE ABOVE THIS LINE ######################*/
  /*####################################################################################*/

  // client socket initialization
  sock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0x00, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(8080);
  serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  //printf("%s\n", argv[4]);
  /* Your Code */
  
  
  // connection test
  /* Your Code */
  if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))==-1){
    printf("connect error\n");
    return False;
  }
  return sock; // return initialized socket
}

// Implement "ls" function.
// NOTE: This is optional and will not be used in grading. 
// For those who get stuck in error, this function may help you get a current directory or something related to this assignment.
void ls(){
  /* Your Code */
  char s[1024] = {};
  getcwd(s,1024);
  //printf("%s\n", s);
  
  DIR* dr;
  struct dirent *d;
  dr = opendir(".");
  while((d = readdir(dr))!=NULL){
      if((strcmp(d->d_name,".")!=0)&&(strcmp(d->d_name, "..")!=0))
        printf("%s ", d->d_name);
  }
  printf("\n");
  closedir(dr);
  
}

// Implement "changedir" function.
// Change the current directory to target directory.
void changedir(const char* name){ 
  /* Your Code */
  chdir(name);
  //printf("change %s\n", name);
}

// Implement "Upload" function.
// Input : socket & file_name you want to upload to server
// Return : if the socket success to upload the accesible file, return "True". Otherwise, return "False".
int upload(int sock, const char* file_name){
  /* Your Code */
    FILE *fp = fopen(file_name, "rb");
    if ((fp == NULL)) return False;
    int send_l = 0;
    int fileLen = 0;
    char buff[256] = {};
    fseek(fp, 0, SEEK_END);
    fileLen=ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if(fileLen <= 0) return False;
    //printf("%d\n", fileLen);
    
    char cmd[4];
    int a = fileLen;
    cmd[0] = (0xff & a);
    cmd[1] = (0xff & (a>>8));
    cmd[2] = (0xff & (a>>16));
    cmd[3] = (0xff & (a>>24));
    if(write(sock, "u", 1) == -1) return False;
    sleep(1);
    if(write(sock, file_name, strlen(file_name)+1)==-1) return False;
    //sleep(1);
    if(write(sock, cmd, 4) == -1) return False;
    sleep(1);

    int read_num = 1;
    int i = 1;
    if(fp != NULL){
        while(read_num > 0){
            memset(buff, 0x00, sizeof(buff));
            read_num = fread(buff, sizeof(char), 256, fp);
            //printf("%ld ",ftell(fp));
            if((send_l = send(sock, buff, read_num, 0))==-1)
                return False;
            //printf(" %d\n",read_num);
            //fileLen -= read_num;
            i++;
        }
        sleep(1);
        //return True;
    }
    else
        return False;
    
    fclose(fp);
    remove(file_name);
    return True;
}

// Implement "Download" function.
// Input : socket & file_name you want to download from server
// Return : if the socket success to download the accessible file, return "True". Otherwise, return "False".
int download(int sock, const char* file_name){
  /* Your Code */
    int send_l = 0;
    int recv_l = 0;
    char buff[256] = {};

    if(write(sock, "d", 1) == -1) return False;
    sleep(1);
    if(write(sock, file_name, strlen(file_name)+1)==-1) return False;
    //sleep(1);

    memset(buff, 0x00, sizeof(buff));
    if((recv_l = recv(sock, buff, 100, 0)) == -1) 
        return False;
      //파일의 전체 크기 받기
    int f_l = buff[3]&0xff;
    f_l = ((f_l<<8) + (buff[2]&0xff));
    f_l = ((f_l<<8) + (buff[1]&0xff));
    f_l = ((f_l<<8) + (buff[0]&0xff));
    //printf("%d\n", (f_l));

    if(f_l>0){
      char d [] = "/elice/project_file/client_files";
      //FILE* fp = fopen(f_name, "w");
      char pathFile[260] = {};
      sprintf(pathFile, "%s/%s", d, file_name);
      FILE* fp = fopen(pathFile, "wb");
      //printf("%s\n", pathFile);
      int i = 1;
      if ((fp==NULL)) return False;
      int f_ll = f_l;
      for(int r = 0; r < (f_ll/256); r++){
        //printf("%d %d\n",i, recv_l);
        //int f = fputs(buff, fp);
        //printf("%ld ",ftell(fp));
        memset(buff, 0x00, sizeof(buff));
        recv_l = recv(sock, buff, 256, 0);
        for(int k = 0 ; k < sizeof(buff); k++){
            fputc(buff[k], fp);
            //printf("%d %c\n",k, buff[k]);
        }
        //printf("%d\n",recv_l);
        //printf("%ld ",ftell(fp));
        f_l = f_l - 256;
        //printf("fl %d\n", f_l);
        i++;
      }
      //printf("f_fl %d\n", f_l);
      memset(buff, 0x00, sizeof(buff));
      recv_l = recv(sock, buff, f_l, 0);
      if(f_l >= 0){
        for (int j = 0; j<f_l; j++){
          fputc(buff[j], fp);
          //printf("%d %d %c\n", recv_l,j, buff[j]);
        }          
      }
      sleep(1);
      fseek(fp, 0, SEEK_END);
      size_t fileLen=ftell(fp);
      fseek(fp, 0, SEEK_SET);
      //printf("%ld\n", fileLen);
      fclose(fp);
      return True;
      //printf("%d", remove(pathFile));
      //break;          
    }
    else
        return False;
    return False;
}

// Implement "Terminate" function.
// Terminate function should send the terminate signal to server.
void terminate(int sock){
  /* Your Code */
    int send_l;
    char buff[256] = {};
    if(write(sock, "t", 1) != -1){
        send_l = send(sock, buff, 256, 0);
    }
    close(sock);
}