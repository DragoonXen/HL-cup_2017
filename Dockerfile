from hl_cmake:latest
WORKDIR /root
COPY ./handlers/ /root/handlers
COPY ./model/ /root/model
COPY ./*.cpp /root/
COPY ./*.h /root/
COPY ./CMakeListsProd.txt /root/CMakeLists.txt
EXPOSE 80
CMD date && \
	unzip -j /tmp/data/data.zip '*.json' -d /root > /dev/null &&\
	cp /tmp/data/options.txt /root &&\	
	echo 'cmake...' &&\
	cmake . > /dev/null &&\
    echo 'make...' &&\
    make > /dev/null &&\
    echo 'Done.' &&\
    date && \
    ./epoll_try