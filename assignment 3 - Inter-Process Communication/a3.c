#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

void free_space(char* buf1, unsigned char
	* connect, unsigned char* ping, unsigned char* pong, unsigned char* create_shm,
	unsigned char* error, unsigned char* success, unsigned char* write_to_shm, unsigned char* map_file, unsigned char* read_from_file_offset, unsigned char* read_from_file_section){ 
	free(buf1);
	free(connect);
	free(ping);
	free(pong);
	free(create_shm);
	free(error);
	free(success);
	free(write_to_shm);
	free(map_file);
	free(read_from_file_offset);
	free(read_from_file_section);
}

int main(int argc, char** argv){
	int fd1;
	int fd2;
	char* buf1;
	unsigned char buf1_length;
	unsigned char* connect;
	unsigned char* ping;
	unsigned char* pong;
	int number=55777;
	int number_shm;
	int shm_ID;
	unsigned char* create_shm;
	unsigned char* error;
	unsigned char* success;
	unsigned char* shared1=NULL;
	unsigned char* write_to_shm;
	unsigned char* map_file;
	char file_name[100];
	off_t size_file_mapping;
	unsigned char* mapping1=NULL;
	unsigned char* read_from_file_offset;
	unsigned char* read_from_file_section;

	buf1=(char*)malloc(50*sizeof(char));
	connect=(unsigned char*)malloc(8*sizeof(char));
	ping=(unsigned char*)malloc(5*sizeof(char));
	pong=(unsigned char*)malloc(5*sizeof(char));
	create_shm=(unsigned char*)malloc(11*sizeof(char));
	error=(unsigned char*)malloc(6*sizeof(char));
	success=(unsigned char*)malloc(8*sizeof(char));
	write_to_shm=(unsigned char*)malloc(13*sizeof(char));
	map_file=(unsigned char*)malloc(9*sizeof(char));
	read_from_file_offset=(unsigned char*)malloc(22*sizeof(char));
	read_from_file_section=(unsigned char*)malloc(23*sizeof(char));

	connect[0]=7;
	connect[1]='C';
	connect[2]='O';
	connect[3]='N';
	connect[4]='N';
	connect[5]='E';
	connect[6]='C';
	connect[7]='T';

	ping[0]=4;
	ping[1]='P';
	ping[2]='I';
	ping[3]='N';
	ping[4]='G';

	pong[0]=4;
	pong[1]='P';
	pong[2]='O';
	pong[3]='N';
	pong[4]='G';

	create_shm[0]=10;
	create_shm[1]='C';
	create_shm[2]='R';
	create_shm[3]='E';
	create_shm[4]='A';
	create_shm[5]='T';
	create_shm[6]='E';
	create_shm[7]='_';
	create_shm[8]='S';
	create_shm[9]='H';
	create_shm[10]='M';

	error[0]=5;
	error[1]='E';
	error[2]='R';
	error[3]='R';
	error[4]='O';
	error[5]='R';

	success[0]=7;
	success[1]='S';
	success[2]='U';
	success[3]='C';
	success[4]='C';
	success[5]='E';
	success[6]='S';
	success[7]='S';

	write_to_shm[0]=12;
	write_to_shm[1]='W';
	write_to_shm[2]='R';
	write_to_shm[3]='I';
	write_to_shm[4]='T';
	write_to_shm[5]='E';
	write_to_shm[6]='_';
	write_to_shm[7]='T';
	write_to_shm[8]='O';
	write_to_shm[9]='_';
	write_to_shm[10]='S';
	write_to_shm[11]='H';
	write_to_shm[12]='M';

	map_file[0]=8;
	map_file[1]='M';
	map_file[2]='A';
	map_file[3]='P';
	map_file[4]='_';
	map_file[5]='F';
	map_file[6]='I';
	map_file[7]='L';
	map_file[8]='E';

	read_from_file_offset[0]=21;
	read_from_file_offset[1]='R';
	read_from_file_offset[2]='E';
	read_from_file_offset[3]='A';
	read_from_file_offset[4]='D';
	read_from_file_offset[5]='_';
	read_from_file_offset[6]='F';
	read_from_file_offset[7]='R';
	read_from_file_offset[8]='O';
	read_from_file_offset[9]='M';
	read_from_file_offset[10]='_';
	read_from_file_offset[11]='F';
	read_from_file_offset[12]='I';
	read_from_file_offset[13]='L';
	read_from_file_offset[14]='E';
	read_from_file_offset[15]='_';
	read_from_file_offset[16]='O';
	read_from_file_offset[17]='F';
	read_from_file_offset[18]='F';
	read_from_file_offset[19]='S';
	read_from_file_offset[20]='E';
	read_from_file_offset[21]='T';

	read_from_file_section[0]=22;
	read_from_file_section[1]='R';
	read_from_file_section[2]='E';
	read_from_file_section[3]='A';
	read_from_file_section[4]='D';
	read_from_file_section[5]='_';
	read_from_file_section[6]='F';
	read_from_file_section[7]='R';
	read_from_file_section[8]='O';
	read_from_file_section[9]='M';
	read_from_file_section[10]='_';
	read_from_file_section[11]='F';
	read_from_file_section[12]='I';
	read_from_file_section[13]='L';
	read_from_file_section[14]='E';
	read_from_file_section[15]='_';
	read_from_file_section[16]='S';
	read_from_file_section[17]='E';
	read_from_file_section[18]='C';
	read_from_file_section[19]='T';
	read_from_file_section[20]='I';
	read_from_file_section[21]='O';
	read_from_file_section[22]='N';

	//creating the response pipe to which the messages will be written
	int success_fifo=mkfifo("RESP_PIPE_55777",0600);

	//if the pipe was no successfully created, an error is printed
	if(success_fifo<0){
		printf("ERROR\ncannot create the response pipe\n");
		free_space(buf1,connect,ping,pong,create_shm,error,success,write_to_shm,map_file,read_from_file_offset,read_from_file_section);
		exit(0);
	}

	//open the pipe from which information will be read
	fd1=open("REQ_PIPE_55777",O_RDONLY);

	//if reading pipe can't be opened, show an error message
	if(fd1<0)
	{
		printf("ERROR\ncannot open the request pipe\n");
		free_space(buf1,connect,ping,pong,create_shm,error,success,write_to_shm,map_file,read_from_file_offset,read_from_file_section);
		exit(1);
	}

	//open the writing pipe
	fd2=open("RESP_PIPE_55777",O_WRONLY);

	if(fd2>0){
	printf("SUCCESS\n");
	}

	//write the string "CONNECT" on the pipe
	write(fd2,connect,8);

	int exit_point=1;
	while(exit_point==1){ 

	//read the messages on the pipe and add the end of string to be able to compare them
	read(fd1,&buf1_length,sizeof(char));
	read(fd1,buf1,buf1_length);
	buf1[(unsigned int)buf1_length]='\0';

	//if the message is EXIT, terminate and close the pipes
	if(strncmp(buf1,"EXIT",4)==0){
		exit_point=0;
	}
	//if the message read is "PING", respond accordingly
	else if(strncmp(buf1,"PING",4)==0){
		write(fd2,ping,5); //send PING

		write(fd2,pong,5); //send PONG

		write(fd2,&number,sizeof(int)); // send 55777
	}
	// if the message is CREATE_SHM, respond accordingly
	else if(strncmp(buf1,"CREATE_SHM",10)==0){

		//read the parameter used to truncate later
		read(fd1,&number_shm,sizeof(int));

		//create shared memory
		shm_ID=shm_open("/pEU0Yu",O_CREAT | O_RDWR,0664);
		if(shm_ID<0){
			//write the according text for failed creation of shared memory
			write(fd2,create_shm,11);
			write(fd2,error,6);
			free_space(buf1,connect,ping,pong,create_shm,error,success,write_to_shm,map_file,read_from_file_offset,read_from_file_section);
			exit(2);
		}

		//truncate according to number extracted
		ftruncate(shm_ID,number_shm);

		//map the shared memory
		shared1=(unsigned char*)mmap(NULL,sizeof(number_shm),PROT_READ | PROT_WRITE, MAP_SHARED, shm_ID,0);

		//i printed the address because i needed to use the variable
		printf("%s\n",shared1);

		//write the according text for successful operation
		write(fd2,create_shm,11);
		write(fd2,success,8);

	}
	//if the message is "WRITE_TO_SHM" respond accordingly
	else if(strncmp(buf1,"WRITE_TO_SHM",12)==0){
		unsigned int offset;
		unsigned int value;
		unsigned char bytes[4];

		//read first the offset and then the value
		read(fd1, &offset, sizeof(unsigned int));
		read(fd1, &value, sizeof(unsigned int));

		//separate the bytes and put them into an array
		bytes[0]=(unsigned char)(value & 0xFF);
		bytes[1]=(unsigned char)((value>>8) & 0xFF);
		bytes[2]=(unsigned char)((value>>16) & 0xFF);
		bytes[3]=(unsigned char)((value>>24) & 0xFF);

		printf("value to be put: %d\n",value);

		//if there is enough space from the offset to the end of the memory area, put the value there
		//0x35D38E is the hexadecimal representation of number_shm
		if((offset+sizeof(value)) < number_shm && bytes[0]< 0x35D38E && bytes[1]<0x35D38E && bytes[2]<0x35D38E && bytes[3]<0x35D38E ){
			printf("hexadec value: %x%x%x%x \n",bytes[3],bytes[2],bytes[1],bytes[0]);
			shared1[offset]=bytes[3];
			shared1[offset+1]=bytes[2];
			shared1[offset+2]=bytes[1];
			shared1[offset+3]=bytes[0];

			write(fd2,write_to_shm,13);
			write(fd2,success,8);
		}
		//else write the error message
		else{
			write(fd2,write_to_shm,13);
			write(fd2,error,6);
		}
	}
	//if the message is "MAP_FILE", respond accordingly
	else if(strncmp(buf1,"MAP_FILE",8)==0){
		unsigned char file_name_size;

		//read the file name given as parameter
		read(fd1,&file_name_size,sizeof(char));
		read(fd1,file_name,file_name_size);

		//to be able to open the file, we need the end of string characters
		file_name[(unsigned int)file_name_size]='\0';

		//open the file for reading
		int fd_file;
		fd_file=open(file_name, O_RDONLY);

		//if opening of file fails, print error message
		if(fd_file<0){
			write(fd2,map_file,9);
			write(fd2,error,6);
			free_space(buf1,connect,ping,pong,create_shm,error,success,write_to_shm,map_file,read_from_file_offset,read_from_file_section);
			exit(3);
		}

		//to know how much of the memory we need, we will find the size of the file using lseek
		size_file_mapping=lseek(fd_file,0,SEEK_END);
		//the cursor should be unchanged, so we put it first after we modified it to find the size
		lseek(fd_file,0,SEEK_SET);

		//mapping of the file
		mapping1=(unsigned char*)mmap(NULL,size_file_mapping,PROT_READ,MAP_SHARED,fd_file,0);

		//if the mapping failed we should print the corresponding message else the success message
		if(mapping1==(void*)-1){
			write(fd2,map_file,9);
			write(fd2,error,6);
			free_space(buf1,connect,ping,pong,create_shm,error,success,write_to_shm,map_file,read_from_file_offset,read_from_file_section);
			exit(4);
		}
		else{
			write(fd2,map_file,9);
			write(fd2,success,8);
		}
	}
	//if message is "READ_FROM_FILE_OFFSET", respond accordingly
	else if(strncmp(buf1,"READ_FROM_FILE_OFFSET",21)==0){
		unsigned int offset;
		unsigned int no_of_bytes;

		//first read the parameters: offset and number of bytes
		read(fd1,&offset,sizeof(unsigned int));
		read(fd1,&no_of_bytes,sizeof(unsigned int));

		if((offset+no_of_bytes)<(unsigned int)size_file_mapping){
			//we need an array of char to store the content that is going to be copied
			char* data;
			data=(char*)malloc((no_of_bytes+1)*sizeof(char));

			//copy each char into the new array, from the specific offset onwards
			for(int i=0;i<no_of_bytes;i++)
				data[i]=mapping1[offset+i];

			//put the end of string characters
			data[no_of_bytes]='\0';

			//printf("data to be transfered: %s\n",data);

			//copy in the shared memory area and write the success message
			strncpy((char*)shared1,data,no_of_bytes);
			write(fd2,read_from_file_offset,22);
			write(fd2,success,8);

			free(data);
		}
		else{
			write(fd2,read_from_file_offset,22);
			write(fd2,error,6);
		}
	}
	//if message is "READ_FROM FILE_SECTION", respond accordingly
	else if(strncmp(buf1,"READ_FROM_FILE_SECTION",22)==0){
		unsigned int section_no;
		unsigned int offset;
		unsigned int no_of_bytes;

		//read the parameters in order
		read(fd1,&section_no,sizeof(unsigned int));
		read(fd1,&offset,sizeof(unsigned int));
		read(fd1,&no_of_bytes,sizeof(unsigned int));

		int total_number_sections;
		total_number_sections=(int)mapping1[4+2+1+1-1];

		if(section_no<=total_number_sections){
			//we need an array of char to store the content that is going to be copied
			char* data;
			data=(char*)malloc((no_of_bytes+1)*sizeof(char));

			//copy each char into the new array, from the specific offset onwards
			for(int i=0;i<no_of_bytes;i++)
				data[i]=mapping1[(section_no-1)*19+offset+i];

			//put the end of string characters
			data[no_of_bytes]='\0';

			printf("data to be transfered: %s\n",data);

			//copy in the shared memory area and write the success message
			strncpy((char*)shared1,data,no_of_bytes);
			write(fd2,read_from_file_section,23);
			write(fd2,success,8);

			free(data);
		}
		else{
			write(fd2,read_from_file_section,23);
			write(fd2,error,6);
		}

	}

	}

	close(fd2);
	close(fd1);
	shm_unlink((char*)shared1);
	munmap(mapping1,size_file_mapping);
	free_space(buf1,connect,ping,pong,create_shm,error,success,write_to_shm,map_file,read_from_file_offset,read_from_file_section);
	return 0;
}