server:
	g++ util.cpp client.cpp -o build/client && \
	g++ util.cpp server.cpp Epoll.cpp InetAddress.cpp Socket.cpp -o build/server
clean:
	rm server && rm client
