all: wrap.o sample_server.o
	gcc wrap.o sample_server.o -o sample_server
	gcc sample_client.c -o sample_client

wrap.o:
	gcc -c wrap.c

sample_server.o:
	gcc -c sample_server.c

clean:
	rm *.o



# all : 
# 	g++ test_net_ser.cpp -o test_net_ser
# 	g++ test_net_cli.cpp -o test_net_cli

# clean :
# 	rm -rf test_net_cli test_net_ser
