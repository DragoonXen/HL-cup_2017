FROM centos:7
WORKDIR /root
RUN yum -y install unzip
RUN yum -y install make
RUN yum -y install centos-release-scl
RUN yum -y install devtoolset-4-gcc*
#RUN scl enable devtoolset-4 bash &&\
#	yum -y install wget &&\
#	wget https://cmake.org/files/v3.9/cmake-3.9.1.tar.gz &&\
#	tar -xvzf cmake-3.9.1.tar.gz &&\
#	cd cmake-3.9.1 &&\
#	./bootstrap &&\
#	make &&\
#	make install
#RUN yum -y install cmake3
RUN yum clean all
ENV LANG C.UTF-8
EXPOSE 80