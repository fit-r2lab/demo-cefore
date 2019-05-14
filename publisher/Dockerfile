# Dockerfile for cefore_publisher:0.1

# To create the image, put this Dockefile in an empty folder and run : docker build -t cefore_publisher .

# Use following options to run the corresponding container:
# docker run  -t -i -p80:80  --rm --name="cefore_publisher" cefore_publisher:latest

FROM ubuntu:16.04

USER root
RUN apt-get update && apt-get install -y \
   git \
   wget \
   subversion \
   emacs \
   iperf

WORKDIR /root

RUN git clone https://github.com/fit-r2lab/demo-cefore.git

WORKDIR /root/demo-cefore

RUN true \
  && cp bin/cefnetd /usr/local/sbin \
  && cp bin/cefctrl /usr/local/sbin \
  && cp bin/cefstatus /usr/local/bin \
  && cp bin/cefnetdstart /usr/local/bin \
  && mkdir -p /usr/local/cefore \
  && cp Cefore-Config/* /usr/local/cefore \
  && echo "CS_MODE=1" > /usr/local/cefore/cefnetd.conf \
  && echo "PORT_NUM=80" >> /usr/local/cefore/cefnetd.conf \
  && echo "ccn:/streaming udp 172.16.0.1" > /usr/local/cefore/cefnetd.fib 


EXPOSE 80

CMD ["/bin/bash"]