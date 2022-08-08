# all: wrap.o sample_server.o
# 	gcc wrap.o sample_server.o -o sample_server
# 	gcc sample_client.c -o sample_client

# wrap.o:
# 	gcc -c wrap.c

# sample_server.o:
# 	gcc -c sample_server.c

# clean:
# 	rm *.o



all : 
	gcc test_sample_server2.c -o test_sample_server2
	gcc test_net_cli.c -o test_net_cli -lpthread

clean :
	rm -rf test_net_cli test_sample_server2
