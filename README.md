# demo-cefore

This git repo hosts the artefacts for the following research paper:

#### title:

"*Efficient Pull-based Mobile Video Streaming leveraging In-Network Functions*"

#### authors:

Kazuhisa Matsuzono, Hitoshi Asaeda, Indukala Naladala and Thierry Turletti

#### venue:

submitted at Globecom'19 conference 



## The scenario
We implemented the proposed scheme using [Cefore](https://cefore.net/), an open source software  that enables CCN-like communications.

To evaluate our proposed mobile video streaming mechanism, we chose a vehicular scenario in which a mobile user (a video consumer in a vehicle) streams a video from the infrastructure through an hybrid wireless/wired network. Then we compare the performance obtained with two other streaming mechanisms: (1) a classical CCN (Cefore-based) streaming application that uses only regular interests (RGI), and (2) a basic TCP streaming application.

Our scenario also uses the [ns-3 network simulator](https://www.nsnam.org/) in emulator mode with [direct code execution (DCE)](https://www.nsnam.org/docs/dce/manual/html/index.html)on one of the R2lab nodes to emulate two mobile content consumers (cars) running real application software (such as Cefore and TCP streaming). A consumer (in France) is connected to a real LTE network and the Internet through a ns-3 WiFi link (between the car and the road side unit RSU) to play a live video service content (published in Japan for the paper).
Note that consumer adaptation mechanisms used to dynamically select the appropriate quality version are not included in the scenario to focus on the mobility and in-network functions. The publisher thus provides a video content with a single bitrate of about 1.0 Mbps.

### How to run the demo

**Prerequisites**: 

You should have an account and a reserved slice on R2lab and have installed a few software on your machine. 

* To sign up and reserve a slice, click [here](https://r2lab.inria.fr/tuto-010-registration.md).   
* To install the few companion software, click [here](https://r2lab.inria.fr/tuto-030-nepi-ng-install.md). 

You need to run the publisher on some host of your choice. We provide a Dockerfile (within the publisher directory) that is ready to use for the demo with Cefore and iperf binaries installed. To build the corresponding docker image, run on an empty directory of the publisher host the following command:

* docker build -t cefore_publisher .

Then to create the publisher container, run:

* docker run  -t -i -p80:80  --rm --name="cefore\_publisher" cefore\_publisher:latest

##### Running the Cefore streaming scenario: 

We assume that your publisher host has the following public IP address: a.b.c.d

At first, run cefnetd on your publisher container:
* cefnetdstart

Run on your machine:

*  ./mosaic-cefore.py -P a.b.c.d -s your_slice -l

Then, wait for a few minutes, and when the script invites you to do so:

Run on the ns-3 R2lab node (fit32 by default):

* /root/NS3/source/ns-3-dce/waf  --run dce-cefore-test

Note that the log file will be at /root/NS3/source/ns-3-dce/files-3/tmp/cefgetstream-thuputLog-1262304001100000

Then, run on the publisher container:

* cefputfile ccn:/streaming/test -f ./publisher/sample.mp4 -r 1 -b 1381



##### Running the Classical CCN (Cefore-based) streaming scenario: 

We assume that your publisher host has the following public IP address: a.b.c.d

At first, run cefnetd on your publisher container:
* cefnetdstart

Run on your machine:

*  ./mosaic-cefore.py -P a.b.c.d -s your_slice -l

Then, wait for a few minutes, and when the script invites you to do so:

Run on the ns-3 R2lab node (fit32 by default):

* /root/NS3/source/ns-3-dce/waf  --run "dce-cefore-test --onlyRGI=1"

Note that the log file will be at /root/NS3/source/ns-3-dce/files-3/tmp/cefgetstream-thuputLog-1262304001100000

Then, run on the publisher container:
* cefputfile ccn:/streaming/test -f ./publisher/sample.mp4 -r 1 -b 1381



##### Running the TCP streaming scenario:
At first, run iperf on your publisher container:
* iperf -s -P 1 -p 80

Run on your machine:

* ./mosaic-cefore.py -t -P a.b.c.d -s your_slice -l
 
Then, wait for a few minutes, and when the script invites you to do so:

Run on the ns-3 R2lab node (fit32 by default):

* /root/NS3/source/ns-3-dce/waf  --run dce-tcp-test

Note that the log file will be at /root/NS3/source/ns-3-dce/files-4/var/log/56884/stdout



#### How to obtain buffer level and QoE logs 
We provide a program to compute the QoE metric on the directory QoE. To compute it, just run:

* g++ cal-buffer-qoe.cc
* ./a.out [thuputLog-file (cefore) or stdout-file (iperf)] [cefore or tcp] [initial buffer level (sec) (default 1s)]
